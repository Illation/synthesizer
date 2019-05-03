#include "stdafx.h"
#include "Filter.h"

#include <Math/MathUtil.h>


//============
// FilterParams
//============

//---------------------------------
// FilterParams::FilterParams
//
// Default filter param constructor
//
FilterParams::FilterParams()
{
	CalculateFeedbackAmount();
}

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
	CalculateFeedbackAmount();
}

//---------------------------------
// FilterParams::SetCutoff
//
// Set cutoff, clamped appropriately, recalculate feedback
//
void FilterParams::SetCutoff(float const val)
{
	m_Cutoff = etm::Clamp(val, 0.9999f, 0.0001f);
	CalculateFeedbackAmount();
}

//---------------------------------
// FilterParams::SetResonance
//
// Set resonance, recalculate feedback
//
void FilterParams::SetResonance(float const val)
{
	m_Resonance = etm::Clamp(val, 1.f, 0.f);
	CalculateFeedbackAmount();
}

//---------------------------------
// FilterParams::CalculateFeedbackAmount
//
// Calculate derived variables
//
void FilterParams::CalculateFeedbackAmount()
{
	m_FeedbackAmount = m_Resonance + m_Resonance / (1.f - m_Cutoff);
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
	// Pass through four filters, the first one adds resonance
	m_Buf0 += m_Params.GetCutoff() * (input - m_Buf0 + m_Params.GetFeedbackAmount() * (m_Buf0 - m_Buf1));
	m_Buf1 += m_Params.GetCutoff() * (m_Buf0 - m_Buf1);
	m_Buf2 += m_Params.GetCutoff() * (m_Buf1 - m_Buf2);
	m_Buf3 += m_Params.GetCutoff() * (m_Buf2 - m_Buf3);

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
