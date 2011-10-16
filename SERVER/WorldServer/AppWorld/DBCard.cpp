#include "StdAfx.h"
#include "DBentityManager.h"
#include "DBCard.h"
#include "cardsdealingsystem/Card.h"
#include "cardsdealingsystem/CardsHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBCard::CDBCard()
: CBaseDBEntity(),
m_lSellerID(NULL_GUID),m_lBuyerID(NULL_GUID),m_dwPrice(0),m_dwChargePrice(0),
m_dwCardState(0),m_dwCardTime(0),m_dwStartTime(0),m_dwSerialNumber(0)
{
	CGUID::CreateGUID(m_GUID);

	memset(m_CardNumber, 0, sizeof(m_CardNumber));
	memset(m_CardType, 0, sizeof(m_CardType));
	memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
	memset(m_BuyerAccount, 0, sizeof(m_BuyerAccount));

	//初始化准备发送的属性Vector
	for(int i=(int)DB_CARD_ATTR_SZ_CARDNUMBER; i <= (int)DB_CARD_ATTR_DWORD_SERIALNUMBER; i++)
	{
		m_SendAttrEnumVector.push_back(1); // 0： 关闭 1: 打开
	}
}
CDBCard::CDBCard(const CGUID& id)
: CBaseDBEntity(id),m_lSellerID(NULL_GUID),m_lBuyerID(NULL_GUID),m_dwPrice(0),m_dwChargePrice(0),
m_dwCardState(0),m_dwCardTime(0),m_dwStartTime(0),m_dwSerialNumber(0)
{
	memset(m_CardNumber, 0, sizeof(m_CardNumber));
	memset(m_CardType, 0, sizeof(m_CardType));
	memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
	memset(m_BuyerAccount, 0, sizeof(m_BuyerAccount));

	//初始化准备发送的属性Vector
	for(int i=(int)DB_CARD_ATTR_SZ_CARDNUMBER; i <= (int)DB_CARD_ATTR_DWORD_SERIALNUMBER; i++)
	{
		m_SendAttrEnumVector.push_back(1); // 0： 关闭 1: 打开
	}
}
CDBCard::~CDBCard()
{

};

// Other interfaces
// 打开准备发送的属性在Vector中的相应位置的标志
void CDBCard::OpenAttrFlagInVector(DB_CARD_ATTR_TYPE attrType)
{
	if(attrType >= DB_CARD_ATTR_SZ_CARDNUMBER && attrType <= DB_CARD_ATTR_DWORD_SERIALNUMBER)
	{
		long pos = DB_CARD_ATTR_DWORD_SERIALNUMBER - attrType;
		m_SendAttrEnumVector.push_back(1);
	}
}
// 关闭准备发送的属性在Vector中的相应位置的标志
void CDBCard::CloseAttrFlagInVector(DB_CARD_ATTR_TYPE attrType)
{
	if(attrType >= DB_CARD_ATTR_SZ_CARDNUMBER && attrType <= DB_CARD_ATTR_DWORD_SERIALNUMBER)
	{
		long pos = DB_CARD_ATTR_DWORD_SERIALNUMBER - attrType;
		m_SendAttrEnumVector[pos] = 0;
	}
}

// virtual interfaces
void CDBCard::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
//	CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
	switch(op)
	{
	case DB_OPER_SAVE:
		Save(msgid, msgBuf, msgBufPos);
		break;
	case DB_OPER_LOAD:
		Load(msgid, msgBuf, msgBufPos);
		break;
	}
}

void CDBCard::Load(long msgid, BYTE* msgBuf, long& msgBufPos)
{
	// 解析消息
	CDBEntityManager::DBCardMapItr itr = GetGame()->GetDBEntityManager()->GetDBCardMap().find(GetExID());

	char szGuid[128];
	GetExID().tostring(szGuid);

	if(itr != GetGame()->GetDBEntityManager()->DBCardMapEnd())
	{
		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos);

		for(int i=0; i<(int)attrNum; i++)
		{
			// add one card data
			_GetWordFromByteArray(msgBuf, msgBufPos);
			DB_CARD_ATTR_TYPE attrType = (DB_CARD_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);
			switch(attrType)
			{
			case DB_CARD_ATTR_SZ_CARDNUMBER:
				{
					char num[1024];
					_GetStringFromByteArray(msgBuf, msgBufPos, num);
					SetCardNumber(num, strlen(num));
				}
				break;
			case DB_CARD_ATTR_SZ_CARDTYPE:
				{
					char num[1024];
					_GetStringFromByteArray(msgBuf, msgBufPos, num);
					SetCardType(num, strlen(num));
				}
				break;
			case DB_CARD_ATTR_SZ_SELLERACCOUNT:
				{
					char num[1024];
					_GetStringFromByteArray(msgBuf, msgBufPos, num);
					SetSellerAccount(num, strlen(num));
				}
				break;
			case DB_CARD_ATTR_LONG_SELLERID:
				{
					CGUID value;
					_GetBufferFromByteArray(msgBuf, msgBufPos, value);
					SetSellerID(value);
				}
				break;
			case DB_CARD_ATTR_SZ_BUYERACCOUNT:
				{
					char num[1024];
					_GetStringFromByteArray(msgBuf, msgBufPos, num);
					SetCardNumber(num, strlen(num));
				}
				break;
			case DB_CARD_ATTR_LONG_BUYERID:
				{
					CGUID value;
					_GetBufferFromByteArray(msgBuf, msgBufPos, value);
					SetBuyerID(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_PRICE:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetPrice(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_CHARGEPRICE:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetChargePrice(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_CARDSTATE:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetCardState(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_CARDTIME:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetCardTime(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_STARTTIME:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetStartTime(value);
				}
				break;
			case DB_CARD_ATTR_DWORD_SERIALNUMBER:
				{
					DWORD value = _GetDwordFromByteArray(msgBuf, msgBufPos);
					SetSerialNumber(value);
				}
				break;
			}
		}

		CCard *pCard = new CCard;

		if( !pCard) 
		{
			return;
		}

		GetGame()->GetDBEntityManager()->CDBCardToCCard(this, pCard);

		// 返回0为真。
		if( CCardsHandler::getInstance()->InsertCard(pCard) )
		{
			return;
		}
		
		
		////AddLogText("DB2W[%d] : DB_OPER_LOAD_CARD, CardID[%s]...OK!", msgid, szGuid);

		return;
	}

//	//AddLogText("DB2W[%d] : DB_OPER_LOAD_CARD, CardID[%s]...FAILED!", msgid, szGuid);
}

void CDBCard::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
{
//	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);

	char szGuid[128];
	GetExID().tostring(szGuid);

	////AddLogText("DB2W[%d] : DB_OPER_SAVE_CARD, CardID[%s]...OK!", msgid, szGuid);
}

// 组装消息
void CDBCard::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
{
	// 传递来的消息前面已经加入了
	// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
	// DBOP Type:     1byte, 数据库操作类型（枚举值）
	// MSG ID:        4byte, 该消息的MSGID
	// Entity Num:    2byte, 该实体个数
	if(DB_OPER_DELETE == opType) // DELETE操作
	{
		_AddToByteArray(&pBA, m_GUID); // 实体ID
		_AddToByteArray(&pBA, (long)0); // 属性种类个数

		// 发送CardNumber 字符串
		_AddToByteArray(&pBA, (WORD)1);
		_AddToByteArray(&pBA, (long)DB_CARD_ATTR_SZ_CARDNUMBER);
		_AddToByteArray(&pBA, GetCardNumber());
		return;
	}

	WORD attrNum = 0;
	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
	{
		if(m_SendAttrEnumVector[i] != 0)
			attrNum++;
	}

	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, attrNum); // 属性种类个数

	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
	{
		if(m_SendAttrEnumVector[i] != 0 
			&& ( DB_OPER_SAVE == opType || DB_OPER_UPDATE == opType || DB_OPER_LOAD == opType ) )// 标志打开并且为SAVE/UPDATE/LOAD操作
		{
			switch(DB_CARD_ATTR_SZ_CARDNUMBER+i)
			{
			case DB_CARD_ATTR_SZ_CARDNUMBER:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_SZ_CARDNUMBER);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetCardNumber());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_SZ_CARDTYPE:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_SZ_CARDTYPE);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetCardType());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_SZ_SELLERACCOUNT:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_SZ_SELLERACCOUNT);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetSellerAccount());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_LONG_SELLERID:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_LONG_SELLERID);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetSellerID());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_SZ_BUYERACCOUNT:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_SZ_BUYERACCOUNT);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetBuyerAccount());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_LONG_BUYERID:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_LONG_BUYERID);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetBuyerID());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_PRICE:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_PRICE);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetPrice());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_CHARGEPRICE:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_CHARGEPRICE);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetChargePrice());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_CARDSTATE:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_CARDSTATE);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetCardState());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_CARDTIME:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_CARDTIME);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetCardTime());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_STARTTIME:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_STARTTIME);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetStartTime());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			case DB_CARD_ATTR_DWORD_SERIALNUMBER:
				{
					_AddToByteArray(&pBA, (WORD)1);
					_AddToByteArray(&pBA, (long)DB_CARD_ATTR_DWORD_SERIALNUMBER);
					if(DB_OPER_LOAD != opType)
						_AddToByteArray(&pBA, GetSerialNumber());
					else
						_AddToByteArray(&pBA, (BYTE)0);
				}
				break;
			}
		}
	}

}