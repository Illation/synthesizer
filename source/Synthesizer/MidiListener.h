#pragma once


//---------------------------------
// E_MidiStatus
//
// Distinguish between types of messages we can get - there are special cases for control change
//
enum class E_MidiStatus : uint8 
{
	NoteOff			= 0x8,
	NoteOn			= 0x9,
	KeyPressure		= 0xA,
	ControlChange   = 0xB,
	ProgramChange	= 0xC,
	ChannelPressure = 0xD,
	PitchBendChange = 0xE,
	System			= 0xF
};

//---------------------------------
// I_MidiListener
//
// Interface that allows listening to the MidiManager
//
class I_MidiListener
{
public:
	virtual ~I_MidiListener() {}

	virtual void OnMidiEvent(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data) = 0;
};

