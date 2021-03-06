#pragma once

#include <EtRendering/Viewport.h>

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>
#include <gtkmm/stack.h>
#include <giomm/settings.h>

//forward declarations
class Gio::File;
class SynthApp;

//---------------------------------
// SynthAppWindow
//
// Main window for this project
//
class SynthAppWindow final : public Gtk::ApplicationWindow
{
public:
	SynthAppWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder);
	virtual ~SynthAppWindow() = default;

	static SynthAppWindow* create(SynthApp *const synthApp);
	void SetSynthApp(SynthApp *const synthApp);

private:

	std::unique_ptr<Viewport> CreateViewport(std::string const& name);

	// Data
	/////////

	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;
	SynthApp* m_SynthApp;

	std::vector<std::unique_ptr<Viewport>> m_OscillatorViewports;
};