#include "StdAfx.h"
#include "DBUnion.h"
#include "DBentityManager.h"
#include "organizingsystem/union.h"
#include "organizingsystem/OrganizingCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBUnion::CDBUnion()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
	for(int pos=(int)DB_UNION_ATTR_LONG_ID;  pos <= (int)DB_UNION_ATTR_BUF_MEMBERS; pos++)
	{
		m_SendAttrEnumVector.push_back(1);
	}
}

CDBUnion::CDBUnion(const CGUID& guid)
: CBaseDBEntity(guid)
{
	for(int pos=(int)DB_UNION_ATTR_LONG_ID;  pos <= (int)DB_UNION_ATTR_BUF_MEMBERS; pos++)
	{
		m_SendAttrEnumVector.push_back(1);
	}
}
CDBUnion::~CDBUnion()
{
	//GetGame()->GetDBEntityManager()->FreeDBUnionID(m_ID);
}

//void CDBUnion::OpenAttrFlagInVector(DB_UNION_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_UNION_ATTR_LONG_ID && attrType <= DB_UNION_ATTR_BUF_MEMBERS)
//	{
//		long pos = DB_UNION_ATTR_BUF_MEMBERS - attrType;
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//// 关闭准备发送的属性在Vector中的相应位置的标志
//void CDBUnion::CloseAttrFlagInVector(DB_UNION_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_UNION_ATTR_LONG_ID && attrType <= DB_UNION_ATTR_BUF_MEMBERS)
//	{
//		long pos = DB_UNION_ATTR_BUF_MEMBERS - attrType;
//		m_SendAttrEnumVector[pos] = 0;
//	}
//}

// virtual interfaces
void CDBUnion::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	////CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
	//switch(op)
	//{
	//case DB_OPER_CREATE:
	//	Create(msgid, msgBuf, msgBufPos);
	//	break;
	//case DB_OPER_DELETE:
	//	Delete(msgid, msgBuf, msgBufPos);
	//	break;
	//case DB_OPER_SAVE:
	//	Save(msgid, msgBuf, msgBufPos);
	//	break;
	//case DB_OPER_LOAD:
	//	Load(msgid, msgBuf, msgBufPos);
	//	break;
	//case DB_OPER_UPDATE:
	//	Update(msgid, msgBuf, msgBufPos);
	//	break;
	//}
}
//
//void CDBUnion::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	//AddLogText("DB2W[%d] : DB_OPER_CREATE_UNION, UnionID[%d]...OK!", msgid, GetExID());
//}
//void CDBUnion::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// flag
//	BYTE flag = _GetByteFromByteArray(msgBuf, msgBufPos);
//	//if(flag == 1)
//		//AddLogText("DB2W[%d] : DB_OPER_DELETE_UNION, UnionID[%d]...OK!", msgid, GetExID());
//	//else
//		//AddLogText("DB2W[%d] : DB_OPER_DELETE_UNION, UnionID[%d]...FAILED!", msgid, GetExID());
//}
//void CDBUnion::Load(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
//	//	Initiate();
//
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos); // MSGID
//
//	//删除定时器
////	GetGame()->DelMsgTimer(msgid);
//
//	//	g_RSCityWar.LoadAllCards();
//
//	// 解析消息
//	CDBEntityManager::DBUnionMapItr itr = GetGame()->GetDBEntityManager()->GetDBUnionMap().find(GetExID());
//
//	if(itr != GetGame()->GetDBEntityManager()->DBUnionMapEnd())
//	{
//		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//
//		for(int i=0; i<(int)attrNum; i++)
//		{
//			// add one card data
//			WORD num = _GetWordFromByteArray(msgBuf, msgBufPos);
//			DB_UNION_ATTR_TYPE attrType = (DB_UNION_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);
//
//			long pos = 0; // 数据块读取指针
//			switch(attrType)
//			{
//			case DB_UNION_ATTR_LONG_ID:
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					SetExID(tGuid);
//				}
//				break;
//			case DB_UNION_ATTR_SZ_NAME:
//				{
//					char name[128];
//					_GetStringFromByteArray(msgBuf, msgBufPos, name);
//					SetName(string(name));
//				}
//				break;
//			case DB_UNION_ATTR_LONG_MASTER_ID:
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					SetMasterID(tGuid);
//				}
//				break;
//		
//				break;
//			case DB_UNION_ATTR_BUF_PROPERTY:
//				{
//					tagFacBaseProperty& bp = GetProperty();
//					_GetBufferFromByteArray(msgBuf, msgBufPos, &bp, sizeof(tagFacBaseProperty));
//				}
//				break;
//			case DB_UNION_ATTR_BUF_MEMBERS:
//				{
//					map<CGUID,tagMemInfo>& mi = GetMembers();
//					mi.clear();
//
//					tagMemInfo tmi;
//					for(int i=0; i<num; i++)
//					{
//						_GetBufferFromByteArray(msgBuf, msgBufPos, &tmi, sizeof(tagMemInfo));
//						mi[tmi.MemberGuid] = tmi;
//					}
//				}
//				break;
//			}
//		}
//
//		CUnion* pUnion = new CUnion(GetExID(), GetMasterID(), GetName());
//
//		GetGame()->GetDBEntityManager()->CDBUnionToCUnion(this, pUnion);
//
//		GetOrganiCtrl()->AddConfederationOrganizing(GetExID(), pUnion);
//		pUnion->SetChangeData(Org_NoDataChange);
//
//		//AddLogText("DB2W[%d] : DB_OPER_LOAD_UNION, UnionID[%d]...OK!", msgid, GetExID());
//
//		return;
//	}
//
//	//AddLogText("DB2W[%d] : DB_OPER_LOAD_UNION, UnionID[%d]...FAILED!", msgid, GetExID());
//}
//
//void CDBUnion::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	//AddLogText("DB2W[%d] : DB_OPER_LOAD_UNION, UnionID[%d]...OK!", msgid, GetExID());
//}
//void CDBUnion::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	//AddLogText("DB2W[%d] : DB_OPER_LOAD_UNION, UnionID[%d]...OK!", msgid, GetExID());
//}
///*
//void CDBCityWar::Initiate(void)
//{
//m_lSellerID			= 0;
//m_lBuyerID			= 0;
//m_dwPrice			= 0;
//m_dwChargePrice		= 0;
//m_dwCardState		= 0;
//m_dwCardTime		= 0;
//m_dwStartTime		= 0;
//m_dwSerialNumber	= 0;
//memset(m_CardNumber, 0, sizeof(m_CardNumber));
//memset(m_CardType,   0, sizeof(m_CardType));
//memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
//memset(m_BuyerAccount,  0, sizeof(m_BuyerAccount));
//}
//*/
//// 组装消息
//void CDBUnion::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
//{
//	// 传递来的消息前面已经加入了
//	// OPEntityID:    4byte, 操作实体的ID值
//	// DBOP Type:     1byte, 数据库操作类型（枚举值）
//	// MSG ID:        4byte, 该消息的MSGID
//	// Entity Num:    2byte, 该实体个数
//	if(DB_OPER_DELETE == opType) // DELETE操作
//	{
//		return;
//	}
//
//	WORD attrNum = 0;
//	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
//	{
//		if(m_SendAttrEnumVector[i] != 0)
//			attrNum++;
//	}
//	_AddToByteArray(&pBA, m_GUID); // 实体ID
//	_AddToByteArray(&pBA, attrNum); // 属性种类个数
//
//	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
//	{
//		if(m_SendAttrEnumVector[i] != 0 
//			&& ( DB_OPER_SAVE == opType || DB_OPER_UPDATE == opType || DB_OPER_LOAD == opType) )// 标志打开并且为SAVE/UPDATE/CREATE操作
//		{
//			switch(DB_UNION_ATTR_LONG_ID+i)
//			{
//			case DB_UNION_ATTR_LONG_ID:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_UNION_ATTR_LONG_ID);
//					if(DB_OPER_LOAD != opType)
//						_AddToByteArray(&pBA, GetExID());
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_UNION_ATTR_SZ_NAME:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_UNION_ATTR_SZ_NAME);
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, GetName().c_str());
//					}
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_UNION_ATTR_LONG_MASTER_ID:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_UNION_ATTR_LONG_MASTER_ID);
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, GetMasterID());
//					}
//					else
//					{
//						_AddToByteArray(&pBA, (BYTE)0);
//					}
//				}
//				break;
//			case DB_UNION_ATTR_BUF_PROPERTY:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_UNION_ATTR_BUF_PROPERTY);
//					if(DB_OPER_LOAD != opType)
//						GetPropertyData(pBA);
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_UNION_ATTR_BUF_MEMBERS:
//				{
//					_AddToByteArray(&pBA, (WORD)m_Members.size());
//					_AddToByteArray(&pBA, (long)DB_UNION_ATTR_BUF_MEMBERS);
//					if(DB_OPER_LOAD != opType)
//						GetMembersData(pBA);
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			}
//		}
//	}
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