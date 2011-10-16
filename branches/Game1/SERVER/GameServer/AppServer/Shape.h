#pragma once
#include "baseobject.h"

extern long _dir[8][2];

// 8个方向的滑动顺序
extern long _slip_order[8][8];

// 在移动时需要检测的前方格子列表[figure][dir]
extern list<POINT> s_listMoveCheckCell[3][8];

class CArea;

#define RET_AI_CONTINUE		1		// 继续执行AI
#define RET_AI_BREAK		0		// 中断执行AI

#define SPEED_LEVEL_COUNT	9		//速度级数
#define  PI	3.141592654f


class CShape :
	public CBaseObject
{
public:
	static void InitMoveCheckCellList();

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
		ACT_NO = -1,			// 没有任何动作			
		ACT_STAND,				// 站(建筑静止)
		ACT_IDLE,				// 站立idle
		ACT_WALK,				// 走
		ACT_RUN,				// 跑
		ACT_BEATTACK,			// 被攻擊
		ACT_INTONATE,			// 吟唱
		ACT_BLOCK,				// 格档
		ACT_PARRY,				// 招架
		ACT_MISS,				// 闪避
		ACT_MEGRIM,				// 眩晕
		ACT_DIED,				// 死亡(建筑击毁)
		ACT_PICK,				// 拾取
		ACT_ALREADYDIED,		// 已经死亡		
		ACT_OPEN,				// 城门打开
		ACT_ALREADYOPEN,		// 城门已经打开

		ACT_PERFORM = 50,		// 施法
		ACT_MINE    = 700,		// 采矿
		ACT_MINEMAX = 721,		// 采矿最大值
		ACT_ATTACK = 950,		// 攻擊
		ACT_MAX_PERFORM = 999,	// 最大施法动作
		ACT_MAX = 1000			// 最多多少个动作
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

	//定义的行走速度分级结构
	struct tagSpeedLvl
	{
		//变化比率
		float	fRatio;
		//动作变化比率
		float	fActionRaio;
		//该级别的速度值
		float	fSpeed;
	};
///////////////////////////////////////////////////////////////
// 属性
///////////////////////////////////////////////////////////////
    
protected:
	eRgnType m_CurRgnType;// 所在的场景的类型:副本/普通场景
	long m_lRegionID;	// 所在的场景
	CGUID m_RegionGuid; //所在场景的GUID
	float m_fPosX;		// 位置X
	float m_fPosY;		// 位置Y	
	float m_fDir;		//方向
	long m_lPos;		// 當前幀
	WORD m_wState;		// 状态
	WORD m_wAction;		// 动作

	//实现移动实体的分级变速
	//实现原理：移动实体共分9个速度级,分别是基本速度的2/6,3/6,4/6,5/6,1,1.5,2,2.5,3
	//级别	范围	速度值
	//1级	-2/6	2/6
	//2级	2/6-3/6	3/6
	//3级	3/6-4/6	4/6
	//4级	4/6-5/6	5/6
	//5级	5/6-1.5	1
	//6级	1-1.5	1.5
	//7级	1.5-2	2
	//8级	2-2.5	2.5
	//9级	3-		3
	//基本行走速度
	float	m_fSpeed;
	//改变的行走速度
	float	m_fChangeSpeed;
	//分级行走速度值
	tagSpeedLvl	m_SpeedLvlValue[SPEED_LEVEL_COUNT];
	//当前的行走速度级别
	short	m_sCurSpeedLvl;
	//速度等级是否发生变化
	bool	m_bSpeedLvlChanged;
public:
	//对应基本速度，设置各等级对应的速度范围值
	void InitSpeedLvlValue();
	//当基本行走速度、变化速度发生改变的时候，重设速度级值
	void SetSpeedLvl();
	//改变速度值
	void ChangeSpeed(short nSpeed);
	void SetChangeSpeed(short nSpeed);
	//当速度发生改变以后触发
	virtual void OnSpeedChanged(){return;}
	
	void SetCurRgnType(eRgnType type) { m_CurRgnType = type; }
	eRgnType GetCurRgnType(void)	{ return m_CurRgnType; }

	virtual long GetRegionID()		{ return m_lRegionID; }
	virtual void SetRegionID(long l){ m_lRegionID = l; }
	virtual const CGUID& GetRegionGUID()				{ return m_RegionGuid; }
	virtual void SetRegionGUID(const CGUID& guid)		{ m_RegionGuid = guid; }

	virtual long GetDir()			{ return RadianDirTo8Dir(m_fDir);}
	virtual void SetDir(long dir)	{ if( dir>= DIR_UP && dir<= DIR_LEFTUP){m_fDir = PI/4*dir-PI/2;}}

	virtual	float GetDirEx()		{return m_fDir;}
	virtual void  SetDirEx(float fDir) {m_fDir = fDir;}
	
	virtual long GetPos()			{ return m_lPos; }
	virtual void SetPos(long l)		{ m_lPos = l; }
	virtual float GetSpeed();
	float GetSpeedChangedRatio();
	virtual void SetSpeed(float l);
	virtual WORD GetState()			{ return m_wState; }
	virtual void SetState(WORD l)	{ m_wState = l; }
	virtual WORD GetAction()		{ return m_wAction; }
	virtual void SetAction(WORD l)	{ m_wAction = l; }
	CShape::eDIR CShape::RadianDirTo8Dir(float fRadianDir);
	long GetTileX() {return static_cast<long>(m_fPosX);}
	long GetTileY() {return static_cast<long>(m_fPosY);}
	float GetPosX()	{return m_fPosX;}
	float GetPosY()	{return m_fPosY;}
	virtual void SetPosXY(float fX,float fY) {m_fPosX=fX;m_fPosY=fY;}

	virtual WORD GetFigure(eDIR dir=DIR_UP)	{return 0;}	// 体形
	virtual void SetBlock(int x, int y, unsigned short block);

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	bool CodeToDataBlock_ForClient(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);		// 解码
	virtual bool IsVisible(OBJECT_TYPE eType, const CGUID& rID) { return true; }	// 是否可被GUID为rID的MoveShape所见

///////////////////////////////////////////////////////////////
// 公共函数
///////////////////////////////////////////////////////////////
public:
	void GetFacePos(long& x, long& y);			// 获取面对的坐标
	void GetBackPos(long& x, long& y);			//获取背后的坐标
	CShape* GetFaceShape();							//获取面对的目标
	void GetFacePosForBigShape(long& x,long& y);	// 获取MONSTER面对的坐标
	void GetDirPos(long dir, long& x, long& y);	// 获取dir方向的坐标
	long GetRearDir();							// 获取面对的坐标
	long GetLeftDir();							// 获取左边的坐标
	long GetRightDir();							// 获取右边的坐标
	long Distance(CShape* pShape);				// 求两个Shape间的距离（单位：格子）
	long Distance(long x,long y);
	long Distance(long sx,long sy,long dx,long dy);
	FLOAT RealDistance( FLOAT fX, FLOAT fY );
	LONG  RealDistance( LONG lX, LONG lY );
	LONG  RealDistance( CShape* pTarget );
	bool IsInAround(CShape* pShape);			// 求两个SHAPE是否在附近(9个Area之内)

///////////////////////////////////////////////////////////////
//	在服务器所在区域指针
///////////////////////////////////////////////////////////////
public:
	enum eChangeState
	{
		CS_DELETE = 1,			// 是否是一个执行删除的对象
		CS_REMOVE,				// 是否是一个执行移出的对象
		CS_CHANGEAREA,			// 是否改变对象所在的AREA
		CS_CHANGEREGION,		// 是否改变场景
		CS_QUIET,				// 是否为离开状态
	};

protected:
	CArea* m_pArea;				// 所在的Area指针
	CArea* m_pNextArea;			// 将要进入的Area
	long m_lNextAreaX;			// 将要进入的区域X
	long m_lNextAreaY;			// 将要进入的区域Y

	CGUID m_NextRgnGUID;        // 将要进入的场景GUID
	long m_lNextRegionID;		// 将要进入的场景ID
	long m_lNextTileX;			// 将要进入的场景X
	long m_lNextTileY;			// 将要进入的场景Y
	long m_lNextDir;			// 将要进入的场景Dir
	long m_lChangeState;		// 对象状态

public:
	CArea* GetArea()				{return m_pArea;}
	virtual void SetArea(CArea* p)	{m_pArea=p;}
	void SetNextAreaX(long l)		{m_lNextAreaX = l;	}
	void SetNextAreaY(long l)		{m_lNextAreaY = l;	}
	long GetNextAreaX()				{return m_lNextAreaX;}
	long GetNextAreaY()				{return m_lNextAreaY;}
	CArea* GetNextArea()			{return m_pNextArea;}
	void SetNextArea(CArea* p)		{m_pNextArea=p;}
	void SetNextRegionID(long l)	{m_lNextRegionID=l;}
	long GetNextRegionID()			{return m_lNextRegionID;}
	void SetNextRegionGUID(const CGUID& l)	{m_NextRgnGUID=l;}
	const CGUID& GetNextRegionGUID()			{return m_NextRgnGUID;}
	void SetNextTileX(long l)		{m_lNextTileX=l;}
	long GetNextTileX()				{return m_lNextTileX;}
	void SetNextTileY(long l)		{m_lNextTileY=l;}
	long GetNextTileY()				{return m_lNextTileY;}
	void SetNextDir(long l)		{m_lNextDir=l;}
	long GetNextDir()			{return m_lNextDir;}
	void SetChangeState( long l)	{ m_lChangeState = l;	}
	long GetChangeState()			{ return m_lChangeState; }
};
