#include "stdafx.h"
#include "MidiManager.h"


// Init / deInit
//////////////////////

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
// MidiManager::InitializeMidi
//
// Initialize default MIDI device
//
bool MidiManager::InitializeMidi()
{
	if (!CreateMidiInput())
	{
		return false;
	}

	LOG("RtMidi version: " + m_MidiInput->getVersion());

	// Check inputs.
	uint32 numPorts = m_MidiInput->getPortCount();
	if (numPorts > 0)
	{
		// yay we have a midi device

		// log our available ports
		LOG("There are '" + std::to_string(numPorts) + std::string("' MIDI input sources available."));
		for (uint32 i = 0; i < numPorts; i++) 
		{
			LOG("\t Input Port #" + std::to_string(i) + std::string(": '") + m_MidiInput->getPortName(i) + std::string("'"));
		}

		// open the port we want
		m_ActivePort = 0;
		m_MidiInput->openPort(static_cast<uint32>(m_ActivePort));

		m_MidiInput->setCallback(&MidiManager::OnMidiCallback);

		// Don't ignore sysex, timing, or active sensing messages.
		m_MidiInput->ignoreTypes(false, false, false);
	}
	else
	{
		LOG("No MIDI devices found, you can use your keyboard to play notes.");
		m_ActivePort = s_DeselectedPort;
	}

	LOG("");

	return true;
}

//---------------------------------
// MidiManager::TerminateMidi
//
// Close MIDI connection
//
void MidiManager::TerminateMidi()
{
	if (m_ActivePort != s_DeselectedPort)
	{
		m_MidiInput->closePort();
		SafeDelete(m_MidiInput);

		m_ActivePort = s_DeselectedPort;
	}
}

//---------------------------------
// MidiManager::CreateMidiInput
//
// Create the RtMidi object and set its error callback
//
bool MidiManager::CreateMidiInput()
{
	// make an input device
	try 
	{
		m_MidiInput = new RtMidiIn();
	}
	catch (RtMidiError &error) 
	{
		OnRtMidiError(error.getType(), error.getMessage(), nullptr);
		return false;
	}

	// Log error messages
	m_MidiInput->setErrorCallback(&MidiManager::OnRtMidiError);

	return true;
}

// Device settings
///////////////////

//---------------------------------
// MidiManager::GetAllPossibleDevices
//
// List all connected midi devices
//
void MidiManager::GetAllPossibleDevices(std::vector<T_DeviceIdNamePair>& deviceIdNamePairs) const
{
	// check pointer
	if (m_MidiInput == nullptr)
	{
		LOG("MidiManager::GetAllPossibleDevices > Couldn't list devices because no midi object exists", LogLevel::Warning);
		return;
	}

	// list devices - there may not be any
	uint32 numPorts = m_MidiInput->getPortCount();
	for (uint32 i = 0; i < numPorts; i++)
	{
		deviceIdNamePairs.emplace_back(i, m_MidiInput->getPortName(i));
	}
}

//---------------------------------
// MidiManager::SetActiveDevice
//
// Set which MIDI device we want to use for input
//
void MidiManager::SetActiveDevice(int32 const deviceId)
{
	if (m_MidiInput == nullptr)
	{
		if (!CreateMidiInput())
		{
			return;
		}
		// Don't ignore sysex, timing, or active sensing messages.
		m_MidiInput->ignoreTypes(false, false, false);
	}

	uint32 numPorts = m_MidiInput->getPortCount();
	if (static_cast<int32>(numPorts) <= deviceId)
	{
		LOG("MidiManager::SetActiveDevice > selected device id not available, id #" + std::to_string(deviceId), LogLevel::Warning);
		return;
	}

	if (m_ActivePort != s_DeselectedPort)
	{
		m_MidiInput->closePort();
	}

	// open the port we want
	m_ActivePort = deviceId;
	m_MidiInput->openPort(static_cast<uint32>(m_ActivePort));
}

// Events
///////////////////

//---------------------------------
// MidiManager::HandleMidiMessage
//
// Recieve MIDI messages, dispatch to listeners
//
void MidiManager::HandleMidiMessage(double const dt, std::vector<uint8> const& message)
{
	UNUSED(dt);

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

// Callbacks
/////////////////

//---------------------------------
// MidiManager::OnRtMidiError
//
// When RtMidi detects an error, log it
//
void MidiManager::OnRtMidiError(RtMidiError::Type type, std::string const&errorText, void* userData)
{
	UNUSED(userData);

	LogLevel logLevel = LogLevel::Error;
	switch (type)
	{
	case RtMidiError::WARNING:
	case RtMidiError::DEBUG_WARNING:
	case RtMidiError::UNSPECIFIED:
	case RtMidiError::NO_DEVICES_FOUND:
		logLevel = LogLevel::Warning;
	}

	LOG(errorText, logLevel);
}

//---------------------------------
// MidiManager::OnMidiCallback
//
// RtMidi Callback function
//
void MidiManager::OnMidiCallback(double dt, std::vector<uint8>* message, void* userData)
{
	UNUSED(userData);

	if (message != nullptr)
	{
		MidiManager::GetInstance()->HandleMidiMessage(dt, *message);
	}
}
