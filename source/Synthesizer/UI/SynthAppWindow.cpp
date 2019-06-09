#include "stdafx.h"
#include "SynthAppWindow.h"

#include <SynthApp.h>
#include <Synth/Synthesizer.h>

#include "OscillatorRenderer.h"

#include <EtCore/Helper/InputManager.h>

#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/settings.h>

//====================
// Synth Application Window
//====================

//---------------------------------
// SynthAppWindow::SynthAppWindow
//
// Synth application window default constructor
//
SynthAppWindow::SynthAppWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder)
	: Gtk::ApplicationWindow(cobject)
	, m_RefBuilder(refBuilder)
	, m_Settings()
{
	m_Settings = Gio::Settings::create("com.leah-lindner.synthesizer");

	// listen for keyboard input
	// on press
	auto keyPressedCallback = [](GdkEventKey* evnt) -> bool
	{
		InputManager::GetInstance()->OnKeyPressed(evnt->keyval);
		return false;
	};
	signal_key_press_event().connect(keyPressedCallback, false);

	// on release
	auto keyReleasedCallback = [](GdkEventKey* evnt) -> bool
	{
		InputManager::GetInstance()->OnKeyReleased(evnt->keyval);
		return false;
	};
	signal_key_release_event().connect(keyReleasedCallback, false);

	//show all the widgets
	show_all_children();

	// Display the application menu in the application, not in the desktop environment.
	Glib::RefPtr<Gtk::Settings> gtk_settings = Gtk::Settings::get_default();
	if (gtk_settings)
	{
		gtk_settings->property_gtk_shell_shows_app_menu() = false;
	}
	set_show_menubar(true);

	// Set the window icon.
	set_icon(Gdk::Pixbuf::create_from_resource("/com/leah-lindner/synthesizer/ui/icons/menu.png"));
}

//static
//---------------------------------
// SynthAppWindow::create
//
// Create a synth app window from the generated source in window.ui
//
SynthAppWindow* SynthAppWindow::create(SynthApp *const synthApp)
{
	// Load the Builder file and instantiate its widgets.
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/synthesizer/ui/window.ui");

	// get the toplevel element
	SynthAppWindow* window = nullptr;
	refBuilder->get_widget_derived("app_window", window);
	if (!window)
	{
		throw std::runtime_error("No 'app_window' object in window.ui");
	}

	window->SetSynthApp(synthApp);

	return window;
}

//---------------------------------
// SynthAppWindow::SetSynthApp
//
// Set the synth app so we can display components of the synthesizer
//
void SynthAppWindow::SetSynthApp(SynthApp *const synthApp)
{
	m_SynthApp = synthApp;

	// Create a visualizer for every oscillator
	SynthParameters const& params = m_SynthApp->GetSynthesizer()->GetParameters();

	for (size_t oscIdx = 0; oscIdx < params.oscillators.size(); ++oscIdx)
	{
		// Create a viewport in the stack
		std::string oscName = "Oscillator " + std::to_string(oscIdx + 1);
		std::unique_ptr<Viewport> viewport = std::move(CreateViewport(oscName));

		// Set the viewports renderer to the oscillator
		viewport->SetRenderer(new OscillatorRenderer(params.oscillators[oscIdx]));

		// manage the pointer to the viewport
		m_OscillatorViewports.emplace_back(std::move(viewport));
	}
}

//---------------------------------
// SynthAppWindow::CreateViewport
//
// Create a viewport with an openGL area in it
//
std::unique_ptr<Viewport> SynthAppWindow::CreateViewport(std::string const& name)
{
	// get the stack
	Gtk::Stack* oscStack;
	m_RefBuilder->get_widget("stack", oscStack);
	if (!oscStack)
	{
		LOG("SynthAppWindow::CreateViewport > No 'stack' object in window.ui!", LogLevel::Error);
	}

	// add a GL area to the stack
	Gtk::GLArea* glArea = Gtk::make_managed<Gtk::GLArea>();
	glArea->set_auto_render(true);
	glArea->show();
	oscStack->add(*glArea, name, name);

	// create a viewport from the area
	return std::make_unique<Viewport>(glArea);
}
