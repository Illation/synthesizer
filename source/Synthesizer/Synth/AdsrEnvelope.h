#pragma once

//---------------------------------
// AdsrParameters
//
// parameters to set up an Attack Decay Sustain Release Envelope
//
//    /|\ ___
//   / | |   |\
//    A D  S  R
//
struct AdsrParameters
{
	AdsrParameters();
	AdsrParameters(double const a, double const d, double const s, double const r);

	// Setters
	void SetAttack(double const val);
	void SetDecay(double const val);
	void SetSustain(double const val);
	void SetRelease(double const val);

	// Getters
	double GetAttack() const { return attack; }
	double GetDecay() const { return decay; }
	double GetSustain() const { return sustain; }
	double GetRelease() const { return release; }

	double GetSustainTime() const { return sustainTime; }
	double GetReleaseTime() const { return releaseTime; }
	double GetSustainInv() const { return sustainInv; }

private:

	void CalculateSustainTime();
	void CalculateReleaseTime();
	void CalculateSustainInv();
	void CalculateRuntimeParams();

	// Data
	/////////

	double attack = 0.f;		// time to peak in seconds
	double decay = 0.0;		// time to sustain level in seconds
	double sustain = 1.f;	// sustain level, between 0 and 1
	double release = 0.f;	// time to release in seconds

	// derived runtime parameters
	double sustainTime; 
	double releaseTime;
	double sustainInv;
};

//---------------------------------
// AdsrEnvelope
//
// object that reflects the current state of an Envelope with ADSR parameters, returns a signal strength over time
//
class AdsrEnvelope
{
public:
	AdsrEnvelope(AdsrParameters const& params) : m_Params(params) {}

	float GetSignal(bool const input, double const dt);
	bool IsComplete() const { return m_Time < 0.0; }

private:
	enum class EnvelopeState
	{
		Attack,
		Decay,
		Sustain,
		Release
	};

	// Data
	//////////
	AdsrParameters const& m_Params;

	EnvelopeState m_State = EnvelopeState::Release;

	double m_Level = 0.f;
	double m_Time = -1.0;
};

