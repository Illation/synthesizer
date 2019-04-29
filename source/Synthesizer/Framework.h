#pragma once
#include <list>
#include <memory>

#include "Synthesizer.h"

// forward declarations
typedef void PaStream;
typedef int PaError;

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
class Framework final
{
public:
	Framework(CommandlineArguments const& args = CommandlineArguments());
	~Framework();

	void Run();

private:
	void InitializeUtilities();

	// Audio
	bool InitializeAudio(); // returns false if something went wrong
	void TerminateAudio();

	void LogPortAudioError(PaError err);

	// UI
	void InitializeGTK();

	// Runtime
	void Loop();
	void Update();

private:
	// Data
	////////
	CommandlineArguments m_CmdArguments;

	std::unique_ptr<Synthesizer> m_Synthesizer;

	// Portaudio stream
	PaStream *m_PaStream;	// Will become invalid after TerminateAudio is called
};
