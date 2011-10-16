#pragma once
#include "baseobject.h"

const static char *g_strRegionHead = "CLS-RGN";	// 文件头
const static long g_lRegionVersion = 1;			// 版本号

class CRegion :
	public CBaseObject
{
public:
	CRegion(void);
	virtual ~CRegion(void);

//////////////////////////////////////////////////////////////////////////
//	数据结构
//////////////////////////////////////////////////////////////////////////
public:
	// 地图属性
	enum eREGION_TYPE
	{
		RT_KILLABLE	= 0x00000001,		// 允许PK战斗
		RT_FIGHTABLE= 0x00000002,		// 允许和平竞技
		RT_NIGHTABLE= 0x00000004,		// 昼夜交替
		RT_RAINABLE	= 0x00000008,		// 允许下雨
		RT_SNOWABLE	= 0x00000010,		// 允许下雪
		RT_DARK		= 0x00000020,		// 黑雾
		RT_RECALL	= 0x00000040,		// 能否用回城卷
		RT_RANDOM	= 0x00000080,		// 能否用随机卷
		RT_DRUG		= 0x00000100,		// 能否用补药
	};

	// 材质类型
	enum eCELL_MATERIAL
	{
		CM_NULL = 0,		// 空
		CM_GRASS,			// 草地
		GM_MUD,				// 泥地
		GM_SAND,			// 沙地
		GM_WOOD,			// 木板
		GM_STONE,			// 石板
		GM_WATER,			// 水
	};

	// 阻挡类型
	enum eBLOCK
	{
		BLOCK_NO = 0,		// 无阻挡
		BLOCK_CANFLY,		// 可飞行阻挡
		BLOCK_UNFLY,		// 不可飞行阻挡
		BLOCK_SHAPE,		// 人物
	};

	//安全区定义
	enum eSecurity
	{
		SECURITY_FREE = 0,		//非安全区
		SECURTIY_FIGHT,			//竞技区
		SECURTIY_SAFE,			//安全区
	};


	// 基本格子结构
	struct tagCell
	{
		unsigned short	bBlock		:3;		// 阻挡（0：可行走  1：可飞行  2：不可飞行）
		unsigned short	bSecurity	:3;		// 安全区
		unsigned short	lWar		:2;		// 城战区
		unsigned short	lMaterial	:8;		// 材质
		WORD			lSwitch;			// 切换点(编号从1开始)
	};

	// 切换点类型
	enum eCHANGE_POINT_TYPE
	{
		CPT_NORMAL,		// 直接切换
		CPT_RANDOM,		// 随机切换
		CPT_SCRIPT,		// 脚本触发
	};

	// 切换点
	struct tagSwitch
	{
		long			lState;							// 切换状态
		long			lRegionID;						// 出口点ID号
		long			lCoordX;						// 出口点坐标
		long			lCoordY;
		long			lDir;							// 出口点方向
	};

/////////////////////////////////////////////////////////////////////
// 地图属性接口
/////////////////////////////////////////////////////////////////////
public:

	const char* GetFileName()			{return m_strFileName.c_str();}
	void SetFileName(const char* str)	{m_strFileName = str;}

	long GetRegionType()			{return m_lRegionType;}
	void SetRegionType(long l)		{m_lRegionType=l;}
	long GetResourceID()			{return m_lResourceID;}
	void SetResourceID(long l)		{m_lResourceID = l;}
	long GetSvrResourceID()			{return m_lSvrResourceID; }
	void SetSvrResourceID( long l ) {m_lSvrResourceID = l; }
	float GetExpScale()				{return m_fExpScale;}
	void SetExpScale(float f)		{m_fExpScale = f;}
	float GetOccuExpScale()			{return m_fOccuExpScale;}
	void SetOccuExpScale(float f)	{m_fOccuExpScale = f;}
	float GetSpScale()				{return m_fSpScale;}
	void SetSpScale(float f)		{m_fSpScale = f;}
	float GetMeriScale()			{return m_fMeriScale;}
	void  SetMeriScale(float f)		{m_fMeriScale = f;}
	long GetWidth()					{return m_lWidth;}
	void SetWidth(long l)			{m_lWidth = l;}
	long GetHeight()				{return m_lHeight;}
	void SetHeight(long l)			{m_lHeight = l;}
	BYTE GetCountry()				{ return m_btCountry; }
	void SetCountry(BYTE c)			{ m_btCountry = c; }

	long GetNotify()				{ return m_lNotify; }
	void SetNotify(long l)			{ m_lNotify = l; }

	bool IsKillAble()		{return (m_lRegionType & RT_KILLABLE)?true:false;}
	bool IsFightAble()		{return (m_lRegionType & RT_FIGHTABLE)?true:false;}
	bool IsNightAble()		{return (m_lRegionType & RT_NIGHTABLE)?true:false;}
	bool IsRainAble()		{return (m_lRegionType & RT_RAINABLE)?true:false;}
	bool IsSnowAble()		{return (m_lRegionType & RT_SNOWABLE)?true:false;}
	bool IsDark()			{return (m_lRegionType & RT_DARK)?true:false;}
	bool IsRecall()			{return (m_lRegionType & RT_RECALL)?true:false;}
	bool IsRandom()			{return (m_lRegionType & RT_RANDOM)?true:false;}
	bool IsDrug()			{return (m_lRegionType & RT_DRUG)?true:false;}

	void SetKillAble(bool b)	{(b)?(m_lRegionType |= RT_KILLABLE):(m_lRegionType &= ~RT_KILLABLE);}
	void SetFightAble(bool b)	{(b)?(m_lRegionType |= RT_FIGHTABLE):(m_lRegionType &= ~RT_FIGHTABLE);}
	void SetNightAble(bool b)	{(b)?(m_lRegionType |= RT_NIGHTABLE):(m_lRegionType &= ~RT_NIGHTABLE);}
	void SetRainAble(bool b)	{(b)?(m_lRegionType |= RT_RAINABLE):(m_lRegionType &= ~RT_RAINABLE);}
	void SetSnowAble(bool b)	{(b)?(m_lRegionType |= RT_SNOWABLE):(m_lRegionType &= ~RT_SNOWABLE);}
	void SetDark(bool b)		{(b)?(m_lRegionType |= RT_DARK):(m_lRegionType &= ~RT_DARK);}
	void SetRecall(bool b)		{(b)?(m_lRegionType |= RT_RECALL):(m_lRegionType &= ~RT_RECALL);}
	void SetRandom(bool b)		{(b)?(m_lRegionType |= RT_RANDOM):(m_lRegionType &= ~RT_RANDOM);}
	void SetDrug(bool b)		{(b)?(m_lRegionType |= RT_DRUG):(m_lRegionType &= ~RT_DRUG);}

	// 格子
	void SetCell(int x, int y, tagCell* p)
	{
		if(x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())
		{m_pCell[GetWidth()*y+x] = *p;}
	}
	tagCell* GetCell(int x, int y)
	{
		return (x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())?&m_pCell[GetWidth()*y+x]:NULL;
	}

	// 阻挡设置
	void SetBlock(int x, int y, BYTE b)	{if(x>-1&&x<GetWidth()&&y>-1&&y<GetHeight()){m_pCell[GetWidth()*y+x].bBlock = b;}}
	BYTE GetBlock(int x, int y)			{return (x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())?m_pCell[GetWidth()*y+x].bBlock:BLOCK_UNFLY;}

	// 切换点
	long AddSwitch(tagSwitch* pSwitch);
	void DelSwitch(long lPos);
	tagSwitch* GetSwitch(long l);
	tagSwitch* GetSwitch(long x, long y);
	long HasSwitch(tagSwitch* pSwitch);	// 是否存在指定的的切换点

	// set rgn type
	void SetRgnType(long type) { m_lRgnType = type; }
	long GetRgnType(void)      { return m_lRgnType; }

	void SetLinedIdFlag(long id) { m_lLinedIdFlag = id; }
	long GetLinedIdFlag(void)      { return m_lLinedIdFlag; }

	void SetACRgnType( long type ) { m_lACRgnType = type; }
	long GetACRgnType() const { return m_lACRgnType; }

	vector<tagSwitch>& GetSwitchVec(void) { return m_vectorSwitch; }
	tagCell* GetCellPtr(void) { return m_pCell; }

	BYTE GetRgnHideFlag(void) { return m_byRgnHideFlag; }
	void SetRgnHideFlag(BYTE flag) { m_byRgnHideFlag = flag; }

    //归队
    long GetRgnRejoinFlag(void)  {return m_lRejoinTeam; }
    void SetRgnRejoinFlag(long flag) { m_lRejoinTeam = flag;}
protected:
	string m_strFileName;				// 文件名

	long m_lRgnType;                    // 地图类型 副本/普通 场景
	long m_lRegionType;					// 地图属性
	long m_lResourceID;					// 场景资源ID
	long m_lSvrResourceID;				// 服务器资源ID
	float m_fExpScale;					// 获得经验系数
	float m_fOccuExpScale;				// 获得职业经验系数
	float m_fSpScale;					// 获取元气的系数
	float m_fMeriScale;					// 获取功勋的系数
	long m_lWidth;						// 格子列数
	long m_lHeight;						// 格子行数
	BYTE m_btCountry;					// 所属国家
	long m_lNotify;						// 击杀通知
	long m_lLinedIdFlag;				// 分线场景ID
	long m_lACRgnType;					// 反外挂场景类型
    long m_lRejoinTeam;                 // 是否进行归队操作

	tagCell*		m_pCell;			// 格子数据
	vector<tagSwitch>	m_vectorSwitch;	// 切换点列表

	BYTE	m_byRgnHideFlag;				// 玩家隐匿标志 0：默认不隐匿可隐匿 1：隐匿 2：不可隐匿
//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual	void Run()		{};
	virtual	void AI()		{};

	virtual BOOL Load();	// 读取
	virtual BOOL Save();	// 存储
	virtual BOOL New();		// 新建

/////////////////////////////////////////////////////////////////////
// 存取接口
/////////////////////////////////////////////////////////////////////
public:
	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
public:	
	bool GetRandomPos( long &lX , long &lY );		// 获取一个无阻挡的随机点
	bool GetRandomPosInRange( long &lX , long &lY, long sx, long sy, long w, long h );	// 在一个范围内获取一个随机不是阻挡的点

	void SetID(long lRegionID){m_lID = lRegionID;};
	long GetID(void){return m_lID;};

	//! 此ID为手动配置的ID
	long m_lID;
};
