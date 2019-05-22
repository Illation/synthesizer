#include "stdafx.h"
#include "LowEndAudioManager.h"

#include "Config.h"

//====================
// Low End Audio Manager 
//====================

//---------------------------------
// LowEndAudioManager::InitializeAudio
//
// Initialize RtAudio
//
bool LowEndAudioManager::InitializeAudio(Synthesizer* const synth)
{
	m_Synthesizer = synth;

	// try initializing rt audio with stored API
	Config::Settings::Output& output = Config::GetInstance()->GetOutput();
	RtAudio::Api const api = RtAudio::getCompiledApiByName(output.ApiId);
	if (api == RtAudio::Api::UNSPECIFIED)
	{
		m_Audio = new RtAudio();
		output.ApiId = GetActiveApiId();
		Config::GetInstance()->Save();
	}
	else
	{
		m_Audio = new RtAudio(api);
	}

	if (m_Audio == nullptr)
	{
		LOG("LowEndAudioManager::InitializeAudio > Failed to initialize RT Audio!", Warning);
		return false;
	}

	LOG("RtAudio version: " + m_Audio->getVersion());
	LOG("RtAudio API: " + RtAudio::getApiDisplayName(m_Audio->getCurrentApi()));

	// Determine the number of devices available
	uint32 deviceCount = m_Audio->getDeviceCount();
	if (deviceCount == 0)
	{
		LOG("LowEndAudioManager::InitializeAudio > No sound device found!", Warning);
		return false;
	}

	// try setting the device with stored config data, otherwise set it from default device
	if (output.DeviceId >= 0 && static_cast<uint32>(output.DeviceId) < deviceCount)
	{
		m_Parameters.deviceId = static_cast<uint32>(output.DeviceId);
	}
	else
	{
		uint32 defaultDeviceIdx = m_Audio->getDefaultOutputDevice();
		RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(defaultDeviceIdx);
		if (info.probed == true)
		{
			LOG("No preferred audio device, using default: #" + std::to_string(defaultDeviceIdx) + std::string(": '") + info.name + std::string("'"));
		}

		m_Parameters.deviceId = defaultDeviceIdx;
		output.DeviceId = static_cast<int32>(defaultDeviceIdx);
		Config::GetInstance()->Save();
	}

	m_Parameters.nChannels = output.Channels;

	// set up sample rate from config
	m_CurrentSampleRate = output.SampleRate;

	ValidateCurrentSampleRate();

	SetupStreamOptions();

	if (!OpenAndStartStream())
	{
		return false;
	}

	LOG("");
	return true;
}

//---------------------------------
// LowEndAudioManager::TerminateAudio
//
// Destroy audio stuff
//
void LowEndAudioManager::TerminateAudio()
{
	m_Audio->stopStream();

	if (m_Audio->isStreamOpen())
	{
		m_Audio->closeStream();
	}

	SafeDelete(m_Audio);
}

//---------------------------------
// LowEndAudioManager::GetAllPossibleApis
//
// List all available audio APIs
//
void LowEndAudioManager::GetAllPossibleApis(std::vector<T_ApiIdNamePair>& apiIdNamePairs) const
{	
	std::vector<RtAudio::Api> apis;
	RtAudio::getCompiledApi(apis);

	apiIdNamePairs.clear();
	for (RtAudio::Api const api : apis)
	{
		apiIdNamePairs.emplace_back(RtAudio::getApiName(api), RtAudio::getApiDisplayName(api));
	}
}

//---------------------------------
// LowEndAudioManager::GetActiveApiId
//
// Get the id (non display name) of the active api
//
std::string LowEndAudioManager::GetActiveApiId() const
{
	if (m_Audio == nullptr)
	{
		LOG("Framework::GetActiveApiId > m_Audio was nullptr!", LogLevel::Error);
		return "";
	}

	return RtAudio::getApiName(m_Audio->getCurrentApi());
}

//---------------------------------
// LowEndAudioManager::SetActiveApi
//
// Set the api from a string which is not the display name
//
void LowEndAudioManager::SetActiveApi(std::string const& apiId)
{
	RtAudio::Api const api = RtAudio::getCompiledApiByName(apiId);
	if (api == RtAudio::Api::UNSPECIFIED)
	{
		LOG("Framework::SetActiveApi > couldn't associate api with api id '" + apiId + std::string("' !"), LogLevel::Warning);
		return;
	}

	// stop everything
	TerminateAudio();

	m_Audio = new RtAudio(api); 
	if (m_Audio == nullptr)
	{
		LOG("LowEndAudioManager::SetActiveApi > Failed to initialize RT Audio!", LogLevel::Error);
		return;
	}

	// Determine the number of devices available
	uint32 deviceCount = m_Audio->getDeviceCount();
	if (deviceCount == 0)
	{
		LOG("LowEndAudioManager::SetActiveApi > No sound device found!", LogLevel::Error);
		return;
	}

	Config::Settings::Output& output = Config::GetInstance()->GetOutput();

	// try setting the device with stored config data, otherwise set it from default device
	m_Parameters.deviceId = m_Audio->getDefaultOutputDevice();
	output.DeviceId = static_cast<int32>(m_Parameters.deviceId);

	ValidateCurrentSampleRate();

	if (!OpenAndStartStream())
	{
		return;
	}

	output.ApiId = apiId;
	Config::GetInstance()->Save();
}

//---------------------------------
// LowEndAudioManager::GetAllPossibleDevices
//
// List all available audio devices/soundcards
//
void LowEndAudioManager::GetAllPossibleDevices(std::vector<T_DeviceIdNamePair>& deviceIdNamePairs) const
{
	// Determine the number of devices available
	uint32 deviceCount = m_Audio->getDeviceCount();

	deviceIdNamePairs.clear();
	for (uint32 deviceIdx = 0; deviceIdx < deviceCount; deviceIdx++)
	{
		RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(deviceIdx);
		if (info.probed == true && info.outputChannels > 0)
		{
			deviceIdNamePairs.emplace_back(deviceIdx, info.name);
		}
	}
}

//---------------------------------
// LowEndAudioManager::GetActiveDevice
//
// Get the currently used soundcard
//
uint32 LowEndAudioManager::GetActiveDevice() const
{
	return m_Parameters.deviceId;
}

//---------------------------------
// LowEndAudioManager::SetActiveDevice
//
// Set the active soundcard
//
void LowEndAudioManager::SetActiveDevice(uint32 const deviceId)
{
	// Determine the number of devices available
	uint32 deviceCount = m_Audio->getDeviceCount();
	if (deviceCount <= deviceId)
	{
		LOG("LowEndAudioManager::SetActiveDevice > Invalid device index, num devices '" + std::to_string(deviceCount) + std::string("', id '")
			+ std::to_string(deviceId) + std::string("'"), LogLevel::Error);
		return;
	}
	m_Parameters.deviceId = deviceId;

	ValidateCurrentSampleRate();

	if (m_Audio->isStreamOpen())
	{
		m_Audio->closeStream();
	}

	if (!OpenAndStartStream())
	{
		return;
	}

	Config::GetInstance()->GetOutput().DeviceId = static_cast<int32>(deviceId);
	Config::GetInstance()->Save();
}

//---------------------------------
// LowEndAudioManager::GetAllPossibleSampleRates
//
// List all sample rates supported by the selected device
//
void LowEndAudioManager::GetAllPossibleSampleRates(std::vector<uint32>& sampleRates) const
{
	sampleRates.clear();

	RtAudio::DeviceInfo const info = m_Audio->getDeviceInfo(m_Parameters.deviceId);
	if (info.probed)
	{
		sampleRates = info.sampleRates;
	}
}

//---------------------------------
// LowEndAudioManager::SetActiveSampleRate
//
// Set the sample rate for the active device
//
void LowEndAudioManager::SetActiveSampleRate(uint32 const sampleRate)
{
	RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(m_Parameters.deviceId);
	if (!info.probed)
	{
		LOG("LowEndAudioManager::SetActiveSampleRate > failed to get device info", LogLevel::Warning);
		return;
	}

	std::vector<uint32> sampleRates = info.sampleRates;
	if (std::find(sampleRates.cbegin(), sampleRates.cend(), m_CurrentSampleRate) != sampleRates.cend())
	{
		m_CurrentSampleRate = sampleRate;
	}
	else
	{
		LOG("LowEndAudioManager::SetActiveSampleRate > sample rate '" + std::to_string(sampleRate)
			+ std::string("' is not supported by '") + info.name + std::string("'"), LogLevel::Warning);

		return;
	}

	if (m_Audio->isStreamOpen())
	{
		m_Audio->closeStream();
	}

	if (!OpenAndStartStream())
	{
		return;
	}

	Config::GetInstance()->GetOutput().SampleRate = sampleRate;
	Config::GetInstance()->GetOutput().DeriveSettings();
	Config::GetInstance()->Save();
}

//---------------------------------
// LowEndAudioManager::OnRtAudioError
//
// Log stuff when rt audio spits out an error
//
void LowEndAudioManager::OnRtAudioError(RtAudioError::Type type, std::string const& errorText)
{
	LogLevel logLevel = LogLevel::Error;
	switch (type)
	{
	case RtAudioError::WARNING:
	case RtAudioError::DEBUG_WARNING:
	case RtAudioError::UNSPECIFIED:
	case RtAudioError::NO_DEVICES_FOUND:
		logLevel = LogLevel::Warning;
	}

	LOG(errorText, logLevel);
}

//---------------------------------
// LowEndAudioManager::OpenAndStartStream
//
// Start the stream with the settings we set up earlier and start it
//
bool LowEndAudioManager::OpenAndStartStream()
{
	Config::Settings::Output & output = Config::GetInstance()->GetOutput();
	uint32 framesPerBuffer = output.FramesPerBuffer; // RtAudios underlying API might change this value

	try
	{
		m_Audio->openStream(&m_Parameters,
			nullptr,
			RTAUDIO_SINT16,
			m_CurrentSampleRate,
			&framesPerBuffer,
			&LowEndAudioManager::AudioCallback<int16>,
			m_Synthesizer,
			&m_Options,
			&LowEndAudioManager::OnRtAudioError);
	}
	catch (RtAudioError& error)
	{
		OnRtAudioError(error.getType(), error.getMessage());
		return false;
	}

	LOG("Opened a stream with '" + std::to_string(framesPerBuffer) + std::string("' frames per buffer."));
	LOG("\tNumber of buffers in this stream: '" + std::to_string(m_Options.numberOfBuffers) + std::string("'"));
	LOG("\tStream callbackFn priority: '" + std::to_string(m_Options.priority) + std::string("'"));

	m_Audio->startStream();

	return true;
}

//---------------------------------
// LowEndAudioManager::SetupStreamOptions
//
// Set stream options, focused on low latency
//
void LowEndAudioManager::SetupStreamOptions()
{
	m_Options.flags |= RTAUDIO_MINIMIZE_LATENCY;
	if (false)
	{
		m_Options.flags |= RTAUDIO_NONINTERLEAVED;
	}
	if (false) // exclusive device access
	{
		m_Options.flags |= RTAUDIO_HOG_DEVICE;
	}
	if (false) // some scheduling stuff I don't understand quite yet
	{
		m_Options.flags |= RTAUDIO_SCHEDULE_REALTIME;
		m_Options.priority = 900000000;
	}
	if (m_Audio->getCurrentApi() == RtAudio::LINUX_ALSA)
	{
		m_Options.flags |= RTAUDIO_ALSA_USE_DEFAULT;
	}
	m_Options.streamName = "Synthesizer";
	m_Options.numberOfBuffers = 0; // as low as possible
}

//---------------------------------
// LowEndAudioManager::ValidateCurrentSampleRate
//
// Check the set sample rate works for the current device, if not switch and save it to config
//
void LowEndAudioManager::ValidateCurrentSampleRate()
{
	RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(m_Parameters.deviceId);
	if (info.probed == true)
	{
		std::vector<uint32> sampleRates = info.sampleRates;
		if (std::find(sampleRates.cbegin(), sampleRates.cend(), m_CurrentSampleRate) == sampleRates.cend())
		{
			LOG("LowEndAudioManager::ValidateCurrentSampleRate > sample rate '" + std::to_string(m_CurrentSampleRate) 
				+ std::string("' is not supported by '") + info.name + std::string("' - switching to '") 
				+ std::to_string(info.preferredSampleRate) + std::string("'"), LogLevel::Warning);

			m_CurrentSampleRate = info.preferredSampleRate;

			Config::GetInstance()->GetOutput().SampleRate = m_CurrentSampleRate;
			Config::GetInstance()->GetOutput().DeriveSettings();
			Config::GetInstance()->Save();
		}
	}
}
