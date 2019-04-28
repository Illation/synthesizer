#pragma once
#include <list>
#include <memory>

#include "Synthesizer.h"

// forward declarations
typedef void PaStream;
typedef int PaError;

//---------------------------------
// Framework
//
// Main class for this project
//
class Framework final
{
public:
	Framework();
	~Framework();

	void Run();

private:
	void InitializeUtilities();
	bool InitializeAudio(); // returns false if something went wrong
	void TerminateAudio();

	void LogPortAudioError(PaError err);

	void Loop();
	void Update();

private:
	// Data
	////////

	std::unique_ptr<Synthesizer> m_Synthesizer;

	// Portaudio stream
	PaStream *m_PaStream;	// Will become invalid after TerminateAudio is called
};
