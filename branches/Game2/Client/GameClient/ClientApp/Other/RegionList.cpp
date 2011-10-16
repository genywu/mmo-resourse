#include "stdafx.h"
#include "../GameClient/Game.h"
#include "RegionList.h"
#include <sstream>
#include "../../../Public/Common/CStrConverter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

list<CRegionList::tagWorldMap>			CRegionList::m_lsWorldMap;
list<CRegionList::tagRegionPathInfo>	CRegionList::m_lsRegionPathList;
long	CRegionList::m_lSearchMode		= -1;
long	CRegionList::m_lSouAreaID		= 0;
CRegionList::CRegionList(void)
{
}

CRegionList::~CRegionList(void)
{
	m_lsWorldMap.clear();
	m_lsRegionPathList.clear();
}

// 获得指定编号的场景信息
CRegionList::tagRegion* CRegionList::GetRegion(long lRegionID)
{
	list<tagWorldMap>::iterator it = m_lsWorldMap.begin();
	for(; it != m_lsWorldMap.end(); ++it)
	{
		// 当前大地图的直属场景列表中查找
		if(it->m_mpRegions.find(lRegionID)!=it->m_mpRegions.end())
			return &(it->m_mpRegions[lRegionID]);
		// 大地图的各个区域下的场景列表查找
		for(size_t i=0; i<it->vecArea.size(); ++i)
		{
			tagArea stArea = (it->vecArea)[i];
			if(stArea.m_mpRegions.find(lRegionID)!=stArea.m_mpRegions.end())
				return &(((it->vecArea)[i]).m_mpRegions[lRegionID]);
		}
	}
	return NULL;
}

// 获得指定编号的区域信息
CRegionList::tagArea * CRegionList::GetArea(long lAreaID)
{
	list<tagWorldMap>::iterator it = m_lsWorldMap.begin();
	for(; it != m_lsWorldMap.end(); ++it)
	{
		// 大地图的各个区域下的场景列表查找
		for(size_t i=0; i<it->vecArea.size(); ++i)
		{
			if(it->vecArea[i].lID == lAreaID)
				return &(it->vecArea[i]);
		}
	}
	return NULL;
}

// 获得指定编号的世界地图
CRegionList::tagWorldMap * CRegionList::GetWorldMap(long lWorldMapID)
{
	list<tagWorldMap>::iterator it = m_lsWorldMap.begin();
	for(; it != m_lsWorldMap.end(); ++it)
	{
		if(it->lID == lWorldMapID)
			return &(*it);
	}
	return NULL;
}

// 初始化世界信息
BOOL CRegionList::InitWorldInfo(tagWorldMap *stWorldMap,TiXmlElement* pElem)
{
	if(!pElem||!stWorldMap)
		return FALSE;
	// 遍历节点,解析该地图下的区域信息或直属场景信息
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL;
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 区域信息
		if(!strcmp(pChildElem->Value(),"AreaMap")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("x1")
			&& NULL!=pChildElem->Attribute("y1")
			&& NULL!=pChildElem->Attribute("x2")
			&& NULL!=pChildElem->Attribute("y2"))
		{
			tagArea stArea;
			stArea.lBorderID = 0;
			stArea.lParentWorldID = stWorldMap->lID;
			stArea.strName		= pChildElem->Attribute("name");
			stArea.lID			= atol(pChildElem->Attribute("id"));
			stArea.rect.left	= atol(pChildElem->Attribute("x1"));
			stArea.rect.top		= atol(pChildElem->Attribute("y1"));
			stArea.rect.right	= atol(pChildElem->Attribute("x2"));
			stArea.rect.bottom	= atol(pChildElem->Attribute("y2"));
			if(pChildElem->Attribute("border"))
				stArea.lBorderID = atol(pChildElem->Attribute("border"));
			if(InitAreaInfo(&stArea,pChildElem))
				stWorldMap->vecArea.push_back(stArea);
		}
		// 直属场景信息
		else if(!strcmp(pChildElem->Value(),"RegionMap")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("x1")
			&& NULL!=pChildElem->Attribute("y1")
			&& NULL!=pChildElem->Attribute("x2")
			&& NULL!=pChildElem->Attribute("y2"))
		{
			tagRegion stRegion;
			stRegion.lParentAreaID = 0;
			stRegion.lParentWorldID = stWorldMap->lID;
			stRegion.strName	 = pChildElem->Attribute("name");
			stRegion.lID		 = atol(pChildElem->Attribute("id"));
			stRegion.rect.left	 = atol(pChildElem->Attribute("x1"));
			stRegion.rect.top	 = atol(pChildElem->Attribute("y1"));
			stRegion.rect.right	 = atol(pChildElem->Attribute("x2"));
			stRegion.rect.bottom = atol(pChildElem->Attribute("y2"));
			// 王峰：区域特效图片 ID，如果没有配置为0，则等于 lParentAreaID
			stRegion.lIconID	= pChildElem->Attribute("MapIcon") ? atol( pChildElem->Attribute("MapIcon") ) : stRegion.lParentAreaID;

			// 直属场景
			if(InitRegionInfo(&stRegion,WORLDMAP_TYPE,stWorldMap,pChildElem))
				stWorldMap->m_mpRegions[stRegion.lID] = stRegion;
		}
	}
	return TRUE;
}

// 初始化大地图下的区域信息
BOOL CRegionList::InitAreaInfo(tagArea *stArea,TiXmlElement* pElem)
{
	if(!pElem||!stArea)
		return FALSE;
	// 遍历节点,解析该区域下的场景信息
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL;
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 区域信息
		if(!strcmp(pChildElem->Value(),"RegionMap")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("x1")
			&& NULL!=pChildElem->Attribute("y1")
			&& NULL!=pChildElem->Attribute("x2")
			&& NULL!=pChildElem->Attribute("y2"))
		{
			tagRegion stRegion;
			stRegion.lParentAreaID = stArea->lID;
			stRegion.lParentWorldID = stArea->lParentWorldID;
			stRegion.strName	 = pChildElem->Attribute("name");
			stRegion.lID		 = atol(pChildElem->Attribute("id"));
			stRegion.rect.left	 = atol(pChildElem->Attribute("x1"));
			stRegion.rect.top	 = atol(pChildElem->Attribute("y1"));
			stRegion.rect.right	 = atol(pChildElem->Attribute("x2"));
			stRegion.rect.bottom = atol(pChildElem->Attribute("y2"));
			// 王峰：区域特效图片 ID，如果没有配置为0，则等于 lParentAreaID
			stRegion.lIconID	= pChildElem->Attribute("MapIcon") ? atol( pChildElem->Attribute("MapIcon") ) : stRegion.lParentAreaID;

			if(InitRegionInfo(&stRegion,AREAMAP_TYPE,stArea,pChildElem))
				stArea->m_mpRegions[stRegion.lID] = stRegion;
		}
	}
	return TRUE;
}

// 初始化场景信息
BOOL CRegionList::InitRegionInfo(tagRegion *stRegion,MAP_TYPE lUpType,void *pUp, TiXmlElement* pElem)
{
	if(!pElem||!stRegion)
		return FALSE;
	// 遍历节点,解析该场景下的地点信息
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL;
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 子场景信息
		if(!strcmp(pChildElem->Value(),"RegionMap")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("x")
			&& NULL!=pChildElem->Attribute("y"))
		{
			tagRegion stChildRegion;
			stChildRegion.lParentRegionID	= stRegion->lID;
			stChildRegion.lParentAreaID		= stRegion->lParentAreaID;
			stChildRegion.lParentWorldID	= stRegion->lParentWorldID;
			stChildRegion.rect				= stRegion->rect;	
			stChildRegion.strName	 = pChildElem->Attribute("name");
			stChildRegion.lID		 = atol(pChildElem->Attribute("id"));
			stChildRegion.stPos.x	 = atol(pChildElem->Attribute("x"));
			stChildRegion.stPos.y	 = atol(pChildElem->Attribute("y"));
			// 王峰：区域特效图片 ID，如果没有配置为0，则等于 lParentAreaID
			stChildRegion.lIconID	= pChildElem->Attribute("MapIcon") ? atol( pChildElem->Attribute("MapIcon") ) : stRegion->lIconID;
			if(InitRegionInfo(&stChildRegion,lUpType,pUp,pChildElem))
			{
				switch(lUpType)
				{
				case REGIONMAP_TYPE:
				case AREAMAP_TYPE:
					{
						CRegionList::tagArea * pArea = (CRegionList::tagArea *)pUp;
						if(pArea)
							pArea->m_mpRegions[stChildRegion.lID] = stChildRegion;
					}
					break;
				case WORLDMAP_TYPE:
					{
						CRegionList::tagWorldMap * pWorldMap = (CRegionList::tagWorldMap *)pUp;
						if(pWorldMap)
							pWorldMap->m_mpRegions[stChildRegion.lID] = stChildRegion;
					}
					break;
				}
			}
		}
		// 场景下的地点信息
		if(!strcmp(pChildElem->Value(),"Address")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("x")
			&& NULL!=pChildElem->Attribute("y"))
		{
			tagAddressDetails tagAddrInfo;
			tagAddrInfo.strAddName  = pChildElem->Attribute("name");
			tagAddrInfo.stPos.x		= atol(pChildElem->Attribute("x"));
			tagAddrInfo.stPos.y		= atol(pChildElem->Attribute("y"));
			// 连接场景ID
			if(NULL!=pChildElem->Attribute("id"))
				tagAddrInfo.AddRegionId = atol(pChildElem->Attribute("id"));
			// 名称颜色
			if(NULL!=pChildElem->Attribute("color"))
				tagAddrInfo.color = CStrConverter::parseStringToDec(pChildElem->Attribute("color"));
			// 名称镶边颜色
			if(NULL!=pChildElem->Attribute("colorEmb"))
				tagAddrInfo.colorEmb = CStrConverter::parseStringToDec(pChildElem->Attribute("colorEmb"));
			stRegion->vecCurAddrDetails.push_back(tagAddrInfo);
		}
	}
	// 该场景下的路标点
	if(stRegion->vec_stRect.empty())
		InitRegionRectInfo(stRegion);
	return TRUE;
}


// 初始化场景下的禁行区域信息
BOOL CRegionList::InitRegionRectInfo(tagRegion *stRegion)
{
	if(!stRegion)
		return FALSE;
	//=================================================================//
	char str[256]="";
	sprintf_s(str,"data/RoadPoint/%d.xml",stRegion->lID);
	CRFile *prfile = rfOpen(str);
	if(NULL == prfile)
	{
		return FALSE;
	}
	TiXmlNode *pNode = NULL;
	TiXmlDocument m_Tdoc(str);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		MessageBox(NULL,str,"",MB_OK);
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("RoadPoint");
	if (pNode==NULL)
	{
		MessageBox(NULL,"No:'RoadPoint'Node","",MB_OK); 
		return FALSE;
	}
	TiXmlElement* pElem = pNode->ToElement();
	// 便利子节点，设置地图禁行区域信息
	for (TiXmlElement *pRegionElem=pElem->FirstChildElement(); pRegionElem!=NULL; 
		pRegionElem=pRegionElem->NextSiblingElement())
	{
		if(!strcmp(pRegionElem->Value(),"Region")
			&& NULL!=pRegionElem->Attribute("id"))
		{
			// 场景ID匹配
			long lID = atol(pRegionElem->Attribute("id"));
			if(lID == stRegion->lID)
			{
				stRegion->vec_stRect.clear();
				// 遍历节点,解析该区域下的禁行区域信息
				for (TiXmlElement *pChildElem=pRegionElem->FirstChildElement(); pChildElem!=NULL;
					pChildElem=pChildElem->NextSiblingElement())
				{
					// 该场景下的禁行区域
					if(!strcmp(pChildElem->Value(),"SignPos")
						&& NULL!=pChildElem->Attribute("x1")
						&& NULL!=pChildElem->Attribute("y1")
						&& NULL!=pChildElem->Attribute("x2")
						&& NULL!=pChildElem->Attribute("y2"))
					{
						RECT stRect;
						stRect.left		= atol(pChildElem->Attribute("x1"));
						stRect.top		= atol(pChildElem->Attribute("y1"));
						stRect.right	= atol(pChildElem->Attribute("x2"));
						stRect.bottom	= atol(pChildElem->Attribute("y2"));
						stRegion->vec_stRect.push_back(stRect);
					}
				}
			}
		}
	}
	return TRUE;
}

// 解析地图配置文件
BOOL CRegionList::LoadMap()
{
	if(m_lsWorldMap.size() == 0)
	{
		//=================================================================//
		CRFile *prfile = rfOpen("data/GameMapConfig.xml");
		if(NULL == prfile)
		{
			//输出错误信息
			MessageBox(NULL,"LoadFileErr:data/GameMapConfig.xml","",MB_OK);
			return FALSE;
		}
		TiXmlNode *pNode = NULL; 		
		TiXmlDocument m_Tdoc("data/GameMapConfig.xml");
		//载入技能配置文件
		if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
		{
			//输出错误信息
			MessageBox(NULL,"LoadFileErr:data/GameMapConfig.xml","",MB_OK);
			rfClose(prfile);
			return FALSE;
		}
		rfClose(prfile);
		//=================================================================//
		//找到根节点，如果无节点，输出错误提示
		pNode = m_Tdoc.FirstChild("Map");
		if (pNode==NULL)
		{
			MessageBox(NULL,"No:'Map'Node","",MB_OK); 
			return FALSE;
		}
		TiXmlElement* pElem = pNode->ToElement();
		// 便利子节点，设置地图信息
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			// 解析地图信息
			if(!strcmp(pChildElem->Value(),"WorldMap")
				&& NULL!=pChildElem->Attribute("name")
				&& NULL!=pChildElem->Attribute("id"))
			{
				tagWorldMap stWorldMap;
				stWorldMap.lID = atol(pChildElem->Attribute("id"));
				stWorldMap.strName = pChildElem->Attribute("name");
				if(InitWorldInfo(&stWorldMap,pChildElem))
					m_lsWorldMap.push_back(stWorldMap);
			}
		}
		return TRUE;
	}
	return FALSE;
}

// 查找指定场景到目标场景的场景路径
BOOL CRegionList::FindRegionPath(long lSourID,long lTargetID)
{
	m_lsRegionPathList.clear();
	m_lSouAreaID = 0;
	// 如果在同一个场景，返回
	if(lSourID == lTargetID)
		return FALSE;
	// 原始场景节点信息
	tagRegionPathInfo stSInfo;
	stSInfo.lRegionID = lSourID;
	// 找到指定场景
	tagRegion * pSourRegion = GetRegion(lSourID);
	tagRegion * pTargRegion = GetRegion(lTargetID);
	if(pSourRegion && pTargRegion)
	{
		if(g_bDebug)
		{
			Log4c::Trace(ROOT_MODULE,"跨场景开始：从%s->%s",pSourRegion->strName.c_str(),pTargRegion->strName.c_str());
		}
		// 区域相同
		if(pSourRegion->lParentAreaID == pTargRegion->lParentAreaID)
		{
			// 区域ID不为0，表示在本国搜索
			if(pSourRegion->lParentAreaID !=0)
				m_lSearchMode = REGION_REGION;
			// 区域ID为0，表示在世界地图的直属场景中查找
			else
				m_lSearchMode = WREGION_WREGION;
		}
		// 区域不同
		else
		{
			// 区域ID都不为0，跨国搜索（需要确定两个国家的出口和入口场景）
			if(pSourRegion->lParentAreaID != 0 && pTargRegion->lParentAreaID != 0)
				m_lSearchMode = AREA_AREA;
			// 源场景的区域ID不为0，目标场景区域ID为0，表示从一个国家到世界直属场景的查找（需要确定源场景的出口场景）
			else if(pSourRegion->lParentAreaID != 0 && pTargRegion->lParentAreaID == 0)
				m_lSearchMode = AREA_WREGION;
			// 源场景的区域ID为0，目标场景区域ID不为0，表示世界场景到某个国家的查找（需要确定目标场景的入口场景）
			else if(pSourRegion->lParentAreaID == 0 && pTargRegion->lParentAreaID != 0)
				m_lSearchMode = WREGION_AREA;
			else
				return FALSE;
		}
		// 记录起始区域
		m_lSouAreaID = pSourRegion->lParentAreaID;
		for (size_t i = 0; i < pSourRegion->vecCurAddrDetails.size(); ++i)
		{
			tagRegionPathInfo stPathInfo;
			stPathInfo.lRegionID = pSourRegion->vecCurAddrDetails[i].AddRegionId;
			stPathInfo.stPos = pSourRegion->vecCurAddrDetails[i].stPos;
			if (stPathInfo.lRegionID == 0)
				continue;
			if(stPathInfo.lRegionID == lTargetID)
			{
				m_lsRegionPathList.clear();
				// 删除原始场景节点
				m_lsRegionPathList.push_back(stPathInfo);
				return TRUE;
			}
			list<CRegionList::tagRegionPathInfo> RegionPathList;
			RegionPathList.push_back(stSInfo);
			if(AddRegionPath(RegionPathList,stPathInfo,pTargRegion))
			{
				RegionPathList.erase(RegionPathList.begin());
				if(m_lsRegionPathList.empty())
					m_lsRegionPathList = RegionPathList;
				else if(m_lsRegionPathList.size() > RegionPathList.size())
					m_lsRegionPathList = RegionPathList;
			}
			RegionPathList.clear();
		}
		// 找到最优路径
		if(!m_lsRegionPathList.empty())
		{
			if(m_lsRegionPathList.front().lRegionID == lSourID)
				m_lsRegionPathList.pop_front();
			if(g_bDebug)
			{
				list<tagRegionPathInfo>::iterator it = m_lsRegionPathList.begin();
				for (it; it != m_lsRegionPathList.end(); ++it)
				{
					Log4c::Trace(ROOT_MODULE,"添加场景ID：%d",(*it).lRegionID);
				}
				Log4c::Trace(ROOT_MODULE,"*****************");
			}
			return TRUE;
		}
	}
	return FALSE;
}

// 添加场景路径
BOOL CRegionList::AddRegionPath(list<CRegionList::tagRegionPathInfo> & lsRegionPath,tagRegionPathInfo stSourInfo,tagRegion * pTarget)
{
	if(!pTarget)
		return FALSE;
	static map<long,CRegionList::tagRegionPathInfo> SearchedPathList;		// 已经查找过的路径
	if(lsRegionPath.size() < 2)
	{
		SearchedPathList.clear();
		SearchedPathList[lsRegionPath.front().lRegionID] = lsRegionPath.front();
	}
	// 如果该起始场景已经被查找过了，且之前的索引小于当前节点索引，返回
	map<long,CRegionList::tagRegionPathInfo>::iterator ite = SearchedPathList.find(stSourInfo.lRegionID);
	if(ite != SearchedPathList.end())
	{
		if(ite->second.lIndex <= stSourInfo.lIndex)
			return FALSE;
	}
	stSourInfo.lIndex = (long)lsRegionPath.size();
	SearchedPathList[stSourInfo.lRegionID] = stSourInfo;
	lsRegionPath.push_back(stSourInfo);
	if(g_bDebug)
	{
		Log4c::Trace(ROOT_MODULE,"添加场景ID：%d",stSourInfo.lRegionID);
	}
	if(!m_lsRegionPathList.empty() && lsRegionPath.size() >= m_lsRegionPathList.size())
		return FALSE;
	// 求出起始和终点场景
	tagRegion * pSourRegion = GetRegion(stSourInfo.lRegionID);
	if(pSourRegion)
	{
		// 在起始场景的相邻场景去查找是否存在目标场景（深度搜索）
		for (size_t i = 0; i < pSourRegion->vecCurAddrDetails.size(); ++i)
		{
			tagRegionPathInfo stPathInfo;
			stPathInfo.lRegionID = pSourRegion->vecCurAddrDetails[i].AddRegionId;
			stPathInfo.stPos = pSourRegion->vecCurAddrDetails[i].stPos;
			stPathInfo.lIndex = (long)lsRegionPath.size();
			if (stPathInfo.lRegionID == 0)
			{
				continue;
			}
			if(stPathInfo.lRegionID == pTarget->lID)
			{
				if(g_bDebug)
				{
					Log4c::Trace(ROOT_MODULE,"**************找到目标场景ID：%d,中间场景数为：%d**************",stPathInfo.lRegionID,lsRegionPath.size());
				}
				// 如果该节点已存在，直接返回
				list<CRegionList::tagRegionPathInfo>::iterator it = lsRegionPath.begin();
				for (; it != lsRegionPath.end(); ++it)
				{
					if(it->lRegionID == pTarget->lID)
						return TRUE;
				}
				lsRegionPath.push_back(stPathInfo);
				return TRUE;
			}
			switch(m_lSearchMode)
			{
				// 本国查找，如果当前搜索的场景不是本国的场景了，返回
			case REGION_REGION:
				{
					if(pSourRegion->lParentAreaID != pTarget->lParentAreaID)
						return FALSE;
				}
				break;
				// 跨国查找，如果当前搜索的场景，不属于起始和目标场景，也不属于世界直属场景，返回
			case AREA_AREA:
				{
					if(pSourRegion->lParentAreaID != 0 &&
						pSourRegion->lParentAreaID != m_lSouAreaID && 
						pSourRegion->lParentAreaID != pTarget->lParentAreaID)
						return FALSE;
				}
				break;
				// 国->世界场景，如果当前搜索的场景，不属于世界直属场景，也不属于起始国家场景，返回
			case AREA_WREGION:
				{
					if(pSourRegion->lParentAreaID != 0 && pSourRegion->lParentAreaID != m_lSouAreaID)
						return FALSE;
				}
				break;
				// 世界场景->国，如果当前搜索的场景，不属于世界直属场景，也不属于目标国家场景，返回
			case WREGION_AREA:
				{
					if(pSourRegion->lParentAreaID != 0 && pSourRegion->lParentAreaID != pTarget->lParentAreaID)
						return FALSE;
				}
				break;
				// 世界场景->世界场景，如果当前这个场景已经属于某个国家，直接返回
			case WREGION_WREGION:
				{
					if(pSourRegion->lParentAreaID != 0)
						return FALSE;
				}
				break;
			}
			// 否则，将该相邻场景作为起始场景进行递归搜索
			list<tagRegionPathInfo>	RegionPathList = lsRegionPath;
			if(AddRegionPath(RegionPathList,stPathInfo,pTarget))
			{
				// 找到，更新路径，回溯到父节点继续查找
				lsRegionPath = RegionPathList;
				if(m_lsRegionPathList.empty())
					m_lsRegionPathList = lsRegionPath;
				else if(m_lsRegionPathList.size() > lsRegionPath.size())
					m_lsRegionPathList = lsRegionPath;
				lsRegionPath.pop_back();
				//return TRUE;
			}
			RegionPathList.clear();
		}
	}
	return FALSE;
}


/******************************************************************************
*  file: CMailBoxList.h
*
*  Brief:场景中的邮筒配置，标示了各个场景中的邮筒位置
*
*  Authtor:刘科
*	
*  Datae:2009-3-7
******************************************************************************/

map<long , CMailBoxPosList::tagMailBoxPos>	CMailBoxPosList::m_mapMailBoxPos;

CMailBoxPosList::CMailBoxPosList(void)
{
}

CMailBoxPosList::~CMailBoxPosList(void)
{
	ReleaseMailBoxPos();
}


// 释放邮筒坐标配置文件
void CMailBoxPosList::ReleaseMailBoxPos()
{
	m_mapMailBoxPos.clear();
}

// 解析地图配置文件
BOOL CMailBoxPosList::LoadMailBoxPos()
{
	m_mapMailBoxPos.clear();
	//=================================================================//
	CRFile *prfile = rfOpen("data/MailBoxPos.xml");
	if(NULL == prfile)
	{
		//输出错误信息
		MessageBox(NULL,"LoadFileErr:data/MailBoxPos.xml","",MB_OK);
		return FALSE;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc("data/MailBoxPos.xml");
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		MessageBox(NULL,"LoadFileErr:data/MailBoxPos.xml","",MB_OK);
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("Mail");
	if (pNode==NULL)
	{
		MessageBox(NULL,"No:'Mail'Node","",MB_OK); 
		return FALSE;
	}
	TiXmlElement* pElem = pNode->ToElement();
	// 便利子节点，设置信息
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 解析地图信息
		if(!strcmp(pChildElem->Value(),"MailBox")
			&& NULL!=pChildElem->Attribute("ResourceID")
			&& NULL!=pChildElem->Attribute("Name"))
		{
			tagMailBoxPos stMailBoxPos;
			stMailBoxPos.lResourceID = atol(pChildElem->Attribute("ResourceID"));
			stMailBoxPos.strName = pChildElem->Attribute("Name");
			if(InitMailPos(stMailBoxPos,pChildElem))
				m_mapMailBoxPos[stMailBoxPos.lResourceID] = stMailBoxPos;
		}
	}
	return TRUE;
}


// 初始化各个地图下的邮箱坐标
BOOL CMailBoxPosList::InitMailPos(tagMailBoxPos& stMailBoxPos,TiXmlElement* pElem)
{
	if(!pElem)
		return FALSE;
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL;
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 坐标信息
		if(!strcmp(pChildElem->Value(),"Pos")
			&& NULL!=pChildElem->Attribute("x")
			&& NULL!=pChildElem->Attribute("y"))
		{
			tagPos stPos;
			stPos.x = atol(pChildElem->Attribute("x"));
			stPos.y = atol(pChildElem->Attribute("y"));
			stMailBoxPos.vecPos.push_back(stPos);
		}
	}
	return TRUE;
}

// 获得指定场景中，离指定坐标最近的邮箱坐标
// 参数1：场景资源ID，参数2，参数3：指定的坐标
void CMailBoxPosList::GetMailBoxPos(long lResourceID,long sx,long sy,long& dx,long& dy)
{
	if(m_mapMailBoxPos.find(lResourceID) != m_mapMailBoxPos.end())
	{
		tagMailBoxPos stMailBoxPos = m_mapMailBoxPos[lResourceID];
		long lDist = 9999999;
		long index = -1;
		// 找出距离最近的点
		for (size_t i = 0; i < stMailBoxPos.vecPos.size(); ++i)
		{
			long lDis = Distance(sx,sy,stMailBoxPos.vecPos[i].x,stMailBoxPos.vecPos[i].y);
			if(lDis < lDist)
			{
				lDist = lDis;
				index = (long)i;
			}
		}
		if(index >= 0 && index < (long)stMailBoxPos.vecPos.size())
		{
			dx = stMailBoxPos.vecPos[index].x;
			dy = stMailBoxPos.vecPos[index].y;
		}
	}
}

/******************************************************************************
*  file:	CMartialPos.h
*
*  Brief:	军事据点配置，可根据此配置初始化大地图中的军事据点的信息以及显示
*
*  Authtor:	刘科
*
*  Datae:	2009-4-22
******************************************************************************/

vector<CMartialPos::tagBeachHead>	CMartialPos::m_listMartialPos;

CMartialPos::CMartialPos(void)
{}

CMartialPos::~CMartialPos(void)
{
	ReleaseMartialPos();
}

// 解析军事据点配置文件
BOOL CMartialPos::LoadMartialPos()
{
	m_listMartialPos.clear();
	//=================================================================//
	CRFile *prfile = rfOpen("data/MartialBeachHead.xml");
	if(NULL == prfile)
	{
		//输出错误信息
		MessageBox(NULL,"LoadFileErr:data/MartialBeachHead.xml","",MB_OK);
		return FALSE;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc("data/MartialBeachHead.xml");
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		MessageBox(NULL,"LoadFileErr:data/MartialBeachHead.xml","",MB_OK);
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("Martial");
	if (pNode==NULL)
	{
		MessageBox(NULL,"No:'Martial'Node","",MB_OK); 
		return FALSE;
	}
	TiXmlElement* pElem = pNode->ToElement();
	// 便利子节点，设置信息
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		// 解析地图信息
		if(!strcmp(pChildElem->Value(),"Area")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("name"))
		{
			tagBeachHead stBeachHead;
			stBeachHead.strAreaName = pChildElem->Attribute("Name");
			if(InitMartialPos(stBeachHead,pChildElem))
				m_listMartialPos.push_back(stBeachHead);
		}
	}
	return TRUE;
}

// 释放军事据点配置文件
void CMartialPos::ReleaseMartialPos()
{
	m_listMartialPos.clear();
}

// 初始化各个地图下的邮箱坐标
BOOL CMartialPos::InitMartialPos(CMartialPos::tagBeachHead& stBeachHead,TiXmlElement* pElem)
{
	if(!pElem)
		return FALSE;
	for (TiXmlElement *pChildElem = pElem->FirstChildElement(); pChildElem != NULL;
		pChildElem = pChildElem->NextSiblingElement())
	{
		// 坐标信息
		if(!strcmp(pChildElem->Value(),"name")
			&& NULL!=pChildElem->Attribute("id")
			&& NULL!=pChildElem->Attribute("x")
			&& NULL!=pChildElem->Attribute("y")
			&& NULL!=pChildElem->Attribute("desc"))
		{
			stBeachHead.strBeachHeadName = pChildElem->Attribute("name");
			stBeachHead.lBeachHeadID	 = atol(pChildElem->Attribute("id"));
			stBeachHead.lPosX			 = atol(pChildElem->Attribute("x"));
			stBeachHead.lPosY			 = atol(pChildElem->Attribute("y"));
			stBeachHead.strDesc			 = pChildElem->Attribute("desc");
		}
	}
	return TRUE;
}
