#pragma once

#include <EtCore/UpdateCycle/RealTimeTickTriggerer.h>

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <gtkmm/stack.h>
#include <giomm/settings.h>
#include <gtkmm/glarea.h>

//forward declarations
class Gio::File;
class SynthApp;

//---------------------------------
// SynthAppWindow
//
// Main window for this project
//
class SynthAppWindow final : public Gtk::ApplicationWindow, public I_RealTimeTickTriggerer
{
public:
	SynthAppWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder);
	virtual ~SynthAppWindow() = default;

	static SynthAppWindow* create(SynthApp *const synthApp);
	void SetSynthApp(SynthApp *const synthApp) { m_SynthApp = synthApp; }

	void Redraw();

protected:

	void OnRealize();
	void OnUnrealize();
	bool OnRender(const Glib::RefPtr<Gdk::GLContext>& context);

private:
	bool Render();

private:

	// Data
	/////////

	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;
	Gtk::Stack* m_Stack;

	Gtk::GLArea* m_GLArea;
	SynthApp* m_SynthApp;

	float m_Timer = 0.f;
};