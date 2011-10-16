/**
*File:  Entity.cpp
*brief: 对象类:该对象只有其自己的属性数据,在结构树中成为叶子节点
        游戏物体基类，所有对象和游戏相关的类由该类派生
*Author:安海川
*Date:  2008.11.03
*/
#include "StdAfx.h"
#include "Entity.h"

#include "EntityGroup.h"
#include "EntityManager.h"

#include "../Thread/DBThread.h"
#include "../Thread/Threaddatadef.h"

#pragma warning(push)
#pragma warning(disable :  4244)

//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

CEntity::CEntity(const string& flag, const CGUID& id)
	:	CBaseEntity(id)
{
	CBaseEntity::m_CompositeType = COM_LEAF;
	CBaseEntity::m_CompositeFlag = flag;
	InitProperty();
}

CEntity::~CEntity(void)
{
}

CEntity& CEntity::operator=(CEntity& right)
{
	CBaseEntity::operator=(right);
	return *this;
}

// 初始化属性配置
void CEntity::InitProperty()
{
	CEntityManager::AttrDefMap& mapAttrDef = GetGame()->GetEntityManager()->GetObjAttrDef();
	CEntityManager::AttrDefMapItr itr = mapAttrDef.find(GetCompositeFlag());
	if(itr != mapAttrDef.end())
	{
		// 初始化属性变量
		tagEntityBuildInfo* pInfo = itr->second;
		if(!pInfo) return;
		//m_DataEntityManager = *itr->second->pDataMgr;
		map<string, tagEntityAttrInfo>::iterator daEpItr = itr->second->pDataMgr.begin();
		for(; daEpItr != itr->second->pDataMgr.end(); daEpItr++)
		{
			m_DataEntityManager.CreateEntityProperty(daEpItr->second.lAttrTypeEnum);
		}

		// 初始化数据
		m_CompositeType = (COMPOSITE_TYPE)pInfo->lComType;
		m_bHasAttrs = pInfo->lHhasAttrFlag;
	}
}


void CEntity::GetFromByteArray(BYTE* msgBuf, long& msgBufPos, long bufSize)
{
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
					BYTE* buf = new BYTE[varBufLen];
					_GetBufferFromByteArray(msgBuf, msgBufPos, buf, varBufLen, bufSize);//变量数据
					ep->SetBufAttr(buf, varBufLen);
					SAFE_DELETE_ARRAY(buf);
				}
				break;
			}
		}
	}
}

void CEntity::AddToByteArray(std::vector<BYTE>& pBA, bool isLoadOperFlag)
{
	_AddToByteArray(&pBA, (BYTE)GetCompositeType());
	_AddToByteArray(&pBA, GetCompositeFlag().c_str());
	_AddToByteArray(&pBA, m_GUID); // 实体ID

	_AddToByteArray(&pBA, (BYTE)GetCurDbOperFlag());
	_AddToByteArray(&pBA, (BYTE)GetCurDbOperType());

	if(   (GetCurDbOperFlag() == false && !isLoadOperFlag )  // 不是读操作并且操作标志为0
		|| (isLoadOperFlag && GetCurDbOperType() == DB_OPER_DELETE)   ) return; // 是读操作并且该节点操作标志为DELETE
	
	// 属性值
	_AddToByteArray(&pBA, (WORD)GetDataEntityManager().GetEntityPropertyMap().size()); // 属性种类个数
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
				itr->second->GetBufAttr( timeValue, 6*sizeof(DWORD));
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
				itr->second->GetGuidAttr( tGUID);
				_AddToByteArray(&pBA, tGUID);//变量数据
			}
			break;
		case DATA_OBJECT_BUFFER:
			{
				long bufSize = itr->second->GetBufSize();
				if(bufSize)
				{
					BYTE* buf = new BYTE[bufSize];
					itr->second->GetBufAttr(buf, bufSize);
					_AddToByteArray(&pBA, buf, bufSize);//变量数据
					SAFE_DELETE_ARRAY(buf);
				}
			}
			break;
		}

	}
}

#pragma warning(pop)
