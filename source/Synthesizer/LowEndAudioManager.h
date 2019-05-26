#pragma once

#include <Synth/Synthesizer.h>

#include <Vendor/RtAudio/RtAudio.h>

//---------------------------------
// LowEndAudioManager
//
// Manages the setup of audio settings on our soundcard
//
class LowEndAudioManager final : public Singleton<LowEndAudioManager>
{
public:
	// typedefs and declarations
	//------------------
	friend class Singleton<LowEndAudioManager>;
	friend class SynthApp;

	typedef std::pair<std::string, std::string> T_ApiIdNamePair;
	typedef std::pair<uint32, std::string> T_DeviceIdNamePair;

private:
	// private init deinit
	//------------------

	LowEndAudioManager() = default;
	virtual ~LowEndAudioManager() = default;

	bool InitializeAudio(Synthesizer* const synth); // returns false if something went wrong
	void TerminateAudio();

public:
	// public interface
	//------------------

	// api setting
	void GetAllPossibleApis(std::vector<T_ApiIdNamePair>& apiIdNamePairs) const;
	std::string GetActiveApiId() const;
	void SetActiveApi(std::string const& apiId);

	// device setting
	void GetAllPossibleDevices(std::vector<T_DeviceIdNamePair>& deviceIdNamePairs) const;
	uint32 GetActiveDevice() const;
	void SetActiveDevice(uint32 const deviceId);

	// format
	void GetAllPossibleSampleRates(std::vector<uint32>& sampleRates) const;
	uint32 GetCurrentSampleRate() const { return m_CurrentSampleRate; }
	void SetActiveSampleRate(uint32 const sampleRate);

protected:
	// audio stream callback fn
	//--------------------------
	template<typename T>
	static int32 AudioCallback(void *outputBuffer, 
		void* inputBuffer, 
		uint32 nBufferFrames, 
		double streamTime, 
		RtAudioStreamStatus status, 
		void* userData);

	static void OnRtAudioError(RtAudioError::Type type, std::string const& errorText);

private:

	bool OpenAndStartStream();
	void SetupStreamOptions();
	void ValidateCurrentSampleRate();

	// Data
	////////

private:
	RtAudio* m_Audio;
	RtAudio::StreamParameters m_Parameters;
	RtAudio::StreamOptions m_Options;
	uint32 m_CurrentSampleRate = 0;

	Synthesizer* m_Synthesizer;
};

#include "LowEndAudioManager.inl"