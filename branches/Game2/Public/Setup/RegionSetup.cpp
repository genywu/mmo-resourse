#include "StdAfx.h"
#include "RegionSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<long,CRegionSetup::tagRegionSetup> CRegionSetup::s_mapRegionSetup;

/// 构造
CRegionSetup::CRegionSetup()
{
}

/// 析构
CRegionSetup::~CRegionSetup()
{
	s_mapRegionSetup.clear();
}

/// 从文件加载场景进入配置
bool CRegionSetup::LoadRegionSetup(const char* filename)
{
	s_mapRegionSetup.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		Log4c::Error(ROOT_MODULE, "%-15s file '%s' can't found!", __FUNCTION__,filename);
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	tagRegionSetup stRegionSetup;
	while(ReadTo(stream, "#"))
	{
		stream  >> stRegionSetup.lID
			>> stRegionSetup.lCanEnterLevel
			>> stRegionSetup.pszEnterScript
			>> stRegionSetup.pszExitScript;

		s_mapRegionSetup[stRegionSetup.lID] = stRegionSetup;
	}

	return true;
}

/// 编码
bool CRegionSetup::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray, long(s_mapRegionSetup.size()));
	map<long,tagRegionSetup>::iterator iter = s_mapRegionSetup.begin();
	for(; iter!=s_mapRegionSetup.end(); ++iter)
	{
		tagRegionSetup *pRS = &iter->second;
		_AddToByteArray(pByteArray, pRS, sizeof(tagRegionSetup));
	}
	return true;
}

/// 解码
bool CRegionSetup::DecordFromByteArray(uchar* pByte, long& pos)
{
	s_mapRegionSetup.clear();	

	tagRegionSetup stRegionSetup;
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long i=0; i<lSize; i++)
	{
		_GetBufferFromByteArray(pByte, pos, &stRegionSetup, sizeof(tagRegionSetup));
		s_mapRegionSetup[stRegionSetup.lID] = stRegionSetup;
	}
	return true;
}

/// 取得进入场景的等级条件
long CRegionSetup::GetEnterRegionLvl(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.lCanEnterLevel;
	}
	return 0;
}

/// 取得进入场景执行的脚本
const char* CRegionSetup::GetEnterRegionScript(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.pszEnterScript;
	}
	return NULL;
}


/// 取得退出场景执行的脚本
const char* CRegionSetup::GetExitRegionScript(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.pszExitScript;
	}
	return NULL;
}
