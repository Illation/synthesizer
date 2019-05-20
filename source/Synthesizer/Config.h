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
public:
	//---------------------------------
	// Config::OutputSettings
	//
	// Configuration for our portaudio output stream
	//
	struct OutputSettings
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

		// Derived settings after loading data
		//////////////////////////////////////
		uint32 BitRate = 0;
		double TimePerBuffer = 0.0;
		double TimePerSample = 0.0;

		RTTR_ENABLE()
	};

	OutputSettings const& GetOutput() const { return m_Output; }

	// initialization
	void Initialize();

private:
	friend class Singleton<Config>;

	// Defualt constructor and destructor
	Config() = default;
	virtual ~Config() = default;

	// DATA
	///////

	OutputSettings m_Output = OutputSettings();
};

