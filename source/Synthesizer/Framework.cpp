#include "stdafx.h"
#include "Framework.h"

#include <AL/al.h>
#include <AL/alc.h>

#include "Helper/Commands.h"

//---------------------------------
// Framework::~Framework
//
// Framework destructor
//
Framework::~Framework()
{
	Time::GetInstance()->DestroyInstance();
	PerformanceInfo::GetInstance()->DestroyInstance();

	Logger::Release();
}

//---------------------------------
// Framework::~Framework
//
// High level of what happens when running the game
//
void Framework::Run()
{
	Logger::Initialize();//Init logger first because all output depends on it from the start
#ifndef SHIPPING
	DebugCopyResourceFiles();
#endif
	InitializeUtilities();
	InitializeAudio();

	Loop();
}

//---------------------------------
// Framework::InitializeUtilities
//
// Initialize all the other cool stuff
//
void Framework::InitializeUtilities()
{
	Time::GetInstance()->Start();
	PerformanceInfo::GetInstance();
}

//---------------------------------
// Framework::InitializeAudio
//
// Initialize OpenAL library
//
void Framework::InitializeAudio()
{
	m_Device = alcOpenDevice(NULL);
	if (!m_Device)
	{
		LOG("Unable to create openAL device", Error);
		return;
	}
	ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	{
		LOG("OpenAL enumeration not supported", Warning);
	}

	ListAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	const ALCchar *defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	m_Device = alcOpenDevice(defaultDeviceName);
	if (!m_Device)
	{
		LOG("Unable to open default openAL device", Error);
		return;
	}
	ALCenum error;

	LOG(std::string("Chosen device: ") + alcGetString(m_Device, ALC_DEVICE_SPECIFIER));
	alGetError();

	m_Context = alcCreateContext(m_Device, NULL);
	if (!alcMakeContextCurrent(m_Context))
	{
		LOG("OpenAL failed to make default context", Error);
		return;
	}
	error = alGetError();

	ALuint buffers[16];
	alGenBuffers(16, &buffers[0]);
	error = alGetError();

	// Queue our buffers onto list
	for (int ii = 0; ii < 16; ++ii) 
	{
		m_Buffers.push_back(buffers[ii]);
	}

	alGenSources(1, &m_Source); // Create a sound source
	error = alGetError();

	if (error != AL_NO_ERROR)
	{
		LOG("OpenAL failed to initialize", Error);
		return;
	}
	LOG("OpenAL loaded\n");
}

//---------------------------------
// Framework::ListAudioDevices
//
// List all soundcards
//
void Framework::ListAudioDevices(char const* devices)
{
	ALCchar const *device = devices, *next = devices + 1;
	size_t len = 0;

	LOG("OpenAL device list:");
	while (device && *device != '\0' && next && *next != '\0')
	{
		LOG(std::string("\t") + device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

//---------------------------------
// Framework::Loop
//
// Keep doing this until the program ends
//
void Framework::Loop()
{
	while (true)
	{
		// Update input events here

		//******
		//UPDATE
		TIME->Update();
		PERFORMANCE->StartFrameTimer();

		Update();

		PERFORMANCE->Update();

		// Swap buffers
	}
}

double GetSampleAmplitude(double const amplitude, double const frequency, double const time, double const phase)
{
	return std::sin(etm::PI2 * frequency * (phase + time)) * amplitude;
}
double GetPhase(double const time, double const frequency)
{
	return std::fmod(time, frequency);
}

//---------------------------------
// Framework::Update
//
// What do we want to do every cycle?
//
void Framework::Update()
{
	ALint availableBuffers = 0; // Buffers to be recovered
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &availableBuffers);

	if (availableBuffers > 0) 
	{
		alSourceUnqueueBuffers(m_Source, availableBuffers, m_BufferHolder);

		for (int i = 0; i < availableBuffers; ++i) 
		{
			// Push the recovered buffers back on the queue
			m_Buffers.push_back(m_BufferHolder[i]);
		}
	}

	// Synthesize wave samples
	static double const s_amplitude = 0.5;
	static double const s_frequency = 44000.0;//hz

	double const time = static_cast<double>(TIME->GetTime());

	double const currentSample = GetSampleAmplitude(s_amplitude, s_frequency, time, 0.0);

	m_Buffer.AddSample(currentSample);

	// Upload to soundcard
	if (m_Buffer.GetNumCapturedSamples() > CAP_SIZE) 
	{
		m_Buffer.Reset();

		// Stuff the captured data in a buffer-object
		if (!m_Buffers.empty()) // We just drop the data if no buffers are available
		{ 
			// Get front buffer
			ALuint currentBuffer = m_Buffers.front(); 
			m_Buffers.pop_front();

			alBufferData(currentBuffer, AL_FORMAT_MONO16, m_Buffer.GetData(), CAP_SIZE * sizeof(short), FREQ);

			// Queue the buffer
			alSourceQueueBuffers(m_Source, 1, &currentBuffer);

			// Restart the source if needed
			// (if we take too long and the queue dries up,
			//  the source stops playing).
			ALint soundState = 0;
			alGetSourcei(m_Source, AL_SOURCE_STATE, &soundState);
			if (soundState != AL_PLAYING) 
			{
				alSourcePlay(m_Source);
			}
		}
	}
}

//---------------------------------
// TestALError
//
// OpenAL error checking and output to log
//
bool TestALError(std::string error)
{
	ALCenum alerr = alGetError();
	if (alerr != AL_NO_ERROR)
	{
		std::string alErrorString;
		switch (alerr)
		{
		case AL_NO_ERROR: alErrorString = "AL_NO_ERROR"; break;
		case AL_INVALID_NAME: alErrorString = "AL_INVALID_NAME"; break;
		case AL_INVALID_ENUM: alErrorString = "AL_INVALID_ENUM"; break;
		case AL_INVALID_VALUE: alErrorString = "AL_INVALID_VALUE"; break;
		case AL_INVALID_OPERATION: alErrorString = "AL_INVALID_OPERATION"; break;
		case AL_OUT_OF_MEMORY: alErrorString = "AL_OUT_OF_MEMORY"; break;
		default:
			alErrorString = "Unknown error code"; break;
		}
		LOG(error + " : " + alErrorString, Error);
		return true;
	}
	return false;
}

//---------------------------------
// SampleBuffer::AddSample
//
// Insert sample into buffer
//
void SampleBuffer::AddSample(double const sample)
{
	static double const s_maxShort = static_cast<double>(std::numeric_limits<short>::max());

	m_SampleBuffer[m_CapturedSamples] = static_cast<short>(sample * s_maxShort);
	m_CapturedSamples++;
}
