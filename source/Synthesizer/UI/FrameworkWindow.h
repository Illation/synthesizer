#pragma once

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <gtkmm/stack.h>
#include <giomm/settings.h>
#include <gtkmm/glarea.h>

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

	void Redraw();

protected:

	void OnRealize();
	void OnUnrealize();
	bool OnRender(const Glib::RefPtr<Gdk::GLContext>& context);

private:

	// Data
	/////////

	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;
	Gtk::Stack* m_Stack;

	Gtk::GLArea* m_GLArea;

	float m_Timer = 0.f;
};