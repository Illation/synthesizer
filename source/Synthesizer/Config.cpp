#include "stdafx.h"
#include "Config.h"

#include <EtCore/Reflection/Serialization.h>

#include <rttr/registration>

// reflection
RTTR_REGISTRATION
{
	using namespace rttr;

	registration::class_<Config::UserDirPointer>("dir pointer")
		.property("user dir path", &Config::UserDirPointer::m_UserDirPath)
		;

	registration::class_<Config::Settings::Output>("output")
		.constructor<>()
		(
		)
		.method("DeriveSettings", &Config::Settings::Output::DeriveSettings)
		(
		)
		.property("sample rate", &Config::Settings::Output::SampleRate)
		.property("channels", &Config::Settings::Output::Channels)
		.property("frames per buffer", &Config::Settings::Output::FramesPerBuffer)
		.property("api id", &Config::Settings::Output::ApiId)
		.property("device id", &Config::Settings::Output::DeviceId)
		;

	registration::class_<Config::Settings::Midi>("midi")
		.constructor<>()
		(
		)
		.property("device id", &Config::Settings::Midi::DeviceId)
		;

	registration::class_<Config::Settings>("settings")
		.constructor<>()
		(
		)
		.property("output", &Config::Settings::m_Output)
		.property("midi", &Config::Settings::m_Midi)
		;
}

//---------------------------------
// Config::Initialize
//
// Load from JSON and generate derived data
//
void Config::Initialize()
{
	// try deserializing the user directory path
	UserDirPointer userDir;
	if (serialization::DeserializeFromFile(s_PointerPath, userDir))
	{
		m_UserDir = userDir;
	}
	else
	{
		LOG("Config::Initialize > unable to deserialize pointer to user directory, using default user directory: '" 
			+ std::string(s_DefaultUserDir) + std::string("'"), Warning);
	}

	// try deserializing settings
	Settings settings;
	if (serialization::DeserializeFromFile(GetUserDirPath() + s_ConfigFileRelativePath, settings))
	{
		m_Settings = settings;
	}
	else
	{
		LOG("Config::Initialize > unable to deserialize config file to settings, using defaults", Warning);
	}

	// derive settings regardless of whether they where loaded or default
	m_Settings.m_Output.DeriveSettings();
}

//---------------------------------
// Config::Initialize
//
// Save the configuration to disk
//
void Config::Save()
{
	if (!serialization::SerializeToFile(GetUserDirPath() + s_ConfigFileRelativePath, m_Settings))
	{
		LOG("Config::Save > unable to serialize settings to config file", Warning);
	}
}

//-------------------------------------------
// Config::Settings::Output::DeriveSettings
//
// Derive some settings from those that where loaded from json
//
void Config::Settings::Output::DeriveSettings()
{
	BitRate = SampleRate * Channels;
	TimePerBuffer = static_cast<double>(FramesPerBuffer) / static_cast<double>(SampleRate);
	TimePerSample = 1.0 / static_cast<double>(SampleRate);
}
