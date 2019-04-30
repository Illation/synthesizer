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
// AdsrParameters::CalculateRuntimeParams
//
// should be recalculated every time any of the parameters change
//
void AdsrParameters::CalculateRuntimeParams()
{
	sustainTime = attack + decay;
	releaseTime = sustainTime + release;
	decayVelocity = decay * (1.0 - sustain);
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
		if (m_Time > m_Params.GetSustainTime() || m_Time < 0.0) // reset to 0 if we are just starting or if we where releaseing
		{
			m_Time = 0.0;
			m_Level = 0.f;
		}

		// increase timer if we are not sustaining
		if (!(m_Time == m_Params.GetSustainTime()))
		{
			m_Time += dt;
		}
		
		if (m_Time <= m_Params.GetAttack()) // increase during attack phase
		{
			m_Level = m_Time / m_Params.GetAttack();
		}
		else if (m_Time <= m_Params.GetSustainTime()) // decrease during decay phase
		{
			m_Level -= dt * m_Params.GetDecayVelocity();
		}
		else // hold at the same level once we are sustainging
		{
			m_Time = m_Params.GetSustainTime();
			m_Level = m_Params.GetSustain();
		}
	}
	else
	{
		if (m_Time >= 0) // we are still in the release phase
		{
			if (m_Time < m_Params.GetSustainTime()) // in case we released before reaching sustain we set the timer to begin after the sustain time
			{
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