#pragma once

// Inline functions
//////////////////////

//---------------------------------
// Framework::AudioCallback
//
// Templated callback function for RtAudio - called when our stream requests more data
// #todo: resolve midi times
//
template<typename T>
int32 Framework::AudioCallback(void *outputBuffer, void* inputBuffer, uint32 nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
	UNUSED(inputBuffer);
	UNUSED(streamTime);		// should be used to sync MIDI events properly

	if (status == RTAUDIO_OUTPUT_UNDERFLOW)
	{
		LOG("onRtAudioCallback > Output underflow detected, buffer ran low!", Warning);
	}

	T* out = static_cast<T*>(outputBuffer);
	Synthesizer* synth = static_cast<Synthesizer*>(userData);

	static float const s_maxInt16 = static_cast<float>(std::numeric_limits<T>::max());
	for (uint32 i = 0; i < nBufferFrames; i++)
	{
		std::vector<float> channels = synth->GetSample();
		for (float const channel : channels)
		{
			*out++ = static_cast<T>(channel * s_maxInt16);
		}
	}

	enum class E_CallbackResult : int32
	{
		Continue = 0,
		Stop = 1,
		Abort = 2
	};
	return static_cast<int32>(E_CallbackResult::Continue);
}
