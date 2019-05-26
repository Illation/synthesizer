#include "stdafx.h"
#include "Filter.h"

#include <EtMath/MathUtil.h>


//============
// FilterParams
//============

//---------------------------------
// FilterParams::FilterParams
//
// Construct filter params with specific variables
//
FilterParams::FilterParams(FilterMode const mode, float const cutoff, float const resonance)
	: m_Mode(mode)
	, m_Cutoff(etm::Clamp(cutoff, 0.9999f, 0.0001f))
	, m_Resonance(etm::Clamp(resonance, 1.f, 0.f))
{
}

//---------------------------------
// FilterParams::SetCutoff
//
// Set cutoff, clamped appropriately, recalculate feedback
//
void FilterParams::SetCutoff(float const val)
{
	m_Cutoff = etm::Clamp(val, 0.9999f, 0.0001f);
}

//---------------------------------
// FilterParams::SetResonance
//
// Set resonance, recalculate feedback
//
void FilterParams::SetResonance(float const val)
{
	m_Resonance = etm::Clamp(val, 1.f, 0.f);
}

//============
// Filter
//============

//---------------------------------
// Filter::GetSignal
//
// Processes the input signal from the synthesizer
//
float Filter::GetSignal(float const input)
{
	float totalCutoff = m_Params.GetCutoff() + m_EnvelopeVal;
	float feedbackAmount = m_Params.GetResonance() + m_Params.GetResonance() / (1.f - totalCutoff);

	// Pass through four filters, the first one adds resonance
	m_Buf0 += totalCutoff * (input - m_Buf0 + feedbackAmount * (m_Buf0 - m_Buf1));
	m_Buf1 += totalCutoff * (m_Buf0 - m_Buf1);
	m_Buf2 += totalCutoff * (m_Buf1 - m_Buf2);
	m_Buf3 += totalCutoff * (m_Buf2 - m_Buf3);

	switch (m_Params.GetMode()) 
	{
	case FilterParams::FilterMode::lowPass:
		return m_Buf3;

	case FilterParams::FilterMode::highPass:
		return input - m_Buf3;

	case FilterParams::FilterMode::bandPass:
		return m_Buf0 - m_Buf3;

	default:
		return 0.f;
	}
}

//---------------------------------
// FilterParams::SetEnvelopeValue
//
// Adds to the cutoff
//
void Filter::SetEnvelopeValue(float const val)
{
	m_EnvelopeVal = etm::Clamp(val, 0.9999f - m_Params.GetCutoff(), 0.f);
}