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
	float GetSignal(float const phase) const;
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
	float GetSignal(float const phase) const;
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

	float GetSignal(float const phase) const;

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
	float GetSignal(float const phase) const;
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

	float GetSignal(float const phase) const;

	void SetAlpha(float const val) { m_Alpha = val; }

private:

	// Data
	///////

	std::unique_ptr<I_WavePattern> m_PatternA;
	std::unique_ptr<I_WavePattern> m_PatternB;
	float m_Alpha = 0.5f;
};