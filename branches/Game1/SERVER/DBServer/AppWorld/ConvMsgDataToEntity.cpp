#include "StdAfx.h"
#include "ConvMsgDataToEntity.h"
#include "EntityProperty.h"
#include "Player.h"
#include "goods\CGoods.h"
#include "..\dbServer\game.h"
#include "Country\Country.h"

CMsgDataWithEntityConvertor g_MsgDataWithEntityConvertor;

CMsgDataWithEntityConvertor::CMsgDataWithEntityConvertor()
{

}
CMsgDataWithEntityConvertor::~CMsgDataWithEntityConvertor()
{

}

// 标志操作
// 从队列后加入
void CMsgDataWithEntityConvertor::PushBackOperPropertyName(string& name)
{
	m_OperPropertyName.insert(m_OperPropertyName.end(), name);
}
// 从队列前弹出
string CMsgDataWithEntityConvertor::PopFrontOperPropertyName(void)
{
	string num = "";
	num = *m_OperPropertyName.begin();
	m_OperPropertyName.erase(m_OperPropertyName.begin());
	return num;
}
// num
long CMsgDataWithEntityConvertor::GetOperPropertyNameNum(void)
{
	long num = 0;
	num = m_OperPropertyName.size();
	return num;
}
//////////////////////////////////////////////////////////////////////////
// Interfaces
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CPlayer* player)
{
	if(ep)
	{
		if(player)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CPlayer::MSGTOATTRFUNC::iterator it = player->GetMSGToAttrFuncMap().find(itr->first);
				if(it != player->GetMSGToAttrFuncMap().end())
					(player->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CGoods* goods)
{
	if(ep)
	{
		if(goods)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CGoods::MSGTOATTRFUNC::iterator it = goods->GetMSGToAttrFuncMap().find(itr->first);
				if(it != goods->GetMSGToAttrFuncMap().end())
					(goods->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CCountry* country)
{
	if(ep)
	{
		if(country)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CCountry::MSGTOATTRFUNC::iterator it = country->GetMSGToAttrFuncMap().find(itr->first);
				if(it != country->GetMSGToAttrFuncMap().end())
					(country->*(it->second))(itr->second);
			}
		}
	}
}

void CMsgDataWithEntityConvertor::EntityToMsgData(CPlayer* player, vector<BYTE>& pBA)
{
	// 添加到MSG
	/*
	|----外部填写
	|OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
	|DBOP Type:     1byte, 数据库操作类型（枚举值）
	|MSG ID:        4byte, 该消息的MSGID
	|Entity Num:	2byte, 该实体个数(X)

	// 此处填写
	---------------从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码----------------
	|	|----
	|   |EntityID:		4byte, 实体ID
	|	|Attr Num:		2byte, 属性种类个数(N)
	|	|	 |----
	|	|	 |该属性个数:		2byte, 属性数据个数(M) 全部： 0xFFFF OPER_ALL_DATA
	|X*	|N * |该属性类型:		4byte, 属性类型枚举值
	|	|	 |该属性ID:			4byte, 属性ID, 
	|	|	 |该属性名字:		nbyte, 属性名字,字符串
	|	|	 |		|----
	|	|	 |		|该属性POS:		2byte, 属性POS, 
	|	|	 |		|该属性size:	2byte, 属性SIZE, 
	|	|	 |	M *	|该属性数据块:	nbtye, 该数据块长度由属性数据长度决定 (若消息为LOAD则该数据块为1byte，值为0)
	|	|	 |		|----
	|	|	 |----
	|	|----
	*/

	if(player)
	{
		_AddToByteArray(&pBA, player->GetExID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();
		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Player");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(player->GetExID(), string(player->GetName()));
		}

		if(ep)
		{
			ep->SetExID(player->GetExID());

			CWrapDataObject* wdo = NULL;
			
			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CPlayer::ATTRTOMSGFUNC::iterator it = player->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != player->GetAttrToMSGFuncMap().end())
				{
					(player->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(CGoods* goods, vector<BYTE>& pBA)
{
	if(goods)
	{
		_AddToByteArray(&pBA, goods->GetExID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();
	
		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Goods");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(goods->GetExID(), string("Goods"));
		}

		if(ep)
		{
			ep->SetExID(goods->GetExID());

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CGoods::ATTRTOMSGFUNC::iterator it = goods->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != goods->GetAttrToMSGFuncMap().end())
				{
					(goods->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(CCountry* country, vector<BYTE>& pBA)
{
	if(country)
	{
		_AddToByteArray(&pBA, country->GetID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Countrys");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(country->GetID(), string("Countrys"));
		}

		if(ep)
		{
			ep->SetID(country->GetID());

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CCountry::ATTRTOMSGFUNC::iterator it = country->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != country->GetAttrToMSGFuncMap().end())
				{
					(country->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}

void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CCard* card)
{
	if(ep)
	{
		if(card)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CCard::MSGTOATTRFUNC::iterator it = card->GetMSGToAttrFuncMap().find(itr->first);
				if(it != card->GetMSGToAttrFuncMap().end())
					(card->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, tagAttackCityTime* CityWarParam)
{
	if(ep)
	{
		if(CityWarParam)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				tagAttackCityTime::MSGTOATTRFUNC::iterator it = CityWarParam->GetMSGToAttrFuncMap().find(itr->first);
				if(it != CityWarParam->GetMSGToAttrFuncMap().end())
					(CityWarParam->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, tagVilWarSetup* VillageWarSetup)
{
	if(ep)
	{
		if(VillageWarSetup)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				tagVilWarSetup::MSGTOATTRFUNC::iterator it = VillageWarSetup->GetMSGToAttrFuncMap().find(itr->first);
				if(it != VillageWarSetup->GetMSGToAttrFuncMap().end())
					(VillageWarSetup->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, tagEnemyFaction* EnemyFactions)
{
	if(ep)
	{
		if(EnemyFactions)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				if(itr->first == "FactionID1")//敌对帮会甲方
				{
					CWrapDataObject::ValueInfo* vi = itr->second->GetValueArray(0);
					if(vi)
					{
						EnemyFactions->FactionGuid1 = *(CGUID*)vi->buf;
					}
				}
				else if(itr->first == "FactionID2")//敌对帮会乙方
				{
					CWrapDataObject::ValueInfo* vi = itr->second->GetValueArray(0);
					if(vi)
					{
						EnemyFactions->FactionGuid2 = *(CGUID*)vi->buf;
					}
				}
				else if(itr->first == "DisandTime")//敌对状态解散倒计时(毫秒)
				{
					CWrapDataObject::ValueInfo* vi = itr->second->GetValueArray(0);
					if(vi)
					{
						EnemyFactions->dwDisandTime = *(DWORD*)vi->buf;
					}
				}
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CWorldRegion* pRegion)
{
	if(ep)
	{
		if(pRegion)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CWorldRegion::MSGTOATTRFUNC::iterator it = pRegion->GetMSGToAttrFuncMap().find(itr->first);
				if(it != pRegion->GetMSGToAttrFuncMap().end())
					(pRegion->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CUnion *pConf)
{
	if(ep)
	{
		if(pConf)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CUnion::MSGTOATTRFUNC::iterator it = pConf->GetMSGToAttrFuncMap().find(itr->first);
				if(it != pConf->GetMSGToAttrFuncMap().end())
					(pConf->*(it->second))(itr->second);
			}
		}
	}
}
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep, CFaction* pFaction)
{
	if(ep)
	{
		if(pFaction)
		{
			CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
			for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
			{
				CFaction::MSGTOATTRFUNC::iterator it = pFaction->GetMSGToAttrFuncMap().find(itr->first);
				if(it != pFaction->GetMSGToAttrFuncMap().end())
					(pFaction->*(it->second))(itr->second);
			}
		}
	}
}

void CMsgDataWithEntityConvertor::EntityToMsgData(CCard* card, const CGUID& entityID, vector<BYTE>& pBA)
{
	if(card)
	{
		_AddToByteArray(&pBA, entityID); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Cards");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(entityID, string("Cards"));
		}

		if(ep)
		{
			ep->SetExID(entityID);

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CCard::ATTRTOMSGFUNC::iterator it = card->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != card->GetAttrToMSGFuncMap().end())
				{
					(card->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(tagAttackCityTime* CityWarParam, const CGUID& entityID, vector<BYTE>& pBA)
{
	if(CityWarParam)
	{
		_AddToByteArray(&pBA, entityID); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("CityWarsParam");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(entityID, string("CityWarsParam"));
		}

		if(ep)
		{
			ep->SetExID(entityID);

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				tagAttackCityTime::ATTRTOMSGFUNC::iterator it = CityWarParam->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != CityWarParam->GetAttrToMSGFuncMap().end())
				{
					(CityWarParam->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(tagVilWarSetup* VillageWarSetup, const CGUID& entityID, vector<BYTE>& pBA)
{
	if(VillageWarSetup)
	{
		_AddToByteArray(&pBA, entityID); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("VillageWarsParam");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(entityID, string("VillageWarsParam"));
		}

		if(ep)
		{
			ep->SetExID(entityID);

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				tagVilWarSetup::ATTRTOMSGFUNC::iterator it = VillageWarSetup->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != VillageWarSetup->GetAttrToMSGFuncMap().end())
				{
					(VillageWarSetup->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(tagEnemyFaction* EnemyFactions, const CGUID& entityID, vector<BYTE>& pBA)
{
	if(EnemyFactions)
	{
		_AddToByteArray(&pBA, entityID); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("EnemyFactions");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(entityID, string("EnemyFactions"));
		}

		if(ep)
		{
			ep->SetExID(entityID);

			CWrapDataObject* wdo = NULL;
			CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find("FactionID1");
			CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().find(0);
			if(viItr != wdoItr->second->GetValueInfoMap().end() && wdoItr != ep->GetWrapDataObjectMap().end())
			{
				viItr->second->dataType = DATA_OBJECT_LONG;
				viItr->second->pos = 0;// 该属性POS:2byte,
				memcpy_s(viItr->second->buf, sizeof(long), 
				&EnemyFactions->FactionGuid1, sizeof(long));	// 该属性数据块:nbtye
				viItr->second->size = 4; 

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, "FactionID1");			// 该属性名字:nbyte
				_AddToByteArray(&pBA, (WORD)sizeof(long));
				_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
			}	

			wdoItr = ep->GetWrapDataObjectMap().find("FactionID2");
			viItr = wdoItr->second->GetValueInfoMap().find(0);
			if(viItr != wdoItr->second->GetValueInfoMap().end() && wdoItr != ep->GetWrapDataObjectMap().end())
			{
				viItr->second->dataType = DATA_OBJECT_LONG;
				viItr->second->pos = 0;// 该属性POS:2byte,
				memcpy_s(viItr->second->buf, sizeof(long), 
				&EnemyFactions->FactionGuid2, sizeof(long));	// 该属性数据块:nbtye
				viItr->second->size = 4; 

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, "FactionID2");			// 该属性名字:nbyte
				_AddToByteArray(&pBA, (WORD)sizeof(long));
				_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
			}

			wdoItr = ep->GetWrapDataObjectMap().find("DisandTime");
			viItr = wdoItr->second->GetValueInfoMap().find(0);//敌对状态解散倒计时(毫秒)
			if(viItr != wdoItr->second->GetValueInfoMap().end() && wdoItr != ep->GetWrapDataObjectMap().end())
			{
				viItr->second->dataType = DATA_OBJECT_LONG;
				viItr->second->pos = 0;// 该属性POS:2byte,
				memcpy_s(viItr->second->buf, sizeof(DWORD), 
					&EnemyFactions->dwDisandTime, sizeof(DWORD));	// 该属性数据块:nbtye
				viItr->second->size = 4; 

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, "DisandTime");			// 该属性名字:nbyte
				_AddToByteArray(&pBA, (WORD)sizeof(long));
				_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
			}
		}
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(CWorldRegion* pRegion, vector<BYTE>& pBA)
{
	if(pRegion)
	{
		_AddToByteArray(&pBA, pRegion->GetExID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Regions");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(pRegion->GetExID(), string("Regions"));
		}

		if(ep)
		{
			ep->SetExID(pRegion->GetExID());

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CWorldRegion::ATTRTOMSGFUNC::iterator it = pRegion->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != pRegion->GetAttrToMSGFuncMap().end())
				{
					(pRegion->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(CUnion *pConf, vector<BYTE>& pBA)
{
	if(pConf)
	{
		_AddToByteArray(&pBA, pConf->GetExID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Unions");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(pConf->GetExID(), string("Unions"));
		}

		if(ep)
		{
			ep->SetExID(pConf->GetExID());

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CUnion::ATTRTOMSGFUNC::iterator it = pConf->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != pConf->GetAttrToMSGFuncMap().end())
				{
					(pConf->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}
void CMsgDataWithEntityConvertor::EntityToMsgData(CFaction* pFaction, vector<BYTE>& pBA)
{
	if(pFaction)
	{
		_AddToByteArray(&pBA, pFaction->GetExID()); //实体ID
		_AddToByteArray(&pBA, (WORD)m_OperPropertyName.size()); // 属性种类个数(N)

		CEntityProperty* ep = NULL;
		OperPropertyNameListItr itr = m_OperPropertyName.begin();

		CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("Factions");
		if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
		{
			ep = epIT->second;
		}
		else
		{
			ep = g_DataEntityManager.CreateEntityProperty(pFaction->GetExID(), string("Factions"));
		}

		if(ep)
		{
			ep->SetExID(pFaction->GetExID());

			CWrapDataObject* wdo = NULL;

			for(; itr != m_OperPropertyName.end(); itr++)
			{
				CFaction::ATTRTOMSGFUNC::iterator it = pFaction->GetAttrToMSGFuncMap().find(*itr);
				CEntityProperty::WrapDataObjectMapItr wdoItr = ep->GetWrapDataObjectMap().find(*itr);
				if(wdoItr != ep->GetWrapDataObjectMap().end() && it != pFaction->GetAttrToMSGFuncMap().end())
				{
					(pFaction->*(it->second))(wdoItr->second);
				}

				// 添加数据到MSG
				_AddToByteArray(&pBA, (WORD)wdoItr->second->GetValueInfoMap().size());// 该属性个数:2byte
				_AddToByteArray(&pBA, (long)wdoItr->second->GetDataType());// 该属性类型:4byte
				_AddToByteArray(&pBA, wdoItr->first.c_str());// 该属性ID:4byte
				_AddToByteArray(&pBA, (*itr).c_str());			// 该属性名字:nbyte

				CWrapDataObject::ValueInfoMapItr viItr = wdoItr->second->GetValueInfoMap().begin();
				for(; viItr != wdoItr->second->GetValueInfoMap().end(); viItr++)
				{
					_AddToByteArray(&pBA, (WORD)viItr->second->pos);// 该属性POS:2byte,
					switch(wdoItr->second->GetDataType())
					{
					case DATA_OBJECT_LONG:
						_AddToByteArray(&pBA, (WORD)sizeof(long));
						_AddToByteArray(&pBA, *(long*)(viItr->second->buf)); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_BUFFER_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, viItr->second->buf, viItr->second->size); // 该属性数据块:nbtye
						break;
					case DATA_OBJECT_CHAR_ARRAY:
						_AddToByteArray(&pBA, (WORD)viItr->second->size);
						_AddToByteArray(&pBA, (char*)viItr->second->buf); // 该属性数据块:nbtye
						break;
					}
				}
			}
		}

		ClearOperPropertyNameList();
	}
}

// Setup
void CMsgDataWithEntityConvertor::MsgDataToEntity(CEntityProperty* ep)
{
	if(ep)
	{
		CEntityProperty::WrapDataObjectMapItr itr = ep->GetWrapDataObjectMap().begin();
		for(; itr != ep->GetWrapDataObjectMap().end(); itr++)
		{
			if(itr->first == "PlayerID")//PlayerID
			{
				CWrapDataObject::ValueInfo* vi = itr->second->GetValueArray(0);
				if(vi)
				{
					GetGame()->SetDBPlayerID(*(CGUID*)vi->buf);
				}
			}
			else if(itr->first == "LeaveWordID")//LeaveWordID
			{
				CWrapDataObject::ValueInfo* vi = itr->second->GetValueArray(0);
				if(vi)
				{
					GetGame()->SetDBLeaveWordID(*(long*)vi->buf);
				}
			}
		}
	}
}

// Setup
void CMsgDataWithEntityConvertor::EntityToMsgData(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, (long)0); //实体ID
	_AddToByteArray(&pBA, (WORD)2); // 属性种类个数(N)

	CEntityProperty* ep = NULL;

	CDataEntityManager::EntityPropertyMapItr epIT = g_DataEntityManager.GetEntityPropertyMap().find("EnemyFactions");
	if(epIT != g_DataEntityManager.GetEntityPropertyMap().end())
	{
		ep = epIT->second;
	}
	else
	{
		ep = g_DataEntityManager.CreateEntityProperty(0, string("Setup"));
	}

		if(ep)
		{
			ep->SetExID(0);
		
			// PlayerID 添加数据到MSG
			_AddToByteArray(&pBA, (WORD)1);// 该属性个数:2byte
			_AddToByteArray(&pBA, (long)DATA_OBJECT_LONG);	// 该属性类型:4byte
			_AddToByteArray(&pBA, (long)0);						// 该属性ID:4byte
			_AddToByteArray(&pBA, "PlayerID");				// 该属性名字:nbyte
			_AddToByteArray(&pBA, (WORD)0);// 该属性POS:2byte,
			_AddToByteArray(&pBA, (WORD)sizeof(long));
			_AddToByteArray(&pBA, GetGame()->GetDBPlayerID()); // 该属性数据块:nbtye

			// LeaveWordID添加数据到MSG
			_AddToByteArray(&pBA, (WORD)1);// 该属性个数:2byte
			_AddToByteArray(&pBA, (long)DATA_OBJECT_LONG);	// 该属性类型:4byte
			_AddToByteArray(&pBA, (long)0);						// 该属性ID:4byte
			_AddToByteArray(&pBA, "LeaveWordID");				// 该属性名字:nbyte
			_AddToByteArray(&pBA, (WORD)0);// 该属性POS:2byte,
			_AddToByteArray(&pBA, (WORD)sizeof(long));
			_AddToByteArray(&pBA, GetGame()->GetDBLeaveWordID()); // 该属性数据块:nbtye

		}
}