#include "stdafx.h"
#include "FrameworkWindow.h"

#include <Helper/InputManager.h>

#include <FileSystem/FileUtil.h>
#include <FileSystem/Entry.h>

#include <gtkmm/object.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>


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

	InitializeGTK();
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
	//Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/synthesizer/window.ui");

	// #todo: switch to using resources
	File* uiFile = new File("../config/UI/window.ui", nullptr);
	if (!uiFile->Open(FILE_ACCESS_MODE::Read))
	{
		throw std::runtime_error("couldn't read window file");
	}
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_string(FileUtil::AsText(uiFile->Read()).c_str());
	SafeDelete(uiFile);

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
// Nothing yet
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
// FrameworkWindow::InitializeGTK
//
// Connect input manager signals
//
void FrameworkWindow::InitializeGTK()
{
	set_border_width(10);

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

