#include "stdafx.h"
#include "Pattern.h"


//---------------------------------
// SinePattern::GetSignal
//
// return a sine wave
//
float SinePattern::GetSignal(float const phase) const
{
	return std::sinf(etm::PI2 * phase);
}

//---------------------------------
// SawPattern::GetSignal
//
// return a saw wave
//
float SawPattern::GetSignal(float const phase) const
{
	return 2.f * (phase < 0.5f ? phase : phase - 1.f);
}

//---------------------------------
// SquarePattern::GetSignal
//
// return a square wave, where the thickness can be modified with the pulse width
//
float SquarePattern::GetSignal(float const phase) const
{
	return phase < m_PulseWidth ? 0.999f : -0.999f;
}

//---------------------------------
// TrianglePattern::GetSignal
//
// return a triangle wave
//
float TrianglePattern::GetSignal(float const phase) const
{
	float adjustedPhase = phase + 0.25f;
	adjustedPhase -= std::floorf(adjustedPhase);
	adjustedPhase = std::fabs(adjustedPhase * 2.f - 1.f);
	return adjustedPhase * -2.f + 1.f;
}

//---------------------------------
// BlendPattern::BlendPattern
//
// BlendPattern constructor
//
BlendPattern::BlendPattern(I_WavePattern* a, I_WavePattern* b, float const alpha) 
	: m_PatternA(std::unique_ptr<I_WavePattern>(a))
	, m_PatternB(std::unique_ptr<I_WavePattern>(b))
	, m_Alpha(alpha) 
{}

//---------------------------------
// BlendPattern::GetSignal
//
// return a blend of both patterns, - linear interpolation
//
float BlendPattern::GetSignal(float const phase) const
{
	float const a = m_PatternA->GetSignal(phase);
	float const b = m_PatternB->GetSignal(phase);
	return a + m_Alpha * (b - m_Alpha);
}