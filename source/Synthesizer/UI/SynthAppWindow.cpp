#include "stdafx.h"
#include "SynthAppWindow.h"

#include <SynthApp.h>
#include <Synth/Synthesizer.h>

#include "OscillatorRenderer.h"

#include <EtCore/Helper/InputManager.h>

#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

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
	, m_Stack(nullptr)
{
	m_RefBuilder->get_widget("stack", m_Stack);
	if (!m_Stack)
	{
		throw std::runtime_error("No 'stack' object in window.ui");
	}

	m_Settings = Gio::Settings::create("com.leah-lindner.synthesizer");
	m_Settings->bind("transition", m_Stack->property_transition_type());

	Gtk::GLArea* glArea;
	m_RefBuilder->get_widget("glArea", glArea);
	if (!glArea)
	{
		LOG("SynthAppWindow::SynthAppWindow > No 'glArea' object in window.ui!", LogLevel::Error);
	}
	m_Viewport = std::make_unique<Viewport>(glArea);

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
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/synthesizer/window.ui");

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

	SynthParameters const& params = m_SynthApp->GetSynthesizer()->GetParameters();

	if (params.oscillators.size() > 0)
	{
		m_Viewport->SetRenderer(new OscillatorRenderer(params.oscillators[0]));
	}
	else
	{
		LOG("SynthAppWindow::SetSynthApp > couldn't create a renderer for an oscillator because there are none!", LogLevel::Warning);
	}
}
