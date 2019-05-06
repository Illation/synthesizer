#pragma once
#include "MidiListener.h"


//---------------------------------
// MidiManager
//
// Recieves, decodes and passes on MIDI events
//
class MidiManager : public Singleton<MidiManager>
{
public:

	void HandleMidiMessage(double const dt, std::vector<uint8> const& message);

	void RegisterListener(E_MidiStatus const status, I_MidiListener* const listener);

private:
	friend class Singleton<MidiManager>;
	friend class Framework;

	// Defualt constructor and destructor
	MidiManager() : m_RegisteredListeners() {}
	virtual ~MidiManager();

	void UnregisterAllListeners();
	void Notify(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data);

	typedef std::vector<I_MidiListener*> T_ListenerList;
	typedef std::pair<E_MidiStatus, T_ListenerList> T_EventListenerPair;

	// DATA
	///////
	std::vector<T_EventListenerPair> m_RegisteredListeners;
};

