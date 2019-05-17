#pragma once
#include <list>
#include <memory>

#include "Synthesizer.h"

#include <gtkmm/application.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

// forward declarations
class RtMidiIn;

class FrameworkWindow;

//---------------------------------
// CommandlineArguments
//
// Store commandline arguments here so we can look them up later, might do more complex things with this at a later point
//
struct CommandlineArguments
{
	CommandlineArguments() = default;
	CommandlineArguments(int argc, char *argv[]) : argumentCount(argc), argumentValues(argv) {}

	int32 argumentCount = 0;
	char** argumentValues = nullptr;
};

//---------------------------------
// Framework
//
// Main class for this project
//
class Framework final : public Gtk::Application 
{
protected:
	Framework();
	virtual ~Framework();

public:
	static Glib::RefPtr<Framework> create();

private:
	FrameworkWindow* CreateFrameworkWindow();
	void OnHideWindow(Gtk::Window* window);

protected:
	// Override default gtkmm application signal handlers:
	void on_startup() override;
	void on_activate() override;
	void on_open(Gio::Application::type_vec_files const& files, Glib::ustring const& hint) override;

private:
	void InitializeUtilities();

	// Audio
	bool InitializeAudio(); // returns false if something went wrong
	void TerminateAudio();

	// Runtime
private:
	bool OnTick();
	void OnActionPreferences();
	void OnActionQuit();

private:
	// Data
	////////
	CommandlineArguments m_CmdArguments;

	std::unique_ptr<Synthesizer> m_Synthesizer;

	// MIDI input
	RtMidiIn *m_MidiInput;
};
