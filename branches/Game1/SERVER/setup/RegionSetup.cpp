#include "StdAfx.h"
#include "regionsetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
map<long,CRegionSetup::tagRegionSetup> CRegionSetup::s_mapRegionSetup;

CRegionSetup::CRegionSetup()
{
}

CRegionSetup::~CRegionSetup()
{
}

bool CRegionSetup::LoadRegionSetup(const char* filename)
{
	s_mapRegionSetup.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(NULL, str, "error", MB_OK);
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

bool CRegionSetup::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,long(s_mapRegionSetup.size()));
	map<long,tagRegionSetup>::iterator iter=s_mapRegionSetup.begin();
	for(;iter!=s_mapRegionSetup.end();iter++)
	{
		tagRegionSetup *pRS = &iter->second;
		_AddToByteArray(pByteArray, pRS, sizeof(tagRegionSetup));
	}
	return true;
}

bool CRegionSetup::DecordFromByteArray(BYTE* pByte, long& pos)
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

//进入场景的等级条件
long CRegionSetup::GetEnterRegionLvl(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.lCanEnterLevel;
	}
	return 0;
}
//进入场景执行的脚本
const char* CRegionSetup::GetEnterRegionScript(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.pszEnterScript;
	}
	return NULL;
}


//退出场景执行的脚本
const char* CRegionSetup::GetExitRegionScript(long lRegionID)
{
	map<long,tagRegionSetup>::iterator it = s_mapRegionSetup.find(lRegionID);
	if( it != s_mapRegionSetup.end() )
	{
		return (*it).second.pszExitScript;
	}
	return NULL;
}
