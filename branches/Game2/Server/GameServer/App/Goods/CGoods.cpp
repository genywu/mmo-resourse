// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CGoods.h"
#include "CGoodsFactory.h"
#include "../GoodsExManage.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "Enchase/cenchasehole.h"
#include "Enchase/CKitCardRuleFactory.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoods::CGoods()
{
	m_lType		= TYPE_GOODS;
	m_dwAmount	= 1;
	m_dwBuyPrice=0;
	m_dwMaxHoleNum=0;
	m_pEnchaseHoleArray=NULL;
	m_dwActivedKitCardId=0;
}

CGoods::~CGoods()
{
	Release();
}

bool  CGoods::SetAttribute(ulong ID, ulong value, int valuePos) 
{
	// 添加到脏数据
	m_listDirtyAttrData.insert(m_listDirtyAttrData.end(), DIRTYATTRDATA(ID, valuePos));

    if(GP_PRICE == ID)
    {
        //m_dwPrice = value;
        return true;
    }
    else if(GP_DESCRIBE == ID)
    {         
       return true;       
    }
    else if(GP_TYPE == ID)
    {
        return true;
    }
    return (true == SetAddonPropertyBaseValues( (eGoodsAddonProperties)ID, valuePos, value ));
}

bool  CGoods::AddAttribute(ulong ID, ulong value, int valuePos/*第pos个值*/)
{
	// 添加到脏数据
	m_listDirtyAttrData.insert(m_listDirtyAttrData.end(), DIRTYATTRDATA(ID, valuePos));

    if(GP_PRICE == ID)
    {
        //m_dwPrice = value;
        return true;
    }
    else if(GP_DESCRIBE == ID)
    {
		return true;
    }
    else if(GP_TYPE == ID)
    {
        return true;
    }
    return (true == AddAddonPropertyBaseValues( (eGoodsAddonProperties)ID, valuePos, value ));
}

ulong CGoods::GetAttribute(ulong ID, int valuePos)
{
    if(GP_PRICE == ID)
    {
        return GoodsSetup::QueryGoodsPrice(valuePos);
    }
    else if(GP_DESCRIBE == ID)
    {
        return (ulong)GoodsSetup::QueryGoodsDescribe(valuePos);
    }
    else if(GP_TYPE == ID)
    {
        return (ulong)GoodsSetup::QueryGoodsType(valuePos);
    }
    else if(GP_ORIGINAL_NAME == ID)
    {
        return (ulong)GoodsSetup::QueryGoodsOriginalName(valuePos);
    }
    else if(GP_NAME == ID)
    {
        return (ulong)GoodsSetup::QueryGoodsName(valuePos);
    }
    return GetAddonPropertyValues( (eGoodsAddonProperties)ID, valuePos );
}
void  CGoods::ChangeAttribute(ulong ID, ulong value, int valuePos/*第pos个值*/, bool bOverload)
{
	if(bOverload)
	{
		SetAddonPropertyBaseValues( (eGoodsAddonProperties)ID, valuePos, value);
	}
	else
	{
		SetAddonPropertyBaseValues( (eGoodsAddonProperties)ID, valuePos,
			value+GetAddonPropertyValues((eGoodsAddonProperties)ID, valuePos) );
	}

    // 添加到脏数据
	list<DIRTYATTRDATA>::iterator itr = m_listDirtyAttrData.begin();

	for(; itr != m_listDirtyAttrData.end(); itr++)
	{
		if(itr->ID == ID && itr->valuePos == valuePos)
			return;
	}

	m_listDirtyAttrData.insert(m_listDirtyAttrData.end(), DIRTYATTRDATA(ID, valuePos));
}

long  CGoods::UpdateAttribute(/*const CGUID&ownerID,*/ const CGUID& sendPlayerID)
{
    long num = 0;

    CMessage msg( MSG_S2C_PLAYER_CHANGE_PROPERTIES );
	msg.Add(GetExID());

	// value num
	msg.Add((long)m_listDirtyAttrData.size());
	list<DIRTYATTRDATA>::iterator itr = m_listDirtyAttrData.begin();
    for(; itr != m_listDirtyAttrData.end(); itr++)
    {
        DIRTYATTRDATA& data = *itr;
		msg.Add((long)data.ID);
		msg.Add((long)data.valuePos);
        msg.Add(GetAddonPropertyValues((eGoodsAddonProperties)data.ID, data.valuePos));
    }
    msg.SendToPlayer(sendPlayerID);
    m_listDirtyAttrData.clear();

    return num;
}

ulong CGoods::GetBasePropertiesIndex()
{	
	return m_dwBasePropertiesIndex;
}


void CGoods::InitEnchaseHole()
{
	//如可以开孔
	ulong dwNum=GetMaxEnchaseHoleNum();
	if(dwNum>0)
	{
		m_pEnchaseHoleArray=new  CEnchaseHole*[dwNum];
		m_dwMaxHoleNum=dwNum;
		for(ulong i=0;i<dwNum;i++)
		{
			m_pEnchaseHoleArray[i]=new CEnchaseHole(this);
		}
	}
}

void CGoods::EnchaseRelease()
{	
	if(m_dwMaxHoleNum>0 && m_pEnchaseHoleArray)
	{
		for(int i=0;i<(int)m_dwMaxHoleNum;i++)
		{
			SAFE_DELETE(m_pEnchaseHoleArray[i]);
		}
	}
	SAFE_DELETE(m_pEnchaseHoleArray);
	m_dwMaxHoleNum=0;
}

bool CGoods::ActiveEnchaseHole(CPlayer* pPlayer,int nIndex)
{	
	if(m_dwMaxHoleNum>0 && nIndex>=0 && nIndex< (int)m_dwMaxHoleNum)
	{
		//必须是未激活状态 且未镶嵌卡片
		if(m_pEnchaseHoleArray[nIndex]->GetState() || 
			m_pEnchaseHoleArray[nIndex]->GetCard())
		{
			return false;
		}	
		m_pEnchaseHoleArray[nIndex]->SetState(true);
		if(m_dwActivedKitCardId>0)
		{
			CKitCardRuleFactory::KitCardInvalidate(pPlayer,this,m_dwActivedKitCardId);
			SetActivedKitCardId(0);
		}
		CKitCardRuleFactory::KitCardActive(pPlayer,this);
		return true;
	}	
	return false;
}

ulong CGoods::ActiveEnchaseHole(CPlayer* pPlayer)
{	
	for(ulong i=0;i<m_dwMaxHoleNum;i++)
	{
		if(!m_pEnchaseHoleArray[i]->GetState() && 
			!m_pEnchaseHoleArray[i]->GetCard())
		{
			m_pEnchaseHoleArray[i]->SetState(true);
			if(m_dwActivedKitCardId>0)
			{
				CKitCardRuleFactory::KitCardInvalidate(pPlayer,this,m_dwActivedKitCardId);
				SetActivedKitCardId(0);
			}
			CKitCardRuleFactory::KitCardActive(pPlayer,this);
			return i;
		}
	}	
	return -1;
}


bool CGoods::CloseEnchaseHole(CPlayer* pPlayer,int nIndex)
{
	ulong dwNum=GetMaxEnchaseHoleNum();
	if(dwNum>0 && nIndex>=0 && nIndex< (int)dwNum)
	{
		//必须是未激活状态 且未镶嵌卡片
		/*
		if(!m_pEnchaseHoleArray[nIndex]->GetState() || 
			m_pEnchaseHoleArray[nIndex]->GetCard())
		{
			return false;
		}*/	
		CGoods* pCard=m_pEnchaseHoleArray[nIndex]->GetCard();
		if(pCard)
		{
			m_pEnchaseHoleArray[nIndex]->RemoveCard(pPlayer,false);
			m_pEnchaseHoleArray[nIndex]->SetCard(NULL);
		}
		m_pEnchaseHoleArray[nIndex]->SetState(false);
		//目前套卡激活
		if(m_dwActivedKitCardId>0)
		{
			CKitCardRuleFactory::KitCardInvalidate(pPlayer,this,m_dwActivedKitCardId);
			SetActivedKitCardId(0);
		}
		CKitCardRuleFactory::KitCardActive(pPlayer,this);
		return true;
	}	
	return false;
}


ulong CGoods::CloseEnchaseHole()
{
	ulong dwNum=GetMaxEnchaseHoleNum();
	if(dwNum>0)
	{
		ulong i=dwNum-1;
		for(;i>=0;i--)
		{
			if(m_pEnchaseHoleArray[i]->GetState())
			{
				if(m_pEnchaseHoleArray[i]->GetCard())
				{
				}
				m_pEnchaseHoleArray[i]->SetState(false);
				return i;
			}
		}	
	}
	return -1;
}

//一次激活指定数量的孔
void CGoods::ActiveHoles(int nNum)
{
	ulong dwNum=GetMaxEnchaseHoleNum();
	if(nNum>0 && nNum<=(int)dwNum)
	{
		int j=0;
		for(ulong i=0;i<dwNum;i++)
		{			
			m_pEnchaseHoleArray[i]->SetState(true);			
			j++;
			if(j>=nNum)
				return;
		}	
	}
}


//卡片重新排序功能
void CGoods::EnchaseArrange()
{	
	vector<CGoods*> vecGoods;
	vecGoods.clear();
	//最大孔数
	ulong dwNum=GetMaxEnchaseHoleNum();

	//wang-xx
	////激活孔数
	//ulong dwActived=GetActivedHoleNum();
	for(ulong i=0;i<dwNum;i++)
	{
		//if(m_pEnchaseHoleArray[i]->GetState() && 
		//	m_pEnchaseHoleArray[i]->GetCard())
		if( m_pEnchaseHoleArray[i]->GetCard() )
		{
			//把镶嵌了的卡放入vector
			vecGoods.push_back(m_pEnchaseHoleArray[i]->GetCard());
		}		
		//m_pEnchaseHoleArray[i]->SetState(false);
		m_pEnchaseHoleArray[i]->SetCard(NULL);
	}	
	
	//按顺序激活孔
	//for(ulong i=0;i<dwNum;i++)
	//{
	//	if(i<dwActived)
	//	{
	//		m_pEnchaseHoleArray[i]->SetState(true);
	//	}
	//}
	//按顺序安装卡
	vector<CGoods*>::iterator goodsIter=vecGoods.begin();
	int j=0;
	for(;goodsIter!=vecGoods.end();goodsIter++)
	{
		m_pEnchaseHoleArray[j]->SetCard(*goodsIter);
		j++;
	}
}

void CGoods::OnSuitNumChanged(CPlayer* pPlayer,int new_num)
{
	InvalidSuitProperties(pPlayer);

	vector<CGoodsBaseProperties::tagSuitProperty>::iterator propertyIter=m_vSuitProperties.begin();
	for(;propertyIter!=m_vSuitProperties.end();propertyIter++)
	{
		if(propertyIter->vValues[2].lBaseValue<=new_num)
		{
			string strPropertyName=CPlayer::GetFightPropertyNameByEnum(propertyIter->gapType);
			if(strPropertyName!="")
			{
				pPlayer->ChangeAttribute(strPropertyName,propertyIter->vValues[0].lBaseValue);
				propertyIter->bActived=true;
			}
		}
	}
	
}

void CGoods::InvalidSuitProperties(CPlayer* pPlayer)
{
	vector<CGoodsBaseProperties::tagSuitProperty>::iterator propertyIter=m_vSuitProperties.begin();
	for(;propertyIter!=m_vSuitProperties.end();propertyIter++)
	{				
	    if(propertyIter->bActived)
		{
			string strPropertyName=CPlayer::GetFightPropertyNameByEnum(propertyIter->gapType);
			propertyIter->bActived=false;			
			if(strPropertyName!="")
			{
				pPlayer->ChangeAttribute(strPropertyName,-(propertyIter->vValues[0].lBaseValue));
			}
		}
	}
}


void CGoods::RunScriptOnGet(CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	ulong dwVal=this->GetAttribute(GAP_GET_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/Goods/%d.lua",dwVal);
		Script::RunEnv env( pPlayer, NULL, static_cast<CRegion*>( pPlayer->GetFather() ) );
		GetInst( ScriptSys ).RunScript( env, strFile );
	}
}

void CGoods::RunScriptOnLose(CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	ulong dwVal=this->GetAttribute(GAP_LOST_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/Goods/%d.lua",dwVal);
		Script::RunEnv env( pPlayer, NULL, static_cast<CRegion*>( pPlayer->GetFather() ) );
		GetInst( ScriptSys ).RunScript( env, strFile );
	}
}

void CGoods::RunScriptOnEquip(CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	ulong dwVal=this->GetAttribute(GAP_EQUIP_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/Goods/%d.lua",dwVal);
		Script::RunEnv env( pPlayer, NULL, static_cast<CRegion*>( pPlayer->GetFather() ), NULL, GetExID() );
		GetInst( ScriptSys ).RunScript( env, strFile );
	}
}


void CGoods::RunScriptOnRemoveEquip(CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	ulong dwVal=this->GetAttribute(GAP_UNEQUIP_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/Goods/%d.lua",dwVal);
		Script::RunEnv env( pPlayer, NULL, static_cast<CRegion*>( pPlayer->GetFather() ), NULL, GetExID() );
		GetInst( ScriptSys ).RunScript( env, strFile );
	}
}


void CGoods::SetAmount(ulong dwAmount)
{	
	if( dwAmount == 0 )
	{
		//##把自己从容器内删除？
	}
	m_dwAmount = dwAmount;
}

ulong CGoods::GetAmount()
{	
	return m_dwAmount;
}

//取得道具的最大叠加数限制
ulong CGoods::GetMaxStackNumber()
{	
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if(GetBasePropertiesIndex()==GoodsSetup::GetGoldCoinIndex())
	{
		return GlobalSetup::GetSetup()->dwGoldCoinLimit;
	}
	else if(GetBasePropertiesIndex()==GoodsSetup::GetSilverIndex())
	{
		return GlobalSetup::GetSetup()->dwSilverLimit;
	}

	ulong dwResult = 1;
	if( pProperties && (pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE ||
		pProperties -> GetGoodsBaseType() == GBT_USELESS) )
	{
		vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut;
		pProperties -> GetAddonPropertyValues( GAP_MAXOVERLAP, vOut );
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i].dwId == 1 )
			{
				dwResult = static_cast<ulong>( vOut[i].lBaseValue );
				break;
			}
		}
		vOut.clear();
	}

	return dwResult;
}

//! 得到物品的绑定冻结属性
CGoods::eBindFreeze CGoods::GetBindFreezeProperties(void)
{
	long lValue = GetAddonPropertyValues(GAP_FREEZE, 1);
	if (0 != lValue) return eBF_Freeze;
	
	lValue = GetAddonPropertyValues(GAP_BIND, 1);
	if (0 == lValue && 2 == lValue) return eBF_NoAll;

	return eBF_OnlyBind;
}

//! 判断物品能否升级、并返回物品等级，若不能，返回-1
long CGoods::CanUpgrade(void)
{
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if( pProperties && pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
	{
		for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
		{
			if( m_vAddonProperties[i].gapType == GAP_WEAPON_LEVEL )
			{
				return m_vAddonProperties[i].lValues[0];
			}
		}
	}
	return -1;
}

//! 判断物品能否魔化、并返回物品魔化等级，若不能，返回-1
long CGoods::CanPlugEvil(void)
{
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if( pProperties && pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
	{
		for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
		{
			if( m_vAddonProperties[i].gapType == GAP_PLUG_EVIL_LEVEL )
			{
				return m_vAddonProperties[i].lValues[0];
			}
		}
	}
	return -1;
}

bool CGoods::CanUpgraded()
{
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if( pProperties && pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
	{
		for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
		{
			if( m_vAddonProperties[i].gapType == GAP_WEAPON_LEVEL )
			{
				return true;
			}
		}
	}
	return false;
}

//判断物品是否可以修理
bool CGoods::CanReparied()
{	
	bool bResult = false;
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	//该物品是装备且没有特殊属性的限制
	if( pProperties && pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
	{
		if( ( GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1) & PA_CANNOT_REPAIRED ) != PA_CANNOT_REPAIRED )
		{
			bResult = true;
		}
	}
	return bResult;
}

eWeaponType CGoods::GetWeaponType()
{
	ulong dwVal=GetAddonPropertyValues(GAP_WEAPON_CATEGORY,1);
	if(dwVal>=WT_SINGLE_SWORD && dwVal<=WT_FIST)
	{
		return static_cast<eWeaponType>(dwVal);
	}
	else
		return WT_UNKNOWN;
}


eWeaponHandType CGoods::GetWeaponHandType()
{
	eWeaponType eWT=GetWeaponType();
	if(eWT!=WHT_UNKNOWN)
	{
		if(eWT==WT_SINGLE_SWORD || eWT==WT_SINGLE_KNIFE || eWT==WT_SINGLE_HAMMER)
			return WHT_SINGLE_HAND;
		else
			return WHT_DOUBLE_HAND;
	}
	else
		return WHT_UNKNOWN;
}

eBusinessGoodsType CGoods::GetBusinessGoodsType()
{
	ulong dwVal=GetAddonPropertyValues(GAP_TRADEGOODS_TYPE,1);
	if(dwVal>=BGT_FOOD && dwVal<BGT_MAX_TYPE)
	{
		eBusinessGoodsType eVal=static_cast<eBusinessGoodsType>(dwVal);
		return eVal;
	}	
	return BGT_NOT_BUSINESS_GOODS;
}

bool CGoods::CanEnchase()
{
	long lBaseHoleNum=GetAddonPropertyValues(GAP_HOLE_NUM,2);
	if(lBaseHoleNum>0)
	{
		return true;
	}
	return false;
}
long* CGoods::GetEnchaseHoleData()
{
	long* pArray=new long[5];
	ulong dwNum=GetMaxEnchaseHoleNum();
	for(ulong i=0;i<5;i++)
	{			
		if(i<dwNum)
		{
			if(m_pEnchaseHoleArray[i]->GetCard())
			{
				pArray[i]=m_pEnchaseHoleArray[i]->GetCard()->GetBasePropertiesIndex();
			}
			else if(m_pEnchaseHoleArray[i]->GetState())
			{
				pArray[i]=0;
			}	
			else 
			{
				pArray[i]=-1;
			}
		}
		else
		{
			pArray[i]=-1;
		}

	}

	return pArray;
}
ulong CGoods::GetMaxEnchaseHoleNum()
{
	return GetAddonPropertyValues(GAP_HOLE_NUM,2);
}

ulong CGoods::GetActivedHoleNum()
{
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	if(dwMaxHoleNum>0)
	{
		ulong dwActivedHoleNum=0;
		for(ulong i=0;i<dwMaxHoleNum;i++)
		{
			if(m_pEnchaseHoleArray[i]->GetState())
			{
				dwActivedHoleNum++;
			}
		}
		return dwActivedHoleNum;
	}	
	return 0;
	
}

ulong CGoods::GetEnchasedCardNum()
{
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	if(dwMaxHoleNum>0)
	{
		ulong dwCardNum=0;
		for(ulong i=0;i<dwMaxHoleNum;i++)
		{
			if(m_pEnchaseHoleArray[i]->GetCard())
			{
				dwCardNum++;
			}
		}
		return dwCardNum;
	}
	return 0;
}

void CGoods::GetEnchasedCard(vector<ulong>& vecCards)
{
	vecCards.clear();
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	if(dwMaxHoleNum>0)
	{
		ulong dwCardNum=0;
		for(ulong i=0;i<dwMaxHoleNum;i++)
		{
			CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
			if(pCard)
			{
				vecCards.push_back(pCard->GetBasePropertiesIndex());
			}
		}
	}
}


CGoods* CGoods::GetEnchasedCard(int nIndex)
{
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	if(nIndex>=0 && nIndex<(int)dwMaxHoleNum)
	{
		return m_pEnchaseHoleArray[nIndex]->GetCard();
	}
	return NULL;
}

void CGoods::UpdateEnchaseAttr()
{
	struct  stCardData
	{
		ulong dwPos;
		CGoods* pGoods;
	};
	vector<stCardData> vecCards;
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	for(ulong i=0;i<dwMaxHoleNum;i++)
	{
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard)
		{
			stCardData stCard;
			stCard.dwPos=i;
			stCard.pGoods=pCard;
			vecCards.push_back(stCard);
			m_pEnchaseHoleArray[i]->SetCard(NULL);
		}
	}
	for(int i=0;i<(int)vecCards.size();i++)
	{
		Enchase(NULL,vecCards[i].pGoods,vecCards[i].dwPos);
	}
}

bool CGoods::Enchase(CPlayer* pPlayer,CGoods* pGoods,int nIndex)
{
	if(!pGoods || nIndex<0)
		return false;
	//物品本身必须可镶嵌
	if(!CanEnchase())
		return false;
	//卡片必须是镶嵌型卡片
	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(pGoods->GetBasePropertiesIndex());
	if(pProperty /*&& pProperty->GetGoodsType()==CGoodsBaseProperties::GT_CARD*/)
	{
		//参数nIndex的有效范围
		if(nIndex>=(int)GetMaxEnchaseHoleNum())
			return false;
		//孔上没有镶嵌卡片
		if(m_pEnchaseHoleArray[nIndex]->GetCard())
			return false;
		//等级匹配
		long lCardLevelLimit=pGoods->GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT,1);
		long lEquipLevelLimit=GetAddonPropertyValues(GAP_ROLE_LEVEL_LIMIT,1);
		if(lCardLevelLimit>lEquipLevelLimit)
			return false;
		//同一ID卡片镶嵌数量限制
		long lNumLimit=pGoods->GetAddonPropertyValues(GAP_CARD_ENCHASE_NUM_LIMIT,1);
		ulong dwEnchaseNum=GetEnchaseNumByGoodsId(pGoods->GetBasePropertiesIndex());
		if(lNumLimit>0)
		{
		if( (long)(dwEnchaseNum+1) >lNumLimit)
			return false;
		}
		bool bRet=m_pEnchaseHoleArray[nIndex]->EnchaseCard(pPlayer,pGoods);
		if(!bRet)
			return false;
		//目前套卡激活
		if(m_dwActivedKitCardId>0)
		{
			CKitCardRuleFactory::KitCardInvalidate(pPlayer,this,m_dwActivedKitCardId);
			SetActivedKitCardId(0);
		}
		CKitCardRuleFactory::KitCardActive(pPlayer,this);
		//属性影响			
		return true;
	}
	return false;
}

bool CGoods::RemoveCard(CPlayer* pPlayer,int nIndex,bool bReserve)
{
	ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	if(nIndex<0 || nIndex>=(int)dwMaxHoleNum)
		return false;
	bool bRet=m_pEnchaseHoleArray[nIndex]->RemoveCard(pPlayer,bReserve);
	if(m_dwActivedKitCardId>0)
	{
		CKitCardRuleFactory::KitCardInvalidate(pPlayer,this,m_dwActivedKitCardId);
		SetActivedKitCardId(0);
	}
	CKitCardRuleFactory::KitCardActive(pPlayer,this);
	return bRet;
	
}

void CGoods::EnchaseAttrUpdate(CPlayer* pPlayer,CGoods* pCard)
{	
	vector<CGoods::tagAddonProperty>::iterator tagCardIter=pCard->GetAddonProperties()->begin();
	CGoodsBaseProperties* pEquipBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	for(;tagCardIter!=pCard->GetAddonProperties()->end();tagCardIter++)
	{	
		if(tagCardIter->gapType==GAP_PARTICULAR_ATTRIBUTE)
			continue;
		if(tagCardIter->gapType==GAP_MIN_REINFORCE_LEVEL || 
			tagCardIter->gapType==GAP_MAX_REINFORCE_LEVEL)
			continue;	
		if(tagCardIter->gapType==GAP_COLOR)
			continue;
		if(tagCardIter->gapType==GAP_QUALITY)
			continue;
		//该属性在装备上是否存在
		bool bBasePropertyExist=pEquipBaseProperty->IsHasAddonPropertie(tagCardIter->gapType);
		//装备上是否有该属性
		bool bExist=HasAddonProperty(tagCardIter->gapType);		

		if(bExist)
		{
			//已经有该属性，直接更新				
			ChangeAttribute(tagCardIter->gapType,tagCardIter->lValues[0],1);
		}
		else
		{
			//不存在该属性，添加			
			AddAttribute(tagCardIter->gapType,tagCardIter->lValues[0],1);
		}
	}
	if(pPlayer)
		UpdateAttribute(pPlayer->GetExID());
}

void CGoods::EnchaseAttrRemove(CPlayer* pPlayer,CGoods* pCard)
{	
	vector<CGoods::tagAddonProperty>::iterator tagCardIter=pCard->GetAddonProperties()->begin();
	CGoodsBaseProperties* pEquipBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	for(;tagCardIter!=pCard->GetAddonProperties()->end();tagCardIter++)
	{				
		//装备上是否有该属性
		if(tagCardIter->gapType==GAP_PARTICULAR_ATTRIBUTE)
			continue;
		if(tagCardIter->gapType==GAP_MIN_REINFORCE_LEVEL || 
			tagCardIter->gapType==GAP_MAX_REINFORCE_LEVEL)
			continue;
		if(tagCardIter->gapType==GAP_COLOR)
			continue;
		if(tagCardIter->gapType==GAP_QUALITY)
			continue;
		

		bool bExist=HasAddonProperty(tagCardIter->gapType);		
		if(bExist)
		{
			//已经有该属性，直接更新
			ChangeAttribute(tagCardIter->gapType,-(tagCardIter->lValues[0]),1);
		}		
	}
	UpdateAttribute(pPlayer->GetExID());
}

ulong CGoods::GetEnchaseNumByGoodsId(ulong dwId)
{
	ulong dwRet=0;
	long lVal=GetMaxEnchaseHoleNum();
	for(int i=0;i<lVal;i++)
	{
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard && pCard->GetBasePropertiesIndex()==dwId)
			dwRet++;
	}
	return dwRet;
}

//是否可以重叠
bool CGoods::CanStacked()
{
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	ulong dwResult = false;
	if( pProperties && pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE ||
		pProperties -> GetGoodsBaseType() == GBT_USELESS )
	{
		return pProperties->IsHasAddonPropertie(GAP_MAXOVERLAP);
	}	
	return false;
}

bool CGoods::IsSuitGoods()
{
	return HasAddonPropertyValues(GAP_SUITID);
}

void CGoods::GetValidAddonProperties(vector<eGoodsAddonProperties>& vOut)
{
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		vOut.push_back( m_vAddonProperties[i].gapType );		
	}

	if( vOut.size() == 0 )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE )
			{
				pProperties -> GetValidAddonProperties( vOut );
			}
		}
	}
}


void CGoods::GetEnabledAddonProperties(vector<eGoodsAddonProperties>& vOut)
{	
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		vOut.push_back( m_vAddonProperties[i].gapType );	
	}

	if( vOut.size() == 0 )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE )
			{
				pProperties -> GetValidAddonProperties( vOut );
			}
		}
	}
}


void CGoods::GetAddonPropertyValues( eGoodsAddonProperties gapType, 
									vector<long>& vOut )
{	
	//首先在自身找找
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			vOut.push_back(m_vAddonProperties[i].lValues[0]);
			vOut.push_back(m_vAddonProperties[i].lValues[1]);			
			break;
		}
	}
	//如果自身没有附带任何属性，尝试从GoodsSetup里面找找。
	if( vOut.size() == 0 )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			GetBasePropertiesIndex() );
		if( pProperties )
		{
			vector<CGoodsBaseProperties::tagAddonPropertyValue> vValues;
			pProperties -> GetAddonPropertyValues( gapType, vValues );
			for( size_t i = 0; i < vValues.size(); i ++ )
			{
				//只萃取出基准值。				
				vOut.push_back( vValues[i].lBaseValue );
			}
		}
	}
}

long CGoods::GetAddonPropertyValues(eGoodsAddonProperties gapType, ulong dwID)
{
	long lResult	= 0;
	if(dwID!=1 && dwID!=2)
		return lResult;	
	bool bFounded	= false;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )//--GAP_FREEZE,冻结 134
		{
			lResult=m_vAddonProperties[i].lValues[dwID-1];
			bFounded=true;
			break;			
		}
	}

	if( bFounded == false )
	{
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
			GetBasePropertiesIndex() );
		if( pProperties )
		{
			vector<CGoodsBaseProperties::tagAddonPropertyValue> vValues;
			pProperties -> GetAddonPropertyValues( gapType, vValues );
			for( size_t i = 0; i < vValues.size(); i ++ )
			{
				if( dwID == vValues[i].dwId )
				{
					//##只萃取出基准值。
					lResult		= vValues[i].lBaseValue;
					bFounded	= true;
				}
			}
		}
	}

	return lResult;
}

bool CGoods::HasAddonPropertyValues(eGoodsAddonProperties gapType)
{
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
		GetBasePropertiesIndex() );
	if( pProperties )
	{
		vector<CGoodsBaseProperties::tagAddonPropertyValue> vValues;
		pProperties -> GetAddonPropertyValues( gapType, vValues );
		if (vValues.size())
			return true;
	}
	return false;
}

bool CGoods::HasAddonProperty(eGoodsAddonProperties gapType)
{
	vector<CGoods::tagAddonProperty>::iterator propertyIter=m_vAddonProperties.begin();
	for(;propertyIter!=m_vAddonProperties.end();propertyIter++)
	{
		if(propertyIter->gapType==gapType)
			return true;
	}
	return false;
}

bool CGoods::SetAddonPropertyBaseValues(eGoodsAddonProperties gapType, ulong dwID, long lValue)
{
	bool bResult = false;
	if(dwID!=1 && dwID!=2)
		return bResult;

	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			m_vAddonProperties[i].lValues[dwID-1]=lValue;
			bResult=true;
			break;			
		}
	}
	return bResult;
}

bool CGoods::AddAddonPropertyBaseValues(eGoodsAddonProperties gapType,ulong dwID,long lVal1,long lVal2,bool bEnable,bool bIsImplicit)
{	
	if(dwID!=1 && dwID!=2)
		return false;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			return false;		
		}
	}
	CGoods::tagAddonProperty stPro;
	stPro.gapType=gapType;
	stPro.lValues[dwID-1]=lVal1;	
	m_vAddonProperties.push_back(stPro);
	return true;
}

void CGoods::AI()
{
}

bool CGoods::Clone(CBaseObject* pObject)
{	
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		vector<uchar> vData;
		long lOffset = 0;

		Serialize( &vData );
		pGoods -> Unserialize( &vData[0], lOffset );

		return true;
	}
	return false;
}

bool CGoods::Serialize(vector<uchar>* pStream, bool b)
{
	CShape::AddToByteArray( pStream, b==false?false:true );

	_AddToByteArray( pStream, m_dwBasePropertiesIndex );
	_AddToByteArray( pStream, m_dwAmount );
	_AddToByteArray( pStream, const_cast<char*>( m_strMakerName.c_str() ) );		

	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return false;
	ulong dwAddonNum=0;
	ulong dwPropertyId=0;
	vector<uchar> vecAddonData;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		if(pBaseProperty->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
		{			
			dwPropertyId=m_vAddonProperties[i].gapType;
			if(GoodsSetup::s_GoodsAttrDBSetup[dwPropertyId][0]==1)
			{
				dwAddonNum++;
				m_vAddonProperties[i].Serialize(&vecAddonData);
			}
		}
	}
	_AddToByteArray( pStream,dwAddonNum);
	if(dwAddonNum>0)
	{
		_AddToByteArray(pStream,&vecAddonData[0],(long)vecAddonData.size());
	}
	

	EnchaseArrange();
	//enchase data	
	
	_AddToByteArray(pStream,m_dwMaxHoleNum);
	//激活孔数量
	_AddToByteArray( pStream, GetActivedHoleNum());
	_AddToByteArray( pStream, m_dwActivedKitCardId);
	for(int i=0;i<(int)m_dwMaxHoleNum;i++)
	{
		//状态
		//王晓轩：所有镶嵌孔设置为开启
		_AddToByteArray(pStream,uchar(1));
		//if(m_pEnchaseHoleArray[i]->GetState())
		//{
		//	_AddToByteArray(pStream,uchar(1));
		//}
		//else
		//{
		//	_AddToByteArray(pStream,uchar(0));
		//}
		//CARD ID
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard)
		{
			_AddToByteArray(pStream,pCard->GetBasePropertiesIndex());
			pCard->Serialize(pStream);
		}
		else
		{
			_AddToByteArray(pStream,ulong(0));
		}
	}
	return true;
}

bool CGoods::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	Release();

	char szString[256];

	CShape::DecordFromByteArray( pStream, lOffset, b==false?false:true );

	m_dwBasePropertiesIndex	= _GetDwordFromByteArray( pStream, lOffset );
	m_dwAmount				= _GetDwordFromByteArray( pStream, lOffset );

	//##获取物品描述
	ZeroMemory( szString, 256 );
	_GetStringFromByteArray( pStream, lOffset, szString );
	m_strMakerName			= szString;


	m_vAddonProperties.clear();
	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return false;	

	CGoodsFactory::CreateAddonProperties(this);
	CGoodsFactory::CreateSuitProperties(this);

	//##获取物品扩展属性
	ulong dwNumProperties	= _GetDwordFromByteArray( pStream, lOffset );
	for( ulong i = 0; i < dwNumProperties; i ++ )
	{
		CGoods::tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( pStream, lOffset );
		if(pBaseProperty->IsHasAddonPropertie(apAddonProperty.gapType))
		{			
			for(int i=0;i< (int)m_vAddonProperties.size();i++)
			{
				if(m_vAddonProperties[i].gapType==apAddonProperty.gapType)
				{
					m_vAddonProperties[i].lValues[0]=apAddonProperty.lValues[0];
					m_vAddonProperties[i].lValues[1]=apAddonProperty.lValues[1];
				}
			}			
		}
	}

	InitEnchaseHole();


	ulong dwMaxHoleNum=_GetDwordFromByteArray(pStream,lOffset);
	ulong dwActivedHoleNum=_GetDwordFromByteArray( pStream, lOffset );
	m_dwActivedKitCardId=_GetDwordFromByteArray( pStream, lOffset );
	for(int i=0;i< (int)dwMaxHoleNum;i++)
	{		
		uchar btState=_GetByteFromByteArray(pStream,lOffset);
		
		//王晓轩：所有镶嵌孔设置为开启
		m_pEnchaseHoleArray[i]->SetState(true);
		//if(btState==1)
		//{
		//	if( i< m_dwMaxHoleNum)
		//		m_pEnchaseHoleArray[i]->SetState(true);
		//	//ActiveEnchaseHole((CPlayer*)NULL);
		//}
		//else
		//{
		//	if( i< m_dwMaxHoleNum)
		//		m_pEnchaseHoleArray[i]->SetState(false);
		//}
		//CARD ID
		ulong dwGoodsId=_GetDwordFromByteArray(pStream,lOffset);		

		if(dwGoodsId>0)
		{
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,40 );
			//bool bRet=Enchase(NULL,pGoods,i);
			if( i< (int)m_dwMaxHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
		}
		else
		{
			
		}
	}
	return true;
}

bool CGoods::SerializeForOldClient(vector<uchar>* pStream, bool b)
{
	bool bResult = false;
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
		GetBasePropertiesIndex() );
	if( pProperties )
	{
		_AddToByteArray( pStream, GetBasePropertiesIndex() );
		_AddToByteArray( pStream, GetExID() );
		_AddToByteArray( pStream, GetAmount() );
		_AddToByteArray( pStream, const_cast<char*>( GetMakerName() ) );		
		_AddToByteArray( pStream, GetPrice() );	
		_AddToByteArray(pStream,GetSilverPrice());
		_AddToByteArray(pStream,m_dwBuyPrice);
		_AddToByteArray(pStream,(ulong)GetGoodsBaseType());
		

		CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
		if(!pBaseProperty)
			return false;
		long lAddonNum=0;
		ulong dwPropertyId=0;

		vector<uchar> vecAddonData;
		for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
		{		
			if(pBaseProperty->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
			{
				dwPropertyId=m_vAddonProperties[i].gapType;
				//if(GoodsSetup::s_GoodsAttrDBSetup[dwPropertyId][1]==1)
				//{
					lAddonNum++;
					_AddToByteArray( &vecAddonData, static_cast<WORD>(m_vAddonProperties[i].gapType) );
					_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[0] );
					_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[1] );
				//}
			}
			else
			{
				lAddonNum++;
				_AddToByteArray( &vecAddonData, static_cast<WORD>(m_vAddonProperties[i].gapType) );
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[0] );
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[1] );
			}
		}
		_AddToByteArray( pStream,lAddonNum);
		if(lAddonNum>0)
		{
			_AddToByteArray(pStream,&vecAddonData[0],(long)vecAddonData.size());
		}
		/////////////////
		
		_AddToByteArray( pStream, GetTileX() );
		_AddToByteArray( pStream, GetTileY() );

		EnchaseArrange();

		//enchase data	
		ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
		if(dwMaxHoleNum>0)
		{
			//最大孔数
			_AddToByteArray(pStream,dwMaxHoleNum);
			//激活孔数量
			//王晓轩：默认所有孔都开启，所以删除此项数据
			//_AddToByteArray( pStream, GetActivedHoleNum());
			for(int i=0;i<(int)dwMaxHoleNum;i++)
			{		
				//CARD ID
				CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
				if(pCard)
				{
					_AddToByteArray(pStream,pCard->GetBasePropertiesIndex());
					pCard->SerializeForOldClient(pStream);
				}
				else
				{
					_AddToByteArray(pStream,ulong(0));
				}
			}
		}

		bResult = true;
	}
	return bResult;
}

bool CGoods::Serialize(DBWriteSet& setWriteDB, bool b)
{
	CShape::CodeToDataBlock( setWriteDB, b==false?false:true );

	setWriteDB.AddToByteArray( m_dwBasePropertiesIndex );
	setWriteDB.AddToByteArray( m_dwAmount );
	//setWriteDB.AddToByteArray( m_dwPrice );
	//setWriteDB.AddToByteArray( m_dwBuyPrice);
	//setWriteDB.AddToByteArray( (ulong)m_GoodsBaseType);
	//专业制造者名字
	setWriteDB.AddToByteArray( const_cast<char*>( m_strMakerName.c_str()));		

	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return false;
	ulong dwAddonNum=0;
	ulong dwPropertyId=0;
	vector<uchar> vecAddonData;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		if(pBaseProperty->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
		{			
			dwPropertyId=m_vAddonProperties[i].gapType;
			if(GoodsSetup::s_GoodsAttrDBSetup[dwPropertyId][0]==1)
			{
				dwAddonNum++;
				m_vAddonProperties[i].Serialize(&vecAddonData);
			}
		}
	}
	setWriteDB.AddToByteArray(dwAddonNum);
	if(dwAddonNum>0)
	{
		setWriteDB.AddToByteArray(&vecAddonData[0],(long)vecAddonData.size());
	}

	EnchaseArrange();
	//enchase data	
	
	setWriteDB.AddToByteArray(m_dwMaxHoleNum);
	//激活孔数量
	setWriteDB.AddToByteArray( GetActivedHoleNum());
	setWriteDB.AddToByteArray( m_dwActivedKitCardId);
	for(int i=0;i< (int)m_dwMaxHoleNum;i++)
	{
		//状态
		//王晓轩：所有镶嵌孔设置为开启
		setWriteDB.AddToByteArray(uchar(1));
		//if(m_pEnchaseHoleArray[i]->GetState())
		//{
		//	setWriteDB.AddToByteArray(uchar(1));
		//}
		//else
		//{
		//	setWriteDB.AddToByteArray(uchar(0));
		//}
		//CARD ID
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard)
		{
			setWriteDB.AddToByteArray(pCard->GetBasePropertiesIndex());
			pCard->Serialize(setWriteDB);
		}
		else
		{
			setWriteDB.AddToByteArray(ulong(0));
		}
	}
	return true;
}
bool CGoods::Unserialize(DBReadSet& setReadDB, bool b)
{
	Release();
	char szString[256];
	CShape::DecodeFromDataBlock( setReadDB, b==false?false:true );

	m_dwBasePropertiesIndex	= setReadDB.GetDwordFromByteArray(  );
	m_dwAmount				= setReadDB.GetDwordFromByteArray(  );

	//##获取物品描述
	ZeroMemory( szString, 256 );
	setReadDB.GetStringFromByteArray( szString ,256);
	m_strMakerName			= szString;


	m_vAddonProperties.clear();
	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return false;	

	CGoodsFactory::CreateAddonProperties(this);
	CGoodsFactory::CreateSuitProperties(this);

	//##获取物品扩展属性
	ulong dwNumProperties	= setReadDB.GetDwordFromByteArray(  );
	for( ulong i = 0; i < dwNumProperties; i ++ )
	{
		CGoods::tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( setReadDB );
		if(pBaseProperty->IsHasAddonPropertie(apAddonProperty.gapType))
		{			
			for(int i=0;i<(int)m_vAddonProperties.size();i++)
			{
				if(m_vAddonProperties[i].gapType==apAddonProperty.gapType)
				{
					m_vAddonProperties[i].lValues[0]=apAddonProperty.lValues[0];
					m_vAddonProperties[i].lValues[1]=apAddonProperty.lValues[1];
				}
			}			
		}
	}

	InitEnchaseHole();

	ulong dwMaxHoleNum=setReadDB.GetDwordFromByteArray();
	ulong dwActivedHoleNum=setReadDB.GetDwordFromByteArray(  );
	m_dwActivedKitCardId=setReadDB.GetDwordFromByteArray(  );
	for(int i=0;i<(int)dwMaxHoleNum;i++)
	{		
		uchar btState=setReadDB.GetByteFromByteArray();
		
		//王晓轩：所有镶嵌孔设置为开启
		m_pEnchaseHoleArray[i]->SetState(true);
		//if(btState==1)
		//{
		//	if( i< m_dwMaxHoleNum)
		//		m_pEnchaseHoleArray[i]->SetState(true);
		//}
		//else
		//{
		//	if( i< m_dwMaxHoleNum)
		//		m_pEnchaseHoleArray[i]->SetState(false);
		//}
		//CARD ID
		ulong dwGoodsId=setReadDB.GetDwordFromByteArray( );		

		if(dwGoodsId>0)
		{
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB ,41);
			if( i< (int)m_dwMaxHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
			else
			{
				Log4c::Warn(ROOT_MODULE,FormatText("GS_GOODS_90",i,m_dwMaxHoleNum,dwMaxHoleNum));//解码镶嵌异常i=%d,m_dwMaxHoleNum=%d,dwMaxHoleNum=%d
				CGoodsFactory::GarbageCollect(&pGoods,41);
			}
		}
		else
		{
			
		}
	}
	return true;
}

bool CGoods::SerializeForRgnDisplay(DBWriteSet& setWriteDB,bool b)
{
	bool bResult=false;
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
		GetBasePropertiesIndex() );
	if( pProperties )
	{
		setWriteDB.AddToByteArray( GetBasePropertiesIndex() );		
		setWriteDB.AddToByteArray( GetAmount() );
		setWriteDB.AddToByteArray(GetPosX());
		setWriteDB.AddToByteArray(GetPosY());
		long lLev=GetAddonPropertyValues(GAP_WEAPON_LEVEL,1);
		setWriteDB.AddToByteArray(lLev);
		bResult=true;
	}
	return bResult;
}

bool CGoods::SerializeForOldClient(DBWriteSet& setWriteDB, bool b)
{
	bool bResult = false;
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties(
		GetBasePropertiesIndex() );
	if( pProperties )
	{
		setWriteDB.AddToByteArray( GetBasePropertiesIndex() );
		setWriteDB.AddToByteArray( GetExID() );
		setWriteDB.AddToByteArray( GetAmount() );
		setWriteDB.AddToByteArray( const_cast<char*>( GetMakerName() ) );		
		setWriteDB.AddToByteArray( GetPrice() );	
		setWriteDB.AddToByteArray( GetSilverPrice());
		setWriteDB.AddToByteArray( m_dwBuyPrice);
		setWriteDB.AddToByteArray( (ulong)GetGoodsBaseType());		

		long lAddonNum=0;
		ulong dwPropertyId=0;

		vector<uchar> vecAddonData;
		for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
		{		
			if(pProperties->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
			{
				dwPropertyId=m_vAddonProperties[i].gapType;				
				lAddonNum++;
				_AddToByteArray( &vecAddonData, static_cast<WORD>(m_vAddonProperties[i].gapType) );
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[0]);
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[1]);
			}
			else
			{
				lAddonNum++;
				_AddToByteArray( &vecAddonData, static_cast<WORD>(m_vAddonProperties[i].gapType) );
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[0]);
				_AddToByteArray( &vecAddonData, m_vAddonProperties[i].lValues[1]);
			}
		}
		setWriteDB.AddToByteArray(lAddonNum);
		if(lAddonNum>0)
		{
			setWriteDB.AddToByteArray( &vecAddonData[0],(long)vecAddonData.size());
		}
		/////////////////
		
		setWriteDB.AddToByteArray( GetTileX() );
		setWriteDB.AddToByteArray( GetTileY() );

		EnchaseArrange();
		//enchase data	
		ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
		if(dwMaxHoleNum>0)
		{
			//最大孔数
			setWriteDB.AddToByteArray( dwMaxHoleNum);
			//激活孔数量
			//王晓轩：默认所有孔都开启，所以删除此项数据
			//setWriteDB.AddToByteArray( GetActivedHoleNum());
			for(int i=0;i<(int)dwMaxHoleNum;i++)
			{		
				//CARD ID
				CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
				if(pCard)
				{
					setWriteDB.AddToByteArray( pCard->GetBasePropertiesIndex());
					pCard->SerializeForOldClient(setWriteDB);
				}
				else
				{
					setWriteDB.AddToByteArray(ulong(0));
				}
			}
		}

		bResult = true;
	}
	return bResult;
}

bool CGoods::AddonPropertyUpdate(eGoodsAddonProperties gapProperty,eGoodsAddonProperties gapRange)
{
	bool bPropertyExist=HasAddonProperty(gapProperty);
	bool bRangeExist=HasAddonProperty(gapRange);
	if(bPropertyExist && bRangeExist)
	{
		long lRange1=GetAddonPropertyValues(gapRange,1);
		long lRange2=GetAddonPropertyValues(gapRange,2);
		if(lRange1>lRange2)
			return false;
		int ranVal=random(lRange2-lRange1+1);
		ranVal+=lRange1;
		ChangeAttribute(gapProperty,ranVal,1,true);
		return true;
	}
	return false;
}

void CGoods::MountBind(const CGUID& playerID)
{	
	if(!GoodsExManage::GetInstance().IsBind(this))
	{
		long lBindType = GetAddonPropertyValues(GAP_BIND, 1);

		if(eBT_DressAndCantUntie == lBindType)//! 装备即绑定（且不能解绑）
		{
			GoodsExManage::GetInstance().SetGoodsBindProperty_System(this,eBT_CantUntie,playerID);
		}
		else if(eBT_DressAndBind == lBindType)//! 装备即绑定（可以解绑）
		{
			GoodsExManage::GetInstance().SetGoodsBindProperty_System(this,eBT_Bind,playerID);
		}
	}	
}

bool CGoods::DelAddonPropertyBaseValues(eGoodsAddonProperties gapType)
{
	vector<CGoods::tagAddonProperty>::iterator proIter=m_vAddonProperties.begin();
	for(; proIter!=m_vAddonProperties.end(); proIter ++ )
	{
		if( proIter->gapType== gapType )
		{
			break;
		}
	}
	if(proIter!=m_vAddonProperties.end())
	{
		m_vAddonProperties.erase(proIter);
		return true;
	}
	return false;
}

bool CGoods::DelAddonPropertyNotInBase(void)
{
	CGoodsBaseProperties* pBaseProperty= GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return false;
	vector<CGoods::tagAddonProperty>::iterator proIter=m_vAddonProperties.begin();
	for(; proIter!=m_vAddonProperties.end();)
	{
		if(!pBaseProperty->IsHasAddonPropertie(proIter->gapType))
		{
			//物编属性查不到
			proIter=m_vAddonProperties.erase(proIter);
		}
		else
		{
			proIter ++;
		}
	}
	
	return true;
}

const char* CGoods::ToString()
{
	return (const char*)0;
}

void CGoods::Release()
{
	EnchaseRelease();
	m_dwBasePropertiesIndex	= 0;
	m_dwAmount				= 0;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		m_vAddonProperties[i].Clear();
	}
	m_vAddonProperties.clear();
}

void CGoods::SetBasePropertiesIndex(ulong dwID)
{
	m_dwBasePropertiesIndex	= dwID;
}

CGoods::tagAddonProperty::tagAddonProperty()
{
	gapType					= GAP_UNKNOW;	
	ZeroMemory(lValues,2*sizeof(long));
}

CGoods::tagAddonProperty::~tagAddonProperty()
{
	Clear();
}

void CGoods::tagAddonProperty::Clear()
{
	gapType					= GAP_UNKNOW;
	ZeroMemory(lValues,2*sizeof(long));
}

bool CGoods::tagAddonProperty::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray( pStream, static_cast<ulong>(gapType) );
	_AddToByteArray(pStream,lValues[0]);
	_AddToByteArray(pStream,lValues[1]);	
	return true;
}

bool CGoods::tagAddonProperty::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	gapType					= static_cast<eGoodsAddonProperties>( 
		_GetDwordFromByteArray(pStream, lOffset) );	
	lValues[0]=_GetLongFromByteArray(pStream,lOffset);
	lValues[1]=_GetLongFromByteArray(pStream,lOffset);
	return true;
}

bool CGoods::tagAddonProperty::Serialize(DBWriteSet& setWriteDB, bool b)
{
	setWriteDB.AddToByteArray( static_cast<ulong>(gapType) );
	setWriteDB.AddToByteArray(lValues[0]);
	setWriteDB.AddToByteArray(lValues[1]);
	return true;
}
bool CGoods::tagAddonProperty::Unserialize(DBReadSet& setReadDB, bool b)
{
	gapType					= static_cast<eGoodsAddonProperties>( 
		setReadDB.GetDwordFromByteArray() );
	lValues[0]=setReadDB.GetLongFromByteArray();
	lValues[1]=setReadDB.GetLongFromByteArray();
	return true;
}

ulong CGoods::GetPrice()
{
	return m_baseProperties->GetPrice();
}

ulong CGoods::GetTotalPrice()
{
	return m_baseProperties->GetPrice() * m_dwAmount;
}


void CGoods::SetGoodsName(const char* strGoodsName)
{
	CShape::SetName( strGoodsName );
}

const char* CGoods::GetGoodsName()
{
	return CShape::GetName();
}

void CGoods::UpdateScriptAddonDesc( const CGUID &playerID )
{
	CMessage msg( MSG_S2C_GOODS_UPDATE_SCRIPTDESC );
	msg.Add( GetExID() );
	msg.Add( GetScriptAddonDesc() );
	msg.SendToPlayer( playerID );
}

void CGoods::AddRefine(long lVal)
{
	if(HasAddonProperty(GAP_REFINE))
	{
		long lCurVal=GetAddonPropertyValues(GAP_REFINE,1);
		long lMaxVal=GetAddonPropertyValues(GAP_REFINE,2);
		lCurVal+=lVal;
		if(lCurVal<0)
			lCurVal=0;
		else if(lCurVal>lMaxVal)
			lCurVal=lMaxVal;
		SetAddonPropertyBaseValues(GAP_REFINE,1,lCurVal);
	}
}

/// 获取物品装备位置栏位
eEquipType CGoods::GetEquipType(void)
{
	return (eEquipType)GetAddonPropertyValues(GAP_EQUIP_CATEGORY, 1);
}

/// 获取物品法宝类型
eTrumpType CGoods::GetTrumpType(void)
{
	return (eTrumpType)GetAddonPropertyValues(GAP_TRUMP_CATEGORY, 1);
}

//bool CGoods::ImmunityUpdate()
//{
//	return AddonPropertyUpdate(CGoodsBaseProperties::GAP_EXCUSE_HURT,CGoodsBaseProperties::GAP_EXCUSE_HURT_RANGE);
//}

////穿透更新
//bool CGoods::PierceUpdate()
//{
//	return AddonPropertyUpdate(CGoodsBaseProperties::GAP_PENETRATE,CGoodsBaseProperties::GAP_PENETRATE_RANGE);
//}

////幸运更新
//bool CGoods::LuckyUpdate()
//{
//	return AddonPropertyUpdate(CGoodsBaseProperties::GAP_LUCKY,CGoodsBaseProperties::GAP_LUCKY_RANGE);
//}

////星座更新
//bool CGoods::ConstellationUpdate()
//{
//	bool bExist=HasAddonProperty(CGoodsBaseProperties::GAP_CONSTELLATION);
//	if(bExist)
//	{
//		long lPropertyVal=GetAddonPropertyValues(CGoodsBaseProperties::GAP_CONSTELLATION,1);
//		if(lPropertyVal!=0)
//			return false;
//		int nRanVal=random(12);
//		nRanVal++;
//		ChangeAttribute(CGoodsBaseProperties::GAP_CONSTELLATION,nRanVal,1,true);
//		return true;
//	}
//	return false;
//}

////hp上限追加范围
//bool CGoods::AddonHpUpdate()
//{
//	return AddonPropertyUpdate(CGoodsBaseProperties::GAP_RANDOM_ADDON_MAXHP,CGoodsBaseProperties::GAP_ADDON_HP_RANGE);
//}

//void CGoods::AtkEDUpdate()
//{
//	//获当前攻击ED
//	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
//	if(!pBaseProperty)
//		return;
//	long lCurAtkED=GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_ATK_ED,1);
//
//	if(lCurAtkED>=0)
//	{
//		float fRate=(float)lCurAtkED/100.0f;
//		//基本最小攻击		
//		long lMinAtk=pBaseProperty->GetAddonPropertyValue(CGoodsBaseProperties::GAP_MIN_ATTACK,1);
//		//基本最大攻击
//		long lMaxAtk=pBaseProperty->GetAddonPropertyValue(CGoodsBaseProperties::GAP_MAX_ATTACK,1);
//		//基本魔法攻击
//		long lMAtk=pBaseProperty->GetAddonPropertyValue(CGoodsBaseProperties::GAP_FUMO_PROPERTY,1);
//		float fMinAtkChg=lMinAtk*fRate;
//		float fMaxAtkChg=lMaxAtk*fRate;
//		float fMAtkChg=lMAtk*fRate;
//		ChangeAttribute(CGoodsBaseProperties::GAP_MIN_ATTACK,ulong(0.5f+lMinAtk+fMinAtkChg),1, true);
//		ChangeAttribute(CGoodsBaseProperties::GAP_MAX_ATTACK,ulong(0.5f+lMaxAtk+fMaxAtkChg),1, true);
//		ChangeAttribute(CGoodsBaseProperties::GAP_FUMO_PROPERTY,ulong(0.5f+lMAtk+fMAtkChg) ,1, true);
//	}
//}

//void CGoods::DefEDUpdate()
//{
//	//获当前攻击ED
//	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
//	if(!pBaseProperty)
//		return;
//	long lCurDefED=GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_DEF_ED,1);
//	if(lCurDefED>=0)
//	{
//		float fRate=(float)lCurDefED/100.0f;
//		//基本防御
//		long lDef=pBaseProperty->GetAddonPropertyValue(CGoodsBaseProperties::GAP_NOR_DEF,1);
//		//基本魔防
//		long lMDef=pBaseProperty->GetAddonPropertyValue(CGoodsBaseProperties::GAP_MAGIC_DEF,1);
//		
//		float fDefChg=lDef*fRate;
//		float fMDefChg=lMDef*fRate;
//
//		ChangeAttribute(CGoodsBaseProperties::GAP_NOR_DEF,ulong(0.5f+lDef+fDefChg),1, true);
//		ChangeAttribute(CGoodsBaseProperties::GAP_MAGIC_DEF,ulong(0.5f+lMDef+fMDefChg),1, true);
//	}	
//}

//void CGoods::EDCalculate()
//{
//	//攻击ED
//	bool bAtkEdAttr=HasAddonProperty(CGoodsBaseProperties::GAP_ATK_ED);
//	bool bCurAtkEdAttr=HasAddonProperty(CGoodsBaseProperties::GAP_CUR_ATK_ED);
//	if(bAtkEdAttr && bCurAtkEdAttr)
//	{
//		long lAtkEdVal1=GetAddonPropertyValues(CGoodsBaseProperties::GAP_ATK_ED,1);
//		long lAtkEdVal2=GetAddonPropertyValues(CGoodsBaseProperties::GAP_ATK_ED,2);
//		if(lAtkEdVal1>lAtkEdVal2 || (lAtkEdVal2-lAtkEdVal1)<0)
//			return;
//		int ranVal=random(lAtkEdVal2 - lAtkEdVal1+1);		
//		//最终随机出来的百分比
//		ranVal+=lAtkEdVal1;
//		//更形当前ED提升百分比
//		ChangeAttribute(CGoodsBaseProperties::GAP_CUR_ATK_ED,ranVal,1,true);
//	}
//
//	//防御ED
//	bool bDefEdAttr=HasAddonProperty(CGoodsBaseProperties::GAP_DEF_ED);
//	bool bCurDefEdAttr=HasAddonProperty(CGoodsBaseProperties::GAP_CUR_DEF_ED);
//	if(bDefEdAttr && bCurDefEdAttr)
//	{
//		long lDefEdVal1=GetAddonPropertyValues(CGoodsBaseProperties::GAP_DEF_ED,1);
//		long lDefEdVal2=GetAddonPropertyValues(CGoodsBaseProperties::GAP_DEF_ED,2);
//		if(lDefEdVal1>lDefEdVal2 || (lDefEdVal2-lDefEdVal1)<0)
//			return;
//		int ranVal=random(lDefEdVal2 - lDefEdVal1+1);		
//		//最终随机出来的百分比
//		ranVal+=lDefEdVal1;
//		//更新当前ED提升百分比
//		ChangeAttribute(CGoodsBaseProperties::GAP_CUR_DEF_ED,ranVal,1, true);
//	}
//
//}

//void CGoods::CalculatePKCP()
//{
//	// 确认属性是否存在
//	bool bHasAddProper = HasAddonProperty( CGoodsBaseProperties::GAP_ADD_MAX_PKCP );
//	bool bHasAddRangeProper = HasAddonProperty( CGoodsBaseProperties::GAP_ADD_MAX_PKCPRANGE );
//	if( bHasAddProper && bHasAddRangeProper )
//	{
//		long lAddMin = GetAddonPropertyValues( CGoodsBaseProperties::GAP_ADD_MAX_PKCPRANGE, 1 );
//		long lAddMax = GetAddonPropertyValues( CGoodsBaseProperties::GAP_ADD_MAX_PKCPRANGE, 2 );
//		if( lAddMax > lAddMin )
//		{
//			long val = random( lAddMax - lAddMin + 1 ) + lAddMin;
//			ChangeAttribute( CGoodsBaseProperties::GAP_ADD_MAX_PKCP, val, 1, true );
//		}
//	}
//
//	// 倍率
//	bool bHasTimesProper = HasAddonProperty( CGoodsBaseProperties::GAP_TIMES_MAX_PKCP );
//	bool bHasTimesRangeProper = HasAddonProperty( CGoodsBaseProperties::GAP_TIMES_MAX_PKCPRANGE );
//	if( bHasTimesProper && bHasTimesRangeProper )
//	{
//		long lTimesMin = GetAddonPropertyValues( CGoodsBaseProperties::GAP_TIMES_MAX_PKCPRANGE, 1 );
//		long lTimesMax = GetAddonPropertyValues( CGoodsBaseProperties::GAP_TIMES_MAX_PKCPRANGE, 2 );
//		if( lTimesMax > lTimesMin )
//		{
//			long val = random( lTimesMax - lTimesMin + 1 ) + lTimesMin;
//			ChangeAttribute( CGoodsBaseProperties::GAP_TIMES_MAX_PKCP, val, 1, true );
//		}
//	}
//}