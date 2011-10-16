#include "stdafx.h"
#include "RegionPlugSetup.h"

extern HWND g_hWnd;
map<long, CRegionPlugSetup::tagRegionPlugSetup>	CRegionPlugSetup::m_mapRegionPlugSetup;

map<long, CRegionPlugSetup::tagRegionPlugSetup>& CRegionPlugSetup::GetRegionPlugSetupMap(void)
{
	return m_mapRegionPlugSetup;
}
BOOL CRegionPlugSetup::LoadRegionPlugSetup(const char* szPath)
{
	if(szPath == NULL) return FALSE;

	m_mapRegionPlugSetup.clear();
	CRFile* prfile = rfOpen(szPath);
	if(prfile == NULL)
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", szPath);
		MessageBox(g_hWnd, str, "Err", MB_OK);
		return FALSE;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	tagRegionPlugSetup tmpRgnPlugSetup;
	while( ReadTo(stream, "#") )
	{
		stream >> tmpRgnPlugSetup.lID
			>> tmpRgnPlugSetup.bType
			>> tmpRgnPlugSetup.lRegionID
			>> tmpRgnPlugSetup.lX1
			>> tmpRgnPlugSetup.lY1
			>> tmpRgnPlugSetup.lX2
			>> tmpRgnPlugSetup.lY2;
		m_mapRegionPlugSetup[tmpRgnPlugSetup.lID] =tmpRgnPlugSetup;
	}

	return TRUE;
}
void CRegionPlugSetup::Release()
{
	m_mapRegionPlugSetup.clear();
}

bool CRegionPlugSetup::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, (long)m_mapRegionPlugSetup.size());// num
	map<long, tagRegionPlugSetup>::iterator itr = m_mapRegionPlugSetup.begin();
	for(; itr!=m_mapRegionPlugSetup.end(); itr++)
	{
		_AddToByteArray(pByteArray, &(itr->second), sizeof(tagRegionPlugSetup));
	}
	return true;
}
bool CRegionPlugSetup::DecordFromByteArray(BYTE* pByte, long& pos)
{
	m_mapRegionPlugSetup.clear();
	long num = _GetLongFromByteArray(pByte, pos);
	for(int i=0; i<num; i++)
	{
		tagRegionPlugSetup tmpRgnPlugSetup;
		_GetBufferFromByteArray(pByte, pos, &tmpRgnPlugSetup, sizeof(tagRegionPlugSetup));
		m_mapRegionPlugSetup[tmpRgnPlugSetup.lID] = tmpRgnPlugSetup;
	}
	return true;
}