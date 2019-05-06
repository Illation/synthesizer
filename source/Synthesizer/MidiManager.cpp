#include "stdafx.h"
#include "MidiManager.h"


//---------------------------------
// MidiManager::HandleMidiMessage
//
// Recieve MIDI messages, dispatch to listeners
//
void MidiManager::HandleMidiMessage(double const dt, std::vector<uint8> const& message)
{
	// no need to handle empty messages
	if (message.size() == 0)
	{
		return;
	}

	//LOG("MIDI timestamp '" + std::to_string(dt) + std::string("'"));

	// Check the first bit to make sure we have a status
	if (!(message[0] >> 7 & 1))
	{
		LOG("MidiManager::t HandleMidiMessage > First byte was not a status!", Warning);
		return;
	}

	// right shift by 4 bits to convert into the number space of the status enum
	E_MidiStatus status = static_cast<E_MidiStatus>(message[0] >> 4); 
	// the less significant 4 bits are the channel
	uint8 channel = message[0] & 0x0F; 
	// the rest of the message is data
	std::vector<uint8> data(message.cbegin() + 1, message.cend());

	// Notify our listeners
	Notify(status, channel, data);
}

//---------------------------------
// MidiManager::RegisterListener
//
// Register an I_MidiListener - it will now recieve events of type 'status'
//
void MidiManager::RegisterListener(E_MidiStatus const status, I_MidiListener* const listener)
{
	// first try to find an event of type status
	auto foundPairIt = std::find_if(m_RegisteredListeners.begin(), m_RegisteredListeners.end(), [status](T_EventListenerPair const& element)
	{
		return element.first == status;
	});

	// if we didn't find one, create one
	if (foundPairIt == m_RegisteredListeners.cend())
	{
		m_RegisteredListeners.emplace_back(status, T_ListenerList());
		foundPairIt = std::prev(m_RegisteredListeners.end());
	}
	else if (std::find(foundPairIt->second.cbegin(), foundPairIt->second.cend(), listener) != foundPairIt->second.cend())
	{
		// if we didn't create one, check we didn't already register this listener
		LOG("MidiManager::RegisterListener > listener already registered to this status", Warning);
		return;
	}

	// add our listener
	foundPairIt->second.emplace_back(listener);
}

//---------------------------------
// MidiManager::~MidiManager
//
// MIDI manager destructor
//
MidiManager::~MidiManager()
{
	UnregisterAllListeners();
}

//---------------------------------
// MidiManager::UnregisterAllListeners
//
// Unregister all I_MidiListeners at once
//
void MidiManager::UnregisterAllListeners()
{
	m_RegisteredListeners.clear();
}

//---------------------------------
// MidiManager::Notify
//
// Notify all listeners listening to 'status'
//
void MidiManager::Notify(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data)
{
	// first try to find an event of type status
	auto foundPairIt = std::find_if(m_RegisteredListeners.begin(), m_RegisteredListeners.end(), [status](T_EventListenerPair const& element)
	{
		return element.first == status;
	});

	// if we found one, notify all the listeners
	if (foundPairIt != m_RegisteredListeners.cend())
	{
		for (I_MidiListener* const listener : foundPairIt->second)
		{
			listener->OnMidiEvent(status, channel, data);
		}

		return;
	}

	// by default we log unhandled MIDI messages

	// make a string for the status
	std::string statusString;
	switch (status)
	{
	case E_MidiStatus::NoteOff:
		statusString = "Note Off";
		break;
	case E_MidiStatus::NoteOn:
		statusString = "Note On";
		break;
	case E_MidiStatus::KeyPressure:
		statusString = "Key pressure (aftertouch)";
		break;
	case E_MidiStatus::ControlChange:
		statusString = "Control Change / Channel Mode";
		break;
	case E_MidiStatus::ProgramChange:
		statusString = "Program Change";
		break;
	case E_MidiStatus::ChannelPressure:
		statusString = "Channel pressure (aftertouch)";
		break;
	case E_MidiStatus::PitchBendChange:
		statusString = "Pitch bend change";
		break;
	case E_MidiStatus::System:
		statusString = "System";
		break;
	}

	LOG("Unhandled MIDI event on channel #" + std::to_string(channel) + " > " + statusString);

	// prepare a string to log in one line
	std::string dataMessage;
	for (uint32 i = 0; i < data.size(); ++i)
	{
		dataMessage += std::string("[") + std::to_string(data[i]) + std::string("]");
		if (i < data.size() - 1)
		{
			dataMessage += std::string(", ");
		}
	}

	LOG("\t Data: " + dataMessage);
}
