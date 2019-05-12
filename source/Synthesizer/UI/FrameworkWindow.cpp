#include "stdafx.h"
#include "FrameworkWindow.h"

#include <Helper/InputManager.h>


//====================
// Framework Window
//====================

//---------------------------------
// FrameworkWindow::FrameworkWindow
//
// Framework window default constructor
//
FrameworkWindow::FrameworkWindow()
	: Gtk::ApplicationWindow()
{
	InitializeGTK();
}

//---------------------------------
// FrameworkWindow::OpenFileView
//
// Nothing yet
//
void FrameworkWindow::OpenFileView(Glib::RefPtr<Gio::File> const& file)
{
	UNUSED(file);
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

