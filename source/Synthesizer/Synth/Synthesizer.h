#pragma once
#include <memory>

#include <EtCore/UpdateCycle/Tickable.h>

#include "Config.h"

#include "MidiListener.h"

#include "AdsrEnvelope.h"
#include "Oscillator.h"
#include "Filter.h"

enum class E_Control : uint8
{
	OscTune = 74,
	OscLevel = 71,
	OscMod1 = 5,
	OscMod2 = 84,
	Cutoff = 78,
	Resonance = 76,
	EnvAmount = 77,
	LFO = 10,
	LevelAttack = 73,
	LevelDecay = 75,
	LevelSustain = 72,
	LevelRelease = 91,
	FilterAttack = 92,
	FilterDecay = 93,
	FilterSustain = 94,
	FilterRelease = 95,
	Volume = 7,
	RightWheel = 1,
	Button1 = 0,
	Button2 = 2,
	Button3 = 3,
	Button4 = 4,
	Button5 = 6,
	Button6 = 8,
	Button7 = 9,
	Button8 = 11,
	Master = 65,
	TrackPrev = 109,
	TrackNext = 120,
	PatchPrev = 111,
	PatchNext = 112,
	Restart = 102,
	Forward = 103,
	Backward = 104,
	Stop = 105,
	Play = 106,
	Record = 107,
};

//---------------------------------
// SynthParameters
//
// Parameters that are the same across every voice in the synthesizer
//
struct SynthParameters
{
	std::vector<OscillatorParameters> oscillators;

	float level;
	float oscBalance;

	AdsrParameters levelEnvelope;

	AdsrParameters filterEnvelope;
	float filterEnvelopeAmount;
	FilterParams filter;
};

//---------------------------------
// Voice
//
// Each voice can represent one key on a keyboard in order to create a polyphonic synthesizer
//
class Voice
{
public:
	Voice(float const frequency, SynthParameters const& params);

	float GetSample(double const dt);

	void SetInputOn(float const velocity);
	void SetInputOff();

	Oscillator& GetOscillator(uint8 const index) { return m_Oscillators[index]; }

private:

	// Data
	////////
	std::vector<Oscillator> m_Oscillators; // oscillators and their volumes

	AdsrEnvelope m_LevelEnvelope;

	AdsrEnvelope m_FilterEnvelope;
	float const& m_FilterEnvelopeAmount;
	Filter m_Filter;

	float m_Frequency = 440.f;

	bool m_InputState = false;
	float m_Velocity = 0.f;
};

//---------------------------------
// Synthesizer
//
// Generates sounds
//
class Synthesizer final : public I_MidiListener, public I_Tickable
{
public:
	Synthesizer() : I_Tickable(0) {}

	void Initialize();
	void OnTick() override;
	
	std::vector<float> GetSample();

	SynthParameters const& GetParameters() const { return m_SynthParameters; }

protected:
	void OnMidiEvent(E_MidiStatus const status, uint8 const channel, std::vector<uint8> const& data) override;

private:
	typedef std::pair<uint8, Voice> T_KeyVoicePair;

	void ChangeControl(E_Control const control, float const value);

	void SetOscillatorMode(uint8 const oscIdx, float const value);
	void SetOscillatorBalance();
	void SetOscillatorTune(float const value); // tunes the second oscillator at intervals

	// Data
	////////
	SynthParameters m_SynthParameters;

	std::vector<T_KeyVoicePair> m_Voices;

	Config::Settings::Output const* m_OutputSettings;
};
