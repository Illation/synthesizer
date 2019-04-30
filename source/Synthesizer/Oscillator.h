#pragma once

#include "Pattern.h"

//---------------------------------
// Oscillator
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
