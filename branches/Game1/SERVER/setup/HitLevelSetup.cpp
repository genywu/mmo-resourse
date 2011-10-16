#include "StdAfx.h"
#include ".\hitlevelsetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

vector<CHitLevelSetup::tagHitLevel> CHitLevelSetup::m_vecHitLevel;

CHitLevelSetup::CHitLevelSetup(void)
{
}

CHitLevelSetup::~CHitLevelSetup(void)
{
}

// 从文件读取原始物品列表
bool CHitLevelSetup::LoadHitLevelSetup(const char* filename)
{
	m_vecHitLevel.clear();

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

	tagHitLevel stHitLevel;
	while(ReadTo(stream, "*"))
	{
		stream  >> stHitLevel.dwLevel
				>> stHitLevel.dwHit
				>> stHitLevel.dwExp;
		m_vecHitLevel.push_back(stHitLevel);
	}

	return true;
}

bool CHitLevelSetup::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,long(m_vecHitLevel.size()));
	vector<CHitLevelSetup::tagHitLevel>::iterator iter=m_vecHitLevel.begin();
	for(;iter!=m_vecHitLevel.end();iter++)
	{
		_AddToByteArray(pByteArray,&(*iter),sizeof(tagHitLevel));
	}
	return true;
}

bool CHitLevelSetup::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_vecHitLevel.clear();	

	// 怪物属性
	tagHitLevel stSetup;
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long i=0; i<lSize; i++)
	{
		_GetBufferFromByteArray(pByte, pos, &stSetup, sizeof(tagHitLevel));
		m_vecHitLevel.push_back(stSetup);
	}
	return true;
}
