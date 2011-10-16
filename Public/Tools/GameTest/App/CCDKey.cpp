#pragma once
#include "StdAfx.h"
#include "CCDKey.h"
#include "Goods.h"
#include "SkillMgr.h"
#include "EquipMgr.h"
#include "GoodsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

hash_map<string,CCDKey*> CCDKey::m_mapCDKeyList;
//////////////////////////////////////////////////////////////////////////
CCDKey::CCDKey(const char* strCDKey)
{
	m_strCDKey = strCDKey;
	m_lLevel = 0;
}
CCDKey::~CCDKey()
{
	for(int i=0;i<CONTAINER_NUM;i++)
	{
		hash_map<CGUID,tagCDKeyGoodsInfo,hash_guid_compare>::iterator it = m_mapContainerID[i].begin();
		for(;it != m_mapContainerID[i].end();it++)
		{
			SAFE_DELETE(it->second.pGoods);
		}
		m_mapContainerID->clear();
	}
	ClearAllSaveMailData();
}
void CCDKey::InitCDKeyList(const char* filename)
{
	std::ifstream in_f;
	if( in_f.fail())
		return;
	in_f.open(filename);

	while(true)
	{
		string strCDKey;
		if( in_f.eof())
			break;
		in_f>>strCDKey;
		if(strCDKey.empty())
			break;
		//
		CCDKey* pCDKey = new CCDKey(strCDKey.c_str());
		m_mapCDKeyList.insert(make_pair(pCDKey->m_strCDKey,pCDKey));
		InitCDKeyData(pCDKey);
	}
	in_f.close();
}
void CCDKey::InitCDKeyData(CCDKey* pCDKey)
{
	if(pCDKey)
	{
		std::ifstream in_f;
		char filename[128];
		sprintf(filename,"PlayerData//%s//GoodsAmountByIndex.ini",pCDKey->m_strCDKey.c_str());
		in_f.open(filename);
		if( !in_f.is_open())
			return;

		string strFlag;
		in_f>>strFlag>>strFlag>>strFlag;
		//
		while( true)
		{
			if( in_f.eof() )
				break;
			DWORD dwIndex,dwAmount;
			in_f>>dwIndex
				>>dwAmount;

			pCDKey->m_mapGoodsAmountByIndex[dwIndex] = dwAmount;
		}
		in_f.close();

		std::ifstream in_f2;
		sprintf(filename,"PlayerData//%s//Level.txt",pCDKey->m_strCDKey.c_str());
		in_f2.open(filename);
		while (1)
		{
			if( !in_f2.is_open() )
				break;

			if( in_f2.eof() )
				break;
			DWORD l;
			in_f2>>l;
			pCDKey->SetLevel(l);
			break;
		}
		in_f2.close();
	}
}
void CCDKey::Release()
{
	if( !m_mapCDKeyList.empty() )
	{
		hash_map<string,CCDKey*>::iterator it = m_mapCDKeyList.begin();
		for(;it != m_mapCDKeyList.end();)
		{
			CCDKey* pCDKey = (*it).second;
			SAFE_DELETE(pCDKey);
			m_mapCDKeyList.erase(it++);
		}
	}
}

CCDKey* CCDKey::GetCDKey(const char* strCDKey)
{
	hash_map<string,CCDKey*>::iterator it = m_mapCDKeyList.find(strCDKey);
	if( it != m_mapCDKeyList.end())
	{
		CCDKey* pCDKey = (*it).second;
		return pCDKey;
	}
	return NULL;
}
void CCDKey::AddCDKey(CCDKey* pCDKey)
{
	if( m_mapCDKeyList.find(pCDKey->m_strCDKey) == m_mapCDKeyList.end() )
	{
		FILE* f;
		string filename = "CDKey//CDKeyList.ini";
		if( (f = fopen(filename.c_str(),"a+")) == NULL )
			return;
		fseek(f,0,SEEK_END);
		fwrite(pCDKey->m_strCDKey.c_str(),strlen(pCDKey->m_strCDKey.c_str()),1,f);
		fwrite("\n",1,1,f);
		fclose(f);

		m_mapCDKeyList.insert(make_pair(pCDKey->m_strCDKey,pCDKey));
	}
}
BOOL CCDKey::FindCDKey(const char* strCDKey)
{ 
	hash_map<string,CCDKey*>::iterator it = m_mapCDKeyList.find(strCDKey);
	if( it != m_mapCDKeyList.end() )
	{
		return TRUE;
	}
	return FALSE;
}

void CCDKey::CheckPlayerData(CPlayer* pPlayer,eSaveDataType eType)
{
	switch(eType)
	{
	case eSAVE_GOODS:
		CheckGoods(pPlayer);
		break;
	case eSAVE_MAIL:
		CheckMailData(pPlayer);
		break;
	case eSAVE_LINKMAN:
		CheckSaveLinkManPanleGroup(pPlayer);
		break;
	case eSAVE_FACTION:
		CheckFaction(pPlayer);
		break;
	case eSAVE_LEVEL:
		CheckLevel(pPlayer);
		break;
	case eSAVE_EQUIP:
		CheckEquip(pPlayer);
		//CheckGoods(pPlayer);
		break;
	default:
		break;
	}
}
void CCDKey::AddPlayerData(CPlayer* pPlayer,eSaveDataType eType)
{
	pPlayer->GetExID().tostring(szPlayerGuid);

	switch(eType)
	{
	case eSAVE_MAIL:
		SavaMailData(pPlayer);
		break;
	case eSAVE_GOODS:
		SaveGoodsData(pPlayer);
		break;
	case eSAVE_QUEST:
		SaveQuest(pPlayer);
		break;
	case eSAVE_SKILL:
		SaveSkill(pPlayer);
		break;
	case eSAVE_LINKMAN:
		AddSaveLinkManPanleGroupByTxt(pPlayer);
		break;
	case eSAVE_FACTION:
		AddSaveFactionByTxt(pPlayer);
		break;
	case eSAVE_LEVEL:
		SaveLevel(pPlayer);
		break;
	case eSAVE_EQUIP:
		SaveEquip(pPlayer);
		SaveGoodsData(pPlayer);
		break;
	default:
		break;
	}
}

void CCDKey::SaveEquip(CPlayer* pPlyaer)
{
	std::ofstream out_f;
	string strPath = "PlayerData//";
	string strCDKeyFolder = strPath + m_strCDKey;
	//以CDKey为名创建文件夹
	CreateDirectory(strCDKeyFolder.c_str(),NULL);
	//文件名
	string strFileName = strCDKeyFolder + "//EquipInfo.ini";
	//打开文件
	out_f.open(strFileName.c_str(),ios::out);

	for(int i=0;i<EQUIP_SUM;i++)
	{
		CGoods* pGoods = pPlyaer->GetGoodsMgr()->GetEquipMgr()->GetEquip(i);
		if( pGoods != NULL )
		{
			char strInput[128];
			//equip item pos
			sprintf(strInput,"%d",i);
			WriteOutInfo(out_f,strInput);
			//index
			sprintf(strInput,"%d",pGoods->GetIndex());
			WriteOutInfo(out_f,strInput);
			//容器的坐标
			pGoods->GetExID().tostring(strInput);
			WriteOutInfo(out_f,strInput);
			//换行
			out_f.put('\n');
			//遍历每个容器,写入数据

			tagCDKeyEquipInfo * pInfo = new tagCDKeyEquipInfo;
			pInfo->dwGoodsIndex = pGoods->GetIndex();
			pInfo->dwItemPos = i;
			strcpy(pInfo->szGuid,strInput);

			m_mapCDKeyEquipInfo[i] = pInfo;
		}
	}
}

void CCDKey::CheckEquip(CPlayer* pPlayer)
{
	char szGuid[256];
	pPlayer->GetExID().tostring(szGuid);
	if( strcmp(szGuid,szPlayerGuid) )
	{
		char szInfo[512];
		//		OutputContainerInfo(m_strCDKey.c_str());
		//		OutputContainerInfo(pPlayer->GetName());
		sprintf(szInfo,"OldGuid:%s",szPlayerGuid);
		//		OutputContainerInfo(szInfo);

		sprintf(szInfo,"NewGuid:%s",szGuid);
		// 		OutputContainerInfo(szInfo);

		//		OutputContainerInfo("删除");
		return ;
	}
	DWORD dwEquipCounts = 0;
	//	遍历玩家每个装备栏位
	for(int i=0;i<EQUIP_SUM;i++)
	{
		CGoods* pGoods = pPlayer->GetGoodsMgr()->GetEquipMgr()->GetEquip(i);
		if( pGoods != NULL )
		{
			dwEquipCounts++;
		}
	}
	if( m_mapCDKeyEquipInfo.size() != dwEquipCounts )
	{
		// 		OutputContainerInfo(m_strCDKey.c_str());
		//		OutputContainerInfo(pPlayer->GetName());
		//		OutputContainerInfo(szPlayerGuid);
		//		OutputContainerInfo("丢失");
		//	MessageBox(NULL,"Equip Lost!","Msg",MB_OK);
	}
	return;
	// 	{
	// 		{
	// 			//	在存盘数据中查找
	// 			map<DWORD,tagCDKeyEquipInfo*>::iterator it = m_mapCDKeyEquipInfo.find(i);
	// 			if( it != m_mapCDKeyEquipInfo.end() )
	// 			{
	// 				if( (*it).second->dwGoodsIndex != pGoods->GetIndex() )
	// 				{
	// 					OutputContainerInfo(m_strCDKey.c_str());
	// 					char szInfo[256];
	// 					sprintf(szInfo,"装备栏位置:%d",i);
	// 					OutputContainerInfo(szInfo);
	// 
	// 					sprintf(szInfo,"存盘Index:%d",(*it).second->dwGoodsIndex );
	// 					OutputContainerInfo(szInfo);
	// 
	// 					sprintf(szInfo,"内存Index:%d",pGoods->GetIndex());
	// 					OutputContainerInfo(szInfo);
	// 				}
	// 
	// 				char szGUid1[128];
	// 				char szGudi2[128];
	// 				pGoods->GetExID().tostring(szGUid1);
	// 				strcpy(szGudi2,(*it).second->szGuid);
	// 
	// 				if( strcmp(szGudi2,szGUid1))
	// 				{
	// 					OutputContainerInfo(m_strCDKey.c_str());
	// 					char szInfo[256];
	// 					sprintf(szInfo,"装备栏位置:%d",i);
	// 					OutputContainerInfo(szInfo);
	// 
	// 					sprintf(szInfo,"存盘Guid:%s",szGudi2);
	// 					OutputContainerInfo(szInfo);
	// 
	// 					sprintf(szInfo,"内存Guid:%s",szGUid1);
	// 					OutputContainerInfo(szInfo);
	// 				}
	// 			}
	// 		}
	// 	}
}

void CCDKey::SaveLevel(CPlayer * pPlayer)
{
// 	m_lLevel = pPlayer->GetLevel();
// 	std::ofstream out_f;
// 	string strPath = "PlayerData//";
// 	string strCDKeyFolder = strPath + m_strCDKey;
// 	//以CDKey为名创建文件夹
// 	CreateDirectory(strCDKeyFolder.c_str(),NULL);
// 	//文件名
// 	string strFileName = strCDKeyFolder + "//Level.txt";
// 	//打开文件
// 	out_f.open(strFileName.c_str(),ios::out);
// 
// 	char strInput[128];
// 	sprintf(strInput,"%d",m_lLevel);
// 	WriteOutInfo(out_f,strInput);
// 	out_f.close();
}

void CCDKey::CheckLevel(CPlayer * pPlayer)
{
// 	if( m_lLevel != pPlayer->GetLevel() )
// 	{
// 		char strOutInfo[128];
// 		sprintf(strOutInfo,"%s,原：%d <<--->> 现:%d",m_strCDKey.c_str(),m_lLevel,pPlayer->GetLevel());
// 		//		PutPlayerDataInfo(strOutInfo);
// 	}
}


void CCDKey::SaveGoodsData(CPlayer* pPlayer)
{
	//清空
	m_mapGoodsAmountByIndex.clear();
	//装备vld
	// 	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	// 	{
	// 		CGoods* pEquip = pPlayer->GetEquip(i);
	// 		if(pEquip!= NULL)
	// 		{
	// 			AddAmountByGoods(pEquip);
	// 		}
	// 	}
	//原始背包
// 	list<tagGoods>* goodsList = pPlayer->GetMainPacket();
// 	list<tagGoods>::iterator iter = goodsList->begin();
// 	for(;iter != goodsList->end();iter++)
// 	{
// 		CGoods* pGoods = iter->pGoods;
// 		if(pGoods!=NULL)
// 			SaveGoodsCounts(pGoods);
// 	}
// 	//子背包
// 	for(int i=0;i<COTE_NUM;i++)
// 	{
// 		tagCote* pCote = pPlayer->GetCoteGoods(i);
// 		if( pCote!=NULL && pCote->pGoods != NULL)
// 		{
// 			SaveGoodsCounts(pCote->pGoods);
// 			//
// 			list<tagGoods>::iterator it = pCote->m_listCoteGoods.begin();
// 			for(;it != pCote->m_listCoteGoods.end();it++)
// 			{
// 				SaveGoodsCounts(it->pGoods);
// 			}
// 		}
// 	}
// 
// 	std::ofstream out_f;
// 	string strPath = "PlayerData//";
// 	string strCDKeyFolder = strPath + m_strCDKey;
// 	//以CDKey为名创建文件夹
// 	CreateDirectory(strCDKeyFolder.c_str(),NULL);
// 	//文件名
// 	string strFileName = strCDKeyFolder + "//GoodsAmountByIndex.ini";
// 	//打开文件
// 	out_f.open(strFileName.c_str(),ios::out);
// 	//写入项
// 	out_f.write("Index	",		sizeof("#Index"));
// 	out_f.write("数量	",		sizeof("数量"));
// 	out_f.write("#",		sizeof("#"));
// 	//换行
// 	out_f.put('\n');	
// 	//遍历每个容器,写入数据
// 	map<DWORD,DWORD>::iterator Iter = m_mapGoodsAmountByIndex.begin();
// 	for(;Iter!=m_mapGoodsAmountByIndex.end();Iter++)
// 	{
// 		char strInput[128];
// 		//index
// 		sprintf(strInput,"%d",Iter->first);
// 		WriteOutInfo(out_f,strInput);
// 		//容器的坐标
// 		sprintf(strInput,"%d",Iter->second);
// 		WriteOutInfo(out_f,strInput);
// 		//换行
// 		out_f.put('\n');
// 	}
// 	out_f.close();
}

void CCDKey::CheckGoods(CPlayer* pPlayer)
{
	//构造一张临时map存放player身上index物品的数量信息
	map<DWORD,DWORD> mapGoodsTemp;
	// 	//装备
	// 	for(int i=0;i<CPlayer::EQUIP_SUM;i++)
	// 	{
	// 		CGoods* pEquip = pPlayer->GetEquip(i);
	// 		if(pEquip!= NULL)
	// 		{
	// 			DWORD dwGoodsIndex = pEquip->GetIndex();
	// 			DWORD dwGoodsNum = pEquip->GetNum();
	// 
	// 			//查找map中是否有该index的物品
	// 			map<DWORD,DWORD>::iterator it = mapGoodsTemp.find(dwGoodsIndex);
	// 
	// 			//找到,则在数量上递加
	// 			if( it!=mapGoodsTemp.end())
	// 			{
	// 
	// 				DWORD dwNum = it->second;
	// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum + dwNum;
	// 			}
	// 			//没有找到,则新加入
	// 			else
	// 			{
	// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum;
	// 			}
	// 		}
	// 	}
	//原始背包
// 	list<tagGoods>* goodsList = pPlayer->GetMainPacket();
// 	list<tagGoods>::iterator iter = goodsList->begin();
// 	for(;iter != goodsList->end();iter++)
// 	{
// 		CGoods* pGoods = iter->pGoods;
// 		if(pGoods!=NULL)
// 		{
// 			DWORD dwGoodsIndex = pGoods->GetIndex();
// 			DWORD dwGoodsNum = pGoods->GetNum();
// 
// 			//查找map中是否有该index的物品
// 			map<DWORD,DWORD>::iterator it = mapGoodsTemp.find(dwGoodsIndex);
// 
// 			//找到,则在数量上递加
// 			if( it!=mapGoodsTemp.end())
// 			{
// 
// 				DWORD dwNum = it->second;
// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum + dwNum;
// 			}
// 			//没有找到,则新加入
// 			else
// 			{
// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum;
// 			}
// 		}
// 	}
// 	//子背包
// 	for(int i=0;i<COTE_NUM;i++)
// 	{
// 		tagCote* pCote = pPlayer->GetCoteGoods(i);
// 		if( pCote!=NULL && pCote->pGoods != NULL)
// 		{
// 			DWORD dwGoodsIndex = pCote->pGoods->GetIndex();
// 			DWORD dwGoodsNum = pCote->pGoods->GetNum();
// 
// 			//查找map中是否有该index的物品
// 			map<DWORD,DWORD>::iterator itCote = mapGoodsTemp.find(dwGoodsIndex);
// 
// 			//找到,则在数量上递加
// 			if( itCote!=mapGoodsTemp.end())
// 			{
// 
// 				DWORD dwNum = itCote->second;
// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum + dwNum;
// 			}
// 			//没有找到,则新加入
// 			else
// 			{
// 				mapGoodsTemp[dwGoodsIndex] = dwGoodsNum;
// 			}
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			list<tagGoods>::iterator itGoods = pCote->m_listCoteGoods.begin();
// 			for(;itGoods != pCote->m_listCoteGoods.end();itGoods++)
// 			{
// 				DWORD dwGoodsIndex = itGoods->pGoods->GetIndex();
// 				DWORD dwGoodsNum = itGoods->pGoods->GetNum();
// 
// 				//查找map中是否有该index的物品
// 				map<DWORD,DWORD>::iterator it = mapGoodsTemp.find(dwGoodsIndex);
// 
// 				//找到,则在数量上递加
// 				if( it!=mapGoodsTemp.end())
// 				{
// 
// 					DWORD dwNum = it->second;
// 					mapGoodsTemp[dwGoodsIndex] = dwGoodsNum + dwNum;
// 				}
// 				//没有找到,则新加入
// 				else
// 				{
// 					mapGoodsTemp[dwGoodsIndex] = dwGoodsNum;
// 				}
// 			}
// 		}
// 	}
	//////////////////////////////////////////////////////////////////////////
	//开始比较
	char strOutInfo[128];
	static const char* szGoodsCheck = "GoodsCheckRet.log";
	//如果2个map表大小不一样,说明数据丢失
	if(mapGoodsTemp.size() != m_mapGoodsAmountByIndex.size())
	{
		sprintf(strOutInfo,"(%s)物品种类数量不一致: Start",m_strCDKey.c_str());
		PutStringToFile(szGoodsCheck,strOutInfo);
		//		PutPlayerDataInfo(strOutInfo);
		for(map<DWORD,DWORD>::iterator it = mapGoodsTemp.begin();it != mapGoodsTemp.end();it++)
		{
			DWORD dwIndex = it->first;
			sprintf(strOutInfo,"	<现有> 物品索引:%d",dwIndex);
			PutStringToFile(szGoodsCheck,strOutInfo);
		}

		for(map<DWORD,DWORD>::iterator it = m_mapGoodsAmountByIndex.begin(); it!= m_mapGoodsAmountByIndex.end();it++)
		{
			DWORD dwIndex = it->first;
			sprintf(strOutInfo,"	<存盘> 物品索引:%d",dwIndex);
			PutStringToFile(szGoodsCheck,strOutInfo);
		}
		PutStringToFile(szGoodsCheck,strOutInfo);
	}

	//详细比较单个物品种类的数量
	map<DWORD,DWORD>::iterator indexIter = m_mapGoodsAmountByIndex.begin();
	for(;indexIter != m_mapGoodsAmountByIndex.end();indexIter++)
	{
		DWORD dwIndex = indexIter->first;
		DWORD dwNum = indexIter->second;

		//存盘的物品在玩家身上可以找到
		if(mapGoodsTemp.find(dwIndex)!=mapGoodsTemp.end())
		{
			//数量不等,说明数量有变化
			if( mapGoodsTemp[dwIndex] < dwNum )
			{
				sprintf(strOutInfo,"(%s)物品索引<%d>数量错误,原有(%d)个;现有(%d)个",m_strCDKey.c_str(),dwIndex,dwNum,mapGoodsTemp[dwIndex]);
				PutStringToFile(szGoodsCheck,strOutInfo);
			}
		}
		else
			//如果没找到,说明物品丢失
		{
			sprintf(strOutInfo,"(%s)物品索引<%d>在玩家身上丢失",m_strCDKey.c_str(),dwIndex);
			PutStringToFile(szGoodsCheck,strOutInfo);
		}
	}
}
void CCDKey::SaveGoodsCounts(CGoods* pGoods)
{
	if(pGoods!=NULL)
	{
		DWORD dwGoodsIndex = pGoods->GetIndex();
		DWORD dwGoodsNum = pGoods->GetNum();

		//查找map中是否有该index的物品
		map<DWORD,DWORD>::iterator it = m_mapGoodsAmountByIndex.find(dwGoodsIndex);

		//找到,则在数量上递加
		if( it!=m_mapGoodsAmountByIndex.end())
		{

			DWORD dwNum = it->second;
			m_mapGoodsAmountByIndex[dwGoodsIndex] = dwGoodsNum + dwNum;
		}
		//没有找到,则新加入
		else
		{
			m_mapGoodsAmountByIndex[dwGoodsIndex] = dwGoodsNum;
		}
	}
}

void CCDKey::SaveQuest(CPlayer* pPlayer)
{
	m_mapQuestInfo.clear();
// 	list<tagRoleRecord*>* pList = pPlayer->GetQuestList();
// 	if(pList)
// 	{
// 		if( !pList->empty() )
// 		{
// 			list<tagRoleRecord*>::iterator it = pList->begin();
// 			for(;it != pList->end();it++)
// 			{
// 				m_mapQuestInfo[(*it)->m_lRoleID] = true;
// 			}
// 		}
// 	}
}
void CCDKey::CheckQuest(CPlayer* pPlayer)
{
//	char strOutInfo[256];
// 	list<tagRoleRecord*>* pList = pPlayer->GetQuestList();
// 	if(pList)
// 	{
// 		if( pList->empty() )
// 		{
// 			sprintf(strOutInfo,"(%s)任务丢失",m_strCDKey.c_str());
// 			//			PutPlayerDataInfo(strOutInfo);
// 		}
// 		else
// 		{
// 			list<tagRoleRecord*>::iterator it = pList->begin();
// 			for(;it != pList->end();it++)
// 			{
// 				if( !(m_mapQuestInfo.find((*it)->m_lRoleID) != m_mapQuestInfo.end()) )
// 				{
// 					sprintf(strOutInfo,"(%s)任务丢失",m_strCDKey.c_str());
// 					//					PutPlayerDataInfo(strOutInfo);
// 				}
// 			}
// 		}
// 	}
}
void CCDKey::SaveSkill(CPlayer* pPlayer)
{
	return;
	std::ofstream out_f;
	string strPath = "PlayerData//";
	string strCDKeyFolder = strPath + m_strCDKey;
	//以CDKey为名创建文件夹
	CreateDirectory(strCDKeyFolder.c_str(),NULL);
	//文件名
	string strFileName = strCDKeyFolder + "//SkillInfo.ini";
	//打开文件
	out_f.open(strFileName.c_str(),ios::out);
	//写入项
	out_f.write("Index	",		sizeof("#Index"));
	out_f.write("#",		sizeof("#"));
	//换行
	out_f.put('\n');
	char strInput[128];

	m_mapSkillInfo.clear();
	
	list<tagPlayerSkill> pList = pPlayer->GetSkillMgr()->GetSkillList();
	list<tagPlayerSkill>::iterator it = pList.begin();
	for(;it!=pList.end();it++)
	{
		m_mapSkillInfo[it->dwID] = true;
		sprintf(strInput,"%d",it->dwID);
		WriteOutInfo(out_f,strInput);
		out_f.put('\n');
	}
	out_f.close();
}

void CCDKey::CheckSkill(CPlayer* pPlayer)
{
	char strOutInfo[256];
	list<tagPlayerSkill> pList = pPlayer->GetSkillMgr()->GetSkillList();;
	if( pList.empty())
	{
		sprintf(strOutInfo,"(%s)技能为空",m_strCDKey.c_str());
		//		PutPlayerDataInfo(strOutInfo);
	}
	else
	{
		list<tagPlayerSkill>::iterator it = pList.begin();
		for(;it != pList.end();it++)
		{
			if( !(m_mapSkillInfo.find(it->dwID) != m_mapSkillInfo.end()) )
			{
				sprintf(strOutInfo,"(%s):丢失技能(%d)",m_strCDKey.c_str(),it->dwID);
				//				PutPlayerDataInfo(strOutInfo);
			}
		}
	}
}

void CCDKey::WriteOutInfo(std::ofstream & out_f,const char* strOutInfo)
{
	out_f.write(strOutInfo,strlen(strOutInfo));
	out_f.put('	');
}

//-----------------------------------------------------------------------------------------
// 对玩家身上的邮件进行数据保存和核对
// by MartySa 2009.1.5
// 函数定义部分
//-----------------------------------------------------------------------------------------

//load from .txt
void CCDKey::AddSaveMailDataByTxt(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// }
	// 
	// 
	// 
	// 	CStdioFile file;
	// 
	// 	//打开文件
	// 
	// 	//设置名字
	// 	std::string strcdkey	 = m_strCDKey.c_str();
	// 	std::string strchar		 = "//";
	// 	std::string strfrist	 = "PlayerData//";
	// 	std::string strend       = "mail.txt";
	// 	std::string strtempname  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 	string cstrBuff;		//读行buff
	// 	string  tempBuff;		
	// //////////////////////////////////////////////////////////////////////////
	// 	std::ifstream in_f;
	// 	if( in_f.fail())
	// 		return;
	// 	in_f.open(strtempname);
	// 
	// 	while(true)
	// 	{
	// 		string strCDKey;
	// 		if( in_f.eof())
	// 			break;
	// 		in_f>>strCDKey;
	// 		if(strCDKey.empty())
	// 			break;
	// 
	// /////////////////////////////////////////////////////////////////////////
	// 
	// 	if (file.Open(strtempname.c_str(), CFile::modeRead))
	// 	{
	// 		while (file.ReadString(cstrBuff.c_str()))				//读行
	// 		{	
	// 			//文本中的邮件数量 
	// 			tempBuff   = GetCStringToString(cstrBuff, ":");
	// 			long lSize = atoi(tempBuff.c_str());
	// 
	// 			for (long i = 0; i!= lSize; ++i)
	// 			{	
	// 				stMailData maildata;
	// 				
	// 				//读取邮件名字
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff             = GetCStringToString(cstrBuff, ":");
	// 				strcpy(maildata.strMailName,tempBuff.c_str());				//写入临时tagmail结构
	// 				
	// 				//读取邮件Guid
	// 				file.ReadString(cstrBuff);
	// 				tempBuff             = GetCStringToString(cstrBuff, ":");	
	// 				CGUID guid(tempBuff.c_str());					//构造一个临时guid	
	// 				maildata.gMailID     = guid;
	// 
	// 				//读取邮件内容
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff             = GetCStringToString(cstrBuff, ":");
	// 				strcpy(maildata.strMailDesc,tempBuff.c_str());				//写入临时tagmail结构
	// 				
	// 				//读取邮件收到的时间
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff             = GetCStringToString(cstrBuff, ":");
	// 				long num = atoi(tempBuff.c_str());
	// 				maildata.dwTimeStart = num;						//写入临时tagmail结构
	// 				
	// 				//读取邮件类型
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff            = GetCStringToString(cstrBuff, ":");
	// 				num = atoi(tempBuff.c_str());
	// 				maildata.eMailType	= (eMAILTYPE)num;			//写入临时tagmail结构
	// 				
	// 				//读取附件金钱
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff            = GetCStringToString(cstrBuff, ":");
	// 				num		 = atoi(tempBuff.c_str());
	// 				maildata.dwGoldNum	= num;						//写入临时tagmail结构
	// 				
	// 				//读取附件物品数量
	// 				file.ReadString(cstrBuff);
	// 
	// 				tempBuff            = GetCStringToString(cstrBuff, ":");
	// 				num		 = atoi(tempBuff.c_str());
	// 				
	// 				for (long j = 0; j!=num; ++j)
	// 				{	
	// 					CGoods *goods  = new CGoods;							//在Lua下调用释放，崩溃时在析构
	// 					//读取附件物品名
	// 					file.ReadString(cstrBuff);
	// 					tempBuff       = GetCStringToString(cstrBuff, ":");		
	// 					goods->SetName(tempBuff.c_str());					//写入临时goods结构
	// 
	// 					//读取物品GUID
	// 					file.ReadString(cstrBuff);
	// 					tempBuff       = GetCStringToString(cstrBuff, ":");	
	// 					CGUID guid(tempBuff.c_str());
	// 					goods->SetExID(guid);								//写入临时goods结构
	// 					
	// 					//读取附件物品数量
	// 					file.ReadString(cstrBuff);
	// 					tempBuff	   = GetCStringToString(cstrBuff, ":");	
	// 					long goodsnum = atoi(tempBuff.c_str());
	// 					goods->SetNum(goodsnum);							//写入临时goods结构
	// 					
	// 					//读取附件物品下标
	// 					file.ReadString(cstrBuff);
	// 					tempBuff	   = GetCStringToString(cstrBuff, ":");	
	// 					goodsnum      = atoi(tempBuff.c_str());
	// 					goods->SetIndex(goodsnum);							//写入临时goods结构
	// 
	// 					maildata.GoodsList.push_back(goods);
	// 				}
	// 				
	// 				m_vecSaveMailData.push_back(maildata);
	// 			}
	// 		}
	// 
	// 		file.Close();
	// 	}
	// 	else
	// 	{	
	// 		//如果当前目录下没有保存文件存在
	// 		SavaMailData(pPlayer);
	// 	}
	// 	
}

//save to .txt
void CCDKey::SavaMailData(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 
	// 		CStdioFile file;
	// 
	// 	std::string strchar  = "//";
	// 	std::string strcdkey = m_strCDKey.c_str();
	// 	std::string strfrist = "PlayerData//";
	// 	std::string strend   = "mail.txt";
	// 	std::string strtemp  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 		//创建文件甲
	// 		std::string strPath = "PlayerData//";
	// 	std::string strCDKeyFolder = strPath+strcdkey;
	// 		//创建文件夹
	// 		CreateDirectory(strCDKeyFolder.c_str(), NULL);
	// 		//写入文件
	// 	if (file.Open(strtemp.c_str(), CFile::modeCreate | CFile::modeWrite))
	// 		{
	// 			vector<stMailData> maildata = pPlayer->GetMailData();
	// 			long lMailSize              = (long)maildata.size();
	// 			//写入邮件大小
	// 			string buff;
	// 			buff.Format("%d", lMailSize);
	// 		buff = GetWriteCString(2, "邮件总数:", buff);
	// 			file.WriteString(buff);
	// 		file.WriteString("\n");
	// 			if (lMailSize > 0)
	// 			{
	// 				for (int i=0; i!=lMailSize; ++i)
	// 				{	
	// 					//写入邮件名
	// 				buff = GetWriteCString(2, "[[[[[ 邮件名字 ]]]]]:", maildata[i].strMailName);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 
	// 				//写入邮件guid
	// 				CGUID guid    = maildata[i].gMailID;
	// 				char *pchGuid = new char[125]; 
	// 				guid.tostring(pchGuid);
	// 				buff          = GetWriteCString(2, "邮件Guid:", pchGuid);
	// 				delete []pchGuid;
	// 				pchGuid       = NULL;
	// 				file.WriteString(buff);
	// 				file.WriteString("\n");
	// 					
	// 					//写入邮件内容
	// 				buff = GetWriteCString(2, "邮件内容:", maildata[i].strMailDesc);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 					
	// 					//写入邮件收到时间
	// 					buff.Format("%d", maildata[i].dwTimeStart); 
	// 				buff = GetWriteCString(2, "收到时间:", buff);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 					
	// 					//写入邮件类型
	// 					buff.Format("%d", maildata[i].eMailType); 
	// 				buff = GetWriteCString(2, "邮件类型:", buff);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 					
	// 					//写入邮件附件金钱
	// 					buff.Format("%d", maildata[i].dwGoldNum); 
	// 				buff = GetWriteCString(2, "附件金钱:", buff);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 					
	// 					//写入附件物品数量
	// 					long goodsnum = maildata[i].GoodsList.size();
	// 					buff.Format("%d", goodsnum);
	// 				buff = GetWriteCString(2, "附件物品:", buff);
	// 					file.WriteString(buff);
	// 				file.WriteString("\n");
	// 
	// 					if (goodsnum > 0)
	// 					{					
	// 						list<CGoods*>::iterator it = maildata[i].GoodsList.begin();
	// 						for ( ; it!= maildata[i].GoodsList.end(); ++it)
	// 						{	
	// 							//写入物品名字
	// 						buff = GetWriteCString(2, " 物品名:", (*it)->GetName());
	// 							file.WriteString(buff);
	// 						file.WriteString("\n");
	// 
	// 						//写入物品GUID
	// 						CGUID guid  = (*it)->GetExID();
	// 						char *pchGuid = new char[125]; 
	// 						guid.tostring(pchGuid);
	// 						buff = GetWriteCString(2, " Guid:", pchGuid);
	// 						delete []pchGuid;
	// 						pchGuid = NULL;
	// 						file.WriteString(buff);
	// 						file.WriteString("\n");
	// 
	// 							//写入物品数量
	// 							buff.Format("%d", (*it)->GetNum()); 
	// 						buff = GetWriteCString(2, " 物品数:", buff);
	// 							file.WriteString(buff);
	// 						file.WriteString("\n");
	// 
	// 							//写入物品下标
	// 							buff.Format("%d", (*it)->GetIndex()); 
	// 						buff = GetWriteCString(2, " 物品标:", buff);
	// 							file.WriteString(buff);
	// 						file.WriteString("\n");
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	// 
	// 		file.Close();
}
// //check maildata
void CCDKey::CheckMailData(CPlayer *pPlayer)
{	
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 
	// 	vector<stMailData> maildata = pPlayer->GetMailData();		//保存当前次玩家身上邮件列表
	// 	if ( m_vecSaveMailData.empty() )
	// 	{	
	// 		//如果没读取成功就读取
	// 		AddSaveMailDataByTxt(pPlayer);
	// 
	// 		return ;
	// 	}
	// 	else if ( maildata.empty() )
	// 	{
	// 		return ;
	// 	}
	// 	//检测开始
	// 	//检查邮件总数
	// 	long iSaveMailSize   = m_vecSaveMailData.size();				//文本中保存邮件数据大小
	// 	long iPlayerMailSize = maildata.size();							//当前玩家身上的邮件数据大小
	// 	string strPlayerName = pPlayer->GetName();
	// 	string strWirte;
	// 	string cstrInt;
	// 	if ( iSaveMailSize < iPlayerMailSize )
	// 	{	
	// 		strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件总数不一样");
	// 		PutCheckMailErr(strWirte.c_str());
	// 
	// 		return ;
	// 	}
	// 	else if (iSaveMailSize == iPlayerMailSize) 
	// 	{
	// 		//检查邮件内数据
	// 		for (int i=0; i!=iSaveMailSize; ++i)
	// 		{	
	// 			//检查邮件guid
	// 			if (m_vecSaveMailData[i].gMailID != maildata[i].gMailID)
	// 			{	
	// 				cstrInt.Format("%d", i+1);
	// 				strWirte = cstrInt.GetBuffer();
	// 				cstrInt.ReleaseBuffer();
	// 				strWirte = GetWirteString(2, "当前邮件封数为:", strWirte.c_str());
	// 				PutCheckMailErr(strWirte.c_str());
	// 
	// 				strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件guid不一样");
	// 				PutCheckMailErr(strWirte.c_str());
	// 				return ;
	// 			}
	// 			////检查邮件类型
	// 			//if (m_vecSaveMailData[i].eMailType != maildata[i].eMailType)
	// 			//{	
	// 			//	cstrInt.Format("%d", i+1);
	// 			//	strWirte = cstrInt.GetBuffer();
	// 			//	cstrInt.ReleaseBuffer();
	// 			//	strWirte = GetWirteString(2, "当前邮件封数为:", strWirte);
	// 			//	PutCheckMailErr(strWirte.c_str());
	// 			//	
	// 			//	strWirte = GetWirteString(2, strPlayerName, "邮件类型不一样");
	// 			//	PutCheckMailErr(strWirte.c_str());
	// 			//	return ;
	// 			//}
	// 			//检查邮件内金钱
	// 			if ( m_vecSaveMailData[i].dwGoldNum < maildata[i].dwGoldNum )
	// 			{
	// 				cstrInt.Format("%d", i+1);
	// 				strWirte = cstrInt.GetBuffer();
	// 				cstrInt.ReleaseBuffer();
	// 				strWirte = GetWirteString(2, "当前邮件封数为:", strWirte.c_str());
	// 				PutCheckMailErr(strWirte.c_str());	
	// 
	// 				strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件金钱不一样");
	// 				PutCheckMailErr(strWirte.c_str());
	// 				return ;
	// 			}
	// 			//检查邮件内物品
	// 			long savemail_goodssize = (m_vecSaveMailData[i].GoodsList).size();
	// 			long mail_goodssize     = (maildata[i].GoodsList).size();
	// 			if ( savemail_goodssize < mail_goodssize )
	// 			{
	// 				cstrInt.Format("%d", i+1);
	// 				strWirte = cstrInt.GetBuffer();
	// 				cstrInt.ReleaseBuffer();
	// 				strWirte = GetWirteString(2, "当前邮件封数为:", strWirte.c_str());
	// 				PutCheckMailErr(strWirte.c_str());
	// 
	// 				strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件内物品总数不一样");
	// 				PutCheckMailErr(strWirte.c_str());
	// 				return ;
	// 			}
	// 			else if(savemail_goodssize == mail_goodssize)
	// 			{	
	// 				//物品Guid
	// 				CGUID guidGoods_1  = CGUID::GUID_INVALID;
	// 				CGUID guidGoods_2  = CGUID::GUID_INVALID;
	// 				//物品数量
	// 				long lGoodsSize_1  = 0;
	// 				long lGoodsSize_2  = 0;
	// 				//物品下标
	// 				long lGoodsIndex_1 = 0;
	// 				long lGoodsIndex_2 = 0;
	// 
	// 				list<CGoods*>::iterator ito_MailData     = maildata[i].GoodsList.begin();
	// 				list<CGoods*>::iterator ito_SaveMailData = m_vecSaveMailData[i].GoodsList.begin();
	// 
	// 				//计算当前第几的个goods
	// 				int goodsnum = 0;	
	// 
	// 				//双vec:itor跌加计算
	// 				for ( ; ito_MailData != maildata[i].GoodsList.end(), ito_SaveMailData != m_vecSaveMailData[i].GoodsList.end(); 
	// 					++ito_MailData, ++ito_SaveMailData)
	// 				{	
	// 					++goodsnum;										//自跌加
	// 
	// 					guidGoods_1   = (*ito_MailData)->GetExID();
	// 					lGoodsSize_1  = (*ito_MailData)->GetNum();
	// 					lGoodsIndex_1 = (*ito_MailData)->GetIndex();
	// 
	// 					guidGoods_2   = (*ito_SaveMailData)->GetExID();
	// 					lGoodsSize_2  = (*ito_SaveMailData)->GetNum();
	// 					lGoodsIndex_2 = (*ito_SaveMailData)->GetIndex();
	// 
	// 					//检查物品数量
	// 					if (lGoodsSize_1 < lGoodsSize_2)
	// 					{	
	// 
	// 						cstrInt.Format("%d", i+1);
	// 						strWirte = cstrInt.GetBuffer();
	// 						cstrInt.ReleaseBuffer();
	// 						strWirte = GetWirteString(2, "当前邮件封数为:", strWirte.c_str());
	// 						PutCheckMailErr(strWirte.c_str());
	// 
	// 						cstrInt.Format("%d", goodsnum);
	// 						strWirte = cstrInt.GetBuffer();
	// 						cstrInt.ReleaseBuffer();
	// 						strWirte = GetWirteString(2, "当前所在goods为:", strWirte.c_str());
	// 						PutCheckMailErr(strWirte.c_str());
	// 
	// 						strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件内单个物叠加数量不一样");
	// 						PutCheckMailErr(strWirte.c_str());
	// 						return ;
	// }
	// 
	// 					//检查物品index
	// 					//if (lGoodsIndex_1 < lGoodsIndex_2)
	// 					//{	
	// 					//	cstrInt.Format("%d", i+1);
	// 					//	strWirte = cstrInt.GetBuffer();
	// 					//	cstrInt.ReleaseBuffer();
	// 					//	strWirte = GetWirteString(2, "当前邮件封数为:", strWirte);
	// 					//	PutCheckMailErr(strWirte.c_str());
	// 
	// 					//	cstrInt.Format("%d", goodsnum);
	// 					//	strWirte = cstrInt.GetBuffer();
	// 					//	cstrInt.ReleaseBuffer();
	// 					//	strWirte = GetWirteString(2, "当前所在goods为:", strWirte);
	// 					//	PutCheckMailErr(strWirte.c_str());
	// 					//	
	// 					//	strWirte = GetWirteString(2, strPlayerName, "邮件内单个物index不一样");
	// 					//	PutCheckMailErr(strWirte.c_str());
	// 					//	return ;
	// 					//}
	// 
	// 					//检查物品guid
	// 
	// 					else if (lGoodsSize_1 == lGoodsSize_2 )
	// 					{	
	// 						if (guidGoods_1 != guidGoods_2)
	// 						{
	// 							cstrInt.Format("%d", i+1);
	// 							strWirte = cstrInt.GetBuffer();
	// 							cstrInt.ReleaseBuffer();
	// 							strWirte = GetWirteString(2, "当前邮件封数为:", strWirte.c_str());
	// 							PutCheckMailErr(strWirte.c_str());
	// 
	// 							cstrInt.Format("%d", goodsnum);
	// 							strWirte = cstrInt.GetBuffer();
	// 							cstrInt.ReleaseBuffer();
	// 							strWirte = GetWirteString(2, "当前所在goods为:", strWirte.c_str());
	// 							PutCheckMailErr(strWirte.c_str());
	// 
	// 							strWirte = GetWirteString(2, strPlayerName.c_str(), "邮件内单个物guid不一样");
	// 							PutCheckMailErr(strWirte.c_str());
	// 							return ;
	// 						}
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
}


//clear svaemaildata
void CCDKey::ClearAllSaveMailData()
{	
	if (!m_vecSaveMailData.empty())
	{	
		vector<stMailData>::iterator it = m_vecSaveMailData.begin();

		for ( ; it!= m_vecSaveMailData.end(); ++it)
		{
			if (!(*it).GoodsList.empty())
			{
				list<CGoods*>::iterator itor = (*it).GoodsList.begin();
				for ( ; itor!=(*it).GoodsList.end(); ++itor)
				{
					SAFE_DELETE(*itor);
				}
				(*it).GoodsList.clear();
			}
		}
		m_vecSaveMailData.clear();
	}
}

//debug for check mail err 
// by MartySa 2009.1.7
void CCDKey::PutCheckMailErr(const char *msg)
{
	static int num=0;
	char t[128];

	FILE *fp;
	if( (fp=fopen("checkmail_Erro.txt","a+")) == NULL )
		return;

	num++;
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(t, "\n%d(%02d-%02d %02d:%02d:%02d):", num, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	fseek(fp, 0, SEEK_END);
	fwrite(t, lstrlen(t), 1, fp);
	fwrite(msg, lstrlen(msg), 1, fp);
	fclose(fp);

	//如果大于 20M  删除debug文件
	if( GetFileLength("checkmail_Erro.txt")/(1024*1024) > 20 )
	{
		DeleteFileA("checkmail_Erro.txt");
	}
}

string CCDKey::GetWriteCString(int cstrSize, ... )
{	
	string cstrBuff, cstrDest;
	char *pStr = NULL;
	va_list vaStr = NULL;
	va_start(vaStr, cstrSize);

	for (int i=0; i!= cstrSize; ++i)
	{
		pStr	 = va_arg(vaStr, char*);
		cstrBuff = pStr;
		cstrDest += cstrBuff;
	}

	return cstrDest;
}

string CCDKey::GetWirteString(int strSize, ...)
{

	char *pStr = NULL;
	va_list vaStr = NULL;
	va_start(vaStr, strSize);

	string strBuff, strDest;

	for (int i=0; i!= strSize; ++i)
	{
		pStr	= va_arg(vaStr, char*);
		strBuff = pStr;
		strDest += strBuff;
	}

	return strDest;
}

//-----------------------------------------------------------------------------------------
// 对玩家身上的好友进行数据保存和核对
// by MartySa 2009.1.12
//-----------------------------------------------------------------------------------------

//	save linkman 
void CCDKey::SaveLinkManPanleGroup(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 
	// 	CStdioFile file;
	// 
	// 	std::string strchar  = "//";
	// 	std::string strcdkey = m_strCDKey.c_str();
	// 	std::string strfrist = "PlayerData//";
	// 	std::string strend   = "linkman.txt";
	// 	std::string strtemp  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 	//创建文件甲
	// 	std::string strPath = "PlayerData//";
	// 	std::string strCDKeyFolder = strPath+strcdkey;
	// 	//创建文件夹
	// 	CreateDirectory(strCDKeyFolder.c_str(), NULL);
	// 	//写入文件
	// 	if (file.Open(strtemp.c_str(), CFile::modeCreate | CFile::modeWrite))
	// 	{	
	// 		//写入分组总数，default为 CPlayer::MAXNUM_PANELS
	// 		string buff;
	// 		buff.Format("%d", (double)CPlayer::MAXNUM_PANELS);
	// 		buff = GetWriteCString(2, "玩家好友分组总数为 :", buff);
	// 		file.WriteString(buff);
	// 		file.WriteString("\n");
	// 		for (int i=0; i!=CPlayer::MAXNUM_PANELS; ++i)
	// 		{
	// 			//循环分别获取不同分组
	// 			CPlayer::SPanel m_playerLinkManPanle = pPlayer->GetSPanle(i);
	// 			//写入当前分组类型
	// 			buff.Format("%d", m_playerLinkManPanle.groupid);
	// 			buff = GetWriteCString(2, " 当前分组为 :", buff);
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 
	// 			//写入人数
	// 			long iMemberSize = (m_playerLinkManPanle.m_PanleMemberData).size();
	// 
	// 			buff.Format("%d", iMemberSize);
	// 			buff = GetWriteCString(2, " 已有联系人数为 :", buff);
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 			//如果该组有联系人
	// 			if ( iMemberSize > 0)
	// 			{
	// 				//逐个联系人写入
	// 				int j = 0;		//计算人数
	// 				list<CPlayer::tagLinkman>::iterator it = m_playerLinkManPanle.m_PanleMemberData.begin();
	// 				for ( ; it!=m_playerLinkManPanle.m_PanleMemberData.end(); ++it)
	// 				{
	// 					//写入联系人名字
	// 					j++;
	// 					buff.Format("%d", j);
	// 					buff = GetWriteCString(2, " 联系人", buff);
	// 					buff = GetWriteCString(3, buff, "名字为 :", (*it).szName);
	// 					file.WriteString(buff);
	// 					file.WriteString("\n");
	// 
	// 					//写入联系人GUID
	// 					CGUID guid    = (*it).guid;
	// 					char *pchGuid = new char[125]; 
	// 					guid.tostring(pchGuid);
	// 					buff          = GetWriteCString(2, "       Guid :", pchGuid);
	// 					delete []pchGuid;
	// 					pchGuid       = NULL;
	// 					file.WriteString(buff);
	// 					file.WriteString("\n");
	// 				}
	// 			}
	// 		}
	// 	}
	// 	file.Close();
}

//load savelinkmanspanel by txt.
void CCDKey::AddSaveLinkManPanleGroupByTxt(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 	//清空保存的数据结构
	// 	//ClearLinkManAllSavePanleGroup();
	// 
	// 	CStdioFile file;
	// 
	// 	//打开文件
	// 
	// 	//设置名字
	// 	std::string strcdkey	 = m_strCDKey.c_str();
	// 	std::string strchar		 = "//";
	// 	std::string strfrist	 = "PlayerData//";
	// 	std::string strend       = "linkman.txt";
	// 	std::string strtempname  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 	string cstrBuff;		//读行buff
	// 	string  tempBuff;		
	// 
	// 	if (file.Open(strtempname.c_str(), CFile::modeRead))
	// 	{	
	// 		while (file.ReadString(cstrBuff))				//读行
	// 		{	
	// 			//读取分组总数
	// 			tempBuff   = GetCStringToString(cstrBuff, ":");
	// 			long lPanelSize = atoi(tempBuff.c_str());
	// 			//逐大组读取写入数据
	// 			for (int i=0; i!= CPlayer::MAXNUM_PANELS; ++i)
	// 			{
	// 				//读取分组类型
	// 				file.ReadString(cstrBuff);
	// 				tempBuff				           = GetCStringToString(cstrBuff, ":");
	// 				long lgroupid					   = atoi(tempBuff.c_str());
	// 				m_SaveLinkManPanelGroup[i].groupid = lgroupid;						//写入保存数据
	// 
	// 				//读取分组联系人人数
	// 				file.ReadString(cstrBuff);
	// 				tempBuff				           = GetCStringToString(cstrBuff, ":");
	// 				long lLinkmanSize				   = atoi(tempBuff.c_str());
	// 				m_SaveLinkManPanelGroup[i].number  = lLinkmanSize;					//写入保存数据
	// 
	// 				if (lLinkmanSize > 0)
	// 				{						
	// 					for (int j=0; j!=lLinkmanSize; ++j)
	// 					{	
	// 						CPlayer::tagLinkman taglinkman;
	// 						//读取单个联系人名字
	// 						file.ReadString(cstrBuff);
	// 						tempBuff				   = GetCStringToString(cstrBuff, ":");
	// 						sprintf(taglinkman.szName, "%s", tempBuff.c_str());
	// 						strncpy(taglinkman.szName, tempBuff.c_str(), 125);
	// 
	// 						//读取单个联系人的guid
	// 						file.ReadString(cstrBuff);
	// 						tempBuff				   = GetCStringToString(cstrBuff, ":");
	// 						CGUID guid(tempBuff.c_str());
	// 						taglinkman.guid            = guid;
	// 						m_SaveLinkManPanelGroup->m_PanleMemberData.push_back(taglinkman);
	// 					}
	// 				}
	// 			}
	// 		}
	// 		file.Close();
	// 	}
	// 	else
	// 	{	
	// 		SaveLinkManPanleGroup(pPlayer);
	// 	}

}

//check linkman
void CCDKey::CheckSaveLinkManPanleGroup(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 
	// 	//
	// 	//检测开始
	// 	string strPlayerName = pPlayer->GetName();
	// 	string strWirte;
	// 	string cstrInt;
	// 
	// 	for (int i=0; i!=CPlayer::MAXNUM_PANELS; ++i)
	// 	{
	// 		CPlayer::SPanel m_playerLinkManPanle = pPlayer->GetSPanle(i);
	// 
	// 		//每组人数总数
	// 		long lSaveMemberSize   = (m_SaveLinkManPanelGroup[i].m_PanleMemberData).size();
	// 		long lPlayerMemberSize = (m_playerLinkManPanle.m_PanleMemberData).size();
	// 		if ( (lSaveMemberSize < lPlayerMemberSize) && lSaveMemberSize > 0)
	// 		{	
	// 			cstrInt.Format("%d", m_playerLinkManPanle.groupid);
	// 			strWirte = cstrInt.GetBuffer();
	// 			cstrInt.ReleaseBuffer();
	// 
	// 			strWirte = GetWirteString(4, strPlayerName.c_str(), "的", strWirte.c_str(), "分组好友总数不一样");
	// 			PutCheckMailErr(strWirte.c_str());
	// 			return ;
	// 		}
	// 		else if ( lSaveMemberSize == lPlayerMemberSize)
	// 		{
	// 			//检查联系人的guid
	// 			int membersize = 0;		//计算人数
	// 			list<CPlayer::tagLinkman>::iterator save_itor   = m_SaveLinkManPanelGroup[i].m_PanleMemberData.begin();
	// 			list<CPlayer::tagLinkman>::iterator player_itor = m_playerLinkManPanle.m_PanleMemberData.begin();
	// 			for ( ; save_itor != m_SaveLinkManPanelGroup[i].m_PanleMemberData.end(), 
	// 				player_itor != m_playerLinkManPanle.m_PanleMemberData.end(); ++save_itor, ++player_itor)
	// 			{	
	// 				membersize++;	
	// 				if ( (*save_itor).guid != (*player_itor).guid)
	// 				{
	// 					cstrInt.Format("%d", m_playerLinkManPanle.groupid);
	// 					strWirte = cstrInt.GetBuffer();
	// 					cstrInt.ReleaseBuffer();
	// 
	// 					string temp_cstrszie;
	// 					string  temp_strsize;
	// 					temp_cstrszie.Format("%d", membersize);
	// 					temp_strsize = cstrInt.GetBuffer();
	// 					temp_cstrszie.ReleaseBuffer();
	// 
	// 					strWirte = GetWirteString(6, strPlayerName.c_str(), "的", strWirte.c_str(), "的分组第", temp_strsize.c_str(), "个联系人Guid不一样");
	// 					PutCheckMailErr(strWirte.c_str());
	// 					break;
	// 					return ;
	// 				}
	// 			}
	// 		}
	// 	}
}

void CCDKey::ClearLinkManAllSavePanleGroup(CPlayer *pPlayer)
{
	for (int i=0; i!=MAXNUM_PANELS; ++i)
	{
		if ( !m_SaveLinkManPanelGroup[i].m_PanleMemberData.empty())
		{	
			list<tagLinkman>::iterator it = m_SaveLinkManPanelGroup[i].m_PanleMemberData.begin();

			for ( ; it!=m_SaveLinkManPanelGroup[i].m_PanleMemberData.end(); ++it)
			{
				(*it).guid = CGUID::GUID_INVALID;
			}
			m_SaveLinkManPanelGroup[i].m_PanleMemberData.clear();
		}
	}

	//清空玩家本身好友数据
//	pPlayer->ClearLinkmanData();
}
//put debug for linkman_err
void CCDKey::PutCheckLinkManErr(const char *msg)
{
	static int num=0;
	char t[128];

	FILE *fp;
	if( (fp=fopen("check_linkman_Erro.txt","a+")) == NULL )
		return;

	num++;
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(t, "\n%d(%02d-%02d %02d:%02d:%02d):", num, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	fseek(fp, 0, SEEK_END);
	fwrite(t, lstrlen(t), 1, fp);
	fwrite(msg, lstrlen(msg), 1, fp);
	fclose(fp);

	//如果大于 20M  删除debug文件
	if( GetFileLength("check_linkman_Erro.txt")/(1024*1024) > 20 )
	{
		DeleteFileA("check_linkman_Erro.txt");
	}
}

//-----------------------------------------------------------------------------------------
// 对玩家身上的好友进行数据保存和核对
// by MartySa 2009.1.13
//-----------------------------------------------------------------------------------------

// save faction .txt
void CCDKey::SaveFaction(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// {
	// 		return ;
	// 	}
	// 
	// 	CStdioFile file;
	// 
	// 	std::string strchar  = "//";
	// 	std::string strcdkey = m_strCDKey.c_str();
	// 	std::string strfrist = "PlayerData//";
	// 	std::string strend   = "faction.txt";
	// 	std::string strtemp  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 	//创建文件甲
	// 	std::string strPath  = "PlayerData//";
	// 	std::string strCDKeyFolder = strPath+strcdkey;
	// 	//创建文件夹
	// 	CreateDirectory(strCDKeyFolder.c_str(), NULL);
	// 	//写入文件
	// 	if (file.Open(strtemp.c_str(), CFile::modeCreate | CFile::modeWrite))
	// 	{	
	// 		string buff;
	// 		string  temp_buff;
	// 		CGUID FamilyGuid = pPlayer->GetFamilyGuid();
	// 		if ( FamilyGuid != CGUID::GUID_INVALID )
	// 		{	
	// 			//写入家族guid
	// 			char *pGuid = new char[125];
	// 			FamilyGuid.tostring(pGuid);
	// 			buff = GetWriteCString(2, "玩家所在行会GUID为 :", pGuid);
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 			delete []pGuid;
	// 			pGuid = NULL;
	// 
	// 			//写入家族名字
	// 			temp_buff = pPlayer->GetFamilyName();
	// 			buff = GetWriteCString(2, "        行会名字为 :", temp_buff.c_str());
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 
	// 			//写入家族称号
	// 			temp_buff = pPlayer->GetFamilyTitle();
	// 			buff = GetWriteCString(2, "        行会称号为 :", temp_buff.c_str());
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 
	// 			//写入会长guid
	// 			//pGuid = new char[125];
	// 			//const CGUID &leader_guid = pPlayer->GetFamilyMasterGuid();
	// 			//FamilyGuid.tostring(pGuid);
	// 			//buff = GetWriteCString(2, "        行会 会长 GUID为 :", pGuid);
	// 			//file.WriteString(buff);
	// 			//file.WriteString("\n");
	// 			//delete []pGuid;
	// 			//pGuid = NULL;
	// 
	// 			//写入行会job等级
	// 			long joblevel = pPlayer->GetFamilyJobLev();
	// 			buff.Format("%d", joblevel);
	// 			buff = GetWriteCString(2, "        行会job等级为 :", buff);
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 		}
	// 		else
	// 		{
	// 			FamilyGuid = CGUID::GUID_INVALID;
	// 			char *pGuid = new char[125];
	// 			FamilyGuid.tostring(pGuid);
	// 			buff = GetWriteCString(2, "玩家没有加入行会,此时Guid为空 :", pGuid);
	// 			file.WriteString(buff);
	// 			file.WriteString("\n");
	// 			delete []pGuid;
	// 			pGuid = NULL;
	// 		}
	// 		file.Close();
	// 	}
	// 
	// 	
}

// load save faction data by .txt
void CCDKey::AddSaveFactionByTxt(CPlayer *pPlayer)
{
	// 	if (pPlayer == NULL)						//指针判断
	// 	{
	// 		return ;
	// 	}
	// 
	// 	CStdioFile file;
	// 
	// 	//打开文件
	// 
	// 	//设置名字
	// 	std::string strcdkey	 = m_strCDKey.c_str();
	// 	std::string strchar		 = "//";
	// 	std::string strfrist	 = "PlayerData//";
	// 	std::string strend       = "faction.txt";
	// 	std::string strtempname  = strfrist+strcdkey+strchar+strcdkey+strend;
	// 	string cstrBuff;		//读行buff
	// 	string  tempBuff;		
	// 
	// 	if (file.Open(strtempname.c_str(), CFile::modeRead))
	// 	{	
	// 		while (file.ReadString(cstrBuff))				//读行
	// 		{	
	// 			//读取行会guid
	// 			tempBuff				   = GetCStringToString(cstrBuff, ":");
	// 			CGUID guid(tempBuff.c_str());
	// 			m_SaveFaction.FamilyGuid   = guid;
	// 			if ( guid != CGUID::GUID_INVALID )
	// 			{
	// 				//读取家族名字
	// 				file.ReadString(cstrBuff);
	// 				tempBuff				      = GetCStringToString(cstrBuff, ":");
	// 				m_SaveFaction.strFamilyName   = tempBuff;
	// 
	// 				//读取家族称号
	// 				file.ReadString(cstrBuff);
	// 				tempBuff				      = GetCStringToString(cstrBuff, ":");
	// 				m_SaveFaction.strFamilyTitle  = tempBuff;
	// 
	// 				////读取家族会长guid
	// 				//file.ReadString(cstrBuff);
	// 				//tempBuff				       = GetCStringToString(cstrBuff, ":");
	// 				//CGUID leader_guid(tempBuff.c_str());
	// 				//m_SaveFaction.FamilyMasterGuid = leader_guid;
	// 
	// 				//读取行会job等级
	// 				file.ReadString(cstrBuff);
	// 				tempBuff				       = GetCStringToString(cstrBuff, ":");
	// 				long lJobLevel				   = atoi(tempBuff.c_str());
	// 				m_SaveFaction.lFamilyJobLevel  = lJobLevel;
	// 			}
	// 			else
	// 			{
	// 				break;
	// 			}
	// 		}
	// 		file.Close();
	// 	}	
}

// check faction data
void CCDKey::CheckFaction(CPlayer *pPlayer)
{
	if (pPlayer == NULL)						//指针判断
	{
		return ;
	}

	if ( m_SaveFaction.FamilyGuid == CGUID::GUID_INVALID )
	{
		AddSaveFactionByTxt(pPlayer);
	}
	//
	//检测开始
	string strPlayerName = pPlayer->GetName();
	string strWirte;
	string cstrInt;

	//检查行会guid
//	const CGUID &check_guid = pPlayer->GetFamilyGuid();
// 	if ( (check_guid != m_SaveFaction.FamilyGuid) && check_guid != CGUID::GUID_INVALID && m_SaveFaction.FamilyGuid != CGUID::GUID_INVALID)
// 	{	
// 		strWirte = GetWirteString(2, strPlayerName.c_str(), "的行会GUID不一样,等于行会数据丢失");
// 		PutCheckFactionErr(strWirte.c_str());
// 		return ;
// 	}
// 	else if ((check_guid == m_SaveFaction.FamilyGuid) && check_guid != CGUID::GUID_INVALID && m_SaveFaction.FamilyGuid != CGUID::GUID_INVALID )
// 	{	
// 
// 		//检查家族会长guid
// 		//const CGUID &leader_guid = pPlayer->GetFamilyMasterGuid();
// 		//if ( (leader_guid != m_SaveFaction.FamilyMasterGuid) && m_SaveFaction.lFamilyJobLevel != -1)
// 		//{
// 		//	strWirte = GetWirteString(2, strPlayerName.c_str(), "的行会会长GUID不一样,");
// 		//	PutCheckFactionErr(strWirte.c_str());
// 		//	return ;
// 		//}
// 
// 		//检查行会名字
// 		strWirte = pPlayer->GetFamilyName();
// 		if ( strWirte.compare(m_SaveFaction.strFamilyName) != 0)
// 		{
// 			strWirte = GetWirteString(2, strPlayerName.c_str(), "的行会名字不一样,");
// 			PutCheckFactionErr(strWirte.c_str());
// 			return ;
// 		}
// 
// 		////检查行会称号
// 		//strWirte = pPlayer->GetFamilyTitle();
// 		//if ( strWirte.compare(m_SaveFaction.strFamilyTitle) != 0)
// 		//{
// 		//	strWirte = GetWirteString(2, strPlayerName.c_str(), "的行会称号不一样,");
// 		//	PutCheckFactionErr(strWirte.c_str());
// 		//	return ;
// 		//}
// 
// 		////检查行会job等级
// 		//long lCheckJobLevel = pPlayer->GetFamilyJobLev();
// 		//if ( lCheckJobLevel != m_SaveFaction.lFamilyJobLevel)
// 		//{	
// 		//	strWirte = GetWirteString(2, strPlayerName.c_str(), "的行会joblevel不一样,");
// 		//	PutCheckFactionErr(strWirte.c_str());
// 		//	return ;
// 		//}
// 	}	
}

void CCDKey::PutCheckFactionErr(const char *msg)
{	
	static int num=0;
	char t[128];

	FILE *fp;
	if( (fp=fopen("check_faction_Erro.txt","a+")) == NULL )
		return;

	num++;
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(t, "\n%d(%02d-%02d %02d:%02d:%02d):", num, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	fseek(fp, 0, SEEK_END);
	fwrite(t, lstrlen(t), 1, fp);
	fwrite(msg, lstrlen(msg), 1, fp);
	fclose(fp);

	//如果大于 20M  删除debug文件
	if( GetFileLength("check_faction_Erro.txt")/(1024*1024) > 20 )
	{
		DeleteFileA("check_faction_Erro.txt");
	}
}

void CCDKey::ClearSaveFaction(CPlayer *pPlayer)
{
	m_SaveFaction.FamilyGuid	   = CGUID::GUID_INVALID;
	m_SaveFaction.FamilyMasterGuid = CGUID::GUID_INVALID;

//	pPlayer->SetFamilyGuid(CGUID::GUID_INVALID);
	//pPlayer->SetFamilyMasterGuid(CGUID::GUID_INVALID);
}