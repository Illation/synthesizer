#pragma once

#include "Pattern.h"

//---------------------------------
// E_PatternType
//
// All possible wave patterns
//
enum class E_PatternType : uint8
{
	Sine,
	Saw,
	Square,
	Triangle,
	COUNT
};

//---------------------------------
// OscillatorParameters
//
// Parameters to set up an oscillator
//
struct OscillatorParameters
{
	float level = 1.f;
	float frequencyMultiplier = 1.f;
	E_PatternType patternType = E_PatternType::Sine;
};

//---------------------------------
// Oscillator
//
// Generates an oscillating signal of a certain pattern
//
class Oscillator
{
public:
	Oscillator(float const frequency, OscillatorParameters const& params);

	void SetFrequency(float const val) { m_Frequency = static_cast<double>(val); }
	void SetPattern(E_PatternType const patternType);

	float GetSample(double const dt);
	float GetLevel() const { return m_Parameters.level; }

private:
	float m_Phase = 0.0;
	double m_Frequency;  // we store this as a double to avoid casting at runtime
	std::unique_ptr<I_WavePattern> m_Pattern;
	OscillatorParameters const& m_Parameters;
};
