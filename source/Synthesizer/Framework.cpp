#include "stdafx.h"
#include "Framework.h"

#include <Vendor/RtMidi.h>

#pragma warning( push )
#pragma warning( disable : 4244 ) // glib warnings
#include <glibmm/main.h>
#include <giomm/applicationcommandline.h>
#pragma warning( pop )

#include <UI/FrameworkWindow.h>
#include <UI/SettingsDialog.h>

#include <Helper/Commands.h>
#include <Helper/InputManager.h>

#include "Config.h"
#include "MidiManager.h"
#include "LowEndAudioManager.h"
#include <glib/gerror.h>
#include <glibmm/ustring.h>
#include <gio/gresource.h>
#include <giomm/resource.h>
#include <glibmm/vectorutils.h>


//====================
// Framework 
//====================

//---------------------------------
// Framework::Framework
//
// Framework constructor
//
Framework::Framework()
	: Gtk::Application("com.leah-lindner.synthesizer", Gio::APPLICATION_HANDLES_OPEN)
	, m_CmdArguments(CommandlineArguments())
	, m_Synthesizer(std::make_unique<Synthesizer>())
{ 
	Logger::Initialize();//Init logger first because all output depends on it from the start
#ifndef SHIPPING
	DebugCopyResourceFiles();
#endif
	InitializeUtilities();

	if (!InitializeAudio())
	{
		LOG("Framework::Framework > Failed to initialize audio!", Warning);
	}

	// Allow updating every frame in a gameloop style - called as quickly as possible
	Glib::signal_idle().connect(sigc::mem_fun(*this, &Framework::OnTick));

	// Initialize commandline arguments for later
	auto onCommandLine = [this](const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) -> int32
	{
		int argc;
		char **argv = command_line->get_arguments(argc);
		m_CmdArguments = CommandlineArguments(argc, argv);
		return 0;
	};
	signal_command_line().connect(onCommandLine, false);

	// when we press close the last window
	signal_shutdown().connect(
		[this]()
	{
		InputManager::GetInstance()->Quit();
	});
}

//---------------------------------
// Framework::~Framework
//
// Framework destructor
//
Framework::~Framework()
{
	TerminateAudio();
	LowEndAudioManager::GetInstance()->DestroyInstance();

	Time::GetInstance()->DestroyInstance();
	PerformanceInfo::GetInstance()->DestroyInstance();
	InputManager::GetInstance()->DestroyInstance();
	Config::GetInstance()->DestroyInstance();

	Logger::Release();
}

//---------------------------------
// Framework::create
//
// Framework creation factory
//
Glib::RefPtr<Framework> Framework::create()
{
	return Glib::RefPtr<Framework>(new Framework());
}

//---------------------------------
// Framework::CreateFrameworkWindow
//
// Create the main window for the framework
//
FrameworkWindow* Framework::CreateFrameworkWindow()
{
	FrameworkWindow* appwindow = FrameworkWindow::create();

	// Make sure that the application runs for as long this window is still open.
	add_window(*appwindow);

	// Gtk::Application::add_window() connects a signal handler to the window's
	// signal_hide(). That handler removes the window from the application.
	// If it's the last window to be removed, the application stops running.
	// Gtk::Window::set_application() does not connect a signal handler, but is
	// otherwise equivalent to Gtk::Application::add_window().

	// Delete the window when it is hidden.
	appwindow->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &Framework::OnHideWindow), appwindow));

	return appwindow;
}

//---------------------------------
// Framework::OnHideWindow
//
// Hiding a window will make us delete it
//
void Framework::OnHideWindow(Gtk::Window* window)
{
	delete window;
}

//---------------------------------
// Framework::on_startup
//
// When the Gtk::Application gets activated we create a window and present it
//
void Framework::on_startup()
{
	// Call the base class's implementation.
	Gtk::Application::on_startup();

	// Add actions and keyboard accelerators for the application menu.
	add_action("preferences", sigc::mem_fun(*this, &Framework::OnActionPreferences));
	add_action("quit", sigc::mem_fun(*this, &Framework::OnActionQuit));
	set_accel_for_action("app.quit", "<Ctrl>Q");

	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_resource("/com/leah-lindner/synthesizer/app_menu.ui");
	}
	catch (const Glib::Error& ex)
	{
		LOG("Framework::on_startup > " + std::string(ex.what()), LogLevel::Error);
		return;
	}
	catch (std::exception const& ex)
	{
		LOG("Framework::on_startup > " + std::string(ex.what()), LogLevel::Error);
		return;
	}

	auto app_menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(refBuilder->get_object("appmenu"));
	if (app_menu)
	{
		set_app_menu(app_menu);
	}
	else
	{
		LOG("Framework::on_startup > No 'appmenu' object in app_menu.ui", LogLevel::Error);
	}
}

//---------------------------------
// Framework::on_activate
//
// When the Gtk::Application gets activated we create a window and present it
//
void Framework::on_activate()
{
	// The application has been started, so let's show a window.
	try
	{
		FrameworkWindow* appwindow = CreateFrameworkWindow();
		appwindow->present();
	}
	// If create_appwindow() throws an exception (perhaps from Gtk::Builder),
	// no window has been created, no window has been added to the application,
	// and therefore the application will stop running.
	catch (Glib::Error const& ex)
	{
		LOG("Framework::on_activate > " + std::string(ex.what().c_str()), LogLevel::Error);
	}
	catch (std::exception const& ex)
	{
		LOG("Framework::on_activate > " + std::string(ex.what()), LogLevel::Error);
	}
}

//---------------------------------
// Framework::on_open
//
// The application has been asked to open some files,
// so let's open a new view for each one.
//
void Framework::on_open(Gio::Application::type_vec_files const& files, Glib::ustring const& hint)
{
	UNUSED(hint);

	// Make sure we have a window to open the files in
	FrameworkWindow* appwindow = nullptr;
	std::vector<Gtk::Window *> windows = get_windows();
	if (windows.size() > 0)
	{
		appwindow = dynamic_cast<FrameworkWindow*>(windows[0]);
	}

	try
	{
		if (appwindow == nullptr)
		{
			appwindow = CreateFrameworkWindow();
		}

		// open those files
		for (Glib::RefPtr<Gio::File> const& file : files)
		{
			appwindow->OpenFileView(file);
		}

		appwindow->present();
	}
	catch (Glib::Error const& ex)
	{
		LOG("Framework::on_open > " + std::string(ex.what()), LogLevel::Error);
	}
	catch (std::exception const& ex)
	{
		LOG("Framework::on_open > " + std::string(ex.what()), LogLevel::Error);
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
	auto onRtMidiError = [](RtMidiError::Type type, std::string const&errorText, void* userData)
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

	return LowEndAudioManager::GetInstance()->InitializeAudio(m_Synthesizer.get());
}

//---------------------------------
// Framework::TerminateAudio
//
// Destroy audio stuff
//
void Framework::TerminateAudio()
{
	LowEndAudioManager::GetInstance()->TerminateAudio();

	SafeDelete(m_MidiInput);
	MidiManager::GetInstance()->DestroyInstance();
}

//---------------------------------
// Framework::OnTick
//
// What do we want to do every cycle?
//
bool Framework::OnTick()
{
	if (!(InputManager::GetInstance()->IsRunning()))
	{
		return false;
	}

	TIME->Update();
	PERFORMANCE->StartFrameTimer();

	m_Synthesizer->Update();

	// Update keystates
	InputManager::GetInstance()->Update();

	PERFORMANCE->Update();

	return true; // we want to keep the callback alive
}

//---------------------------------
// Framework::OnActionPreferences
//
// On clicking the preferences button in the menu we show the SettingsDialog
//
void Framework::OnActionPreferences()
{
	try
	{
		SettingsDialog* prefs_dialog = SettingsDialog::create(*get_active_window());
		prefs_dialog->present();

		// Delete the dialog when it is hidden.
		prefs_dialog->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &Framework::OnHideWindow), prefs_dialog));
	}
	catch (const Glib::Error& ex)
	{
		LOG("Framework::OnActionPreferences > " + std::string(ex.what()), LogLevel::Error);
	}
	catch (const std::exception& ex)
	{
		LOG("Framework::OnActionPreferences > " + std::string(ex.what()), LogLevel::Error);
	}
}

//---------------------------------
// Framework::OnActionQuit
//
// On clicking the quit button in the menu
//
void Framework::OnActionQuit()
{
	// Gio::Application::quit() will make Gio::Application::run() return,
	// but it's a crude way of ending the program. The window is not removed
	// from the application. Neither the window's nor the application's
	// destructors will be called, because there will be remaining reference
	// counts in both of them. If we want the destructors to be called, we
	// must remove the window from the application. One way of doing this
	// is to hide the window. See comment in CreateFrameworkWindow().
	auto windows = get_windows();
	for (auto window : windows)
	{
		window->hide();
	}
}
