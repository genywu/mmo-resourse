#include "StdAfx.h"
#include "..\GameTest\resource.h"
#include ".\goods.h"
#include "other\goodslist.h"
//#include "..\Net\Message.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoods::CGoods(void)
{
	SetType(TYPE_GOODS);
	m_dwIndex=0;
	m_lNum=0;
	//m_bInTradePage=false;
	//m_bInPlayerShop = false;
	m_bHaveShadow = false;
	m_dwPurchaseLimit = 0;



	//m_dwNameColor=0;

	m_bDropEffect = FALSE;
	m_fDropHeight = 0.2f;

	m_wHoleNum = 0;

	// 初始 锁定计数为 0
	m_lLockCounter = 0;

	m_strManufacturerName = "";
}

CGoods::CGoods(const CGoods &srcGoods)
{
	GetEffect()->clear();
	m_wHoleNum = 0;
	m_mapHoleState.clear();
	m_InlayHole.clear();

	SetType(TYPE_GOODS);
	SetExID(srcGoods.m_guExID);
	m_dwIndex = srcGoods.m_dwIndex;
	m_lNum = srcGoods.m_lNum;
	//m_bInTradePage = srcGoods.m_bInTradePage;
	//m_bInPlayerShop = srcGoods.m_bInPlayerShop;
	m_wHoleNum = srcGoods.m_wHoleNum;
	if (m_wHoleNum>0&&!srcGoods.m_mapHoleState.empty())
	{
		map<WORD,bool>::const_iterator Iter = srcGoods.m_mapHoleState.begin();
		for (;Iter!=srcGoods.m_mapHoleState.end();Iter++)
		{
			m_mapHoleState[Iter->first] = Iter->second;
		}
	}
	m_bHaveShadow = srcGoods.m_bHaveShadow;
	m_lLockCounter = srcGoods.m_lLockCounter;
	m_dwPurchaseLimit = srcGoods.m_dwPurchaseLimit;


	m_bDropEffect = srcGoods.m_bDropEffect;
	m_fDropHeight = srcGoods.m_fDropHeight;
	m_stBaseProperty.strName = CGoodsList::GetProperty(m_dwIndex)->BaseProperty.strName;
	SetName(m_stBaseProperty.strName.c_str());
	m_stBaseProperty.dwBuyPrice = srcGoods.m_stBaseProperty.dwBuyPrice;
	m_stBaseProperty.dwValue = srcGoods.m_stBaseProperty.dwValue;
	m_stBaseProperty.dwSilverPrice = srcGoods.m_stBaseProperty.dwSilverPrice;
	m_stBaseProperty.dwType = CGoodsList::GetProperty(m_dwIndex)->BaseProperty.dwType;
	m_GoodsBaseType = static_cast<eGoodsBaseType>(srcGoods.m_stBaseProperty.dwType);
	if (!srcGoods.m_InlayHole.empty())
	{
		map<WORD,tagCardProperty>::const_iterator iter = srcGoods.m_InlayHole.begin();
		for (;iter!=srcGoods.m_InlayHole.end();iter++)
		{
			m_InlayHole[iter->first] = iter->second;
		}
	}
	vector<CGoodsList::tagEffect>::const_iterator EffectIter = srcGoods.m_vectorEffect.begin();
	for(;EffectIter!= srcGoods.m_vectorEffect.end();EffectIter++)
	{
		m_vectorEffect.push_back(*EffectIter);
	}

	m_strManufacturerName = srcGoods.m_strManufacturerName;
}

CGoods::CGoods(DWORD dwIndex)
{
	CGoodsList::tagGoods2* pTagGoods = CGoodsList::GetProperty(dwIndex);
	if( pTagGoods != NULL )
	{
		GetEffect()->clear();
		m_wHoleNum = 0;
		m_mapHoleState.clear();
		m_InlayHole.clear();

		SetType(TYPE_GOODS);
		SetExID(CGUID::GUID_INVALID);
		m_dwIndex = dwIndex;
		m_lNum = 0;
		//m_bInTradePage=false;
		//m_bInPlayerShop = false;
		m_bHaveShadow = false;
		m_dwPurchaseLimit = 0;
		m_wHoleNum = 0;
		m_lLockCounter = 0;

		m_bDropEffect = FALSE;
		m_fDropHeight = 0.2f;
		m_stBaseProperty.strName = pTagGoods->BaseProperty.strName;
		SetName(m_stBaseProperty.strName.c_str());
		m_stBaseProperty.dwValue = pTagGoods->BaseProperty.dwPrice;
		m_stBaseProperty.dwSilverPrice = pTagGoods->BaseProperty.dwPrice;
		m_stBaseProperty.dwType = pTagGoods->BaseProperty.dwType;
		m_GoodsBaseType = static_cast<eGoodsBaseType>(m_stBaseProperty.dwType);

		vector<CGoodsList::tagEffect>::const_iterator EffectIter = pTagGoods->vectorEffect.begin();
		for(;EffectIter!= pTagGoods->vectorEffect.end();EffectIter++)
		{
			m_vectorEffect.push_back(*EffectIter);
		}

		m_strManufacturerName = "";
	}
}

CGoods::~CGoods(void)
{
	m_vectorEffect.clear();
}
// 添加到CByteArray
bool CGoods::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CGoodsList::tagBaseProperty *pstGood=GetProperty();
	_AddToByteArray(pByteArray, GetIndex());//压入物品INDEX
	_AddToByteArray(pByteArray,GetExID());
	_AddToByteArray(pByteArray,GetNum());

	_AddToByteArray(pByteArray,(char *)GetName(),strlen(GetName()));
	_AddToByteArray(pByteArray,GetBaseProperty()->dwValue);
	_AddToByteArray(pByteArray,GetBaseProperty()->dwType);
	_AddToByteArray(pByteArray,GetEffectVal(37,2));

	_AddToByteArray(pByteArray,GetPosX());
	_AddToByteArray(pByteArray,GetPosY());

	return true;
}
//外部调用的刷新属性接口
void CGoods::UpdateProperty(CMessage* pMsg)
{
	long lNum = pMsg->GetLong();
	long pro_type = 0;
	long data_pos = 0;
	long data = 0;
	for( int i=0;i<lNum;i++)
	{
		pro_type = pMsg->GetLong();
		data_pos = pMsg->GetLong();
		data = pMsg->GetLong();
		SetNewEffectVal((WORD)pro_type,(int)data_pos,data);
	}
}
// 解码
bool CGoods::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	GetEffect()->clear();
	m_wHoleNum = 0;
	m_mapHoleState.clear();
	m_InlayHole.clear();

	if( !bExData )
	{
		SetIndex(_GetDwordFromByteArray(pByte,pos));
		SetNum(_GetLongFromByteArray(pByte,pos));
		SetPosX(_GetLongFromByteArray(pByte,pos));
		SetPosY(_GetLongFromByteArray(pByte,pos));
		CGoodsList::tagGoods2* pstGoods = CGoodsList::GetProperty(GetIndex());
		if( pstGoods )
		{
			m_stBaseProperty.strName = pstGoods->BaseProperty.strName;
			m_stBaseProperty.dwValue = pstGoods->BaseProperty.dwPrice;					//	金币价格
			m_stBaseProperty.dwSilverPrice = pstGoods->BaseProperty.dwSilverPrice;		// 银币价格
			m_stBaseProperty.dwBuyPrice = 0;											//	物品买入价格
			m_stBaseProperty.dwType = pstGoods->BaseProperty.dwType;
			// 先根据物编里的数据填充物品的附加属性列表
			vector<CGoodsList::tagEffect>::iterator EffectIter = pstGoods->vectorEffect.begin();
			for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
			{
				CGoodsList::tagEffect GoodsEffect;
				GoodsEffect.wType = EffectIter->wType;
				GoodsEffect.dwValue1 = EffectIter->dwValue1;
				GoodsEffect.dwValue2 = EffectIter->dwValue2;
				GoodsEffect.bEnable = false;
				GoodsEffect.bHide = false;
				GetEffect()->push_back(GoodsEffect);
			}
			SetNewEffectVal(CGoods::GAP_WEAPON_LEVEL,1,_GetLongFromByteArray(pByte, pos));
			//SetGraphicsID(GetProperty()->dwGroundId);
			m_stBaseProperty.dwType = GetProperty()->dwType;
			m_GoodsBaseType = static_cast<eGoodsBaseType>(m_stBaseProperty.dwType);
			SetName(m_stBaseProperty.strName.c_str());
			return true;
		}
		else	// 客户端goods.dat中没有这个物品，不加入进去
		{
			DWORD dwDur=_GetDwordFromByteArray(pByte,pos);
			long lEffectSize=_GetLongFromByteArray(pByte,pos);
			SetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,2,dwDur);
			SetPosX(_GetFloatFromByteArray(pByte, pos));
			SetPosY(_GetFloatFromByteArray(pByte, pos));

			char str[256];
			sprintf(str,"Goods.cpp 物品<%s>解码错误", m_stBaseProperty.strName.c_str());
			AfxMessageBox(str,MB_OK,NULL);
			return false;
		}
	}
	else
	{
		SetIndex(_GetDwordFromByteArray(pByte,pos));
		_GetBufferFromByteArray(pByte, pos,m_guExID);
		SetNum(_GetLongFromByteArray(pByte,pos));
		//制造者的名字 add by MartySa
		char str[256];
		_GetStringFromByteArray(pByte, pos, str);
		m_strManufacturerName = str;
		//	活动信息
		//_GetStringFromByteArray(pByte,pos,str);
		//m_strActiveInfor = str;
		//
		GetBaseProperty()->dwValue = _GetDwordFromByteArray(pByte,pos);		//	金币价格
		GetBaseProperty()->dwSilverPrice = _GetDwordFromByteArray(pByte,pos);		// 银币价格

		GetBaseProperty()->dwBuyPrice = _GetDwordFromByteArray(pByte,pos);	//	物品买入价格
		GetBaseProperty()->dwType = _GetDwordFromByteArray(pByte,pos);
		m_GoodsBaseType=static_cast<eGoodsBaseType>(GetBaseProperty()->dwType);

		CGoodsList::tagGoods2* pstGoods = CGoodsList::GetProperty(GetIndex());
		if(pstGoods)
		{
			// 先根据物编里的数据填充物品的附加属性列表
			vector<CGoodsList::tagEffect>::iterator EffectIter = pstGoods->vectorEffect.begin();
			for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
			{
				CGoodsList::tagEffect GoodsEffect;
				GoodsEffect.wType = EffectIter->wType;//_GetWordFromByteArray(pByte,pos);
				GoodsEffect.dwValue1 = EffectIter->dwValue1;//_GetLongFromByteArray(pByte,pos);
				GoodsEffect.dwValue2 = EffectIter->dwValue2;//_GetLongFromByteArray(pByte,pos);
				GoodsEffect.bEnable = false;
				GoodsEffect.bHide = false;
				GetEffect()->push_back(GoodsEffect);
			}

			// 根据服务器传过来的部分附加属性数据重新设置这部分数据（或者是相对与物编新增加的一些属性）
			long lEffectSize = _GetLongFromByteArray(pByte,pos);
			for (int i = 0;i<lEffectSize;i++)
			{
				WORD wEffectType = _GetWordFromByteArray(pByte,pos);
				long lValue1 = _GetLongFromByteArray(pByte,pos);
				long lValue2 = _GetLongFromByteArray(pByte,pos);
				SetNewEffectVal(wEffectType,1,lValue1);
				SetNewEffectVal(wEffectType,2,lValue2);
			}

			SetPosX(_GetLongFromByteArray(pByte, pos));
			SetPosY(_GetLongFromByteArray(pByte, pos));

			//		SetGraphicsID(GetProperty()->dwGroundId);
			m_stBaseProperty.strName = pstGoods->BaseProperty.strName;
			SetName(m_stBaseProperty.strName.c_str());
			long lBaseHoleNum = GetEffectVal(GAP_HOLE_NUM,2);
			if (lBaseHoleNum!=0)
			{
				DWORD dwCardNum = _GetDwordFromByteArray(pByte,pos);
				//m_wHoleNum = _GetDwordFromByteArray(pByte,pos);
				for (long i = 0;i<lBaseHoleNum;i++)
				{
					m_mapHoleState.insert(pair<WORD,bool>(i,false));
				}

				DWORD dwGoodsIndex = 0;
				CGoods* pCardGoods = NULL;
				for (int i = 0;i<dwCardNum;i++)
				{
					dwGoodsIndex = _GetDwordFromByteArray(pByte,pos);
					if (dwGoodsIndex!=0)
					{
						pCardGoods = new CGoods();
						pCardGoods->DecordFromByteArray(pByte,pos);
						tagCardProperty temp;
						temp.dwCardIndex = pCardGoods->GetIndex();
						temp.CardGUID = pCardGoods->GetExID();
						temp.dwCardType = pCardGoods->GetEffectVal(GAP_CARD_CATEGORY,1);
						temp.bMaxNum = pCardGoods->GetEffectVal(GAP_CARD_ENCHASE_NUM_LIMIT,1);
						temp.strCardName = pCardGoods->GetName();
						vector<CGoodsList::tagEffect>::iterator Iter = pCardGoods->GetEffect()->begin();
						for(;Iter!=pCardGoods->GetEffect()->end();Iter++)
						{
							temp.CardEffect.push_back(*Iter);
						}
						m_InlayHole.insert(pair<WORD,tagCardProperty>(i,temp));
						SAFE_DELETE(pCardGoods);
					}
				}
			}
			return true;
		}
		else	// 客户端goods.dat中没有这个物品，不加入进去
		{
			DWORD dwDur=_GetDwordFromByteArray(pByte,pos);
			long lEffectSize=_GetLongFromByteArray(pByte,pos);
			SetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,2,dwDur);
			SetPosX(_GetFloatFromByteArray(pByte, pos));
			SetPosY(_GetFloatFromByteArray(pByte, pos));

			char str[256];
			sprintf(str,"Goods.cpp 物品<%s>解码错误", m_stBaseProperty.strName.c_str());
			AfxMessageBox(str,MB_OK,NULL);
			return false;
		}
		return false;
	}
}

bool CGoods::DecordFromDataBlock(DBReadSet& rdb,bool bExData /* = true  */)
{
	GetEffect()->clear();
	m_wHoleNum = 0;
	m_mapHoleState.clear();
	m_InlayHole.clear();

	if( !bExData )
	{
		SetIndex(rdb.GetDwordFromByteArray());
		SetNum( rdb.GetLongFromByteArray() );
		SetPosX( rdb.GetFloatFromByteArray() );
		SetPosY( rdb.GetFloatFromByteArray() );
		CGoodsList::tagGoods2 * ptagGoods = CGoodsList::GetProperty( GetIndex() );
		if(ptagGoods)
		{
			m_stBaseProperty.strName = ptagGoods->BaseProperty.strName;
			// 先根据物编里的数据填充物品的附加属性列表
			vector<CGoodsList::tagEffect>::iterator EffectIter = ptagGoods->vectorEffect.begin();
			for(;EffectIter!=ptagGoods->vectorEffect.end();EffectIter++)
			{
				CGoodsList::tagEffect GoodsEffect;
				GoodsEffect.wType = EffectIter->wType;
				GoodsEffect.dwValue1 = EffectIter->dwValue1;
				GoodsEffect.dwValue2 = EffectIter->dwValue2;
				GoodsEffect.bEnable = false;
				GoodsEffect.bHide = false;
				GetEffect()->push_back(GoodsEffect);
			}
			SetNewEffectVal(CGoods::GAP_WEAPON_LEVEL,1,rdb.GetLongFromByteArray());
			//SetGraphicsID(GetProperty()->dwGroundId);
			m_stBaseProperty.dwType = GetProperty()->dwType;
			m_GoodsBaseType = static_cast<eGoodsBaseType>(m_stBaseProperty.dwType);
			SetName(m_stBaseProperty.strName.c_str());
			return true;
		}
		else	// 客户端goods.dat中没有这个物品，不加入进去
		{
			char str[256];
			sprintf(str,"Goods.cpp 物品<%s>解码错误", m_stBaseProperty.strName.c_str());
			AfxMessageBox(str,MB_OK,NULL);
			return false;
		}
	}
	else
	{
		SetIndex( rdb.GetDwordFromByteArray() );
		rdb.GetBufferFromByteArray( m_guExID );
		SetNum(rdb.GetLongFromByteArray());
		//
		//char str[256];
		//rdb.GetStringFromByteArray(str,256);
		//m_stBaseProperty.strName = str;
		//SetName(GetBaseProperty()->strName.c_str());
		//
		/// 制造者的名字
		char str[256];
		rdb.GetStringFromByteArray(str,256);
		m_strManufacturerName = str;
		/// 活动信息
		//rdb.GetStringFromByteArray(str,512);
		//m_strActiveInfor = str;
		//
		GetBaseProperty()->dwValue = rdb.GetDwordFromByteArray();		//	金币价格
		GetBaseProperty()->dwSilverPrice = rdb.GetDwordFromByteArray();		// 银币价格

		GetBaseProperty()->dwBuyPrice = rdb.GetDwordFromByteArray();	//	物品买入价格
		GetBaseProperty()->dwType = rdb.GetDwordFromByteArray();
		m_GoodsBaseType=static_cast<eGoodsBaseType>(GetBaseProperty()->dwType);

		CGoodsList::tagGoods2* pstGoods = CGoodsList::GetProperty(GetIndex());
		if(pstGoods)
		{
			// 先根据物编里的数据填充物品的附加属性列表
			vector<CGoodsList::tagEffect>::iterator EffectIter = pstGoods->vectorEffect.begin();
			for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
			{
				CGoodsList::tagEffect GoodsEffect;
				GoodsEffect.wType = EffectIter->wType;//_GetWordFromByteArray(pByte,pos);
				GoodsEffect.dwValue1 = EffectIter->dwValue1;//_GetLongFromByteArray(pByte,pos);
				GoodsEffect.dwValue2 = EffectIter->dwValue2;//_GetLongFromByteArray(pByte,pos);
				GoodsEffect.bEnable = false;
				GoodsEffect.bHide = false;
				GetEffect()->push_back(GoodsEffect);
			}

			// 根据服务器传过来的部分附加属性数据重新设置这部分数据（或者是相对与物编新增加的一些属性）
			long lEffectSize = rdb.GetLongFromByteArray();
			for (int i = 0;i<lEffectSize;i++)
			{
				WORD wEffectType = rdb.GetWordFromByteArray();
				long lValue1 = rdb.GetLongFromByteArray();
				long lValue2 = rdb.GetLongFromByteArray();
				SetNewEffectVal(wEffectType,1,lValue1);
				SetNewEffectVal(wEffectType,2,lValue2);
			}

			SetPosX(rdb.GetLongFromByteArray());
			SetPosY(rdb.GetLongFromByteArray());

			//SetGraphicsID(GetProperty()->dwGroundId);
			m_stBaseProperty.strName = pstGoods->BaseProperty.strName;
			SetName(m_stBaseProperty.strName.c_str());

			long lBaseHoleNum = GetEffectVal(GAP_HOLE_NUM,2);
			if (lBaseHoleNum!=0)
			{
				DWORD dwCardNum = rdb.GetDwordFromByteArray();
				//m_wHoleNum = rdb.GetDwordFromByteArray();
				m_wHoleNum = lBaseHoleNum;
				for (long i = 0;i<lBaseHoleNum;i++)
				{
					m_mapHoleState.insert(pair<WORD,bool>(i,false));
				}

				DWORD dwGoodsIndex = 0;
				CGoods* pCardGoods = NULL;
				for (int i = 0;i<dwCardNum;i++)
				{
					dwGoodsIndex = rdb.GetDwordFromByteArray();
					if (dwGoodsIndex!=0)
					{
						pCardGoods = new CGoods();
						pCardGoods->DecordFromDataBlock(rdb,bExData);
						tagCardProperty temp;
						temp.dwCardIndex = pCardGoods->GetIndex();
						temp.CardGUID = pCardGoods->GetExID();
						temp.dwCardType = pCardGoods->GetEffectVal(GAP_CARD_CATEGORY,1);
						temp.bMaxNum = pCardGoods->GetEffectVal(GAP_CARD_ENCHASE_NUM_LIMIT,1);
						temp.strCardName = pCardGoods->GetName();
						vector<CGoodsList::tagEffect>::iterator Iter = pCardGoods->GetEffect()->begin();
						for(;Iter!=pCardGoods->GetEffect()->end();Iter++)
						{
							temp.CardEffect.push_back(*Iter);
						}
						m_InlayHole.insert(pair<WORD,tagCardProperty>(i,temp));
						SAFE_DELETE(pCardGoods);
					}
				}
			}
			return true;
		}
		else	// 客户端goods.dat中没有这个物品，不加入进去
		{
			DWORD dwDur = rdb.GetDwordFromByteArray();
			long lEffectSize = rdb.GetLongFromByteArray();
			SetEffectVal(CGoods::GAP_GOODS_MAXIMUM_DURABILITY,2,dwDur);
			SetPosX(rdb.GetFloatFromByteArray());
			SetPosY(rdb.GetFloatFromByteArray());

			char str[256];
			sprintf(str,"Goods.cpp 物品<%s>解码错误", m_stBaseProperty.strName.c_str());
			AfxMessageBox(str,MB_OK,NULL);
			return false;
		}
	}

	return false;
}

DWORD CGoods::GetGoodsBaseType()
{
	CGoodsList::tagGoods2* ptagGoods = CGoodsList::GetProperty(m_dwIndex);
	if( !ptagGoods)
		return -1;
	return ptagGoods->BaseProperty.dwType;
}

//判断能否拾取可叠加物品
bool CGoods::isGoodsCanAccumulate(CGoods *pOnGroundGoods,CPlayer* pPlayer)
{
	if (!pOnGroundGoods)	return false;
	if(pPlayer != NULL)
	{
		int iExtendID = -1;
		int iPos = -1;
//		if( pPlayer->FindOverLapGoodsByIndex(pOnGroundGoods->GetIndex(),pOnGroundGoods->GetNum(),iExtendID,iPos))
		{
			return true;
		}
	}
	return false;
}

bool CGoods::IsLocked()
{
	return m_lLockCounter >0? true:false;
}
void CGoods::Lock()
{
	m_lLockCounter = 1;
	if( m_lLockCounter >0)
		SetHaveShadowState(true);
}
void CGoods::UnLock()
{
	m_lLockCounter = 0;
	if( m_lLockCounter == 0)
		SetHaveShadowState(false);
}

long CGoods::GetEffectVal(WORD wType,int nWhich)
{
	if(!GetEffect()) return 0;

	if(GetEffect()->empty())return 0;
	vector<CGoodsList::tagEffect>::iterator iter=GetEffect()->begin();
	for(;iter!=GetEffect()->end();iter++)
	{
		if(iter->wType==wType)
		{
			if(nWhich==1)
				return iter->dwValue1;
			else if(nWhich==2)
				return iter->dwValue2;
		}
	}

	return 0;
}

void CGoods::SetEffectVal(WORD wType,int nWhich,long lVal)
{
	vector<CGoodsList::tagEffect>::iterator iter=GetEffect()->begin();
	for(;iter!=GetEffect()->end();iter++)
	{
		if(iter->wType==wType)
		{
			if(nWhich==1)
				iter->dwValue1=lVal;
			else
				iter->dwValue2=lVal;
			return;
		}
	}
}

//给物品添加新的附加属性
void CGoods::SetNewEffectVal(WORD wType,int nWhich,long lVal)
{
	vector<CGoodsList::tagEffect>::iterator iter=GetEffect()->begin();
	for(;iter!=GetEffect()->end();iter++)
	{
		if(iter->wType==wType)
		{
			if(nWhich==1)
				iter->dwValue1=lVal;
			else
				iter->dwValue2=lVal;
			return;
		}
	}
	CGoodsList::tagEffect temp;
	temp.wType = wType;
	if (nWhich==1)
	{
		temp.dwValue1 = lVal;
	}else
	{
		temp.dwValue2 = lVal;
	}
	m_vectorEffect.push_back(temp);
}

bool CGoods::CanRepair()
{	
	if(GetBaseProperty()->dwType>=CGoodsList::GT_HEAD && 
		GetBaseProperty()->dwType<=CGoodsList::GT_FROCK && 
		GetBaseProperty()->dwType!=CGoodsList::GT_WING )
	{
		return true;
	}
	return false;
}

bool CGoods::CanUpgrade()
{
	if(GetBaseProperty()->dwType==CGoodsList::GT_HEAD || 
		GetBaseProperty()->dwType==CGoodsList::GT_BODY || 
		GetBaseProperty()->dwType==CGoodsList::GT_GLOVE || 
		GetBaseProperty()->dwType==CGoodsList::GT_WEAPON || 
		GetBaseProperty()->dwType==CGoodsList::GT_BOOT ||
		GetBaseProperty()->dwType==CGoodsList::GT_RING ||
		GetBaseProperty()->dwType==CGoodsList::GT_NECKLACE)
	{
		return true;
	}
	return false;
}

bool CGoods::HasEffect( WORD wType )
{
	if( GetEffect() )
	{
		if( ! GetEffect()->empty() )
		{
			vector< CGoodsList::tagEffect >::iterator	iter;
			for( iter = GetEffect()->begin(); iter != GetEffect()->end(); iter++ )
			{
				if( iter->wType == wType )
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CGoods::IsEffectChange()
{
#pragma message ("-----------是否应该根据bSave来判定-----------")
	if(GetBaseProperty()->dwType==CGoodsList::GT_EXPENDABLE)
		return false;
	else
		return true;
}

DWORD CGoods::RepairPrice()
{
	// NOTE: Requires a correct return value to compile.
	DWORD dwRepairPrice = 0;
	if( CanRepair() )
	{
		LONG lCurrentDurability	= GetEffectVal( 37, 2 );
		LONG lMaxDurability		= GetEffectVal( 37, 1 );
		//		FLOAT fFactor			= GetGame()->GetSetup()->fRepairFactor;
		FLOAT fRate				= ( lMaxDurability - lCurrentDurability ) / static_cast<FLOAT>( lMaxDurability );
		if( fRate < 0.0f )
		{
			fRate = 0.0f;
		}
		//		dwRepairPrice = static_cast<DWORD>( GetPrice() * fRate * fFactor );
	}
	return dwRepairPrice;
}

//void CGoods::Display(void)
//{
//}

// 初始化闪光特效
//void CGoods::IniFlashEffect()
//{
//}

//得到闪光的桢数
//void CGoods::DispalayFlashEffect()
//{
//}

DWORD CGoods::GetUpdatePrice()
{
	DWORD dwResult = 0;
	if( CanUpgrade() )
	{
		// 每次升级费用＝物品升级价格×（1＋（38－4×lg物品升级价格）/15）pow（物品当前等级）
		//LONG lCurrentLevel	= GetEffectVal( 48, 1 );
		LONG lUpgradePrice	= GetEffectVal( 72, 1 );
		return lUpgradePrice;
		//FLOAT fFactor		= 1 + ( 38 - 4 * log10( static_cast<FLOAT>(lUpgradePrice) ) ) / 15;
		//fFactor				= pow( fFactor, lCurrentLevel );
		//dwResult			= static_cast<DWORD>( lUpgradePrice * fFactor );
	}
	return dwResult;
}

DWORD CGoods::GetPrice()
{
	return GetBaseProperty()->dwValue > m_stBaseProperty.dwSilverPrice ? m_stBaseProperty.dwValue:m_stBaseProperty.dwSilverPrice;
}

DWORD CGoods::GetPriceType()
{
	return GetBaseProperty()->dwPriceType;
}

DWORD CGoods::GetSalePrice()
{
	DWORD dwVendPrice = 0;
	return dwVendPrice;
}
//DWORD CGoods::GetNameColor()
//{
//	return m_dwNameColor;
//}
//void CGoods::SetNameColor(DWORD dwColor)
//{
//	m_dwNameColor=dwColor;
//}

void CGoods::SetDropEffect()
{
	m_bDropEffect = TRUE;
	m_dwDropTimeStart = timeGetTime();
}

//重载高度
float CGoods::GetHeight()
{
	return 0;
	//	if (m_bDropEffect)
	//	{
	// 		return m_fDropHeight ;//+ m_fHeight;
	//	}
	//	return m_fHeight;
}

////////////物品的镶嵌孔接口////////////////

//给物品打孔的接口
bool CGoods::Stiletto()
{
	if (m_wHoleNum>=0&&m_wHoleNum<GetEffectVal(GAP_HOLE_NUM,1))
	{
		m_mapHoleState[m_wHoleNum] = true;
		m_wHoleNum++;
		this->SetEffectVal(GAP_REFINE,1,0);
		return true;
	}
	return false;
}

//封堵镶嵌孔
bool CGoods::PlugUpHole(BYTE index,WORD wHoleNum)
{
	if (index<0||index>=GetEffectVal(GAP_HOLE_NUM,2)) 
		return false;
	m_wHoleNum = wHoleNum;
	if (m_wHoleNum>=0)
	{
		map<WORD,tagCardProperty>::iterator it = m_InlayHole.find(index);
		if (it!=m_InlayHole.end())
		{
			m_InlayHole.erase(it);
		}
		map<WORD,bool>::iterator HoleIter = m_mapHoleState.find(index);
		if (HoleIter!=m_mapHoleState.end()&&HoleIter->second)
		{
			HoleIter->second=false;
		}
		return true;
	}else
	{
		m_wHoleNum = 0;
	}
	return false;	
}

//镶嵌卡片
bool CGoods::EnchaseCard(BYTE index,tagCardProperty card)
{
	if (index>=m_InlayHole.size()&&index<m_wHoleNum)
	{
		m_InlayHole.insert(pair<WORD,tagCardProperty>(index,card));
		return true;
	}
	return false;
}

//拆除卡片
bool CGoods::RemoveCard(BYTE index)
{
	map<WORD,tagCardProperty>::iterator it = m_InlayHole.find(index);
	if (it!=m_InlayHole.end())
	{
		m_InlayHole.erase(it);
		return true;
	}
	return false;
}


//设置物品的当前孔数
void CGoods::SetHoleNum(WORD wNum)
{
	if (wNum>=0&&wNum<=GetEffectVal(GAP_HOLE_NUM,1))
	{
		m_wHoleNum = wNum;
	}
}
//获取物品指定位置的镶嵌孔的状态（开、关）
bool CGoods::GetHoleState(BYTE HoleIndex) 
{
	if (HoleIndex>=0&&HoleIndex<GetEffectVal(GAP_HOLE_NUM,2))
	{
		return m_mapHoleState[HoleIndex];
	}
	return false;
}
//设置物品指定位置的镶嵌孔的状态（开、关）
void CGoods::SetHoleState(BYTE HoleIndex,bool bState)
{
	if (HoleIndex>=0&&HoleIndex<GetEffectVal(GAP_HOLE_NUM,2))
	{
		m_mapHoleState[HoleIndex] = bState;
	}
}