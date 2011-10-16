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

CCountryParam::CCountryParam(void)
{
}

CCountryParam::~CCountryParam(void)
{
}

bool CCountryParam::Initialize()
{
	return Load();
}

void CCountryParam::Release()
{
	SAFE_DELETE(instance);
}

bool CCountryParam::Load()
{
	m_mpStartRegions.clear();
	m_mpStartRect.clear();
	m_mpStartDir.clear();
	m_mpMainRegions.clear();
	m_mpMainRect.clear();
	m_mpMainDir.clear();
	m_mpJailVillageRegions.clear();// 流放村场景
	m_mpJailVillageRect.clear();// 流放村坐标

	m_mpJailIslandRegions.clear();// 流放岛场景
	m_mpJailIslandRect.clear();// 流放岛坐标

	char filename[] = "data/CountryParam.ini";
	
	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		Log4c::Warn(ROOT_MODULE,FormatText("WS_COUNTRY_0", filename));
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strTemp;
	stream >> strTemp >> m_lMaxCountyrs
		>> strTemp >> m_lMaxCountryPower;

	int id;
	long region,dir,camp;
	RECT rect;
	while(ReadTo(stream, "*"))
	{
		stream >> id
			>> region
			>> camp
			>> rect.left
			>> rect.top
			>> rect.right
			>> rect.bottom
			>> dir;
		m_mpStartRegions[id] = region;
		m_mpStartRect[id] = rect;
		m_mpStartDir[id] = dir;
		m_mpStartCamp[id] = camp;

		stream >> region
			>> rect.left
			>> rect.top
			>> rect.right
			>> rect.bottom
			>> dir;
		m_mpMainRegions[id] = region;
		m_mpMainRect[id] = rect;
		m_mpMainDir[id] = dir;

		stream >> region
			>> rect.left
			>> rect.top
			>> rect.right
			>> rect.bottom
			>> dir;
		m_mpJailVillageRegions[id] = region;// 流放村场景
		m_mpJailVillageRect[id] = rect;// 流放村坐标
		m_mpJailVillageDir[id] = dir;// 流放村方向

		stream >> region
			>> rect.left
			>> rect.top
			>> rect.right
			>> rect.bottom
			>> dir;
		m_mpJailIslandRegions[id] = region;// 流放岛场景
		m_mpJailIslandRect[id] = rect;// 流放岛坐标
		m_mpJailIslandDir[id] = dir;// 流放岛方向
	}

	return true;
}

// 添加到CByteArray
bool CCountryParam::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, m_lMaxCountyrs);
	_AddToByteArray(pByteArray, m_lMaxCountryPower);

	_AddToByteArray(pByteArray, (long)m_mpMainRegions.size());
	for( map<BYTE,long>::iterator it = m_mpMainRegions.begin(); it!=m_mpMainRegions.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}

	_AddToByteArray(pByteArray, (long)m_mpMainRect.size());
	for( map<BYTE,RECT>::iterator it = m_mpMainRect.begin(); it!=m_mpMainRect.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray(pByteArray, &(it->second), sizeof(RECT));
	}

	_AddToByteArray(pByteArray, (long)m_mpMainDir.size());
	for( map<BYTE,long>::iterator it = m_mpMainDir.begin(); it!=m_mpMainDir.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}

	//////////////////////////////////////////////////////////////////////////
	_AddToByteArray(pByteArray, (long)m_mpJailVillageRegions.size());
	for( map<BYTE,long>::iterator it = m_mpJailVillageRegions.begin(); it!=m_mpJailVillageRegions.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}

	_AddToByteArray(pByteArray, (long)m_mpJailVillageRect.size());
	for( map<BYTE,RECT>::iterator it = m_mpJailVillageRect.begin(); it!=m_mpJailVillageRect.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray(pByteArray, &(it->second), sizeof(RECT));
	}

	_AddToByteArray(pByteArray, (long)m_mpJailVillageDir.size());
	for( map<BYTE,long>::iterator it = m_mpJailVillageDir.begin(); it!=m_mpJailVillageDir.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}
	//////////////////////////////////////////////////////////////////////////
	_AddToByteArray(pByteArray, (long)m_mpJailIslandRegions.size());
	for( map<BYTE,long>::iterator it = m_mpJailIslandRegions.begin(); it!=m_mpJailIslandRegions.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}

	_AddToByteArray(pByteArray, (long)m_mpJailIslandRect.size());
	for( map<BYTE,RECT>::iterator it = m_mpJailIslandRect.begin(); it!=m_mpJailIslandRect.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray(pByteArray, &(it->second), sizeof(RECT));
	}

	_AddToByteArray(pByteArray, (long)m_mpJailIslandDir.size());
	for( map<BYTE,long>::iterator it = m_mpJailIslandDir.begin(); it!=m_mpJailIslandDir.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}

	_AddToByteArray(pByteArray,(long)m_mpStartCamp.size());
	for( map<BYTE,long>::iterator it = m_mpStartCamp.begin(); it!=m_mpStartCamp.end(); ++it)
	{
		_AddToByteArray( pByteArray, it->first );
		_AddToByteArray( pByteArray, it->second );
	}
	return true;
}

// 解码
bool CCountryParam::DecordFromByteArray(BYTE* pByte, long& pos)
{
	//
	//_AddToByteArray(pByteArray, m_lMaxCountyrs);
	//_AddToByteArray(pByteArray, m_lMaxCountryPower);
	//
	//m_vcMainRegions.clear();
	//m_vcMainRect.clear();
	//m_vcMainDir.clear();
	//long s;
	//long temp;
	//RECT rect;
	//_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	//for(int i=0; i<s; ++i)
	//{
	//	_GetBufferFromByteArray(pByte, pos, &temp, sizeof(long) );
	//	m_vcMainRegions.push_back(temp);
	//}

	//_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	//for(int i=0; i<s; ++i)
	//{
	//	_GetBufferFromByteArray(pByte, pos, &rect, sizeof(long) );
	//	m_vcMainRect.push_back(rect);
	//}

	//_GetBufferFromByteArray(pByte, pos, &s, sizeof(long) );
	//for(int i=0; i<s; ++i)
	//{
	//	_GetBufferFromByteArray(pByte, pos, &temp, sizeof(long) );
	//	m_vcMainDir.push_back(temp);
	//}


	return true;
}
