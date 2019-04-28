#include "stdafx.h"
#include "Config.h"

#include <FileSystem/Entry.h>
#include <FileSystem/JSONparser.h>
#include <FileSystem/FileUtil.h>
#include <FileSystem/JSONdom.h>

//---------------------------------
// Config::Initialize
//
// Load from JSON and generate derived data
//
void Config::Initialize()
{
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
	File* jsonFile = new File("./config.json", nullptr);
	if (!jsonFile->Open(FILE_ACCESS_MODE::Read))
		return;

	// Read the file into a json parser
	JSON::Parser parser = JSON::Parser(FileUtil::AsText(jsonFile->Read()));

	// We can now close the json file again
	delete jsonFile;
	jsonFile = nullptr;

	// if we don't have a root object parsing json was unsuccesful
	JSON::Object* root = parser.GetRoot();
	if (!root)
	{
		LOG("unable to read config json", Warning);
		return;
	}

	// read the output settings
	JSON::Object* outputSettings = (*root)["outputSettings"]->obj();
	if (outputSettings)
	{
		JSON::ApplyNumValue(outputSettings, m_Output.SampleRate, "sample rate");
		JSON::ApplyNumValue(outputSettings, m_Output.Channels, "channels");
		JSON::ApplyNumValue(outputSettings, m_Output.FramesPerBuffer, "frames per buffer");
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
