#pragma once

class Timer
{
	DWORD m_dwTime;
public:
	Timer(void);
	~Timer(void);

	DWORD GetTime() { return m_dwTime;}
	void  UpdateTime(DWORD dwTimeElapse) { m_dwTime += dwTimeElapse;}
};
