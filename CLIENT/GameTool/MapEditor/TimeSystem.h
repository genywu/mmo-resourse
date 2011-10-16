#pragma once

class TimeSystem
{
	DECLARE_SINGLETON(TimeSystem);
	float m_fSecond;

	DWORD m_dwTimeLast;

	float m_fTimeRatio;

public:
	TimeSystem(void);
	~TimeSystem(void);

	void Update();
	float GetTime();
};
