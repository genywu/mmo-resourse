#pragma once

#include "baseobject.h"
#include "Other/AlphaFadeObjectList.h"
#include "../../Engine/Render.h"

class CEffect;
class CBitmapX;
class AnimInfo;
class CDisplayModel;
class CAlphaFadeObjectList;

static int _dir[8][2] =
{
	0,-1,	1,-1,	1,0,	1,1,
	0,1,	-1,1,	-1,0,	-1,-1
};

//在场景中拾取物品的格子顺序
static int _Pick_Order[9][2] = 
{
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

#define RET_AI_CONTINUE		1		// 继续执行AI
#define RET_AI_BREAK		0		// 中断执行AI

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
        ACT_ALREADYOPEN,        // 城门已打开
		ACT_FALLDOWN,           // 摔倒

		

		// 选人界面动作
		ACT_SEL_X0,				// 空手站立
		ACT_SEL_X1,				// 拔剑
		ACT_SEL_X2,				// 挥剑x
		ACT_SEL_X3,				// 持剑站立x
		ACT_SEL_X4,				// 收剑x
		ACT_SEL_X5,				// 恢复站立
		
		ACT_PERFORM		  = 50,		// 施法
		ACT_MINE		  = 751,	// 采矿
		ACT_MINEMAX		  = 771,	// 采矿最大值
		ACT_FACEACTION	  = 800,	// 仪态动作的初始值
		ACT_FACEACTIONMAX = 899,	// 仪态动作的最大值
		ACT_ATTACK		  = 950,	// 攻擊
		ACT_MAX_PERFORM	  = 999,	// 最大施法动作
		ACT_MAX			  = 1000	// 最多多少个动作
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

///////////////////////////////////////////////////////////////
// 属性
///////////////////////////////////////////////////////////////
protected:
	long  m_lCurRgnType;//所在场景的类型
	long  m_lRegionID;	// 所在场景的id
	CGUID m_RegionID;	// 所在场景的guid
	float m_fPosX;		// 位置X
	float m_fPosY;		// 位置Y
	float m_fDir;		// 方向(弧度)(0-360)	

	bool m_bIsSelected;	// 是否被选中
	float m_fSpeed;		// 速度
	WORD m_wState;		// 状态
	WORD m_wAction;		// 动作
	DWORD m_dwFMFTime[8];//脚印时间标记,最多8...条...腿，应该够了吧？

	float m_fHeight;	// 高度

public:
	virtual DWORD GetFMFTime(DWORD dwIndex) const { return m_dwFMFTime[dwIndex];}
	virtual void SetFMFTime(DWORD dwIndex,DWORD dwTime) { m_dwFMFTime[dwIndex] = dwTime;}
	virtual long GetRegionID(){return m_lRegionID;}
	virtual void SetRegionID(long l){m_lRegionID=l;}
	virtual const CGUID& GetRegionGUID()		{ return m_RegionID; }
	virtual void GetRegionGUID(const CGUID& id){ m_RegionID = id; }
	virtual void SetPosXY(float x, float y);
	virtual float GetPosX()			{ return m_fPosX; }
	virtual void SetPosX(float l)	{ SetPosXY(l, GetPosY()); /*m_fPosX = l;*/ }
	virtual float GetPosY()			{ return m_fPosY; }
	virtual void SetPosY(float l)	{ SetPosXY(GetPosX(), l); /*m_fPosY = l;*/ }
	virtual eDIR GetDir();
	virtual void SetDir(eDIR dir);
	//得到或设置弧度方向的接口
	virtual float GetDirEx() { return m_fDir; }
	virtual void SetDirEx(float f) {m_fDir = f;}

	virtual float GetSpeed()		{ return m_fSpeed; }
	virtual void SetSpeed(float l)	{ m_fSpeed = l; }
	virtual WORD GetState()			{ return m_wState; }
	virtual void SetState(WORD l)	{ m_wState = l; }
	virtual WORD GetAction()		{return m_wAction;}
	virtual void SetAction(WORD l)	{ m_wAction = l; }
	virtual void OnActionChange(){}

	virtual float GetHeight()		{return m_fHeight;}
	virtual void SetHeight(float f)	{m_fHeight=f;}

	// add by lpf - 2007.12.17
	float GetPickBoxHeight();			//得到模型拾取盒局部坐标系绝对高度
	float GetSizeBoxHeight();			//得到模型尺寸盒局部坐标系绝对高度

	// add by lpf - 2009.02.05
	POINT & GetPickBoxCenterOfScreenCoord(POINT & ptScreen);			//得到模型拾取盒中心点的屏幕坐标
	POINT & GetSizeBoxCenterOfScreenCoord(POINT & ptScreen);			//得到模型尺寸盒中心点的屏幕坐标

	long GetTileX();
	long GetTileY();

	virtual BYTE GetFigure(eDIR dir=DIR_UP)	{return 0;}	// 体形
	virtual void SetBlock(int x, int y, unsigned short block);

	virtual bool AddShapeToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	virtual bool DecordShapeFromByteArray(BYTE* pByte, long& pos);	// 解码

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);

	virtual void AI(void);
	virtual	bool Display(void);
	virtual void Display(render::Camera * pCamera);
	virtual void DisplayEffect();																						// 显示特效
			void DrawSubsectionText(const char* str,D3DXVECTOR3 pos2,D3DCOLOR color,D3DCOLOR bkcolor,D3DCOLOR bkBarColor);		//画分段字符窜
	virtual void DisplayText(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0, bool bIsDisplayHPBar = false);		// 在shape身上显示文字（可分段显示，当分段显示时传入的字符窜中插如“/”来分割）
	virtual void DisplayWarBitmap(){}								//显示战斗状态的相关图标
	virtual void DisplayHoverArrow(DWORD dwColor);
	virtual void DisplayMasterName(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0);		
	void DisplayChat(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0);				// 在shape身上显示聊天文字
	void DisplayHpBar(CBitmapX* pBitmap, long lHpBarLong, long lHp, long lMaxHp, float fPos);												// 显示血条
	// 在shape身上显示减血伤害数字
	void DisplayDamage(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0);
	// 在shape身上显示吸收图片和受伤数字
	void DisplaySuck(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);
	// 在shape身上显示受伤数字和爆击图片
	void DisplayNumber(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, float fScale2=1.0, bool bBlastAttck=false);	
	// 在shape身上显示格档受伤数字和格档图片
	void DisplayBlock(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);	
	// 在shape身上显示反弹受伤数字和反弹图片
	void DisplayRebound(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);	
	// 在shape身上显示闪避图片
	void DisplayMiss(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck = false);	
	// 在shape身上显示招架图片
	void DisplayParry(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);	
	// 在shape身上显示抵抗图片
	void DisplayResist(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);
	// 在shape身上显示丢失图片
	void DisplayLost(D3DCOLOR color, D3DCOLOR bkcolor, float fPos, float fX=-1.f, float fY=-1.f, float fScale=1.0, bool bBlastAttck=false);

	virtual bool IsNeedDisplay();	// 该shape是否需要显示

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
	/***********************************************************************/
	/*  zhaohang 2010-11-15 add*/
	void GetDirPos(long dir,float& x,float& y);
	/***********************************************************************/
	void GetDirPosFromDistance(long dir, long& x, long& y, DWORD lCell);		//根据指定的方向和距离得到对应的坐标

	long GetFaceDir();								// 获取面对的坐标
	void GetPosFromRelativeDir(eDIR Dir,long &x, long &y,DWORD lDis = 1);		//相对于自身面对方向上，获取Dir方向上距离为lDis的坐标
	/***********************************************************************/
	/*  zhaohang 2010-11-15 add*/
	void GetPosFromRelativeDir(eDIR Dir,float &x,float &y,DWORD lDis = 1);
	/***********************************************************************/

	void GetPosFromCameraView(eDIR Dir, long &x,long &y);	 //根据摄像机的视角方向来计算相对位置
	virtual long Distance(CShape* pShape);			// 求两个Shape间的距离（单位：格子）
	virtual long MinXYDistance(CShape* pShape);		// 求x,y方向上的最小距离（单位：格子）

	FLOAT GetRealDistance( FLOAT fX, FLOAT fY );	// 与服务器的RealDistance( FLOAT fX, FLOAT fY )函数统一
	FLOAT RealDistance( FLOAT fX, FLOAT fY );
	LONG  RealDistance( LONG lX, LONG lY );
	LONG  RealDistance( CShape* pTarget );

	static float GetLineDirF(float sx, float sy, float dx, float dy, float fdir = 0.0f);	// 获取从(sx,sy)到(dx,dy)的方向(0-PI)
	static eDIR RadianDirTo8Dir(float fRadianDir);//从幅度方向值转换到8个枚举值方向

	void SetAppearTime(long l)				{ m_lAppearTime = l; }
	long GetAppearTime()					{ return m_lAppearTime; }
	void SetAlphaFadeInStartTime(long l)	{ m_lAlphaFadeInStartTime = l; }
	void SetAlphaFadeInTimeLength(long l)	{ m_lAlphaFadeInTimeLength = l; }
	long GetAlphaFadeInStartTime()			{ return m_lAlphaFadeInStartTime; }
	long GetAlphaFadeInTimeLength()			{ return m_lAlphaFadeInTimeLength; }
	void SetIsDeleted(bool bIs);
	bool GetIsDeleted();
	void SetIsNeedAlphaFade(bool bIs);											//是否需要Alpha渐变
	bool GetIsNeedAlphaFade()				{ return m_bIsNeedAlphaFade; }		//是否可以Alpha渐变
	void SetAlphaFadeMode(long l)			{ m_eAlphaFadeMode = (CAlphaFadeObjectList::eALPHA_FADE_MODE)l; }			//得到Alpha渐变模式
	bool GetIsAddAlphaFaded()				{ return m_bIsAddAlphaFaded; }
	void SetIsAddAlphaFaded(bool b)			{ m_bIsAddAlphaFaded = b; }
	long GetAlphaFadeMode()					{ return m_eAlphaFadeMode; }		//得到Alpha渐变模式
	float GetAlphaValue()					{ return m_fAlphaValue; }
	void SetAlphaValue(float f)				{ m_fAlphaValue = f; }

	void SetDisplayFinish(bool b)	{m_bDisplayFinish=b;}
	BOOL IsDisplayFinish()			{return m_bDisplayFinish;}
	void SetInViewRange(bool b)		{m_bInViewRange=b;}
	bool IsInViewRange()			{return m_bInViewRange;}
	void SetActionToLastFrame();

	bool CopyShapeDisplayInfo(CShape *& pOutObj);		//拷贝当前对象的显示信息到新的对象中

protected:
	long 			 m_lAppearTime;				// 出现时间
	long			 m_lAlphaFadeInStartTime;	// Alpha渐进开始时间
	long			 m_lAlphaFadeInTimeLength;	// Alpha渐进时间长度
	bool 			 m_bIsDeleted;  			// 是否是一个执行删除的对象
	bool 			 m_bIsNeedAlphaFade;		// 该对象是否需要进行Alpha渐变操作
	bool			 m_bIsAddAlphaFaded;		// 是否已经将该对象添加到了Alpha渐变列表中
	float			 m_fAlphaValue;				// 当前Alpha值
	CAlphaFadeObjectList::eALPHA_FADE_MODE m_eAlphaFadeMode;			// 该对象Alpha渐变操作模式

	AnimInfo	  * m_pAnimInfo;
	CDisplayModel * m_pDisplayModel;//显示模型数据	

	BOOL m_bDisplayFinish;		// 是否播放结束
	bool m_bInViewRange;		// 是否在显示范围内

//////////////////////////////////////////////////////////////////////////
//	显示方向
//////////////////////////////////////////////////////////////////////////
public:
	void SetNowDir(float f)					{m_fNowDir=f;}
	float GetNowDir()						{return m_fNowDir;}
	void Set3DRotMatrix(D3DXMATRIX dir)					{m_matdir=dir;}
	D3DXMATRIX Get3DRotMatrix()						{return m_matdir;}
	//设置显示模型数据
	AnimInfo * GetAnimInfo() {return m_pAnimInfo;}
	virtual void SetDisplayModel(long lType,long lID){}
	CDisplayModel* GetDisplayModel()	{return m_pDisplayModel;}

	//初始化显示模型
	bool SetDisplayModel(bool bAsyn = true);
	//设置模型显示的Group
	virtual void SetDisplayModelGroup(bool bAsyn=true);

	void UpdateAnimation();							//更新动画

protected:
	float	m_fNowDir;			// 当前的显示方向(0-360)
	D3DXMATRIX m_matdir;        // 3D旋转矩阵     

//////////////////////////////////////////////////////////////////////////
//	模型颜色
//////////////////////////////////////////////////////////////////////////
public:
	static D3DCOLOR s_colorModelDefault;
	D3DCOLOR m_colorModel;				// 模型颜色
	DWORD	 m_dwCurrentModelCol;		// 当前绘制的模型颜色

public:
	D3DCOLOR GetModelColor()			{ return m_colorModel; }
	D3DCOLOR GetCurrentModelColor()		{ return m_dwCurrentModelCol; }
	void SetModelColor(D3DCOLOR color)	{ m_colorModel=color; }

//////////////////////////////////////////////////////////////////////////
//	附带的特效
//////////////////////////////////////////////////////////////////////////
protected:
	list<CEffect*>		m_listEffect;	// 特效列表

public:
	CEffect* FindEffect(long lGraphicsID, long lID = 0);		// 查找是否有特效
	CEffect* FindEffectByNum(long lNum);		// 查找是否有特效
	const CGUID& AddEffect(long lGrahpicsID, float fx=-99999.f, float fy=-99999.f, float fHeight=-99999.f, 
		bool bLoop=false, float fDir=0.f, long lID = 0,bool bAsyn=true);		// 添加特效
	const CGUID& AddEffectLocator(long lGraphicsID, CShape* pHoster, eLOCATOR eLocator, bool bLoop=false, float fDir=0.f, long lID = 0,bool bAsyn = true, float fHeightAdjust = 0.0f);	// 播放特效(定位点)
	void DelEffect(long lGrahpicsID, long lID = 0);			// 删除特效
	void DelAllEffect();						// 删除所有特效

	//================07.12.19liuke==================
	CEffect* FindEffectByIndex(CGUID guid);		// 查找指定索引的特效
	void DelEffectByIndex(CGUID guid);				// 删除指定索引的特效
	void RemoveEffectByIndex(CGUID guid);			// 将指定索引的特效从列表中移出
	//===============================================

	static eLOCATOR GetLocatorName(DWORD dwIndex);	// 根据索引获取定位点的值
	BOOL GetLocatorPos(eLOCATOR eLocator, float &x, float &y, float &fHeight,DWORD dwFrame = -1);		// 获取定位点坐标

	// ----------拷贝指定shape的所有装备信息08.8.28-------------
	virtual void CopyAllEquip(CShape *pShape)	{};
	// 拷贝模型信息
	void CopyModelInfo(CShape * pShape);

private:
	static long lEffectID;						// 特效ID计数，每添加一个ID，该技术累加1。

//////////////////////////////////////////////////////////////////////////
//	攻击动作（2套中的一套）
//////////////////////////////////////////////////////////////////////////
public:
	void	SetAttackAction(char c)		{m_cAttackAction=c;}
	char	GetAttackAction()			{return m_cAttackAction;}
protected:
	char	m_cAttackAction;		// 当前的攻击动作

	DWORD   m_iActionCount;				//当前动作播放的次数上限
};
