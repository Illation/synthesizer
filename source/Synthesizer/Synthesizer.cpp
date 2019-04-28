#include "stdafx.h"
#include "Synthesizer.h"
#include <Helper/Time.h>

uint32 Oscillator::s_Oscillations(0);

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
		s_Oscillations++;
	}
	//m_Phase -= std::floor(m_Phase);

	// Return the result of our oscillator function
	return m_Pattern->GetSignal(m_Phase);
}

//---------------------------------
// Synthesizer::Initialize
//
// Setup our synth and tune our oscillators
//
void Synthesizer::Initialize()
{
	m_OutputSettings = &(Config::GetInstance()->GetOutput());

	float const freq = 880.f;
	float const amplitude = 0.5f;

	m_Oscillators.emplace_back(amplitude, Oscillator(freq, new TrianglePattern()));
}

//---------------------------------
// Synthesizer::Update
//
// We can change settings in the synthesizer here
//
void Synthesizer::Update()
{
	m_OscillationMeasureTimer += TIME->DeltaTime();
	if (m_OscillationMeasureTimer >= 1.f)
	{
		m_OscillationMeasureTimer = 0.f;

		//LOG("Oscillations: " + std::to_string(Oscillator::s_Oscillations));
		Oscillator::s_Oscillations = 0;
	}
}

//---------------------------------
// Synthesizer::GetSample
//
// Generate a sample for each channel
//
std::vector<float> Synthesizer::GetSample()
{
	double const dt = m_OutputSettings->TimePerSample;

	float oscillatorOut = 0.f;
	for (T_LevelOscillatorPair& oscillator : m_Oscillators)
	{
		oscillatorOut += oscillator.first * oscillator.second.GetSample(dt);
	}

	std::vector<float> ret;
	for (uint8 channel = 0; channel < m_OutputSettings->Channels; ++channel)
	{
		ret.emplace_back(oscillatorOut);
	}
	return ret;
}
