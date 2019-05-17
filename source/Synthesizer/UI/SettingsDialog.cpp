#include "stdafx.h"
#include "SettingsDialog.h"

#include <LowEndAudioManager.h>


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
	m_RefBuilder->get_widget("font", m_Font);
	if (!m_Font)
	{
		throw std::runtime_error("No 'font' object in prefs.ui");
	}

	m_RefBuilder->get_widget("transition", m_Transition);
	if (!m_Transition)
	{
		throw std::runtime_error("No 'transition' object in prefs.ui");
	}

	m_RefBuilder->get_widget("api", m_ApiSelector);
	if (!m_ApiSelector)
	{
		throw std::runtime_error("No 'api' object in prefs.ui");
	}
	m_ApiSelector->signal_changed().connect(sigc::mem_fun(*this, &SettingsDialog::OnApiComboChanged));

	m_Settings = Gio::Settings::create("com.leah-lindner.synthesizer");
	m_Settings->bind("font", m_Font->property_font_name());
	m_Settings->bind("transition", m_Transition->property_active_id());
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

	return dialog;
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
