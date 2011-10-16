#include "StdAfx.h"
#include "DBRegion.h"
#include "..\..\..\dbaccess\worlddb\RSRegion.h"
#include "DBEntityManager.h"
#include "../DBServer/game.h"
#include "ThreadDataDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDBRegion::CDBRegion()
: CBaseDBEntity()
{
	CGUID::CreateGUID(m_GUID);
	m_bLoadedFromDB=false;
}

CDBRegion::CDBRegion(const CGUID& id)
:CBaseDBEntity(id)
{
	m_bLoadedFromDB=false;
}

CDBRegion::~CDBRegion()
{
}

// virtual interfaces
void CDBRegion::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
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
			
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_REGION);
			// 需要记录一个时间戳,在存盘线程做
			
			SaveForThread(msgid, msgBuf, msgBufPos, saveData->saveObj);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_REGION;
			saveData->saveObj->objNum  = 1;
			*(CDBRegion*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
			// 组装消息
			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
			// DBOP Type:     1byte, 数据库操作类型（枚举值）
			// MSG ID:        4byte, 该消息的MSGID
			// Entity Num:	2byte, 该实体个数(X)
			if(b_IsFirstCreated) // 是刚创建的对象,到数据库读取
			{
				LoadThreadData* loadData = new LoadThreadData(GetExID(), DB_REGION);
				long lRegionId=_GetLongFromByteArray(msgBuf,msgBufPos);
				SetID(lRegionId);
				loadData->loadObj->objGUID = GetExID();
				loadData->loadObj->objType = DB_REGION;
				loadData->loadObj->objNum  = 1;				
				*(CDBRegion*)(loadData->dbObj) = *this;
				GetGame()->GetDBEntityManager()->PushLoadThreadData(loadData);
			}
			else //不是刚创建的对象,直接返回内存数据
			{
				// 组装消息				
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
				// DBOP Type:     1byte, 数据库操作类型（枚举值）
				// MSG ID:        4byte, 该消息的MSGID
				// Entity Num:	2byte, 该实体个数(X)
				msg.Add((long)DB_REGION);
				msg.Add((BYTE)0);
				msg.Add((BYTE)DB_OPER_LOAD);
				msg.Add(msgid);
				msg.Add((WORD)1);
				vector<BYTE> pBA;
				_AddToByteArray(&pBA,m_GUID);
				_AddToByteArray(&pBA,(BYTE*)(&GetRegionParam()),sizeof(tagRegionParam));
				msg.Add((long)pBA.size());
				msg.AddEx(&pBA[0], pBA.size());				
				msg.SendToSocket(GetGame()->GetWSSocket());
			}
		}
		break;
	}
}

void CDBRegion::SaveForThread(long msgid, BYTE* msgBuf, long& msgBufPos, SaveObj* saveData)
{
	tagRegionParam stParam;
	_GetBufferFromByteArray(msgBuf,msgBufPos,&stParam,sizeof(tagRegionParam));
	SetRegionParam(stParam);
}