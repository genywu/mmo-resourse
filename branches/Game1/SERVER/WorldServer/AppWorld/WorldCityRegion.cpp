///////////////////////////////////////////////////////////////
//worldregion.cpp///////////////////////////////////////////////
//攻城战地图类，负责攻城战的处理
//Author：Joe
//Create Date:2004.7.19
#include "StdAfx.h"
#include ".\worldcityregion.h"
#include "..\..\..\nets\networld\message.h"
#include "Organizingsystem\AttackCitySys.h"

#include "GlobalRgnManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWorldCityRegion::CWorldCityRegion(void)
{
	m_lSymbolTotalNum=3;
	m_lWinVicSymbolNum=3;
	m_lVicSymbolNum=2;
}

CWorldCityRegion::~CWorldCityRegion(void)
{
}

// 读取
BOOL CWorldCityRegion::Load()
{
	BOOL bRet = CWorldWarRegion::Load();
	if(bRet)
		bRet = LoadCitySetup();
	////检查复活点配置
	//if(m_stSetup.bUse == false ||
	//	m_stSetup.ReturnRegionID != GetID() ||
	//	m_DefenceSideRS.ReturnRegionID != GetID() )
	//{
	//	return false;
	//}
	return bRet;
}


//读取城市配置
BOOL CWorldCityRegion::LoadCitySetup()
{
	// 读新的
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.city", GetID());

	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		// 读入城门配置
		char str[256]="";
		m_CityGates.clear();
		while(1)
		{
			stream >> str;
			if ( strcmp(str,"<end>") == 0 || stream.eof())
			{
				break;
			}
			if ( strcmp(str,"#") == 0 )
			{
				tagBuild CityGate;
				stream >> CityGate.lID
					>> CityGate.strName
					>> CityGate.lPicID
					>> CityGate.lDir
					>> CityGate.lAction
					>> CityGate.lMaxHP
					>> CityGate.lDef
					>> CityGate.lElementResistant
					>> CityGate.lTitleX
					>> CityGate.lTitleY
					>> CityGate.lWidthInc
					>> CityGate.lHeightInc
					>> CityGate.strScript;
				m_CityGates.push_back(CityGate);
			}
		}

		// 读入防守方复活点配置
		while(1)
		{
			stream >> str;
			if ( strcmp(str,"<end>") == 0 || stream.eof())
			{
				break;
			}
			////if (strcmp(str,"#") == 0)
			////{
			////	stream >> m_DefenceSideRS.ReturnRegionID
			////		>> m_DefenceSideRS.rtReturnPoint.left
			////		>> m_DefenceSideRS.rtReturnPoint.top
			////		>> m_DefenceSideRS.rtReturnPoint.right
			////		>> m_DefenceSideRS.rtReturnPoint.bottom;
			////}
		}
		return true;
	}
	return false;
}

//保存城市配置
BOOL CWorldCityRegion::Save()
{
	return true;
}

// 添加到CByteArray
bool CWorldCityRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CWorldWarRegion::AddToByteArray(pByteArray, bExData);
	
	//添加防守方复活区域
	_AddToByteArray(pByteArray, (BYTE*)&m_DefenceSideRS, sizeof(tagRegionSetup));
	//添加城门信息
	_AddToByteArray(pByteArray, (long)m_CityGates.size());
	list<tagBuild>::iterator it = m_CityGates.begin();
	for(;it != m_CityGates.end();it++)
	{
		_AddToByteArray(pByteArray, (BYTE*)&(*it), sizeof(tagBuild)-sizeof(string)*2);
		_AddToByteArray(pByteArray, (char*)(*it).strName.c_str());
		_AddToByteArray(pByteArray, (char*)(*it).strScript.c_str());
	}
	return true;
}
// 解码
bool CWorldCityRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CWorldWarRegion::DecordFromByteArray(pByte,pos,bExData);
	return true;
}


void CWorldCityRegion::SetEnterPosXY(CPlayer*pPlayer)
{
//	eCityState citystate = GetAttackCitySys()->GetCityState(GetID());
//	if( citystate != CIS_Fight && citystate != CIS_Mass ) return;
//
//	long lTileX = static_cast<long>(pPlayer->GetPosX());
//	long lTileY = static_cast<long>(pPlayer->GetPosY());
//	tagCell* pCell = GetCell(lTileX,lTileY);
//
//	CGUID RegionID = NULL_GUID;
//	long sx = 0;
//	long sy = 0;
//	long w =  0;
//	long h =  0;
//
//	if( NULL_GUID != pPlayer->GetFactionID() && pPlayer->GetFactionID() == GetOwnedCityFaction() )
//	{
////		RegionID = m_DefenceSideRS.ReturnRegionID;
//		sx = m_DefenceSideRS.rtReturnPoint.left;
//		sy = m_DefenceSideRS.rtReturnPoint.top;
//		w =  m_DefenceSideRS.rtReturnPoint.right - sx;
//		h =  m_DefenceSideRS.rtReturnPoint.bottom - sy;
//	}
//	else
//	{
//		long rgnID, l, t, r, b, d;
//		l = t = r = b = d = -1;
//		GetReturnPoint(pPlayer,rgnID , l, t, r ,b ,d);
//		long sx = l;
//		long sy = t;
//		long w = r - sx;
//		long h = b - sy;
//
//		long GSID = GetGame()->GetGlobalRgnManager()->FindGSIDByRgnID(RegionID);
//		RegionID = GetGame()->GetGlobalRgnManager()->GetGSRgnManager().find(GSID);
//	}
//	
//
//	long GSID = GetGame()->GetGlobalRgnManager()->FindGSIDByRgnID(RegionID);
//
//	CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindGSRgnByGUID(GSID, RegionID);
//		
//	if(rgn != NULL)
//		rgn->GetRandomPosInRange(lTileX, lTileY, sx, sy, w, h);
//	
//	pPlayer->SetRegionExID(RegionID);
//	pPlayer->SetPosXY(lTileX+0.5f,lTileY+0.5f);
}
