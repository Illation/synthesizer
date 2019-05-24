#include "stdafx.h"
#include "Synthesizer.h"

#include "MidiManager.h"

#include <EtCore/Helper/Time.h>
#include <EtCore/Helper/InputManager.h>

#include <gdk/gdkkeysyms-compat.h>

//============
// Voice
//============

//---------------------------------
// Voice::Voice
//
// Construct a voice from synthesizer parameters
//
Voice::Voice(float const frequency, SynthParameters const& params)
	: m_Frequency(frequency)
	, m_LevelEnvelope(AdsrEnvelope(params.levelEnvelope))
	, m_FilterEnvelope(AdsrEnvelope(params.filterEnvelope))
	, m_FilterEnvelopeAmount(params.filterEnvelopeAmount)
	, m_Filter(Filter(params.filter))
{
	// Create the oscillators
	for (OscillatorParameters const& oscParams : params.oscillators)
	{
		m_Oscillators.emplace_back(Oscillator(frequency, oscParams));
	}
}

//---------------------------------
// Voice::GetSample
//
// Synthesize this voice
//
float Voice::GetSample(double const dt)
{
	// Don't calculate anything if this voice is not active
	if (!m_InputState && m_LevelEnvelope.IsComplete())
	{
		return 0.f;
	}

	// Get the combined value from the oscillators
	float oscillatorOut = 0.f;
	for (Oscillator& oscillator : m_Oscillators)
	{
		if (oscillator.GetLevel() != 0.f)
		{
			oscillatorOut += oscillator.GetLevel() * oscillator.GetSample(dt);
		}
	}

	// Set the filter envelope
	m_Filter.SetEnvelopeValue(m_FilterEnvelopeAmount * m_FilterEnvelope.GetSignal(m_InputState, dt));

	// Filter the signal from the oscillators
	oscillatorOut = m_Filter.GetSignal(oscillatorOut);

	// multiply the signal with the volume of the level envelope
	return m_Velocity * m_LevelEnvelope.GetSignal(m_InputState, dt) * oscillatorOut;
}

//---------------------------------
// Voice::SetInputOn
//
// Activates a note and changes its velocity
//
void Voice::SetInputOn(float const velocity)
{
	m_InputState = true;
	m_Velocity = velocity;
}

//---------------------------------
// Voice::SetInputOff
//
// Makes a note stop playing. Tone may continue for a while depending on envelope.
//
void Voice::SetInputOff()
{
	m_InputState = false;
}

//============
// Synthesizer
//============

//---------------------------------
// Synthesizer::Initialize
//
// Setup our synth and tune our oscillators
//
void Synthesizer::Initialize()
{
	// config data
	m_OutputSettings = &(Config::GetInstance()->GetOutput());

	// Synth parameters
	///////////////////
	m_SynthParameters = SynthParameters();

	// level envelope
	m_SynthParameters.levelEnvelope = AdsrParameters(0.1, 0.05, 0.5, 0.5);

	// filter
	m_SynthParameters.filterEnvelope = AdsrParameters(0.1, 0.05, 0.5, 0.5);
	m_SynthParameters.filter = FilterParams(FilterParams::FilterMode::lowPass, 1.f, 0.f);
	m_SynthParameters.filterEnvelopeAmount = 0.f;

	// Oscillators
	m_SynthParameters.level = 0.5f;
	m_SynthParameters.oscBalance = 0.f;

	OscillatorParameters osc;
	osc.patternType = E_PatternType::Saw;
	m_SynthParameters.oscillators.emplace_back(osc);

	osc.patternType = E_PatternType::Triangle;
	m_SynthParameters.oscillators.emplace_back(osc);

	SetOscillatorBalance();

	// input setup
	//////////////
	MidiManager::GetInstance()->RegisterListener(E_MidiStatus::NoteOn, this);
	MidiManager::GetInstance()->RegisterListener(E_MidiStatus::NoteOff, this);
	MidiManager::GetInstance()->RegisterListener(E_MidiStatus::ControlChange, this);

	// Create a voice for every possible midi note
	for (uint8 note = 0; note < 128; ++note)
	{
		// calculate frequency based on Note number
		float frequency = 440.f * powf(2.f, static_cast<float>(static_cast<int32>(note) - 69) / 12.f);

		m_Voices.emplace_back(note, Voice(frequency, m_SynthParameters));
	}
}

//---------------------------------
// Synthesizer::Update
//
// Update which voices are active, adjust parameters
//
void Synthesizer::Update()
{
	// Map all the keys on our keyboard we want our synthesizer to play - later this could work through MIDI
	std::vector<uint32> keys;
	keys.emplace_back(GDK_a);
	keys.emplace_back(GDK_w);
	keys.emplace_back(GDK_s);
	keys.emplace_back(GDK_e);
	keys.emplace_back(GDK_d);
	keys.emplace_back(GDK_f);
	keys.emplace_back(GDK_t);
	keys.emplace_back(GDK_g);
	keys.emplace_back(GDK_y);
	keys.emplace_back(GDK_h);
	keys.emplace_back(GDK_u);
	keys.emplace_back(GDK_j);
	keys.emplace_back(GDK_k);

	const uint8 offset = 57; // A will be the 57th MIDI note

	// Update voice input states
	for (uint32 key = 0; key < keys.size(); ++key)
	{
		// unsafe for now!
		T_KeyVoicePair& keyVoice = m_Voices[key + offset];

		if (InputManager::GetInstance()->GetKeyState(keys[key]) == E_KeyState::Pressed)
		{
			keyVoice.second.SetInputOn(1.f);
		}

		if (InputManager::GetInstance()->GetKeyState(keys[key]) == E_KeyState::Released)
		{
			keyVoice.second.SetInputOff();
		}
	}

	// change parameters filter parameters
	float changeSpeed = 0.5f;

	// cutoff
	if (InputManager::GetInstance()->GetKeyState(GDK_Up) >= E_KeyState::Down)
	{
		m_SynthParameters.filter.SetCutoff(m_SynthParameters.filter.GetCutoff() + changeSpeed * TIME->DeltaTime());
	}

	if (InputManager::GetInstance()->GetKeyState(GDK_Down) >= E_KeyState::Down)
	{
		m_SynthParameters.filter.SetCutoff(m_SynthParameters.filter.GetCutoff() - changeSpeed * TIME->DeltaTime());
	}

	// resonance
	if (InputManager::GetInstance()->GetKeyState(GDK_Right) >= E_KeyState::Down)
	{
		m_SynthParameters.filter.SetResonance(m_SynthParameters.filter.GetResonance() + changeSpeed * TIME->DeltaTime());
	}

	if (InputManager::GetInstance()->GetKeyState(GDK_Left) >= E_KeyState::Down)
	{
		m_SynthParameters.filter.SetResonance(m_SynthParameters.filter.GetResonance() - changeSpeed * TIME->DeltaTime());
	}

	// poly blep
	if (InputManager::GetInstance()->GetKeyState(GDK_Return) == E_KeyState::Pressed)
	{
		for (OscillatorParameters& osc : m_SynthParameters.oscillators)
		{
			osc.usePolyBlep = !osc.usePolyBlep;
		}
		LOG("PolyBlep " + std::string(m_SynthParameters.oscillators[0].usePolyBlep ? "on" : "off"));
	}
}

//---------------------------------
// Synthesizer::GetSample
//
// Generate a sample for each channel
//
std::vector<float> Synthesizer::GetSample()
{
	// how much time passed since the last sample - static value as this is called in intervals instead of steadily
	double const dt = m_OutputSettings->TimePerSample;

	// Get a sample for every voice
	float voiceOut = 0.f;
	for (T_KeyVoicePair& keyVoice : m_Voices)
	{
		voiceOut += keyVoice.second.GetSample(dt);
	}

	// clamp within allowerd range
	voiceOut = etm::Clamp(voiceOut, 1.f, -1.f);

	// map the result from our voices equally across all channels for now
	std::vector<float> ret;
	for (uint8 channel = 0; channel < m_OutputSettings->Channels; ++channel)
	{
		ret.emplace_back(voiceOut);
	}
	return ret;
}

//---------------------------------
// Synthesizer::OnMidiEvent
//
// Change the state of the synthesizer based on midi events
//
void Synthesizer::OnMidiEvent(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data)
{
	UNUSED(channel);

	// Parse the data
	if (data.size() != 2)
	{ 
		LOG("Synthesizer::OnMidiEvent > Expected two bytes of data!", Warning);
		return;
	}

	uint8 note = data[0];
	float value = static_cast<float>(data[1]) / 127.f;

	// unsafe for now!
	T_KeyVoicePair& keyVoice = m_Voices[note];

	switch (status)
	{
	case E_MidiStatus::NoteOn:
		keyVoice.second.SetInputOn(value);
		break;
	case E_MidiStatus::NoteOff:
		keyVoice.second.SetInputOff();
		break;
	case E_MidiStatus::ControlChange:
		ChangeControl(static_cast<E_Control>(note), value);
		break;
	}
}

//---------------------------------
// Synthesizer::ChangeControl
//
// Change synth parameters based on control values
//
void Synthesizer::ChangeControl(E_Control const control, float const value)
{
	// #todo: Make this configurable
	static const auto getEnvelopeParam = [](float const value) -> float
	{
		return (1.f - value) * 5.f;
	};

	switch (control)
	{
	case E_Control::OscTune:
		SetOscillatorTune(value);
		break;
	case E_Control::OscLevel:
		m_SynthParameters.oscBalance = value;
		SetOscillatorBalance();
		break;
	case E_Control::OscMod1:
		SetOscillatorMode(0, value);
		break;
	case E_Control::OscMod2:
		SetOscillatorMode(1, value);
		break;

	case E_Control::Cutoff:
		m_SynthParameters.filter.SetCutoff(value);
		break;
	case E_Control::Resonance:
		m_SynthParameters.filter.SetResonance(value);
		break;
	case E_Control::EnvAmount:
		m_SynthParameters.filterEnvelopeAmount = value;
		break;

	case E_Control::LevelAttack:
		m_SynthParameters.levelEnvelope.SetAttack(getEnvelopeParam(value));
		break;
	case E_Control::LevelDecay:
		m_SynthParameters.levelEnvelope.SetDecay(getEnvelopeParam(value));
		break;
	case E_Control::LevelSustain:
		m_SynthParameters.levelEnvelope.SetSustain(value);
		break;
	case E_Control::LevelRelease:
		m_SynthParameters.levelEnvelope.SetRelease(getEnvelopeParam(value));
		break;

	case E_Control::FilterAttack:
		m_SynthParameters.filterEnvelope.SetAttack(getEnvelopeParam(value));
		break;
	case E_Control::FilterDecay:
		m_SynthParameters.filterEnvelope.SetDecay(getEnvelopeParam(value));
		break;
	case E_Control::FilterSustain:
		m_SynthParameters.filterEnvelope.SetSustain(value);
		break;
	case E_Control::FilterRelease:
		m_SynthParameters.filterEnvelope.SetRelease(getEnvelopeParam(value));
		break;

	case E_Control::Volume:
		m_SynthParameters.level = value;
		SetOscillatorBalance();
		break;

	default:
		LOG("Unhandled MIDI control change, control: " + std::to_string(static_cast<uint8>(control)) + " : " + std::to_string(value));
	}
}

//---------------------------------
// Synthesizer::SetOscillatorMode
//
// Sets the pattern of an oscillator based on the fraction of its value - assumes value is between 0 and 1
//
void Synthesizer::SetOscillatorMode(uint8 const oscIdx, float const value)
{
	// check safety
	if (oscIdx >= m_SynthParameters.oscillators.size())
	{
		LOG("Synthesizer::SetOscillatorMode > Oscillator index > number of oscillators", Warning);
		return;
	}

	// figure out the pattern we want based on the value
	uint8 patternIndex = static_cast<uint8>(value * static_cast<float>(E_PatternType::COUNT));
	if (patternIndex == static_cast<uint8>(E_PatternType::COUNT)) // this can happen if value == 1.f
	{
		patternIndex--;
	}
	E_PatternType newPattern = static_cast<E_PatternType>(patternIndex);

	// check that we are not already of that pattern type
	if (m_SynthParameters.oscillators[oscIdx].patternType != newPattern)
	{
		m_SynthParameters.oscillators[oscIdx].patternType = newPattern;

		// set the oscillators for every voice to the new type
		for (T_KeyVoicePair& keyVoice : m_Voices)
		{
			keyVoice.second.GetOscillator(oscIdx).SetPattern(newPattern);
		}

		// log the new type
		LOG("Synthesizer::SetOscillatorMode > Oscillator #" + std::to_string(oscIdx) + std::string(" set to: ") + GetPatternName(newPattern));
	}
}

//---------------------------------
// Synthesizer::SetOscillatorBalance
//
// Sets the volume balance for the first two oscillators
//
void Synthesizer::SetOscillatorBalance()
{
	// check safety
	if (m_SynthParameters.oscillators.size() != 2)
	{
		LOG("Synthesizer::SetOscillatorBalance > this function assumes there are two oscillators", Warning);
		return;
	}

	m_SynthParameters.oscillators[0].level = m_SynthParameters.level * (1.f - m_SynthParameters.oscBalance);
	m_SynthParameters.oscillators[1].level = m_SynthParameters.level * m_SynthParameters.oscBalance;
}

//---------------------------------
// Synthesizer::SetOscillatorTune
//
// tunes the second oscillator at intervals in octaves
//
void Synthesizer::SetOscillatorTune(float const value)
{
	// check safety
	if (m_SynthParameters.oscillators.size() < 2)
	{
		LOG("Synthesizer::SetOscillatorTune > this function assumes there are at least two oscillators", Warning);
		return;
	}

	// #todo: Make this configurable
	float const maxOctaveOffset = 3.25f; // adding 0.5f gives us some wiggleroom

	float const octaveOffset = std::floorf(((value * 2.f - 1.f) * maxOctaveOffset) + 0.5f);
	float const newFreqMultiplier = std::powf(2.f, octaveOffset);

	if (newFreqMultiplier != m_SynthParameters.oscillators[1].frequencyMultiplier)
	{
		m_SynthParameters.oscillators[1].frequencyMultiplier = std::powf(2.f, octaveOffset);

		LOG("Synthesizer::SetOscillatorMode > Oscillator #2 octave offset: '" + std::to_string(octaveOffset) + std::string("'"));
	}
}
