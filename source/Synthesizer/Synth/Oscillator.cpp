#include "stdafx.h"
#include "Oscillator.h"


//==============================================
// E_PatternType
//==============================================

//---------------------------------
// GetPatternName
//
// Converts a pattern type into a string
//
std::string GetPatternName(E_PatternType const patternType)
{
	std::string patternString;
	switch (patternType)
	{
	case E_PatternType::Sine:
		patternString = "Sine Wave";
		break;
	case E_PatternType::Square:
		patternString = "Square Wave";
		break;
	case E_PatternType::Saw:
		patternString = "Saw Wave";
		break;
	case E_PatternType::Triangle:
		patternString = "Triangle Wave";
		break;
	}
	return patternString;
}

//==============================================
// Oscillator
//==============================================

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
	float const deltaPhase = static_cast<float>(dt * m_Frequency * m_Parameters.frequencyMultiplier);

	m_Phase += deltaPhase;
	if (m_Phase > 1.0)
	{
		m_Phase -= 1.0;
	}
	//m_Phase -= std::floor(m_Phase);

	// Return the result of our oscillator function
	float signal = m_Pattern->GetSignal(m_Phase); 

	// reduce harmonic aliasing
	if (m_Parameters.usePolyBlep)
	{
		switch (m_Parameters.patternType)
		{
		case E_PatternType::Saw:
			signal -= GetPolyBlep(deltaPhase, std::fmod(m_Phase + 0.5f, 1.f));
			break;
		case E_PatternType::Square:
		//case E_PatternType::Triangle:
			signal += GetPolyBlep(deltaPhase, m_Phase);
			signal -= GetPolyBlep(deltaPhase, std::fmod(m_Phase + 0.5f, 1.f));
			break;
		}
	}

	// we're done
	return signal;
}

//---------------------------------
// Oscillator::GetPolyBlep
//
// Get the required offset to reduce harmonic aliasing or raw waveforms
//
float Oscillator::GetPolyBlep(float const deltaPhase, float const phase) const
{
	if (phase < deltaPhase) // 0 <= phase < 1
	{
		float const t = phase / deltaPhase;
		return t + t - t * t - 1.f;
	}
	else if (phase > 1.0 - deltaPhase) // -1 < phase < 0
	{
		float const t = (phase - 1.f) / deltaPhase;
		return t * t + t + t + 1.f;
	}

	// 0 otherwise
	return 0.f;
}

