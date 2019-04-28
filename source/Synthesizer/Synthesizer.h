#pragma once

//---------------------------------
// Synthesizer
//
// Generates sounds
//
class Synthesizer
{
public:
	void GetSample(float& left, float& right);

private:
	float left_phase;
	float right_phase;
};


