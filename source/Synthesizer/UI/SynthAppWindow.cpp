#include "stdafx.h"
#include "SynthAppWindow.h"

#include <EtCore/Helper/InputManager.h>
#include <SynthApp.h>

#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include <epoxy/gl.h>

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
	RegisterAsTriggerer();

	m_RefBuilder->get_widget("stack", m_Stack);
	if (!m_Stack)
	{
		throw std::runtime_error("No 'stack' object in window.ui");
	}

	m_Settings = Gio::Settings::create("com.leah-lindner.synthesizer");
	m_Settings->bind("transition", m_Stack->property_transition_type());

	m_RefBuilder->get_widget("glArea", m_GLArea);
	if (!m_GLArea)
	{
		LOG("SynthAppWindow::SynthAppWindow > No 'glArea' object in window.ui!", LogLevel::Error);
	}
	m_GLArea->signal_realize().connect(sigc::mem_fun(*this, &SynthAppWindow::OnRealize));
	m_GLArea->signal_unrealize().connect(sigc::mem_fun(*this, &SynthAppWindow::OnUnrealize), false);
	m_GLArea->signal_render().connect(sigc::mem_fun(*this, &SynthAppWindow::OnRender), false);

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
// SynthAppWindow::Redraw
//
// Trigger redrawing the window
//
void SynthAppWindow::Redraw()
{
	m_GLArea->queue_draw();
}

//---------------------------------
// SynthAppWindow::OnRealize
//
// init open gl stuff
//
void SynthAppWindow::OnRealize()
{
	m_GLArea->make_current();
	try
	{
		m_GLArea->throw_if_error();
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("An error occured making the context current during realize:", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);
	}
}

//---------------------------------
// SynthAppWindow::OnUnrealize
//
// Uninit open gl stuff
//
void SynthAppWindow::OnUnrealize()
{
	m_GLArea->make_current();
	try
	{
		m_GLArea->throw_if_error();
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("An error occured making the context current during unrealize", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);
	}
}

//---------------------------------
// SynthAppWindow::OnRender
//
// This function updates everything in a gameloops style and then calls Render, making sure to refresh itself at screen refresh rate
//
bool SynthAppWindow::OnRender(const Glib::RefPtr<Gdk::GLContext>& context)
{
	UNUSED(context);

	TriggerTick(); // if this is the first real time thing we will start the update process here

	bool result = Render();

	m_GLArea->queue_draw(); // request drawing again

	return result;
}

//---------------------------------
// SynthAppWindow::Render
//
// Draws the GL Area
//
bool SynthAppWindow::Render()
{
	m_Timer += TIME->DeltaTime();
	while (m_Timer > 1.f)
	{
		m_Timer -= 1.f;
	}

	try
	{
		m_GLArea->throw_if_error();
		glClearColor(0.05f, 0.075f, 0.2f * m_Timer, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glFlush();
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("An error occurred in the render callback of the GLArea", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);

		return false;
	}

	return true;
}

