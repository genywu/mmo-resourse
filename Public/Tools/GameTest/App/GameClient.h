//=============================================================================
/**
*  file: Client.h
*
*  Brief:针对一个客户端的包装
*
*  Authtor:wangqiao
*	
*	Datae:20077-20
*/
//=============================================================================
#ifndef __Game_Client_H__
#define __Game_Client_H__
#pragma once
#include "../Utility/TimerDef.h"
#include "../Utility/FindPath.h"
#include "..\App\Goods.h"
#include "..\App\MoveShape.h"
#include "..\App\Monster.h"
#include "..\App\Player.h"
#include "..\App\CollectionGoods.h"
//#include "..\App\PetSystem\CFairyPet.h"
//#include "..\App\PetSystem\CFighterPet.h"
//#include "..\App\PetSystem\CSummonPet.h"
//#include "..\App\PetSystem\CPet.h"
//#include "..\App\PetSystem\CTrap.h"

//#include "..\Utility\PlayerDef.h"
#include "../App/CCDKey.h"
enum OPERATION_TYPE
{
	OT_ROLL_BACK		= 0,
	OT_MOVE_OBJECT,
	OT_NEW_OBJECT,
	OT_DELETE_OBJECT,
	OT_SWITCH_OBJECT,
	OT_SWITCH_FILL,
	OT_CHANGE_AMOUNT,
};

class CPlayer;
class CMoveShape;
class CClientRegion;
class CClientRgnObjMgr;

class CGameClient
{
public:
	CGameClient();
	CGameClient(Timer* pTimer);
	virtual ~CGameClient(void);
private:
	//主玩家实例
	CPlayer*	   m_pMainPlayer;
	long			m_lReGameCode;

	//玩家所在的场景实例
	CClientRegion* m_pRegion;

	//	场景对象管理器
	CClientRgnObjMgr*	m_pClientObjMgr;

	//对应的socketid
	long		   m_lSocketID;

	//cdkey
	string m_strCDkey;

	//时间定时器
	Timer*		   m_pTimer;
	long		   m_lTimeID;
	//  [8/11/2009 Square]
	//typedef list<CBaseObject*>		CListObject;
	//typedef CListObject::iterator	itListObject;
	//CListObject		m_listClientObject;
public:
	CBaseObject*	FindObjectFromClientRgn(long lType,const CGUID& guid);
	CBaseObject*	FindObjectFromClientRgn(const CGUID& guid);
	// 	virtual void DeleteChildObject(long typeID,const CGUID& gID);		// 删除子OBJECT
	// 	virtual void DeleteChildObject(CBaseObject* p);						// 删除子OBJECT
	// 	virtual void DeleteAllChildObject(CBaseObject* pExpect=NULL);		// 删除所有子OBJECT(pExpect = 除了)
	void RemoveAllObject();
	void RemoveObjectFromRgn(CBaseObject* pObject);					// 移出OBJECT
	void AddObjectToClientRgn(CBaseObject* pObject);						// 添加OBJECT

	void AddGoods(CGoods* pGoods,DWORD dwPos,LONG lExtendID);
	CGoods*	RemoveGoods(CGUID& guid, DWORD dwPos, LONG lExtendID, DWORD dwAmount);

	CShape* GetShape(long x, long y);
	CShape* GetShapeFromSpace(long x, long y, long lSize,OBJECT_TYPE eType, CShape* pShape);


	CClientRgnObjMgr*	GetClientRgnObjMgr()	{ return m_pClientObjMgr; }

protected:
	//MappingFile 线程Id
	unsigned        m_uID;
public:
	void			SetMemServeruID(unsigned id)	{ m_uID = id; }
	unsigned		GetMemServeruID()				{ return m_uID; }

private:
	CGUID	m_FollowObjectGuid;				//跟踪对象的GUID
	long	m_lDestX;						//移动目标X
	long	m_lDestY;						//移动目标Y
	long	m_lAimRegionID;					//目标场景ID

	long m_lLineNum;
	long m_lCurSelLineID;
	long m_lLineID;

	long m_lKilMotrNum;



	vector<long>	m_vecTemplateID;
	vector<CGUID>	m_vecRegionGuid;
public:
	long GetTempID(long lIndex)	{
		if( lIndex>=0 && lIndex < m_vecTemplateID.size() )
			return m_vecTemplateID[lIndex];
		return -1;
	}

	const CGUID& GetRegionGuid(long lIndex){
		if(lIndex>=0&& lIndex<m_vecRegionGuid.size())
			return m_vecRegionGuid[lIndex];
		return CGUID::GUID_INVALID;
	}

	long GetLineID()	{return m_lLineID;}
	long GetCurSelLineID() { return m_lCurSelLineID; }
	long GetLineNum()	{ return m_lLineNum; }

	void SetLineID(long id)	{ m_lLineID = id; }
	void SetCurSelLineID(long id)	{ m_lCurSelLineID = id; }
	void SetLineNum(long l)	{ m_lLineNum = l; }

	void DecordLineFromDataBlock(DBReadSet& db_r);

	void SetKilMotrNum(long num)  { m_lKilMotrNum = num; }
	long GetKilMotrNum()		  { return m_lKilMotrNum;}

public:
	//
	void		   SetMainClientTimer( long l)				{ m_lTimeID = l;}
	long		   GetMainClientTimer()						{ return m_lTimeID;}
	void		   SetDestX(long x)							{ m_lDestX = x;}
	void		   SetDestY(long y)							{ m_lDestY = y;}
	long		   GetDestX()								{ return m_lDestX;}
	long		   GetDestY()								{ return m_lDestY;}
	void	       SetAimRegionID(long l)					{ m_lAimRegionID = l;}
	long	       GetAimRegionID()							{ return m_lAimRegionID;}
	void	       SetFollowObjectGuid(const CGUID& guid)	{ m_FollowObjectGuid = guid;}
	const CGUID   &GetFollowObjectGuid()					{ return m_FollowObjectGuid;}
	void		   SetMainPlayer(CPlayer* pPlayer)			
	{ 
		m_pMainPlayer = pPlayer;
//		m_pMainPlayer->SetIsSelf(true);
	}
	CPlayer		  *GetMainPlayer()							{ return m_pMainPlayer; }

	long	GetReGameCode()			{ return m_lReGameCode; }
	void	SetReGameCode(long l)	{ m_lReGameCode = l; }

	void		   SetClientRegion(CClientRegion* pRegion)	{ m_pRegion = pRegion;}
	CClientRegion *GetClientRegion()						{ return m_pRegion;}
	void		   SetSocketID(long lSocketID)				{ m_lSocketID = lSocketID;}
	long		   GetSocketID()							{ return m_lSocketID ; }

	void		   SetCDKey(const char* cdkey)				{ m_strCDkey = cdkey;}
	const char    *GetCDKey()								{ return m_strCDkey.c_str();}

	CFindPath	  *GetFindPath()							{ return m_pRegion->GetFindPath(); }
private:
	DWORD m_dwAimConPos;

	//////////////////////////////////////////////////////////////////////////
	//	物品高级操作
	//////////////////////////////////////////////////////////////////////////
	int m_nCurOpenWindowsType;		//	打开的界面类型
	ePageType m_eCurOpenPageType;
public:
	void SetCurOpendWindowsType(int nType)	{ m_nCurOpenWindowsType = nType; }
	int GetCurOpendWindowsType()	{ return m_nCurOpenWindowsType; }

	void	  SetCurOpenPageType(ePageType e)		{ m_eCurOpenPageType = e; }
	ePageType GetCurOpenPageType()					{ return m_eCurOpenPageType; }

private:
	//玩家是否在移动 for DriveClient
	// by MartySa 2009.2.7
	bool			  m_bIsPlayerMove;

public:
	void			  SetPlayerIsMove(bool b)	{ m_bIsPlayerMove = b; }
	bool			  GetPlayerIsMove()			{ return m_bIsPlayerMove; }
	//--------------------------------------------------------------------------------------
	//物品升级
	//--------------------------------------------------------------------------------------
public:
	tagUpgradeStuff *GetUpgradeStuff(void)			{ return &m_UpgradeStuff; }
private:
	tagUpgradeStuff m_UpgradeStuff;
	//------------------------------------------------------------------------------------
	//镶卡
	//------------------------------------------------------------------------------------
private:
	tagEnchaseSession m_tagEnchaseSession;
	tagEnchaseGoods	 *m_pEnchaseGoods;

public:
	void SetEnchaseBaoShi(CGoods* pBaoShi)			{ m_pEnchaseGoods->pBaoShi = pBaoShi; }
	void SetEnchaseGoods(CGoods* pGoods)			{ m_pEnchaseGoods->pGoods = pGoods; }
	void SetEnchaseCard(CGoods* pCard,int nIndex)	{ m_pEnchaseGoods->pCard[nIndex] = pCard; }
	void SetEnchaseSessionGuid(const CGUID& guid)	{ m_tagEnchaseSession.m_SessionGuid = guid;}
	void SetEnchasePlayerID(long id)				{ m_tagEnchaseSession.lPlayerID = id; }
	void SetEnchasePageType(BYTE b)					{ m_tagEnchaseSession.iPageType = b; }
	void SetEnchaseAimConPosForMsg(DWORD msgpos)	{ m_dwAimConPos = msgpos; }		

	CGoods		*GetEnchaseBaoshi()					{ return m_pEnchaseGoods->pBaoShi; }
	CGoods		*GetEnchaseGoods()					{ return m_pEnchaseGoods->pGoods; }
	CGoods		*GetEnchaseCard(int nIndex)			{ return m_pEnchaseGoods->pCard[nIndex]; }
	const CGUID &GetEnchaseSessionGuid()			{ return m_tagEnchaseSession.m_SessionGuid; }
	long	     GetEnchasePlayerID()				{ return m_tagEnchaseSession.lPlayerID; }
	BYTE		 GetEnchasePageType()				{ return m_tagEnchaseSession.iPageType; }
	DWORD        GetEnchaseAimConPosForMsg()		{ return m_dwAimConPos; }
private:
	bool bOpenFacturePage;		//制作界面是否打开
	bool bOpenTalkBoxPage;		//TalkBox界面
	bool bOpenTradePage;		//交易界面
	bool bOpenTreasureShopPage; //增值商店界面
	bool bOpenNPCShopPage;		//NPC商店界面
	bool bOpenPlayerShopPage;	//个人商店界面
	bool bOpenDepotPage;		//仓库界面
	bool bOpenComPage;
	bool bOpenDecomPage;
	bool bOpenEquipUpgradePage;	//装备升级界面
	bool bOpenEquipMagicPage;	//装备魔化界面
	bool bOpenMagicDecomPage;	//装备魔化分解
	//------------------------------------------------------------------------------------
	//物品合成
	//------------------------------------------------------------------------------------
	//! C2S消息用 合成物数组结构
	//这里+5 是为了测试服务器的错误处理

private:
	tagSynthesizeGoods  m_tagSynthesizeGoods;	
	long			    m_lPrescriptionIndex;		//配方

public:
	tagSynthesizeGoods &GetSynthesizeGoods()										{ return m_tagSynthesizeGoods; }
	void			    SetSynthesizeGoods(tagGoods *pstGoods, int index);
	void				ClearSynthesizeGoods();		//发送合成协议后 可以选择清空当前结构体
	void				SetSynPrescriptionIndex(long lIndex)						{ m_lPrescriptionIndex = lIndex; }
	long				GetSynPrescriptionIndex()									{ return m_lPrescriptionIndex; }
	//------------------------------------------------------------------------------------
	//材料合成
	//------------------------------------------------------------------------------------
private:
	tagEvilStuff	m_tagEvilStuff;

public:
	tagEvilStuff	&GettagEvilStuff()								{ return m_tagEvilStuff; }
	void			 SettagEvilStuff(tagGoods *taggoods);
	void			 CleartagEvilStuff();			//发送合成协议后 可以选择清空当前结构体
	//------------------------------------------------------------------------------------
	//// Ping Server add By MartySa
	//------------------------------------------------------------------------------------
private:

	DWORD m_dwLastPingTime;			// 上次PingSrv的时间
	DWORD m_dwBlankPingTime;		// PingSrv间隔
	bool  m_bIsCanPingSrv;			// 是否发送ping消息

public:

	void  SetBlankPingTime(DWORD Bktime) { m_dwBlankPingTime = Bktime; /*60*1000*10*/}	//设置间隔时间
	bool  GetIsCanPingSrv()	{
		DWORD dwNowTime = ::timeGetTime();
		if ( dwNowTime - m_dwLastPingTime > m_dwBlankPingTime )
		{
			m_dwLastPingTime = dwNowTime; 
			return true;
		}
		return false;
	}
	DWORD GetPingSrvSendVal();												//获取PingSrv值	

	///////
	//----------------------------------------------------------------------------------------------------------------
	// client S2C消息处理
public:
	virtual void OnMessage(CMessage* pMsg);
	//	宠物
	void OnPetMessage(CMessage* pMsg);
	//
	void	OnShapeMessage(CMessage* pMsg);
	//	赠送
	void OnBounsMessage(CMessage* pMsg);
	//	
	void OnContainerMsg(CMessage* pMsg);
	//	
	void OnOtherMessage(CMessage* pMsg);

	void OnSkillMessage(CMessage* pMsg);

	void OnPlayerMessage(CMessage* pMsg);

	void OnGoodsMessage(CMessage* pMsg);

	//Player相关S2C处理
	void S2CPlayer(CMessage* pMsg);
	void PlayerLevelUp(CMessage* pMsg);
	void PlayerUpdateProperty(CMessage* pMsg);
	void PlayerAddSkill(CMessage* msg);
	void PlayerAnswerRelive(CMessage* msg);
	void PlayerUseItem(CMessage* msg);
	//
	void OnReginMessage(CMessage* pMsg);
	//	void S2CRegion(CMessage* pMsg);
	//	void RegionAddShape(CMessage* msg);
	//	void RegionDelShape(CMessage* msg);
	//	virtual void RegionChanged(CMessage *msg);
	//	virtual void RegionChangeServer(CMessage* msg);
	//

	//	void S2CContainer(CMessage* pMsg);
	//	void ContainerObjectMove(CMessage* msg);
	//	void ContainerObjectAmountChange(CMessage* msg);
	//
	//	void S2CSkill(CMessage* pMsg);
	//	void SkillUse(CMessage* msg);
	//	void SkillStateChange(CMessage* msg);
	//
	void S2COther(CMessage* msg);
	void AnswerNpcList(CMessage* msg);
	//
	void S2CGoods(CMessage* msg);
	void OpenGoodsEnchase(CMessage* msg);								//打开卡片镶嵌功能界面
	void OpenGoodsHandleWindow(CMessage* msg);
	void AnswerDepotPwdInput(CMessage *msg);
	void OrderDepotClose(CMessage* msg);
	//组队消息
	void OnTeamMessage(CMessage* pMsg);
	void S2CTeam(CMessage* msg);
	void TeamCreat(CMessage *msg);										//创建队伍的时候发送
	void TeamData(CMessage *msg);		
	void TeamMemberData(CMessage *msg);
	void TeamQueryJion(CMessage* msg);
	void TeamMemAllInfo(CMessage* msg);
	void TeamMemberLeave(CMessage* msg);								//! 成员离队
	void TeamMastterChange(CMessage* msg);								//! 队长改变
	void TeamRecruit(CMessage* msg);									//! 更新招募状态
	void TeamRecruitedNum(CMessage* msg);								//! 更新已招募的人数
	void TeamAnswerJoin(CMessage* msg);								//回复邀请加入
	void TeamSynchronize(CMessage* msg);								//同步队伍的信息
	void TeamMatChangeRegion(CMessage* msg);
	void TeamChangeLeader(CMessage* msg);								//改变队长
	void TeamChangeState(CMessage* msg);
	//添加好友消息
	void S2CLinkMan(CMessage* msg);
	void LinkManInit(CMessage* msg);
	void LinkManAdd(CMessage* msg);
	void DelLinkman(CMessage* msg);
	void RemoveLinkMan(CMessage* msg);
	//交易
	void PlayerTradeRequest(CMessage* msg);
	void PlayerTradeBegin(CMessage* msg);
	void PlayerTradeLock(CMessage* msg);
	void PlayerTradeFinish(CMessage* msg);
	//NPC商店
	void S2CShop(CMessage* msg);
	void ShopOpen(CMessage* msg);
	void ShopBuy(CMessage* msg);
	// 家族,行会[12/22/2008 Square]
	void OnFamilyMessage(CMessage* pMsg);
	void DecordFamilySetup(BYTE*pData,long& lPos);
	//玩家个人商店
	void S2CPlayerShop(CMessage* msg);
	void PlayerShopCreate(CMessage* msg);
	void OnPlayerShopToBuyer(CMessage* pMsg);
	//邮件
	void S2CMail(CMessage* msg);
	void MailReceiver(CMessage* msg);
	void MailSendFailed(CMessage* msg);
	void MailSend(CMessage* msg);
	void MailRead(CMessage* msg);
	void MailFailedMoveGoods(CMessage* msg);
	void MailMoveGoodsResult(CMessage* msg);
	void MailDeleteResult(CMessage* msg);
	void MailDeleteOk(CMessage* msg);
	void MailAgreedPay(CMessage* msg);
	void MailAgreedPresent(CMessage* msg);
	void MailGetGoldResult(CMessage* msg);
	void MailPersentFailed(CMessage* msg);

	//玩家增值商店
	void S2CIncrementShop(CMessage* msg);
	void IncrementShopOpen(CMessage* msg);
	// 小精灵 [12/30/2008 Square]
	void OnSpriteMessage(CMessage* pMsg);
	//玩家搜索
	void S2CPlayerManage(CMessage* msg);
	void PlayerManageAtk(CMessage* msg);
	//学习副职业生产技能
	void OccuskillStudyBegin(CMessage* msg);
	void OccuskillStudySuccess(CMessage* msg);

	//回答问题
	void AntiCheat(CMessage *msg);
	void AntiCheatQuestion(CMessage *msg);


	//////////////////////////////////////////////////////////////////////////
	//	Login
	void OnLoginMessage(CMessage* pMsg);
};
#endif