#include "stdafx.h"
#include "BusinessSystem.h"
#include "../../../Public/Common/CreditSystem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef map<WORD/*各国家的ID*/,DWORD/*声望值*/>::iterator MapCountryCreditIter;
typedef map<WORD/*各国家的ID*/,DWORD/*投资总额*/>::iterator MapCountryInvestIter;
typedef map<DWORD,CBusinessSystem::tagTradeGoodsInfor>::iterator MapTradeGoodsInforIter;
typedef map<WORD,CBusinessSystem::tagBusinessZone>::iterator MapZoneIter;
typedef map<WORD,CBusinessSystem::tagBusinessPoint>::iterator MapPointIter;
typedef map<WORD,CBusinessSystem::tagBusinessZone>* MapZonePtr;
typedef map<WORD,CBusinessSystem::tagBusinessPoint>* MapPointPtr;
typedef vector<CBusinessSystem::tagCreditLevel>::iterator VecCreditLevelIter;
typedef map<int,DWORD>::iterator MapAreaIter;

map<WORD/*各贸易区的ID*/,CBusinessSystem::tagBusinessZone>  CBusinessSystem::m_mapBusinessZones;			//各贸易区数据
map<WORD/*各贸易点的ID*/,CBusinessSystem::tagBusinessPoint> CBusinessSystem::m_mapBusinessPoint;			//各贸易点数据
map<DWORD,CBusinessSystem::tagTradeGoodsInfor>				CBusinessSystem::m_mapTradeGoods;				//当前所有物品卖某个NPC的价格表
CBusinessSystem*											CBusinessSystem::m_pBusinessSystem = NULL;
CBusinessSystem::tagInvestInfor								CBusinessSystem::m_tagInvest;					//玩家在某个贸易点的一次投资信息
vector<CBusinessSystem::tagCreditLevel>						CBusinessSystem::m_vecCreditLevel;				//声望等级列表
map<int,CBusinessSystem::tagAreaCredit>                     CBusinessSystem::m_mapCreditArea;
long                                                        CBusinessSystem::m_lAreaNumSize = 0;             //配置中的区域数量
CCreditSystem*                                              CBusinessSystem::m_pCreditSystem = NULL;

CBusinessSystem::CBusinessSystem()
{
	//m_mapBusinessPoint.clear();
	//m_mapBusinessPoint.clear();
	//m_vecTradeGoods.clear();
}

CBusinessSystem::tagBusinessZone* CBusinessSystem::GetZoneInfor(WORD wZoneID)
{
	MapZoneIter iter = m_mapBusinessZones.find(wZoneID);
	if (iter!=m_mapBusinessZones.end())
	{
		return &(iter->second);
	}
	return NULL;
}

CBusinessSystem::tagBusinessPoint* CBusinessSystem::GetPointInfor(WORD wPointID)
{
	MapPointIter iter = m_mapBusinessPoint.find(wPointID);
	if (iter!=m_mapBusinessPoint.end())
	{
		return &(iter->second);
	}
	return NULL;
}

bool CBusinessSystem::CreateBusinessSystem()
{
	m_pBusinessSystem = new CBusinessSystem();	
	if (m_pBusinessSystem!=NULL)
	{
		return true;
	}
	return false;
}

//bool CBusinessSystem::InitBusinessSystem()
//{
//	
//}

void CBusinessSystem::Release()
{
	SAFE_DELETE(m_pBusinessSystem);
}

bool CBusinessSystem::ResetSystem()
{
	return false;
}

void CBusinessSystem::UpdateZoneData(WORD wZoneID)
{

}

void CBusinessSystem::UpdatePointData(WORD wPointID)
{
	
}

bool CBusinessSystem::InitCreditLevel(const char* filename)
{
	m_vecCreditLevel.clear();

	tagCreditLevel stCredit;

	stringstream stream;	
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}
	prfile->ReadToStream(stream);
	rfClose(prfile);


	while(ReadTo(stream, "#"))
	{
		stream >> stCredit.lMinNum >> stCredit.lMaxNum >> stCredit.LevelName;
		m_vecCreditLevel.push_back(stCredit);
	}

	return TRUE;
}

//根据传入的声望值获取声望等级名称
char* CBusinessSystem::GetCreditLevelName(long lCurCreditValue)
{
	VecCreditLevelIter iter = m_vecCreditLevel.begin();
	for (;iter!=m_vecCreditLevel.end();iter++)
	{
		if (lCurCreditValue>=iter->lMinNum&&lCurCreditValue<=iter->lMaxNum)
		{
			return iter->LevelName;
		}
	}
	return NULL;
}

//根据传入的声望值获取当前等级的最大声望值
long CBusinessSystem::GetMaxCreditValue(long lCurCreditValue)
{
	VecCreditLevelIter iter = m_vecCreditLevel.begin();
	for (;iter!=m_vecCreditLevel.end();iter++)
	{
		if (lCurCreditValue>=iter->lMinNum&&lCurCreditValue<=iter->lMaxNum)
		{
			return iter->lMaxNum;
		}
	}
	return -1;
}

//根据传入的声望值获取当前等级的最大声望值
long CBusinessSystem::GetCurCreditLevel(long lCurCreditValue)
{
	VecCreditLevelIter iter = m_vecCreditLevel.begin();
	long lLevel=0;
	for (;iter!=m_vecCreditLevel.end();iter++)
	{
		if (lCurCreditValue>=iter->lMinNum&&lCurCreditValue<=iter->lMaxNum)
		{
			return lLevel;
		}
		lLevel++;
	}
	return -1;
}


//根据传入的索引id查找这个区域的显示名
const char* CBusinessSystem::GetAreaName(int id)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->CreditAreaNameByIndex(id);
	}
    return NULL;
}


bool  CBusinessSystem::InitCreditSystem()
{
	m_pCreditSystem = new CCreditSystem();
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->LoadAreaCredit();
	}
	return FALSE;
}

//根据传入的组织ID 和对应的声望等级返回当前等级最大的声望值上限
long  CBusinessSystem::GetOrgeCreditMaxValue(DWORD dwOrganID,DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->MaxOrganValue(dwOrganID,dwValue);
	}
	return 0;
}

//获取指定组织的指定声望值 所对应的名字
const char* CBusinessSystem::CreditOrganName(DWORD dwOrganID,DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->CreditOrganName(dwOrganID,dwValue);
	}
	return NULL;
}

//根据传入的组织id和声望值得到对应的声望等级
long CBusinessSystem::CreditOrganLevel(DWORD dwOrganID,DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->CreditOrganLevel(dwOrganID,dwValue);
	}
	return  0;
}

//获取指定等级的区域声望上限
long CBusinessSystem::MaxAreaValue(DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->MaxAreaValue(dwValue);
	}
	return 0;
}

//获取指定值获取声望的等级
long CBusinessSystem::AreaLevel(DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->CreditLevel(dwValue);
	}
	return 0;
}

//根据声望值获取当前声望的显示名
const char* CBusinessSystem::AreaCreditName(DWORD dwValue)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->CreditName(dwValue);
	}
	return NULL;
}

//释放声望系统资源
void CBusinessSystem::ReleaseCreditSystem()
{
	if(m_pCreditSystem)
	{
	   SAFE_DELETE(m_pCreditSystem);
	}
}

//获得声望区域的数量
size_t CBusinessSystem::GetAreaNumSize()
{
	if(m_pCreditSystem)
	{
		return (int)m_pCreditSystem->GetCreditAreaSize();
	}
	return 0;
}

//获得声望组织的数量
size_t CBusinessSystem::GetOrganNumSize()
{
	if(m_pCreditSystem)
	{
		return (int)m_pCreditSystem->GetCreditOrganSize();
	}
	return 0;
}

//获得组织声望的最大值
long CBusinessSystem::GetMaxCreditByOrgan(long ID)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->GetMaxCreditByOrgan(ID);
	}
	return 0;
}

//获得区域声望的最大值
long CBusinessSystem::MaxAreaCredit()
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->MaxAreaCredit();
	}
	return 0;
}

long CBusinessSystem::GetMaxCreditLevelByOrgan(long id)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->GetMaxCreditLevelByOrgan(id);
	}
	return 0;
}

long CBusinessSystem::MaxAreaCreditLevel()
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->MaxAreaCreditLevel();
	}
	return 0;
}

//根据组织声望的ID获取此声望的名称
const char* CBusinessSystem::GetOrganName(DWORD dwOrganID)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->GetOrganName(dwOrganID);
	}
	return NULL;
}

//根据组织声望的ID获取此声望的名称
const char* CBusinessSystem::GetDirtyName(DWORD dwOrganID)
{
	if(m_pCreditSystem)
	{
		return m_pCreditSystem->GetDirtyName(dwOrganID);
	}
	return NULL;
}
