#pragma once
#include <Helper/Singleton.h>

#include <rttr/type>

//---------------------------------
// Config
//
// Config data for this project
//
class Config : public Singleton<Config>
{
	// Definitions
	//-----------------
public:

	static constexpr char s_PointerPath[] = "../config/userDirPointer.json";
	static constexpr char s_DefaultUserDir[] = "../config/";
	static constexpr char s_ConfigFileRelativePath[] = "./config.json";

	//---------------------------------
	// Config::UserDirPointer
	//
	// figure out where our user data lives
	//
	struct UserDirPointer
	{
		std::string m_UserDirPath = s_DefaultUserDir;

		RTTR_ENABLE()
	};

	//---------------------------------
	// Config::Settings
	//
	// Configuration settings
	//
	class Settings
	{
	public:
		//---------------------------------
		// Config::Settings::Output
		//
		// Configuration for our rt audio output stream
		//
		struct Output
		{
			void DeriveSettings();
			// Settings loaded from JSON
			///////////////////////////
			// We set default values incase loading goes wrong
			uint32 SampleRate = 22050;
			uint32 Channels = 2;

			// the number of sample frames that PortAudio will request from the callback. Many apps may want to use paFramesPerBufferUnspecified, which
			// tells PortAudio to pick the best, possibly changing, buffer size
			uint32 FramesPerBuffer = 256;

			std::string ApiId;
			int32 DeviceId = -1;

			// Derived settings after loading data
			//////////////////////////////////////
			uint32 BitRate = 0;
			double TimePerBuffer = 0.0;
			double TimePerSample = 0.0;

			RTTR_ENABLE()
		};
		//---------------------------------
		// Config::Settings::Midi
		//
		// Configuration for Midi devices
		//
		struct Midi
		{
			int32 DeviceId = -1;

			RTTR_ENABLE()
		};

		Output m_Output;
		Midi m_Midi;

		RTTR_ENABLE()
	};

private:
	friend class Singleton<Config>;


	// Default constructor and destructor
	//-----------------
	Config() = default;
	virtual ~Config() = default;


	// Public interface
	//-----------------
public:
	std::string const& GetUserDirPath() const { return m_UserDir.m_UserDirPath; }

	Settings::Output const& GetOutput() const { return m_Settings.m_Output; }
	Settings::Output & GetOutput() { return m_Settings.m_Output; }

	Settings::Midi const& GetMidi() const { return m_Settings.m_Midi; }
	Settings::Midi & GetMidi() { return m_Settings.m_Midi; }

	// initialization
	void Initialize();
	void Save();

	// DATA
	///////
private:

	UserDirPointer m_UserDir;

	Settings m_Settings = Settings();
};

