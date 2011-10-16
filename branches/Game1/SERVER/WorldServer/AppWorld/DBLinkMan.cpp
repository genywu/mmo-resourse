#include "StdAfx.h"
#include "DBLinkMan.h"
#include "DBentityManager.h"
#include "../worldServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBLinkMan::CDBLinkMan()
: CBaseDBEntity(),m_PlayerGuid(NULL_GUID)
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
	// 初始化字段数据
	InitProperty();
}

CDBLinkMan::CDBLinkMan(const CGUID& id)
: CBaseDBEntity(id),b_IsFirstCreated(true),m_PlayerGuid(NULL_GUID)
{
	// 初始化字段数据
	InitProperty();
}

CDBLinkMan::~CDBLinkMan()
{
}

// virtual interfaces
void CDBLinkMan::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_LOAD:
		{
			Load(msgid, msgBuf, msgBufPos, bufSize);
		}
		break;
	}
}

void CDBLinkMan::Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
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
			std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(string("player"));
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

void CDBLinkMan::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
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
void CDBLinkMan::InitProperty()
{
	CDataEntityManager* pdeManager = NULL;
	std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(string("player"));
	if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
		pdeManager = itr->second;

	if(pdeManager)
	{
		CDataEntityManager::EntityPropertyMapItr attrItr = pdeManager->GetEntityPropertyMap().begin();

		for(; attrItr != pdeManager->GetEntityPropertyMap().end(); attrItr++)
		{
			if(attrItr->second->GetDBTableName() == "FriendsMember")
			{
				CEntityProperty* pe = m_DataEntityManager.CreateEntityProperty(attrItr->first, attrItr->second->GetDBTableName(), attrItr->second->GetDataType());
				if(pe)
				{
					if(pe->GetDataType() == DATA_OBJECT_TIME)
					{
						long timeBuf[6] = {0};
						timeBuf[0] = 1990;
						timeBuf[1] = 1;
						timeBuf[2] = 1;
						pe->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
					}
				}
			}
		}
	}
}

// 检查更新属性里是否有指定表名的属性名
bool CDBLinkMan::FindTableUpdateFlag(const string& tableName)
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