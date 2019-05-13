#pragma once

#include <gtkmm/dialog.h>
#include <giomm/settings.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>

#include <gtkmm/fontbutton.h>
#include <gtkmm/comboboxtext.h>

// forward declarations
//class Gtk::FontButton;
//class Gtk::ComboBoxText;

//---------------------------------
// SettingsDialog
//
// Dialog that allows us to change the frameworks settings
//
class SettingsDialog : public Gtk::Dialog
{
public:
	SettingsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

	static SettingsDialog* create(Gtk::Window& parent);

protected:
	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;
	Gtk::FontButton* m_Font;
	Gtk::ComboBoxText* m_Transition;
};