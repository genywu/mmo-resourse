#include "StdAfx.h"
#include "..\..\..\dbaccess\worlddb\RSDupRgn.h"
#include "DBentityManager.h"
#include "DBDupRgn.h"
#include "../DBServer/game.h"
#include "DBAccessThread.h"
#include "DBSaveThread.h"
#include "DBLoadThread.h"
#include "ThreadDataDef.h"
#include "DBRgnGoodsGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBDupRgn::CDBDupRgn(const CGUID& guid)
: CBaseDBEntity(guid),m_GetGoodsGroupPtr(NULL)
{
	// 初始化字段数据
	InitProperty();
	m_GetGoodsGroupPtr = new CDBRgnGoodsGroup(NULL_GUID);
	m_GetGoodsGroupPtr->SetOwnerGUID(guid);
}

CDBDupRgn::~CDBDupRgn()
{
	SAFE_DELETE(m_GetGoodsGroupPtr);
}

// virtual interfaces
void CDBDupRgn::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			// 组装消息
			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
			// DBOP Type:     1byte, 数据库操作类型（枚举值）
			// MSG ID:        4byte, 该消息的MSGID
			// Entity Num:	2byte, 该实体个数(X)

			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_PHDUPRGN);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_PHDUPRGN;
			saveData->saveObj->objNum  = 1;
			*(CDBDupRgn*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
			if(b_IsFirstCreated)
			{
				LoadThreadData* loadData = new LoadThreadData(GetExID(), DB_PHDUPRGN);
				loadData->loadObj->objGUID = GetExID();
				loadData->loadObj->objType = DB_PHDUPRGN;
				loadData->loadObj->objNum  = 1;
				*(CDBDupRgn*)(loadData->dbObj) = *this;
				GetGame()->GetDBEntityManager()->PushLoadThreadData(loadData);
			}
			else //不是刚创建的对象,直接返回内存数据
			{
				// 组装消息
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				// opType		  4byte, 操作对象类型
				// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
				// DBOP Type:     1byte, 数据库操作类型（枚举值）
				// MSG ID:        4byte, 该消息的MSGID
				// Entity Num:	2byte, 该实体个数(X)
				msg.Add((long)DB_PHDUPRGN);
				msg.Add((BYTE)0);
				msg.Add((BYTE)DB_OPER_LOAD);
				msg.Add(msgid);
				msg.Add((WORD)1);
				vector<BYTE> pBA;
				AddEntityDataToMsg(DB_OPER_LOAD, pBA);
				msg.Add((long)pBA.size());
				msg.AddEx(&pBA[0], pBA.size());
				msg.SendToSocket(GetGame()->GetWSSocket());
			}
		}
		break;
	}
}

void CDBDupRgn::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	m_mapDirtyAttrData.clear();

	// 解析消息
	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 属性种类个数

	char varName[1024];
	for(int i=0; i<(int)attrNum; i++)
	{
		//--新格式
		//变量名字符串长度（long）
		//变量名字符串
		//变量类型（long）
		//变量数据长度（long）
		//变量数据
		long varNameLen = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量名字符串长度（long）
		_GetStringFromByteArray(msgBuf, msgBufPos, varName, bufSize);//变量名字符串
		m_mapDirtyAttrData.insert(string(varName));
		long bufType = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量类型（long）
		long varBufLen = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);//变量数据长度（long）

		CEntityProperty* ep = NULL;
		CDataEntityManager::EntityPropertyMapItr itr = GetDataEntityManager().GetEntityPropertyMap().find(varName);
		if(itr != GetDataEntityManager().GetEntityPropertyMap().end())
			ep = itr->second;
		else
		{
			CDataEntityManager* pdeManager = NULL;
			std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(string("duprgn"));
			if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
				pdeManager = itr->second;

			if(pdeManager)
			{
				CEntityProperty* templateEP = pdeManager->GetEntityProperty(varName);
				if(templateEP)
					ep = GetDataEntityManager().CreateEntityProperty(varName, templateEP->GetDBTableName(), templateEP->GetDataType());
				else
					AddLogText("根据属性名[%s]查找属性指针为空!", varName);
			}
		}

		assert(ep != NULL);

		switch((DATA_OBJECT_TYPE)bufType)
		{
		case DATA_OBJECT_TIME:
			{
				// 6个long值 : 年 月 日 时 分 秒
				BYTE timeValue[6*sizeof(DWORD)];
				memset(timeValue, 0, 6*sizeof(DWORD));
				_GetBufferFromByteArray(msgBuf, msgBufPos, timeValue, 6*sizeof(DWORD), bufSize);
				ep->SetBufAttr(0, timeValue, 6*sizeof(DWORD));
			}
			break;
		case DATA_OBJECT_STRING:
			{
				char varValue[1024];
				_GetStringFromByteArray(msgBuf, msgBufPos, varValue, bufSize);//变量数据
				ep->SetStringAttr(0, string((char*)varValue));
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
				ep->SetLongAttr(0, (long)value);
			}
			break;
		case DATA_OBJECT_GUID:
			{
				CGUID tGUID;
				_GetBufferFromByteArray(msgBuf, msgBufPos, tGUID, bufSize);//变量数据
				ep->SetGuidAttr(0, tGUID);
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				BYTE* buf = new BYTE[varBufLen];
				_GetBufferFromByteArray(msgBuf, msgBufPos, buf, varBufLen, bufSize);//变量数据
				ep->SetBufAttr(0, buf, varBufLen);
				SAFE_DELETE_ARRAY(buf);
			}
			break;
		}
	}
}

void CDBDupRgn::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
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
		_AddToByteArray(&pBA, (long)itr->first.size());//变量名字符串长度（long）
		_AddToByteArray(&pBA, itr->first.c_str());//变量名字符串
		_AddToByteArray(&pBA, (long)itr->second->GetDataType());//变量类型（long）
		_AddToByteArray(&pBA, (long)itr->second->GetBufSize(0));//变量数据长度（long）
		switch(itr->second->GetDataType())
		{
		case DATA_OBJECT_TIME:
			{
				// 6个long值 : 年 月 日 时 分 秒
				BYTE timeValue[6*sizeof(DWORD)];
				memset(timeValue, 0, 6*sizeof(DWORD));
				itr->second->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
				_AddToByteArray(&pBA, timeValue, 6*sizeof(DWORD));
			}
			break;
		case DATA_OBJECT_STRING:
			{
				_AddToByteArray(&pBA, itr->second->GetStringAttr(0));//变量数据
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
				_AddToByteArray(&pBA, (long)itr->second->GetLongAttr(0));//变量数据
			}
			break;
		case DATA_OBJECT_GUID:
			{
				CGUID tGUID;
				itr->second->GetGuidAttr(0, tGUID);
				_AddToByteArray(&pBA, tGUID);//变量数据
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				long bufSize = itr->second->GetBufSize(0);
				if(bufSize)
				{
					BYTE* buf = new BYTE[bufSize];
					itr->second->GetBufAttr(0, (void*)buf, bufSize);
					_AddToByteArray(&pBA, buf, bufSize);//变量数据
					SAFE_DELETE_ARRAY(buf);
				}
			}
			break;
		}

	}
}

// 初始化属性配置
void CDBDupRgn::InitProperty()
{
	CDataEntityManager* pdeManager = NULL;
	std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(string("duprgn"));
	if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
		pdeManager = itr->second;

	if(pdeManager)
	{
		CDataEntityManager::EntityPropertyMapItr attrItr = pdeManager->GetEntityPropertyMap().begin();

		for(; attrItr != pdeManager->GetEntityPropertyMap().end(); attrItr++)
		{
			CEntityProperty* ep = m_DataEntityManager.CreateEntityProperty(attrItr->first, attrItr->second->GetDBTableName(), attrItr->second->GetDataType());
			if(ep)
			{
				ep->SetDbFlag(attrItr->second->GetDbFlag());
				if(ep->GetDataType() == DATA_OBJECT_TIME)
				{
					long timeBuf[6] = {0};
					timeBuf[0] = 1990;
					timeBuf[1] = 1;
					timeBuf[2] = 1;
					ep->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
				}
			}
		}
	}
}

// 检查更新属性里是否有指定表名的属性名
bool CDBDupRgn::FindTableUpdateFlag(const string& tableName)
{
	set<string>::iterator itr = m_mapDirtyAttrData.begin();
	for (; itr != m_mapDirtyAttrData.end(); itr++)
	{
		CEntityProperty* ep = GetDataEntityManager().GetEntityProperty(*itr);
		if( ep )
		{
			if(ep->GetDBTableName() == tableName)
				return true;
		}
	}
	return false;
}