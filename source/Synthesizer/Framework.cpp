#include "stdafx.h"
#include "Framework.h"

#include <portaudio.h>

#include <Helper/Commands.h>
#include "Config.h"

//---------------------------------
// Framework::Framework
//
// Framework constructor
//
Framework::Framework()
	: m_Synthesizer(std::make_unique<Synthesizer>())
{ }

//---------------------------------
// Framework::~Framework
//
// Framework destructor
//
Framework::~Framework()
{
	Time::GetInstance()->DestroyInstance();
	PerformanceInfo::GetInstance()->DestroyInstance();

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

	Time::GetInstance()->Start();
	PerformanceInfo::GetInstance();
}

//---------------------------------
// Framework::InitializeAudio
//
// Initialize portaudio library, create a stream
//
bool Framework::InitializeAudio()
{
	// Initialize portaudio
	PaError err = Pa_Initialize();
	if (err != paNoError)
	{
		LogPortAudioError(err);
		return false;
	}

	// Get number of soundcards from portaudio, print any errors
	LOG("PortAudio version: " + std::string(Pa_GetVersionText()));
	LOG("");
	int numDevices;
	numDevices = Pa_GetDeviceCount();
	if (numDevices == 0)
	{
		LOG("Framework::InitializeAudio > No soundcard found!", Warning);
		return false;
	}
	if (numDevices < 0)
	{
		LOG("Framework::InitializeAudio > Pa_CountDevices returned 0x" + std::to_string(numDevices), Warning);
		err = numDevices;
		LogPortAudioError(err);
		return false;
	}

	// Enumerate Soundcards and log them
	LOG("Sound devices detected by port audio:");
	for (int32 i = 0; i < numDevices; i++)
	{
		PaDeviceInfo const* deviceInfo = Pa_GetDeviceInfo(i);
		PaHostApiInfo const* hostApi = Pa_GetHostApiInfo(deviceInfo->hostApi);
		LOG("\t" + std::string(deviceInfo->name) + std::string(" - hostApi: ") + std::string(hostApi->name));
	}

	// The callback function called when our stream requests more data
	auto onPortAudioCallback = [](
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData)
	{
		UNUSED(inputBuffer);
		UNUSED(statusFlags);
		UNUSED(timeInfo);

		int16* out = static_cast<int16*>(outputBuffer);
		Synthesizer* synth = static_cast<Synthesizer*>(userData);

		static float const s_maxInt16 = static_cast<float>(std::numeric_limits<int16>::max());
		for (uint32 i = 0; i < framesPerBuffer; i++)
		{
			std::vector<float> channels = synth->GetSample();
			for (float const channel : channels)
			{
				*out++ = static_cast<int16>(channel * s_maxInt16);
			}
		}

		return static_cast<int>(PaStreamCallbackResult::paContinue);
	};

	// Open an audio I/O stream. 2nd param is input channel num, 3rd is output channels, i.e stereo
	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();
	err = Pa_OpenDefaultStream(&m_PaStream, 
		0, // no input channels
		output.Channels, 
		paInt16,
		output.SampleRate,
		output.FramesPerBuffer, 
		onPortAudioCallback, 
		m_Synthesizer.get());
	if (err != paNoError)
	{
		LogPortAudioError(err);
		return false;
	}

	// Start playing right away
	err = Pa_StartStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
		return false;
	}

	m_Synthesizer->Initialize();

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
	PaError err = Pa_StopStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	err = Pa_CloseStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	err = Pa_Terminate();
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}
}

//---------------------------------
// Framework::LogPortAudioError
//
// Print port audio error to log
//
void Framework::LogPortAudioError(PaError err)
{
	LOG("Framework::LogPortAudioError > " + std::string(Pa_GetErrorText(err)), Warning);
}


//---------------------------------
// Framework::Loop
//
// Keep doing this until the program ends
//
void Framework::Loop()
{
	while (true)
	{
		// Update input events here

		//******
		//UPDATE
		TIME->Update();
		PERFORMANCE->StartFrameTimer();

		Update();

		PERFORMANCE->Update();

		// Swap buffers
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
