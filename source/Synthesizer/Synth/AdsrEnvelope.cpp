#include "stdafx.h"
#include "AdsrEnvelope.h"

//---------------------------------
// AdsrParameters::AdsrParameters
//
// AdsrParameters default constructor
//
AdsrParameters::AdsrParameters()
{
	CalculateRuntimeParams();
}

//---------------------------------
// AdsrParameters::AdsrParameters
//
// AdsrParameters parametric contructor
//
AdsrParameters::AdsrParameters(double const a, double const d, double const s, double const r) 
	: attack(a)
	, decay(d)
	, sustain(s)
	, release(r)
{
	CalculateRuntimeParams();
}

//---------------------------------
// AdsrParameters::SetAttack
//
// Set attack and recalculate its derived parameters
//
void AdsrParameters::SetAttack(double const val)
{
	attack = val;
	CalculateSustainTime();
}

//---------------------------------
// AdsrParameters::SetDecay
//
// Set decay and recalculate its derived parameters
//
void AdsrParameters::SetDecay(double const val)
{
	decay = val;
	CalculateSustainTime();
}

//---------------------------------
// AdsrParameters::SetSustain
//
// Set sustain and recalculate its derived parameters
//
void AdsrParameters::SetSustain(double const val)
{
	sustain = val;
	CalculateSustainInv();
}

//---------------------------------
// AdsrParameters::SetRelease
//
// Set release and recalculate its derived parameters
//
void AdsrParameters::SetRelease(double const val)
{
	release = val;
	CalculateReleaseTime();
}

//---------------------------------
// AdsrParameters::CalculateSustainTime
//
// Calculate sustain time from attack and decay
//
void AdsrParameters::CalculateSustainTime()
{
	sustainTime = attack + decay;
	CalculateReleaseTime();
}

//---------------------------------
// AdsrParameters::CalculateReleaseTime
//
// calculate release time from sustain time and release
//
void AdsrParameters::CalculateReleaseTime()
{
	releaseTime = sustainTime + release;
}

//---------------------------------
// AdsrParameters::CalculateSustainInv
//
// Calculate inverse sustain from sustain
//
void AdsrParameters::CalculateSustainInv()
{
	sustainInv = 1.0 - sustain;
}

//---------------------------------
// AdsrParameters::CalculateRuntimeParams
//
// should be recalculated every time any of the parameters change
//
void AdsrParameters::CalculateRuntimeParams()
{
	CalculateSustainTime();
	CalculateSustainInv();
}

//---------------------------------
// AdsrEnvelope::GetSignal
//
// Updates the envelope and returns its signal
//
float AdsrEnvelope::GetSignal(bool const input, double const dt)
{
	if (input) // the key is pressed
	{
		if (m_State == EnvelopeState::Release || m_Time < 0.0) // reset to 0 if we are just starting or if we where releaseing
		{
			m_State = EnvelopeState::Attack;
			m_Time = 0.0;
			m_Level = 0.f;
		}

		switch (m_State)
		{
		case EnvelopeState::Attack:
			m_Time += dt;
			if (m_Time >= m_Params.GetAttack())
			{
				if (m_Time >= m_Params.GetSustainTime())
				{
					m_State = EnvelopeState::Sustain;
					m_Level = m_Params.GetSustain();
				}
				else
				{
					m_State = EnvelopeState::Decay;
					m_Level = 1.0;
				}
			}
			else // level during attack phase
			{
				m_Level = m_Time / m_Params.GetAttack();
			}
			break;
		case EnvelopeState::Decay:
			m_Time += dt;
			if (m_Time > m_Params.GetSustainTime())
			{
				m_State = EnvelopeState::Sustain;
				m_Level = m_Params.GetSustain();
			}
			else // level during decay phase
			{
				m_Level = 1.0 - ((m_Time - m_Params.GetAttack()) / m_Params.GetDecay()) * m_Params.GetSustainInv();
			}
			break;
		case EnvelopeState::Sustain: // during sustain phase we do nothing
			break;
		}
	}
	else
	{
		if (m_Time >= 0) // we are still in the release phase
		{
			if (m_State != EnvelopeState::Release) // in case we released before reaching sustain we set the timer to begin after the sustain time
			{
				m_State = EnvelopeState::Release;
				m_Time = m_Params.GetSustainTime();
			}
			
			// increase the timer
			m_Time += dt;

			if (m_Time >= m_Params.GetReleaseTime()) // reset if we finished sustaining
			{
				m_Time = -1.0;
				m_Level = 0.0;
			}
			else
			{
				// interpolate between the level we released at and 0 based on how much time we spent in the release phase
				return static_cast<float>(m_Level * (1.0 - ((m_Time - m_Params.GetSustainTime()) / m_Params.GetRelease())));
			}
		}
		else // we finished releasing, return 0
		{
			m_Level = 0.f;
		}
	}

	return static_cast<float>(m_Level);
}