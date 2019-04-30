#include "stdafx.h"
#include "Oscillator.h"

//---------------------------------
// Oscillator::Oscillator
//
// Oscillator constructor
//
Oscillator::Oscillator(float const frequency, I_WavePattern* pattern)
	: m_Frequency(static_cast<double>(frequency))
	, m_Pattern(std::unique_ptr<I_WavePattern>(pattern))
{

}

//---------------------------------
// Oscillator::GetSample
//
// Use our pattern function to return a sample 
//
float Oscillator::GetSample(double const dt)
{
	// Change the phase based on our time delta since the last sample - at a higher frequency we change phase quicker
	m_Phase += static_cast<float>(dt * m_Frequency);
	if (m_Phase > 1.0)
	{
		m_Phase -= 1.0;
	}
	//m_Phase -= std::floor(m_Phase);

	// Return the result of our oscillator function
	return m_Pattern->GetSignal(m_Phase);
}

