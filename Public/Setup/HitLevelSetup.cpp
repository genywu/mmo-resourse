#include "StdAfx.h"
#include "HitLevelSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
vector<HitLevelSetup::tagHitLevel> HitLevelSetup::m_vecHitLevel;

/// 构造
HitLevelSetup::HitLevelSetup(void)
{
}

/// 析构
HitLevelSetup::~HitLevelSetup(void)
{
}

/// 从文件加载连击等级配置
bool HitLevelSetup::LoadHitLevelSetup(const char* filename)
{
	m_vecHitLevel.clear();

	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_31", filename));
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

/// 编码
bool HitLevelSetup::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray, long(m_vecHitLevel.size()));
	vector<HitLevelSetup::tagHitLevel>::iterator iter = m_vecHitLevel.begin();
	for(; iter!=m_vecHitLevel.end(); ++iter)
	{
		_AddToByteArray(pByteArray, &(*iter), sizeof(tagHitLevel));
	}
	return true;
}

/// 解码
bool HitLevelSetup::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_vecHitLevel.clear();	

	tagHitLevel stSetup;
	long lSize = _GetLongFromByteArray(pByte, pos);
	for(long i=0; i<lSize; i++)
	{
		_GetBufferFromByteArray(pByte, pos, &stSetup, sizeof(tagHitLevel));
		m_vecHitLevel.push_back(stSetup);
	}
	return true;
}
