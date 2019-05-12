#pragma once

#include <gtkmm/applicationwindow.h>
#include <glibmm/refptr.h>

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
	FrameworkWindow();
	virtual ~FrameworkWindow() = default;

	void OpenFileView(Glib::RefPtr<Gio::File> const& file);

private:

	// UI
	void InitializeGTK();
};