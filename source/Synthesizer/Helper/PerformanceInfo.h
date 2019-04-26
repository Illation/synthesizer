#pragma once
#include "Singleton.h"

class PerformanceInfo : public Singleton<PerformanceInfo>
{
public:
	PerformanceInfo();
	virtual ~PerformanceInfo();

	int32 GetRegularFPS() { return m_RegularFPS; }
	float GetFrameMS() { return m_FrameMS; }

private:
	friend class Framework; //should init and destroy singleton

	void Update();
	void StartFrameTimer();

	float m_RegFPSTimer = 10;
	int32 m_RegularFPS = 0;

	float m_FrameMS = 0;
	float m_FrameMSStart = 0;
};