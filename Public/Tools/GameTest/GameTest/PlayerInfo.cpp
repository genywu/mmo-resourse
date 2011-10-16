//=============================================================================
/**
 *  file: PlayerInfo.cpp
 *
 *  Brief:显示玩家信息
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "stdafx.h"
#include "GameTest.h"
#include "PlayerInfo.h"
#include "WndMsgDef.h"
#include ".\playerinfo.h"
#include "../App/Player.h"
#include "../App/ClientRegion.h"
#include "TestInfo.h"
#include "../App/GameClient.h"
#include "../Script/ScriptSys.h"

#include "..\App\GoodsMgr.h"
#include "..\app\PacketMgr.h"
#include "..\app\DepotMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPlayerInfo, CDialog)
CPlayerInfo::CPlayerInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerInfo::IDD, pParent)
{
	bControl = false;
}

CPlayerInfo::~CPlayerInfo()
{
	list<staPackGoods*> p = GetPackGoods();
	list<staPackGoods*>::iterator it = p.begin();
	list<staPackGoods*>::iterator temp;
	if (!p.empty())
	{
		for (;it!=p.end();)
		{
			temp = it;
			++it;
			delete *temp;
		}
	}
}

void CPlayerInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PLAYE_NAME, m_staPlayerName);
	DDX_Control(pDX, IDC_STATIC_PALYER_ID, m_staPlayrID);
	DDX_Control(pDX, IDC_STATIC_PLAYE_Hp, m_staPlayerHp);
	DDX_Control(pDX, IDC_STATIC_PLAYE_MaxHp, m_staPlayerMaxHp);
	DDX_Control(pDX, IDC_STATIC_PLAYE_Mp, m_staPlayerMp);
	DDX_Control(pDX, IDC_STATIC_PLAYE_MaxMp, m_staPlayerMaxMp);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Exp, m_staPlayerExp);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Level, m_staPlayerLevel);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Money, m_staPlayerMoney);
	DDX_Control(pDX, IDC_STATIC_PLAYER_UpExp, m_staPlayerUpExp);
	DDX_Control(pDX, IDC_Goods_List, m_GoodsList);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Sex, m_staSex);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Stamina, m_staStamina);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Occupation, m_staOccupation);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Energy, m_staEnergy);
	DDX_Control(pDX, IDC_STATIC_PLAYER_CountryContribute, m_staCountryContribute);
	DDX_Control(pDX, IDC_STATIC_PLAYER_UpgradeExp, m_staUpgradeExp);
	DDX_Control(pDX, IDC_STATIC_PLAYER_Country, m_staCountry);
	DDX_Control(pDX, IDC_STATIC_PLAYER_RanOfNobCredit, m_staRanOfNobCredit);
	DDX_Control(pDX, IDC_STATIC_PLAYER_RankOfNobility, m_staRankOfNobility);
	DDX_Control(pDX, IDC_STATIC_PLAYER_SpouseID, m_staSpouseID);
	DDX_Control(pDX, IDC_STATIC_PLAYE_SocketID, m_staPlayerSocketId);
}


BEGIN_MESSAGE_MAP(CPlayerInfo, CDialog)
	ON_WM_PAINT()
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()
	

// CPlayerInfo 消息处理程序
BOOL CPlayerInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_GoodsList.InsertColumn(0,"显示名字",50,70,50);
	m_GoodsList.InsertColumn(1,"位置",10,30,10);
	m_GoodsList.InsertColumn(2,"物品ID",50,70,50);
	m_GoodsList.InsertColumn(3,"数量",50,50,50);
	m_GoodsList.InsertColumn(4,"背包编号",50,50,50);

	return TRUE;
}

void CPlayerInfo::SetDisplayPlayer(CGameClient* pSelGameClient)
{
	if ( pSelGameClient == NULL ) return;

	//玩家背包是否正被操作
	if (bControl==true)
	{
		return;
	}

	CPlayer* pPlayer = pSelGameClient->GetMainPlayer();
	m_pDisplayPlayer=pPlayer;
	if(m_pDisplayPlayer)
	{
 		char strSockID[64];
		sprintf(strSockID,"%d",pSelGameClient->GetSocketID());
 		m_staPlayerSocketId.SetWindowText(strSockID);

 		m_staPlayerName.SetWindowText(m_pDisplayPlayer->GetName());

 		char strID[128];
 		sprintf(strID,"%d",m_pDisplayPlayer->GetExID());
 		m_staPlayrID.SetWindowText("");

 		char strHp[128];
 		sprintf(strHp,"%d",m_pDisplayPlayer->GetHp());
 		m_staPlayerHp.SetWindowText(strHp);

 		char strMaxHp[128];
 		sprintf(strMaxHp,"%d",m_pDisplayPlayer->GetMaxHp());
 		m_staPlayerMaxHp.SetWindowText(strMaxHp);

 		char strMp[128];
 		sprintf(strMp,"%d",m_pDisplayPlayer->GetMp());
 		m_staPlayerMp.SetWindowText(strMp);

 		char strMaxMp[128];
 		sprintf(strMaxMp,"%d",m_pDisplayPlayer->GetMaxMp());
 		m_staPlayerMaxMp.SetWindowText(strMaxMp);

 		char strExp[128];
 		sprintf(strExp,"%d",m_pDisplayPlayer->GetExp());
 		m_staPlayerExp.SetWindowText(strExp);

 		char strPlayerUpExp[128];
 		sprintf(strPlayerUpExp,"%d",m_pDisplayPlayer->GetUpgradeExp());
 		m_staPlayerUpExp.SetWindowText(strPlayerUpExp);

 		char strPlayerLevel[128];
 		sprintf(strPlayerLevel,"%d",m_pDisplayPlayer->GetLevel());
 		m_staPlayerLevel.SetWindowText(strPlayerLevel);

 		char strMoney[128];
 		sprintf(strMoney,"%d",1/*m_pDisplayPlayer->GetPlayerMoneyNum()*/);
 		m_staPlayerMoney.SetWindowText(strMoney);

 		char strSex[64];
 		sprintf(strSex,"%d",m_pDisplayPlayer->GetSex());
 		m_staSex.SetWindowText(strSex);

 		char strStamina[128];
 		sprintf(strStamina,"%d",m_pDisplayPlayer->GetStamina());
 		m_staStamina.SetWindowText(strStamina);

 		char strOccupation[128];
 		sprintf(strOccupation,"%d",m_pDisplayPlayer->GetOccupation());
 		m_staOccupation.SetWindowText(strOccupation);

 		char strEnergy[128];
 		sprintf(strEnergy,"%d",m_pDisplayPlayer->GetEnergy());
 		m_staEnergy.SetWindowText(strEnergy);

 		char strCountryContribute[128];
 		sprintf(strCountryContribute,"%d",m_pDisplayPlayer->GetCountryContribute());
 		m_staCountryContribute.SetWindowText(strCountryContribute);

 		char strUpgradeExp[128];
 		sprintf(strUpgradeExp,"%d",m_pDisplayPlayer->GetUpgradeExp());
 		m_staUpgradeExp.SetWindowText(strUpgradeExp);

 		char strCountry[128];
 		sprintf(strCountry,"%d",m_pDisplayPlayer->GetCountry());
 		m_staCountry.SetWindowText(strCountry);

 		//char strRanOfNodCredit[128];
 		//sprintf(strRanOfNodCredit,"%d",m_pDisplayPlayer->GetRankOfNobility());
 		//m_staRanOfNobCredit.SetWindowText(strRanOfNodCredit);

 		//char strRanOfNobility[128];
 		//sprintf(strRanOfNobility,"%d",m_pDisplayPlayer->GetRankOfNobility());
 		//m_staRankOfNobility.SetWindowText(strRanOfNobility);

 		char strSpouse[128];
 		sprintf(strSpouse,"%d",m_pDisplayPlayer->GetSpouseID());
 		m_staSpouseID.SetWindowText(strSpouse);

		//char strGoods[128];
		m_GoodsList.DeleteAllItems();

		list<staPackGoods*> p = GetPackGoods();
		list<staPackGoods*>::iterator it = p.begin();
		list<staPackGoods*>::iterator temp;
		if (!p.empty())
		{
			for (;it!=p.end();)
			{
				temp = it;
				++it;
				delete *temp;
			}
		}
		m_pPackGoods.clear();

		int iRow(0);	//行号
		char strGoods[128];
		//背包
		for(int i=0;i<PACK_NUM;i++)
		{
			long _PacketListNum = m_pDisplayPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(i)->size();
			if(_PacketListNum != 0)
			{
				list<tagGoods>::iterator GoodsIter = m_pDisplayPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(i)->begin();

				for(;GoodsIter!= m_pDisplayPlayer->GetGoodsMgr()->GetPacketMgr()->GetGoodsList(i)->end();++GoodsIter)
				{	
					staPackGoods* pPackGoods = new staPackGoods;
					
					

					//显示名
					sprintf(strGoods,"%s",GoodsIter->pGoods->GetName());         
					m_GoodsList.InsertItem(iRow,strGoods);

					//位置
					sprintf(strGoods,"%d",GoodsIter->lPos);													
					m_GoodsList.SetItemText(iRow,1,strGoods);

					//ID
					sprintf(strGoods,"%s",CGoodsList::GetProperty(GoodsIter->pGoods->GetIndex())->BaseProperty.strOrginName.c_str());			
					m_GoodsList.SetItemText(iRow,2,strGoods);

					//数量
					sprintf(strGoods,"%d",GoodsIter->lNum);													 
					m_GoodsList.SetItemText(iRow,3,strGoods);

					sprintf(strGoods,"%d",i);
					m_GoodsList.SetItemText(iRow,4,strGoods);

					pPackGoods->guID = GoodsIter->pGoods->GetExID();
					pPackGoods->dwType = CGoodsList::GetProperty(GoodsIter->pGoods->GetIndex())->BaseProperty.dwType;
					pPackGoods->sOriName = CGoodsList::GetProperty(GoodsIter->pGoods->GetIndex())->BaseProperty.strOrginName;
					pPackGoods->lPackID = i;

					m_GoodsList.SetItemData(0,(DWORD_PTR)pPackGoods);
					m_pPackGoods.push_back(pPackGoods);
				//	++iRow;
				}
			}
		}

		//仓库
 		hash_map<CGUID,tagGoods,hash_guid_compare>* pList = m_pDisplayPlayer->GetGoodsMgr()->GetDepotMgr()->GetDepotGoodsList(DEPOT_MAIN_PACK);

 		if( pList && pList->size() >0 )
 		{
 			hash_map<CGUID,tagGoods,hash_guid_compare>::iterator itHash = pList->begin();
 
 			for(;itHash!= pList->end();++itHash)
 			{	
 				tagGoods GoodsIter = itHash->second;
 				staPackGoods* pPackGoods = new staPackGoods;
 
 				sprintf(strGoods,"%s",GoodsIter.pGoods->GetName());//GetBaseProperty()->strName.c_str());            //显示名
 				m_GoodsList.InsertItem(iRow,strGoods);
 				sprintf(strGoods,"%d",GoodsIter.lPos);													//位置
 				m_GoodsList.SetItemText(iRow,1,strGoods);
 				sprintf(strGoods,"%s",CGoodsList::GetProperty(GoodsIter.pGoods->GetIndex())->BaseProperty.strOrginName.c_str());			 //ID
 				m_GoodsList.SetItemText(iRow,2,strGoods);
 				sprintf(strGoods,"%d",GoodsIter.lNum);													 //数量
 				m_GoodsList.SetItemText(iRow,3,strGoods);
 				sprintf(strGoods,"%d",(long)eDCT_Primary);
 				m_GoodsList.SetItemText(iRow,4,strGoods);
 
 				pPackGoods->guID = GoodsIter.pGoods->GetExID();
 				pPackGoods->dwType = CGoodsList::GetProperty(GoodsIter.pGoods->GetIndex())->BaseProperty.dwType;
 				pPackGoods->sOriName = CGoodsList::GetProperty(GoodsIter.pGoods->GetIndex())->BaseProperty.strOrginName;
 				pPackGoods->lPackID = (long)eDCT_Primary;
 
 				m_GoodsList.SetItemData(0,(DWORD_PTR)pPackGoods);
 				m_pPackGoods.push_back(pPackGoods);
 				//	++iRow;
 			}
 
 		}
	}
	else
	{
		m_staPlayerName.SetWindowText("");
		m_staPlayrID.SetWindowText("");
	}
	bControl = false;
}

void CPlayerInfo::OnPaint()
{
	CPaintDC dc(this);
}