#pragma once

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <gtkmm/stack.h>
#include <giomm/settings.h>

//forward declarations
class Gio::File;

//---------------------------------
// FrameworkWindow
//
// Main window for this project
//
class FrameworkWindow final : public Gtk::ApplicationWindow
{
public:
	FrameworkWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder);
	virtual ~FrameworkWindow() = default;

	static FrameworkWindow* create();

	void OpenFileView(Glib::RefPtr<Gio::File> const& file);

private:

	// UI
	void InitializeGTK();

protected:

	// Data
	/////////

	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;
	Gtk::Stack* m_Stack;
};