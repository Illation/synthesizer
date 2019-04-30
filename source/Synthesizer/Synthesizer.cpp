#include "stdafx.h"
#include "Synthesizer.h"
#include <Helper/Time.h>
#include <Helper/InputManager.h>
#include "gdk/gdkkeysyms-compat.h"

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

//---------------------------------
// Synthesizer::Initialize
//
// Setup our synth and tune our oscillators
//
void Synthesizer::Initialize()
{
	m_OutputSettings = &(Config::GetInstance()->GetOutput());

	float const amplitude = 0.5f;

	m_Oscillators.emplace_back(amplitude, Oscillator(m_Frequency, new SquarePattern()));
}

//---------------------------------
// Synthesizer::Update
//
// We can change settings in the synthesizer here
//
void Synthesizer::Update()
{
	float const c_KeyChangeSpeed = 10.f;
	bool freqChanged = false;

	if (InputManager::GetInstance()->GetKeyState(GDK_Down) >= E_KeyState::Down)
	{
		m_Frequency -= TIME->DeltaTime() * c_KeyChangeSpeed;
		freqChanged = true;
	}

	if (InputManager::GetInstance()->GetKeyState(GDK_Up) >= E_KeyState::Down)
	{
		m_Frequency += TIME->DeltaTime() * c_KeyChangeSpeed;
		freqChanged = true;
	}

	if (freqChanged)
	{
		for (T_LevelOscillatorPair& oscillator : m_Oscillators)
		{
			oscillator.second.SetFrequency(m_Frequency);
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
