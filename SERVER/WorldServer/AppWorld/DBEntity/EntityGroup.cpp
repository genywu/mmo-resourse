//EntityGroup.cpp/////////////////////////////////////////////////////////////////////
//对象类:该对象有其自己的属性数据,还包含子节点对象
//Author:安海川
//Create Time:2008.11.03
#include "StdAfx.h"
#include "entityManager.h"
#include "EntityGroup.h"
#include <assert.h>
#include "../public/entityproperty.h"
#include "..\nets\networld\message.h"


using namespace std;

CEntityGroup::CEntityGroup(const string& flag)
: CBaseEntity(),
m_lLeafComType(-1),
m_lLeafComFlag(""),
m_lDetailComPositeFlagType(-1),
m_bHasDetailLeavesFlag(0),
m_bIsFindByName(false)
{
	memset(m_szName, 0, sizeof(m_szName));
	CBaseEntity::m_CompositeFlag = flag;
	InitProperty();
	map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.begin();
	for(; itr != m_mapEntityGroup.end(); itr++)
	{
		itr->second->InitProperty();
	}
}

CEntityGroup::CEntityGroup(const string& flag, const CGUID& id)
: CBaseEntity(id),
m_lLeafComType(-1),
m_lLeafComFlag(""),
m_lDetailComPositeFlagType(-1),
m_bHasDetailLeavesFlag(0),
m_bIsFindByName(false)
{
	memset(m_szName, 0, sizeof(m_szName));
	CBaseEntity::m_CompositeFlag = flag;
	InitProperty();
	m_GUID = id;
	map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.begin();
	for(; itr != m_mapEntityGroup.end(); itr++)
	{
		itr->second->InitProperty();
	}
}
	CEntityGroup::~CEntityGroup()
	{
		// 1 清除所有的叶节点对象
		std::map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.begin();
		for(; itr != m_mapEntityGroup.end(); itr++)
		{
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
		}
		m_mapEntityGroup.clear();

		vector<CTempEntityProperty*>::iterator retitr = m_pQueryDataMgr.begin();
		for(; retitr != m_pQueryDataMgr.end(); retitr++)
			delete (*retitr);
		m_pQueryDataMgr.clear();

		retitr = m_pRetQueryDataMgr.begin();
		for(; retitr != m_pRetQueryDataMgr.end(); retitr++)
			delete (*retitr);
		m_pRetQueryDataMgr.clear();
	}
	CEntityGroup& CEntityGroup::operator=(CEntityGroup& right)
	{
		CBaseEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;

			map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.begin();
			for(; itr != m_mapEntityGroup.end(); itr++)
			{
				GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
			}
			m_mapEntityGroup.clear();

			itr = right.m_mapEntityGroup.begin();
			for(; itr != right.m_mapEntityGroup.end(); itr++)
			{
				CBaseEntity* tGoods = GetGame()->GetEntityManager()->NewBaseEntity(itr->second->GetCompositeFlag(), itr->first);
				*tGoods = *(itr->second);
				m_mapEntityGroup[itr->first] = tGoods;
			}
			m_mapGuidByComFlag = right.m_mapGuidByComFlag;
			m_lLeafComType = right.m_lLeafComType;
			m_lLeafComFlag = right.m_lLeafComFlag;
			// 其父节点的子类型区分的数据库变量名
			m_strDetailComPositeFlagName = right.m_strDetailComPositeFlagName;
			m_lDetailComPositeFlagType = right.m_lDetailComPositeFlagType;
			m_bHasDetailLeavesFlag = right.m_bHasDetailLeavesFlag;
		}
		return *this;
	}
	void CEntityGroup::SetName(const char* pName) 
	{
		if (NULL != pName)
		{
			long lBufSize = sizeof(m_szName);
			long lSize = strlen(pName);
			lSize = (lBufSize >= lSize ) ? lSize : lBufSize;
			memmove(m_szName, pName, lSize);
			if(lSize == lBufSize)
				m_szName[lSize-1] = '\0';
		}
		else
		{
			AddLogText("SetName:参数为空！");
		}
	}
	// virtual interfaces
	void CEntityGroup::ProcessMsg(bool isLoadOperFlag, const CGUID& SessionID, long retFlag, BYTE* msgBuf, long& msgBufPos, long bufSize)
	{
		// 需要记录一个时间戳,在存盘线程做
		GetFromByteArray(isLoadOperFlag, msgBuf, msgBufPos, bufSize);
	}

	void CEntityGroup::AddToByteArray(vector<BYTE>& pBA)
	{
		_AddToByteArray(&pBA, (BYTE)GetCompositeType());
		_AddToByteArray(&pBA, GetCompositeFlag().c_str());
		_AddToByteArray(&pBA, m_GUID); // 实体ID

		_AddToByteArray(&pBA, (BYTE)GetCurDbOperFlag());
		_AddToByteArray(&pBA, (BYTE)GetCurDbOperType());

		if(GetCurDbOperFlag() == false) return;

		// 查询变量
		_AddToByteArray(&pBA, (WORD)GetQueryDataVec().size());
		vector<CTempEntityProperty*>::iterator epItr = GetQueryDataVec().begin();
		for(; epItr != GetQueryDataVec().end(); epItr++)
		{	
			_AddToByteArray(&pBA, (*epItr)->GetVarName().c_str());
			_AddToByteArray(&pBA, (long)(*epItr)->GetDataType());//变量类型（long）
			_AddToByteArray(&pBA, (long)(*epItr)->GetBufSize());//变量数据长度（long）

			switch((*epItr)->GetDataType())
			{
			case DATA_OBJECT_BUFFER:
				{
					long lQueryBufSize = (*epItr)->GetBufSize();
					if(lQueryBufSize)
					{
						BYTE* queryBuf = (BYTE*)M_ALLOC(lQueryBufSize);
						(*epItr)->GetBufAttr(queryBuf, lQueryBufSize);
						_AddToByteArray(&pBA, queryBuf, sizeof(queryBuf));
						M_FREE(queryBuf,lQueryBufSize);
					}
				}
				break;
			case  DATA_OBJECT_STRING:
				{
					_AddToByteArray(&pBA, (*epItr)->GetStringAttr());
				}
				break;
			case DATA_OBJECT_GUID:
				{
					CGUID tGuid;
					(*epItr)->GetGuidAttr(tGuid);
					_AddToByteArray(&pBA, tGuid);
				}
				break;
			case DATA_OBJECT_BOOL:
			case DATA_OBJECT_CHAR:
			case DATA_OBJECT_BYTE:
			case DATA_OBJECT_SHORT:
			case DATA_OBJECT_WORD:
			case DATA_OBJECT_USHORT:
			case DATA_OBJECT_FLOAT:
			case DATA_OBJECT_LONG:
			case DATA_OBJECT_ULONG:
			case DATA_OBJECT_DWORD:
				{
					_AddToByteArray(&pBA, (long)(*epItr)->GetLongAttr());
				}
				break;
			}

			_AddToByteArray(&pBA, (BYTE)(*epItr)->GetDbRelateType());
		}

		// 查询返回变量
		_AddToByteArray(&pBA, (WORD)GetRetQueryDataVec().size());
		epItr = GetRetQueryDataVec().begin();
		for(; epItr != GetRetQueryDataVec().end(); epItr++)
		{	
			long lDataType = GetGame()->GetEntityManager()->GetDataObjectType(GetCompositeFlag(), (*epItr)->GetAttrNumEnum());
			_AddToByteArray(&pBA, (*epItr)->GetVarName().c_str());
			_AddToByteArray(&pBA, (long)(*epItr)->GetDataType());//变量类型（long）

			switch((*epItr)->GetDataType())
			{
			case  DATA_OBJECT_STRING:
				{
					_AddToByteArray(&pBA, (*epItr)->GetStringAttr());
				}
				break;
			case DATA_OBJECT_GUID:
				{
					CGUID tGuid;
					(*epItr)->GetGuidAttr(tGuid);
					_AddToByteArray(&pBA, tGuid);
				}
				break;
			case DATA_OBJECT_BOOL:
			case DATA_OBJECT_CHAR:
			case DATA_OBJECT_BYTE:
			case DATA_OBJECT_SHORT:
			case DATA_OBJECT_WORD:
			case DATA_OBJECT_USHORT:
			case DATA_OBJECT_FLOAT:
			case DATA_OBJECT_LONG:
			case DATA_OBJECT_ULONG:
			case DATA_OBJECT_DWORD:
				{
					_AddToByteArray(&pBA, (long)(*epItr)->GetLongAttr());
				}
				break;
			}

			_AddToByteArray(&pBA, (BYTE)(*epItr)->GetDbRelateType());
		}
		
		//-------------------------------------
		_AddToByteArray(&pBA, (WORD)GetDataEntityManager().GetEntityPropertyMap().size()); // 属性种类个数
		//--新格式
		//变量名字符串长度（long）
		//变量名字符串
		//变量类型（long）
		//变量数据长度（long）
		//变量数据
		CDataEntityManager::EntityPropertyMapItr itr = GetDataEntityManager().GetEntityPropertyMap().begin();
		for(; itr != GetDataEntityManager().GetEntityPropertyMap().end(); itr++)
		{
			long lDataType = GetGame()->GetEntityManager()->GetDataObjectType(GetCompositeFlag(), itr->first);
			_AddToByteArray(&pBA, (long)itr->first);//变量名字枚举值（long）
			_AddToByteArray(&pBA, (long)itr->second->GetBufSize());//变量数据长度（long）

			switch(lDataType)
			{
			case DATA_OBJECT_TIME:
				{
					// 6个long值 : 年 月 日 时 分 秒
					BYTE timeValue[6*sizeof(DWORD)];
					memset(timeValue, 0, 6*sizeof(DWORD));
					itr->second->GetBufAttr(timeValue, 6*sizeof(DWORD));
					_AddToByteArray(&pBA, timeValue, 6*sizeof(DWORD));
				}
				break;
			case DATA_OBJECT_STRING:
				{
					_AddToByteArray(&pBA, itr->second->GetStringAttr());//变量数据
				}
				break;
			case DATA_OBJECT_FLOAT:
			case DATA_OBJECT_LONG:
			case DATA_OBJECT_SHORT:
			case DATA_OBJECT_ULONG:
			case DATA_OBJECT_USHORT:
			case DATA_OBJECT_BOOL:
			case DATA_OBJECT_CHAR:
			case DATA_OBJECT_BYTE:
			case DATA_OBJECT_WORD:
			case DATA_OBJECT_DWORD:
				{
					_AddToByteArray(&pBA, (long)itr->second->GetLongAttr());//变量数据
				}
				break;
			case DATA_OBJECT_GUID:
				{
					CGUID tGUID;
					itr->second->GetGuidAttr(tGUID);
					_AddToByteArray(&pBA, tGUID);//变量数据
				}
				break;
			case DATA_OBJECT_BUFFER:
				{
					long bufSize = itr->second->GetBufSize();
					if(bufSize)
					{
						BYTE* buf = (BYTE*)M_ALLOC(bufSize);
						itr->second->GetBufAttr(buf, bufSize);
						_AddToByteArray(&pBA, buf, bufSize);//变量数据
						M_FREE(buf,bufSize);
					}
				}
				break;
			}

		}

		//---------------------------------------
		// childNum:		2byte, 子节点个数(X)
		_AddToByteArray(&pBA, (WORD)m_mapEntityGroup.size());

		map<CGUID, CBaseEntity*>::iterator entityItr = m_mapEntityGroup.begin();
		for(; entityItr != m_mapEntityGroup.end(); entityItr++)
		{
			(entityItr->second)->AddToByteArray(pBA);
		}
		QueryDataRelease();
	}

	void CEntityGroup::GetFromByteArray(bool isLoadOperFlag, BYTE* msgBuf, long& msgBufPos, long bufSize)
	{
		RetQueryDataRelease();


		// 查询返回的变量
		long lQueryAttrNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize);

		for(int j=0; j<lQueryAttrNum; j++)
		{	
			char szQueryStrName[128];
			_GetStringFromByteArray(msgBuf, msgBufPos, szQueryStrName, bufSize);
			DATA_OBJECT_TYPE objType = (DATA_OBJECT_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量类型（long）
			
			CTempEntityProperty* pEP = new CTempEntityProperty(szQueryStrName, objType);
			if(pEP)
			{
				switch(objType)
				{
				case  DATA_OBJECT_STRING:
					{
						char szValue[128];
						memset(szValue, 0, sizeof(szValue));
						_GetStringFromByteArray(msgBuf, msgBufPos, szValue, bufSize);
						pEP->SetStringAttr(string(szValue));
					}
					break;
				case DATA_OBJECT_GUID:
					{
						CGUID tGuid;
						_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid, bufSize);
						pEP->SetGuidAttr(tGuid);
					}
					break;
				case DATA_OBJECT_BOOL:
				case DATA_OBJECT_CHAR:
				case DATA_OBJECT_BYTE:
				case DATA_OBJECT_SHORT:
				case DATA_OBJECT_WORD:
				case DATA_OBJECT_USHORT:
				case DATA_OBJECT_FLOAT:
				case DATA_OBJECT_LONG:
				case DATA_OBJECT_ULONG:
				case DATA_OBJECT_DWORD:
					{
						pEP->SetLongAttr(_GetLongFromByteArray(msgBuf, msgBufPos, bufSize));
					}
					break;
				}

				DB_RELATE_TYPE relateType = (DB_RELATE_TYPE)_GetByteFromByteArray(msgBuf, msgBufPos, bufSize);
				pEP->SetDbRelateType(relateType);
				m_pRetQueryDataMgr.push_back(pEP);
			}
		}

		// 解析消息
		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 属性种类个数

		if(attrNum)
		{
			for(int i=0; i<(int)attrNum; i++)
			{
				long lAttrEnum = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量类型枚举值（long）
				long varBufLen = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量数据长度（long）

				CEntityProperty* ep = NULL;
				long bufType = GetGame()->GetEntityManager()->GetDataObjectType(GetCompositeFlag(), lAttrEnum);
				CDataEntityManager::EntityPropertyMapItr itr = GetDataEntityManager().GetEntityPropertyMap().find(lAttrEnum);
				if(itr != GetDataEntityManager().GetEntityPropertyMap().end())
					ep = itr->second;

				assert(ep != NULL);

				switch((DATA_OBJECT_TYPE)bufType)
				{
				case DATA_OBJECT_TIME:
					{
						// 6个long值 : 年 月 日 时 分 秒
						BYTE timeValue[6*sizeof(DWORD)];
						memset(timeValue, 0, 6*sizeof(DWORD));
						_GetBufferFromByteArray(msgBuf, msgBufPos, timeValue, 6*sizeof(DWORD), bufSize);
						ep->SetBufAttr(timeValue, 6*sizeof(DWORD));
					}
					break;
				case DATA_OBJECT_STRING:
					{
						char varValue[1024];
						_GetStringFromByteArray(msgBuf, msgBufPos, varValue, bufSize);//变量数据
						ep->SetStringAttr(string((char*)varValue));
					}
					break;
				case DATA_OBJECT_FLOAT:
				case DATA_OBJECT_LONG:
				case DATA_OBJECT_SHORT:
				case DATA_OBJECT_ULONG:
				case DATA_OBJECT_USHORT:
				case DATA_OBJECT_BOOL:
				case DATA_OBJECT_CHAR:
				case DATA_OBJECT_BYTE:
				case DATA_OBJECT_WORD:
				case DATA_OBJECT_DWORD:
					{
						long value = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量数据
						ep->SetLongAttr((long)value);
					}
					break;
				case DATA_OBJECT_GUID:
					{
						CGUID tGUID;
						_GetBufferFromByteArray(msgBuf, msgBufPos, tGUID, bufSize);//变量数据
						ep->SetGuidAttr(tGUID);
					}
					break;
				case DATA_OBJECT_BUFFER:
					{
						BYTE* buf = (BYTE*)M_ALLOC(varBufLen);
						_GetBufferFromByteArray(msgBuf, msgBufPos, buf, varBufLen, bufSize);//变量数据
						ep->SetBufAttr(buf, varBufLen);
						M_FREE(buf,varBufLen);
					}
					break;
				}
			}
		}
		
		// 是否有数据标志位
		WORD num = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize);

		if(num)
		{
			for(int j=0; j<num; j++)
			{
				BYTE entityType = _GetByteFromByteArray(msgBuf, msgBufPos, bufSize);

				char comFlag[64];
				memset(comFlag, 0, sizeof(comFlag));
				_GetStringFromByteArray(msgBuf, msgBufPos, comFlag, bufSize);
				CGUID guid;
				_GetBufferFromByteArray(msgBuf, msgBufPos, guid, bufSize);

				BYTE DbOperFlag = _GetByteFromByteArray(msgBuf, msgBufPos, bufSize);
				BYTE DBOperType = _GetByteFromByteArray(msgBuf, msgBufPos, bufSize);

				if(!isLoadOperFlag) // 不是读取操作
				{
					if(DbOperFlag != 1) continue;
					switch(DBOperType)
					{
					case DB_OPER_DELETE_INSERT:
						{
							CBaseEntity* pEntity = GetChild(guid);
							if(pEntity) 
							{
								if(pEntity->GetCompositeType() == COM_COMPOSITE)
								{
									if(!((CEntityGroup*)pEntity)->GetHasDetailLeavesFlag())// 非分层group(GoodsGroup)
									{
										ReleaseLeaf(guid);
										pEntity = GetGame()->GetEntityManager()->NewBaseEntity(comFlag, guid);
										AddChild(pEntity);
									}
								}
							}
							else
							{
								pEntity = GetGame()->GetEntityManager()->NewBaseEntity(comFlag, guid);
								AddChild(pEntity);
							}

							if(pEntity)
							{
								pEntity->SetCurDbOperType(DBOperType);
								pEntity->SetCurDbOperFlag(DbOperFlag);
								if(DbOperFlag == 1)
								{
									pEntity->GetFromByteArray(isLoadOperFlag, msgBuf, msgBufPos, bufSize);
								}
							}
						}
						break;
					case DB_OPER_DELETE:
						{
							ReleaseLeaf(guid);
						}
						break;
					case DB_OPER_INSERT:
					case DB_OPER_SAVE:
						{
							CBaseEntity* pLeaf = GetChild(guid);
							if(!pLeaf)
							{
								pLeaf = GetGame()->GetEntityManager()->NewBaseEntity(comFlag, guid);
								AddChild(pLeaf);
							}
							if(pLeaf)
							{
								pLeaf->SetCurDbOperType(DBOperType);
								pLeaf->SetCurDbOperFlag(DbOperFlag);
								if(DbOperFlag == 1)
									pLeaf->GetFromByteArray(isLoadOperFlag, msgBuf, msgBufPos, bufSize);
							}
						}
						break;
					default:
						{	
							CBaseEntity* pEntity = GetGame()->GetEntityManager()->NewBaseEntity(comFlag, guid);
							if(pEntity)
							{
								pEntity->SetCurDbOperType(DBOperType);
								pEntity->SetCurDbOperFlag(DbOperFlag);
								if(DbOperFlag == 1)
									pEntity->GetFromByteArray(isLoadOperFlag, msgBuf, msgBufPos, bufSize);
							}
							GetGame()->GetEntityManager()->DelBaseEntity(pEntity);
							AddLogText("GetFromByteArray(): 根节点%s 子结点%s 的操作类型%d 无效!", 
								GetCompositeFlag().c_str(), comFlag, DBOperType);
						}
						break;
					}
				}
				else
				{
					if(DBOperType == DB_OPER_DELETE)
					{
						ReleaseLeaf(guid);
					}
					else
					{
						CBaseEntity* pLeaf = GetChild(guid);
						if(!pLeaf)
						{
							pLeaf = GetGame()->GetEntityManager()->NewBaseEntity(comFlag, guid);
							AddChild(pLeaf);
						}
						if(pLeaf)
						{
							pLeaf->SetCurDbOperType(DB_OPER_LOAD);
							pLeaf->SetCurDbOperFlag(1);
							pLeaf->GetFromByteArray(isLoadOperFlag, msgBuf, msgBufPos, bufSize);
						}
					}
				}
			}
		}
	}

	void CEntityGroup::ReleaseLeaf(const CGUID& guid)
	{
		std::map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.find(guid);
		if( itr != m_mapEntityGroup.end() )
		{
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
			itr = m_mapEntityGroup.erase(itr);
		}
	}
	// 清除某个叶子的叶子,不包括叶子
	void CEntityGroup::ClearLeafChilds(const CGUID& guid)
	{
		std::map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.find(guid);
		if( itr != m_mapEntityGroup.end() )
		{
			if(itr->second->GetCompositeType() == COM_COMPOSITE)
				((CEntityGroup*)itr->second)->ReleaseChilds();
		}
	}

	void CEntityGroup::ReleaseChilds()
	{
		std::map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.begin();
		while( itr != m_mapEntityGroup.end() )
		{
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
			itr = m_mapEntityGroup.erase(itr);
		}
		m_mapEntityGroup.clear();
	}
	// 初始化属性配置
	void CEntityGroup::InitProperty()
	{
		CEntityManager::AttrDefMap& mapAttrDef = GetGame()->GetEntityManager()->GetObjAttrDef();
		CEntityManager::AttrDefMapItr itr = mapAttrDef.find(GetCompositeFlag());
		if(itr != mapAttrDef.end())
		{
			// 初始化属性变量
			tagEntityBuildInfo* pInfo = itr->second;
			if(!pInfo) return;
			
			map<string, tagEntityAttrInfo>::iterator daEpItr = itr->second->pDataMgr.begin();
			for(; daEpItr != itr->second->pDataMgr.end(); daEpItr++)
			{
				m_DataEntityManager.CreateEntityProperty(daEpItr->second.lAttrTypeEnum);
			}

			// 初始化数据
			m_lLeafComFlag = pInfo->strLeafComFlag;
			m_lLeafComType = pInfo->lLeafComType;
			m_bHasDetailLeavesFlag = pInfo->lHasDetailLeaves;

			m_CompositeType = (COMPOSITE_TYPE)pInfo->lComType;
			
			m_strDetailComPositeFlagName = pInfo->strDbQueryName;
			m_lDetailComPositeFlagType = pInfo->lDbQueryType;

			if(m_lDetailComPositeFlagType != -1) // 有分散类型
			{
				long lQueryAttrType = GetGame()->GetEntityManager()->GetAttrEnumByStr(pInfo->strComFlag, pInfo->strDbQueryName);

				CEntityProperty* pEP = m_DataEntityManager.CreateEntityProperty(lQueryAttrType);
				if(pEP)
				{
					switch((DATA_OBJECT_TYPE)pInfo->lDbQueryType)
					{
					case DATA_OBJECT_TIME:
						{
							pEP->SetBufAttr(((void*)&pInfo->pQueryVar[0]), 6*sizeof(DWORD));
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							pEP->SetStringAttr(string((char*)&pInfo->pQueryVar[0]));
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							pEP->SetBufAttr(((void*)&pInfo->pQueryVar[0]), 6*sizeof(DWORD));
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID bguid;
							memcpy(&bguid, pInfo->pQueryVar, sizeof(CGUID));
							pEP->SetGuidAttr(bguid);
						}
						break;
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_DWORD:
						{
							long lVar = *((long*)&pInfo->pQueryVar[0]);
							pEP->SetLongAttr(lVar);
						}
						break;
					}
				}
			}

			if(m_CompositeFlag == "[account]" & m_lLeafComFlag != "" && pInfo->mapBuildFileInfo.size() == 0)
			{
				AddLogText("InitProperty() entity[%s] leaf[%s] can't init!", m_CompositeFlag.c_str(), m_lLeafComFlag.c_str());
			}

			// 初始化结构
			map<string, tagBuildFileInfo*>::iterator leafItr = pInfo->mapBuildFileInfo.begin();
			for(; leafItr != pInfo->mapBuildFileInfo.end(); leafItr++)
			{
				tagBuildFileInfo* leafInfo = leafItr->second;
				CEntityGroup* pLeaf = static_cast<CEntityGroup*>(GetGame()->GetEntityManager()->NewBaseEntity(leafInfo->strLeafComFlag, leafInfo->leafGuid));
				if(pLeaf)
				{
					AddChild(pLeaf);
					pLeaf->InitProperty();
				}
			}
		}
	}
	
	void CEntityGroup::AddChild(CBaseEntity* entity)
	{
		if(entity)
		{
			map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.find(entity->GetGUID());
			if(itr != m_mapEntityGroup.end())
			{
				GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
				m_mapEntityGroup.erase(itr);
			}
			m_mapEntityGroup[entity->GetGUID()] = entity;
			m_mapGuidByComFlag[entity->GetCompositeFlag()] = entity->GetGUID();
		}
	}
	void CEntityGroup::RemoveChild(const CGUID& guid)
	{
		map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.find(guid);
		if(itr != m_mapEntityGroup.end())
		{
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
			itr = m_mapEntityGroup.erase(itr);
		}
	}
	CBaseEntity* CEntityGroup::GetChild(const CGUID& guid)
	{
		CBaseEntity*retEntity = NULL;
		map<CGUID, CBaseEntity*>::iterator itr = m_mapEntityGroup.find(guid);
		if(itr != m_mapEntityGroup.end())
		{
			retEntity = itr->second;
		}
		return retEntity;
	}

	// 遍历对象树查找指定节点
	CBaseEntity* CEntityGroup::FindEntityBytravelTree(const CGUID& desGuid)
	{
		std::map<CGUID, CBaseEntity*>::iterator itr = GetEntityGroupMap().find(desGuid);
		if(itr != GetEntityGroupMap().end())
		{
			return itr->second;
		}
		else
		{
			for(itr = GetEntityGroupMap().begin(); itr != GetEntityGroupMap().end(); itr++)
			{
				if(itr->second->GetCompositeType() == COM_COMPOSITE)
				{
					CBaseEntity* pRet = ((CEntityGroup*)itr->second)->FindEntityBytravelTree(desGuid);
					if(pRet)
						return pRet;
				}
			}
		}
		return NULL;
	}