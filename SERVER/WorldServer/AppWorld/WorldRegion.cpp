#include "StdAfx.h"
#include ".\worldregion.h"
#include "..\..\..\nets\networld\message.h"
#include "../setup/GlobeSetup.h"

//#include "..\dbaccess\worlddb\RsRegion.h"
#include "Organizingsystem\OrganizingCtrl.h"
#include "Country\CountryParam.h"
#include "script\VariableList.h"
#include "..\..\setup\MonsterList.h"
#include "..\..\setup\GateList.h"
#include "GlobalRgnManager.h"



long g_lTotalMonster = 0;

CWorldRegion::CWorldRegion(void)
: CRegion(),m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),m_CurDupRgnPlayerNum(0),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_bIsTemplateRgnFlag(false)
{
	m_Gsid = -1;

	memset( &m_Param,0,sizeof(tagRegionParam) );
	
	
	m_dwAreaId=0;
	m_bNoPk = 0;
	m_bNoContribute = false;
	//场景的空间类型
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;
	m_pVariableList = MP_NEW CVariableList;
	//原地复活标志符
	m_bRelive = false;
	//下线回城标志符，0表示死亡回城，1表示非死亡下线回城
	m_bDeadReturn = false;
	m_bStrongPintFlag = 0;
}

CWorldRegion::~CWorldRegion(void)
{
	if(m_pVariableList)
		MP_DELETE(m_pVariableList);
}
CVariableList* CWorldRegion::GetVariableList(void)
{
	return m_pVariableList;
}
void CWorldRegion::SetVariableList(CVariableList* vl)
{
	if(vl==NULL)
		return;

	m_pVariableList->Release();

	*m_pVariableList = *vl;
}
void CWorldRegion::UpdateVariableList(CVariableList* vl)
{
	if(vl==NULL)
		return ;

//	m_pVariableList->UpdateVarList(vl->GetVarList());
}
void CWorldRegion::AI()
{
}

// 获取指定index的Monster分布
CWorldRegion::tagMonster* CWorldRegion::GetMonster(long lIndex)
{
    map<DWORD,tagMonster>::iterator it=m_mpMonster.find(lIndex);
    if (it!=m_mpMonster.end())
    {
        return & ((*it).second);
    }

	//for(list<tagMonster>::iterator it=m_listMonster.begin(); it!=m_listMonster.end(); it++)
	//{
	//	if ((*it).lIndex == lIndex)
	//	{
	//		return &(*it);
	//	}
	//}
	return NULL;
}

// 读取地图的Monster分布
BOOL CWorldRegion::LoadMonsterList()
{
	// 读新的
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.Monster", GetSvrResourceID());

	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		m_mpMonster.clear();

		string str;

		// 读入怪物分布区域
		while(1)
		{
			stream >> str;
			if (str=="<end>" || stream.eof())
			{
				break;
			}

			if (str=="#")
			{
				tagMonster st;
				stream >> st.lIndex
					>> st.rectRange.left 
					>> st.rectRange.top
					>> st.rectRange.right
					>> st.rectRange.bottom
					>> st.lNum
					>> st.lResetTime
					>> st.lStartTime
					>> st.lDir;

				if (st.lNum>1)
				{
					st.lNum *= CGlobeSetup::GetSetup()->fMonsterNumScale;
					if (st.lNum==0 )
						st.lNum = 1;
				}

				st.lResetTime *= 1000;
				st.lStartTime *= 1000;

                m_mpMonster[st.lIndex] = st;

				g_lTotalMonster += st.lNum;
			}
		}

		// 读每个刷新区域的刷新列表
		while(1)
		{
			stream >> str;
			if (str=="<end>" || stream.eof())
			{
				break;
			}

			if (str=="id")
			{
				long lID = 0;
				stream >> lID;

				tagMonster* pMonster = GetMonster(lID);
				if (pMonster==NULL)
				{
					continue;
				}

				WORD wLastOdds = 0;
				while(1)
				{					
					stream >> str;
					if (str=="<end>" || stream.eof())
					{
						break;
					}

					if (str=="#")
					{
						tagMonsterList stMonsterList;
						stream >> stMonsterList.strName
							>> stMonsterList.wOdds
							>> stMonsterList.wSign
							>> stMonsterList.wLeaderSign
							>> stMonsterList.wLeaderDistance
							>> stMonsterList.wRoadIndex
							>> stMonsterList.strScript
							>> stMonsterList.strTalkScript;

						ReplaceLine((char*)stMonsterList.strScript.c_str());
						strlwr((char*)stMonsterList.strScript.c_str());

						stMonsterList.wOdds += wLastOdds;
						wLastOdds = stMonsterList.wOdds;

						pMonster->vectorMonsterList.push_back(stMonsterList);
					}
				}
			}
		}

		return true;
	}
	return false;
}

BOOL CWorldRegion::LoadCollectionList()					// 读取地图的Collection分布
{
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.Collection", GetSvrResourceID());

	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
		return FALSE;

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	m_mapCollection.clear();

	string str;
	/////////
	while(1)
	{
		stream >> str;
		if (str=="<end>" || stream.eof())
		{
			break;
		}

		if (str=="#")
		{
			tagCollection st;
			stream >> st.lIndex
				>> st.rectRange.left 
				>> st.rectRange.top
				>> st.rectRange.right
				>> st.rectRange.bottom
				>> st.lNum
				>> st.lResetTime
				>> st.lStartTime
				>> st.lDir;

			if (st.lNum>1)
			{
				st.lNum *= CGlobeSetup::GetSetup()->fMonsterNumScale;
				if (st.lNum==0 )
					st.lNum = 1;
			}

			st.lResetTime *= 1000;
			st.lStartTime *= 1000;

			//m_mapCollection.push_back(st);
			map<long,tagCollection>::iterator it=m_mapCollection.find(st.lIndex);
			if (it== m_mapCollection.end())
			{
				m_mapCollection[st.lIndex]=st;
			}

			g_lTotalMonster += st.lNum;
		}
	}

	//
	// 读每个刷新区域的刷新列表
	while(1)
	{
		stream >> str;
		if (str=="<end>" || stream.eof())
		{
			break;
		}

		if (str=="id")
		{
			long lID = 0;
			stream >> lID;

			tagCollection* pCollection = GetCollection(lID);
			if (pCollection==NULL)
			{
				continue;
			}

			WORD wLastOdds = 0;
			while(1)
			{					
				stream >> str;
				if (str=="<end>" || stream.eof())
				{
					break;
				}

				if (str=="#")
				{
					tagCollectionList stCollectionList;
					string name;
					stream>> name;
					stream>> stCollectionList.lOdds;
					if (name.size()<48)
					{
						memcpy(stCollectionList.cName,name.c_str(),name.size()+1);
					}
					else
					{
						memcpy(stCollectionList.cName,name.c_str(),48);
						stCollectionList.cName[48]='\0';
					}
					//stream >> stCollectionList.strName
					//	>> stCollectionList.wOdds
					//	>> stCollectionList.wSign
					//	>> stCollectionList.wLeaderSign
					//	>> stCollectionList.wLeaderDistance
					//	>> stCollectionList.wRoadIndex
					//	>> stCollectionList.strScript
					//	>> stCollectionList.strTalkScript;

					//ReplaceLine((char*)stMonsterList.strScript.c_str());
					//strlwr((char*)stMonsterList.strScript.c_str());
					stCollectionList.lOdds+= wLastOdds;
					wLastOdds= stCollectionList.lOdds;
					//stMonsterList.wOdds += wLastOdds;
					//wLastOdds = stMonsterList.wOdds;

					pCollection->vectorCollectionList.push_back(stCollectionList);
				}
			}
		}
	}

	return true;
}
CWorldRegion::tagCollection* CWorldRegion::GetCollection(long lIndex)	// 获取指定index的Collection分布
{
	map<long,tagCollection>::iterator it=m_mapCollection.find(lIndex);
	if (it!= m_mapCollection.end())
	{
		return &(*it).second;
	}
	return 0;
}
BOOL CWorldRegion::LoadGateList()
{
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.door", GetID());

	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
		return FALSE;
	
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);
	
	m_mapGate.clear();
	
	string str;
	while(1)
	{
		stream>>str;
		if("<end>"==str||stream.eof())
		{
			break;
		}
		if("#"==str)
		{
			tagGate gate;
			stream>>gate.lIndex
				>>gate.strOrigName
				>>gate.lxPos
				>>gate.lyPos
				>>gate.lDir
				>>gate.nOpenState
				>>gate.strAttack
				>>gate.strOpen
				>>gate.strClose
				>>gate.strDestory
				>>gate.strRefresh;
			m_mapGate[gate.lIndex]=gate;
		}
	}
	return TRUE;
}
BOOL CWorldRegion::LoadRoads()
{
	char filename[MAX_PATH];
	int add=GetID();
	sprintf(filename, "regions/%d.roads", GetSvrResourceID());

	vector<POINT> enpty_road;
	m_vecRoadsList.push_back(enpty_road);

	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		m_vecRoadsList.clear();

		vector<POINT> road;
		POINT point;
		string strTemp;
 		while (ReadTo(stream,"*"))
		{
			road.clear();
			stream>>strTemp;
			while(1)
			{
				stream>>strTemp;
				if (strTemp=="<end>")
				{
					break;	
				}
				stream>>point.x
					>>point.y;
				road.push_back(point);		
			}
			m_vecRoadsList.push_back(road);
		}

		/*int RoadCount=0;
		int RoadIndex=0;
		int RoadPointNum=0;
		vector<POINT> road;
		POINT	point;

		m_vecRoadsList.push_back(road);

		while(ReadTo(stream,"*"))
		{
			road.clear();

			RoadCount++;
			stream>>RoadIndex;
			stream>>RoadPointNum;
			if(RoadCount!= RoadIndex)
				return false;

			for (int a=0; a<RoadPointNum; a++)
			{
				stream>>point.x;
				stream>>point.y;
				road.push_back(point);
			}

			m_vecRoadsList.push_back(road);
			
		}*/   

	}
	return TRUE;

}

BOOL	CWorldRegion::LoadRegionDrop()
{
	char filename[MAX_PATH];
	int add=GetID();
	sprintf(filename, "regions/%d.xml", GetID());
	return CMonsterList::LoadRegionDrop(filename,GetID());
}
//////////////////////////////////////////////////////////////////////////
//	天气效果
//////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------
// Name: TranslateColorCode(char* pColorCode)
// Describe: 将16进制文本颜色代码转换为PIXEL颜色值
// -------------------------------------------------------
DWORD TranslateColorCode(char* pColorCode)
{
	unsigned long color=0;

	for (int i=0;i<6;i++)
	{
		color<<=4;
		if (*(pColorCode+i)>='0' && *(pColorCode+i)<='9')
			color=color+(*(pColorCode+i)-'0');
		else if(*(pColorCode+i)>='A' && *(pColorCode+i)<='F')
			color=color+(*(pColorCode+i)-'A')+10;
	}
	return color;
}

BOOL CWorldRegion::LoadWeatherSetup()
{
	m_vectorWeatherSetup.clear();

	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.weather", GetID());

	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		while( ReadTo(stream, "time") )
		{
			char strTemp[256];
			tagWeatherTime st;
			long lOptionNum = 0, lWeatherNum = 0;
			stream >> st.lTime
				>> strTemp >> lOptionNum;

			long lLastOdds = 0;
			for (int i=0; i<lOptionNum; i++)
			{
				tagWeatherTime::tagOption stOption;
				stream >> stOption.lOdds
					>> lWeatherNum;
				stOption.lOdds += lLastOdds;
				lLastOdds = stOption.lOdds;

				for (int j=0; j<lWeatherNum; j++)
				{
					tagWeatherTime::tagOption::tagWeather stWeather;
					stream >> stWeather.lWeatherIndex;

					if (stWeather.lWeatherIndex / 100 == 4)	// 雾
					{
						stream >> strTemp;
						stWeather.dwFogColor = TranslateColorCode(strTemp);
						stWeather.dwFogColor |= 0xff000000;
					}

					stOption.vectorWeather.push_back(stWeather);
				}

				st.vectorOption.push_back(stOption);
			}
			m_vectorWeatherSetup.push_back(st);
		}
		return true;
	}
	return false;
}

BOOL CWorldRegion::LoadSetup()
{
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.rs", GetID());

	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(NULL, str, "error", MB_OK);
		return false;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strSetupEnd;
	
	// 读取各种复活点配置数据
	long type = 0, lRegionID = 0, lPosX1 = 0, lPosX2 = 0, lPosY1 = 0, lPosY2 = 0;
	bool bPointFlag = 0;
	
	while(1)
	{
		stream >> strSetupEnd;
		if (strSetupEnd=="<end>" || stream.eof())
			break;
		
		if (strSetupEnd=="#")
		{
			stream >> type >> bPointFlag
				>> lRegionID			
				>> lPosX1	
				>> lPosY1
				>> lPosX2
				>> lPosY2;

			switch(type)
			{
			case 1: // 复活点
				{
					m_stSetup.bReLiveIsUsed = true;
					m_stSetup.bReLivePointFlag = bPointFlag;	//复活点类型（type） 0：原地复活 1：指定地图复活
					m_stSetup.lReLiveRegionID	= lRegionID;			//复活点地图ID
					m_stSetup.lReLivePosX1		= lPosX1;			//复活点坐标范围
					m_stSetup.lReLivePosX2		= lPosX2;
					m_stSetup.lReLivePosY1		= lPosY1;
					m_stSetup.lReLivePosY2		= lPosY2;
				}
				break;
			case 2: // 下线回城点
				{
					m_stSetup.bLogoutIsUsed = true;
					m_stSetup.lLogoutRegionID	= lRegionID; //下线回城点地图ID
					m_stSetup.lLogoutPosX1		= lPosX1;	//下线回城点坐标范围
					m_stSetup.lLogoutPosX2		= lPosX2;
					m_stSetup.lLogoutPosY1		= lPosY1;
					m_stSetup.lLogoutPosY2		= lPosY2;
				}
				break;
			case 3: // 符文石回城点
				{
					m_stSetup.bRuneStoneIsUsed = true;
					m_stSetup.lRuneStoneRegionID	= lRegionID;//符文石回城点地图ID
					m_stSetup.lRuneStonePosX1		= lPosX1;	//符文石回城点坐标范围
					m_stSetup.lRuneStonePosX2		= lPosX2;
					m_stSetup.lRuneStonePosY1		= lPosY1;
					m_stSetup.lRuneStonePosY2		= lPosY2;
				}
				break;
			case 4: // 副本回城点
				{
					m_stSetup.bDupRgnIsUsed = true;
					m_stSetup.lDupRgnRegionID	= lRegionID;//副本回城点地图ID
					m_stSetup.lDupRgnPosX1		= lPosX1;	//副本回城点坐标范围
					m_stSetup.lDupRgnPosX2		= lPosX2;
					m_stSetup.lDupRgnPosY1		= lPosY1;
					m_stSetup.lDupRgnPosY2		= lPosY2;
				}
				break;
			}
		}
	}

	ReadTo(stream, "#");
	stream >> m_stSetup.bReLiveCurPosFlag;	//原地复活开关 1//1：开 0：关
	ReadTo(stream, "#");
	stream >> m_stSetup.bForbiddenFlag;		//禁制复活开关 1//1：开 0：关
	ReadTo(stream, "#");
	stream >> m_stSetup.bRuneStoneCanRecord;//符文石回城记录开关 1//1：开 0：关
	ReadTo(stream, "#");
	stream >> m_stSetup.bWarningFlag;//警告开关 1//1：开 0：关

	// 禁用物品配置
	string strGoodName;
	ReadTo(stream, "#");
	m_ForbidMakeGoods.clear();
	while(1)
	{
		stream >> strGoodName;
		if (strGoodName=="<end>" || stream.eof())
		{
			break;
		}
		m_ForbidMakeGoods.push_back(strGoodName);
		ReadTo(stream, "#");
	}

	// 据点配置
	tagStrongPoint tmpSP;
	ReadTo(stream, "<begin>");
	m_vecStrongPoint.clear();
	while(1)
	{
		stream >> strSetupEnd;
		if (strSetupEnd=="<end>" || stream.eof())
			break;

		if (strSetupEnd=="#")
		{
			stream >> tmpSP.szName >> tmpSP.lPosX >> tmpSP.lPosY;
			m_vecStrongPoint.push_back(tmpSP);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////

// 读取
BOOL CWorldRegion::Load()
{
	BOOL ret = CRegion::Load();
	LoadRoads();
	LoadMonsterList();
	LoadCollectionList();
	LoadGateList();
	//LoadNpcList();
	LoadWeatherSetup();
	LoadSetup();
	LoadRegionDrop();

	if(ret)
		ret = LoadTaxParam();
	return ret; 
}


//初始化和帮会有关的联系
void	CWorldRegion::InitOwnerRelation()
{
	if(m_Param.OwnedFactionID == NULL_GUID)	return;

	if( GetOrganiCtrl()->GetFactionOrganizing(m_Param.OwnedFactionID) == NULL )
	{
		m_Param.OwnedFactionID = NULL_GUID;
		m_Param.OwnedUnionID  = NULL_GUID;
	}
	if( GetOrganiCtrl()->GetUnionOrganizing(m_Param.OwnedUnionID) == NULL )
	{
		m_Param.OwnedUnionID  = NULL_GUID;
	}
		
	if(GetOwnedCityFaction() != NULL_GUID)
	{		
		////给帮会添加主城
		//GetOrganiCtrl()->AddOwnedCityToFaction( m_Param.OwnedFactionID, GetID() );
		////根据占领的帮会，设置该场景所属的国家
		//SetCountry(GetOrganiCtrl()->GetCountryByFaction(GetOwnedCityFaction()));
	}
}

// 添加到CByteArray
bool CWorldRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CRegion::AddToByteArray(pByteArray, bExData);
	_AddToByteArray(pByteArray,(BYTE)m_WarRegionType);
	_AddToByteArray(pByteArray,(BYTE)m_eSpaceType);
	_AddToByteArray(pByteArray,m_dwAreaId);

	_AddToByteArray(pByteArray,(BYTE)m_bNoPk);
	_AddToByteArray(pByteArray,(char)m_bNoContribute);
	_AddToByteArray(pByteArray,(BYTE)m_Ride);	
	_AddToByteArray(pByteArray,(BYTE)m_ChangeEquip);
	_AddToByteArray(pByteArray,(BYTE)m_bRelive);
	_AddToByteArray(pByteArray,(BYTE)m_bDeadReturn);
    _AddToByteArray(pByteArray,(BYTE)m_bCommonHook);
    _AddToByteArray(pByteArray,(BYTE)m_bStableHook);
    _AddToByteArray(pByteArray, (BYTE)m_bNoChangeFactionMember);

	//路径

	_AddToByteArray(pByteArray, (long)m_vecRoadsList.size());
	for (vector<vector<POINT>>::iterator it=m_vecRoadsList.begin(); it!=m_vecRoadsList.end(); it++)
	{
		_AddToByteArray(pByteArray, (long)(*it).size());
		//_AddToByteArray(pByteArray, );
		for (int a=0; a<(*it).size() ; a++)
		{
			_AddToByteArray(pByteArray, (*it)[a].x);
			_AddToByteArray(pByteArray, (*it)[a].y);
		}
	}

	// MonsterList
	_AddToByteArray(pByteArray, (DWORD)m_mpMonster.size());
	for(map<DWORD,tagMonster>::iterator it=m_mpMonster.begin(); it!=m_mpMonster.end(); ++it)
	{
        _AddToByteArray(pByteArray, (BYTE*)&((*it).second), sizeof(tagMonster)-sizeof(vector<tagMonsterList>));		
		long lNum = (long)(*it).second.vectorMonsterList.size();
		_AddToByteArray(pByteArray, lNum);
		for (int i=0; i<lNum; i++)
		{
			_AddToByteArray(pByteArray, &(*it).second.vectorMonsterList[i], sizeof(tagMonsterList)-sizeof(string)*3);
			_AddToByteArray(pByteArray, (char*)(*it).second.vectorMonsterList[i].strName.c_str());
			_AddToByteArray(pByteArray, (char*)(*it).second.vectorMonsterList[i].strScript.c_str());
			_AddToByteArray(pByteArray, (char*)(*it).second.vectorMonsterList[i].strTalkScript.c_str());
		}		
	}

	//CollectionList
	_AddToByteArray(pByteArray,(long)m_mapCollection.size());
	for (map<LONG,tagCollection>::iterator it=m_mapCollection.begin(); it!=m_mapCollection.end(); it++)
	{
		_AddToByteArray(pByteArray, (BYTE*)&((*it).second), sizeof(tagCollection)-sizeof(vector<tagCollectionList>));
		long lNum = (long)(*it).second.vectorCollectionList.size();
		_AddToByteArray(pByteArray, lNum);
		for (int a=0; a<lNum; a++)
		{
			_AddToByteArray(pByteArray,&(*it).second.vectorCollectionList[a],sizeof(tagCollectionList));
		}
	}
	
	//门
	_AddToByteArray(pByteArray,(long)m_mapGate.size());
	for(map<long,tagGate>::iterator it=m_mapGate.begin();it!=m_mapGate.end();it++)
	{
		_AddToByteArray(pByteArray,(BYTE*)&((*it).second),sizeof(tagGate)-sizeof(string)*6);
		_AddToByteArray(pByteArray,(char*)(*it).second.strOrigName.c_str());
		_AddToByteArray(pByteArray,(char*)(*it).second.strAttack.c_str());
		_AddToByteArray(pByteArray,(char*)(*it).second.strOpen.c_str());
		_AddToByteArray(pByteArray,(char*)(*it).second.strClose.c_str());
		_AddToByteArray(pByteArray,(char*)(*it).second.strDestory.c_str());
		_AddToByteArray(pByteArray,(char*)(*it).second.strRefresh.c_str());

	}

	// 天气
	_AddToByteArray(pByteArray, (long)m_vectorWeatherSetup.size());
	for (vector<tagWeatherTime>::iterator it=m_vectorWeatherSetup.begin(); it!=m_vectorWeatherSetup.end(); it++)
	{
		tagWeatherTime* pWT = &(*it);
		long lNum = (long)pWT->vectorOption.size();
		_AddToByteArray(pByteArray, pWT->lTime);
		_AddToByteArray(pByteArray, lNum);
		for (int i=0; i<lNum; i++)
		{
			_AddToByteArray(pByteArray, pWT->vectorOption[i].lOdds);
			long lWeatherNum = (long)pWT->vectorOption[i].vectorWeather.size();
			_AddToByteArray(pByteArray, lWeatherNum);
			for (int j=0; j<lWeatherNum; j++)
			{
				_AddToByteArray(pByteArray, &pWT->vectorOption[i].vectorWeather[j], sizeof(tagWeatherTime::tagOption::tagWeather));
			}
		}
	}
	AddSetupToByteArray(pByteArray,bExData);
	////添加场景相关参数
	_AddToByteArray(pByteArray, (BYTE*)&m_Param, sizeof(tagRegionParam));

	// 变量列表
	m_pVariableList->AddToByteArray(pByteArray);

	// 添加据点信息
	_AddToByteArray(pByteArray, (BYTE)GetStrongPointFlag());
	_AddToByteArray(pByteArray, (long)m_vecStrongPoint.size());
	std::vector<tagStrongPoint>::iterator spItr = m_vecStrongPoint.begin();
	for(; spItr != m_vecStrongPoint.end(); spItr++)
	{
		_AddToByteArray(pByteArray, &(*spItr), sizeof(tagStrongPoint));
	}
	return true;
}


// 添加到CByteArray
bool CWorldRegion::AddToByteArrayForProxy(vector<BYTE>* pByteArray, bool bExData)
{
	CBaseObject::AddToByteArray(pByteArray, bExData);
	//添加所属国家
	_AddToByteArray(pByteArray, m_btCountry);
	//添加场景类型
	_AddToByteArray(pByteArray,(long)m_WarRegionType);
	////添加场景相关参数
	_AddToByteArray(pByteArray, (BYTE*)&m_Param, sizeof(tagRegionParam));
	return true;
}

bool CWorldRegion::AddSetupToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	_AddToByteArray(pByteArray,&m_stSetup,sizeof(tagRegionSetup));

	//添加禁止使用的物品
	long lSize = m_ForbidMakeGoods.size();
	_AddToByteArray(pByteArray,lSize);
	for(int i=0;i < lSize;i++)
	{
		_AddToByteArray(pByteArray, (char*)(m_ForbidMakeGoods[i].c_str()));
	}

	return true;
}
// 解码
bool CWorldRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CRegion::DecordFromByteArray(pByte, pos, bExData);	
	m_ID = m_lID;
	m_WarRegionType = (eRWT)_GetByteFromByteArray(pByte, pos);
	m_eSpaceType = (eRST)_GetByteFromByteArray(pByte, pos);
	char ch=_GetByteFromByteArray(pByte,pos);
	m_Ride=ch==0?false:true;
	ch=_GetByteFromByteArray(pByte,pos);
	m_ChangeEquip=ch==0?false:true;
	m_dwAreaId=_GetDwordFromByteArray(pByte,pos);
	ch = _GetCharFromByteArray(pByte, pos);
	m_bNoPk = ch;
	ch = _GetCharFromByteArray(pByte, pos);
	m_bNoContribute = ch==0?false:true;
	ch = _GetByteFromByteArray(pByte,pos);
	m_bRelive = ch==0?false:true;
	ch = _GetByteFromByteArray(pByte,pos);
	m_bDeadReturn = ch==0?false:true;
    ch = _GetByteFromByteArray(pByte,pos);
    m_bCommonHook = (ch==0?false:true);
    ch = _GetByteFromByteArray(pByte,pos);
    m_bStableHook = (ch==0?false:true);
    ch = _GetByteFromByteArray(pByte,pos);
    m_bNoChangeFactionMember = (ch==0?false:true);
	
	return true;
}


// 解码常见参数
bool CWorldRegion::DecordRegionParamFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	tagRegionParam TemptRegionParam;
	_GetBufferFromByteArray(pByte, pos, &TemptRegionParam, sizeof(tagRegionParam));
	m_Param.dwTodayTotalTax = TemptRegionParam.dwTodayTotalTax;
	m_Param.dwTotalTax = TemptRegionParam.dwTotalTax;
	m_Param.lCurrentTaxRate = TemptRegionParam.lCurrentTaxRate;
	m_Param.OwnedFactionID = TemptRegionParam.OwnedFactionID;
	return true;
}

//产生保存数据
CWorldRegion* CWorldRegion::GenerateSaveData()
{
	CWorldRegion* pRegion = MP_NEW CWorldRegion;
	pRegion->SetRegionParam(m_Param);	
	return pRegion;
}
void CWorldRegion::GetReLiveReturnPoint(CPlayer* pPlayer, long &lRegion, long flag/*是那一种复活方式*/, long &l, long &t, long &r, long &b, long &d)
{
	//复活点类型（flag） 0：原地复活 1：指定地图复活
	
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}
	
	if(flag == 0) // 原地复活
	{
		lRegion = pPlayer->GetRegionID();
		l		= m_stSetup.lReLivePosX1;
		t		= m_stSetup.lReLivePosY1;
		r		= m_stSetup.lReLivePosX2;
		b		= m_stSetup.lReLivePosY2;
		d		= -1;
	}
	else// 指定地图复活
	{
		lRegion = m_stSetup.lReLiveRegionID;
		l		= m_stSetup.lReLivePosX1;
		t		= m_stSetup.lReLivePosY1;
		r		= m_stSetup.lReLivePosX2;
		b		= m_stSetup.lReLivePosY2;
		d		= -1;
	}
}
void CWorldRegion::GetLogoutReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lLogoutRegionID;
	l		= m_stSetup.lLogoutPosX1;
	t		= m_stSetup.lLogoutPosY1;
	r		= m_stSetup.lLogoutPosX2;
	b		= m_stSetup.lLogoutPosY2;
	d		= -1;
}
void CWorldRegion::GetRuneStoneReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lRuneStoneRegionID;
	l		= m_stSetup.lRuneStonePosX1;
	t		= m_stSetup.lRuneStonePosY1;
	r		= m_stSetup.lRuneStonePosX2;
	b		= m_stSetup.lRuneStonePosY2;
	d		= -1;
}
void CWorldRegion::GetDupRgnReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lDupRgnRegionID;
	l		= m_stSetup.lDupRgnPosX1;
	t		= m_stSetup.lDupRgnPosY1;
	r		= m_stSetup.lDupRgnPosX2;
	b		= m_stSetup.lDupRgnPosY2;
	d		= -1;
}

void CWorldRegion::SetEnterPosXY(CPlayer* pPlayer)
{
	if(pPlayer == NULL)	return;
	//如果是据点地图，并且已经过了清人时间，则需要在回程点上线
	//if( GetVilWarSys()->IsVilRegionLeftClearTime(GetID()) )
	//{
	//	CGUID rgnID;
	//	long GSID = GetGame()->GetGlobalRgnManager()->FindGSIDByRgnID(rgnID);

	//	long lX, lY;
	//	long l, t, r, b, d;
	//	long retRgnID = l = t = r = b = d = -1;
	//	GetReturnPoint(pPlayer, retRgnID , l, t, r ,b ,d);

	//	long sx = l;
	//	long sy = t;
	//	long w = r - sx;
	//	long h = b - sy;

	//	CWorldRegion* rgn = NULL;

	//	CGlobalRgnManager::RgnManagerByGSIDMapItr gsRgnManagerItr = GetGame()->GetGlobalRgnManager()->GetGSRgnManager().find(GetID());
	//	if(gsRgnManagerItr != GetGame()->GetGlobalRgnManager()->GetGSRgnManager().end())
	//	{
	//		CGlobalRgnManager::tagRegion* tRgn = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(retRgnID);
	//		if(tRgn)
	//		{
	//			rgnID = tRgn->pRegion->GetExID();
	//			rgn = tRgn->pRegion;
	//		}
	//	}

	////	CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindGSRgnByGUID(GSID, rgnID);
	//	
	//	if(rgn)
	//		rgn->GetRandomPosInRange(lX, lY, sx, sy, w, h);

	//	pPlayer->SetRegionExID(rgnID);
	//	pPlayer->SetPosXY(lX+0.5f,lY+0.5f);
	//}
}

//读取税收相关参数
BOOL CWorldRegion::LoadTaxParam()
{
	// 读新的
	char filename[MAX_PATH];
	sprintf(filename, "regions/%d.Tax", GetID());

	CRFile* prfile = rfOpen(filename);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		if( ReadTo(stream,"*") )
		{
			stream >>m_Param.lCurrentTaxRate
				>>m_Param.lYuanbaoTaxRate
				>>m_Param.lBusinessTaxRate
				>> m_Param.lMaxTaxRate
				>> m_Param.lSupRegionID
				>> m_Param.lTurnInTaxRate;
		}
	}

	m_Param.lID = GetID();
//	GetGame()->SendLoadRegionParam(m_guExID,m_Param.lID);
//	GetGame()->GetRsRegion()->LoadRegionParam(this);
	return true;
}


void CWorldRegion::SetParamFromDB(const CGUID& OwnedFactionGuid,const CGUID& OwnedUnionGuid,long lCurTaxRate,long lTotadyTotalTax,
								  long lTotalTax)
{
	m_Param.OwnedFactionID = OwnedFactionGuid;
	m_Param.OwnedUnionID = OwnedUnionGuid;
	if(lCurTaxRate < m_Param.lMaxTaxRate)
		m_Param.lCurrentTaxRate = lCurTaxRate;
	else
		m_Param.lCurrentTaxRate = m_Param.lMaxTaxRate;
	m_Param.dwTodayTotalTax = lTotadyTotalTax;
	m_Param.dwTotalTax = lTotalTax;
}

void CWorldRegion::SetParamFromGS(long lCurTaxRate,DWORD dwTotadyTotalTax,DWORD dwTotalTax)
{
	m_Param.lCurrentTaxRate = lCurTaxRate;
	m_Param.dwTodayTotalTax = dwTotadyTotalTax;
	m_Param.dwTotalTax = dwTotalTax;
}

// 根据当前场景复制数据
bool CWorldRegion::CloneData(CWorldRegion* pSrcRgn)
{
	if(pSrcRgn)
	{
		// baseobject
		m_lType = pSrcRgn->GetType();
		m_guExID = pSrcRgn->GetExID();
		m_lGraphicsID = pSrcRgn->GetGraphicsID();
		if(pSrcRgn->GetName())
			m_strName = pSrcRgn->GetName();

		// region
		SetID(pSrcRgn->GetID());
		m_lLinedIdFlag	= pSrcRgn->GetLinedIdFlag();
		m_lRgnType		= pSrcRgn->GetRgnType();
		m_lRegionType	= pSrcRgn->GetRegionType();
		m_lResourceID	= pSrcRgn->GetResourceID();
		m_fExpScale		= pSrcRgn->GetExpScale();
		m_fOccuExpScale	= pSrcRgn->GetOccuExpScale();
		m_fSpScale		= pSrcRgn->GetSpScale();
		m_lWidth		= pSrcRgn->GetWidth();
		m_lHeight		= pSrcRgn->GetHeight();

		// worldregion
		m_btCountry		= pSrcRgn->GetCountry();
		m_lNotify		= pSrcRgn->GetNotify();
		m_dwAreaId		= pSrcRgn->GetAreaId();
		m_bRelive	    = pSrcRgn->GetRelive();
		m_bDeadReturn	= pSrcRgn->GetDeadReturn();
		//m_bNoPk			= pSrcRgn->IsNoPk();
		//m_bNoContribute = pSrcRgn->IsNoContribute();

		m_byRgnHideFlag = pSrcRgn->GetRgnHideFlag();
		return true;
	}
	return false;
}

// 复制全部数据
bool CWorldRegion::CloneAllData(CWorldRegion* pRgn)
{
	if(pRgn)
	{
		SetVariableList(pRgn->GetVariableList());

		// 地图Monster设置
		m_mpMonster.clear();
		map<DWORD, tagMonster>::iterator itr = pRgn->GetMonsterSetup().begin();
		for(; itr != pRgn->GetMonsterSetup().end(); itr++)
		{
			tagMonster tagMo;
			tagMo.lIndex = itr->second.lIndex;		
			tagMo.rectRange = itr->second.rectRange;		
			tagMo.lNum = itr->second.lNum;			
			tagMo.lResetTime = itr->second.lResetTime;	
			tagMo.lStartTime = itr->second.lStartTime;	
			tagMo.lDir = itr->second.lDir;

			vector<CWorldRegion::tagMonsterList>::iterator itrMoList = itr->second.vectorMonsterList.begin();
			for(; itrMoList != itr->second.vectorMonsterList.end(); itrMoList++)
			{
				CWorldRegion::tagMonsterList tmpList;
				tmpList.wOdds = (*itrMoList).wOdds;				
				tmpList.wSign = (*itrMoList).wSign;				
				tmpList.wLeaderSign = (*itrMoList).wLeaderSign;		
				tmpList.wLeaderDistance = (*itrMoList).wLeaderDistance;	
				tmpList.wRoadIndex = (*itrMoList).wRoadIndex;		
				tmpList.strName	 = (*itrMoList).strName;
				tmpList.strScript = (*itrMoList).strScript;
				tmpList.strTalkScript = (*itrMoList).strTalkScript;
				tagMo.vectorMonsterList.push_back(tmpList);
			}
			m_mpMonster[itr->first] = tagMo;
		}

		// 地图collection设置 
		m_mapCollection.clear();
		map<long, tagCollection>::iterator itrCo = pRgn->GetCollectionMap().begin();
		for(; itrCo != pRgn->GetCollectionMap().end(); itrCo++)
		{
			tagCollection tagCo;
			tagCo.lIndex = itrCo->second.lIndex;		
			tagCo.rectRange = itrCo->second.rectRange;		
			tagCo.lNum = itrCo->second.lNum;			
			tagCo.lResetTime = itrCo->second.lResetTime;	
			tagCo.lStartTime = itrCo->second.lStartTime;	
			tagCo.lDir = itrCo->second.lDir;
			tagCo.lLivingCount = itrCo->second.lLivingCount;

			tagCo.vectorCollectionList.clear();
			vector<tagCollectionList>::iterator itr = itrCo->second.vectorCollectionList.begin();
			for(; itr != itrCo->second.vectorCollectionList.end(); itr++)
			{
				tagCollectionList tmpList;
				memset(tmpList.cName, 0, sizeof(tmpList.cName));
				memcpy(tmpList.cName, (*itr).cName, sizeof(tmpList.cName));
				tmpList.lOdds = (*itr).lOdds;
				tagCo.vectorCollectionList.push_back(tmpList);
			}
			m_mapCollection[itrCo->first] = tagCo;
		}

		// 地图door设置 
		m_mapGate = pRgn->GetGateMap();

	
		// 地图设置

		 m_stSetup = pRgn->GetSetup();
		 m_ForbidMakeGoods.clear();
		 vector<string>::iterator vecforitr = pRgn->GetForbidMakeGoodsVec().begin();
		 for(; vecforitr != pRgn->GetForbidMakeGoodsVec().end(); vecforitr++)
			 m_ForbidMakeGoods.push_back(*vecforitr);

		//	地图路点设置
		 m_vecRoadsList.clear();
		 vector<vector<POINT>>::iterator roaditr = pRgn->GetRoadList().begin();
		 for(; roaditr != pRgn->GetRoadList().end(); roaditr++)
		 {
			 vector<POINT> vp;
			 vector<POINT>::iterator pitr = (*roaditr).begin();
			 for(; pitr != (*roaditr).end(); pitr++)
			 {
				 POINT p;
				 p.x = (*pitr).x;
				 p.y = (*pitr).y;
				 vp.push_back(p);
			 }
			 m_vecRoadsList.push_back(vp); 
		 }

		//	天气效果
		m_vectorWeatherSetup.clear();
		vector<tagWeatherTime>::iterator itrwe = pRgn->GetWeatherTimeVec().begin();
		for(; itrwe != pRgn->GetWeatherTimeVec().end(); itrwe++)
		{	
			tagWeatherTime tagTi;
			tagTi.lTime = (*itrwe).lTime;
			tagTi.vectorOption.clear();
			vector<tagWeatherTime::tagOption>::iterator itr = (*itrwe).vectorOption.begin();
			for(; itr != (*itrwe).vectorOption.end(); itr++)
			{
				tagWeatherTime::tagOption tagOp;
				tagOp.lOdds = (*itr).lOdds;
				tagOp.vectorWeather.clear();
				vector<tagWeatherTime::tagOption::tagWeather>::iterator itrop = (*itr).vectorWeather.begin();
				for(; itrop != (*itr).vectorWeather.end(); itrop++)
				{
					tagWeatherTime::tagOption::tagWeather tagWe;
					tagWe.lWeatherIndex = (*itrop).lWeatherIndex;
					tagWe.dwFogColor = (*itrop).dwFogColor;
					tagOp.vectorWeather.push_back(tagWe);
				}
				tagTi.vectorOption.push_back(tagOp);
			}
			m_vectorWeatherSetup.push_back(tagTi);
		}

		m_Param = pRgn->GetRegionParam();
		CloneData(pRgn);

		M_FREE(m_pCell,sizeof(tagCell)*m_lWidth*m_lHeight);
		m_pCell = (tagCell*)M_ALLOC(sizeof(tagCell)*m_lWidth*m_lHeight);
		memcpy(m_pCell, pRgn->GetCellPtr(), sizeof(tagCell)*m_lHeight*m_lWidth);

		m_vectorSwitch.clear();
		vector<tagSwitch>::iterator switchitr = pRgn->GetSwitchVec().begin();
		for(; switchitr != pRgn->GetSwitchVec().end(); switchitr++)
		{
			tagSwitch tagSw;
			tagSw.lCoordX = (*switchitr).lCoordX;
			tagSw.lCoordY = (*switchitr).lCoordY;
			tagSw.lDir = (*switchitr).lDir;
			tagSw.lRegionID = (*switchitr).lRegionID;
			tagSw.lState = (*switchitr).lState;
			m_vectorSwitch.push_back(tagSw); 
		}

		m_Ride = pRgn->IsRide();

		// 据点信息
		m_bStrongPintFlag = pRgn->GetStrongPointFlag();
		m_vecStrongPoint.clear();
		vector<tagStrongPoint>::iterator spItr = pRgn->GetStrongPointVec().begin();
		for(; spItr != pRgn->GetStrongPointVec().end(); spItr++)
			m_vecStrongPoint.push_back((*spItr));

		return true;
	}
	return false;
}

long CWorldRegion::GetStrongPointState(const char* szName)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				return (*it).lState;
			}
		}
	}
	return -1;
}
void CWorldRegion::SetStrongPointState(const char* szName, long lState)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				(*it).lState = lState;
				return;
			}
		}
	}
}
void CWorldRegion::GetStrongPointPos(const char* szName, long& lX, long& lY)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				lX = (*it).lPosX;
				lY = (*it).lPosY;
				return;
			}
		}
	}
}