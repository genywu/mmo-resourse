#pragma once

#pragma once

#include <windows.h>

class CPlayerTracker
{
	BOOL  m_bShocked;
	float m_fShockMaxRatio;
	float m_fShcokrOffset;

	DWORD m_dwShockStartTime;
	DWORD m_dwShockDelayTime;
	DWORD m_dwShockTimeLength;

	//水平视角
	int m_iHoriAngle;

	//垂直视角
	int   m_iMinVertAngle;
	int   m_iMaxVertAngle;
	int   m_iVertAngle;

	//观察距离
	float m_fMinDistance;
	float m_fMaxDistance;
	float m_fDistance;

	//
	int m_nScorll;	// -1:向左转 0:不动 1:向右转

	bool	m_bKeyboardAble;		// 键盘有效
	bool	m_bMouseWheelAble;		// 鼠标滚轮有效
	BOOL	m_bHRotataion;

	static float s_fDistance;		// 记录玩家的视角距离

	// 鼠标右键按下,记录此坐标
	POINT	m_ptOldMouse;
	// 记录原度数
	int m_iOldHoriAngle;
	// 最初的度数
	int m_iSourceHoriAngle;
	//是否平滑
	BOOL m_bsmooth;
	BOOL m_brepos;

public:
	CPlayerTracker(void);
	~CPlayerTracker(void);

	void Update(void);
	void Run(void);

	void SetShock(float fMaxRatio,DWORD dwDelayTime,DWORD dwTimeLength);

	BOOL IsShock() const { return m_bShocked;}
	//ADD by 彭谦
	BOOL IsSmooth()const {return m_bsmooth;}
	void SetSmooth(BOOL bsmooth){m_bsmooth = bsmooth;}
	//BOOL Isrepos()const {return m_brepos;}
	//void Setrepos(BOOL brepos){m_brepos = brepos;}
	//ADD by 彭谦
	float GetSockOffset() const { return m_fShcokrOffset;}

	void GetPosition(D3DXVECTOR3 &vPosition);

	float GetHoriAngle() { return (float)m_iHoriAngle*(3.1415926f/180.0f); }

	void SetKeyboardAble(bool b)	{m_bKeyboardAble=b;}
	void SetMouseWheelAble(bool b)	{m_bMouseWheelAble=b;}
	void SetHRotation(BOOL bValue)  {m_bHRotataion = bValue;}

	float GetCameraPosModify()	{return (m_fDistance-m_fMinDistance)/(m_fMaxDistance-m_fMinDistance);}
};

