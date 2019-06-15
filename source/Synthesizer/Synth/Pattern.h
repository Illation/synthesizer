#pragma once

//---------------------------------
// I_WavePattern
//
// Interface for a class with a function that turns a phase into a signal between 0 and 1
//
class I_WavePattern
{
public:
	virtual ~I_WavePattern() {}

	virtual float GetSignal(float const phase) const = 0;
};

//---------------------------------
// SinePattern
//
//    ****
//  **    **
// *        *        *
//           **    **
//             ****
//
class SinePattern : public I_WavePattern
{
public:
	float GetSignal(float const phase) const override;
};

//---------------------------------
// SawPattern
//
//         **
//     **** *
//  ***     *     **
//          * ****   
//          **     
//
class SawPattern : public I_WavePattern
{
public:
	float GetSignal(float const phase) const override;
};

//---------------------------------
// SquarePattern
//
// **********
// *        *
// *        *        *
//          *        *
//          **********
//
class SquarePattern : public I_WavePattern
{
public:
	SquarePattern(float const pulseWidth = 0.5f) : m_PulseWidth(pulseWidth) {}

	float GetSignal(float const phase) const override;

	void SetPulseWidth(float const val) { m_PulseWidth = val; }

private:

	// Data
	///////

	float m_PulseWidth = 0.5f;
};

//---------------------------------
// TrianglePattern
//
//     **    
//   **  **  
// **      **      *
//           **  **   
//             **    
//
class TrianglePattern : public I_WavePattern
{
public:
	float GetSignal(float const phase) const override;
};

//---------------------------------
// BlendPattern
//
// Pattern generated from a lookup table
//
class WaveTable : public I_WavePattern
{
public:
	// typedefs and declarations
	//------------------
	typedef std::vector<float> T_Table;
	typedef std::pair<float, T_Table> T_MorphTable;

	// Setup
	//------------------
	WaveTable() = default;

	template<class BasePattern>
	void AddTableFromBasePattern(float const morphLevel = 0.f, size_t const samples = 256);
	T_Table& AddTable(T_Table const& table, float const morphLevel = 0.f);

	template<class BasePattern>
	static WaveTable* CreateFromBasePattern(size_t const samples = 256);

	// pattern interface
	//------------------
	float GetSignal(float const phase) const override;

	// settings 
	//------------------
	void SetMorph(float const val) { m_Morph = val; }

private:

	float LookupTable(float const phase, T_Table const& table) const;

	// Data
	///////

	std::vector<T_MorphTable> m_Tables;

	float m_Morph = 0.f;
};

//---------------------------------
// BlendPattern
//
// Pattern that can mix multiple wave patterns together
//
class BlendPattern : public I_WavePattern
{
public:
	BlendPattern(I_WavePattern* a, I_WavePattern* b, float const alpha = 0.5f);

	float GetSignal(float const phase) const override;

	void SetAlpha(float const val) { m_Alpha = val; }

private:

	// Data
	///////

	std::unique_ptr<I_WavePattern> m_PatternA;
	std::unique_ptr<I_WavePattern> m_PatternB;
	float m_Alpha = 0.5f;
};

#include "Pattern.inl"
