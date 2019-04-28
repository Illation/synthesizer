#include "stdafx.h"
#include "Synthesizer.h"

//---------------------------------
// Synthesizer::GetSample
//
// Generate a sample at a given time
//
void Synthesizer::GetSample(float& left, float& right)
{
	left = left_phase;
	right = right_phase;

	// Generate simple sawtooth phaser that ranges between -1.0 and 1.0
	left_phase += 0.01f;
	
	// When signal reaches top, drop back down
	if (left_phase >= 1.0f)
	{
		left_phase -= 2.0f;
	}

	// higher pitch so we can distinguish left and right
	right_phase += 0.03f;
	if (right_phase >= 1.0f)
	{
		right_phase -= 2.0f;
	}
}

