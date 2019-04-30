#pragma once
#include <memory>

#include "Config.h"
#include "Pattern.h"

//---------------------------------
// Synthesizer
//
// Generates an oscillating signal of a certain pattern
//
class Oscillator
{
public:
	Oscillator(float const frequency, I_WavePattern* pattern);

	void SetFrequency(float const val) { m_Frequency = static_cast<double>(val); }

	float GetSample(double const dt);

private:
	float m_Phase = 0.0;
	double m_Frequency;  // we store this as a double to avoid casting at runtime
	std::unique_ptr<I_WavePattern> m_Pattern;
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
	typedef std::pair<float, Oscillator> T_LevelOscillatorPair;

	std::vector<T_LevelOscillatorPair> m_Oscillators; // oscillators and their volumes

	Config::OutputSettings const* m_OutputSettings;

	float m_Frequency = 440.f;
};
