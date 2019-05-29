#include "stdafx.h"
#include "SynthApp.h"

#pragma warning( push )
#pragma warning( disable : 4244 ) // glib warnings
#include <glibmm/main.h>
#include <giomm/applicationcommandline.h>
#pragma warning( pop )

#include <UI/SynthAppWindow.h>
#include <UI/SettingsDialog.h>

#include <EtCore/Content/ContentManager.h>
#include <EtCore/Helper/Commands.h>
#include <EtCore/Helper/InputManager.h>
#include <EtCore/UpdateCycle/TickManager.h>

#include "Config.h"
#include "MidiManager.h"
#include "LowEndAudioManager.h"

#include <glib/gerror.h>
#include <glibmm/ustring.h>
#include <gio/gresource.h>
#include <giomm/resource.h>
#include <glibmm/vectorutils.h>


//====================
// Synthesizer Application 
//====================

//---------------------------------
// SynthApp::SynthApp
//
// Synthesizer application constructor
//
SynthApp::SynthApp()
	: Gtk::Application("com.leah-lindner.synthesizer", Gio::APPLICATION_HANDLES_OPEN)
	, m_CmdArguments(CommandlineArguments())
	, m_Synthesizer(std::make_unique<Synthesizer>())
{ 
	RegisterAsTriggerer();
	
	Logger::Initialize();//Init logger first because all output depends on it from the start
#ifndef SHIPPING
	DebugCopyResourceFiles();
#endif
	InitializeUtilities();

	if (!InitializeAudio())
	{
		LOG("SynthApp::SynthApp > Failed to initialize audio!", Warning);
	}

	// Allow updating every frame in a gameloop style - called as quickly as possible
	Glib::signal_idle().connect(sigc::mem_fun(*this, &SynthApp::OnTick));

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
		OnActionQuit();
	});
}

//---------------------------------
// SynthApp::~SynthApp
//
// Synthesizer application destructor
//
SynthApp::~SynthApp()
{
	TerminateAudio();

	Time::GetInstance()->DestroyInstance();
	PerformanceInfo::GetInstance()->DestroyInstance();
	InputManager::GetInstance()->DestroyInstance();
	Config::GetInstance()->DestroyInstance();

	ContentManager::GetInstance()->DestroyInstance();

	Logger::Release();
	TickManager::GetInstance()->DestroyInstance();
}

//---------------------------------
// SynthApp::create
//
// creation factory
//
Glib::RefPtr<SynthApp> SynthApp::create()
{
	return Glib::RefPtr<SynthApp>(new SynthApp());
}

//---------------------------------
// SynthApp::CreateMainWindow
//
// Create the main window for the synth application
//
SynthAppWindow* SynthApp::CreateMainWindow()
{
	SynthAppWindow* appwindow = SynthAppWindow::create(this);

	// Make sure that the application runs for as long this window is still open.
	add_window(*appwindow);

	// Gtk::Application::add_window() connects a signal handler to the window's
	// signal_hide(). That handler removes the window from the application.
	// If it's the last window to be removed, the application stops running.
	// Gtk::Window::set_application() does not connect a signal handler, but is
	// otherwise equivalent to Gtk::Application::add_window().

	// Delete the window when it is hidden.
	appwindow->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &SynthApp::OnHideWindow), appwindow));

	return appwindow;
}

//---------------------------------
// SynthApp::OnHideWindow
//
// Hiding a window will make us delete it
//
void SynthApp::OnHideWindow(Gtk::Window* window)
{
	delete window;
}

//---------------------------------
// SynthApp::on_startup
//
// When the Gtk::Application gets activated we create a window and present it
//
void SynthApp::on_startup()
{
	// Call the base class's implementation.
	Gtk::Application::on_startup();

	// Add actions and keyboard accelerators for the application menu.
	add_action("preferences", sigc::mem_fun(*this, &SynthApp::OnActionPreferences));
	add_action("quit", sigc::mem_fun(*this, &SynthApp::OnActionQuit));
	set_accel_for_action("app.quit", "<Ctrl>Q");

	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_resource("/com/leah-lindner/synthesizer/ui/app_menu.ui");
	}
	catch (const Glib::Error& ex)
	{
		LOG("SynthApp::on_startup > " + std::string(ex.what()), LogLevel::Error);
		return;
	}
	catch (std::exception const& ex)
	{
		LOG("SynthApp::on_startup > " + std::string(ex.what()), LogLevel::Error);
		return;
	}

	auto appMenu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(refBuilder->get_object("appmenu"));
	if (appMenu)
	{
		set_app_menu(appMenu);
	}
	else
	{
		LOG("SynthApp::on_startup > No 'appmenu' object in app_menu.ui", LogLevel::Error);
	}
}

//---------------------------------
// SynthApp::on_activate
//
// When the Gtk::Application gets activated we create a window and present it
//
void SynthApp::on_activate()
{
	// The application has been started, so let's show a window.
	try
	{
		SynthAppWindow* appwindow = CreateMainWindow();
		appwindow->present();
	}
	// If create_appwindow() throws an exception (perhaps from Gtk::Builder),
	// no window has been created, no window has been added to the application,
	// and therefore the application will stop running.
	catch (Glib::Error const& ex)
	{
		LOG("SynthApp::on_activate > " + std::string(ex.what().c_str()), LogLevel::Error);
	}
	catch (std::exception const& ex)
	{
		LOG("SynthApp::on_activate > " + std::string(ex.what()), LogLevel::Error);
	}
}

//---------------------------------
// SynthApp::InitializeUtilities
//
// Initialize all the other cool stuff
//
void SynthApp::InitializeUtilities()
{
	// Load Config data
	Config::GetInstance()->Initialize();

	// Log what we loaded
	Config::Settings::Output const& output = Config::GetInstance()->GetOutput();
	LOG("Output Settings: ");
	LOG("\tSample Rate       > " + std::to_string(output.SampleRate));
	LOG("\tChannels          > " + std::to_string(output.Channels));
	LOG("\tFrames per buffer > " + std::to_string(output.FramesPerBuffer));
	LOG("Derived Settings: ");
	LOG("\tBit Rate          > " + std::to_string(output.BitRate));
	LOG("\tTime per buffer   > " + std::to_string(output.TimePerBuffer));
	LOG("\tTime per sample   > " + std::to_string(output.TimePerSample));
	LOG("");

	ContentManager::GetInstance()->Init();

	InputManager::GetInstance();

	Time::GetInstance()->Start();
	PerformanceInfo::GetInstance();
}

//---------------------------------
// SynthApp::InitializeAudio
//
// Initialize RtAudio library, create a stream and start listening for MIDI messages
//
bool SynthApp::InitializeAudio()
{
	// Initialize MIDI
	MidiManager::GetInstance()->InitializeMidi();

	// Initialize Synth
	m_Synthesizer->Initialize();

	// Initialize RtAudio
	return LowEndAudioManager::GetInstance()->InitializeAudio(m_Synthesizer.get());
}

//---------------------------------
// SynthApp::TerminateAudio
//
// Destroy audio stuff
//
void SynthApp::TerminateAudio()
{
	LowEndAudioManager::GetInstance()->TerminateAudio();
	LowEndAudioManager::GetInstance()->DestroyInstance();

	MidiManager::GetInstance()->TerminateMidi();
	MidiManager::GetInstance()->DestroyInstance();
}

//---------------------------------
// SynthApp::OnTick
//
// What do we want to do every cycle?
//
bool SynthApp::OnTick()
{
	if (!(InputManager::GetInstance()->IsRunning()))
	{
		return false;
	}

	TriggerTick(); // try triggering a tick in case this is not being handled by realtime triggerers

	return true; // we want to keep the callback alive
}

//---------------------------------
// SynthApp::OnActionPreferences
//
// On clicking the preferences button in the menu we show the SettingsDialog
//
void SynthApp::OnActionPreferences()
{
	try
	{
		SettingsDialog* prefsDialog = SettingsDialog::create(*get_active_window());
		prefsDialog->present();

		// Delete the dialog when it is hidden.
		prefsDialog->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &SynthApp::OnHideWindow), prefsDialog));
	}
	catch (const Glib::Error& ex)
	{
		LOG("SynthApp::OnActionPreferences > " + std::string(ex.what()), LogLevel::Error);
	}
	catch (const std::exception& ex)
	{
		LOG("SynthApp::OnActionPreferences > " + std::string(ex.what()), LogLevel::Error);
	}
}

//---------------------------------
// SynthApp::OnActionQuit
//
// On clicking the quit button in the menu
//
void SynthApp::OnActionQuit()
{
	// Gio::Application::quit() will make Gio::Application::run() return,
	// but it's a crude way of ending the program. The window is not removed
	// from the application. Neither the window's nor the application's
	// destructors will be called, because there will be remaining reference
	// counts in both of them. If we want the destructors to be called, we
	// must remove the window from the application. One way of doing this
	// is to hide the window. See comment in CreateMainWindow().
	auto windows = get_windows();
	for (auto window : windows)
	{
		window->hide();
	}
}
