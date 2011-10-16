#include "StdAfx.h"
#include "dupliregionsetup.h"
#include "tools.h"
#include "readwrite.h"
#include "public.h"

CDupliRegionSetup::CDupliRegionSetup(void)
{
	m_DupliRegions.clear();
}

CDupliRegionSetup::~CDupliRegionSetup(void)
{
}


bool CDupliRegionSetup::Load()
{
	m_DupliRegions.clear();

	char filename[]="setup/DupliRegionsSetup.ini";

	ifstream stream;
	stream.open(filename);
	if( !stream.is_open() )
	{
		return false;
	}

	while(ReadTo(stream, "*"))
	{
		tagDupliRegion dupregion;	
		stream>>dupregion.lRegionID
			>>dupregion.lDupRegionID;
		m_DupliRegions.push_back(dupregion);
	}
	stream.close();
	return true;
}

// Ìí¼Óµ½CByteArray
bool CDupliRegionSetup::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, (long)m_DupliRegions.size());
	itDR it = m_DupliRegions.begin();
	for(;it != m_DupliRegions.end();it++)
	{
		_AddToByteArray(pByteArray, (BYTE*)&(*it), (long)sizeof(tagDupliRegion));
	}
	return true;
}

// ½âÂë
bool CDupliRegionSetup::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_DupliRegions.clear();
	int size = _GetLongFromByteArray(pByte, pos);
	for(int i=0;i<size;i++)
	{
		tagDupliRegion dupliregion;
		_GetBufferFromByteArray(pByte, pos, &dupliregion, (long)sizeof(tagDupliRegion));
		m_DupliRegions.push_back(dupliregion);
	}
	return true;
}

bool CDupliRegionSetup::IsDupliRegion(long lRegionID,long lDupliRegionID)
{
	itDR it = m_DupliRegions.begin();
	for(;it != m_DupliRegions.end();it++)
	{
		if(lRegionID == (*it).lRegionID &&
			lDupliRegionID == (*it).lDupRegionID)
			return true;
	}
	return false;
}

long CDupliRegionSetup::GetRandomRegion(long lRegionID)
{
	vector<long>	regions;regions.clear();
	regions.push_back(lRegionID);
	itDR it = m_DupliRegions.begin();
	for(;it != m_DupliRegions.end();it++)
	{
		if(lRegionID == (*it).lRegionID )
		{
			regions.push_back( (*it).lDupRegionID );
		}
	}
	long num = regions.size();
	num = random(num);
	return regions[num];
}