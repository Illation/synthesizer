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
// WaveTable::AddTable
//
// Add a table to look up from and return a reference to it
//
WaveTable::T_Table& WaveTable::AddTable(T_Table const& table, float const morphLevel)
{
	// find the first other table that has a larger morph level
	auto foundNextIt = std::find_if(m_Tables.rbegin(), m_Tables.rend(), [morphLevel](T_MorphTable const& nextTable)
	{
		return morphLevel < nextTable.first;
	});

	// if there is none we add our new table at the end of the list
	if (foundNextIt == m_Tables.rend())
	{
		m_Tables.emplace_back(morphLevel, table);
		return (*(std::prev(m_Tables.end()))).second;
	}

	// otherwise we insert it in front of the other iterator
	return (*(m_Tables.emplace((foundNextIt + 1).base(), morphLevel, table))).second;
}

//---------------------------------
// WaveTable::GetSignal
//
// Morphs between tables at the current phase
//
float WaveTable::GetSignal(float const phase) const
{
	assert(m_Tables.size() > 0u);

	// for 1d tables just return a lookup from the first
	if (m_Tables.size() == 1u)
	{
		return LookupTable(phase, m_Tables[0].second);
	}

	// otherwise blend between two

	auto firstTableIt = std::find_if(m_Tables.cbegin(), m_Tables.cend(), [phase](T_MorphTable const& morphTable)
	{
		return morphTable.first >= phase;
	});


	if (firstTableIt == m_Tables.cend())
	{
		return LookupTable(phase, (*std::prev(firstTableIt)).second);
	}

	return LookupTable(phase, (*firstTableIt).second);

	//auto secondTableIt = firstTableIt;
}

//---------------------------------
// WaveTable::LookupTable
//
// Look up the signal of a single morph table at the phase
//
float WaveTable::LookupTable(float const phase, T_Table const& table) const
{
	assert(table.size() > 1u);

	float const adjustedPhase = (phase >= 1.f) ? phase - 1.f : phase;

	// distance between table steps
	float const delta = 1.f / static_cast<float>(table.size());

	// indicies of values surrounding the current phase
	size_t const lowerIdx = static_cast<size_t>(adjustedPhase / delta);
	size_t upperIdx = lowerIdx + 1;

	// blend values between phases
	float const alpha = (adjustedPhase - static_cast<float>(lowerIdx) * delta); // / delta;

	// adjust upper index by modulo to avoid looking into invalid array elements
	upperIdx %= table.size();

	return table[lowerIdx] + alpha * (table[upperIdx] - alpha);
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
