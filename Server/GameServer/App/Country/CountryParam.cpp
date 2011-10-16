#include "StdAfx.h"
#include "Countryparam.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Create Time:2005.3.15

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CCountryParam* CCountryParam::instance = NULL;

// 复活相关
map<BYTE,long>	CCountryParam::m_mpMainRegions;// 主城场景
map<BYTE,RECT>	CCountryParam::m_mpMainRect;// 主城坐标
map<BYTE,long>	CCountryParam::m_mpMainDir;// 主城方向
map<BYTE,long>  CCountryParam::m_mpStartCamp;//阵营编号

map<BYTE,long>	CCountryParam::m_mpJailVillageRegions;// 流放村场景
map<BYTE,RECT>	CCountryParam::m_mpJailVillageRect;// 流放村坐标
map<BYTE,long>	CCountryParam::m_mpJailVillageDir;// 流放村方向

map<BYTE,long>	CCountryParam::m_mpJailIslandRegions;// 流放岛场景
map<BYTE,RECT>	CCountryParam::m_mpJailIslandRect;// 流放岛坐标
map<BYTE,long>	CCountryParam::m_mpJailIslandDir;// 流放岛方向

long CCountryParam::m_lMaxCountyrs = 0;
long CCountryParam::m_lMaxCountryPower = 0;

CCountryParam::CCountryParam(void)
{
}

CCountryParam::~CCountryParam(void)
{
}

bool CCountryParam::Initialize()
{
	return true;
}

void CCountryParam::Release()
{
	SAFE_DELETE(instance);
}

// 添加到CByteArray
bool CCountryParam::AddToByteArray(vector<BYTE>* pByteArray)
{
	//_AddToByteArray(pByteArray, (long)m_vcMainRegions.size());
	//for(int i=0; i<(int)m_vcMainRegions.size(); ++i)
	//{
	//	_AddToByteArray(pByteArray, m_vcMainRegions[i]);
	//}

	//_AddToByteArray(pByteArray, (long)m_vcMainRect.size());
	//for(int i=0; i<(int)m_vcMainRect.size(); ++i)
	//{
	//	_AddToByteArray(pByteArray, &m_vcMainRect[i], sizeof(RECT));
	//}

	//_AddToByteArray(pByteArray, (long)m_vcMainDir.size());
	//for(int i=0; i<(int)m_vcMainDir.size(); ++i)
	//{
	//	_AddToByteArray(pByteArray, m_vcMainDir[i]);
	//}
	return true;
}
//根据同盟ID获取国家列表
void	CCountryParam::GetCountryList(long lCampID,list<BYTE> &lCountryID)
{
	map<BYTE,long>::iterator it =  m_mpStartCamp.begin();
	for (; it!=m_mpStartCamp.end(); ++it)
	{
		if (it->second == lCampID)
		{
			lCountryID.push_back(it->first);
		}
	}
}

// 解码
bool CCountryParam::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_lMaxCountyrs = _GetLongFromByteArray(pByte, pos);
	m_lMaxCountryPower= _GetLongFromByteArray(pByte, pos);

	m_mpMainRegions.clear();
	m_mpMainRect.clear();
	m_mpMainDir.clear();
	BYTE id;
	long s;
	long temp;
	RECT rect;
	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);
		//_GetBufferFromByteArray(pByte, pos, &temp, sizeof(long) );
		m_mpMainRegions[id] = temp;
	}

	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		_GetBufferFromByteArray(pByte, pos, &rect, sizeof(RECT) );
		m_mpMainRect[id] = rect;
	}


	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);		
		m_mpMainDir[id] = temp;
	}

	//////////////////////////////////////////////////////////////////////////
	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);
		//_GetBufferFromByteArray(pByte, pos, &temp, sizeof(long) );
		m_mpJailVillageRegions[id] = temp;
	}

	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		_GetBufferFromByteArray(pByte, pos, &rect, sizeof(RECT) );
		m_mpJailVillageRect[id] = rect;
	}
	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);		
		m_mpJailVillageDir[id] = temp;
	}
	//////////////////////////////////////////////////////////////////////////
	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);
		//_GetBufferFromByteArray(pByte, pos, &temp, sizeof(long) );
		m_mpJailIslandRegions[id] = temp;
	}

	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		_GetBufferFromByteArray(pByte, pos, &rect, sizeof(RECT) );
		m_mpJailIslandRect[id] = rect;
	}
	
	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);		
		m_mpJailIslandDir[id] = temp;
	}
	//////////////////////////////////////////////////////////////////////////

	_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	for(int i=0; i<s; ++i)
	{
		id = _GetByteFromByteArray(pByte, pos);
		temp = _GetLongFromByteArray(pByte, pos);		
		m_mpStartCamp[id] = temp;
	}



	return true;
}


// 主城相关属性接口
long CCountryParam::GetCountryMainRegionID(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpMainRegions.find(byCountryID);
		if(itr != instance->m_mpMainRegions.end())
		{
			return itr->second;
		}
	}
	return 0;
}
long CCountryParam::GetCountryMainRectLeft(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpMainRect.find(byCountryID);
		if(itr != instance->m_mpMainRect.end())
		{
			return itr->second.left;
		}
	}
	return 0;
}
long CCountryParam::GetCountryMainRectRight(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpMainRect.find(byCountryID);
		if(itr != instance->m_mpMainRect.end())
		{
			return itr->second.right;
		}
	}
	return 0;
}
long CCountryParam::GetCountryMainRectBottom(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpMainRect.find(byCountryID);
		if(itr != instance->m_mpMainRect.end())
		{
			return itr->second.bottom;
		}
	}
	return 0;
}
long CCountryParam::GetCountryMainRectTop(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpMainRect.find(byCountryID);
		if(itr != instance->m_mpMainRect.end())
		{
			return itr->second.top;
		}
	}
	return 0;
}
long CCountryParam::GetCountryMainDir(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpMainDir.find(byCountryID);
		if(itr != instance->m_mpMainDir.end())
		{
			return itr->second;
		}
	}
	return 0;
}
long CCountryParam::GetCountryStartCamp(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpStartCamp.find(byCountryID);
		if(itr != instance->m_mpStartCamp.end())
		{
			return itr->second;
		}
	}
	return 0;
}

// 流放村相关属性接口
long CCountryParam::GetCountryJailVillageRegionID(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpJailVillageRegions.find(byCountryID);
		if(itr != instance->m_mpJailVillageRegions.end())
		{
			return itr->second;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailVillageRectLeft(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailVillageRect.find(byCountryID);
		if(itr != instance->m_mpJailVillageRect.end())
		{
			return itr->second.left;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailVillageRectRight(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailVillageRect.find(byCountryID);
		if(itr != instance->m_mpJailVillageRect.end())
		{
			return itr->second.right;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailVillageRectBottom(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailVillageRect.find(byCountryID);
		if(itr != instance->m_mpJailVillageRect.end())
		{
			return itr->second.bottom;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailVillageRectTop(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailVillageRect.find(byCountryID);
		if(itr != instance->m_mpJailVillageRect.end())
		{
			return itr->second.top;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailVillageDir(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpJailVillageDir.find(byCountryID);
		if(itr != instance->m_mpJailVillageDir.end())
		{
			return itr->second;
		}
	}
	return 0;
}

// 流放岛相关属性接口
long CCountryParam::GetCountryJailIsLandRegionID(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpJailIslandRegions.find(byCountryID);
		if(itr != instance->m_mpJailIslandRegions.end())
		{
			return itr->second;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailIsLandRectLeft(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailIslandRect.find(byCountryID);
		if(itr != instance->m_mpJailIslandRect.end())
		{
			return itr->second.left;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailIsLandRectRight(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailIslandRect.find(byCountryID);
		if(itr != instance->m_mpJailIslandRect.end())
		{
			return itr->second.right;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailIsLandRectBottom(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailIslandRect.find(byCountryID);
		if(itr != instance->m_mpJailIslandRect.end())
		{
			return itr->second.bottom;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailIsLandRectTop(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,RECT>::iterator itr = instance->m_mpJailIslandRect.find(byCountryID);
		if(itr != instance->m_mpJailIslandRect.end())
		{
			return itr->second.top;
		}
	}
	return 0;
}
long CCountryParam::GetCountryJailIsLandDir(BYTE byCountryID)
{
	if(instance)
	{
		map<BYTE,long>::iterator itr = instance->m_mpJailIslandDir.find(byCountryID);
		if(itr != instance->m_mpJailIslandDir.end())
		{
			return itr->second;
		}
	}
	return 0;
}