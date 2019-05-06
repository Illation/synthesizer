#include "stdafx.h"
#include "Oscillator.h"

//---------------------------------
// Oscillator::Oscillator
//
// Oscillator constructor
//
Oscillator::Oscillator(float const frequency, OscillatorParameters const& params)
	: m_Frequency(static_cast<double>(frequency))
	, m_Parameters(params)
{
	SetPattern(params.patternType);
}

//---------------------------------
// Oscillator::SetPattern
//
// Create the pattern from a pattern type
//
void Oscillator::SetPattern(E_PatternType const patternType)
{
	// set the pattern
	I_WavePattern* wavePattern = nullptr;
	switch (patternType)
	{
	case E_PatternType::Sine:
		wavePattern = new SinePattern();
		break;
	case E_PatternType::Saw:
		wavePattern = new SawPattern();
		break;
	case E_PatternType::Square:
		wavePattern = new SquarePattern();
		break;
	case E_PatternType::Triangle:
		wavePattern = new TrianglePattern();
		break;
	}

	m_Pattern = std::unique_ptr<I_WavePattern>(wavePattern);
}

//---------------------------------
// Oscillator::GetSample
//
// Use our pattern function to return a sample 
//
float Oscillator::GetSample(double const dt)
{
	// Change the phase based on our time delta since the last sample - at a higher frequency we change phase quicker
	m_Phase += static_cast<float>(dt * m_Frequency * m_Parameters.frequencyMultiplier);
	if (m_Phase > 1.0)
	{
		m_Phase -= 1.0;
	}
	//m_Phase -= std::floor(m_Phase);

	// Return the result of our oscillator function
	return m_Pattern->GetSignal(m_Phase);
}

