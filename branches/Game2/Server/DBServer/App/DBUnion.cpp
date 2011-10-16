#include "StdAfx.h"
#include "DBUnion.h"
#include "..\..\..\dbaccess\worlddb\RSUnion.h"
#include "DBEntityManager.h"
#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBUnion::CDBUnion()
: CBaseDBEntity()
{
	//GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBUnion::CDBUnion(const CGUID& id)
: CBaseDBEntity(id)
{

}
CDBUnion::~CDBUnion()
{
//	GetGame()->GetDBEntityManager()->FreeDBUnionID(m_ID);
}

// virtual interfaces
void CDBUnion::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
//	switch(op)
//	{
//	case DB_OPER_CREATE:
//		Create(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_DELETE:
//		Delete(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_SAVE:
//		{
//			// 组装消息
//			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
//			// DBOP Type:     1byte, 数据库操作类型（枚举值）
//			// MSG ID:        4byte, 该消息的MSGID
//			// Entity Num:	2byte, 该实体个数(X)
//			Save(msgid, msgBuf, msgBufPos);
////			g_RsUnion.SaveConfederation(this);
//		}
//		break;
//	case DB_OPER_LOAD:
//		{
//			// 组装消息
//			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
//			// DBOP Type:     1byte, 数据库操作类型（枚举值）
//			// MSG ID:        4byte, 该消息的MSGID
//			// Entity Num:	2byte, 该实体个数(X)
//			GetGame()->GetRsConfederation()->LoadConfederation(this);
//			vector<BYTE> pBA;
//			Load(msgid, msgBuf, msgBufPos, pBA);
//		}
//		break;
//	case DB_OPER_UPDATE:
//		//g_RSCards.SaveLargess(this);
//		Update(msgid, msgBuf, msgBufPos);
//		break;
//	}
}
//void CDBUnion::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//
//}
//void CDBUnion::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	
//}
//void CDBUnion::Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA)
//{
//	// initiate
//	//Initiate();
//
//	CDBEntityManager::DBUnionMapItr itr = GetGame()->GetDBEntityManager()->DBUnionMapBegin();
//	for(; itr != GetGame()->GetDBEntityManager()->DBUnionMapEnd(); itr++)
//	{
//		_AddToByteArray(&pBA, m_GUID); // 实体ID
//		_AddToByteArray(&pBA, (WORD)5/*CDBCityWar的所有属性种类个数*/); // 发送的CDBCityWar的所有属性种类个数
//
//		// add one Faction data
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_UNION_ATTR_LONG_ID);
//		_AddToByteArray(&pBA, GetExID());
//
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_UNION_ATTR_SZ_NAME);
//		_AddToByteArray(&pBA, (char*)GetName().c_str());
//				
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_UNION_ATTR_LONG_MASTER_ID);
//		_AddToByteArray(&pBA, GetMasterID());
//		
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_UNION_ATTR_BUF_PROPERTY);
//		GetPropertyData(pBA);
//			
//		_AddToByteArray(&pBA, (WORD)m_Members.size());
//		_AddToByteArray(&pBA, (long)DB_UNION_ATTR_BUF_MEMBERS);
//		GetMembersData(pBA);	
//	}
//
//}
//
//void CDBUnion::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
//	//	Initiate();
//
//	// MSGID
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	// 解析消息
//	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//	for(WORD i = 0; i < attrNum; i++)
//	{
//		WORD num = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
//		DB_UNION_ATTR_TYPE type = (DB_UNION_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值
//
//		long pos = 0; // 数据块读取指针
//		switch(type)
//		{
//		case DB_UNION_ATTR_LONG_ID:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetExID(tGuid);
//			}
//			break;
//		case DB_UNION_ATTR_SZ_NAME:
//			{
//				char name[128];
//				_GetStringFromByteArray(msgBuf, msgBufPos, name);
//				SetName(string(name));
//			}
//			break;
//		case DB_UNION_ATTR_LONG_MASTER_ID:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetMasterID(tGuid);
//			}
//			break;
//
//			break;
//		case DB_UNION_ATTR_BUF_PROPERTY:
//			{
//				tagFacBaseProperty& bp = GetProperty();
//				_GetBufferFromByteArray(msgBuf, msgBufPos, &bp, sizeof(tagFacBaseProperty));
//			}
//			break;
//		case DB_UNION_ATTR_BUF_MEMBERS:
//			{
//				map<CGUID,tagMemInfo>& mi = GetMembers();
//				mi.clear();
//
//				tagMemInfo tmi;
//				for(int i=0; i<num; i++)
//				{
//					_GetBufferFromByteArray(msgBuf, msgBufPos, &tmi, sizeof(tagMemInfo));
//					mi[tmi.MemberGuid] = tmi;
//				}
//			}
//			break;
//		}
//	}
//}
//void CDBUnion::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	
//}
//
////得到属性数据
//bool CDBUnion::GetPropertyData(vector<BYTE>& ByteArray)
//{
//	_AddToByteArray(&ByteArray,&m_Property,sizeof(tagFacBaseProperty));
//	return true;
//}
////得到成员信息数据
//bool CDBUnion::GetMembersData(vector<BYTE>& ByteArray)
//{
//	map<CGUID,tagMemInfo>::iterator it = m_Members.begin();
//	for(;it!=m_Members.end();it++)
//	{
//		_AddToByteArray(&ByteArray,&(*it).second,sizeof(tagMemInfo) );
//	}
//	return true;
//}