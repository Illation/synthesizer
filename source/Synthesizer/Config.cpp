#include "stdafx.h"
#include "Config.h"

#include <FileSystem/Entry.h>
#include <FileSystem/JSONparser.h>
#include <FileSystem/FileUtil.h>
#include <FileSystem/JSONdom.h>

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

	LoadFromJSON();
	m_Output.DeriveSettings();
}

//---------------------------------
// Config::LoadFromJSON
//
// Load the config data from a JSON file
//
void Config::LoadFromJSON()
{
	// Open the json file
	File* jsonFile = new File("../config/config.json", nullptr);
	if (!jsonFile->Open(FILE_ACCESS_MODE::Read))
	{
		LOG("Config::LoadFromJSON > unable to open file, using defaults", Warning);
		return;
	}

	// Read the file into a json parser
	JSON::Parser parser = JSON::Parser(FileUtil::AsText(jsonFile->Read()));

	// We can now close the json file again
	delete jsonFile;
	jsonFile = nullptr;

	// if we don't have a root object parsing json was unsuccesful
	JSON::Object* root = parser.GetRoot();
	if (!root)
	{
		LOG("Config::LoadFromJSON > unable to read config json, using defaults", Warning);
		return;
	}

	OutputSettings output;
	if (serialization::DeserializeFromJson(root, output))
	{
		m_Output = output;
	}
	else
	{
		LOG("Config::LoadFromJSON > unable to deserialize json to output settings, using defaults", Warning);
	}
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
