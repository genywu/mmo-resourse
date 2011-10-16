#include "stdafx.h"
#include "DeputyOccuSystemClient.h"
#include "CollectionGoods.h"
#include "../GameClient/Game.h"
#include "ClientRegion.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


map<DWORD,CCollectionLayout*>  CDeputyOccuSystemClient::m_mapCollectionListClient;
//map<string,CFactureStudy*>	  CDeputyOccuSystemClient::m_mapStudyListClient;
const CCollectionLayout* CDeputyOccuSystemClient::GetCollectionLayout(const char* Name)
{
	return CDeputyOccuSystem::GetCollectionLayout(Name);
}

const CFacture* CDeputyOccuSystemClient::GetFacture(DWORD ID)
{
	return CDeputyOccuSystem::GetFacture(ID);
}

LONG CDeputyOccuSystemClient::GetCollectionExp(LONG level)
{
	return CDeputyOccuSystem::GetCollectionExp(level);
}

DWORD CDeputyOccuSystemClient::GetCollectionColor(DWORD level)
{
	return CDeputyOccuSystem::GetCollectionColor(level);
}

DWORD CDeputyOccuSystemClient::GetCollectionNeededExp(DWORD level)
{
	return CDeputyOccuSystem::GetCollectionNeededExp(level);
}

LONG CDeputyOccuSystemClient::GetFactureExp(LONG level)
{
	return CDeputyOccuSystem::GetFactureExp(level);
}

LONG CDeputyOccuSystemClient::GetFactureCredit(LONG level)
{
	return CDeputyOccuSystem::GetFactureCredit(level);
}

DWORD CDeputyOccuSystemClient::GetFactureColor(DWORD level)
{
	return CDeputyOccuSystem::GetFactureColor(level);
}

DWORD CDeputyOccuSystemClient::GetFactureNeededExp(DWORD level)
{
	return CDeputyOccuSystem::GetFactureNeededExp(level);
}

const CFactureStudy* CDeputyOccuSystemClient::GetFactureStudy(const char* NpcName)
{
	return CDeputyOccuSystem::GetFactureStudy(NpcName);
}

const string* CDeputyOccuSystemClient::GetFactureTypeName(DWORD FactureType)
{
	return CDeputyOccuSystem::GetFactureSortName(FactureType);
}


bool CDeputyOccuSystemClient::InitOccuSystemClient(const char* filename)
{
	if (CDeputyOccuSystem::Init(filename))
	{
		if (!InitCollectionListClient())
		{
			Log4c::Warn(ROOT_MODULE,"InitCollectionList Failed!");
		}
		return true;
	}
	return false;
}

BOOL CDeputyOccuSystemClient::InitCollectionListClient()
{	
	m_mapCollectionListClient.clear();
	map<string,CCollectionLayout*>::iterator CollectionListiter = CDeputyOccuSystem::m_mapCollectionList.begin();
	for (;CollectionListiter!=m_mapCollectionList.end();CollectionListiter++)
	{
		DWORD dwCollectGoodsIndex = (CollectionListiter->second)->GetCollectionID();
		m_mapCollectionListClient.insert(pair<DWORD,CCollectionLayout*>(dwCollectGoodsIndex,CollectionListiter->second));
	}
	return true;
}

const CCollectionLayout* CDeputyOccuSystemClient::GetCollectionLayout(DWORD dwCollectionGoodsID)
{
	map<string,CCollectionLayout*>::iterator LayoutIter = CDeputyOccuSystem::m_mapCollectionList.begin();
	for (;LayoutIter!=m_mapCollectionList.end();LayoutIter++)
	{
		if ((LayoutIter->second)->GetCollectionID()==dwCollectionGoodsID)
		{
			return LayoutIter->second;
		}
	}
	return NULL;
}

BOOL CDeputyOccuSystemClient::InitFactureStudyListClient()
{
	//m_mapStudyListClient.clear();
	//map<string,CFactureStudy*>::iterator FactureStudyListiter = m_mapStudyList.begin();
	//for (;FactureStudyListiter!=m_mapStudyList.end();FactureStudyListiter++)
	//{
	//	for (DWORD i = 0;i<FactureStudyListiter->second->GetStudySum();i++)
	//	{
	//		DWORD dwFactureStudyIndex = (FactureStudyListiter->second)->GetFactureID(i);
	//		m_mapStudyListClient.insert(pair<string,CFactureStudy*>(dwFactureStudyIndex,FactureStudyListiter->second));
	//	}
	//}
	return true;
}

// 如果完成的任务和场景中采集物所需要的任务ID相符，则刷新采集物的特效显示
void CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(DWORD dwQuestID,bool bAddOrDel)
{
	list<CBaseObject*> *pObjectList =  GetGame()->GetRegion()->GetObjectList();
	for (itBaseObject it = pObjectList->begin(); it!=pObjectList->end(); it++)
	{
		if ((*it)->GetType()==TYPE_COLLECTION)
		{
			CCollectionGoods* pCollectShape = dynamic_cast<CCollectionGoods*>(*it);
			if (pCollectShape!=NULL&&pCollectShape->GetOccuID()==DOccu_Other)
			{
				const vector<tagQuestGoods>* pQustList = pCollectShape->GetRoleInfo();
				if (pQustList!=NULL)
				{
					vector<tagQuestGoods>::const_iterator QustIter = pQustList->begin();
					for (;QustIter!=pQustList->end();QustIter++)
					{
						if (QustIter->nQuestId==dwQuestID)
						{
							if (bAddOrDel)
							{
								((CShape*)(*it))->AddEffect(11114,((CShape*)(*it))->GetPosX(),((CShape*)(*it))->GetPosY(),((CShape*)(*it))->GetHeight(),true);
							}else
								((CShape*)(*it))->DelEffect(11114);
							break;
						}
					}
				}
			}
		}
	}
}

//采集物的条件满足与否影响特效的显示
//void CDeputyOccuSystemClient::UpdateCollectionEffectByCondition()
//{
//	
//}