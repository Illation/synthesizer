#pragma once

#include <EtCore/UpdateCycle/DefaultTickTriggerer.h>

#include <list>
#include <memory>

#include <Synth/Synthesizer.h>

#include <gtkmm/application.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

// forward declarations
class SynthAppWindow;

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
// SynthApp
//
// Main class for this project
//
class SynthApp final : public Gtk::Application, public I_DefaultTickTriggerer
{
protected:
	SynthApp();
	virtual ~SynthApp();

public:
	static Glib::RefPtr<SynthApp> create();

private:
	SynthAppWindow* CreateMainWindow();
	void OnHideWindow(Gtk::Window* window);

protected:
	// Override default gtkmm application signal handlers:
	void on_startup() override;
	void on_activate() override;

private:
	void InitializeUtilities();

	// Audio
	bool InitializeAudio(); // returns false if something went wrong
	void TerminateAudio();

	// Runtime
public:
	void Update();
private:
	bool OnTick();
	void OnActionPreferences();
	void OnActionQuit();

private:
	// Data
	////////
	CommandlineArguments m_CmdArguments;

	std::unique_ptr<Synthesizer> m_Synthesizer;

};
