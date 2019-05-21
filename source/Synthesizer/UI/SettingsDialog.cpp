#include "stdafx.h"
#include "SettingsDialog.h"

#include <LowEndAudioManager.h>
#include <MidiManager.h>


//====================
// Settings Dialog
//====================

//---------------------------------
// SettingsDialog::SettingsDialog
//
// Settings Dialog default constructor
//
SettingsDialog::SettingsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
	: Gtk::Dialog(cobject)
	, m_RefBuilder(refBuilder)
	, m_Settings()
{
	m_Settings = Gio::Settings::create("com.leah-lindner.synthesizer");

	// Audio output
	m_RefBuilder->get_widget("api", m_ApiSelector);
	if (!m_ApiSelector)
	{
		LOG("SettingsDialog::SettingsDialog > No 'api' object in prefs.ui!", LogLevel::Error);
	}
	m_ApiSelector->signal_changed().connect(sigc::mem_fun(*this, &SettingsDialog::OnApiComboChanged));

	m_RefBuilder->get_widget("device", m_DeviceSelector);
	if (!m_DeviceSelector)
	{
		LOG("SettingsDialog::SettingsDialog > No 'device' object in prefs.ui!", LogLevel::Error);
	}
	m_DeviceSelector->signal_changed().connect(sigc::mem_fun(*this, &SettingsDialog::OnDeviceComboChanged));

	// Midi
	m_RefBuilder->get_widget("midi", m_MidiSelector);
	if (!m_MidiSelector)
	{
		LOG("SettingsDialog::SettingsDialog > No 'midi' object in prefs.ui!", LogLevel::Error);
	}
	m_MidiSelector->signal_changed().connect(sigc::mem_fun(*this, &SettingsDialog::OnMidiComboChanged));
}

//static
//---------------------------------
// SettingsDialog::create
//
// Create a settings dialog from the generated source in prefs.ui
//
SettingsDialog* SettingsDialog::create(Gtk::Window& parent)
{
	// Load the Builder file and instantiate its widgets.
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/synthesizer/prefs.ui");

	SettingsDialog* dialog = nullptr;
	refBuilder->get_widget_derived("prefs_dialog", dialog);
	if (!dialog)
	{
		throw std::runtime_error("No 'prefs_dialog' object in prefs.ui");
	}

	dialog->set_transient_for(parent);
	dialog->PopulateApiOptions();
	dialog->PopulateDeviceOptions();
	dialog->PopulateMidiOptions();

	return dialog;
}

//---------------------------------
// SettingsDialog::PopulateApiOptions
//
// Fill the API box with options
//
void SettingsDialog::PopulateApiOptions()
{
	// get the api names
	std::vector<LowEndAudioManager::T_ApiIdNamePair> apis;
	LowEndAudioManager::GetInstance()->GetAllPossibleApis(apis);

	// populate the combobox with them
	m_ApiSelector->remove_all();
	for (LowEndAudioManager::T_ApiIdNamePair const& api : apis)
	{
		m_ApiSelector->append(Glib::ustring(api.first), Glib::ustring(api.second));
	}

	// set the currently active api in the combobox
	std::string currentApi(LowEndAudioManager::GetInstance()->GetActiveApiId());
	auto findResultIt = std::find_if(apis.cbegin(), apis.cend(), [currentApi](LowEndAudioManager::T_ApiIdNamePair const& api)
	{
		return api.first == currentApi;
	});

	if (findResultIt != apis.cend())
	{
		m_AutoApiComboChanged = true; // make sure setting the combobox doesn't trigger api selection
		m_ApiSelector->set_active(static_cast<int32>(findResultIt - apis.cbegin()));
	}
}

//---------------------------------
// SettingsDialog::PopulateApiOptions
//
// Fill the device combo box with options
//
void SettingsDialog::PopulateDeviceOptions(bool preventDeviceRecreateion)
{
	// get the device names
	std::vector<LowEndAudioManager::T_DeviceIdNamePair> devices;
	LowEndAudioManager::GetInstance()->GetAllPossibleDevices(devices);

	// populate the combobox with them
	m_DeviceSelector->remove_all();
	for (LowEndAudioManager::T_DeviceIdNamePair const& device : devices)
	{
		m_DeviceSelector->append(Glib::ustring(std::to_string(device.first)), Glib::ustring(device.second));
	}

	// set the currently active device in the combobox
	uint32 currentDevice(LowEndAudioManager::GetInstance()->GetActiveDevice());
	auto findResultIt = std::find_if(devices.cbegin(), devices.cend(), [currentDevice](LowEndAudioManager::T_DeviceIdNamePair const& device)
	{
		return device.first == currentDevice;
	});

	if (findResultIt != devices.cend())
	{
		if (preventDeviceRecreateion)
		{
			m_AutoDeviceComboChanged = true; // make sure setting the combobox doesn't trigger device selection
		}
		m_DeviceSelector->set_active(static_cast<int32>(findResultIt - devices.cbegin()));
	}
}

//---------------------------------
// SettingsDialog::PopulateMidiOptions
//
// Fill the midi combo box with options
//
void SettingsDialog::PopulateMidiOptions()
{
	// get the device names
	std::vector<MidiManager::T_DeviceIdNamePair> devices;
	MidiManager::GetInstance()->GetAllPossibleDevices(devices);

	// populate the combobox with them
	m_MidiSelector->remove_all();
	for (MidiManager::T_DeviceIdNamePair const& device : devices)
	{
		m_MidiSelector->append(Glib::ustring(std::to_string(device.first)), Glib::ustring(device.second));
	}

	// set the currently active device in the combobox
	int32 currentDevice(MidiManager::GetInstance()->GetActiveDevice()); // returns -1 if nothing is selected

	m_AutoDeviceComboChanged = true; // make sure setting the combobox doesn't trigger device selection
	m_MidiSelector->set_active(currentDevice);
}

//---------------------------------
// SettingsDialog::OnApiComboChanged
//
// What happens when the user selects a different API
//
void SettingsDialog::OnApiComboChanged()
{
	// we don't want to recreate the audio device if this was automatically selected
	if (m_AutoApiComboChanged)
	{
		m_AutoApiComboChanged = false;
		return;
	}

	Glib::ustring apiName = m_ApiSelector->get_active_id();
	if (!(apiName.empty()))
	{
		LowEndAudioManager::GetInstance()->SetActiveApi(apiName.raw());
	}

	// make sure the device list is up to date
	PopulateDeviceOptions(false);
}

//---------------------------------
// SettingsDialog::OnDeviceComboChanged
//
// What happens when the user selects a different Device
//
void SettingsDialog::OnDeviceComboChanged()
{
	// we don't want to recreate the audio device if this was automatically selected
	if (m_AutoDeviceComboChanged)
	{
		m_AutoDeviceComboChanged = false;
		return;
	}

	Glib::ustring deviceIdString = m_DeviceSelector->get_active_id();
	if (!(deviceIdString.empty()))
	{
		int32 deviceId = std::stoi(deviceIdString.raw());

		LowEndAudioManager::GetInstance()->SetActiveDevice(static_cast<uint32>(deviceId));
	}
}

//---------------------------------
// SettingsDialog::OnMidiComboChanged
//
// What happens when the user selects a different MIDI deveice
//
void SettingsDialog::OnMidiComboChanged()
{
	// we don't want to recreate the midi device if this was automatically selected
	if (m_AutoMidiComboChanged)
	{
		m_AutoMidiComboChanged = false;
		return;
	}

	Glib::ustring deviceIdString = m_MidiSelector->get_active_id();
	if (!(deviceIdString.empty()))
	{
		int32 deviceId = std::stoi(deviceIdString.raw());

		MidiManager::GetInstance()->SetActiveDevice(deviceId);
	}
}
