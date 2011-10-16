//EntityManager.cpp/////////////////////////////////////////////////////////////////////
//对象类:该对象管理所有的对象实例和对象的配置信息数据
//Author:安海川
//Create Time:2008.11.03
#include "StdAfx.h"
#include "Account.h"
#include "entityManager.h"
#include "Entity.h"
#include "EntityGroup.h"
#include "../Thread/DBAdoBase.h"
#include "../../../../dbaccess/WorldDB/RsEntity.h"
#include "../../dbserver/DBServer.h"
#include "../Thread/DBThread.h"
#include "../Thread/DBAccessThread.h"
#include "../Thread/DBMailThread.h"
#include "../public/entityproperty.h"
#include "../public/public.h"
#include "../public/tools.h"
#include "../public/RFile.h"
#include "../public/readwrite.h"
#include "../../../../dbaccess/MyAdoBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

std::string NULL_STRING = "";

HANDLE   CEntityManager::m_hLoadThreadEvent;
HANDLE   CEntityManager::m_hSaveThreadEvent;

HANDLE   CEntityManager::m_hMailThreadEvent;

extern long lMapAccounts;
extern long lMapPlayers;
extern long lMapFactions;
extern long lMapScripts;
extern long lMapProcs;

extern bool g_bSaveNowData;


void CEntityManager::Release(void)
{
	// 创建消息
	ThreadData* mailDataQuit = new ThreadData;
	mailDataQuit->retFlag = -99999999; // 传入一个值表示线程退出
	list<ThreadData*> mailDataList;
	mailDataList.push_back(mailDataQuit);
	PushMailThreadData(mailDataList);
	// 等待mail线程推出
	WaitForSingleObject(m_MailThreadQuitEvent, INFINITE);

	// 删除线程
	m_pMailThread->Exit();
	SAFE_DELETE(m_pMailThread);

	// 创建线程退出数据
	list<ThreadData*> tmpLoadDataList;
	for (map<long, DBThread*>::iterator ite = m_mapLoadThread.begin(); ite != m_mapLoadThread.end(); ++ite)
	{
		ThreadData* tLoadData = new ThreadData;
		tLoadData->retFlag = -99999999; // 传入一个值表示线程退出
		tmpLoadDataList.push_back(tLoadData);
	}
	PushLoadThreadData(tmpLoadDataList);

	list<ThreadData*> tmpSaveDataList;
	for (map<long, DBThread*>::iterator ite = m_mapSaveThread.begin(); ite != m_mapSaveThread.end(); ++ite)
	{
		ThreadData* tLoadData = new ThreadData;
		tLoadData->retFlag = -99999999; // 传入一个值表示线程退出
		tmpSaveDataList.push_back(tLoadData);
	}
	PushSaveThreadData(tmpSaveDataList);

	// 等待线程退出
	WaitForMultipleObjects(m_mapLoadThread.size()+m_mapSaveThread.size(), &m_vecThreadQuitEvent[0], TRUE, INFINITE);

	for (map<long, DBThread*>::iterator ite = m_mapLoadThread.begin(); ite != m_mapLoadThread.end(); ++ite)
	{
		// 删除线程
		ite->second->Exit();
		SAFE_DELETE(ite->second);
	}
	m_mapLoadThread.clear();

	for (map<long, DBThread*>::iterator ite = m_mapSaveThread.begin(); ite != m_mapSaveThread.end(); ++ite)
	{
		// 删除线程
		ite->second->Exit();
		SAFE_DELETE(ite->second);
	}
	m_mapSaveThread.clear();

	std::map<string, tagEntityBuildInfo*>::iterator deItr = m_mapObjAttrDef.begin();
	for(; deItr != m_mapObjAttrDef.end(); deItr++)
	{
		delete deItr->second;
	}
	m_mapObjAttrDef.clear();

	DBEntityComponentMapItr enItr = m_mapEntitys.begin();
	for (; enItr != m_mapEntitys.end(); enItr++)
	{
		map<CGUID, CEntityGroup*>::iterator entityItr = enItr->second.begin();
		for(; entityItr != enItr->second.end(); entityItr++)
			delete entityItr->second;
	}
	m_mapEntitys.clear();

	AccountMapItr accItr = m_mapAccounts.begin();
	for(; accItr != m_mapAccounts.end(); accItr++)
		delete accItr->second;
	m_mapAccounts.clear();

	DeleteCriticalSection(&m_MailCriticalSection);
	DeleteCriticalSection(&m_LoadCriticalSection);
	DeleteCriticalSection(&m_SaveCriticalSection);
	DeleteCriticalSection(&m_RetCriticalSection);
	DeleteCriticalSection(&m_RetMailCriticalSection);
	DeleteCriticalSection(&m_ThreadQuitCS);

	m_pAccessThread->GetAdoBasePtr()->CloseCn();
	CoUninitialize();
	SAFE_DELETE(m_pAccessThread);

	// 邮件操作完全独立
	list<ThreadData*>::iterator listItr = m_listMailThreadData.begin();
	for(; listItr != m_listMailThreadData.end(); listItr)
		delete (*listItr);
	m_listMailThreadData.clear();

	// 邮件操作返回数据
	listItr = m_listMailThreadRetData.begin();
	for(; listItr != m_listMailThreadRetData.end(); listItr)
		delete (*listItr);
	m_listMailThreadRetData.clear();

	// 投递到线程的操作数据
	listItr = m_listLoadThreadOperData.begin();
	for(; listItr != m_listLoadThreadOperData.end(); listItr)
		delete (*listItr);
	m_listLoadThreadOperData.clear();

	listItr = m_listSaveThreadOperData.begin();
	for(; listItr != m_listSaveThreadOperData.end(); listItr)
		delete (*listItr);
	m_listSaveThreadOperData.clear();

	// 读取或存储过程返回数据
	listItr = m_listThreadRetData.begin();
	for(; listItr != m_listThreadRetData.end(); listItr)
		delete (*listItr);
	m_listThreadRetData.clear();
}

// 是否还在运行
bool bDBSExit;

CEntityManager::CEntityManager()
{
	m_lAttrEnumValue = 0;

	m_hLoadThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSaveThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hMailThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pAccessThread = NULL;
	m_pMailThread   = NULL;

	InitializeCriticalSection(&m_MailCriticalSection);
	InitializeCriticalSection(&m_LoadCriticalSection);
	InitializeCriticalSection(&m_SaveCriticalSection);
	InitializeCriticalSection(&m_RetCriticalSection);
	InitializeCriticalSection(&m_RetMailCriticalSection);
	InitializeCriticalSection(&m_ThreadQuitCS);
}
CEntityManager::~CEntityManager()
{
	Release();
}
// 初始化DBThread
void CEntityManager::InitDBThread(string strProvider, 
								  string strDataSource, 
								  string strInitialCatalog,
								  string strUserID, 
								  string strPassword)
{
	// 创建读线程
	for(int i=0; i<GetGame()->GetSetup()->dwPlayerLoadThreadNumber; i++)
	{
		DBThread* loadThread1 = new DBThread(strProvider, strDataSource, 
			strInitialCatalog, strUserID, strPassword, eThreadLoadType);
		m_mapLoadThread[i] = loadThread1;
		loadThread1->Begin();
	}

	// 创建写线程
	    for(int i=0; i<GetGame()->GetSetup()->dwPlayerSaveThreadNumber; i++)
	{
		DBThread* saveThread1 = new DBThread(strProvider, strDataSource, 
			strInitialCatalog, strUserID, strPassword, eThreadSaveType);
		m_mapSaveThread[i] = saveThread1;
		saveThread1->Begin();
	}

	m_pAccessThread = new DBAccessThread(strProvider, strDataSource, 
		strInitialCatalog, strUserID, strPassword);

	m_pMailThread = new DBMailThread(strProvider, strDataSource, 
		strInitialCatalog, strUserID, strPassword);
	if(m_pMailThread)
		m_pMailThread->Begin();

	CMyAdoBase::Initialize(strProvider, strDataSource, strInitialCatalog, strUserID, strPassword);
	m_pAccessThread->GetAdoBasePtr()->OpenCn();
	InitPropertyIniData();
}


// 从消息取得操作对象
CEntityGroup* CEntityManager::FindRootEntity(const string& flag, const CGUID& guid)
{
	DBEntityComponentMapItr itr = m_mapEntitys.find(flag);
	if(itr != m_mapEntitys.end())
	{
		std::map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(guid);
		if(enItr != itr->second.end())
			return enItr->second;
	}
	return NULL;
}
void CEntityManager::ProcessMsg(CMessage* pMsg)
{
	//需要返回的消息类型
	CGUID SessionID;
	pMsg->GetGUID(SessionID);

	// root comflag
	char rootComFlag[64];
	memset(rootComFlag, 0, sizeof(rootComFlag));
	pMsg->GetStr(rootComFlag, sizeof(rootComFlag));

	// root guid
	CGUID rootGuid;
	pMsg->GetGUID(rootGuid);

	// root name
	char szRootName[128];
	pMsg->GetStr(szRootName, sizeof(szRootName));

	BYTE rootFindFlag = pMsg->GetByte();

	long bufSize = pMsg->GetLong();
	long pos = 0;
	BYTE* buf = new BYTE[bufSize];
	pMsg->GetEx(buf, bufSize);

	_GetByteFromByteArray(buf, pos, bufSize);
	char szComFlag[128];
	_GetStringFromByteArray(buf, pos, szComFlag, bufSize);
	CGUID leafGuid;
	_GetBufferFromByteArray(buf, pos, leafGuid, bufSize);
	BYTE DbOperFlag = _GetByteFromByteArray(buf, pos, bufSize);
	BYTE DBOperType = _GetByteFromByteArray(buf, pos, bufSize);

 	if(DbOperFlag == 1)
	{
		// 先判断cdkey字符串是否有效
		if( (strcmp(rootComFlag, "[account]") == 0) )
		{
			if(szRootName[0] == '\0')
			{
				AddLogText("ProcessMsg:[%d][%d] Account账号数据无效！",DbOperFlag,DBOperType);
				SAFE_DELETE_ARRAY(buf);
				return;
			}
		}

		bool isEntityMailGroupFlag = false;
		CEntityGroup* entity = NULL;
		if( (strcmp(rootComFlag, szComFlag) != 0) ) // 本次对象是某对象的子节点
		{
			if( (strcmp(rootComFlag, "[account]") == 0) )
			{
				//现在普通队列中查找，如果没有该根节点则在临时队列生成一个
				//在MainLoop()中需要投递线程消息时释放该临时对象
				entity = FindAccount(string(szRootName));
				if(!entity)
				{
					entity = new Account(rootGuid);
					entity->SetName(szRootName);
					entity->SetIsTempEntityFlag(1);//临时数据
					if(!AddAccount((Account*)entity))
					{
						SAFE_DELETE(entity);
					}
				}
			}
			else
			{
				entity = FindRootEntity(rootComFlag, rootGuid);
				if(!entity)
				{
					entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
					entity->SetIsTempEntityFlag(1);//临时数据
					AddOneRootEntity(entity);
				}
			}

			if(entity)
			{
				entity->SetName(szRootName);
				if(DBOperType == DB_OPER_LOAD)
					entity->SetCurDbOperType(DB_OPER_LOAD);
				else if(DBOperType == DB_OPER_LOAD_NOCACHE)
					entity->SetCurDbOperType(DB_OPER_LOAD_NOCACHE);
				else
					entity->SetCurDbOperType(DB_OPER_SAVE);
				entity->SetCurDbOperFlag(1);
			}

			CEntityGroup* pFather = FindLeafFatherByComFlag(entity, szComFlag);
			if(pFather)
			{
				if(DBOperType == DB_OPER_LOAD)
					pFather->SetCurDbOperType(DB_OPER_LOAD);
				else if(DBOperType == DB_OPER_LOAD_NOCACHE)
					pFather->SetCurDbOperType(DB_OPER_LOAD_NOCACHE);
				else
					pFather->SetCurDbOperType(DB_OPER_SAVE);
				pFather->SetCurDbOperFlag(1);
			}

			switch(DBOperType)
			{
			case DB_OPER_DELETE_INSERT:
			case DB_OPER_ONLY_MEMSAVE:
				{
					if(pFather)
					{
						pFather->ReleaseLeaf(leafGuid);
						CEntityGroup* pLeaf = (CEntityGroup*)NewBaseEntity(szComFlag, leafGuid);
						pLeaf->SetCurDbOperType(DBOperType);
						pLeaf->SetCurDbOperFlag(DbOperFlag);
						if(DbOperFlag == 1)
							pLeaf->GetFromByteArray(buf, pos, bufSize);
						pFather->AddChild(pLeaf);
						pLeaf->SetSessionID(SessionID);
					}
					else
						AddLogText("ProcMsg DB_OPER_DELETE_INSERT 添加子结点%s时未找到其父节点", szComFlag);
				}
				break;
			case DB_OPER_DELETE:
				{
					if(pFather)
					{
						// 等待MainLoop()处理删除
						CEntityGroup* pLeaf = (CEntityGroup*)pFather->GetChild(leafGuid);		
						if(pLeaf)
						{
							pLeaf->SetCurDbOperType(DBOperType);
							pLeaf->SetCurDbOperFlag(DbOperFlag);
							if(DbOperFlag == 1)
							{
								pLeaf->GetFromByteArray(buf, pos, bufSize);
								pLeaf->SetIsTempEntityFlag(1);// 设置成临时变量
								pLeaf->SetSessionID(SessionID);
								// 设置根节点删除数据
								entity->SetDelLeafGuid(leafGuid);
								entity->SetDelLeafFlag(szComFlag);
							}
						}
					}
					else
						AddLogText("ProcMsg DB_OPER_DELETE 添加子结点%s时未找到其父节点", szComFlag);
				}
				break;
			case DB_OPER_INSERT:
				{
					if(pFather)
					{
						CEntityGroup* pLeaf = (CEntityGroup*)NewBaseEntity(szComFlag, leafGuid);
						pLeaf->SetCurDbOperType(DBOperType);
						pLeaf->SetCurDbOperFlag(DbOperFlag);
						if(DbOperFlag == 1)
							pLeaf->GetFromByteArray(buf, pos, bufSize);
						pFather->AddChild(pLeaf);
						pLeaf->SetSessionID(SessionID);
					}
					else
						AddLogText("ProcMsg DB_OPER_INSERT 添加子结点%s时未找到其父节点", szComFlag);
				}
				break;
			case DB_OPER_SAVE:
				{
					if(pFather)
					{
						CEntityGroup* pLeaf = (CEntityGroup*)pFather->GetChild(leafGuid);
						if(!pLeaf)
						{
							pLeaf = (CEntityGroup*)NewBaseEntity(szComFlag, leafGuid);
							CEntityGroup* pFather = FindLeafFatherByComFlag(entity, szComFlag);
							if(pFather)
								pFather->AddChild(pLeaf);
						}
						if(pLeaf)
						{
							pLeaf->SetCurDbOperType(DBOperType);
							pLeaf->SetCurDbOperFlag(DbOperFlag);
							if(DbOperFlag == 1)
								pLeaf->GetFromByteArray(buf, pos, bufSize);
							pLeaf->SetSessionID(SessionID);
						}
					}
					else
						AddLogText("ProcMsg DB_OPER_SAVE 添加子结点%s时未找到其父节点", szComFlag);
				}
				break;
			case DB_OPER_LOAD:
			case DB_OPER_LOAD_NOCACHE:
				{
					if(pFather)
					{
						CEntityGroup* pLeaf = (CEntityGroup*)pFather->GetChild(leafGuid);
						if(pLeaf && !pLeaf->GetIsTempEntityFlag())
						{
							CMessage msg(MSG_DB2W_OPER_ENTITY);
							msg.Add(SessionID);
							msg.Add((long)S_OK);
							msg.Add(rootComFlag);
							msg.Add(rootGuid);
							msg.Add("");
							msg.Add((BYTE)0);
							vector<BYTE> pBA;
							pLeaf->AddToByteArray(pBA, true); // 读操作
							msg.Add((long)pBA.size());
							if(pBA.size())
								msg.AddEx(&pBA[0], (long)pBA.size());
							msg.SendToSocket(GetGame()->GetWSSocket());
							SAFE_DELETE_ARRAY(buf);
							return;
						}
						else
						{
							pLeaf = (CEntityGroup*)NewBaseEntity(szComFlag, leafGuid);
							CEntityGroup* pFather = FindLeafFatherByComFlag(entity, szComFlag);
							if(pFather)
								pFather->AddChild(pLeaf);
							pLeaf->SetIsTempEntityFlag(1);// 临时数据
							pLeaf->SetSessionID(SessionID);
						}

						if(entity)
						{
							// 投递Load队列
							ThreadData* loadData = new ThreadData;
							loadData->pEntity = entity;
							PushLoadThreadData(loadData);
							SAFE_DELETE_ARRAY(buf);
						}
					}
					else
						AddLogText("ProcMsg DB_OPER_LOAD 添加子结点%s时未找到其父节点", szComFlag);
				}
				break;
			default:
				{
					AddLogText("ProcessMsg(): 根节点%s 子结点%s 的操作类型%d 无效!", 
						rootComFlag, szComFlag, DBOperType);
					SAFE_DELETE_ARRAY(buf);
					return;
				}
				break;
			}
		}
		else // 是根结点
		{
			// 是否邮件对象，单独处理
			if(strcmp(rootComFlag, "[mailgroup]") == 0)
				isEntityMailGroupFlag = true;
			switch(DBOperType)
			{
			case DB_OPER_DELETE:
				{
					if(DbOperFlag == 1)
					{
						if( (strcmp(rootComFlag, "[account]") == 0) )
						{
							entity = FindAccount(szRootName);
							if(!entity)
							{
								entity = new Account(rootGuid);
								entity->SetName(szRootName);
							}
						}
						else
						{
							entity = FindRootEntity(rootComFlag, rootGuid);
							if(!entity)
							{
								if(!isEntityMailGroupFlag)
									entity = CreateRootEntity(rootComFlag, rootGuid);
								else
									entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
							}
						}
						if(entity)
						{
							entity->GetFromByteArray(buf, pos, bufSize);
							entity->SetCurDbOperType(DBOperType);
							entity->SetCurDbOperFlag(DbOperFlag);
							entity->SetIsTempEntityFlag(1);
						}
					}
				}
				break;
			case DB_OPER_DELETE_INSERT:
			case DB_OPER_ONLY_MEMSAVE:
				{
					if(DbOperFlag == 1)
					{
						if( (strcmp(rootComFlag, "[account]") == 0) )
						{
							entity = FindAccount(szRootName);
							if(!entity)
							{
								entity = new Account(rootGuid);
								entity->SetName(szRootName);
								if(!AddAccount((Account*)entity))
								{
									SAFE_DELETE(entity);
								}
							}
						}
						else
						{
							entity = FindRootEntity(rootComFlag, rootGuid);
							if(!entity)
							{
								if(!isEntityMailGroupFlag)
								{
									entity = CreateRootEntity(rootComFlag, rootGuid);
									if( strcmp(rootComFlag, "[friendgroup]")==0 && entity )
										entity->SetIsTempEntityFlag(1);
								}
								else
								{
									entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
									entity->SetIsTempEntityFlag(1);
								}
							}
						}
						if(entity)
						{
							entity->ReleaseChilds();
							entity->GetFromByteArray(buf, pos, bufSize);
							entity->SetCurDbOperType(DBOperType);
							entity->SetCurDbOperFlag(DbOperFlag);
						}
					}
				}
				break;
			case DB_OPER_INSERT:
				{
					if(DbOperFlag == 1)
					{
						if( (strcmp(rootComFlag, "[account]") == 0) )
						{
							entity = FindAccount(szRootName);
							if(!entity)
							{
								entity = new Account(rootGuid);
								entity->SetName(szRootName);
								if(!AddAccount((Account*)entity))
								{
									SAFE_DELETE(entity);
								}
							}
						}
						else
						{
							entity = FindRootEntity(rootComFlag, rootGuid);

							if(!entity)
							{
								if(!isEntityMailGroupFlag)
									entity = CreateRootEntity(rootComFlag, rootGuid);
								else
								{
									entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
									entity->SetIsTempEntityFlag(1);
								}
							}
						}
						if(entity)
						{
							entity->GetFromByteArray(buf, pos, bufSize);
							entity->SetCurDbOperType(DBOperType);
							entity->SetCurDbOperFlag(DbOperFlag);
						}
					}
				}
				break;
			case DB_OPER_LOAD:
			case DB_OPER_LOAD_NOCACHE:
				{
 					if(DbOperFlag == 1)
					{
						if( (strcmp(rootComFlag, "[account]") == 0) )
						{
							entity = FindAccount(szRootName);
							if(!entity)
							{
								entity = new Account(rootGuid);
								entity->SetName(szRootName);
								entity->SetIsTempEntityFlag(1);
							
								entity->GetFromByteArray(buf, pos, bufSize);
								entity->SetCurDbOperType(DBOperType);
								entity->SetCurDbOperFlag(DbOperFlag);
								entity->SetSessionID(SessionID);
								// 投递Load队列
								ThreadData* loadData = new ThreadData;
								loadData->pEntity = entity;
								PushLoadThreadData(loadData);
								SAFE_DELETE_ARRAY(buf);
								return;
							}
						}
						else
						{
							if(DB_OPER_LOAD_NOCACHE != DBOperType)
								entity = FindRootEntity(rootComFlag, rootGuid);

							if(!entity)
							{
								entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
								entity->SetIsTempEntityFlag(1);
								entity->GetFromByteArray(buf, pos, bufSize);
								entity->SetCurDbOperType(DBOperType);
								entity->SetCurDbOperFlag(DbOperFlag);
								entity->SetSessionID(SessionID);
								// 投递Load队列
								ThreadData* loadData = new ThreadData;
								loadData->pEntity = entity;
								if(!isEntityMailGroupFlag) // 如果是邮件Load则不通过此处直接投递线程
									PushLoadThreadData(loadData);
								else
									PushMailThreadData(loadData);
								SAFE_DELETE_ARRAY(buf);
								return;
							}
						}

						if(entity)
						{
							if(!isEntityMailGroupFlag && !entity->GetIsTempEntityFlag())
							{
								CMessage msg(MSG_DB2W_OPER_ENTITY);
								msg.Add(SessionID);
								msg.Add((long)S_OK);
								msg.Add(rootComFlag);
								msg.Add(rootGuid);

								if( (strcmp(rootComFlag, "[account]") == 0) )
								{
									msg.Add(szRootName);
									msg.Add((BYTE)1);

									// 如果是账号被Load，则取消其定时删除
									((Account*)entity)->SetLastLoginTime(-1);
								}
								else
								{
									msg.Add("");
									msg.Add((BYTE)0);
								}

								vector<BYTE> pBA;
								entity->AddToByteArray(pBA, true); // 读操作
								msg.Add((long)pBA.size());
								if(pBA.size())
									msg.AddEx(&pBA[0], (long)pBA.size());

								msg.SendToSocket(GetGame()->GetWSSocket());
								SAFE_DELETE_ARRAY(buf);
								return;
							}
						}
					}
				}
				break;
			case DB_OPER_SAVE:
				{	
					if(DbOperFlag == 1)
					{
						if( (strcmp(rootComFlag, "[account]") == 0) )
						{
							entity = FindAccount(szRootName);
							if(!entity)
							{
								entity = new Account(rootGuid);
								entity->SetName(szRootName);
								if(!AddAccount((Account*)entity))
								{
									SAFE_DELETE(entity);
								}
								if(entity)
									entity->SetIsTempEntityFlag(1);
							}
						}
						else
						{
							entity = FindRootEntity(rootComFlag, rootGuid);
							
							if(!entity)
							{
								if(!isEntityMailGroupFlag)
									entity = CreateRootEntity(rootComFlag, rootGuid);
								else
								{
									entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
									entity->SetIsTempEntityFlag(1);
								}
							}
						}
						if(entity)
						{
							entity->GetFromByteArray(buf, pos, bufSize);
							entity->SetCurDbOperType(DBOperType);
							entity->SetCurDbOperFlag(DbOperFlag);
						}
					}
				}
				break;
			default: // Procedure
				{	
					if(DBOperType==DB_OPER_INIT || DBOperType==DB_OPER_REGISTER || DBOperType==DB_OPER_UNREGISTER)
					{
						AddLogText("ProceMsg:%s type %d is err!", rootComFlag, DBOperType);
					}
					else
					{
						if(DbOperFlag == 1)
						{
							entity = (CEntityGroup*)NewBaseEntity(rootComFlag, rootGuid);
							if(entity)
							{
								entity->GetFromByteArray(buf, pos, bufSize);
								entity->SetCurDbOperType(DBOperType);
								entity->SetCurDbOperFlag(DbOperFlag);
								entity->SetIsTempEntityFlag(1);
								entity->SetSessionID(SessionID);
								if(DBOperType == DB_OPER_INSERTMAIL_PROCEDURE) // 线程优先处理
								{
									// 投递Mail队列
									ThreadData* loadData = new ThreadData;
									loadData->pEntity = entity;
									PushMailThreadData(loadData);
									SAFE_DELETE_ARRAY(buf);
									return;
								}
								else
								{
									// 投递队列
									ThreadData* loadData = new ThreadData;
									loadData->pEntity = entity;
									PushLoadThreadData(loadData);
									SAFE_DELETE_ARRAY(buf);
									return;
								}
							}
						}
					}
				}
				break;
			}
		}

		if(entity)
		{
			entity->SetSessionID(SessionID);

			if(strcmp(rootComFlag, "[account]") == 0)
			{
				AddDirtyEntityName(szRootName);
			}
			else
			{
				if(isEntityMailGroupFlag) // 是邮件操作
				{
					// 投递Load队列
					ThreadData* loadData = new ThreadData;
					loadData->pEntity = entity;
					PushMailThreadData(loadData);
				}
				else
				{
					AddDirtyEntityGuid(rootGuid);
				}
			}
		}
	}
	SAFE_DELETE_ARRAY(buf);
}

// 创建对象
CEntityGroup* CEntityManager::CreateRootEntity(const string& flag, const CGUID& guid)
{
	CEntityGroup* retEntity = new CEntityGroup(flag, guid);
	// 默认此构造对象为root
	if(retEntity)
	{
		AddOneRootEntity(retEntity);
	}
	return retEntity;
}
// interfaces

//!	压入一个ThreadData
bool CEntityManager::PushMailThreadData(ThreadData* loadData)
{
	EnterCriticalSection(&m_MailCriticalSection);

	if(m_listMailThreadData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetMailThreadEvent();
	}

	m_listMailThreadData.push_back(loadData);
	lMails = m_listMailThreadData.size();
	LeaveCriticalSection(&m_MailCriticalSection);
	return true;
}
bool CEntityManager::PushMailThreadData(list<ThreadData*>& loadDataList)
{
	EnterCriticalSection(&m_MailCriticalSection);

	if(m_listMailThreadData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetMailThreadEvent();
	}

	m_listMailThreadData.insert(m_listMailThreadData.end(), loadDataList.begin(), loadDataList.end());
	lMails = m_listMailThreadData.size();
	LeaveCriticalSection(&m_MailCriticalSection);
	return true;
}
//!	取出一个ThreadData
bool CEntityManager::GetMailThreadData(list<ThreadData*>& loadDataList, long dwThreadID)
{
	EnterCriticalSection(&m_MailCriticalSection);
	while( m_listMailThreadData.size() == 0)
	{	
		CEntityManager::ResetMailThreadEvent();
		//如果没有操作命令，则等待。
		LeaveCriticalSection(&m_MailCriticalSection);

		CEntityManager::WaitMailThreadEvent();	
		//等待通知事件
		EnterCriticalSection(&m_MailCriticalSection);
	}

	loadDataList = m_listMailThreadData;
	m_listMailThreadData.clear();
	lMails = 0;
	LeaveCriticalSection(&m_MailCriticalSection);
	return true;
}
bool CEntityManager::SetMailThreadData(list<ThreadData*>& loadDataList)
{
	EnterCriticalSection(&m_RetMailCriticalSection);
	m_listMailThreadRetData = loadDataList;
	LeaveCriticalSection(&m_RetMailCriticalSection);
	return true;
}
long CEntityManager::GetSaveThreadOperDataSize(list<CGUID>& saveBeginGuidList)
{
	long lRet = 0;
	EnterCriticalSection(&m_SaveCriticalSection);
	saveBeginGuidList = m_listSaveBeginEntityGuid;
	lRet = m_listSaveThreadOperData.size();
	LeaveCriticalSection(&m_SaveCriticalSection);
	return lRet;
}
//!	压入一个ThreadData
bool CEntityManager::PushLoadThreadData(ThreadData* loadData)
{
	EnterCriticalSection(&m_LoadCriticalSection);

	if(m_listLoadThreadOperData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetLoadThreadEvent();
	}
	if(loadData && loadData->pEntity)
	{
		m_listLoadThreadOperData.push_front(loadData);
		lOperingLoadEntitys = m_listLoadThreadOperData.size();
		m_listLoadBeginEntityGuid.push_front(loadData->pEntity->GetGUID());
		lBeginLoadNum = m_listLoadBeginEntityGuid.size();
	}
	LeaveCriticalSection(&m_LoadCriticalSection);
	return true;
}
bool CEntityManager::PushSaveThreadData(ThreadData* loadData)
{
	EnterCriticalSection(&m_SaveCriticalSection);

	if(m_listSaveThreadOperData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetSaveThreadEvent();
	}

	if(loadData && loadData->pEntity)
	{
		m_listSaveThreadOperData.push_back(loadData);
		lOperingSaveEntitys = m_listSaveThreadOperData.size();
		m_listSaveBeginEntityGuid.push_back(loadData->pEntity->GetGUID());	
		lBeginSaveNum = m_listSaveBeginEntityGuid.size();
	}
	LeaveCriticalSection(&m_SaveCriticalSection);
	return true;
}

bool CEntityManager::PushLoadThreadData(list<ThreadData*>& loadDataList)
{
	EnterCriticalSection(&m_LoadCriticalSection);

	if(m_listLoadThreadOperData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetLoadThreadEvent();
	}

	list<ThreadData*>::iterator itr = loadDataList.begin();
	for(; itr != loadDataList.end(); itr++)
	{
		if((*itr) && (*itr)->pEntity)
			m_listLoadBeginEntityGuid.push_back((*itr)->pEntity->GetGUID());	
	}
	lBeginLoadNum = m_listLoadBeginEntityGuid.size();
	m_listLoadThreadOperData.insert(m_listLoadThreadOperData.end(), loadDataList.begin(), loadDataList.end());
	LeaveCriticalSection(&m_LoadCriticalSection);
	return true;
}

bool CEntityManager::PushSaveThreadData(list<ThreadData*>& saveDataList)
{
	EnterCriticalSection(&m_SaveCriticalSection);

	if(m_listSaveThreadOperData.size() == 0)
	{
		// 设置事件
		CEntityManager::SetSaveThreadEvent();
	}

	list<ThreadData*>::iterator itr = saveDataList.begin();
	for(; itr != saveDataList.end(); itr++)
	{
		if((*itr) && (*itr)->pEntity)
			m_listSaveBeginEntityGuid.push_back((*itr)->pEntity->GetGUID());	
	}
	lBeginSaveNum = m_listSaveBeginEntityGuid.size();
	m_listSaveThreadOperData.insert(m_listSaveThreadOperData.end(), saveDataList.begin(), saveDataList.end());
	LeaveCriticalSection(&m_SaveCriticalSection);
	return true;
}

//!	弹出一个ThreadData
bool CEntityManager::GetThreadData(ThreadData** loadData, long dwThreadID, eThreadType type)
{
	if(type == eThreadLoadType)
	{
		EnterCriticalSection(&m_LoadCriticalSection);
		while( m_listLoadThreadOperData.size() == 0 )
		{	
			CEntityManager::ResetLoadThreadEvent();
			//如果没有操作命令，则等待。
			LeaveCriticalSection(&m_LoadCriticalSection);

			CEntityManager::WaitLoadThreadEvent();	
			//等待通知事件
			EnterCriticalSection(&m_LoadCriticalSection);
		}

		if(m_listLoadThreadOperData.size())
		{
			*loadData = (*m_listLoadThreadOperData.begin());
			m_listLoadThreadOperData.erase(m_listLoadThreadOperData.begin());
		}
		
		LeaveCriticalSection(&m_LoadCriticalSection);
	}
	else if(type == eThreadSaveType)
	{
		EnterCriticalSection(&m_SaveCriticalSection);
		while( m_listSaveThreadOperData.size() == 0 )
		{	
			CEntityManager::ResetSaveThreadEvent();
			//如果没有操作命令，则等待。
			LeaveCriticalSection(&m_SaveCriticalSection);

			CEntityManager::WaitSaveThreadEvent();	
			//等待通知事件
			EnterCriticalSection(&m_SaveCriticalSection);
		}

		if(m_listSaveThreadOperData.size())
		{
			*loadData = (*m_listSaveThreadOperData.begin());
			m_listSaveThreadOperData.erase(m_listSaveThreadOperData.begin());
		}
		
		LeaveCriticalSection(&m_SaveCriticalSection);
	}
	else
	{
		AddLogText("GetThreadData() thread[%d] type is Init!", dwThreadID);
	}
	return true;
}
//! 弹出一个正在被线程处理的guid
bool CEntityManager::PopBeginEntityGuid(const CGUID& guid, eThreadType type)
{
	if(type == eThreadLoadType)
	{
		EnterCriticalSection(&m_LoadCriticalSection);
		list<CGUID>::iterator itr = std::find(m_listLoadBeginEntityGuid.begin(), m_listLoadBeginEntityGuid.end(), guid);
		if(itr != m_listLoadBeginEntityGuid.end())
			m_listLoadBeginEntityGuid.erase(itr);
		lBeginLoadNum = m_listLoadBeginEntityGuid.size();
		LeaveCriticalSection(&m_LoadCriticalSection);
	}
	else if(type == eThreadSaveType)
	{
		EnterCriticalSection(&m_SaveCriticalSection);
		list<CGUID>::iterator itr = std::find(m_listSaveBeginEntityGuid.begin(), m_listSaveBeginEntityGuid.end(), guid);
		if(itr != m_listSaveBeginEntityGuid.end())
			m_listSaveBeginEntityGuid.erase(itr);
		lBeginSaveNum = m_listSaveBeginEntityGuid.size();
		LeaveCriticalSection(&m_SaveCriticalSection);
	}
	else
	{
		AddLogText("PopBeginEntityGuid() thread type is Init!");
	}
	
	return true;
}

bool CEntityManager::PushRetThreadData(ThreadData* loadData)
{
	EnterCriticalSection(&m_RetCriticalSection);
	if(loadData && loadData->pEntity)
	{
		if(loadData->pEntity->GetCurDbOperType() == DB_OPER_LOAD || loadData->pEntity->GetCurDbOperType() == DB_OPER_LOAD_NOCACHE)
			m_listThreadRetData.push_front(loadData);
		else
			m_listThreadRetData.push_back(loadData);
	}
	LeaveCriticalSection(&m_RetCriticalSection);
	return true;
}
bool CEntityManager::GetRetThreadData(list<ThreadData*>& loadDataList)
{
	EnterCriticalSection(&m_RetCriticalSection);
	loadDataList = m_listThreadRetData;
	m_listThreadRetData.clear();
	LeaveCriticalSection(&m_RetCriticalSection);
	return true;
}
void CEntityManager::SetLoadThreadEvent(void)
{
	SetEvent(m_hLoadThreadEvent);
}
void CEntityManager::WaitLoadThreadEvent(void)
{
	WaitForSingleObject(m_hLoadThreadEvent, INFINITE);
}

void CEntityManager::ResetLoadThreadEvent(void)
{
	ResetEvent(m_hLoadThreadEvent);
}

void CEntityManager::SetSaveThreadEvent(void)
{
	SetEvent(m_hSaveThreadEvent);
}
void CEntityManager::WaitSaveThreadEvent(void)
{
	WaitForSingleObject(m_hSaveThreadEvent, INFINITE);
}

void CEntityManager::ResetSaveThreadEvent(void)
{
	ResetEvent(m_hSaveThreadEvent);
}

//!								设置SaveEvent
void CEntityManager::SetMailThreadEvent(void)
{
	SetEvent(m_hMailThreadEvent);
}
//!								等待SaveEvent
void CEntityManager::WaitMailThreadEvent(void)
{
	WaitForSingleObject(m_hMailThreadEvent, INFINITE);
}
void CEntityManager::ResetMailThreadEvent(void)
{
	ResetEvent(m_hMailThreadEvent);
}

//! 压入一个线程推出事件句柄
bool CEntityManager::PushThreadQuitEventHandle(HANDLE& handle)
{
	EnterCriticalSection(&m_ThreadQuitCS);
	m_vecThreadQuitEvent.push_back(handle);
	LeaveCriticalSection(&m_ThreadQuitCS);
	return true;
}
//
bool CEntityManager::AddMailThreadQuitEventHandle(HANDLE& handle)
{	
	m_MailThreadQuitEvent = handle;
	return true;
}
// 加载表字段配置
void CEntityManager::LoadObjInfo(vector<const char*>& AttrInfoList,
								 const char* szStructInfo,
								 const char* szTable)
{
	vector<const char*>::iterator itr = AttrInfoList.begin();
	string strNode = *itr;
	long lPos = strNode.find('[');
	strNode.erase(0, lPos);
	lPos = strNode.find('.');
	strNode.erase(lPos, strNode.size());

	for( ; itr != AttrInfoList.end(); ++itr )
	{
		LoadObjAttrInfo(*itr);
	}
	LoadObjStructInfo(szStructInfo);

	// 创建表对象
	_RecordsetPtr rsPtr(0);
	if(m_pAccessThread)
	{
		char szSql[1024];
		sprintf(szSql, "SELECT * FROM %s",
			szTable);

		try
		{
			CMyAdoBase::CreateRs(rsPtr);
			CMyAdoBase::OpenRs(szSql, rsPtr, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(rsPtr);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(szTable, strNode);
		}
		
	}
}

// 初始化属性配置
void CEntityManager::InitPropertyIniData()
{
	// 初始化loginplayer对象
	LoadObjAttrInfo("structinfo\\[loginplayer].ini");
	LoadObjAttrInfo("structinfo\\[loginplayergroup].ini");

	//// 初始化account对象
	LoadObjAttrInfo("structinfo\\[account].ini");
	LoadObjStructInfo("structinfo\\[account_build].ini");

	// 创建表对象
	_RecordsetPtr rsacc(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_PLAYER_BASE");

		try
		{
			CMyAdoBase::CreateRs(rsacc);
			CMyAdoBase::OpenRs(sql, rsacc, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(rsacc);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_PLAYER_BASE"), string("[loginplayer]"));
		}
		
	}

	// 初始化goods对象
	LoadObjAttrInfo("structinfo\\[goods].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_depot].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_depotgold].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_depotsecondary].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_depotsilver].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_equip].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_gold].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_pack].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_packet].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_silver].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subdepotone].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subdepotfour].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subdepotthree].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subdepottwo].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subpackfive].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subpackfour].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subpackone].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subpackthree].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_subpacktwo].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_businesspack].ini");
	LoadObjAttrInfo("structinfo\\goodsgroup_leaves\\[gp_businesspack_field].ini");
	LoadObjAttrInfo("structinfo\\[goodsgroup].ini");
	LoadObjStructInfo("structinfo\\[goodsgroup_build].ini");
	// 创建表对象
	_RecordsetPtr goodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_PLAYER_GOODS");
		try
		{
			CMyAdoBase::CreateRs(goodsrs);
			CMyAdoBase::OpenRs(sql, goodsrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(goodsrs);
			
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_PLAYER_GOODS"), string("[goods]"));
		}
		
	}

	// 创建对象
	LoadObjAttrInfo("structinfo\\[player_friend].ini");
	LoadObjAttrInfo("structinfo\\[playerfriendgroup].ini");
	LoadObjStructInfo("structinfo\\[playerfriendgroup_build].ini");
	LoadObjAttrInfo("structinfo\\[friendgroup].ini");
	LoadObjStructInfo("structinfo\\[friendgroup_build].ini");
	//CreateRootEntity("friendgroup", NULL_GUID);
	_RecordsetPtr playerFriendRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_PLAYER_FRIEND");
		try
		{
			CMyAdoBase::CreateRs(playerFriendRs);
			CMyAdoBase::OpenRs(sql, playerFriendRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(playerFriendRs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_PLAYER_FRIEND"), string("[player_friend]"));
		}
		
	}

	LoadObjAttrInfo("structinfo\\[playerfriendgroupOwner].ini");
	LoadObjAttrInfo("structinfo\\[playerfriendgroupAim].ini");

	// 创建对象
	LoadObjAttrInfo("structinfo\\[player_sprite].ini");
	LoadObjAttrInfo("structinfo\\[playerspritegroup].ini");
	_RecordsetPtr playerSpriteRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_PLAYER_SPRITE");
		try
		{
			CMyAdoBase::CreateRs(playerSpriteRs);
			CMyAdoBase::OpenRs(sql, playerSpriteRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(playerSpriteRs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_PLAYER_SPRITE"), string("[player_sprite]"));
		}
		
	}

	// Fox@20081218 宠物表结构
	vector<const char*> AttrInfoList;
	AttrInfoList.push_back("structinfo/[pet].ini");
	AttrInfoList.push_back("structinfo/[playerpetlist].ini");
	LoadObjInfo(AttrInfoList, NULL, "TBL_PET");
	AttrInfoList.clear();

	// 商业系统表结构
	AttrInfoList.push_back( "structinfo/business/[trade_spot].ini" );
	LoadObjInfo( AttrInfoList, NULL, "TBL_TRADE_SPOT" );
	LoadObjAttrInfo("structinfo/business/[trade_spot_group].ini");
	AttrInfoList.clear();

	// 初始化Faction_Apply对象
	LoadObjAttrInfo("structinfo\\[faction_apply].ini");
	LoadObjAttrInfo("structinfo\\[factionapplygroup].ini");
	// 创建表对象
	_RecordsetPtr facApplyRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_FACTION_APPLY");
		try
		{
			CMyAdoBase::CreateRs(facApplyRs);
			CMyAdoBase::OpenRs(sql, facApplyRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(facApplyRs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_FACTION_APPLY"), string("[faction_apply]"));
		}
		
	}

	// 初始化Faction_Member对象
	LoadObjAttrInfo("structinfo\\[faction_member].ini");
	LoadObjAttrInfo("structinfo\\[factionmembergroup].ini");
	// 创建表对象
	_RecordsetPtr facMembersRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_FACTION_MEMBER");
		try
		{
			CMyAdoBase::CreateRs(facMembersRs);
			CMyAdoBase::OpenRs(sql, facMembersRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(facMembersRs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_FACTION_MEMBER"), string("[faction_member]"));
		}
		
	}

	// 创建表对象
	LoadObjAttrInfo("structinfo\\[faction].ini");
	LoadObjAttrInfo("structinfo\\[factiongroup].ini");
	LoadObjStructInfo("structinfo\\[faction_build].ini");
	_RecordsetPtr facBaseDataRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_FACTION_BASE");
		try
		{
			CMyAdoBase::CreateRs(facBaseDataRs);
			CMyAdoBase::OpenRs(sql, facBaseDataRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(facBaseDataRs);
			
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_FACTION_BASE"), string("[faction]"));
		}
		
	}

	// 初始化Faction_Member对象
	LoadObjAttrInfo("structinfo\\[union_member].ini");
	LoadObjAttrInfo("structinfo\\[union_member_group].ini");
	// 创建表对象
	_RecordsetPtr UnionMembersRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_UNION_MEMBER");
		try
		{
			CMyAdoBase::CreateRs(UnionMembersRs);
			CMyAdoBase::OpenRs(sql, UnionMembersRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(UnionMembersRs);
			
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_UNION_MEMBER"), string("[union_member]"));
		}
		
	}

	// 创建表对象
	LoadObjAttrInfo("structinfo\\[union].ini");
	LoadObjAttrInfo("structinfo\\[union_group].ini");
	LoadObjStructInfo("structinfo\\[union_build].ini");
	_RecordsetPtr UnionBaseDataRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_UNION_BASE");
		try
		{
			CMyAdoBase::CreateRs(UnionBaseDataRs);
			CMyAdoBase::OpenRs(sql, UnionBaseDataRs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(UnionBaseDataRs);
			
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_UNION_BASE"), string("[union]"));
		}
		
	}


	// 初始化mailgoods对象
	LoadObjAttrInfo("structinfo\\[mailgoods].ini");
	LoadObjAttrInfo("structinfo\\[mailgoodsgroup].ini");
	// 创建表对象
	_RecordsetPtr mailgoodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_MAIL_GOODS");
		try
		{
			CMyAdoBase::CreateRs(mailgoodsrs);
			CMyAdoBase::OpenRs(sql, mailgoodsrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(mailgoodsrs);
			
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_MAIL_GOODS"), string("[mailgoods]"));
		}
		
	}

	// 初始化mail对象
	LoadObjAttrInfo("structinfo\\[mail].ini");
	LoadObjAttrInfo("structinfo\\[mailgroup].ini");
	LoadObjStructInfo("structinfo\\[mail_build].ini");

	// 创建表对象
	_RecordsetPtr mailrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_MAIL");
		try
		{
			CMyAdoBase::CreateRs(mailrs);
			CMyAdoBase::OpenRs(sql, mailrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(mailrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_MAIL"), string("[mail]"));
		}
		
	}


	// 创建表对象
	_RecordsetPtr rs(0);
	LoadObjAttrInfo("structinfo\\[player].ini");
	LoadObjStructInfo("structinfo\\[player_build].ini");
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_PLAYER_DETAIL");
		try
		{
			CMyAdoBase::CreateRs(rs);
			CMyAdoBase::OpenRs(sql, rs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(rs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_PLAYER_DETAIL"), string("[player]"));
		}
		
	}

	LoadObjAttrInfo("structinfo\\[country].ini");
	LoadObjAttrInfo("structinfo\\[countrygroup].ini");
	// 创建表对象
	_RecordsetPtr countrysrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_COUNTRY");
		try
		{
			CMyAdoBase::CreateRs(countrysrs);
			CMyAdoBase::OpenRs(sql, countrysrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(countrysrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_COUNTRY"), string("[country]"));
		}
		
	}


	// 创建表对象
	LoadObjAttrInfo("structinfo\\[region].ini");
	LoadObjAttrInfo("structinfo\\[regiongroup].ini");
	_RecordsetPtr regionsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_REGION");
		try
		{
			CMyAdoBase::CreateRs(regionsrs);
			CMyAdoBase::OpenRs(sql, regionsrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(regionsrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_REGION"), string("[region]"));
		}
		
	}

	// 初始化Global Var对象
	LoadObjAttrInfo("structinfo\\[scriptvar].ini");
	LoadObjAttrInfo("structinfo\\[scriptvargroup].ini");

	// 创建表对象
	_RecordsetPtr varrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_SCRIPTVAR");
		try
		{
			CMyAdoBase::CreateRs(varrs);
			CMyAdoBase::OpenRs(sql, varrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(varrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_SCRIPTVAR"), string("[scriptvar]"));
		}
		
	}

	// 初始化个人房屋对象
	LoadObjAttrInfo("structinfo\\[phrgngoods].ini");
	LoadObjAttrInfo("structinfo\\[phrgngoodsgroup].ini");
	// 创建表对象
	_RecordsetPtr phgoodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_DUPRGN_GOODS");
		try
		{
			CMyAdoBase::CreateRs(phgoodsrs);
			CMyAdoBase::OpenRs(sql, phgoodsrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(phgoodsrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_DUPRGN_GOODS"), string("[phrgngoods]"));
		}
		
	}

	LoadObjAttrInfo("structinfo\\[phrgn].ini");
	LoadObjAttrInfo("structinfo\\[phrgngroup].ini");
	LoadObjStructInfo("structinfo\\[phrgn_build].ini");
	// 创建表对象
	_RecordsetPtr phrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_DUPRGN_BASE");
		try
		{
			CMyAdoBase::CreateRs(phrs);
			CMyAdoBase::OpenRs(sql, phrs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(phrs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_DUPRGN_BASE"), string("[phrgn]"));
		}
		
	}

	LoadObjAttrInfo("structinfo\\[limit_goods_record].ini");	
	LoadObjAttrInfo("structinfo\\[limit_goods_record_group].ini");
	// 创建表对象
	_RecordsetPtr limit_goods_record_rs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT count(*) FROM TBL_LIMIT_GOODS_RECORD");
		try
		{
			CMyAdoBase::CreateRs(limit_goods_record_rs);
			CMyAdoBase::OpenRs(sql, limit_goods_record_rs, m_pAccessThread->GetConnPtr()); // 创建表
			CMyAdoBase::ReleaseRs(limit_goods_record_rs);
		}
		catch(_com_error& e)
		{
			if(e.Error() == 0x80040E37) // DB_E_NOTABLE
				CreateOtherTableInDb(string("TBL_LIMIT_GOODS_RECORD"), string("[limit_goods_record]"));
		}
	}

	LoadObjAttrInfo("structinfo\\[procedure].ini");
	LoadProcedureInfo("Procedure\\CreatePlayerCheckProc.proc");
	LoadProcedureInfo("Procedure\\PlayerNameCheckProc.proc");
	LoadProcedureInfo("Procedure\\CdkeyCheckProc.proc");
	LoadProcedureInfo("Procedure\\SystemDeleteMailCheckProc.proc");
	LoadProcedureInfo("Procedure\\QueryPHRgnOwnerNameProc.proc");
	LoadProcedureInfo("Procedure\\InsertMailProc.proc");
	LoadProcedureInfo("Procedure\\DelRoleMailProc.proc");
	LoadProcedureInfo("Procedure\\ChangePlayerNameProc.proc");

	// 自动执行一次清除超时邮件的存储过程
	RunDeleteTimeoutMailProc();
	// 生成指定存储过程
	// TBL_PLAYER_BASE
	CBaseEntity* pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[loginplayer]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	// TBL_PLAYER_DETAIL
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[player]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[player_friend]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[player_sprite]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[region]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	//TBL_PLAYER_GOODS
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[goods]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	//
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[faction]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	//
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[faction_apply]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[faction_member]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[mail]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[mailgoods]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[pet]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);

	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgn]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgngoods]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[scriptvar]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[union]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[union_member]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[trade_spot]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
	
	pRoot = GetGame()->GetEntityManager()->NewBaseEntity(string("[limit_goods_record]"), NULL_GUID);
	CreateTblProcedure(pRoot);
	SAFE_DELETE(pRoot);
}

tagEntityBuildInfo::tagEntityBuildInfo()
: lDbQueryType(-1),
lComType(0),strComFlag(""),lHeight(0),
guid(NULL_GUID),strTblName(""),lDbOperFlag(0),lLeafNum(0),
strPrimarykey(""),lHhasAttrFlag(0),lLeafComType(0),
strLeafComFlag(""),lHasDetailLeaves(0),strDbQueryName("")
{
	memset(pQueryVar, 0, sizeof(pQueryVar));
	pDataMgr.clear();
}

tagEntityBuildInfo::~tagEntityBuildInfo()
{
	pDataMgr.clear();
	map<string, tagBuildFileInfo*>::iterator itr = mapBuildFileInfo.begin();
	for(; itr != mapBuildFileInfo.end(); itr++)
		SAFE_DELETE(itr->second);
}

void CEntityManager::LoadObjStructInfo(const char* szPath)
{
	if(NULL == szPath) return;

	CRFile* prfile = rfOpen(szPath);
	if( prfile )
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		tagEntityBuildInfo* entityBuildInfo = NULL;
		string deManagerName = szPath;

		// 去掉文件夹路径
		long pos = deManagerName.find('[');
		deManagerName.erase(0, pos);
		pos = deManagerName.find('_');
		deManagerName.erase(pos, deManagerName.size());
		deManagerName.append("]");
		std::map<string, tagEntityBuildInfo*>::iterator itr = m_mapObjAttrDef.find(deManagerName);
		if(itr == m_mapObjAttrDef.end())
		{
			AddLogText("未先创建该对象[%s]根结点!",deManagerName.c_str());
			return;
		}

		entityBuildInfo = itr->second;

		string tmp;
		// 树高度
		ReadTo(stream, "#");
		stream >> tmp >> entityBuildInfo->lHeight;

		// 叶子个数
		ReadTo(stream, "#");
		stream >> tmp >> entityBuildInfo->lLeafNum;

		for(int i = 0; i < entityBuildInfo->lLeafNum; i++)
		{
			ReadTo(stream, "#");

			// 该节点GUID
			string szChildGuid;
			// 父节点GUID
			string szFatherGuid;

			long value = 0;
			tagBuildFileInfo* tFileInfo = new tagBuildFileInfo;
			stream 
				>> szChildGuid 
				>> tFileInfo->lLevel 
				>> tFileInfo->lComType 
				>> tFileInfo->strLeafComFlag 
				>> tFileInfo->strRootComFlag 
				>> szFatherGuid
				;

			CGUID childGUID(szChildGuid.c_str());
			CGUID fatherGUID(szFatherGuid.c_str());
			tFileInfo->leafGuid = childGUID;
			tFileInfo->rootGuid = fatherGUID;

			entityBuildInfo->mapBuildFileInfo[tFileInfo->strLeafComFlag] = tFileInfo;
		}
		return;
	}
}

const char* CEntityManager::LoadObjAttrInfo(const char* szPath)
{
	CRFile* prfile = NULL;
	if( NULL == szPath ||
		!(prfile = rfOpen(szPath)) ) 
		return NULL;

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string deManagerName = szPath;

	tagEntityBuildInfo* entityBuildInfo = new tagEntityBuildInfo;

	string tmp;
	//对象类型
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->lComType;

	//拥有对象标志
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strComFlag;

	// 对象GUID
	string szGuid;
	ReadTo(stream, "#");
	stream >> tmp >> szGuid;
	CGUID tmpGuid(szGuid.c_str());
	entityBuildInfo->guid = tmpGuid;		

	// 数据库表名
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strTblName;

	// 数据库操作标志
	ReadTo(stream, "#");
	long lDbOperFlag = 0;
	stream >> tmp >> lDbOperFlag;

	// MEM主键名
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strPrimarykey;

	// DB主键名
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strDBPrimarykey;

	// 是否有属性
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->lHhasAttrFlag;

	// 子节点类型
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->lLeafComType;

	// 子节点名字
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strLeafComFlag;

	// 是否有分散节点
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->lHasDetailLeaves;

	// 数据库查找变量名
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->strDbQueryName;

	// 数据库查找变量类型
	ReadTo(stream, "#");
	stream >> tmp >> entityBuildInfo->lDbQueryType;

	// 数据库查找变量(当前默认为long)
	ReadTo(stream, "#");
	long value = 0;
	stream >> tmp >> value;
	memset(entityBuildInfo->pQueryVar, 0, sizeof(entityBuildInfo->pQueryVar));
	
	memcpy(entityBuildInfo->pQueryVar, &value, sizeof(long));

	// 数据表聚集索引名
	string strTemp="";
	while(1)
	{
		stream >> strTemp;
		if (strTemp=="<end>" || stream.eof())
		{
			break;
		}

		if (strTemp=="#")
		{
			int lTmpUnique = -1;
			stream >> strTemp >> strTemp >> lTmpUnique;
			tagDbIdxInfo idxInfo(strTemp, lTmpUnique);
			entityBuildInfo->vecIndex.push_back(idxInfo);
		}
	}
	
	// 数据表非聚集索引名
	string strCluTemp="";
	while(1)
	{
		stream >> strCluTemp;
		if (strCluTemp=="<end>" || stream.eof())
		{
			break;
		}

		if (strCluTemp=="#")
		{
			int lTmpUnique = -1;
			stream >> strCluTemp >> strCluTemp >> lTmpUnique;
			tagDbIdxInfo idxInfo(strCluTemp, lTmpUnique);
			entityBuildInfo->vecNoCluIndex.push_back(idxInfo);

		}
	}

	// 数据表复合索引名
	string strComTemp="";
	while(1)
	{
		stream >> strComTemp;
		if (strComTemp=="<end>" || stream.eof())
		{
			break;
		}

		if (strComTemp=="#")
		{
			int lTmpUnique = -1;
			stream >> strComTemp >> strComTemp >> lTmpUnique;
			tagDbIdxInfo idxInfo(strComTemp, lTmpUnique);
			entityBuildInfo->vecCoIndex.push_back(idxInfo);
		}
	}

	CGUID tmpGUID(szGuid.c_str());
	// 去掉文件夹路径
	long pos = deManagerName.find('[');
	deManagerName.erase(0, pos);
	// 去掉后缀名
	pos = deManagerName.find('.');
	deManagerName.erase(pos, deManagerName.size());

	m_mapObjAttrDef[deManagerName] = entityBuildInfo;

	while(ReadTo(stream, "#"))
	{
		string attrName = "";
		long attrType = 0;
		long UseageFlag = 0;
		long attrMaxSize = 0;
		long lAttrTypeEnum = 0;
		tagEntityAttrInfo tagAttrInfo;
		stream >> attrName >> lAttrTypeEnum >> tagAttrInfo.lMaxSize >> tagAttrInfo.lUseage;
		tagAttrInfo.eNumDataType = (DATA_OBJECT_TYPE)lAttrTypeEnum;
	
		m_lAttrEnumValue++;
		tagAttrInfo.lAttrTypeEnum = m_lAttrEnumValue;
		AddAttrEnumAndStr(attrName, m_lAttrEnumValue);
		entityBuildInfo->vecAttrEnum.push_back(m_lAttrEnumValue);
		entityBuildInfo->pDataMgr[attrName] = tagAttrInfo;
	}

	return deManagerName.c_str();
}
//!	创建复合索引
void CEntityManager::CreateOtherTableComIndexInDb(const string& tableName, std::vector<tagDbIdxInfo>& vecIdx)
{
	if(vecIdx.size() <= 0) return;

	string strCreateTableSql;

	//! 生成语句头
	strCreateTableSql = "\r\nCREATE ";
	if(vecIdx[0].lUnique == 1)
		strCreateTableSql += "UNIQUE ";
	 strCreateTableSql += "INDEX [";

	strCreateTableSql += "IDX_Com" + tableName + "] ON [dbo].[" + tableName + "]\r\n(";

	for(int i=0; i<vecIdx.size(); i++)
	{
		if(vecIdx[i].strIndex == "NULL") return;

		strCreateTableSql += "\r\n[";
		strCreateTableSql += vecIdx[i].strIndex;
		strCreateTableSql += "] ASC";

		if(i < vecIdx.size()-1) // 最后一个属性不加','
			strCreateTableSql += ",";		
	}
	strCreateTableSql += "\r\n)WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";
	ExecSqlString(strCreateTableSql);
}
void CEntityManager::CreateOtherTableIndexInDb(const string& tableName, std::vector<tagDbIdxInfo>& vecIdx, bool isClustered)
{
	string strCreateTableSql;
	
	for(int i=0; i<vecIdx.size(); i++)
	{
		if(vecIdx[i].strIndex == "NULL") return;

		strCreateTableSql = "\r\nCREATE ";
		if(vecIdx[i].lUnique == 1)
			strCreateTableSql += "UNIQUE ";

		//! 生成语句头
		if(!isClustered)
		{
			
			strCreateTableSql += "NONCLUSTERED INDEX [";
		}
		else
		{
			strCreateTableSql += "CLUSTERED INDEX [";
		}

		strCreateTableSql += "IDX_" + tableName + vecIdx[i].strIndex + "] ON [dbo].[" + tableName + "]\r\n(";
		strCreateTableSql += "\r\n[";
		strCreateTableSql += vecIdx[i].strIndex;
		strCreateTableSql += "] ASC";
	
		strCreateTableSql += "\r\n)WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";
		ExecSqlString(strCreateTableSql);
	}
}
void CEntityManager::CreateOtherTableInDb(const string& tableName, const string& otherIniName)
{
	//! SQL语句的头			= 1 + 表名 + 3
	string strHead_1		= "\r\n\r\nCREATE TABLE [dbo].[";
	string strHead_3		= "] (\r\n";

	//! 8位整数类型字段	= 1 + 字段名 + 3
	string strInt8_1		= "\t[";
	string strInt8_3		= "] [tinyint] NOT NULL ,\r\n";

	//! 16位整数类型字段	= 1 + 字段名 + 3
	string strInt16_1		= "\t[";
	string strInt16_3		= "] [smallint] NOT NULL ,\r\n";

	//! 32位整数类型字段	= 1 + 字段名 + 3
	string strInt32_1		= "\t[";
	string strInt32_3		= "] [int] NOT NULL ,\r\n";
	//! 浮点类型字段		= 1 + 字段名 + 3
	string strFloat_1		= "\t[";
	string strFloat_3		= "] [float] NOT NULL ,\r\n";
	//! 日期类型字段		= 1 + 字段名 + 3
	string strDate_1		= "\t[";
	string strDate_3		= "] [datetime] NULL ,\r\n";
	//!	GUID类型字段		= 1 + 字段名 + 3
	string strGuid_1		= "\t[";
	string strGuid_3		= "] [uniqueidentifier] NOT NULL ,\r\n";
	//!	字符串类型字段 = 1 + 字段名 + 3 + size(%d) + 5
	string strStr50_1		= "\t[";
	string strStr50_3		= "] [nvarchar] (";
	string strStr50_5		= ") COLLATE Chinese_PRC_CI_AS NOT NULL ,\r\n";

	//! IMAGE类型字段	= 1 + 字段名 + 3
	string image_1		= "\t[";
	string image_3		= "] [image] NULL , \r\n";

	//!	SQL语句结尾
	string strFinality		= ") ON [PRIMARY]";

	//! 数组顺序参考eDataTypeCode顺序
	string arrItemName[] = 
	{
		strInt8_1,		strInt8_3,
		strInt16_1,		strInt16_3,
		strInt32_1,		strInt32_3,
		strFloat_1,		strFloat_3,
		strDate_1,		strDate_3,
		strGuid_1,		strGuid_3,
		strStr50_1,		strStr50_3, //strStr50_5,
		image_1,		image_3,
	};

	string strCreateTableSql;
	//! 生成语句头
	strCreateTableSql += strHead_1 + tableName + strHead_3;
	tagEntityBuildInfo* pObjAttrDef = NULL;
	std::map<string, tagEntityBuildInfo*>::iterator itr = m_mapObjAttrDef.find(otherIniName);
	if(itr != m_mapObjAttrDef.end())
		pObjAttrDef = itr->second;

	if(!pObjAttrDef) return;

	bool hasUseImageFlag = false;

	//! 生成语句身
	vector<long>::iterator attrItr = pObjAttrDef->vecAttrEnum.begin();
	for (; attrItr != pObjAttrDef->vecAttrEnum.end(); attrItr++)
	{
		DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(itr->first, *attrItr);
		string strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*attrItr);
		long lMaxSize = GetGame()->GetEntityManager()->GetBufMaxSize(itr->first, *attrItr);

		switch(enumDataType)
		{
		case DATA_OBJECT_LONG:
		case DATA_OBJECT_DWORD:
			{
				strCreateTableSql += 
					arrItemName[2 * 2] + 
					strName +
					arrItemName[2 * 2 + 1];
			}
			break;
		case DATA_OBJECT_SHORT:
		case DATA_OBJECT_WORD:
			{
				strCreateTableSql += 
					arrItemName[2 * 1] + 
					strName +
					arrItemName[2 * 1 + 1];
			}
			break;
		case DATA_OBJECT_CHAR:
		case DATA_OBJECT_BYTE:
			{
				strCreateTableSql += 
					arrItemName[2 * 1] + 
					strName +
					arrItemName[2 * 1 + 1];
			}
			break;
		case DATA_OBJECT_FLOAT:
			{
				strCreateTableSql += 
					arrItemName[2 * 3] + 
					strName +
					arrItemName[2 * 3 + 1];
			}
			break;
		case DATA_OBJECT_STRING:
			{
				char szNum[128];
				itoa(lMaxSize, szNum, 10);
				strCreateTableSql += 
					arrItemName[2 * 6] + 
					strName +
					arrItemName[2 * 6 + 1] + szNum
					+ strStr50_5;
			}
			break;
		case DATA_OBJECT_TIME:
			{
				strCreateTableSql += 
					arrItemName[2 * 4] + 
					strName +
					arrItemName[2 * 4 + 1];
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				hasUseImageFlag = true;
				strCreateTableSql += 
					arrItemName[2 * 7] + 
					strName +
					arrItemName[2 * 7 + 1];
			}
			break;
		case DATA_OBJECT_GUID:
			{
				strCreateTableSql += 
					arrItemName[2 * 5] + 
					strName +
					arrItemName[2 * 5 + 1];
			}
			break;
		}
		
	}


	//! 有主键
	if(pObjAttrDef->strDBPrimarykey != "NULL")
	{
		strCreateTableSql += " CONSTRAINT [PK_" + pObjAttrDef->strTblName + "] PRIMARY KEY NONCLUSTERED\r\n(\r\n["
			+ pObjAttrDef->strDBPrimarykey + "] ASC\r\n)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]\r\n";

	}
	//! 生成语句尾
	if(hasUseImageFlag)
		strCreateTableSql += strFinality + " TEXTIMAGE_ON [PRIMARY]";
	else
		strCreateTableSql += strFinality;

	ExecSqlString(strCreateTableSql);

	// 创建非聚集索引
	CreateOtherTableIndexInDb(pObjAttrDef->strTblName, pObjAttrDef->vecIndex, true);

	// 创建聚集索引
	CreateOtherTableIndexInDb(pObjAttrDef->strTblName, pObjAttrDef->vecNoCluIndex, false);

	// 创建复合索引
	CreateOtherTableComIndexInDb(pObjAttrDef->strTblName, pObjAttrDef->vecCoIndex);
}

void CEntityManager::ExecSqlString(const string& sqlStr)
{
	//! 执行语句
	//! 在配置的数据库中执行
	try
	{	
		m_pAccessThread->GetConnPtr()->Execute(sqlStr.c_str(), NULL, adCmdText);
	}
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("ExecSqlString() Err!", e);
		return;
	}
}

void CEntityManager::AddTblProcedure(CBaseEntity* pRoot, DB_OPERATION_TYPE opType, const string& name)
{
	if(pRoot)
	{
		map<string, map<DB_OPERATION_TYPE,string>>::iterator itr = m_mapTblProc.find(pRoot->GetCompositeFlag());
		if(itr != m_mapTblProc.end())
		{
			itr->second[opType] = name;
		}
		else
		{
			map<DB_OPERATION_TYPE,string> mapName;
			mapName[opType] = name;
			m_mapTblProc[pRoot->GetCompositeFlag()] = mapName;
		}
	}
}
void CEntityManager::CreateTblProcedure(CBaseEntity* pRoot)
{
	if(!pRoot) return;

	//! 8位整数类型字段
	string strInt8	= "tinyint";

	//! 16位整数类型字段
	string strInt16 = "smallint";

	//! 32位整数类型字段
	string strInt32 = "int";
	//! 浮点类型字段
	string strFloat = "float";
	//! 日期类型字段
	string strDate = "datetime";
	//!	GUID类型字段
	string strGuid = "uniqueidentifier";
	//!	字符串类型字段 = strnVarChar + ( + size() + )
	string strnVarChar = "nvarchar(";
	//! IMAGE类型字段
	string image = "image";

	string strProcData = "";

	// 生成Insert into
	string strTbl = "";
	string strPK = "";
	string strDbPK = "";
	tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pRoot->GetCompositeFlag());
	if(!pInfo) return;
	
	strTbl = pInfo->strTblName;
	strPK = pInfo->strPrimarykey;
	strDbPK = pInfo->strDBPrimarykey;

	// 生成存储过程名
	string strInsertProcName = "Insert" + strTbl + "Proc";
	strProcData = "CREATE PROCEDURE " + strInsertProcName + "\r\n";

	// 生成参数
	vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
	for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
	{
		DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pRoot->GetCompositeFlag(), *epItr);
		string strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
		long lUseageFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pRoot->GetCompositeFlag(), *epItr);

		switch(enumDataType)
		{
		case DATA_OBJECT_GUID:
			{
				strProcData += "@" + strName + "\t" + strGuid;
			}
			break;
		case DATA_OBJECT_STRING:
			{
				long lMaxSize = GetGame()->GetEntityManager()->GetBufMaxSize(pRoot->GetCompositeFlag(), *epItr);
				char strLen[128];
				sprintf(strLen, "%d)", lMaxSize);
				string tmpStr = strnVarChar + strLen;
				strProcData += "@" + strName + "\t" + tmpStr;
			}
			break;
		case DATA_OBJECT_TIME:
			{
				strProcData += "@" + strName + "\t" + strDate;
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				strProcData += "@" + strName + "\t" + image;
			}
			break;
		default:
			{
				strProcData += "@" + strName + "\t" + strInt32;
			}
			break;
		}
			strProcData += ",";
		strProcData += "\r\n";
	}
	strProcData[strProcData.size()-3] = ' ';

	// 生成存储过程内容
	strProcData += "AS\r\ninsert into " + strTbl + " with(ROWLOCK) (";
	epItr = pInfo->vecAttrEnum.begin();
	for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
	{
		string strEpName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
		strProcData += strEpName;
			strProcData += ",";
	}
	strProcData[strProcData.size()-1] = ' ';

	strProcData += ") values(";
	epItr = pInfo->vecAttrEnum.begin();
	for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
	{
		string strEpName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
		strProcData += "@" + strEpName;
			strProcData += ",";
	}
	strProcData[strProcData.size()-1] = ' ';
	strProcData += ")";

	// 创建存储过程
	string strClearPro = 
		"SET ANSI_NULLS ON\r\n\
		SET QUOTED_IDENTIFIER ON\r\n\
		\r\n\
		IF EXISTS (\r\n\
		SELECT * \r\n\
		FROM INFORMATION_SCHEMA.ROUTINES\r\n\
		WHERE SPECIFIC_NAME = N'"+ strInsertProcName + "'\r\n\
		)\r\n\
		DROP PROCEDURE "+ strInsertProcName+";\r\n";

	try
	{
		HRESULT hRet = CMyAdoBase::ExecuteCn(strClearPro.c_str(), GetAccessThread()->GetConnPtr());
		if(hRet > 0)
		{
			hRet = CMyAdoBase::ExecuteCn(strProcData.c_str(), GetAccessThread()->GetConnPtr());
		}
	}
	catch (_com_error& e)
	{
		AddLogText("CreateTblProcedure() 执行存储过程[%s]出错!", strInsertProcName.c_str());
		CMyAdoBase::PrintErr(strInsertProcName.c_str(), e);
	}
	AddTblProcedure(pRoot, DB_OPER_INSERT, strInsertProcName);
	AddTblProcedure(pRoot, DB_OPER_DELETE_INSERT, strInsertProcName);

	// 生成Update
	if(strPK == "NULL") return;

	strProcData.clear();
	// 生成存储过程名
	string strUpdateProcName = "Update" + strTbl + "Proc";
	strProcData = "CREATE PROCEDURE " + strUpdateProcName + "\r\n";

	// 生成参数
	epItr = pInfo->vecAttrEnum.begin();
	for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
	{
		DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pRoot->GetCompositeFlag(), *epItr);
		string strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
		long lUseageFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pRoot->GetCompositeFlag(), *epItr);

		switch(enumDataType)
		{
		case DATA_OBJECT_GUID:
			{
				strProcData += "@" + strName + "\t" + strGuid;
			}
			break;
		case DATA_OBJECT_STRING:
			{
				long lMaxSize = GetGame()->GetEntityManager()->GetBufMaxSize(pRoot->GetCompositeFlag(), *epItr);
				char strLen[128];
				sprintf(strLen, "%d)", lMaxSize);
				string tmpStr = strnVarChar + strLen;
				strProcData += "@" + strName + "\t" + tmpStr;
			}
			break;
		case DATA_OBJECT_TIME:
			{
				strProcData += "@" + strName + "\t" + strDate;
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				strProcData += "@" + strName + "\t" + image;
			}
			break;
		default:
			{
				strProcData += "@" + strName + "\t" + strInt32;
			}
			break;
		}
			strProcData += ",";
		strProcData += "\r\n";
	}
	strProcData[strProcData.size()-3] = ' ';

	// 生成存储过程内容
	strProcData += "AS\r\nupdate " + strTbl + " with(ROWLOCK) set";
	epItr = pInfo->vecAttrEnum.begin();
	for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
	{
		string strEpName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
		BYTE lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pRoot->GetCompositeFlag(), *epItr);
		if( strEpName == strDbPK || lUseFlag == 0xFF )//1:程序自动生成 -1;一旦生成就不再进行写操作
			continue;

		strProcData += " " + strEpName + "=@" + strEpName;
			strProcData += ",";
	}
	strProcData[strProcData.size()-1] = ' ';
	strProcData += " where " + strPK + "=@" + strPK;

	// 创建存储过程
	strClearPro = 
		"SET ANSI_NULLS ON\r\n\
		SET QUOTED_IDENTIFIER ON\r\n\
		\r\n\
		IF EXISTS (\r\n\
		SELECT * \r\n\
		FROM INFORMATION_SCHEMA.ROUTINES\r\n\
		WHERE SPECIFIC_NAME = N'"+ strUpdateProcName + "'\r\n\
		)\r\n\
		DROP PROCEDURE "+ strUpdateProcName+";\r\n";

	try
	{
		HRESULT hRet = CMyAdoBase::ExecuteCn(strClearPro.c_str(), GetAccessThread()->GetConnPtr());
		if(hRet > 0)
		{
			hRet = CMyAdoBase::ExecuteCn(strProcData.c_str(), GetAccessThread()->GetConnPtr());
		}
	}
	catch (_com_error& e)
	{
		AddLogText("CreateTblProcedure() 执行存储过程[%s]出错!", strUpdateProcName.c_str());
		CMyAdoBase::PrintErr(strUpdateProcName.c_str(), e);
	}
	AddTblProcedure(pRoot, DB_OPER_SAVE, strUpdateProcName);
}

const string& CEntityManager::FindTblProcedure(CBaseEntity* pRoot)
{
	if(pRoot)
	{
		map<string, map<DB_OPERATION_TYPE,string>>::iterator itr = m_mapTblProc.find(pRoot->GetCompositeFlag());
		if(itr != m_mapTblProc.end())
		{
			map<DB_OPERATION_TYPE,string>::iterator nameItr = itr->second.find((DB_OPERATION_TYPE)pRoot->GetCurDbOperType());
			if(nameItr != itr->second.end())
				return nameItr->second;
		}
	}
	return NULL_STRING;
}


//!					运行一个清除超时邮件的存储过程
void CEntityManager::RunDeleteTimeoutMailProc(void)
{
	_CommandPtr		m_pCommand;   
	_RecordsetPtr	rs;   
	_variant_t		result1;   
	_bstr_t			strProc="SystemDeleteMailCheckProc";

	try
	{
		m_pCommand.CreateInstance(__uuidof(Command));
		m_pCommand->ActiveConnection = GetAccessThread()->GetAdoBasePtr()->GetConnPtr();
		m_pCommand->CommandText = strProc;
		m_pCommand->CommandType = adCmdStoredProc;

		//! 设置参数1
		_ParameterPtr tParam1;
		time_t tCurTime = time(0);
		tParam1 = m_pCommand->CreateParameter(_bstr_t("CurrTime"), adInteger, adParamInput, sizeof(LONG), (long)tCurTime);
		m_pCommand->Parameters->Append(tParam1);

		//! 设置参数2
		_ParameterPtr tParam2;
		long buf = 0;
		long bufSize = sizeof(long);
		VARIANT varBLOB;
		SAFEARRAY *psa = 0;
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound = 0;
		rgsabound->cElements = bufSize;
		psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
		for(long i=0; i<bufSize; i++)
			SafeArrayPutElement(psa, &i, &((char*)&buf)[i]);
		varBLOB.parray = psa;
		varBLOB.vt = VT_ARRAY | VT_UI1;
		tParam2 = m_pCommand->CreateParameter(_bstr_t("IncGoodsBuf"), adVarBinary, adParamInput, bufSize, varBLOB);
		m_pCommand->Parameters->Append(tParam2);
		SafeArrayDestroy(psa);
		
		//! 执行
		rs = m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
	}
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("Run DeleteTimeoutMailProc error！", e);
		AddLogText("运行清除超时邮件存储过程失败！");
		return;
	}
	AddLogText("运行清除超时邮件存储过程成功！");
}

// 添加一个BaseEntity到Map末尾
void CEntityManager::AddOneRootEntity(CEntityGroup* entity)
{
	if(entity)
	{
		DBEntityComponentMapItr itr = m_mapEntitys.find(entity->GetCompositeFlag());
		if(itr == m_mapEntitys.end())
		{
			map<CGUID, CEntityGroup*> entityMap;
			entityMap[entity->GetGUID()] = entity;
			m_mapEntitys[entity->GetCompositeFlag()] = entityMap;
			return;
		}
		else
		{
			map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(entity->GetGUID());
			if(enItr != itr->second.end())
			{
				if(entity != enItr->second)
					delete enItr->second;
				else
				{
					char szGuid[128];
					entity->GetGUID().tostring(szGuid);
					AddLogText("AddOneRootEntity:old ptr[%s],type[%s] has found.", szGuid, entity->GetCompositeFlag().c_str());
				}
				itr->second.erase(enItr);
			}
			itr->second[entity->GetGUID()] = entity;
		}
	}
}
// 删除一个BaseEntity
void CEntityManager::DelOneRootEntity(CEntityGroup* entity)
{
	DBEntityComponentMapItr itr = m_mapEntitys.find(entity->GetCompositeFlag());
	if(itr != m_mapEntitys.end())
	{
		map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(entity->GetGUID());
		if(enItr != itr->second.end())
		{
			delete enItr->second;
			itr->second.erase(enItr);
		}
	}
}
// 删除一个BaseEntity
void CEntityManager::DelOneRootEntity(const string& comFlag, const CGUID& playerID)
{
	DBEntityComponentMapItr itr = m_mapEntitys.find(comFlag);
	if(itr != m_mapEntitys.end())
	{
		map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(playerID);
		if(enItr != itr->second.end())
		{
			delete enItr->second;
			itr->second.erase(enItr);
		}
	}

}
// 添加一个Account到Map末尾
bool CEntityManager::AddAccount(Account* acc)
{
	if(acc)
	{
		string szCdkey = acc->GetName();
		if(szCdkey != "")
		{
			AccountMap::iterator itr = m_mapAccounts.find(szCdkey);
			if(itr != m_mapAccounts.end())
			{
				if(acc != itr->second)
				{
					delete itr->second;
					m_mapAccounts[szCdkey] = acc;
				}
				else
				{
					AddLogText("AddAccount: old ptr[%s] has found.", szCdkey);
				}
			}
			else
			{
				m_mapAccounts[szCdkey] = acc;
			}
			return true;
		}
		else
		{
			AddLogText("AddAccount: szCdkey is NULL.");
		}
	}
	return false;
}
// 删除一个Account
void CEntityManager::DelAccount(const char* szCdkey)
{
	if(szCdkey)
	{
		// 现在vector中查找是否已经存在
		AccountMap::iterator itr = m_mapAccounts.find(string(szCdkey));
		if(itr != m_mapAccounts.end())
		{
			delete itr->second;
			m_mapAccounts.erase(itr);
		}
	}
}

Account* CEntityManager::FindAccount(const std::string& strCdkey)
{
	// 现在vector中查找是否已经存在
	AccountMap::iterator itr = m_mapAccounts.find(strCdkey);
	if(itr != m_mapAccounts.end())
	{
		return itr->second;
	}
	return NULL;
}
void CEntityManager::LoadProcedureInfo(const char* szPath)
{
	if(szPath)
	{
		CRFile* file = rfOpen(szPath);
		if(file)
		{
			long fileLen = file->GetDatalen();
			char* fileData = new char[fileLen+1];
			memset(fileData, 0, fileLen+1);
			memcpy(fileData, file->GetData(), fileLen);

			string procName = szPath;
			// 去掉文件夹路径
			long pos = procName.find('\\');
			procName.erase(0, pos+1);
			// 去掉后缀名
			pos = procName.find('.');
			procName.erase(pos, procName.size());
			m_vecProcedureName.push_back(procName);

			string strClearPro = 
				"SET ANSI_NULLS ON\r\n\
				SET QUOTED_IDENTIFIER ON\r\n\
				\r\n\
				IF EXISTS (\r\n\
				SELECT * \r\n\
				FROM INFORMATION_SCHEMA.ROUTINES\r\n\
				WHERE SPECIFIC_NAME = N'"+ procName + "'\r\n\
				)\r\n\
				DROP PROCEDURE "+ procName+";\r\n";

			string strCreatePro = fileData;
			try
			{
				HRESULT hRet = CMyAdoBase::ExecuteCn(strClearPro.c_str(), GetAccessThread()->GetConnPtr());
				if(hRet > 0)
				{
					hRet = CMyAdoBase::ExecuteCn(strCreatePro.c_str(), GetAccessThread()->GetConnPtr());
				}
				SAFE_DELETE_ARRAY(fileData);
			}
			catch (_com_error& e)
			{
				char szOutInfo[1024];
				sprintf(szOutInfo, "LoadProcedureInfo() 执行存储过程[%s]出错!", procName.c_str());
				CMyAdoBase::PrintErr(szOutInfo, e);
				SAFE_DELETE_ARRAY(fileData);
			}
			rfClose(file);
		}
	}
}
// 根据叶子类型查找其父节点
CEntityGroup* CEntityManager::FindLeafFatherByComFlag(CEntityGroup* pRoot, const string& leafComFlag)
{
	if(pRoot)
	{
		if( ((CEntityGroup*)pRoot)->GetLeafComFlag() == leafComFlag ) 
			return pRoot;

		if(pRoot->GetCompositeType() == COM_COMPOSITE)
		{
			map<CGUID, CBaseEntity*>::iterator itr = pRoot->GetEntityGroupMap().begin();
			for(; itr != pRoot->GetEntityGroupMap().end(); itr++)
			{
				if(itr->second->GetCompositeType() == COM_COMPOSITE)
				{
					if(((CEntityGroup*)itr->second)->GetLeafComFlag() == leafComFlag)
					return (CEntityGroup*)itr->second;

					CEntityGroup* retEntity = FindLeafFatherByComFlag((CEntityGroup*)itr->second, leafComFlag);
					if(retEntity)
						return retEntity;
				}
			}
		}
	}
	return NULL;
}
// 根据叶子类型在其根结点下查找其节点
CBaseEntity* CEntityManager::FindLeafByComFlag(CEntityGroup* pRoot, const string& leafComFlag, const CGUID& leafGuid)
{
	if(pRoot)
	{
		if(pRoot->GetCompositeType() == COM_COMPOSITE)
		{
			map<CGUID, CBaseEntity*>::iterator itr = pRoot->GetEntityGroupMap().begin();
			for(; itr != pRoot->GetEntityGroupMap().end(); itr++)
			{
				if((itr->second)->GetCompositeFlag() == leafComFlag 
					&& itr->second->GetGUID() == leafGuid)
					return (CEntityGroup*)itr->second;

				if(itr->second->GetCompositeType() == COM_COMPOSITE)
				{
					CEntityGroup* retEntity = (CEntityGroup*)FindLeafByComFlag((CEntityGroup*)itr->second, leafComFlag, leafGuid);
					if(retEntity)
						return retEntity;
				}
			}
		}
	}
	return NULL;
}

// 根据叶子类型在其根结点下查找其节点
CEntityGroup* CEntityManager::FindLeafGroupByComFlag(CEntityGroup* pRoot, const string& leafComFlag)
{
	if(pRoot)
	{
		if(pRoot->GetCompositeType() == COM_COMPOSITE)
		{
			map<CGUID, CBaseEntity*>::iterator itr = pRoot->GetEntityGroupMap().begin();
			for(; itr != pRoot->GetEntityGroupMap().end(); itr++)
			{
				if((itr->second)->GetCompositeFlag() == leafComFlag)
				{
					return (CEntityGroup*)itr->second;
				}

				if(itr->second->GetCompositeType() == COM_COMPOSITE)
				{
					CEntityGroup* retEntity = FindLeafGroupByComFlag((CEntityGroup*)itr->second, leafComFlag);
					if(retEntity)
						return retEntity;
				}
			}
		}
	}
	return NULL;
}

// 设置玩家节点上某些子节点的数据库操作标志位
void CEntityManager::SetEntityLeavesDbOperFlag(CEntityGroup* pBaseEntity, const string& leafComFlag, bool operFlag)
{
	if(!pBaseEntity) return;
	CEntityGroup* pGoodsGroup = NULL;
	map<string, CGUID>::iterator guidItr = pBaseEntity->GetGuidByComFlagMap().find(leafComFlag);
	if(guidItr != pBaseEntity->GetGuidByComFlagMap().end())
	{
		map<CGUID, CBaseEntity*>::iterator gpItr = pBaseEntity->GetEntityGroupMap().find(guidItr->second);
		if(gpItr != pBaseEntity->GetEntityGroupMap().end())
			gpItr->second->SetCurDbOperFlag(operFlag);
	}
}
// 将玩家节点上所有子节点数据库操作标志位清零
void CEntityManager::ResetEntityAllDbOperFlag(CBaseEntity* pBaseEntity, bool flag)
{
	if(!pBaseEntity) return;

	pBaseEntity->SetCurDbOperFlag(flag);

	if(pBaseEntity->GetCompositeType() != COM_COMPOSITE) return;// 不是叶子节点

	pBaseEntity->SetCurDbOperFlag(flag);
	map<CGUID, CBaseEntity*>::iterator gpItr = ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().begin();
	for(; gpItr != ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().end(); gpItr++)
	{
		ResetEntityAllDbOperFlag(gpItr->second, flag);
	}
}

// 设置从根结点开始的所有节点数据库操作标志
void CEntityManager::SetEntityAllDbOperType(CBaseEntity* pBaseEntity, DB_OPERATION_TYPE opType)
{
	if(!pBaseEntity) return;

	pBaseEntity->SetCurDbOperType(opType);

	if(pBaseEntity->GetCompositeType() != COM_COMPOSITE) return;// 不是叶子节点

	pBaseEntity->SetCurDbOperType(opType);
	map<CGUID, CBaseEntity*>::iterator gpItr = ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().begin();
	for(; gpItr != ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().end(); gpItr++)
	{
		SetEntityAllDbOperType(gpItr->second, opType);
	}
}

// 计算一个EntityGroup以及其子结点的Size
long CEntityManager::ComputeEntityGroupMemSize(CEntityGroup* pBaseEntity)
{
	return 0;
}
// 映射属性字符串名和枚举值名
void CEntityManager::AddAttrEnumAndStr(const string& strName, long lEnum)
{
	m_mapAttrEnumToStr[lEnum] = strName;
}
long CEntityManager::GetAttrEnumByStr(const string& strComFlag, const string& strName)
{
	AttrDefMapItr itr = m_mapObjAttrDef.find(strComFlag);
	if(itr != m_mapObjAttrDef.end())
	{
		map<string, tagEntityAttrInfo>::iterator attrItr = itr->second->pDataMgr.find(strName);
		if(attrItr != itr->second->pDataMgr.end())
			return (long)attrItr->second.lAttrTypeEnum;
	}
	return -1;
}
const string& CEntityManager::GetAttrStrByEnum(long lEnum)
{
	map<long, string>::iterator itr = m_mapAttrEnumToStr.find(lEnum);
	if(itr != m_mapAttrEnumToStr.end())
		return itr->second;
	return NULL_STRING;
}
// 取得配置文件的数据信息
tagEntityBuildInfo* CEntityManager::GetEntityBuildInfo(const string& strComFlag)
{
	AttrDefMapItr itr = m_mapObjAttrDef.find(strComFlag);
	if(itr != m_mapObjAttrDef.end())
	{
		return itr->second;
	}
	return NULL;
}
// DATA_OBJECT_TYPE
long CEntityManager::GetDataObjectType(const string& strComFlag, const string& strName)
{
	AttrDefMapItr itr = m_mapObjAttrDef.find(strComFlag);
	if(itr != m_mapObjAttrDef.end())
	{
		map<string, tagEntityAttrInfo>::iterator attrItr = itr->second->pDataMgr.find(strName);
		if(attrItr != itr->second->pDataMgr.end())
			return (long)attrItr->second.eNumDataType;
	}
	return (long)DATA_OBJECT_UNKNOWN;
}
long CEntityManager::GetDataObjectType(const string& strComFlag, long lEnum)
{
	return GetDataObjectType(strComFlag, GetAttrStrByEnum(lEnum));
}
// 数据库属性标志位: 1|0: 1(数据库自动操作) 0程序操作(默认)
BYTE CEntityManager::GetDbUseageFlag(const string& strComFlag, const string& strName)
{
	AttrDefMapItr itr = m_mapObjAttrDef.find(strComFlag);
	if(itr != m_mapObjAttrDef.end())
	{
		map<string, tagEntityAttrInfo>::iterator attrItr = itr->second->pDataMgr.find(strName);
		if(attrItr != itr->second->pDataMgr.end())
			return attrItr->second.lUseage;
	}
	return 0;
}
BYTE CEntityManager::GetDbUseageFlag(const string& strComFlag, long lEnum)
{
	return GetDbUseageFlag(strComFlag, GetAttrStrByEnum(lEnum));
}
// 数据块最大长度
long CEntityManager::GetBufMaxSize(const string& strComFlag, const string& strName)
{
	AttrDefMapItr itr = m_mapObjAttrDef.find(strComFlag);
	if(itr != m_mapObjAttrDef.end())
	{
		map<string, tagEntityAttrInfo>::iterator attrItr = itr->second->pDataMgr.find(strName);
		if(attrItr != itr->second->pDataMgr.end())
			return attrItr->second.lMaxSize;
	}
	return 0;
}
long CEntityManager::GetBufMaxSize(const string& strComFlag, long lEnum)
{
	return GetBufMaxSize(strComFlag, GetAttrStrByEnum(lEnum));
}

// 根据对象的节点信息产生查询条件
bool CEntityManager::GenerateQuerySavePlayerInfo(CEntityGroup* pPlayer)
{
	if(pPlayer && pPlayer->GetCurDbOperType() == DB_OPER_SAVE)
	{
		// 根节点
		if(pPlayer->GetCurDbOperFlag())
		{
			pPlayer->QueryDataRelease();
			// 添加查询条件
			CTempEntityProperty* pGuid = new CTempEntityProperty(string("guid"), DATA_OBJECT_GUID);
			if(pGuid)
			{
				pGuid->SetGuidAttr( pPlayer->GetGUID());
				pGuid->SetDbRelateType(DB_RELATE_NULL);
				pPlayer->GetQueryDataVec().push_back(pGuid);
			}
		}
		// goodsgroup节点
		CEntityGroup* pGoodsGroup = GetGame()->GetEntityManager()->FindLeafGroupByComFlag(pPlayer, string("[goodsgroup]"));
		if(pGoodsGroup 
			&& pGoodsGroup->GetCurDbOperFlag() == 1
			&& pGoodsGroup->GetCurDbOperType() == DB_OPER_DELETE_INSERT)
		{
			// 先清空该对象的子节点数据库操作标志
			pGoodsGroup->QueryDataRelease();
			// 添加查询玩家的GUID和AND (
			CTempEntityProperty* pPlayerID = new CTempEntityProperty( string("PlayerID"), DATA_OBJECT_GUID);
			if(pPlayerID)
			{
				pPlayerID->SetGuidAttr( pPlayer->GetGUID());
				pPlayerID->SetDbRelateType(DB_RELATE_LBLANKAND);
				pGoodsGroup->GetQueryDataVec().push_back(pPlayerID);
			}
			// 添加子结点
			list<CEntityGroup*> listOperGroup;
			map<CGUID, CBaseEntity*>::iterator leafItr = ((CEntityGroup*)pGoodsGroup)->GetEntityGroupMap().begin();
			for(; leafItr != pGoodsGroup->GetEntityGroupMap().end(); leafItr++)
			{
				if(leafItr->second->GetCurDbOperFlag() == 1
					&& pGoodsGroup->GetCurDbOperType() == DB_OPER_DELETE_INSERT)
					listOperGroup.push_back((CEntityGroup*)leafItr->second);
			}

			if(listOperGroup.size())
			{
				list<CEntityGroup*>::iterator listItr = listOperGroup.begin();
				list<CEntityGroup*>::iterator listendItr = listOperGroup.end();
				// trick:先取得倒数第一个Group的Iter
				--listendItr;
				for(; listItr != listOperGroup.end(); listItr++)
				{
					if( listItr != listendItr ) // 不是倒数第1个
					{
						CTempEntityProperty* pRetName = new CTempEntityProperty(string("place"), DATA_OBJECT_LONG);
						if(pRetName)
						{
							tagEntityBuildInfo* pBuildInfo = GetEntityBuildInfo((*listItr)->GetCompositeFlag());
							if(pBuildInfo)
							{
								long lPlace = *((long*)&pBuildInfo->pQueryVar[0]);
								pRetName->SetLongAttr(lPlace);
								pRetName->SetDbRelateType(DB_RELATE_OR);
								pGoodsGroup->GetQueryDataVec().push_back(pRetName);
							}
						}
					}
					else
					{
						CTempEntityProperty* pRetName = new CTempEntityProperty(string("place"), DATA_OBJECT_LONG);
						if(pRetName)
						{
							tagEntityBuildInfo* pBuildInfo = GetEntityBuildInfo((*listItr)->GetCompositeFlag());
							if(pBuildInfo)
							{
								long lPlace = *((long*)&pBuildInfo->pQueryVar[0]);
								pRetName->SetLongAttr(lPlace);
								pRetName->SetDbRelateType(DB_RELATE_RBLANK);
								pGoodsGroup->GetQueryDataVec().push_back(pRetName);
							}
						}
					}
				}
			}
			else // 没有子结点要操作，清除其操作数据
			{
				pGoodsGroup->SetCurDbOperFlag(0);
				pGoodsGroup->QueryDataRelease();
			}
		}

		// 好友数据节点
		CEntityGroup* pFriendGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafGroupByComFlag(pPlayer, string("[playerfriendgroupOwner]"));
		if(pFriendGroup 
			&& pFriendGroup->GetCurDbOperFlag() == 1
			&& pFriendGroup->GetCurDbOperType() == DB_OPER_DELETE_INSERT)
		{
			// 先清空该对象的子节点数据库操作标志
			pFriendGroup->QueryDataRelease();
			CTempEntityProperty* pEP1 = new CTempEntityProperty( string("OwnerGUID"), DATA_OBJECT_GUID);
			if(pEP1)
			{
				pEP1->SetGuidAttr( pPlayer->GetGUID());
				pEP1->SetDbRelateType(DB_RELATE_NULL);
				pFriendGroup->GetQueryDataVec().push_back(pEP1);
			}
		}
		// 小精灵数据

		CEntityGroup* pSpriteGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafGroupByComFlag(pPlayer, string("[playerspritegroup]"));
		if(pSpriteGroup 
			&& pSpriteGroup->GetCurDbOperFlag() == 1
			&& pSpriteGroup->GetCurDbOperType() == DB_OPER_DELETE_INSERT)
		{
			pSpriteGroup->QueryDataRelease();
			CTempEntityProperty *pEP = new CTempEntityProperty( string("PlayerGuid"), DATA_OBJECT_GUID);
			if(pEP)
			{
				pEP->SetGuidAttr( pPlayer->GetGUID());
				pEP->SetDbRelateType(DB_RELATE_NULL);
				pSpriteGroup->GetQueryDataVec().push_back(pEP);
			}
		}
		// 宠物数据
		CEntityGroup* pPetList = GetGame()->GetEntityManager()->FindLeafGroupByComFlag(pPlayer, "[playerpetlist]");
		if( pPetList 
			&& pPetList->GetCurDbOperFlag() == 1
			&& pPetList->GetCurDbOperType() == DB_OPER_DELETE_INSERT)
		{
			pPetList->QueryDataRelease();
			CTempEntityProperty* pOwnerGuid = new CTempEntityProperty("HostID", DATA_OBJECT_GUID);
			pOwnerGuid->SetGuidAttr(pPlayer->GetGUID());
			pOwnerGuid->SetDbRelateType(DB_RELATE_NULL);
			pPetList->GetQueryDataVec().push_back(pOwnerGuid);
		}
	}
	return false;
}
//! 取得LoadThreadDataSize
long CEntityManager::GetLoadThreadDataSize(void)
{
	long lRet = 0;
	EnterCriticalSection(&m_LoadCriticalSection);
	lRet = m_listLoadThreadOperData.size();
	LeaveCriticalSection(&m_LoadCriticalSection);
	return lRet;
}
//! 遍历所有对象，产生消息
void CEntityManager::TravelForGenerateThreadData(void)
{
	static long lMaxThreadDataNum  = m_mapSaveThread.size();
	static long lHalfThreadDataNum = (lMaxThreadDataNum / 2) ? (lMaxThreadDataNum / 2):1;

	long lCurTravelNum = 0;

	if(m_listDirtyEntityName.size() == 0
		&& m_listDirtyEntityGuid.size() == 0 ) return;

	if(GetLoadThreadDataSize() != 0) return;

	list<CGUID> tmpBeginGuid;
	if(GetSaveThreadOperDataSize(tmpBeginGuid) < lMaxThreadDataNum)
	{
		long lCurThreadDataPos = 0;
		list<ThreadData*> threadDataList;

		if(m_listDirtyEntityName.size() != 0)
		{
			list<string> tmpCheckNameList;
			list<string>::iterator accItr = m_listDirtyEntityName.begin();
			while(accItr != m_listDirtyEntityName.end())
			{
				if(lCurTravelNum == GetGame()->GetSetup()->dwTravleMaxSize) break;
				// 判断是否有重复
				list<string>::iterator tmpCheckItr = std::find(tmpCheckNameList.begin(), tmpCheckNameList.end(), *accItr);
				if(tmpCheckItr != tmpCheckNameList.end())
				{
					++accItr;
					lCurTravelNum++;
					continue;
				}

				AccountMap::iterator itr = m_mapAccounts.find(*accItr);
				if(itr != m_mapAccounts.end())
				{
					// 产生ThreadData
					ThreadData* pData = new ThreadData;

					if(!itr->second->GetIsTempEntityFlag())
					{	
						CEntityGroup* tmpEntity = new Account(itr->second->GetGUID());
						*((CEntityGroup*)tmpEntity) = *((CEntityGroup*)(itr->second));
						pData->pEntity = tmpEntity;
						ResetEntityAllDbOperFlag(itr->second);
					}
					else// 如果时临时变量则取出来处理
					{
						pData->pEntity = ReleaseAccountPtr(itr->first);
					}
					threadDataList.push_back(pData);
					lCurThreadDataPos++;
				}
				tmpCheckNameList.push_back(*accItr);
				accItr = m_listDirtyEntityName.erase(accItr);
				
				lCurTravelNum++;

				if(lCurThreadDataPos == lHalfThreadDataNum)
					break;
			}
		}

		if(m_listDirtyEntityGuid.size() != 0)
		{
			list<CGUID> tmpCheckGuidList;
			list<CGUID>::iterator guidItr = m_listDirtyEntityGuid.begin();
			while(guidItr != m_listDirtyEntityGuid.end())
			{
				if(lCurTravelNum == GetGame()->GetSetup()->dwTravleMaxSize) break;
				// 判断是否在线程处理中
				list<CGUID>::iterator threadGuidItr = std::find(tmpBeginGuid.begin(), tmpBeginGuid.end(), *guidItr);
				if(threadGuidItr != tmpBeginGuid.end())
				{
					++guidItr;
					lCurTravelNum++;
					continue;
				}
				// 判断是否有重复
				threadGuidItr = std::find(tmpCheckGuidList.begin(), tmpCheckGuidList.end(), *guidItr);
				if(threadGuidItr != tmpCheckGuidList.end())
				{
					++guidItr;
					lCurTravelNum++;
					continue;
				}

				bool isFind = false;
				DBEntityComponentMapItr engroupitr = m_mapEntitys.begin();
				while(engroupitr != m_mapEntitys.end())
				{
					if(engroupitr->first == "[mailgroup]") 
					{
						++engroupitr;
						continue;
					}

					std::map<CGUID, CEntityGroup*>::iterator enItr = engroupitr->second.find(*guidItr);
					if(enItr != engroupitr->second.end())
					{
						isFind = true;
						// 产生ThreadData
						ThreadData* pData = new ThreadData;
						switch(enItr->second->GetCurDbOperType())
						{
						case DB_OPER_DELETE:
							{
								pData->pEntity = ReleaseEntityPtr(enItr->second->GetCompositeFlag(), enItr->first);
							}
							break;
						default:
							{								
								if(!enItr->second->GetIsTempEntityFlag())
								{
									CEntityGroup* tmpEntity = new CEntityGroup(enItr->second->GetCompositeFlag(), enItr->first);
									*((CEntityGroup*)tmpEntity) = *((CEntityGroup*)(enItr->second));
									pData->pEntity = tmpEntity;
									ResetEntityAllDbOperFlag(enItr->second);
								}
								else// 如果时临时变量则取出来处理
								{
									pData->pEntity = ReleaseEntityPtr(enItr->second->GetCompositeFlag(), enItr->first);
								}
							}
							break;
						}

						// 测试代码
#ifdef _TESTRUNSTACKINFO_
						{
							if(pData->pEntity->GetCompositeFlag() == "[player]")
							{
								char szOutPutStr[2048] = {0};
								char szOutGuid[128];
								pData->pEntity->GetGUID().tostring(szOutGuid);
								sprintf(szOutPutStr, "Travel() %s[%s] leaf num %d, optype %d, isTemp %d.", pData->pEntity->GetCompositeFlag().c_str(), szOutGuid, 
									((CEntityGroup*)pData->pEntity)->GetEntityGroupMap().size(), pData->pEntity->GetCurDbOperType(), pData->pEntity->GetIsTempEntityFlag());
								CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);

								map<CGUID, CBaseEntity*>::iterator tmpItr = ((CEntityGroup*)pData->pEntity)->GetEntityGroupMap().begin();
								for(; tmpItr != ((CEntityGroup*)pData->pEntity)->GetEntityGroupMap().end(); tmpItr++)
								{
									if(tmpItr->second->GetCompositeFlag() == "[goodsgroup]")
									{
										tmpItr->second->GetGUID().tostring(szOutGuid);
										sprintf(szOutPutStr, "--Travel() %s[%s] leaf num %d, optype %d, isTemp %d.--", 
											tmpItr->second->GetCompositeFlag().c_str(), szOutGuid, ((CEntityGroup*)tmpItr->second)->GetEntityGroupMap().size(),
											tmpItr->second->GetCurDbOperType(), tmpItr->second->GetIsTempEntityFlag());
										CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);

										map<CGUID, CBaseEntity*>::iterator leafItr = ((CEntityGroup*)tmpItr->second)->GetEntityGroupMap().begin();
										for(; leafItr != ((CEntityGroup*)tmpItr->second)->GetEntityGroupMap().end(); leafItr++)
										{
											leafItr->second->GetGUID().tostring(szOutGuid);
											sprintf(szOutPutStr, "--Travel() %s[%s] leaf num %d, optype %d, isTemp %d.--", 
												leafItr->second->GetCompositeFlag().c_str(), szOutGuid, ((CEntityGroup*)leafItr->second)->GetEntityGroupMap().size(),
												leafItr->second->GetCurDbOperType(), leafItr->second->GetIsTempEntityFlag());
											CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
										}
									}
								}
							}
						}
#endif

						threadDataList.push_back(pData);
						++lCurThreadDataPos;
						break;
					}
					++engroupitr;
				}

				if(isFind)
					tmpCheckGuidList.push_back(*guidItr);
				
				guidItr = m_listDirtyEntityGuid.erase(guidItr);

				lCurTravelNum++;
				
				if(lCurThreadDataPos > lMaxThreadDataNum)
					break;
			}
		}

		if (threadDataList.size())
			PushSaveThreadData(threadDataList);
	}
}
//! 取得存储过程的对象指针
CEntityGroup* CEntityManager::ReleaseProcedurePtr(const CGUID& guid)
{
	DBEntityComponentMapItr itr = m_mapEntitys.find(string("[procedure]"));
	if(itr != m_mapEntitys.end())
	{
		std::map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(guid);
		if(enItr != itr->second.end())
		{
			CEntityGroup* pRet = enItr->second;
			itr->second.erase(enItr);
			return pRet;
		}
	}
	return NULL;
}
//! 取得指定类型对象指针
CEntityGroup* CEntityManager::ReleaseEntityPtr(const string& strComFlag, const CGUID& guid)
{
	DBEntityComponentMapItr itr = m_mapEntitys.find(strComFlag);
	if(itr != m_mapEntitys.end())
	{
		std::map<CGUID, CEntityGroup*>::iterator enItr = itr->second.find(guid);
		if(enItr != itr->second.end())
		{
			CEntityGroup* pRet = enItr->second;
			itr->second.erase(enItr);
			return pRet;
		}
	}
	return NULL;
}
//! 取得account对象指针
CEntityGroup* CEntityManager::ReleaseAccountPtr(const string& strCdkey)
{
	// 现在vector中查找是否已经存在
	AccountMap::iterator itr = m_mapAccounts.find(strCdkey);
	if(itr != m_mapAccounts.end())
	{
		CEntityGroup* pRet = itr->second;
		m_mapAccounts.erase(itr);
		return pRet;
	}
	return NULL;
}

//! 处理从线程返回的消息
void CEntityManager::ProcessRetThreadData(void)
{
#ifdef _DEBUG_SM_INFO_
	char szOutPutStr[1024] = {0};
#endif

	list<ThreadData*> retDataList;
	GetRetThreadData(retDataList);
	list<ThreadData*>::iterator itr = retDataList.begin();
	for(; itr != retDataList.end(); itr++)
	{
		CEntityGroup* pBaseEntity = (CEntityGroup*)((*itr)->pEntity);
		if(!pBaseEntity)
		{
			AddLogText("ProcRetData() entity ptr is null!");
			continue;
		}

		// 如果是玩家存盘异常则单独处理
		if((*itr)->retFlag != S_OK)
		{
			// 弹出正在操作的对象GUID
			PopBeginEntityGuid(pBaseEntity->GetGUID(), (eThreadType)((*itr)->lThreadOpType));

			if(pBaseEntity->GetCompositeFlag() == "[player]" && pBaseEntity->GetCurDbOperType() == DB_OPER_SAVE)
			{
				// 重新存盘一次
				CEntityGroup* player = (CEntityGroup*)NewBaseEntity(std::string("[player]"), pBaseEntity->GetGUID());
				CEntityGroup* tplayer = FindRootEntity(std::string("[player]"), pBaseEntity->GetGUID());
				if(player && tplayer)
				{
					ResetEntityAllDbOperFlag(tplayer, true);
					*player = *tplayer;
					ResetEntityAllDbOperFlag(tplayer, false);
					player->SetCurDbOperType(DB_OPER_SAVE);
					player->SetCurDbOperFlag(1);

					CEntityGroup* pGoodsGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[goodsgroup]"));
					if(pGoodsGroup)
					{
						SetEntityAllDbOperType(pGoodsGroup, DB_OPER_DELETE_INSERT);
						ResetEntityAllDbOperFlag(pGoodsGroup, true);
					}

					// 好友数据
					CEntityGroup* pFriendGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[playerfriendgroup]"));
					if(pFriendGroup)
					{
						SetEntityAllDbOperType(pFriendGroup, DB_OPER_DELETE_INSERT);
						ResetEntityAllDbOperFlag(pFriendGroup, true);
					}

					// 小精灵数据
					CEntityGroup* pSpriteGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[playerspritegroup]"));
					if(pSpriteGroup)
					{
						SetEntityAllDbOperType(pSpriteGroup, DB_OPER_DELETE_INSERT);
						ResetEntityAllDbOperFlag(pSpriteGroup, true);
					}

					AddDirtyEntityGuid(player->GetGUID());
				}
				ThreadData* pTmpThreadData = new ThreadData;
				pTmpThreadData->pEntity = player;
				PushSaveThreadData(pTmpThreadData);
				
				// 重新计时删除时间
				const char* szCdkey = pBaseEntity->GetStringAttr(std::string("Account"));
				if(szCdkey)
				{
					Account* pAcc = FindAccount(szCdkey);
					if(pAcc)
					{
						if(pAcc->GetLastLoginTime() != -1)
							pAcc->SetLastLoginTime(timeGetTime());
					}
				}
				SAFE_DELETE(*itr);
				continue;
			}
		}

		if(pBaseEntity->GetCurDbOperType() == DB_OPER_CREATEROLE_PROCEDURE)
		{
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "IN ProcessRetThreadData() DB_OPER_CREATEROLE_PROCEDURE Start.");
			CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
			// 先判断创建是否成功
			if(pBaseEntity->GetRetQueryDataVec().size() == 2)
			{
				if((*itr)->retFlag == S_OK)
				{
					long NameIsExist = 0;
					long charNum = 0;	
					NameIsExist = pBaseEntity->GetRetQueryDataVec()[0]->GetLongAttr();
					charNum = pBaseEntity->GetRetQueryDataVec()[1]->GetLongAttr();
					if( NameIsExist == 0 && charNum == 1 )
					{
						CGUID tmpGuid;
						map<CGUID, CBaseEntity*>::iterator lgitr = ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().begin();
						lgitr->second->GetGuidAttr(string("guid"), tmpGuid);
						const char* szCdkey = lgitr->second->GetStringAttr(string("Account"));
						if(szCdkey)
						{
							Account* pAcc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
							if(pAcc)
							{
								lgitr->second->SetGUID(tmpGuid);
								CEntityGroup* pLoginPlayer = pAcc->FindLoginPlayer(tmpGuid);
								if(!pLoginPlayer)
								{
									pAcc->AddLoginPlayer((CEntityGroup*)lgitr->second);
									GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag(pAcc, false);
									pAcc->SetIsTempEntityFlag(0);
								}
								else
									delete lgitr->second;

								lgitr++;
								lgitr->second->SetGUID(tmpGuid);
								CEntityGroup* pPlayer = GetGame()->GetEntityManager()->FindRootEntity(std::string("[player]"), tmpGuid);
								if(!pPlayer)
								{
									GetGame()->GetEntityManager()->AddOneRootEntity((CEntityGroup*)lgitr->second);
									GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag((CEntityGroup*)lgitr->second, false);
									lgitr->second->SetIsTempEntityFlag(0);
								}
								else
									delete lgitr->second;

								pBaseEntity->ReleaseChildPtr(NULL_GUID);
								pBaseEntity->ReleaseChildPtr(tmpGuid);
							}
							else
								pBaseEntity->ReleaseChilds();
						}
						else
							pBaseEntity->ReleaseChilds();
					}
					else
						pBaseEntity->ReleaseChilds();
				}
				else
				{
					map<CGUID, CBaseEntity*>::iterator lgitr = ((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().begin();
					const char* szCdkey = lgitr->second->GetStringAttr(string("Account"));
					AddLogText("CreatePlayer: [%s], [%d] failed!", szCdkey, (*itr)->retFlag);
					pBaseEntity->ReleaseChilds();
				}
				// 发送消息
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				msg.Add(pBaseEntity->GetSessionID());
				msg.Add((long)(*itr)->retFlag);
				msg.Add(pBaseEntity->GetCompositeFlag().c_str());
				msg.Add(pBaseEntity->GetGUID());
				msg.Add("");
				msg.Add((BYTE)0);
				vector<BYTE> pBA;
				pBaseEntity->AddToByteArray(pBA, false);
				msg.Add((long)pBA.size());
				if(pBA.size())
					msg.AddEx(&pBA[0], pBA.size());
				msg.SendToSocket(GetGame()->GetWSSocket());
			}
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "IN ProcessData() DB_OPER_CREATEROLE_PROCEDURE End.");
			CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
	}
		else if(pBaseEntity->GetCurDbOperType() == DB_OPER_CHANGENAME_PROCEDURE)
		{
			if((*itr)->retFlag == S_OK && pBaseEntity->GetRetQueryDataVec().size() == 1 && pBaseEntity->GetRetQueryDataVec()[0] && pBaseEntity->GetRetQueryDataVec()[0]->GetLongAttr() == 1) // 修改成功
			{
				if(pBaseEntity->GetQueryDataVec().size() == 2 && pBaseEntity->GetQueryDataVec()[0] && pBaseEntity->GetQueryDataVec()[1])
				{
					CGUID guid;
					pBaseEntity->GetQueryDataVec()[0]->GetGuidAttr(guid);
					char* szName = (char*)pBaseEntity->GetQueryDataVec()[1]->GetStringAttr();
					CEntityGroup* pPlayer = FindRootEntity(string("[player]"), guid);
					if(szName && pPlayer)
						pPlayer->SetStringAttr(string("Name"), szName);
				}
			}

			CMessage msg(MSG_DB2W_OPER_ENTITY);
			msg.Add(pBaseEntity->GetSessionID());
			msg.Add((long)(*itr)->retFlag);
			msg.Add(pBaseEntity->GetCompositeFlag().c_str());
			msg.Add(pBaseEntity->GetGUID());
			msg.Add(pBaseEntity->GetName());
			msg.Add((BYTE)pBaseEntity->GetFindByNameFlag());
			// 发送消息
			vector<BYTE> pBA;
			pBaseEntity->AddToByteArray(pBA, false);
			msg.Add((long)pBA.size());
			if(pBA.size())
				msg.AddEx(&pBA[0], pBA.size());
			msg.SendToSocket(GetGame()->GetWSSocket());
		}
		else
		{
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "IN ProcessData() opFlag:%d, Entity[%s] Start.",pBaseEntity->GetCurDbOperType(),pBaseEntity->GetCompositeFlag().c_str());
			CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif

			// 内存缓冲区内对象更新
			if(pBaseEntity->GetCurDbOperType() == DB_OPER_LOAD)
			{
				if((*itr)->retFlag == S_OK)
				{
					GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag(pBaseEntity);
					pBaseEntity->SetIsTempEntityFlag(0);

					// 肯定是根结点
					if( pBaseEntity->GetCompositeFlag() == "[account]" )
					{
						Account* pTmpAcc = GetGame()->GetEntityManager()->FindAccount(pBaseEntity->GetName());
						if(!pTmpAcc)
						{
							GetGame()->GetEntityManager()->AddAccount((Account*)pBaseEntity);
							// 转移指针的控制权
							(*itr)->pEntity = NULL;
						}
						else
							(*itr)->retFlag = E_FAIL;
					}
					else
					{
						CEntityGroup* pPlayer = GetGame()->GetEntityManager()->FindRootEntity(std::string("[player]"), pBaseEntity->GetGUID());
						if(!pPlayer)
						{
							GetGame()->GetEntityManager()->AddOneRootEntity(pBaseEntity);
							// 转移指针的控制权
							(*itr)->pEntity = NULL;
						}
						else
							(*itr)->retFlag = E_FAIL;
					}
				}
			}

			// 只有Load操作才返回数据
			if(pBaseEntity->GetCurDbOperType() == DB_OPER_LOAD
				|| pBaseEntity->GetCurDbOperType() == DB_OPER_LOAD_NOCACHE
				|| pBaseEntity->GetCurDbOperType() == DB_OPER_PROCEDURE
				|| pBaseEntity->GetCurDbOperType() == DB_OPER_INSERTMAIL_PROCEDURE)
			{
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				msg.Add(pBaseEntity->GetSessionID());
				msg.Add((long)(*itr)->retFlag);
				msg.Add(pBaseEntity->GetCompositeFlag().c_str());
				msg.Add(pBaseEntity->GetGUID());
				msg.Add(pBaseEntity->GetName());
				msg.Add((BYTE)pBaseEntity->GetFindByNameFlag());
				// 发送消息
				vector<BYTE> pBA;
				if(pBaseEntity->GetCurDbOperType() == DB_OPER_LOAD
					||pBaseEntity->GetCurDbOperType() == DB_OPER_LOAD_NOCACHE)
					pBaseEntity->AddToByteArray(pBA, true);
				else
					pBaseEntity->AddToByteArray(pBA, false);

				msg.Add((long)pBA.size());
				if(pBA.size())
					msg.AddEx(&pBA[0], pBA.size());
				msg.SendToSocket(GetGame()->GetWSSocket());
			}
			else if(pBaseEntity->GetCurDbOperType() == DB_OPER_SAVE && pBaseEntity->GetDelLeafGuid() != NULL_GUID)
			{
				CEntityGroup* pFather = GetGame()->GetEntityManager()->FindLeafFatherByComFlag(pBaseEntity, 
					pBaseEntity->GetDelLeafFlag());
				if(pFather)
					pFather->ReleaseLeaf(pBaseEntity->GetDelLeafGuid());
			}
			else if(pBaseEntity->GetCurDbOperType() == DB_OPER_DELETE)
			{
				GetGame()->GetEntityManager()->DelOneRootEntity(pBaseEntity->GetCompositeFlag(), pBaseEntity->GetGUID());
			}

#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "IN ProcessData() opFlag:%d, Entity[%s] End.",
				pBaseEntity->GetCurDbOperType(),pBaseEntity->GetCompositeFlag().c_str());
			CMessage::AsyWriteFile(GetGame()->GetMainStatckFileName(),szOutPutStr);
#endif
		}
		SAFE_DELETE(*itr);
	}
	retDataList.clear();
}
//	不重复的插入一个变化对象的GUID
void CEntityManager::AddDirtyEntityGuid(const CGUID& guid)
{
	list<CGUID>::iterator itr = std::find(m_listDirtyEntityGuid.begin(), m_listDirtyEntityGuid.end(), guid);
	if(itr == m_listDirtyEntityGuid.end())
	{
		m_listDirtyEntityGuid.push_back(guid);
	}
}
void CEntityManager::AddDirtyEntityName(const char* szName)
{
	if(szName)
	{
		string tmpStr = szName;
		list<string>::iterator itr = std::find(m_listDirtyEntityName.begin(), m_listDirtyEntityName.end(), tmpStr);
		if(itr == m_listDirtyEntityName.end())
		{
			m_listDirtyEntityName.push_back(tmpStr);
		}
	}
}
CBaseEntity* CEntityManager::NewBaseEntity(const string& strComFlag, const CGUID& guid)
{
	CBaseEntity* pEntity = NULL;
	tagEntityBuildInfo* pInfo = GetEntityBuildInfo(strComFlag);
	if(pInfo)
	{
		if(pInfo->lComType == COM_COMPOSITE)
			pEntity = new CEntityGroup(strComFlag, guid);
		else
			pEntity = new CEntity(strComFlag, guid);
	}
	return pEntity;
}
void CEntityManager::DelBaseEntity(CBaseEntity** pEntity)
{
	SAFE_DELETE(*pEntity);
}
// 保存全部对象
void CEntityManager::SaveAllEntity(void)
{
	// 保存玩家对象
	DBEntityComponentMapItr epItr = m_mapEntitys.find(string("[player]"));
	if(epItr != m_mapEntitys.end())
	{
		map<CGUID, CEntityGroup*>::iterator playerItr = epItr->second.begin();
		for(; playerItr != epItr->second.end(); playerItr++)
		{
			CEntityGroup* player = playerItr->second;
			if(player)
			{
				player->SetCurDbOperType(DB_OPER_SAVE);
				player->SetCurDbOperFlag(1);

				CEntityGroup* pGoodsGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[goodsgroup]"));
				if(pGoodsGroup)
				{
					SetEntityAllDbOperType(pGoodsGroup, DB_OPER_DELETE_INSERT);
					ResetEntityAllDbOperFlag(pGoodsGroup, true);
				}

				// 好友数据
				CEntityGroup* pFriendGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[playerfriendgroup]"));
				if(pFriendGroup)
				{
					SetEntityAllDbOperType(pFriendGroup, DB_OPER_DELETE_INSERT);
					ResetEntityAllDbOperFlag(pFriendGroup, true);
				}

				// 小精灵数据
				CEntityGroup* pSpriteGroup = (CEntityGroup*)FindLeafGroupByComFlag(player, string("[playerspritegroup]"));
				if(pSpriteGroup)
				{
					SetEntityAllDbOperType(pSpriteGroup, DB_OPER_DELETE_INSERT);
					ResetEntityAllDbOperFlag(pSpriteGroup, true);
				}

				AddDirtyEntityGuid(player->GetGUID());
			}
		}
	}
	AddLogText("保存玩家详细数据成功(等待线程结束)！");

	AccountMapItr accItr = m_mapAccounts.begin();
	for(; accItr != m_mapAccounts.end(); accItr++)
	{
		CEntityGroup* pAcc = accItr->second;
		if(pAcc)
		{
			// 关闭定时删除状态,存盘时不能清除
			((Account*)pAcc)->SetLastLoginTime(-1);

			pAcc->SetCurDbOperType(DB_OPER_SAVE);
			pAcc->SetCurDbOperFlag(1);

			CEntityGroup* lpGroup = (CEntityGroup*)FindLeafGroupByComFlag(pAcc, string("[loginplayergroup]"));
			if(lpGroup)
			{
				SetEntityAllDbOperType(lpGroup, DB_OPER_SAVE);
				ResetEntityAllDbOperFlag(lpGroup, true);
			}

			// 为Loginplayer添加查询条件
			map<CGUID, CBaseEntity*>::iterator lpItr = lpGroup->GetEntityGroupMap().begin();
			for(; lpItr != lpGroup->GetEntityGroupMap().end(); lpItr++)
			{
				((CEntityGroup*)lpItr->second)->QueryDataRelease();
				((CEntityGroup*)lpItr->second)->RetQueryDataRelease();
				CTempEntityProperty* pGuid = new CTempEntityProperty(string("guid"), DATA_OBJECT_GUID);
				if(pGuid)
				{
					pGuid->SetGuidAttr( lpItr->first );
					pGuid->SetDbRelateType(DB_RELATE_NULL);
					((CEntityGroup*)lpItr->second)->GetQueryDataVec().push_back(pGuid);
				}
			}
			AddDirtyEntityName(pAcc->GetName());
		}
	}
	AddLogText("保存玩家基本数据成功(等待线程结束)！");

	epItr = m_mapEntitys.find(string("[scriptvargroup]"));
	if(epItr != m_mapEntitys.end())
	{
		map<CGUID, CEntityGroup*>::iterator accItr = epItr->second.begin();
		for(; accItr != epItr->second.end(); accItr++)
		{
			CEntityGroup* pVarGroup = accItr->second;
			if(pVarGroup)
			{
				ResetEntityAllDbOperFlag(pVarGroup, true);
				SetEntityAllDbOperType(pVarGroup, DB_OPER_DELETE_INSERT);
				AddDirtyEntityGuid(pVarGroup->GetGUID());
			}
		}
	}
	AddLogText("保存全局变量数据成功(等待线程结束)！");

	epItr = m_mapEntitys.find(string("[factiongroup]"));
	if(epItr != m_mapEntitys.end())
	{
		map<CGUID, CEntityGroup*>::iterator facGroupItr = epItr->second.begin();
		for(; facGroupItr != epItr->second.end(); facGroupItr++)
		{
			CEntityGroup* pFacGroup = facGroupItr->second;
			if(pFacGroup)
			{
				map<CGUID, CBaseEntity*>::iterator facItr = pFacGroup->GetEntityGroupMap().begin();
				for(; facItr != pFacGroup->GetEntityGroupMap().end(); facItr++)
				{
					CEntityGroup *pFaction = (CEntityGroup*)(facItr->second);
					if(pFaction == NULL) continue;

					pFaction->QueryDataRelease();
					pFaction->RetQueryDataRelease();

					// 设置查询条件
					CTempEntityProperty* pFactionGuid = new CTempEntityProperty(string("FactionGuid"), DATA_OBJECT_GUID);
					pFactionGuid->SetGuidAttr(pFaction->GetGUID());
					pFactionGuid->SetDbRelateType(DB_RELATE_NULL);
					pFaction->GetQueryDataVec().push_back(pFactionGuid);

					// 设置成员查询条件
					CEntityGroup* pMemberGroup = NULL;
					map<string, CGUID>::iterator guidItr = pFaction->GetGuidByComFlagMap().find(string("[factionmembergroup]"));
					if(guidItr != pFaction->GetGuidByComFlagMap().end())
					{
						map<CGUID, CBaseEntity*>::iterator enItr = pFaction->GetEntityGroupMap().find(guidItr->second);
						if(enItr != pFaction->GetEntityGroupMap().end())
							pMemberGroup = (CEntityGroup*)enItr->second;
					}
					if(pMemberGroup)
					{
						((CEntityGroup*)pMemberGroup)->QueryDataRelease();
						((CEntityGroup*)pMemberGroup)->RetQueryDataRelease();

						CTempEntityProperty* pFactionGuid_members = new CTempEntityProperty(string("FactionGuid_members"), DATA_OBJECT_GUID);
						pFactionGuid_members->SetGuidAttr( pFaction->GetGUID());
						pFactionGuid_members->SetDbRelateType(DB_RELATE_NULL);
						pMemberGroup->GetQueryDataVec().push_back(pFactionGuid_members);
						char szFacGuid[128];
						pFaction->GetGUID().tostring(szFacGuid);
						map<CGUID, CBaseEntity*>::iterator enItr = pMemberGroup->GetEntityGroupMap().begin();
						for(; enItr != pMemberGroup->GetEntityGroupMap().end(); enItr++)
						{
							CGUID tmpGuid;
							enItr->second->GetGuidAttr(string("FactionGuid_members"),  tmpGuid);
							tmpGuid.tostring(szFacGuid);
						}
					}

					CEntityGroup* pApplyGroup = NULL;
					guidItr = pFaction->GetGuidByComFlagMap().find(string("[factionapplygroup]"));
					if(guidItr != pFaction->GetGuidByComFlagMap().end())
					{
						map<CGUID, CBaseEntity*>::iterator enItr = pFaction->GetEntityGroupMap().find(guidItr->second);
						if(enItr != pFaction->GetEntityGroupMap().end())
							pApplyGroup = (CEntityGroup*)enItr->second;
					}
					if(pApplyGroup)
					{
						((CEntityGroup*)pApplyGroup)->QueryDataRelease();
						((CEntityGroup*)pApplyGroup)->RetQueryDataRelease();

						CTempEntityProperty* pFactionGuid_apply = new CTempEntityProperty(string("FactionGuid_apply"), DATA_OBJECT_GUID);
						pFactionGuid_apply->SetGuidAttr( pFaction->GetGUID());
						pFactionGuid_apply->SetDbRelateType(DB_RELATE_NULL);
						pApplyGroup->GetQueryDataVec().push_back(pFactionGuid_apply);
					}
					
					SetEntityAllDbOperType(pFaction, DB_OPER_DELETE_INSERT);
					ResetEntityAllDbOperFlag(pFaction, true);
				}

				pFacGroup->SetCurDbOperType(DB_OPER_SAVE);
				pFacGroup->SetCurDbOperFlag(1);
				AddDirtyEntityGuid(pFacGroup->GetGUID());
			}
		}
	}
	AddLogText("保存帮会数据成功(等待线程结束)！");

	GetGame()->SetSaveStartFlag(true);
}

//!	清除玩家
bool CEntityManager::ClearEntitys(void)
{
	DBEntityComponentMapItr enItr = m_mapEntitys.begin();
	for (; enItr != m_mapEntitys.end(); enItr++)
	{
		map<CGUID, CEntityGroup*>::iterator entityItr = enItr->second.begin();
		for(; entityItr != enItr->second.end(); entityItr++)
			delete entityItr->second;
	}
	m_mapEntitys.clear();

	AccountMapItr accItr = m_mapAccounts.begin();
	for(; accItr != m_mapAccounts.end(); accItr++)
		delete accItr->second;
	m_mapAccounts.clear();
	AddLogText("清除全部缓存数据成功！");
	return true;
}

//! 查找是否在操作队列有该账号
bool CEntityManager::FindAccountIsInOperQueue(const string& strAcc)
{
	list<string>::iterator itr = std::find(m_listDirtyEntityName.begin(), m_listDirtyEntityName.end(), strAcc);
	if(itr != m_listDirtyEntityName.end())
		return true;
	return false;
}
bool CEntityManager::FindEntityIsInOperQueue(const CGUID& guid)
{
	list<CGUID>::iterator itr = std::find(m_listDirtyEntityGuid.begin(), m_listDirtyEntityGuid.end(), guid);
	if(itr != m_listDirtyEntityGuid.end())
		return true;

	itr = std::find(m_listSaveBeginEntityGuid.begin(), m_listSaveBeginEntityGuid.end(), guid);
	if(itr != m_listSaveBeginEntityGuid.end())
		return true;

	return false;
}