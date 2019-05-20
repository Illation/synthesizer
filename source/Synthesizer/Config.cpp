#include "stdafx.h"
#include "Config.h"

#include <Reflection/Serialization.h>
#include <rttr/registration>

// reflection
RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Config::OutputSettings>("outputSettings")
		.constructor<>()
		(
		)
		.method("DeriveSettings", &Config::OutputSettings::DeriveSettings)
		(
		)
		// register directly a member object pointer; mark it as 'private'
		.property("sample rate", &Config::OutputSettings::SampleRate)
		.property("channels", &Config::OutputSettings::Channels)
		.property("frames per buffer", &Config::OutputSettings::FramesPerBuffer)
		;
}

//---------------------------------
// Config::Initialize
//
// Load from JSON and generate derived data
//
void Config::Initialize()
{
	rttr::type t = rttr::type::get<Config::OutputSettings>();

	// try deserializing
	OutputSettings output;
	if (serialization::DeserializeFromFile("../config/config.json", output))
	{
		m_Output = output;
	}
	else
	{
		LOG("Config::LoadFromJSON > unable to deserialize config file to output settings, using defaults", Warning);
	}

	// derive settings regardless of whether they where loaded or default
	m_Output.DeriveSettings();
}

//-------------------------------------------
// Config::OutputSettings::DeriveSettings
//
// Derive some settings from those that where loaded from json
//
void Config::OutputSettings::DeriveSettings()
{
	BitRate = SampleRate * Channels;
	TimePerBuffer = static_cast<double>(FramesPerBuffer) / static_cast<double>(SampleRate);
	TimePerSample = 1.0 / static_cast<double>(SampleRate);
}
