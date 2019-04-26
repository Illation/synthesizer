#pragma once
#include <list>

// forward declarations
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef unsigned int ALuint;
typedef int ALCint;

#define FREQ 22050   // Sample rate
#define CAP_SIZE 16384 // How much to capture at a time (affects latency)

class SampleBuffer
{
public:
	void AddSample(double const sample);
	void Reset() { m_CapturedSamples = 0; }

	short const* GetData() const { return m_SampleBuffer; }
	uint32 GetNumCapturedSamples() const { return m_CapturedSamples; }
private:
	short m_SampleBuffer[FREQ * 2]; // A buffer to hold captured audio
	uint32 m_CapturedSamples = 0;  // How many samples are captured
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
	void ListAudioDevices(char const* devices);

	void Loop();
	void Update();

private:
	// Data
	////////

	ALCdevice* m_Device = nullptr;
	ALCcontext *m_Context = nullptr;
	
	std::list<ALuint> m_Buffers;
	ALuint m_Source;

	SampleBuffer m_Buffer = SampleBuffer();

	double m_CurrentPhase = 0;

	ALuint m_BufferHolder[16]; // An array to hold catch the unqueued buffers
};

bool TestALError(std::string error);
