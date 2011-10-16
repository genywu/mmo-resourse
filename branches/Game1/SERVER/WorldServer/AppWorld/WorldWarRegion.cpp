//////////////////////////////////////////////////////////////////////////
//worldwarregion.cpp/
//战斗争夺场景的基类
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\worldwarregion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWorldWarRegion::CWorldWarRegion(void)
{

}

CWorldWarRegion::~CWorldWarRegion(void)
{
}

// 读取
BOOL CWorldWarRegion::Load()
{
	BOOL bRet = CWorldRegion::Load();
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.war", GetID());
	
	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		if(ReadTo(stream, "#"))
		{
			stream>>m_lSymbolTotalNum
				>>m_lWinVicSymbolNum
				>>m_lVicSymbolNum;
		}
	}
	return bRet;
}

// 读取
BOOL CWorldWarRegion::Save()
{
	return true;
}

// 添加到CByteArray
bool CWorldWarRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CWorldRegion::AddToByteArray(pByteArray, bExData);
	_AddToByteArray(pByteArray,m_lSymbolTotalNum);
	_AddToByteArray(pByteArray,m_lWinVicSymbolNum);
	_AddToByteArray(pByteArray,m_lVicSymbolNum);
	return true;
}
// 解码
bool CWorldWarRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CWorldRegion::DecordFromByteArray(pByte,pos,bExData);
	return true;
}
