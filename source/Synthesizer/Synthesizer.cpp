#include "stdafx.h"
#include "Synthesizer.h"
#include <Helper/Time.h>
#include <Helper/InputManager.h>
#include "gdk/gdkkeysyms-compat.h"

//============
// Voice
//============

//---------------------------------
// Voice::Voice
//
// Construct a voice from synthesizer parameters
//
Voice::Voice(float const frequency, SynthParameters const& params)
	: m_Frequency(frequency)
	, m_Envelope(AdsrEnvelope(params.levelEnvelope))
{
	// Create the oscillators
	for (SynthParameters::OscillatorParameters const& oscParams : params.oscillators)
	{
		// set the pattern
		I_WavePattern* wavePattern;
		switch (oscParams.patternType)
		{
		case SynthParameters::OscillatorParameters::E_PatternType::Sine:
			wavePattern = new SinePattern();
			break;
		case SynthParameters::OscillatorParameters::E_PatternType::Saw:
			wavePattern = new SawPattern();
			break;
		case SynthParameters::OscillatorParameters::E_PatternType::Square:
			wavePattern = new SquarePattern();
			break;
		case SynthParameters::OscillatorParameters::E_PatternType::Triangle:
			wavePattern = new TrianglePattern();
			break;
		}

		// create the oscillator paired with it's level (amplitude)
		m_Oscillators.emplace_back(oscParams.level, Oscillator(frequency + oscParams.frequencyOffset, wavePattern));
	}
}

//---------------------------------
// Voice::GetSample
//
// Get a sample for this voices oscillators multiplied by its envelope
//
float Voice::GetSample(double const dt)
{
	if (!m_InputState && m_Envelope.IsComplete())
	{
		return 0.f;
	}

	float envelopeSignal = m_Envelope.GetSignal(m_InputState, dt);

	float oscillatorOut = 0.f;
	for (T_LevelOscillatorPair& oscillator : m_Oscillators)
	{
		oscillatorOut += oscillator.first * oscillator.second.GetSample(dt);
	}

	return envelopeSignal * oscillatorOut;
}

//============
// Synthesizer
//============

//---------------------------------
// Synthesizer::Initialize
//
// Setup our synth and tune our oscillators
//
void Synthesizer::Initialize()
{
	// config data
	m_OutputSettings = &(Config::GetInstance()->GetOutput());

	// Synth parameters
	m_SynthParameters = SynthParameters();
	m_SynthParameters.levelEnvelope = AdsrParameters(0.1, 0.05, 0.5, 0.7);

	// Oscillators
	float const amplitude = 0.5f;

	SynthParameters::OscillatorParameters osc;
	osc.level = amplitude;
	osc.patternType = SynthParameters::OscillatorParameters::E_PatternType::Triangle;
	m_SynthParameters.oscillators.emplace_back(osc);

	osc.level = amplitude / 4.0;
	osc.patternType = SynthParameters::OscillatorParameters::E_PatternType::Saw;
	m_SynthParameters.oscillators.emplace_back(osc);

	// Map all the keys on our keyboard we want our synthesizer to play - later this could work through MIDI
	std::vector<uint32> keys;
	keys.emplace_back(GDK_a);
	keys.emplace_back(GDK_w);
	keys.emplace_back(GDK_s);
	keys.emplace_back(GDK_e);
	keys.emplace_back(GDK_d);
	keys.emplace_back(GDK_f);
	keys.emplace_back(GDK_t);
	keys.emplace_back(GDK_g);
	keys.emplace_back(GDK_y);
	keys.emplace_back(GDK_h);
	keys.emplace_back(GDK_u);
	keys.emplace_back(GDK_j);
	keys.emplace_back(GDK_k);

	// create a voice for every key we can press
	float frequency = 261.6256f / 2; // start at a C3
	for (uint32 const key : keys)
	{
		m_Voices.emplace_back(key, Voice(frequency, m_SynthParameters));

		// increase frequency with equal temperament
		frequency *= 1.05946f; 
	}
}

//---------------------------------
// Synthesizer::Update
//
// Update which voices are active
//
void Synthesizer::Update()
{
	for (T_KeyVoicePair& keyVoice : m_Voices)
	{
		if (InputManager::GetInstance()->GetKeyState(keyVoice.first) == E_KeyState::Pressed)
		{
			keyVoice.second.SetInputOn();
		}
		if (InputManager::GetInstance()->GetKeyState(keyVoice.first) == E_KeyState::Released)
		{
			keyVoice.second.SetInputOff();
		}
	}
}

//---------------------------------
// Synthesizer::GetSample
//
// Generate a sample for each channel
//
std::vector<float> Synthesizer::GetSample()
{
	// how much time passed since the last sample - static value as this is called in intervals instead of steadily
	double const dt = m_OutputSettings->TimePerSample;

	// Get a sample for every voice
	float voiceOut = 0.f;
	for (T_KeyVoicePair& keyVoice : m_Voices)
	{
		voiceOut += keyVoice.second.GetSample(dt);
	}

	// clamp within allowerd range
	voiceOut = etm::Clamp(voiceOut, 1.f, -1.f);

	// map the result from our voices equally across all channels for now
	std::vector<float> ret;
	for (uint8 channel = 0; channel < m_OutputSettings->Channels; ++channel)
	{
		ret.emplace_back(voiceOut);
	}
	return ret;
}