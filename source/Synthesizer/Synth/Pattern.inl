#pragma once



//===================
// Wave Table
//===================


//---------------------------------
// WaveTable::WaveTable
//
// Construct a wave pattern from another pattern, then delete the other one
//
template<class BasePattern>
void WaveTable::AddTableFromBasePattern(float const morphLevel, size_t const samples)
{
	BasePattern basePattern;

	T_Table& newTable = AddTable(T_Table(), morphLevel);
	for (size_t i = 0; i < samples; ++i)
	{
		newTable.emplace_back(basePattern.GetSignal(static_cast<float>(i) / static_cast<float>(samples)));
	}
}

//---------------------------------
// WaveTable::CreateFromBasePattern
//
// Create a new wave table from a base pattern
//
template<class BasePattern>
WaveTable* WaveTable::CreateFromBasePattern(size_t const samples /*= 256*/)
{
	WaveTable* table = new WaveTable();

	table->AddTableFromBasePattern<BasePattern>(0.f, samples);

	return table;
}

