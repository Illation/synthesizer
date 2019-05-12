#include "stdafx.h"
#include "Framework.h"

#include <Vendor/RtMidi.h>
#include <Vendor/RtAudio.h>

#include <gtk/gtk.h>

#include <Helper/Commands.h>
#include <Helper/InputManager.h>

#include "Config.h"
#include "MidiManager.h"

//---------------------------------
// Framework::Framework
//
// Framework constructor
//
Framework::Framework(CommandlineArguments const& args)
	: m_CmdArguments(args)
	, m_Synthesizer(std::make_unique<Synthesizer>())
{ }

//---------------------------------
// Framework::~Framework
//
// Framework destructor
//
Framework::~Framework()
{
	TerminateAudio();

	Time::GetInstance()->DestroyInstance();
	PerformanceInfo::GetInstance()->DestroyInstance();
	InputManager::GetInstance()->DestroyInstance();
	Config::GetInstance()->DestroyInstance();

	Logger::Release();
}

//---------------------------------
// Framework::~Framework
//
// High level of what happens when running the game
//
void Framework::Run()
{
	Logger::Initialize();//Init logger first because all output depends on it from the start
#ifndef SHIPPING
	DebugCopyResourceFiles();
#endif
	InitializeUtilities();

	if (InitializeAudio())
	{
		InitializeGTK();

		Loop();
	}
}

//---------------------------------
// Framework::InitializeUtilities
//
// Initialize all the other cool stuff
//
void Framework::InitializeUtilities()
{
	// Load Config data
	Config::GetInstance()->Initialize();

	// Log what we loaded
	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();
	LOG("Output Settings: ");
	LOG("\tSample Rate       > " + std::to_string(output.SampleRate));
	LOG("\tChannels          > " + std::to_string(output.Channels));
	LOG("\tFrames per buffer > " + std::to_string(output.FramesPerBuffer));
	LOG("Derived Settings: ");
	LOG("\tBit Rate          > " + std::to_string(output.BitRate));
	LOG("\tTime per buffer   > " + std::to_string(output.TimePerBuffer));
	LOG("\tTime per sample   > " + std::to_string(output.TimePerSample));
	LOG("");

	InputManager::GetInstance();

	Time::GetInstance()->Start();
	PerformanceInfo::GetInstance();
}

//---------------------------------
// Framework::InitializeAudio
//
// Initialize RtAudio library, create a stream and start listening for MIDI messages
//
bool Framework::InitializeAudio()
{
	// Initialize MIDI
	//////////////

	// run rtMidi errors through our logging system
	auto onRtMidiError = [](RtMidiError::Type type, const std::string &errorText, void* userData)
	{
		UNUSED(userData);

		LogLevel logLevel = LogLevel::Error;
		switch (type)
		{
		case RtMidiError::WARNING:
		case RtMidiError::DEBUG_WARNING:
		case RtMidiError::UNSPECIFIED:
		case RtMidiError::NO_DEVICES_FOUND:
			logLevel = LogLevel::Warning;
		}

		LOG(errorText, logLevel);
	};

	// make an input device
	try 
	{
		m_MidiInput = new RtMidiIn();
	}
	catch (RtMidiError &error) 
	{
		onRtMidiError(error.getType(), error.getMessage(), nullptr);
		return false;
	}

	LOG("RtMidi version: " + m_MidiInput->getVersion());

	// Log error messages
	m_MidiInput->setErrorCallback(onRtMidiError);

	// Check inputs.
	uint32 numPorts = m_MidiInput->getPortCount();
	if (numPorts > 0)
	{
		// yay we have a midi device

		// log our available ports
		LOG("There are '" + std::to_string(numPorts) + std::string("' MIDI input sources available."));
		for (uint32 i = 0; i < numPorts; i++) 
		{
			LOG("\t Input Port #" + std::to_string(i) + std::string(": '") + m_MidiInput->getPortName(i) + std::string("'"));
		}

		// open the port we want
		m_MidiInput->openPort(0);

		// midi callback function
		auto onMidiCallback = [](double dt, std::vector<uint8>* message, void* userData)
		{
			UNUSED(userData);

			if (message != nullptr)
			{
				MidiManager::GetInstance()->HandleMidiMessage(dt, *message);
			}
		};

		m_MidiInput->setCallback(onMidiCallback);

		// Don't ignore sysex, timing, or active sensing messages.
		m_MidiInput->ignoreTypes(false, false, false);
	}
	else
	{
		LOG("No MIDI devices found, you can use your keyboard to play notes.");

		SafeDelete(m_MidiInput);
	}

	LOG("");

	// Initialize Synth
	///////////////////////

	m_Synthesizer->Initialize();

	// Initialize RtAudio
	///////////////////////

	m_Audio = new RtAudio(); // we could specify the library here
	if (m_Audio == nullptr)
	{
		LOG("Framework::InitializeAudio > Failed to initialize RT Audio!", Warning);
		return false;
	}

	LOG("RtAudio version: " + m_Audio->getVersion());
	LOG("RtAudio API: " + RtAudio::getApiDisplayName(m_Audio->getCurrentApi()));

	// Determine the number of devices available
	unsigned int deviceCount = m_Audio->getDeviceCount();
	if (deviceCount == 0)
	{
		LOG("Framework::InitializeAudio > No sound device found!", Warning);
		return false;
	}

	LOG("There are '" + std::to_string(deviceCount) + std::string("' audio devices available."));

	// Scan through devices for various capabilities
	for (uint32 deviceIdx = 0; deviceIdx < deviceCount; deviceIdx++)
	{
		RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(deviceIdx);
		if (info.probed == true) 
		{
			LOG("\t Device #" + std::to_string(deviceIdx) + std::string(": '") + info.name + std::string("'"));
		}
	}

	// settings for our audio stream
	uint32 defaultDeviceIdx = m_Audio->getDefaultOutputDevice();
	LOG("Default device index: '" + std::to_string(defaultDeviceIdx) + std::string("'"));

	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();

	RtAudio::StreamParameters parameters;
	parameters.deviceId = defaultDeviceIdx;
	parameters.nChannels = output.Channels;

	RtAudio::StreamOptions options;
	options.flags |= RTAUDIO_MINIMIZE_LATENCY;
	if (false)
	{
		options.flags |= RTAUDIO_NONINTERLEAVED;
	}
	if (false) // exclusive device access
	{
		options.flags |= RTAUDIO_HOG_DEVICE;
	}
	if (false) // some scheduling stuff I don't understand quite yet
	{
		options.flags |= RTAUDIO_SCHEDULE_REALTIME;
		options.priority = 900000000;
	}
	if (m_Audio->getCurrentApi() == RtAudio::LINUX_ALSA)
	{
		options.flags |= RTAUDIO_ALSA_USE_DEFAULT;
	}
	options.streamName = "Synthesizer";
	options.numberOfBuffers = 0; // as low as possible

	uint32 framesPerBuffer = output.FramesPerBuffer; // RtAudios underlying API might change this value

	// run rtAudio errors through our logging system
	auto onRtAudioError = [](RtAudioError::Type type, std::string const& errorText)
	{
		LogLevel logLevel = LogLevel::Error;
		switch (type)
		{
		case RtAudioError::WARNING:
		case RtAudioError::DEBUG_WARNING:
		case RtAudioError::UNSPECIFIED:
		case RtAudioError::NO_DEVICES_FOUND:
			logLevel = LogLevel::Warning;
		}

		LOG(errorText, logLevel);
	};

	// open a stream with all our settings
	try 
	{
		m_Audio->openStream( &parameters, 
			nullptr, 
			RTAUDIO_SINT16,
			output.SampleRate * output.Channels,
			&framesPerBuffer,
			&Framework::AudioCallback<int16>,
			m_Synthesizer.get(), 
			&options,
			onRtAudioError);
	}
	catch (RtAudioError& error) 
	{
		onRtAudioError(error.getType(), error.getMessage());
		return false;
	}

	LOG("Opened a stream with '" + std::to_string(framesPerBuffer) + std::string("' frames per buffer."));
	LOG("\tNumber of buffers in this stream: '" + std::to_string(options.numberOfBuffers) + std::string("'"));
	LOG("\tStream callbackFn priority: '" + std::to_string(options.priority) + std::string("'"));

	m_Audio->startStream();

	LOG("");

	// Succesfully initialized sound
	return true;
}

//---------------------------------
// Framework::TerminateAudio
//
// Destroy audio stuff
//
void Framework::TerminateAudio()
{
	m_Audio->stopStream();

	if (m_Audio->isStreamOpen())
	{
		m_Audio->closeStream();
	}

	SafeDelete(m_Audio);

	SafeDelete(m_MidiInput);
	MidiManager::GetInstance()->DestroyInstance();
}

//---------------------------------
// Framework::InitializeGTK
//
// Initialize GTK library, open a window
//
void Framework::InitializeGTK()
{
	// init library
	gtk_init(&m_CmdArguments.argumentCount, &m_CmdArguments.argumentValues);

	// create window
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	typedef void(*T_QuitLambdaType)(void);
	auto quitCallback = []()
	{
		InputManager::GetInstance()->Quit();
		gtk_main_quit();
	};
	g_signal_connect(window, "destroy", G_CALLBACK((T_QuitLambdaType)quitCallback), nullptr);

	// listen for keyboard input
	// on press
	typedef gboolean(*T_KeyLambdaType)(GtkWidget*, GdkEventKey*, gpointer);

	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
	auto keyPressedCallback = [](GtkWidget* widget, GdkEventKey* evnt, gpointer data) -> gboolean
	{
		UNUSED(data);
		UNUSED(widget);

		InputManager::GetInstance()->OnKeyPressed(evnt->keyval);
		return FALSE;
	};
	g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK((T_KeyLambdaType)keyPressedCallback), nullptr);

	// on release
	gtk_widget_add_events(window, GDK_KEY_RELEASE_MASK);
	auto keyReleasedCallback = [](GtkWidget* widget, GdkEventKey* evnt, gpointer data) -> gboolean
	{
		UNUSED(data);
		UNUSED(widget);

		InputManager::GetInstance()->OnKeyReleased(evnt->keyval);
		return FALSE;
	};
	g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK((T_KeyLambdaType)keyReleasedCallback), nullptr);

	//show all the widgets
	gtk_widget_show_all(window);
}

//---------------------------------
// Framework::Loop
//
// Keep doing this until the program ends
//
void Framework::Loop()
{
	while (InputManager::GetInstance()->IsRunning())
	{
		//******
		//UPDATE
		TIME->Update();
		PERFORMANCE->StartFrameTimer();

		// run gtk main loop
		gtk_main_iteration_do(false);

		Update();

		// Update keystates
		InputManager::GetInstance()->Update();

		PERFORMANCE->Update();
	}
}

//---------------------------------
// Framework::Update
//
// What do we want to do every cycle?
//
void Framework::Update()
{
	m_Synthesizer->Update();
}