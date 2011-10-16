//=============================================================================
/**
*  file: CRegionList.h
*
*  Brief:地图配置。改为xml配置地图信息
*
*  Authtor:刘科
*	
*  Datae:2008-2-1
*/
//=============================================================================
#pragma once

// 状态设置
class CRegionList
{
public:
	// 地图类型
	enum MAP_TYPE
	{
		REGIONMAP_TYPE = 0,			// 场景
		AREAMAP_TYPE,				// 区域
		WORLDMAP_TYPE				// 世界
	};
	// 场景节点搜索模式
	enum SEARCH_MODE
	{
		UNKNOWN_MODE = -1,			// 无效
		REGION_REGION,				// 在一个国家里的场景查找（源场景和目标场景的区域ID相同，选择这种方式）
		AREA_AREA,					// 从一个国家到另一个国家的查找（源场景和目标场景的区域ID不同，且都不为0，选择这种方式）
		AREA_WREGION,				// 从一个国家到世界场景的查找（源场景的区域ID不为0，目标场景区域ID为0，选择这种方式）
		WREGION_AREA,				// 世界场景到某个国家的查找（源场景的区域ID为0，目标场景区域ID不为0，选择这种方式）
		WREGION_WREGION,			// 世界场景到世界场景的查找（源场景和目标场景的区域ID都为0，选择这种方式）
	};
	// 坐标点
	struct tagPos
	{
		long x;
		long y;
		tagPos()
		{
			x = 0;
			y = 0;
		}
	};
	// 场景中的地点信息
	struct tagAddressDetails
	{
		tagPos	stPos;
		long AddRegionId;				// 该地点对应的场景ID（如果该地点表示的是场景切换点的话才有效）
		string strAddName;				// 在当前地图下显示的地点名字
		tagAddressDetails()
		{
			AddRegionId = 0;
			strAddName = "";
		}
	};
	// 场景结构
	struct tagRegion
	{
		long	lID;									// id
		string	strName;								// 名字
		RECT	rect;									// 在大地图上的位置范围
		tagPos	stPos;									// 在父场景里的位置
		long	lParentRegionID;						// 父场景ID
		long	lParentAreaID;							// 所属区域ID
		long	lParentWorldID;							// 所属大地图ID
		long	lIconID;								// 当前区域的 特效图片 ID
		vector<tagAddressDetails> vecCurAddrDetails;	// 保存该大地图下的详细地点信息
		vector<RECT> vec_stRect;						// 该场景下的禁行区域
		tagRegion()
		{
			lID = 0;
			lParentRegionID = 0;
			lParentAreaID = 0;
			lParentWorldID = 0;
			strName = "";
		}
	};
	// 区域结构
	struct tagArea
	{
		long	lID;						// id
		string	strName;					// 名字
		RECT	rect;						// 在大地图上的位置范围
		long	lParentWorldID;				// 所属大地图ID
		long	lBorderID;					// 该国的边境ID
		map<long, tagRegion> m_mpRegions;	// 保存该区域下的详细场景信息
	};
	// 世界地图结构
	struct tagWorldMap
	{
		long	lID;						// 世界地图的编号
		string	strName;					// 世界地图的描述名字
		vector<tagArea>		vecArea;		// 本地图下的区域列表
		map<long, tagRegion> m_mpRegions;	// 本地图下的直属场景列表
	};
	// 场景路径信息（主要用于描述跨场景寻路的各个场景节点的信息）
	struct tagRegionPathInfo 
	{
		long lRegionID;						// 场景节点
		tagPos stPos;						// 要到达该场景，所需要的在本场景下对应的坐标（一般指玩家所在场景到该场景的切换点）
		long lIndex;						// 该点在场景路径列表中的索引
		tagRegionPathInfo()
		{
			lRegionID = 0;
			lIndex = 0;
		}
	};

	//=======================================================
private:
	static long m_lSouAreaID;				// 起始场景
	// 查找模式
	static long m_lSearchMode;				// 搜索模式
	// 查找的场景路径列表
	static list<tagRegionPathInfo>	m_lsRegionPathList;
	// 世界地图列表
	static list<tagWorldMap>	m_lsWorldMap;
	// 初始化世界信息
	static BOOL InitWorldInfo(tagWorldMap *stWorldMap,TiXmlElement* pElem);
	// 初始化大地图下的区域信息
	static BOOL InitAreaInfo(tagArea *stArea,TiXmlElement* pElem);
	// 初始化场景信息(第2、3个参数分别表示该场景上一层的类型和ID)
	static BOOL InitRegionInfo(tagRegion *stRegion,MAP_TYPE lUpType,void *pUp, TiXmlElement* pElem);
	// 初始化场景下的禁行区域信息
	static BOOL InitRegionRectInfo(tagRegion *stRegion);
	// 添加场景路径(最后一个参数表示搜索模式)
	static BOOL AddRegionPath(list<tagRegionPathInfo> & lsRegionPath,tagRegionPathInfo stSourInfo,tagRegion * pTarget);

public:
	CRegionList(void);
	virtual ~CRegionList(void);

	// 解析地图配置文件
	static BOOL LoadMap();
	// 获得查找的场景路径列表
	static list<tagRegionPathInfo> & GetRegionPathList()	{return m_lsRegionPathList;}
	// 获得指定编号的世界地图
	static tagWorldMap * GetWorldMap(long lWorldMapID);
	// 获得指定编号的区域
	static tagArea * GetArea(long lAreaID);
	// 获得指定编号的场景
	static tagRegion* GetRegion(long lRegionID);
	// 查找指定场景到目标场景的场景路径
	static BOOL FindRegionPath(long lSourID,long lTargetID);
	//=======================================================
};



/******************************************************************************
*  file: CMailBoxList.h
*
*  Brief:场景中的邮筒配置，标示了各个场景中的邮筒位置
*
*  Authtor:刘科
*	
*  Datae:2009-3-7
******************************************************************************/

class CMailBoxPosList
{
public:
	// 坐标点
	struct tagPos
	{
		long x;
		long y;
		tagPos()
		{
			x = 0;
			y = 0;
		}
	};
	// 场景中的邮箱结构
	struct tagMailBoxPos
	{
		long					lResourceID;				// 场景资源ID
		string					strName;					// 场景名字
		vector<tagPos>			vecPos;						// 邮箱坐标列表
	};

	CMailBoxPosList(void);
	virtual ~CMailBoxPosList(void);

	// 解析邮筒坐标配置文件
	static BOOL LoadMailBoxPos();
	// 释放邮筒坐标配置文件
	static void ReleaseMailBoxPos();
	// 获得指定场景中，离指定坐标最近的邮箱坐标
	// 参数1：场景资源ID，参数2，参数3：指定的坐标
	static void GetMailBoxPos(long lResourceID,long sx,long sy,long& dx,long& dy);

private:
	// 所有场景的邮箱
	static map<long , tagMailBoxPos>			m_mapMailBoxPos;
	// 初始化各个地图下的邮箱坐标
	static BOOL InitMailPos(tagMailBoxPos& stMailBoxPos,TiXmlElement* pElem);
};