#pragma once
#include "MoveShape.h"

class CEffect :
	public CMoveShape
{

public:
	CEffect(void);
	CEffect(const CEffect &srcEffect);
	virtual ~CEffect(void);

	long GetEffectID()					{ return m_lEffectID; }
	void SetEffectID(long l)				{ m_lEffectID = l; }

	void AI();
	bool Display(void);

	void SetLoop(bool b)	{m_bLoop=b;}
	void Set3DRotEnable(bool b)	{m_b3DRot=b;}
	bool IsLoop()			{return m_bLoop;}

	void SetHost(long lType, long lGraphicsType, long lGraphicsID, const CGUID& ID, eLOCATOR eLoc = LOCATOR_FOOTCENTER)
	{
		m_lHType = lType;
		m_lHostType = lGraphicsType;
		m_lHostGraphicsID = lGraphicsID;
		m_HostID = ID;
		m_eHostLocator = eLoc;
	};

	long GetHType()			{return m_lHType;}
	long GetHostType()		{return m_lHostType;}
	long GetHostGraphicsID(){return m_lHostGraphicsID;}
	const CGUID& GetHostID()		{return m_HostID;}
	eLOCATOR GetHostLocator()	{return m_eHostLocator;}
	void SetHeightAdjust(float fHeightAdjust)		{ m_fHeightAdjust = fHeightAdjust; }
	void  SetLight(DWORD dwColor,DWORD dwType,float fRadius,DWORD dwStartTime,DWORD dwLength);
	BOOL  IsLightAvailable() { return m_bLightAvailable ;}
	float GetLightRadius();
	DWORD GetLightType()   { return m_dwLightType;}
	DWORD GetLightColor();
	bool	IsTask(){return m_bTask;}
	void	SetTask(bool b){m_bTask=b;}

	DWORD GetLightFadeStartTime() {return m_dwLightFadeStartTime;}
	DWORD GetLightFadeLength() {return m_dwLightFadeLength;}
	DWORD GetLightStartTime() {return m_dwLightStartTime;}
	void  SetUsingSelfAnim(bool b){IsUsingSelfAnim = b;}

		
private:
	long	m_lEffectID;
	bool	m_bLoop;			// 是否循环播放
	bool    m_b3DRot;
	
	long	m_lHType;			// 所有者类型
	long	m_lHostType;		// 所有者的变身类型
	long	m_lHostGraphicsID;	// 所有者的图形id
	CGUID	m_HostID;			// 所有者的ID
	eLOCATOR	m_eHostLocator;	// 对应所有者的定位点
	float	m_fHeightAdjust;	// 播放特效的高度修正值

	DWORD   m_dwLightStartTime;

	DWORD   m_dwLightFadeStartTime;
	DWORD	m_dwLightFadeLength;	
	DWORD   m_dwLightColor;
	DWORD	m_dwLightType;
	DWORD   m_bLightAvailable;
	float   m_fLightRadius;
	bool	m_bTask;
	bool	IsUsingSelfAnim;		//特效是否试用自定义骨骼国画
};
