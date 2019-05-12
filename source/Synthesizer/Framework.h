#pragma once
#include <list>
#include <memory>

#include "Synthesizer.h"

// forward declarations
class RtMidiIn;
class RtAudio;
typedef unsigned int RtAudioStreamStatus;

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

	template<typename T>
	static int32 AudioCallback(void *outputBuffer, void* inputBuffer, uint32 nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);

private:
	void InitializeUtilities();

	// Audio
	bool InitializeAudio(); // returns false if something went wrong
	void TerminateAudio();

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

	// MIDI input
	RtMidiIn *m_MidiInput;

	// RT audio
	RtAudio* m_Audio;
};

#include "Framework.inl"