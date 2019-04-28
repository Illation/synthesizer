#include "stdafx.h"
#include "Framework.h"

#include "Helper/Commands.h"


//---------------------------------
// Synthesizer::GetSample
//
// Generate a sample at a given time
//
void Synthesizer::GetSample(float& left, float& right)
{
	left = left_phase;
	right = right_phase;

	// Generate simple sawtooth phaser that ranges between -1.0 and 1.0
	left_phase += 0.01f;
	
	// When signal reaches top, drop back down
	if (left_phase >= 1.0f)
	{
		left_phase -= 2.0f;
	}

	// higher pitch so we can distinguish left and right
	right_phase += 0.03f;
	if (right_phase >= 1.0f)
	{
		right_phase -= 2.0f;
	}
}

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
// Initialize portaudio library
//
void Framework::InitializeAudio()
{
	PaError err = Pa_Initialize();
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	LOG("PortAudio version: " + std::string(Pa_GetVersionText()));
	int numDevices;
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0)
	{
		printf("ERROR: Pa_CountDevices returned 0x%x\n", numDevices);
		LOG("ERROR: Pa_CountDevices returned 0x" + std::to_string(numDevices));
		err = numDevices;
		LogPortAudioError(err);
	}
	LOG("Sound devices detected by port audio:");
	for (int32 i = 0; i < numDevices; i++)
	{
		PaDeviceInfo const* deviceInfo = Pa_GetDeviceInfo(i);
		PaHostApiInfo const* hostApi = Pa_GetHostApiInfo(deviceInfo->hostApi);
		LOG("\t" + std::string(deviceInfo->name) + std::string(" - hostApi: ") + std::string(hostApi->name));
	}

	// Open an audio I/O stream. 2nd param is input channel num, 3rd is output channels, i.e stereo
	err = Pa_OpenDefaultStream(&m_PaStream, 0, 2, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, 
		[](const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			UNUSED(inputBuffer);

			Synthesizer* synth = static_cast<Synthesizer*>(userData);
			float *out = (float*)outputBuffer;

			for (uint32 i = 0; i < framesPerBuffer; i++)
			{
				float left, right;
				synth->GetSample(left, right);
				*out++ = left;
				*out++ = right;
			}

			return 0;
		}
		, &m_Synthesizer);

	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	err = Pa_StartStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}
}

//---------------------------------
// Framework::TerminateAudio
//
// Destroy audio stuff
//
void Framework::TerminateAudio()
{
	PaError err = Pa_StopStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	err = Pa_CloseStream(m_PaStream);
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}

	err = Pa_Terminate();
	if (err != paNoError)
	{
		LogPortAudioError(err);
	}
}

//---------------------------------
// Framework::LogPortAudioError
//
// Print port audio error to log
//
void Framework::LogPortAudioError(PaError err)
{
	LOG("PortAudio error: " + std::string(Pa_GetErrorText(err)));
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

//---------------------------------
// GetSampleAmplitude
//
// What do we want to do every cycle?
//
double GetSampleAmplitude(double const amplitude, double const frequency, double const time, double const phase)
{
	return std::sin(phase + (etm::PI2 * frequency * time)) * amplitude;
}

//---------------------------------
// GetPhase
//
// Time within one cycle
//
double GetPhase(double const time, double const frequency)
{
	double tf = time * frequency;
	return tf - std::floor(tf);
}

double GetFrequency(float const time)
{
	UNUSED(time);
	return 440.0;
	//return static_cast<double>(20150.f-sqrtf((sinf(time) * 10050.f) + 10100.f)*(20100.f/sqrtf(20100.f))); //oscillates between 50 and 20150 hz
}

//---------------------------------
// Framework::Update
//
// What do we want to do every cycle?
//
void Framework::Update()
{

}
