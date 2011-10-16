#pragma once
#include "baseobject.h"

class CShape :
	public CBaseObject
{
public:
	CShape(void);
	virtual ~CShape(void);

	// 状态
	enum eSTATE
	{
		STATE_PEACE,			// 和平状态
		STATE_FIGHT,			// 战斗状态
		STATE_DIED,				// 死亡状态
		STATE_FEIGNDEATH,		// 假死状态
		STATE_HANGUP,			// 挂起
		STATE_DELET,			// 删除
	};

	// 动作
	enum eACTION
	{
		ACT_STAND,				// 站(建筑静止)
		ACT_IDLE,				// 站立idle
		ACT_WALK,				// 走
		ACT_RUN,				// 跑
		ACT_ATTACK,				// 攻擊
		ACT_BEATTACK,			// 被攻擊
		ACT_DIED,				// 死亡(建筑击毁)
		ACT_OPEN,				// 城门打开

		ACT_PERFORM = 21,		// 施法
	};

	// 方向
	enum eDIR
	{
		DIR_UP = 0,
		DIR_RIGHTUP,
		DIR_RIGHT,
		DIR_RIGHTDOWN,
		DIR_DOWN,
		DIR_LEFTDOWN,
		DIR_LEFT,
		DIR_LEFTUP,
	};

///////////////////////////////////////////////////////////////
// 属性
///////////////////////////////////////////////////////////////
protected:
	eRgnType m_CurRgnType;// 所在的场景的类型:副本/普通场景
	long m_lRegionID;	// 所在的场景
	CGUID m_RegionID;    // 所在的场景GUID
	float m_fPosX;		// 位置X
	float m_fPosY;		// 位置Y
	long m_lDir;		// 方向
	long m_lPos;		// 當前幀
	float m_fSpeed;		// 速度
	WORD m_wState;		// 状态
	WORD m_wAction;		// 动作

public:
	void SetCurRgnType(eRgnType type) { m_CurRgnType = type; }
	eRgnType GetCurRgnType(void)	{ return m_CurRgnType; }

	virtual long GetRegionID()		{ return m_lRegionID; }
	virtual void SetRegionID(long l);
	virtual const CGUID& GetRegionExID()		{ return m_RegionID; }
	virtual void SetRegionExID(const CGUID& l){ m_RegionID = l; }
	virtual void SetPosX(float l)	{ m_fPosX = l; }
	virtual void SetPosY(float l)	{ m_fPosY = l;}
	virtual long GetDir()			{ return m_lDir; }
	virtual void SetDir(long l)		{ if(l>-1 && l<8) m_lDir = l; }
	virtual long GetPos()			{ return m_lPos; }
	virtual void SetPos(long l)		{ m_lPos = l; }
	virtual float GetSpeed()		{ return m_fSpeed; }
	virtual void SetSpeed(float l)	{ m_fSpeed = l; }
	virtual WORD GetState()			{ return m_wState; }
	virtual void SetState(WORD l)	{ m_wState = l; }
	virtual WORD GetAction()		{ return m_wAction; }
	virtual void SetAction(WORD l)	{ m_wAction = l; }

	float GetPosX();
	float GetPosY();
	virtual void SetPosXY(float x, float y);
	virtual BYTE GetFigure()	{return 0;}	// 体形
	void SetBlock(int x, int y, unsigned short block);

	virtual bool AddShapeToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	virtual bool DecordShapeFromByteArray(BYTE* pByte, long& pos);	// 解码

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);	
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	

	virtual void AI(void);

///////////////////////////////////////////////////////////////
// 公共函数
///////////////////////////////////////////////////////////////
public:
	void GetDirPos(long dir, float& x, float& y);	// 获取dir方向的坐标
	long GetFaceDir();							// 获取面对的坐标
};
