#pragma once

#include "MidiListener.h"
#include <Vendor/RtMidi.h>


//---------------------------------
// MidiManager
//
// Recieves, decodes and passes on MIDI events
//
class MidiManager : public Singleton<MidiManager>
{
public:
	// typedefs and declarations
	//------------------
	typedef std::pair<uint32, std::string> T_DeviceIdNamePair;

	typedef std::vector<I_MidiListener*> T_ListenerList;
	typedef std::pair<E_MidiStatus, T_ListenerList> T_EventListenerPair;

	static constexpr int32 s_DeselectedPort = -1;

	friend class Singleton<MidiManager>;
	friend class Framework;

private:
	// Init / deInit
	//--------------------------------
	MidiManager() : m_RegisteredListeners() {}
	virtual ~MidiManager();

	bool InitializeMidi();
	void TerminateMidi();

	bool CreateMidiInput();

	// device setting
	//-------------------
public:
	void GetAllPossibleDevices(std::vector<T_DeviceIdNamePair>& deviceIdNamePairs) const;
	int32 GetActiveDevice() const { return m_ActivePort; }
	void SetActiveDevice(int32 const deviceId);

	// events
	//-----------------------
	void RegisterListener(E_MidiStatus const status, I_MidiListener* const listener);

private:
	void HandleMidiMessage(double const dt, std::vector<uint8> const& message);

	void UnregisterAllListeners();
	void Notify(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data);

	// callback
	//------------------
	static void OnRtMidiError(RtMidiError::Type type, std::string const&errorText, void* userData);
	static void OnMidiCallback(double dt, std::vector<uint8>* message, void* userData);


	// DATA
	///////
private:
	std::vector<T_EventListenerPair> m_RegisteredListeners;

	// MIDI input
	RtMidiIn *m_MidiInput;

	int32 m_ActivePort = s_DeselectedPort;
};

