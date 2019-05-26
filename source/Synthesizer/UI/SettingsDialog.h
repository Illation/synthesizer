#pragma once

#include <gtkmm/dialog.h>
#include <giomm/settings.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>

#include <gtkmm/fontbutton.h>
#include <gtkmm/comboboxtext.h>

// forward declarations
class SynthApp;

//---------------------------------
// SettingsDialog
//
// Dialog that allows us to change the applications settings
//
class SettingsDialog : public Gtk::Dialog
{
public:
	SettingsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

	static SettingsDialog* create(Gtk::Window& parent);

	void PopulateApiOptions();
	void PopulateDeviceOptions(bool preventDeviceRecreateion = true);
	void PopulateSampleRateOptions();

	void PopulateMidiOptions();
protected:

	void OnApiComboChanged();
	void OnDeviceComboChanged();
	void OnSampleRateComboChanged();

	void OnMidiComboChanged();

	// Data
	/////////

	Glib::RefPtr<Gtk::Builder> m_RefBuilder;
	Glib::RefPtr<Gio::Settings> m_Settings;

	Gtk::ComboBoxText* m_ApiSelector = nullptr;
	bool m_AutoApiComboChanged = false;

	Gtk::ComboBoxText* m_DeviceSelector = nullptr;
	bool m_AutoDeviceComboChanged = false;

	Gtk::ComboBoxText* m_MidiSelector = nullptr;
	bool m_AutoMidiComboChanged = false;

	Gtk::ComboBoxText* m_SampleRateSelector = nullptr;
	bool m_AutoSampleRateComboChanged = false;
};