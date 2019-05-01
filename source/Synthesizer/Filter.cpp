#include "stdafx.h"
#include "Filter.h"


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
FilterParams::FilterParams(FilterMode const mode, double const cutoff, double const resonance)
	: m_Mode(mode)
	, m_Cutoff(cutoff)
	, m_Resonance(resonance)
{
	CalculateFeedbackAmount();
}

//---------------------------------
// FilterParams::CalculateFeedbackAmount
//
// Calculate derived variables
//
void FilterParams::CalculateFeedbackAmount()
{
	m_FeedbackAmount = m_Resonance + m_Resonance / (1.0 - m_Cutoff);
}

//============
// Filter
//============

//---------------------------------
// Filter::GetSignal
//
// Processes the input signal from the synthesizer
//
double Filter::GetSignal(double const input)
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
		return 0.0;
	}
}
