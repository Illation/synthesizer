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

	m_Audio = new RtAudio(); // we could specify the library here
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

	LOG("There are '" + std::to_string(deviceCount) + std::string("' audio devices available."));

	// Scan through devices for various capabilities
	for (uint32 deviceIdx = 0; deviceIdx < deviceCount; deviceIdx++)
	{
		RtAudio::DeviceInfo info = m_Audio->getDeviceInfo(deviceIdx);
		if (info.probed == true)
		{
			LOG("\t Device #" + std::to_string(deviceIdx) + std::string(": '") + info.name + std::string("'"));
		}
	}

	// settings for our audio stream
	uint32 defaultDeviceIdx = m_Audio->getDefaultOutputDevice();
	LOG("Default device index: '" + std::to_string(defaultDeviceIdx) + std::string("'"));

	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();

	m_Parameters.deviceId = defaultDeviceIdx;
	m_Parameters.nChannels = output.Channels;

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

	uint32 framesPerBuffer = output.FramesPerBuffer; // RtAudios underlying API might change this value

	// open a stream with all our settings
	try
	{
		m_Audio->openStream(&m_Parameters,
			nullptr,
			RTAUDIO_SINT16,
			output.SampleRate * output.Channels,
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
	m_Parameters.deviceId = m_Audio->getDefaultOutputDevice();

	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();
	uint32 framesPerBuffer = output.FramesPerBuffer; // RtAudios underlying API might change this value

	try
	{
		m_Audio->openStream(&m_Parameters,
			nullptr,
			RTAUDIO_SINT16,
			output.SampleRate * output.Channels,
			&framesPerBuffer,
			&LowEndAudioManager::AudioCallback<int16>,
			m_Synthesizer,
			&m_Options,
			&LowEndAudioManager::OnRtAudioError);
	}
	catch (RtAudioError& error)
	{
		OnRtAudioError(error.getType(), error.getMessage());
		return;
	}

	LOG("Opened a stream with '" + std::to_string(framesPerBuffer) + std::string("' frames per buffer."));
	LOG("\tNumber of buffers in this stream: '" + std::to_string(m_Options.numberOfBuffers) + std::string("'"));
	LOG("\tStream callbackFn priority: '" + std::to_string(m_Options.priority) + std::string("'"));

	m_Audio->startStream();

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
		if (info.probed == true)
		{
			deviceIdNamePairs.emplace_back(deviceIdx, info.name);
		}
		else
		{
			deviceIdNamePairs.emplace_back(deviceIdx, "");
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

	m_Audio->stopStream();

	if (m_Audio->isStreamOpen())
	{
		m_Audio->closeStream();
	}

	Config::OutputSettings const& output = Config::GetInstance()->GetOutput();
	uint32 framesPerBuffer = output.FramesPerBuffer; // RtAudios underlying API might change this value

	try
	{
		m_Audio->openStream(&m_Parameters,
			nullptr,
			RTAUDIO_SINT16,
			output.SampleRate * output.Channels,
			&framesPerBuffer,
			&LowEndAudioManager::AudioCallback<int16>,
			m_Synthesizer,
			&m_Options,
			&LowEndAudioManager::OnRtAudioError);
	}
	catch (RtAudioError& error)
	{
		OnRtAudioError(error.getType(), error.getMessage());
		return;
	}

	LOG("Opened a stream with '" + std::to_string(framesPerBuffer) + std::string("' frames per buffer."));
	LOG("\tNumber of buffers in this stream: '" + std::to_string(m_Options.numberOfBuffers) + std::string("'"));
	LOG("\tStream callbackFn priority: '" + std::to_string(m_Options.priority) + std::string("'"));

	m_Audio->startStream();

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
