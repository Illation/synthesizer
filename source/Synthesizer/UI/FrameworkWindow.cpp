#include "stdafx.h"
#include "FrameworkWindow.h"

#include <Helper/InputManager.h>

#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include <epoxy/gl.h>

//====================
// Framework Window
//====================

//---------------------------------
// FrameworkWindow::FrameworkWindow
//
// Framework window default constructor
//
FrameworkWindow::FrameworkWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder)
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

	m_RefBuilder->get_widget("glArea", m_GLArea);
	if (!m_GLArea)
	{
		LOG("SettingsDialog::FrameworkWindow > No 'glArea' object in window.ui!", LogLevel::Error);
	}
	m_GLArea->signal_realize().connect(sigc::mem_fun(*this, &FrameworkWindow::OnRealize));
	m_GLArea->signal_unrealize().connect(sigc::mem_fun(*this, &FrameworkWindow::OnUnrealize), false);
	m_GLArea->signal_render().connect(sigc::mem_fun(*this, &FrameworkWindow::OnRender), false);

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
// FrameworkWindow::create
//
// Create a Framework window from the generated source in window.ui
//
FrameworkWindow* FrameworkWindow::create()
{
	// Load the Builder file and instantiate its widgets.
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/synthesizer/window.ui");

	// get the toplevel element
	FrameworkWindow* window = nullptr;
	refBuilder->get_widget_derived("app_window", window);
	if (!window)
	{
		throw std::runtime_error("No 'app_window' object in window.ui");
	}

	return window;
}

//---------------------------------
// FrameworkWindow::OpenFileView
//
// From gtkmm example, open a file
//
void FrameworkWindow::OpenFileView(Glib::RefPtr<Gio::File> const& file)
{
	std::string const basename = file->get_basename();

	auto scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
	scrolled->set_hexpand(true);
	scrolled->set_vexpand(true);
	scrolled->show();
	auto view = Gtk::make_managed<Gtk::TextView>();
	view->set_editable(false);
	view->set_cursor_visible(false);
	view->show();
	scrolled->add(*view);
	m_Stack->add(*scrolled, basename, basename);

	auto buffer = view->get_buffer();
	try
	{
		char* contents = nullptr;
		gsize length = 0;

		file->load_contents(contents, length);
		buffer->set_text(contents, contents + length);
		g_free(contents);
	}
	catch (const Glib::Error& ex)
	{
		LOG("ExampleAppWindow::OpenFileView [" + std::string(file->get_parse_name()) + std::string("] > ") + std::string(ex.what()), Warning);
	}

	auto tag = buffer->create_tag();
	m_Settings->bind("font", tag->property_font());
	buffer->apply_tag(tag, buffer->begin(), buffer->end());
}

//---------------------------------
// FrameworkWindow::Redraw
//
// Trigger redrawing the window
//
void FrameworkWindow::Redraw()
{
	m_GLArea->queue_draw();
}

//---------------------------------
// FrameworkWindow::OnRealize
//
// init open gl stuff
//
void FrameworkWindow::OnRealize()
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
// FrameworkWindow::OnUnrealize
//
// Uninit open gl stuff
//
void FrameworkWindow::OnUnrealize()
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
// FrameworkWindow::OnRender
//
// Render the content of the GL area
//
bool FrameworkWindow::OnRender(const Glib::RefPtr<Gdk::GLContext>& context)
{
	UNUSED(context);

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

		return true;
	}
	catch (const Gdk::GLError& gle)
	{
		LOG("An error occurred in the render callback of the GLArea", LogLevel::Warning);
		LOG(std::to_string(gle.domain()) + std::string("-") + std::to_string(gle.code()) + std::string("-") + gle.what().raw(), LogLevel::Warning);
		return false;
	}
}

