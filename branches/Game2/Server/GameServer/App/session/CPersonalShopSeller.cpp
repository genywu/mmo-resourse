// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CPersonalShopSeller.h"
#include "CPersonalShopBuyer.h"
#include "CSession.h"
#include "CSessionFactory.h"
#include "../Container/CContainer.h"
#include "../Container/CTestContainer.h"
#include "../BaseObject.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"
#include "../Player.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../MessagePackage/CS2CContainerObjectAmountChange.h"
#include "../../Server/BillClient.h"
#include "../BillApp/BillTrade.h"

#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool PerShopProcessing(const CGUID &SessionID, long lPlugID, long lTotalMoney, long lTotalYuanbao )
{
	CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
	if( !pSession || !pSession->IsSessionAvailable() )
		return false;

	CPersonalShopBuyer* pBuyerPlug = dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
	if( !pBuyerPlug )
		return false;

	CPersonalShopSeller* pSellerPlug = pBuyerPlug->GetSeller();
	if( !pSellerPlug )
		return false;

	CPlayer *pSeller = GetGame()->FindPlayer(pSellerPlug->GetOwnerID());
	if( !pSeller )
		return false;

	return pSellerPlug->PurchaseEx(lPlugID, lTotalMoney, lTotalYuanbao, 0 );
}

//##ModelId=42266DFF0242
CPersonalShopSeller::CPersonalShopSeller():m_strShopName(AppFrame::GetText("GS_PERSONALSHOP_18"))
{
	// TODO: Add your specialized code here.
	m_bShopOpened = FALSE;
	m_cSample.SetContainerVolume( 5,1 );
	m_cSample.AddListener( this );
	m_vecTradeInfo.reserve(5);
}

//##ModelId=42266E09037A
//CPersonalShopSeller::CPersonalShopSeller(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=42266E1A0109
CPersonalShopSeller::~CPersonalShopSeller()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=42266E2A035B
BOOL CPersonalShopSeller::IsPlugAvailable()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CMoveShape* pOwner = GetOwner();
	if( pOwner && pOwner->IsDied() == FALSE )
	{		
			CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pOwner->GetFather() );
			if( pRegion )
			{
				//if( pRegion->GetSecurity( pOwner->GetTileX(), pOwner->GetTileY() ) == 
				//	CRegion::SECURTIY_SAFE )
				//{
				//	bResult = TRUE;
				//}
			}		
	}
	//return bResult;
	return TRUE;
}

//##ModelId=42266E34005D
BOOL CPersonalShopSeller::OnSessionEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=42266E3C02EE
BOOL CPersonalShopSeller::OnSessionAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=42266E4B00FA
BOOL CPersonalShopSeller::OnPlugInserted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_cSample.SetOwner( TYPE_SESSION, m_SessionID );
	m_cSample.SetContainerExtendID( Get32ID() << 8 );

	//##把监听器加入到玩家的容器内。
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer ->getOriginPack()->AddListener( &m_cSample );
		pPlayer->m_cEquipment.AddListener( &m_cSample );
		pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->AddListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->AddListener(&m_cSample);
	}

	return TRUE;
}

//##ModelId=42266E54005D
BOOL CPersonalShopSeller::OnPlugAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=42266E6900AB
BOOL CPersonalShopSeller::OnPlugEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		m_mGoodsPrice.clear();
		m_cSample.Clear();
		pPlayer->ClearPersonalShopGoodsData();
		pPlayer ->getOriginPack()->RemoveListener( &m_cSample );
		pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer->RemoveListener(&m_cSample);
		pPlayer->m_cEquipment.RemoveListener( &m_cSample );
		if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL)
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );
		if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_SHOPPING)
			pPlayer->SetCurrentProgress( CPlayer::PROGRESS_NONE );

		CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(m_SessionID) );
		if( pSession )
		{
			list<long>::iterator it = pSession -> GetPlugList().begin();
			for( ; it != pSession -> GetPlugList().end(); it ++ )
			{
				CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
				if( pPlug && pPlug -> GetOwnerType() == TYPE_PLAYER )
				{
					CPlayer* pPlugPlayer=dynamic_cast<CPlayer*>(pPlug->GetOwner());
					if(pPlugPlayer)
					{
						pPlugPlayer->SetCurrentProgress(CPlayer::PROGRESS_NONE);
					}
				}
			}
		}
	


		CMessage msg( MSG_S2C_PLAYERSHOP_CLOSE );
		msg.Add( pPlayer->GetExID() );
		msg.Add( m_SessionID );
		msg.SendToAround( pPlayer );

		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=42266E710232
BOOL CPersonalShopSeller::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return TRUE;
}

//##ModelId=42266FB40128
void CPersonalShopSeller::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bShopOpened = FALSE;
	m_mGoodsPrice.clear();
	m_strShopName.clear();
	//##影子容器的Release函数不会删除原物品，只会将自己清空，然后移出
	//##所有的监听器。
	m_cSample.Release();
	ReleaseMapBuying();
}

//##ModelId=42266FBD031C
CContainer* CPersonalShopSeller::GetContainer(LONG lID)
{
	// TODO: Add your specialized code here.
	//if( m_bShopOpened == FALSE && lID == 0 )
	{
		return &m_cSample;
	}
	//return NULL;
}

//##ModelId=422E542D01A5
bool CPersonalShopSeller::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##发送给各个
	return false;
}

//##ModelId=422E543D0251
bool CPersonalShopSeller::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##移出某个商品的时候，将价格列表中的哪一项删除，因为影子容器不会通知具体是哪个被删除了，
	//##这儿就只能全部效验一次。确定那个是需要被删除的项
	itGoodsPrice it = m_mGoodsPrice.begin();
	for( ; it != m_mGoodsPrice.end(); )
	{
		DWORD dwPosition	= 0;
		CGoods* pGoods		= dynamic_cast<CGoods*>( m_cSample.Find(TYPE_GOODS, it->first) );

		//##如果该物品存在，且在影子容器内有有效位置。则跳过。否则就删除掉该项。
		if( pGoods && m_cSample.QueryGoodsPosition(pGoods, dwPosition) )
		{
			it ++;
		}
		else
		{
			//##先移出该影子，然后再删除
			m_cSample.RemoveShadow( it->first );
			it = m_mGoodsPrice.erase( it );
		}
	}
	return true;
}

//##ModelId=422E545301D4
bool CPersonalShopSeller::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return true;
}

//##ModelId=42267E6701D4
BOOL CPersonalShopSeller::OpenForBusiness()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_bShopOpened == FALSE )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
		if( pPlayer )
		{
			pPlayer->SetPersonalShopFlag( m_SessionID, Get32ID() );
			m_bShopOpened = TRUE;
		}
	}
	return m_bShopOpened;
}

//##ModelId=4227C0520186
BOOL CPersonalShopSeller::CloseDown()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_bShopOpened )
	{
		m_mGoodsPrice.clear();
		m_cSample.Clear();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
		if( pPlayer )
		{
			pPlayer->SetPersonalShopFlag( NULL_GUID, 0 );
			m_bShopOpened = FALSE;
		}
	}
	return !m_bShopOpened;
}

//##ModelId=422BAFA30148
BOOL CPersonalShopSeller::SetShopName(CHAR* strName)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( strName )
	{
		m_strShopName	= strName;
		bResult			= TRUE;
	}
	return bResult;
}

//##ModelId=422BAFBD007D
CHAR* CPersonalShopSeller::GetShopName()
{
	// TODO: Add your specialized code here.
	return const_cast<CHAR*>( m_strShopName.c_str() );
}

//取得商店名称
CHAR* CPersonalShopSeller::GetShopInfo()
{
	return const_cast<CHAR*>(m_strShopInfo.c_str());
}
//设置商店名称
BOOL CPersonalShopSeller::SetShopInfo( string strShopInfo )
{
	BOOL bResult = FALSE;
	if( !strShopInfo.empty() )
	{
		m_strShopInfo	= strShopInfo;
		bResult			= TRUE;
	}
	return bResult;
}

//取得交易信息
vector<string>& CPersonalShopSeller::GetTradeInfo()
{
	return m_vecTradeInfo;
}
//插入一条交易信息（最多保存5条）
void CPersonalShopSeller::InsertTradeInfo( const string& strTradeInfo )
{
	if ( m_vecTradeInfo.size() < MAX_TRADE_INFO )
	{
		m_vecTradeInfo.push_back( strTradeInfo );
	}
	else if ( m_vecTradeInfo.size() == MAX_TRADE_INFO )
	{
		m_vecTradeInfo.erase( m_vecTradeInfo.begin() );
		m_vecTradeInfo.push_back( strTradeInfo );
	}
}

//##ModelId=422BB13A02EE
//设置物品价格信息（包括贩卖方式信息）
BOOL CPersonalShopSeller::SetGoodsPrice(const CGUID& guid,LONG lGoodsId, WORD wSellType, 
				   WORD dwPriceType, DWORD dwGroupNum,  DWORD dwOneGroupCnt,DWORD dwPrice)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	if( m_cSample.Find(TYPE_GOODS, guid) )
	{
		tagPrice price;
		price.dwSellType = wSellType;
		price.dwOneGroupCnt = dwOneGroupCnt;
		price.dwGroupNum = dwGroupNum;
		price.dwPrice		= dwPrice;
		price.dwPriceType	= dwPriceType;
		price.guid=guid;
		price.lGoodsId=lGoodsId;
		m_mGoodsPrice[guid] = price;
		bResult = TRUE;
	}

	return bResult;
}

//##ModelId=422BB17300AB
//取得物品价格信息
BOOL CPersonalShopSeller::GetGoodsPrice(const CGUID& guid, LONG lGoodsId,DWORD &dwPriceType, DWORD& dwPrice)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	DWORD dwMaxStack=GoodsSetup::QueryGoodsMaxStackNumber(lGoodsId);
	dwPrice = 0;
	if(dwMaxStack>1)
	{
		itGoodsPrice it=m_mGoodsPrice.begin();
		for(;it!=m_mGoodsPrice.end();it++)
		{
			if(it->second.lGoodsId==lGoodsId)
			{
				dwPriceType	= it->second.dwPriceType;
				dwPrice		= it->second.dwPrice;
				bResult = TRUE;
			}
		}
	}
	else
	{
		itGoodsPrice it = m_mGoodsPrice.find( guid );
		if( it != m_mGoodsPrice.end() )
		{
			dwPriceType	= it->second.dwPriceType;
			dwPrice		= it->second.dwPrice;
			bResult = TRUE;
		}
	}	
	return bResult;
}

//取得物品贩卖方式信息
BOOL CPersonalShopSeller::GetGoodsSellType(const CGUID& guid, LONG lGoodsId, WORD& wSellType, DWORD& dwGroupNum, DWORD& dwOneGroupCnt )
{
	BOOL bResult = FALSE;
	DWORD dwMaxStack=GoodsSetup::QueryGoodsMaxStackNumber(lGoodsId);
	if(dwMaxStack>1)
	{
		itGoodsPrice it=m_mGoodsPrice.begin();
		for(;it!=m_mGoodsPrice.end();it++)
		{
			if(it->second.lGoodsId==lGoodsId)
			{
				wSellType = (WORD)it->second.dwSellType;
				dwOneGroupCnt = it->second.dwOneGroupCnt;
				dwGroupNum = it->second.dwGroupNum;
				bResult = TRUE;
			}
		}
	}
	else
	{
		itGoodsPrice it = m_mGoodsPrice.find( guid );
		if( it != m_mGoodsPrice.end() )
		{
			wSellType = (WORD)it->second.dwSellType;
			dwOneGroupCnt = it->second.dwOneGroupCnt;
			dwGroupNum = it->second.dwGroupNum;
			assert( dwOneGroupCnt ==1 || dwGroupNum ==1 );
			bResult = TRUE;
		}
	}	
	return bResult;
}

//是否存在指定goodsid的物品价格
BOOL CPersonalShopSeller::GoodsPriceExist(LONG lGoodsId)
{
	itGoodsPrice it=m_mGoodsPrice.begin();
	for(;it!=m_mGoodsPrice.end();it++)
	{
		if(it->second.lGoodsId==lGoodsId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CGoodsShadowContainer::tagGoodsShadow* CPersonalShopSeller::RemoveByGoodsId(DWORD dwId)
{
	return m_cSample.RemoveByGoodsId(dwId);
}

void CPersonalShopSeller::AddShadowTag(CGoodsShadowContainer::tagGoodsShadow* pTag)
{
	m_cSample.AddShadowTag(pTag);
}

// 检测购买条件
BOOL CPersonalShopSeller::CheckCondition(LONG lPlugID, DWORD &dwTotalMoney, DWORD& dwTotalWeimian )
{
	MapBuying::iterator buyIter=m_mapBuying.find(lPlugID);
	if(buyIter==m_mapBuying.end())
		return CR_OTHR_ERROR;
	VecBuyInfo *pVec =buyIter->second;
	CSession* pSession	= GetSession();
	CPlayer* pSeller	= dynamic_cast<CPlayer*>( GetOwner() );
	if( !pVec ||
		!pSession ||
		!pSeller ||
		!pSession->IsSessionAvailable()
		)
		return CR_OTHR_ERROR;

	CPlayer* pBuyPlayer	= NULL;
	CPersonalShopBuyer* pBuyerPlug	= dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
	if( pBuyerPlug )
	{
		pBuyPlayer = dynamic_cast<CPlayer*>( pBuyerPlug->GetOwner() );
	}
	if( !pBuyPlayer )
		return CR_OTHR_ERROR;

	// 首先获取该物品的价格，如果失败，则该物品无法被出售。
	DWORD dwPriceType		= 0;
	DWORD dwPrice			= 0;

	VecBuyInfo::iterator tagIter = pVec->begin();
	for( ; tagIter!=pVec->end(); ++tagIter )
	{
		if( GetGoodsPrice( tagIter->guid, tagIter->lId, dwPriceType, dwPrice ) )
		{
			if ( dwPriceType == PT_GOLD_SELL )//金币
				dwTotalMoney += tagIter->dwNum * dwPrice;
			else if ( dwPriceType == PT_WEIMIAN_SELL )//位面精华
				dwTotalWeimian += tagIter->dwNum * dwPrice;
		}
		else
		{
			return CR_OTHR_ERROR;
		}
		if ( (dwTotalMoney == 0) && (dwTotalWeimian == 0) )
		{
			return CR_OTHR_ERROR;
		}
	}

	// 金钱购买
	if( pBuyPlayer->GetMoney() < dwTotalMoney )
	{
		const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_7");
		if(strRes)
		{
			pBuyPlayer->SendNotifyMessage(strRes);
		}
		
		return CR_MISS_MONEY;
	}

	//位面购买
	if ( pBuyPlayer->GetGoodsAmountByOriginName( 
		GoodsSetup::QueryGoodsOriginalName( 
		GlobalSetup::GetSetup()->dwWeimianGoodsId ) )  < dwTotalWeimian )
	{
		const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_14");//(位面不足，无法购买。)
		if(strRes)
		{
			pBuyPlayer->SendNotifyMessage(strRes);
		}

		return CR_MISS_WEIMIAN;
	}

	//SELLER的钱是否超过上限
	if(pSeller->GetMoney()+dwTotalMoney>GlobalSetup::GetSetup()->dwGoldCoinLimit)
	{
		const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_10");
		if(strRes)
		{
			pBuyPlayer->SendNotifyMessage(strRes);
		}
		return CR_OTHR_ERROR;
	}

	return CR_WAIT_FINISH;
}

bool CPersonalShopSeller::PurchaseEx(LONG lPlugID, DWORD dwTotalMoney, long lTotalYuanbao, long lTotalWeimian )
{
	DWORD dwGroupNum, dwOneGroupCnt;
	WORD wSellType;
	DWORD dwPriceType		= 0;
	DWORD dwPrice			= 0;

	VecBuyInfo *pVec = m_mapBuying.find(lPlugID)->second;
	CSession* pSession	= GetSession();
	CPlayer* pSeller	= dynamic_cast<CPlayer*>( GetOwner() );
	if( !pVec ||
		!pSession ||
		!pSeller ||
		!pSession->IsSessionAvailable()
		)
		return false;

	//检查SELLER物品 确保现在SELLER身上的物品足够
	VecBuyInfo::iterator tagBuyGoodsIter=pVec->begin();
	for(;tagBuyGoodsIter!=pVec->end();++tagBuyGoodsIter)
	{
		const char* strName=GoodsSetup::QueryGoodsOriginalName(tagBuyGoodsIter->lId);
		if(!strName)
			return false;
		DWORD dwNum=pSeller->GetGoodsAmountByOriginName(strName);
		if(dwNum<tagBuyGoodsIter->dwNum || tagBuyGoodsIter->dwNum==0)
			return false;
	}

	CPlayer* pBuyPlayer	= NULL;
	CPersonalShopBuyer* pBuyerPlug	= dynamic_cast<CPersonalShopBuyer*>( pSession->QueryPlugByID(lPlugID) );
	if( pBuyerPlug )
	{
		pBuyPlayer = dynamic_cast<CPlayer*>( pBuyerPlug->GetOwner() );
	}
	if( !pBuyPlayer )
		return false;


	//这里检测购买者背包是否符合条件 如符合 输出物品放置结果
	//同时先创建添加给购买者的物品
	vector<CGoods*> vecGoodsToBuyer;
	VecBuyInfo::iterator tagIter = pVec->begin();
	for(;tagIter!=pVec->end();++tagIter)
	{
		if ( !GetGoodsSellType( tagIter->guid, tagIter->lId, wSellType, dwGroupNum, dwOneGroupCnt ) )
		{
			return false;
		}

		DWORD dwSellNum;
		if ( wSellType == ST_GROUP_SELL )
		{
			dwSellNum = tagIter->dwNum *  dwOneGroupCnt;
		}
		else if ( wSellType == ST_SINGLE_SELL )
		{
			dwSellNum = tagIter->dwNum;
		}
		CGoodsFactory::CreateGoods(tagIter->lId,dwSellNum,vecGoodsToBuyer,51);
	}
	CTestContainer testContainerForBuyer;
	testContainerForBuyer.Init(pBuyPlayer);

	//检测购买者扣除位面精华
	vector<tagDelTestResult> vecDelResult;
	if ( lTotalWeimian > 0 )
	{
		vector<tagTestArg_ID_Num> vecDelGoodsForBuyer;
		tagTestArg_ID_Num DelTest;
		DelTest._dwGoodsID = GlobalSetup::GetSetup()->dwWeimianGoodsId;
		DelTest._dwNum = lTotalWeimian;
		vecDelGoodsForBuyer.push_back( DelTest );
		BOOL bSucced = testContainerForBuyer.DelTest( vecDelGoodsForBuyer, &vecDelResult );
		if ( !bSucced )
		{
			return false;
		}
	}

	vector<tagTestResult> vecResult;	
	BOOL bSucced=testContainerForBuyer.AddTest(vecGoodsToBuyer,&vecResult);	
	for(int i=0;i< (int)vecGoodsToBuyer.size();i++)
	{
		OBJ_RELEASE(CGoods, vecGoodsToBuyer[i]);
	}
	vecGoodsToBuyer.clear();
	if(!bSucced)
	{
		//购买者背包空间不够
		const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_8");
		if(strRes)
		{
			pBuyPlayer->SendNotifyMessage(strRes);
		}
		return false;
	}				

	// 卖方格子数
	DWORD dwSellerGrid = 0;
	for(tagIter=pVec->begin();tagIter!=pVec->end();++tagIter)
	{
		DWORD dwMaxStack = GoodsSetup::QueryGoodsMaxStackNumber(tagIter->lId);
		if( dwMaxStack > 1 )
		{
			if ( !GetGoodsSellType( tagIter->guid, tagIter->lId, wSellType, dwGroupNum, dwOneGroupCnt ) )
			{
				return false;
			}

			DWORD dwSellerRemainNum;
			if ( wSellType == ST_GROUP_SELL )
			{
				dwSellerRemainNum = tagIter->dwNum *  dwOneGroupCnt;
			}
			else if ( wSellType == ST_SINGLE_SELL )
			{
				dwSellerRemainNum = tagIter->dwNum;
			}

			CGoodsShadowContainer::goodsshadowIt shadowIter = m_cSample.GetGoodsShadowMap()->begin();
			for( ; shadowIter!=m_cSample.GetGoodsShadowMap()->end(); ++shadowIter )
			{
				if( shadowIter->second.dwGoodsBasePropertiesIndex == tagIter->lId )
				{	
					DWORD dwCalNum = 0;
					if( dwSellerRemainNum >= shadowIter->second.dwGoodsAmount )
					{
						//需要删除该物品
						dwCalNum = shadowIter->second.dwGoodsAmount;
						dwSellerGrid++;
					}
					else
					{
						dwCalNum = dwSellerRemainNum;
						dwSellerGrid++;
					}
					dwSellerRemainNum -= dwCalNum;
					if(dwSellerRemainNum <= 0)
						break;
				}
			}
		}
		else
		{
			dwSellerGrid++;
		}
	}

	//卖家位面添加检测，此处只检测未扣除上架物品前，卖家是否能够添加相应的位面
	vector<tagTestResult> vecAddResultForSeller;	
	if ( lTotalWeimian > 0 )
	{	
		CTestContainer testContainerToSeller;
		testContainerToSeller.Init(pSeller);
		//位面添加
		vector<CGoods*> vecWeimianGoodsToSeller;
		CGoodsFactory::CreateGoods( GlobalSetup::GetSetup()->dwWeimianGoodsId,
			lTotalWeimian, vecWeimianGoodsToSeller, 180 );
		BOOL bAddToSeller =  testContainerToSeller.AddTest( vecWeimianGoodsToSeller, &vecAddResultForSeller );
		for ( int i = 0; i < (int)vecWeimianGoodsToSeller.size(); ++i )
		{
			CGoodsFactory::GarbageCollect(&vecWeimianGoodsToSeller[i],188);
		}
		if ( !bAddToSeller )
		{
			//卖家空间不足以添加交易来的位面
			const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_17");//(摆摊玩家空间不足，购买失败。)
			if( strRes )
			{
				pBuyPlayer->SendNotifyMessage(strRes);
			}
			return FALSE;
		}
	}

	CMessage msgToBuyer(MSG_S2C_PLAYERSHOP_TOBUYER);
	CMessage msgToSeller(MSG_S2C_PLAYERSHOP_TOSELLER);

	msgToBuyer.Add((DWORD)(vecResult.size()+vecDelResult.size()));
	msgToSeller.Add((DWORD)(dwSellerGrid+vecAddResultForSeller.size()));

	//买家所需支付的位面精华移除操作
	for ( int i = 0; i <(int)vecDelResult.size(); ++i )
	{
		CVolumeLimitGoodsContainer* pVolumeContainer =
			dynamic_cast<CVolumeLimitGoodsContainer*>( pBuyPlayer->FindContainer(vecDelResult[i].dwContainerId) );
		if(pVolumeContainer)
		{
			if( vecDelResult[i].eOperType == CS2CContainerObjectMove::OT_DELETE_OBJECT)
			{
				//删除
				CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecDelResult[i].dwPos );
				msgToBuyer.Add((BYTE)CS2CContainerObjectMove::OT_DELETE_OBJECT);
				msgToBuyer.Add( goodsGuid );									//GUID
				msgToBuyer.Add( vecDelResult[i].dwNum);				//数量
				msgToBuyer.Add( vecDelResult[i].dwContainerId );	//背包ID
				msgToBuyer.Add( vecDelResult[i].dwPos );					//背包位置
				CGoods* pGoods = 
					dynamic_cast<CGoods*>( pVolumeContainer->Remove( goodsGuid ) );
				if ( pGoods )
				{
					CGoodsFactory::GarbageCollect(&pGoods,222);
					GetLogicLogInterface()->logT504_weimian_log(pBuyPlayer,vecDelResult[i].dwNum,1,1);
				}
				else
				{
					return false;
				}
			}
			else if ( vecDelResult[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT )
			{
				//修改数量
				CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecDelResult[i].dwPos );
				LONG lGoodsNum = pVolumeContainer->GetGoods( vecDelResult[i].dwPos )->GetAmount() - vecDelResult[i].dwNum;
				msgToBuyer.Add((BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
				msgToBuyer.Add( goodsGuid );										//GUID			
				msgToBuyer.Add( lGoodsNum );										//数量（买家删除后的数量）
				msgToBuyer.Add( vecDelResult[i].dwContainerId );		//背包ID
				msgToBuyer.Add( vecDelResult[i].dwPos );						//背包位置
				pVolumeContainer->GetGoods( vecDelResult[i].dwPos )->SetAmount( lGoodsNum );
				GetLogicLogInterface()->logT504_weimian_log(pBuyPlayer,vecDelResult[i].dwNum,1,2);
			}
		}
	}

	//交易商品操作
	for( tagIter=pVec->begin(); tagIter!=pVec->end(); ++tagIter )
	{
		GetGoodsPrice( tagIter->guid, tagIter->lId, dwPriceType, dwPrice );
		DWORD dwMaxStack = GoodsSetup::QueryGoodsMaxStackNumber(tagIter->lId);
		if( dwMaxStack > 1 )								// 叠加物品
		{
			// 该ID物品在摆摊中的数量
			DWORD dwSellNum = m_cSample.GetGoodsNumByIndex(tagIter->lId);

			GetGoodsSellType( tagIter->guid, tagIter->lId, wSellType, dwGroupNum, dwOneGroupCnt );

			DWORD dwBuyNum;
			DWORD dwCalcTotalMoney = dwPrice * tagIter->dwNum;
			if ( wSellType == ST_GROUP_SELL )
			{
				dwBuyNum = tagIter->dwNum *  dwOneGroupCnt;
			}
			else if ( wSellType == ST_SINGLE_SELL )
			{
				dwBuyNum = tagIter->dwNum;
			}

			if( dwSellNum >= dwBuyNum )				// 足够剩余数量的物品
			{
				long lSellerRemainNum	= dwBuyNum;
				DWORD dwBuyerRemainNum	= dwBuyNum;

				while( lSellerRemainNum > 0 )
				{
					CGoodsShadowContainer::goodsshadowIt shadowIter = m_cSample.GetGoodsShadowMap()->begin();
					for( ; shadowIter!=m_cSample.GetGoodsShadowMap()->end(); shadowIter++)
					{
						CGoods *pCurGoods = pSeller->FindGoods(shadowIter->second.lOriginalContainerExtendID,
							shadowIter->second.dwOriginalGoodsPosition);

						if( !pCurGoods )
							continue;

						if( shadowIter->second.dwGoodsBasePropertiesIndex == tagIter->lId )
						{	
							DWORD dwCalNum=0;
							if( (long)shadowIter->second.dwGoodsAmount > lSellerRemainNum )
							{
								// 锁定的大于剩余扣除
								dwCalNum = lSellerRemainNum;
								shadowIter->second.dwGoodsAmount-=dwCalNum;
								LONG lRet=m_cSample.RemoveEx(shadowIter->second.OriginalContainerID,
									shadowIter->second.lOriginalContainerExtendID,
									shadowIter->second.dwOriginalGoodsPosition,dwCalNum);
								msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
								msgToSeller.Add(shadowIter->second.guGoodsID);							//GUID
								msgToSeller.Add( (BYTE)0 );																//操作类型（0为扣除操作，1为添加操作）
								msgToSeller.Add( (LONG)dwCalNum );												//此次扣除的数量
								msgToSeller.Add(shadowIter->second.lOriginalContainerExtendID);	//背包ID
								msgToSeller.Add(shadowIter->second.dwOriginalGoodsPosition);	//背包位置

								lSellerRemainNum-=dwCalNum;
								break;
							}
							else
							{
								// 锁定的小于等于剩余扣除，物品扣除									
								if(shadowIter->second.dwGoodsAmount >= pCurGoods->GetAmount())
								{
									// 扣全部
									dwCalNum=shadowIter->second.dwGoodsAmount;										
									msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_DELETE_OBJECT);
									msgToSeller.Add(shadowIter->second.guGoodsID);							//GUID
									msgToSeller.Add((LONG)dwCalNum);													//数量
									msgToSeller.Add(shadowIter->second.lOriginalContainerExtendID);	//背包ID
									msgToSeller.Add(shadowIter->second.dwOriginalGoodsPosition);	//背包位置

									CGoods* pRemovedGoods=m_cSample.RemoveEx(shadowIter->second.OriginalContainerID,
										shadowIter->second.lOriginalContainerExtendID,
										shadowIter->second.dwOriginalGoodsPosition);
									if(pRemovedGoods)
										CGoodsFactory::GarbageCollect(&pRemovedGoods,50);
									lSellerRemainNum-=dwCalNum;
									break;
								}
								else
								{
									// 扣部分
									dwCalNum=shadowIter->second.dwGoodsAmount;
									LONG lRet=m_cSample.RemoveEx(shadowIter->second.OriginalContainerID,
										shadowIter->second.lOriginalContainerExtendID,
										shadowIter->second.dwOriginalGoodsPosition,dwCalNum);
									msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
									msgToSeller.Add(shadowIter->second.guGoodsID);							//GUID
									msgToSeller.Add( (BYTE)0 );																//操作类型（0为扣除操作，1为添加操作）
									msgToSeller.Add( (LONG)dwCalNum );												//此次扣除的数量
									msgToSeller.Add(shadowIter->second.lOriginalContainerExtendID);	//背包ID
									msgToSeller.Add(shadowIter->second.dwOriginalGoodsPosition);	//背包位置

									m_cSample.GetGoodsShadowMap()->erase(shadowIter);
									lSellerRemainNum-=dwCalNum;
									break;
								}									
							}
						}
					}	// for( ; shadowIter!=m_cSample.GetGoodsShadowMap()->end(); ++shadowIter )
				}	// while( dwSellerRemainNum > 0 )

				// buyer相应物品添加
				vector<CGoods*> goodsVec;
				CGoodsFactory::CreateGoods(tagIter->lId,dwBuyNum,goodsVec,52);
				for( int i=0; i<(int)goodsVec.size(); i++ )
				{
					CTestContainer testContainer;
					testContainer.Init(pBuyPlayer);
					vector<tagTestResult> vecResult;	
					BOOL bSucced=testContainer.AddTest(goodsVec[i],&vecResult);	
					if(bSucced)
					{
						CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(pBuyPlayer->FindContainer(vecResult[0].dwContainerId));
						if(pVolumeContainer)
						{
							DWORD dwGoodsId=goodsVec[i]->GetBasePropertiesIndex();
							const char* strGoodsName=GoodsSetup::QueryGoodsName(dwGoodsId);
							CGUID goodsGuid=goodsVec[i]->GetExID();
							
							if(vecResult[0].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
							{
								//添加新物品								
								pVolumeContainer->Add(vecResult[0].dwPos,goodsVec[i], NULL, NULL);				
								msgToBuyer.Add((BYTE)CS2CContainerObjectMove::OT_NEW_OBJECT);
								vector<BYTE> vecGoods;
								goodsVec[i]->SerializeForOldClient(&vecGoods);
								msgToBuyer.AddEx(&vecGoods[0],(long)vecGoods.size());
								msgToBuyer.Add((LONG)vecResult[0].dwContainerId);
								msgToBuyer.Add(vecResult[0].dwPos);
							}
							else if(vecResult[0].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
							{
								DWORD dwGoodInhereNum = pVolumeContainer->GetGoods( vecResult[0].dwPos )->GetAmount();
								DWORD dwGoodNum=goodsVec[i]->GetAmount();
								CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecResult[0].dwPos );

								pVolumeContainer->Add(vecResult[0].dwPos,goodsVec[i], NULL);

								msgToBuyer.Add((BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
								msgToBuyer.Add( goodsGuid );
								msgToBuyer.Add(dwGoodInhereNum+dwGoodNum);
								msgToBuyer.Add((LONG)vecResult[0].dwContainerId);
								msgToBuyer.Add(vecResult[0].dwPos);
							}
							
							GetLogicLogInterface()->logT010_goods_log_trade(pSeller,pBuyPlayer,dwGoodsId,goodsGuid,strGoodsName,1,
								dwCalcTotalMoney,dwBuyNum, dwPriceType);
						}
					}
					else
					{
						for(vector<CGoods*>::size_type i=0;i<goodsVec.size();i++)
						{
#ifdef _GOODSLOG1_
							if(goodsVec[i])
							{
								char pszGoodsLog[1024]="";			
								char srcGoodsGUID[50]="";
								goodsVec[i]->GetExID().tostring(srcGoodsGUID);					
								_snprintf(pszGoodsLog,1024,"[物品回收:psshopSeller792][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
									goodsVec[i]->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(goodsVec[i]->GetBasePropertiesIndex()),srcGoodsGUID,goodsVec[i]->GetAmount());
								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
							}		
#endif
							CGoodsFactory::GarbageCollect(&goodsVec[i],205);
						}
						const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_1");
						pBuyPlayer->SendNotifyMessage(strRes);
						return false;
					}
				}

			}
		}
		else												// 非叠加物品
		{
			CGoodsShadowContainer::goodsshadowIt shadowIter=m_cSample.GetGoodsShadowMap()->begin();
			for(;shadowIter!=m_cSample.GetGoodsShadowMap()->end();shadowIter++)
			{
				if(shadowIter->first==tagIter->guid)
				{
					LONG lContainerExtendID=shadowIter->second.lOriginalContainerExtendID;
					DWORD dwGoodsPosition=shadowIter->second.dwOriginalGoodsPosition;
					CGoods* pGoods=m_cSample.RemoveEx(shadowIter->second.OriginalContainerID,
						shadowIter->second.lOriginalContainerExtendID,
						shadowIter->second.dwOriginalGoodsPosition);
					msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_DELETE_OBJECT);
					CGUID tempGuid;
					tempGuid=pGoods->GetExID();
					msgToSeller.Add(tempGuid);
					msgToSeller.Add((LONG)1);
					msgToSeller.Add(lContainerExtendID);
					msgToSeller.Add(dwGoodsPosition);

					CTestContainer testContainer;
					testContainer.Init(pBuyPlayer);
					vector<tagTestResult> vecResult;
					vector<CGoods*> vecGoodsTemp;
					vecGoodsTemp.push_back(pGoods);
					BOOL bSucced=testContainer.AddTest(vecGoodsTemp,&vecResult);	
					if(bSucced)
					{
						CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(pBuyPlayer->FindContainer(vecResult[0].dwContainerId));
						if(pVolumeContainer)
						{
							DWORD dwGoodsId=vecGoodsTemp[0]->GetBasePropertiesIndex();
							const char* strGoodsName=GoodsSetup::QueryGoodsName(dwGoodsId);
							CGUID goodsGuid=vecGoodsTemp[0]->GetExID();
							if(vecResult[0].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
							{
								//添加新物品								
								pVolumeContainer->Add(vecResult[0].dwPos,vecGoodsTemp[0], NULL, NULL);				
								msgToBuyer.Add((BYTE)CS2CContainerObjectMove::OT_NEW_OBJECT);
								vector<BYTE> vecGoods;
								vecGoodsTemp[0]->SerializeForOldClient(&vecGoods);
								msgToBuyer.AddEx(&vecGoods[0],(long)vecGoods.size());
								msgToBuyer.Add((LONG)vecResult[0].dwContainerId);
								msgToBuyer.Add(vecResult[0].dwPos);
							}
							
							GetLogicLogInterface()->logT010_goods_log_trade(pSeller,pBuyPlayer,dwGoodsId,goodsGuid,strGoodsName,1,
								dwPrice,tagIter->dwNum,dwPriceType);
						}
					}
					else
					{
						const char* strRes=AppFrame::GetText("GS_PERSONALSHOP_1");
						pBuyPlayer->SendNotifyMessage(strRes);
						return false;
					}
					break;
				}
			}
		}
	}	// for( tagIter=pVec->begin(); tagIter!=pVec->end(); ++tagIter )

	//卖家位面添加操作
	if ( lTotalWeimian > 0 )
	{
		vector<CGoods*> vecWeimianGoodsToSeller;
		CGoodsFactory::CreateGoods( GlobalSetup::GetSetup()->dwWeimianGoodsId,
			lTotalWeimian, vecWeimianGoodsToSeller, 180 );
		CTestContainer testContainerForSeller;
		testContainerForSeller.Init(pSeller);
		vector<tagTestResult> vecResultForSeller;	
		testContainerForSeller.AddTest( vecWeimianGoodsToSeller, &vecResultForSeller );
		for ( int i = 0; i < (int)vecResultForSeller.size(); ++i )
		{
			CVolumeLimitGoodsContainer* pVolumeContainer =
				dynamic_cast<CVolumeLimitGoodsContainer*>(pSeller->FindContainer(vecResultForSeller[i].dwContainerId));
			if(pVolumeContainer)
			{
				DWORD dwGoodsId=vecWeimianGoodsToSeller[i]->GetBasePropertiesIndex();
				assert( dwGoodsId == 180 );
				const char* strGoodsName=GoodsSetup::QueryGoodsName(dwGoodsId);
				CGUID goodsGuid=vecWeimianGoodsToSeller[i]->GetExID();

				if(vecResultForSeller[i].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
				{
					//添加新物品	(位面精华)						
					pVolumeContainer->Add(vecResultForSeller[i].dwPos,vecWeimianGoodsToSeller[i], NULL, NULL);				
					msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_NEW_OBJECT);
					vector<BYTE> vecGoods;
					vecWeimianGoodsToSeller[i]->SerializeForOldClient(&vecGoods);
					msgToSeller.AddEx(&vecGoods[0],(long)vecGoods.size());
					msgToSeller.Add((LONG)vecResultForSeller[i].dwContainerId);
					msgToSeller.Add(vecResultForSeller[i].dwPos);
				}
				else if(vecResultForSeller[i].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
				{
					DWORD dwGoodNum=vecWeimianGoodsToSeller[i]->GetAmount();
					DWORD dwGoodInhereNum = pVolumeContainer->GetGoods( vecResultForSeller[i].dwPos )->GetAmount();
					CGUID goodsGuid = pVolumeContainer->GetCellGuid().at( vecResultForSeller[i].dwPos );

					pVolumeContainer->GetGoods( vecResultForSeller[i].dwPos )->SetAmount( dwGoodNum +dwGoodInhereNum  );

					msgToSeller.Add((BYTE)CS2CContainerObjectMove::OT_CHANGE_AMOUNT);
					msgToSeller.Add( goodsGuid );
					msgToSeller.Add( (BYTE)1 );//添加操作
					msgToSeller.Add( (LONG)dwGoodNum );

					msgToSeller.Add((LONG)vecResultForSeller[i].dwContainerId);
					msgToSeller.Add(vecResultForSeller[i].dwPos);
				}
			}
			else
			{
				CGoodsFactory::GarbageCollect( &vecWeimianGoodsToSeller[i],205 );
				return FALSE;
			}
		}
		GetLogicLogInterface()->logT504_weimian_log(pSeller,lTotalWeimian,0,1);
	}

	//交易信息生成和保存
	//string strTradeInfo;
	//tagIter = pVec->begin();
	//for( ; tagIter != pVec->end(); ++tagIter )
	//{
	//	GetGoodsPrice( tagIter->guid, tagIter->lId, dwPriceType, dwPrice);
	//	GetGoodsSellType( tagIter->guid, tagIter->lId, wSellType, dwGroupNum, dwOneGroupCnt );
	//	DWORD dwTatalMoney = dwPrice * tagIter->dwNum;
	//	char strTemp[256] = "";
	//	char strTempTime[256] = "";

	//	SYSTEMTIME	stTime;
	//	GetLocalTime(&stTime);
	//	_snprintf(strTempTime, 256, "%d:%d:%d" ,
	//		stTime.wHour,
	//		stTime.wMinute,
	//		stTime.wSecond);

	//	if ( dwPriceType == PT_GOLD_SELL )//金币
	//	{
	//		if ( wSellType == ST_SINGLE_SELL )
	//		{
	//			//  ××花费了××金币（位面精华）购买了×个（组）××（时间××）
	//			_snprintf( strTemp, 256, AppFrame::GetText("19"), 
	//				pBuyPlayer->GetName(),
	//				dwTatalMoney, tagIter->dwNum, 
	//				GoodsSetup::QueryGoodsName( tagIter->lId ),
	//				strTempTime);
	//			strTradeInfo = string( strTemp );
	//		}
	//		else if ( wSellType == ST_GROUP_SELL )
	//		{
	//			//  ××花费了××金币（位面精华）购买了×个（组）××（时间××）
	//			_snprintf( strTemp, 256, AppFrame::GetText("21"), 
	//				pBuyPlayer->GetName(),
	//				dwTatalMoney, tagIter->dwNum, 
	//				GoodsSetup::QueryGoodsName( tagIter->lId ),
	//				strTempTime);
	//			strTradeInfo = string( strTemp );
	//		}
	//	}
	//	else if ( dwPriceType == PT_WEIMIAN_SELL )//位面精华
	//	{
	//		if ( wSellType == ST_SINGLE_SELL )
	//		{
	//			//  ××花费了××金币（位面精华）购买了×个（组）××（时间××）
	//			_snprintf( strTemp, 256, AppFrame::GetText("20"), 
	//				pBuyPlayer->GetName(),
	//				dwTatalMoney, tagIter->dwNum, 
	//				GoodsSetup::QueryGoodsName( tagIter->lId ),
	//				strTempTime);
	//			strTradeInfo = string( strTemp );
	//		}
	//		else if ( wSellType == ST_GROUP_SELL )
	//		{
	//			//  ××花费了××金币（位面精华）购买了×个（组）××（时间××）
	//			_snprintf( strTemp, 256, AppFrame::GetText("22"), 
	//				pBuyPlayer->GetName(),
	//				dwTatalMoney, tagIter->dwNum, 
	//				GoodsSetup::QueryGoodsName( tagIter->lId ),
	//				strTempTime);
	//			strTradeInfo = string( strTemp );
	//		}
	//	}

	//	InsertTradeInfo( strTradeInfo );

	//	CMessage msg( MSG_S2C_PLAYERSHOP_MSGBOARDGET );
	//	msg.Add( GetSessionGUID() );
	//	msg.Add( pBuyPlayer->GetExID() );
	//	msg.Add( (BYTE)4 );//消息类型
	//	msg.Add( (BYTE)1 );//消息条数
	//	msg.Add( strTradeInfo.c_str() );

	//	//群发当前交易信息
	//	list<LONG>::iterator plugIter = pSession->GetPlugList().begin();
	//	for( ; plugIter != pSession->GetPlugList().end(); plugIter ++ )
	//	{
	//		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*plugIter) );
	//		if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER )
	//		{
	//			msg.SendToPlayer(pPlug->GetOwnerID());
	//		}
	//	}
	//}

	if( dwTotalMoney )
	{
		pBuyPlayer->SetMoney(pBuyPlayer->GetMoney()-dwTotalMoney,35);
		pSeller->SetMoney(pSeller->GetMoney()+dwTotalMoney,36);
	}

	msgToBuyer.Add((DWORD)pBuyPlayer->GetMoney());
	msgToBuyer.Add((DWORD)pBuyPlayer->GetYuanbao());
	msgToSeller.Add((DWORD)pSeller->GetMoney());
	msgToSeller.Add((DWORD)pSeller->GetYuanbao());
	msgToSeller.Add((DWORD)dwTotalMoney);
	msgToSeller.Add((DWORD)lTotalYuanbao);
	msgToSeller.Add( (DWORD)lTotalWeimian );
	msgToBuyer.SendToPlayer(pBuyPlayer->GetExID());
	msgToSeller.SendToPlayer(pSeller->GetExID());


	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACKET,0);
	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACK1,0);
	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACK2,0);
	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACK3,0);
	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACK4,0);
	GetGame()->UpdatePlayerPack(pBuyPlayer,CS2CContainerObjectMove::PEI_PACK5,0);

	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACKET,0);
	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK1,0);
	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK2,0);
	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK3,0);
	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK4,0);
	GetGame()->UpdatePlayerPack(pSeller,CS2CContainerObjectMove::PEI_PACK5,0);


	CMessage msg(MSG_S2C_PLAYERSHOP_UPDATE);

	vector<BYTE> vecGoodsList;
	GetGoodsList( vecGoodsList );

	msg.AddEx( &vecGoodsList[0], static_cast<LONG>( sizeof(BYTE) * vecGoodsList.size() ) );

	list<LONG>::iterator plugIter = pSession->GetPlugList().begin();
	for( ; plugIter != pSession->GetPlugList().end(); plugIter ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*plugIter) );
		if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER )
		{
			msg.SendToPlayer(pPlug->GetOwnerID());
		}
	}
	pBuyerPlug->Exit();
	return true;
}

BOOL CPersonalShopSeller::GetGoodsList(vector<BYTE>& vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bShopOpened )
	{
		//##记录数量
		DWORD dwGoodsAmount = GetValidGoodsAmountEx();		

		vector<tagSellInfo> vecSellInfo;
		itGoodsPrice it=m_mGoodsPrice.begin();
		vector<CGoods*> vecGoodsToGC;

		map<CGUID,CGoodsShadowContainer::tagGoodsShadow,hash_guid_compare>* pMap=m_cSample.GetGoodsShadowMap();
		CGoodsShadowContainer::goodsshadowIt shadowIt=pMap->begin();
		for(;shadowIt!=pMap->end();shadowIt++)
		{								
			DWORD dwMaxStack=GoodsSetup::QueryGoodsMaxStackNumber(shadowIt->second.dwGoodsBasePropertiesIndex);
			if(dwMaxStack>1)
			{	
				DWORD dwSellNum=0;
				CGoods* pGoods=CGoodsFactory::CreateGoods(shadowIt->second.dwGoodsBasePropertiesIndex,53);
				pGoods->SetExID(shadowIt->second.guGoodsID);
				vecGoodsToGC.push_back(pGoods);
				
				
				vector<tagSellInfo>::iterator infoIter=vecSellInfo.begin();
				for(;infoIter!=vecSellInfo.end();infoIter++)
				{
					if(infoIter->pGoods->GetBasePropertiesIndex()==shadowIt->second.dwGoodsBasePropertiesIndex)
					{
						LONG LContainerId=shadowIt->second.lOriginalContainerExtendID;
						LONG lPos=shadowIt->second.dwOriginalGoodsPosition;
						
						break;
					}
				}
				if(infoIter!=vecSellInfo.end())
				{
					infoIter->dwNum+=shadowIt->second.dwGoodsAmount;
				}
				else
				{
					it=m_mGoodsPrice.begin();
					for(;it!=m_mGoodsPrice.end();it++)
					{
						if(it->second.lGoodsId==shadowIt->second.dwGoodsBasePropertiesIndex)
						{
							tagSellInfo stSellInfo;
							stSellInfo.dwNum=shadowIt->second.dwGoodsAmount;
							stSellInfo.dwPrice=it->second.dwPrice;
							stSellInfo.dwPriceType=it->second.dwPriceType;
							stSellInfo.dwSellType = it->second.dwSellType;
							stSellInfo.dwOneGroupCnt = it->second.dwOneGroupCnt;
							stSellInfo.pGoods=pGoods;						
							vecSellInfo.push_back(stSellInfo);
							break;
						}
					}
					
				}
			}
			else
			{
				DWORD dwSellNum=0;
				CGoods* pGoods =m_cSample.FindByGuid(TYPE_GOODS, shadowIt->second.guGoodsID,dwSellNum);
				if(!pGoods)
					return FALSE;
				it=m_mGoodsPrice.begin();
				for(;it!=m_mGoodsPrice.end();it++)
				{
					if(it->second.guid==shadowIt->second.guGoodsID)
					{
						tagSellInfo stSellInfo;
						stSellInfo.dwNum=shadowIt->second.dwGoodsAmount;
						stSellInfo.dwPrice=it->second.dwPrice;
						stSellInfo.dwPriceType=it->second.dwPriceType;
						stSellInfo.dwSellType = it->second.dwSellType;
						stSellInfo.dwOneGroupCnt = it->second.dwOneGroupCnt;
						stSellInfo.pGoods=pGoods;						
						vecSellInfo.push_back(stSellInfo);
						break;
					}
				}
			}
		}
		_AddToByteArray( &vOut, (DWORD)vecSellInfo.size() );
		vector<tagSellInfo>::iterator infoIter =vecSellInfo.begin();
		for(;infoIter!=vecSellInfo.end();infoIter++)
		{
			infoIter->pGoods->SerializeForOldClient( &vOut );

			_AddToByteArray( &vOut, infoIter->dwSellType );
			_AddToByteArray( &vOut, infoIter->dwPriceType );
			DWORD dwGroupNum;
			if ( infoIter->dwSellType == ST_SINGLE_SELL )
			{
				dwGroupNum = infoIter->dwNum;
			}
			else if ( infoIter->dwSellType == ST_GROUP_SELL )
			{
				dwGroupNum = (infoIter->dwNum) / (infoIter->dwOneGroupCnt );
				assert( ((infoIter->dwNum) % (infoIter->dwOneGroupCnt )) == 0 );
			}

			_AddToByteArray( &vOut, dwGroupNum );
			_AddToByteArray( &vOut, infoIter->dwOneGroupCnt );
			_AddToByteArray( &vOut,infoIter->dwPrice );
		}

		for(size_t i=0;i<vecGoodsToGC.size();i++)
		{
			CGoodsFactory::GarbageCollect(&(vecGoodsToGC[i]),53);
		}
		bResult = TRUE;
	}
	return bResult;
}

void CPersonalShopSeller::GetUpdateGoodsList(vector<BYTE>& vOut)
{

	DWORD dwGoodsAmount = GetValidGoodsAmountEx();

	vector<tagSellInfo> vecSellInfo;

	itGoodsPrice it = m_mGoodsPrice.begin();
	for( ; it != m_mGoodsPrice.end(); it ++ )
	{
		DWORD dwNumRet=0;
		CGoods* pGoods = dynamic_cast<CGoods*>( m_cSample.FindByGuid(TYPE_GOODS, it->first,dwNumRet) );
		if( pGoods )
		{				
			DWORD dwMaxStack=pGoods->GetMaxStackNumber();
			if(dwMaxStack>1)
			{					
				vector<tagSellInfo>::iterator infoIter=vecSellInfo.begin();
				for(;infoIter!=vecSellInfo.end();infoIter++)
				{
					if(infoIter->pGoods->GetBasePropertiesIndex()==pGoods->GetBasePropertiesIndex())
					{
						break;
					}
				}

				if(infoIter!=vecSellInfo.end())
				{
					infoIter->dwNum+=dwNumRet;
				}
				else
				{
					tagSellInfo stSellInfo;
					stSellInfo.dwNum=dwNumRet;
					stSellInfo.dwPrice=it->second.dwPrice;
					stSellInfo.dwPriceType=it->second.dwPriceType;
					stSellInfo.dwSellType = it->second.dwSellType;
					stSellInfo.dwOneGroupCnt = it->second.dwOneGroupCnt;
					stSellInfo.pGoods=pGoods;					
					vecSellInfo.push_back(stSellInfo);
				}
			}
			else
			{
				tagSellInfo stSellInfo;
				stSellInfo.dwNum=dwNumRet;
				stSellInfo.dwPrice=it->second.dwPrice;
				stSellInfo.dwPriceType=it->second.dwPriceType;
				stSellInfo.dwSellType = it->second.dwSellType;
				stSellInfo.dwOneGroupCnt = it->second.dwOneGroupCnt;
				stSellInfo.pGoods=pGoods;					
				vecSellInfo.push_back(stSellInfo);
			}
		}
	}
	_AddToByteArray( &vOut, (DWORD)vecSellInfo.size() );
	vector<tagSellInfo>::iterator infoIter =vecSellInfo.begin();
	for(;infoIter!=vecSellInfo.end();infoIter++)
	{
		infoIter->pGoods->SerializeForOldClient( &vOut );

		_AddToByteArray( &vOut, infoIter->dwSellType );
		_AddToByteArray( &vOut, infoIter->dwPriceType );
		DWORD dwGroupNum;
		if ( infoIter->dwSellType == ST_SINGLE_SELL )
		{
			dwGroupNum = infoIter->dwNum;
		}
		else if ( infoIter->dwSellType == ST_GROUP_SELL )
		{
			dwGroupNum = (infoIter->dwNum) / (infoIter->dwOneGroupCnt );
			assert( ((infoIter->dwNum) % (infoIter->dwOneGroupCnt )) == 0 );
		}

		_AddToByteArray( &vOut, dwGroupNum );
		_AddToByteArray( &vOut, infoIter->dwOneGroupCnt );
		_AddToByteArray( &vOut,infoIter->dwPrice );
	}		
}	


//##ModelId=422FAFB40196
BOOL CPersonalShopSeller::DoesShopOpenedForBusiness()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_bShopOpened;
}

//##ModelId=422FD7B40167
DWORD CPersonalShopSeller::GetValidGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( m_bShopOpened )
	{
		itGoodsPrice it = m_mGoodsPrice.begin();
		for( ; it != m_mGoodsPrice.end(); it ++ )
		{
			DWORD dwPosition	= 0;
			CGoods* pGoods		= dynamic_cast<CGoods*>( m_cSample.Find(TYPE_GOODS, it->first) );

			if( pGoods && m_cSample.QueryGoodsPosition(pGoods, dwPosition) )
			{
				dwResult ++;
			}
		}
	}
	return dwResult;
}

DWORD CPersonalShopSeller::GetValidGoodsAmountEx()
{
	return m_cSample.GetCellsSize();
}

void CPersonalShopSeller::AddMapBuying(long lPlugID, vector<CPersonalShopBuyer::tagBuyInfo> *pVecBuyInfo)
{
	m_mapBuying[lPlugID] = pVecBuyInfo;
}

void CPersonalShopSeller::DelMapBuying(long lPlugID)
{
	ItrBuying itr = m_mapBuying.find(lPlugID);
	if(itr!=m_mapBuying.end())
	{
		SAFE_DELETE(itr->second);
		m_mapBuying.erase(itr);
	}	
}

void CPersonalShopSeller::ReleaseMapBuying()
{
	ItrBuying itr=m_mapBuying.begin();
	for(;itr!=m_mapBuying.end();itr++)
	{
		SAFE_DELETE(itr->second);		
	}
	m_mapBuying.clear();
}
