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
		CM_MUD,				// 泥地
		CM_SAND,			// 沙地
		CM_WOOD,			// 木板
		CM_STONE,			// 石板
		CM_WATER,			// 水
		CM_SNOW,			// 雪地
		CM_MARISH,			// 沼泽
	};

	// 阻挡类型
	enum eBLOCK
	{
		BLOCK_NO = 0,		// 无阻挡
		BLOCK_CANFLY,		// 可飞行阻挡
		BLOCK_UNFLY,		// 不可飞行阻挡
		BLOCK_SHAPE,		// 人物
		BLOCK_AIM,			// 怪物以及采集物
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

	//占据该格子的对象
	struct tagCellObj
	{
		//保存在该格子上的对象数量
		long nObjNum;
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
	void SetResourceID(long l)		{m_lResourceID=l;}
	long GetConfigID()				{return m_lConfigID;}
	void SetConfigID(long l)		{m_lConfigID=l;}
	float GetExpScale()				{return m_fExpScale;}
	void SetExpScale(float f)		{ m_fExpScale = f;}
	long GetWidth()					{return m_lWidth;}
	void SetWidth(long l)			{m_lWidth = l;}
	long GetHeight()				{return m_lHeight;}
	void SetHeight(long l)			{m_lHeight = l;}

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

	//得到某个格子上的obj数量
	long GetCellObjNum(long x,long y)
	{
		if(x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())
		{
			tagCellObj* pCellObj = &m_pCellObj[GetWidth()*y+x];
			return pCellObj->nObjNum;
		}
		return 0;
	}
	//对格子上obj数量加1
	void IncCellObjNum(long x,long y)
	{
		if(x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())
		{
			tagCellObj* pCellObj = &m_pCellObj[GetWidth()*y+x];
			pCellObj->nObjNum++;
		}
	}
	//对格子上obj数量减1
	void DecCellObjNum(long x,long y)
	{
		if(x>-1&&x<GetWidth()&&y>-1&&y<GetHeight())
		{
			tagCellObj* pCellObj = &m_pCellObj[GetWidth()*y+x];
			if(pCellObj->nObjNum>0)
				pCellObj->nObjNum--;
		}
	}

	// 切换点
	long AddSwitch(tagSwitch* pSwitch);
	void DelSwitch(long lPos);
	tagSwitch* GetSwitch(long l);
	tagSwitch* GetSwitch(long x, long y);
	long HasSwitch(tagSwitch* pSwitch);	// 是否存在指定的的切换点

protected:
	string m_strFileName;				// 文件名

	long m_lRegionType;					// 地图属性
	long m_lConfigID;					// 配置文件设置的ID
	long m_lResourceID;					// 资源ID
	float m_fExpScale;					// 获得的经验倍数
	long m_lWidth;						// 格子列数
	long m_lHeight;						// 格子行数

	tagCell*		m_pCell;			// 格子数据
	vector<tagSwitch>	m_vectorSwitch;	// 切换点列表

	tagCellObj* m_pCellObj;				//格子上的Obj数据

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual	void Run()		{};
//	virtual	void AI()		{};
	virtual	bool Display()	{ return true;}
	virtual	void ScrollCenter(long x, long y)	{};

	virtual BOOL Load();	// 读取
	virtual BOOL Save();	// 存储
	virtual BOOL New();		// 新建
};
