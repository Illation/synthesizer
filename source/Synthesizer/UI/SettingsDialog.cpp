#include "stdafx.h"
#include "SettingsDialog.h"


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
	, m_Font(nullptr)
	, m_Transition(nullptr)
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

	return dialog;
}