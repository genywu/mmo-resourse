#pragma once
#pragma warning(disable:4819)

#include <d3d9.h>
#include <d3dx9.h>

#include "baseobject.h"
#include "mmsystem.h"

class CEffect;
class CBitmapX;

static int _dir[8][2] =
{
	0,-1,	1,-1,	1,0,	1,1,
	0,1,	-1,1,	-1,0,	-1,-1
};

//在场景中拾取物品的格子顺序
static int _Pick_Orher[9][2] = {
	0,0,	//玩家所在的格子
	-1,0,	//下
	1,0,	//上	
	0,-1,	//右
	0,1,	//左
	-1,1,	//左下
	-1,-1,	//右下
	1,-1,	//右上
	1,1		//左上
};
// 8个方向的滑动顺序
static int _slip_order[8][8] =
{
	0, 7, 1, 6, 2, 5, 3, 4,
	1, 0, 2, 7, 3, 6, 4, 5,
	2, 1, 3, 0, 4, 7, 5, 6,
	3, 2, 4, 1, 5, 0, 6, 7,
	4, 3, 5, 2, 6, 1, 7, 0,
	5, 4, 6, 3, 7, 2, 0, 1,
	6, 5, 7, 4, 0, 3, 1, 2,
	7, 6, 0, 5, 1, 4, 2, 3
};


class CShape :
	public CBaseObject
{
public:
	CShape(void);
	inline virtual ~CShape(void);

	// 状态
	enum eSTATE
	{
		STATE_PEACE,			// 和平状态
		STATE_FIGHT,			// 战斗状态
		STATE_DIED,				// 死亡状态
		STATE_FEIGNDEATH,		// 假死状态
		STATE_OPEN,				// 城门打开状态
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


		// 选人界面动作
		ACT_SEL_X0,				// 空手站立
		ACT_SEL_X1,				// 拔剑
		ACT_SEL_X2,				// 挥剑x
		ACT_SEL_X3,				// 持剑站立x
		ACT_SEL_X4,				// 收剑x
		ACT_SEL_X5,				// 恢复站立

		ACT_PERFORM = 50,		// 施法
		ACT_MINE    = 751,		// 采矿
		ACT_MINEMAX = 771,		// 采矿最大值
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

	// 定位点
	enum eLOCATOR
	{
		LOCATOR_WIND			= MAKEFOURCC('0','0','0','0'),		// 胸口
		LOCATOR_HEAD			= MAKEFOURCC('0','0','0','1'),		// 头顶
		LOCATOR_FOOTCENTER		= MAKEFOURCC('0','0','0','2'),		// 脚底中心
		LOCATOR_LEFTHAND		= MAKEFOURCC('0','0','0','3'),		// 左手
		LOCATOR_RIGHTHAND		= MAKEFOURCC('0','0','0','4'),		// 右手
		LOCATOR_LEFTFOOT		= MAKEFOURCC('0','0','0','5'),		// 左脚
		LOCATOR_RIGHTFOOT		= MAKEFOURCC('0','0','0','6'),		// 右脚
		LOCATOR_BACK			= MAKEFOURCC('0','0','0','7'),		// 背
		LOCATOR_LEFTWAIST		= MAKEFOURCC('0','0','0','8'),		// 左腰	
		LOCATOR_RIGHTWAIST		= MAKEFOURCC('0','0','0','9'),		// 右腰	
		LOCATOR_LEFTARM         = MAKEFOURCC('0','0','1','0'),		// 左臂
		LOCATOR_RIGHTARM        = MAKEFOURCC('0','0','1','1'),		// 右臂
		LOCATOR_LEFTBACK		= MAKEFOURCC('0','0','1','2'),		// 左后背
		LOCATOR_RIGHTBACK		= MAKEFOURCC('0','0','1','3'),		// 右后背
		LOCATOR_BIGSWORD		= MAKEFOURCC('0','0','1','4'),		// 后背长剑
		LOCATOR_BACKBOW			= MAKEFOURCC('0','0','1','5'),		// 后背弓
		LOCATOR_BACKARROW		= MAKEFOURCC('0','0','1','6'),		// 后背箭袋
		LOCATOR_BACKSTAFF       = MAKEFOURCC('0','0','1','7'),		// 后背杖,斧
		LOCATOR_LEFTBOXING      = MAKEFOURCC('0','0','1','8'),		// 左拳
		LOCATOR_RIGHTBOXING     = MAKEFOURCC('0','0','1','9'),		// 右拳
		LOCATOR_LEFTHURTPOINT   = MAKEFOURCC('0','0','2','1'),		// 左受伤点
		LOCATOR_RIGHTHURTPOINT  = MAKEFOURCC('0','0','2','2'),		// 右受伤点
		LOCATOR_HOLDPOINT		= MAKEFOURCC('0','0','2','3'),		// 抱人点

	};

	// 基础属性
protected:
	long  m_lCurRgnType;					// 所在场景的类型
	long  m_lRegionID;						// 所在场景的id
	CGUID m_RegionGuid;						// 所在场景的guid
	float m_fPosX;							// 所在场景位置X
	float m_fPosY;							// 所在场景位置Y
	float m_fDir;							// 所在场景方向

	//	bool m_bIsSelected;						//是否被选中
    float m_fSpeed;							// 速度
    WORD m_wState;							// 状态
	WORD m_wAction;							// 动作
	//	DWORD m_dwFMFTime[8];						//脚印时间标记,最多8...条...腿，应该够了吧？

	//	float m_fHeight;						// 高度

public:
	//	virtual DWORD GetFMFTime(DWORD dwIndex) const { return m_dwFMFTime[dwIndex];}
	//	virtual void SetFMFTime(DWORD dwIndex,DWORD dwTime) { m_dwFMFTime[dwIndex] = dwTime;}

	virtual long GetRegionID(){return m_lRegionID;}
	virtual void SetRegionID(long l){m_lRegionID=l;}

	virtual const CGUID& GetRegionGUID()		{ return m_RegionGuid; }
	virtual void GetRegionGUID(const CGUID& id){ m_RegionGuid = id; }

	virtual void SetPosXY(float x, float y);

	virtual float GetPosX()			{ return m_fPosX; }
	virtual void SetPosX(float l)	{ SetPosXY(l, GetPosY()); /*m_fPosX = l;*/ }

	virtual float GetPosY()			{ return m_fPosY; }
	virtual void SetPosY(float l)	{ SetPosXY(GetPosX(), l); /*m_fPosY = l;*/ }

	virtual eDIR GetDir();
	virtual void SetDir(eDIR dir);
	//得到/设置 弧度方向
	virtual float GetDirEx()	{ return m_fDir;}
	virtual void SetDirEx( float f) { m_fDir = f;}

    virtual float GetSpeed()		{ return m_fSpeed; }
    virtual void SetSpeed(float l)	{ m_fSpeed = l; }

	virtual WORD GetState()			{ return m_wState; }
	virtual void SetState(WORD l)	{ m_wState = l; }

	virtual WORD GetAction()		{return m_wAction;}
	virtual void SetAction(WORD l)	{ m_wAction = l; }
	virtual void OnActionChange(){}

	//	virtual float GetHeight()		{return m_fHeight;}
	//	virtual void SetHeight(float f)	{m_fHeight=f;}

	long GetTileX();
	long GetTileY();

	virtual BYTE GetFigure(eDIR dir=DIR_UP)	{return 0;}	// 体形
	virtual void SetBlock(int x, int y, unsigned short block);

	virtual bool AddShapeToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	virtual bool DecordShapeFromByteArray(BYTE* pByte, long& pos);	// 解码

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	//----------------------------------------------
	//	新加解码接口
	//----------------------------------------------
	virtual bool DecordFromDataBlock(DBReadSet& db,bool bEx = true);
public:
	virtual	DWORD	GetValue(const char* strName)	{return 0;};
	virtual	DWORD	SetValue(const char* strName, DWORD dwValue)	{return 0;};
	virtual	DWORD	ChangeValue(const char* strName, long lValue)		{return 0;};

	///////////////////////////////////////////////////////////////
	// 公共函数
	///////////////////////////////////////////////////////////////
public:
	void GetFacePos(long& x, long& y);				// 获取面对的坐标
	CShape* GetFaceShape();							// 获取面对的目标

	void GetDirPos(long dir, long& x, long& y);		// 获取dir方向的坐标
	long GetFaceDir();								// 获取面对的坐标

	virtual long Distance(CShape* pShape);			// 求两个Shape间的距离（单位：格子）
	virtual long MinXYDistance(CShape* pShape);		// 求x,y方向上的最小距离（单位：格子）

	FLOAT RealDistance( FLOAT fX, FLOAT fY );
	LONG  RealDistance( LONG lX, LONG lY );
	LONG  RealDistance( CShape* pTarget );

	static float GetLineDirF(float sx, float sy, float dx, float dy, float fdir = 0.0f);	// 获取从(sx,sy)到(dx,dy)的方向(0-PI)
	static eDIR RadianDirTo8Dir(float fRadianDir);
	//	显示方向
public:
	void SetNowDir(float f)					{m_fNowDir = f;}
	float GetNowDir()						{return m_fNowDir;}
	//显示方向
protected:
	float	m_fNowDir;			// 当前的显示方向(0-360)

	//用于计算shape的2D平面坐标值
	// by  MartySa 2009.2.5

	//POINT	&GetPickBoxCenterOfScreenCoord(POINT &ptScreen, long lTileHeight, D3DXMATRIX &ViewMatrix, D3DXMATRIX &ProjectionMatrix);
	//判断当前shape是否受到攻击，用于DriveClient 
private:
	bool	m_bIsAtk;
public:
	void    SetShapeIsAtk(bool b)			{ m_bIsAtk = b; }
	bool	GetShapeIsAtk()					{ return m_bIsAtk; }

};