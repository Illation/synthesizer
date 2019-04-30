#pragma once
#include <memory>

#include "Config.h"
#include "AdsrEnvelope.h"
#include "Oscillator.h"

//---------------------------------
// SynthParameters
//
// Parameters that are the same across every voice in the synthesizer
//
struct SynthParameters
{
	//---------------------------------
	// SynthParameters::OscillatorParameters
	//
	// Parameters to set up an oscillator
	//
	struct OscillatorParameters
	{
		//---------------------------------
		// SynthParameters::OscillatorParameters::E_PatternType
		//
		// All possible wave patterns
		//
		enum class E_PatternType
		{
			Sine,
			Saw,
			Square,
			Triangle
		};

		float level = 1.f;
		float frequencyOffset = 0.f;
		E_PatternType patternType = E_PatternType::Sine;
	};

	AdsrParameters levelEnvelope;
	std::vector<OscillatorParameters> oscillators;
};

//---------------------------------
// Voice
//
// Each voice can represent one key on a keyboard in order to create a polyphonic synthesizer
//
class Voice
{
public:
	Voice(float const frequency, SynthParameters const& params);

	float GetSample(double const dt);

	void SetInputState(bool const input) { m_InputState = input; }

private:

	typedef std::pair<float, Oscillator> T_LevelOscillatorPair;

	// Data
	////////
	std::vector<T_LevelOscillatorPair> m_Oscillators; // oscillators and their volumes

	AdsrEnvelope m_Envelope;

	float m_Frequency = 440.f;

	bool m_InputState = false;
};

//---------------------------------
// Synthesizer
//
// Generates sounds
//
class Synthesizer
{
public:
	void Initialize();
	void Update();
	
	std::vector<float> GetSample();

private:
	typedef std::pair<uint32, Voice> T_KeyVoicePair;

	// Data
	////////
	SynthParameters m_SynthParameters;

	std::vector<T_KeyVoicePair> m_Voices;

	Config::OutputSettings const* m_OutputSettings;
};
