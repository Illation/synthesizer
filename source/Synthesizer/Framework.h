#pragma once
#include <list>
#include <portaudio.h>

// forward declarations

#define SAMPLE_RATE 44100   // Sample rate

// the number of sample frames that PortAudio will
// request from the callback.Many apps
// may want to use
// paFramesPerBufferUnspecified, which
// tells PortAudio to pick the best,
// possibly changing, buffer size
#define FRAMES_PER_BUFFER 256 

#define TIME_PER_BUFFER (static_cast<double>(FRAMES_PER_BUFFER)/static_cast<double>(SAMPLE_RATE/2))

//---------------------------------
// Synthesizer
//
// Generates sounds
//
class Synthesizer
{
public:
	void GetSample(float& left, float& right);

private:
	float left_phase;
	float right_phase;
};

//---------------------------------
// Framework
//
// Main class for this project
//
class Framework final
{
public:
	~Framework();

	void Run();

private:
	void InitializeUtilities();
	void InitializeAudio();
	void TerminateAudio();

	void LogPortAudioError(PaError err);

	void Loop();
	void Update();

private:
	// Data
	////////

	Synthesizer m_Synthesizer;
	PaStream *m_PaStream;
};
