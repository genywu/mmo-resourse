#include "stdafx.h"
#include "Game.h"
#include "GameControl.h"
#include "../GameClient/ClientApp/ClientRegion.h"
#include "../GameClient/ClientApp/PetSystem/Pet.h"
#include "../GameClient/ClientApp/MainPlayerHand.h"
#include "../GameClient/ClientApp/Skills/SkillXml.h"
#include "../GameClient/ClientApp/Skills/SkillListIni.h"
#include "../GameClient/ClientApp/Monster.h"
#include "../GameClient/ClientApp/CollectionGoods.h"
#include "../GameClient/ClientApp/Other/DeputyOccuSystemClient.h"
#include "../GameClient/ClientApp/Other/StateListXml.h"
#include "../GameClient/ClientApp/MiniMap.h"
#include "../GameClient/ClientApp/Other/RegionList.h"
#include "../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../Public/Common/SkillRelated.h"
#include "../../Public/Common/DepotDef.h"

#include "../../UI/UIDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//一次移动的最大格子数
#define MAX_MOVE_CELLNUM	2

CGameControl::CGameControl(void)
{
    //pMainPlayerHand = NULL;
    m_dwMoveMsgCounter = 0;
    m_fParameterOfDisFromCamara = 1.125f;
    m_FloatMove = new CFloatMove;
    dwBegionMoveTime = 0;
    ReNew();
}

CGameControl::~CGameControl(void)
{
    //SAFE_DELETE(pMainPlayerHand);
    SAFE_DELETE(m_FloatMove);
}

void CGameControl::ReNew()
{
    m_FollowLeader = false;
    m_FollowDestX = 0.0f;
    m_FollowDestY = 0.0f;
    m_FollowRegionID = 0;
    pRegion = NULL;
    pMainPlayer = NULL;
    //if (pMainPlayerHand==NULL)
    //{
    //	pMainPlayerHand = new CMainPlayerHand;
    //}
    m_bIsInTryOnState = false;
    m_bControlAble = true;
    m_bMoveAble = true;
    m_bIntonateBreak = false;
    m_lSendedMoveMsgCount = 0;
    m_bChangingMoveMode = 0;
    m_lSendedUseSkillMsgCount = 0;
    m_lLastControlTimer = 0;
    m_dwLeftHoldingTime = 0;
    m_dwRightHoldingTime = 0;
    m_dwSelectedShapeStartTime = 0;
    m_pMouseShapeGoods = NULL;				// 鼠标指向的物品
    m_pMouseShape = NULL;					// 鼠标指向的对象
    m_pSelectedShape = NULL;				// 当前选中的对象
    m_pCurAttractShape = NULL;				// 当前正在被攻击的对象
    m_pPressRBtnShape = NULL;				// 当前按下右键指向的对象

    m_lSelectedType = 0;
    m_guidSelected = CGUID::GUID_INVALID;

    m_bIsTabTeamMate = false;

    m_lFollowingObjType = 0;
    m_FollowingObjID = CGUID::GUID_INVALID;
    m_PickGoodsID = NULL_GUID;
    //m_CollectGoodsID = NULL_GUID;
    m_lFollowMaxDistance = 1;
    m_fStartDestX = 0;
    m_fStartDestY = 0;
    m_fDestX = 0.0f;
    m_fDestY = 0.0f;
    m_MainPlayerActionMode = MPAM_No;
    m_AutoMoveMode = AMM_NO;
    m_bResetAutoMoveMode = false;
    m_AutoAttactMode = AAM_NO;

    m_NextMainPlayerActionMode = MPAM_No;

    m_dwBufferSkillID = 0;
    m_dwAutoSkillID = 0;
    m_nLastPressHotKey = 0;
    m_dwCurSkillID = 0;
    //m_dwGoodsIndexForSkill = 0;
    m_lSkillGoodsExtendID = 0;
    m_lSkillGoodsPos = -1;
    m_lTargetRegionID = 0;
    m_lSPosX = 0;
    m_lSPosY = 0;
    m_lTPosX = 0;
    m_lTPosY = 0;

    m_dwLastPingTime = timeGetTime();
    m_dwLocalIP =  0;

    SetUpdateXY(0,0);
}

// 控制
void CGameControl::Run()
{
    pRegion = GetGame()->GetRegion();
    pMainPlayer = GetGame()->GetMainPlayer();

    if (pRegion == NULL || pMainPlayer == NULL)
        return;

    PingServer();

    if(pMainPlayer->IsDied())
        m_AutoMoveMode = AMM_NO;

    // 刷新技能冷却时间
    pMainPlayer->UpdateSkillCoolDownTime();
    //if(GetGame()->GetCGuiEx()->GetMainBarPageEx())
    //	GetGame()->GetCGuiEx()->GetMainBarPageEx()->UpdataHotCompCoolEffect();
    // 如果一段时间没有收到服务器返回的操作响应，那么判定为操作失败
    if (IsControlAble()==false && GetLastControlTimer() + 500 < (long)GetCurTickCount() )
    {
        SetControlAble(true);
    }
    // 视角控制
    pRegion->GetRegionCamera()->Run();
    // 不能进行任何操作
    if(IsEveryThingNotToDo())
    {
        return;
    }
    //////////////////////////////////////////////////////////////////////////
    //	操作处理
    //////////////////////////////////////////////////////////////////////////
    OnMouseEvent();					// 鼠标处理

    OnAnotherEvent();				// 其它处理（自动处理等）
    OnKeyboardEvent();				// 键盘处理

    // 向服务器发送选择目标
    CShape *pSelect = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lSelectedType, m_guidSelected));
    if(m_pSelectedShape && m_CurSelectedShape!= m_pSelectedShape )
    {
        SetCurLogSelectShape(m_pSelectedShape);
        FireUIEvent("ObjectPage", EVENT_OPEN);
        FireUIEvent("ObjectPage", "ObjectAttr");
    } 

    // 开始记录选中目标
    if(m_dwSelectedShapeStartTime > 0)
    {
        DWORD time = timeGetTime() - m_dwSelectedShapeStartTime;
        if(time > 1000)
        {
            // 向服务器发送选择目标
            CShape *pSelect = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lSelectedType, m_guidSelected));
            if(NULL == pSelect)
            {
                m_lSelectedType = 0;
                m_guidSelected = CGUID::GUID_INVALID;
            }
            // 向服务器发送当前学则的对象
            CMessage msg(MSG_C2S_SHAPE_SETSELECTEDSHAPE);
            msg.Add(m_lSelectedType);
            msg.Add(m_guidSelected);
            msg.Send();
            m_dwSelectedShapeStartTime = 0;
        }
    }
    //CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
    //if (pItemPage!=NULL)
    //{
    //	pItemPage->UpdataGoodsCoolEffect();
    //}

    /// 当处于背包整理状态时就继续执行下去。。。直到整理完毕。。
    static DWORD dwStarTime = 0;
    DWORD dwTime = GetCurTickCount() - dwStarTime;
    if (dwTime > CGlobeSetup::GetSetup()->dwSendFinishBagDelayTime )
    {
        /*if (pItemPage->CanFinishBag()&&pItemPage->GoodsSort())
        {
        dwStarTime = GetCurTickCount();
        pItemPage->SendMsgForFinishBag();
        }*/
    }

    // 王峰：仓库整理
    /*if( CDepotPageEx* pDepotPageEx = GetGame()->GetCGuiEx()->GetDepotPageEx() )
    {
    pDepotPageEx->SendSortMsg();
    }*/
}

// 鼠标处理
long CGameControl::OnMouseEvent()
{
    //if(!IsCanMoveCondition())
    //	return 0;
    // 有模态对话框弹出
    //IGUIRoot *pRoot = IGUIRoot::GetGUIInstance();
    //if(pRoot && pRoot->GetGUIComp()->GetLock())
    //	return 0;
    switch(GetMouse()->GetCurrentButton())
    {
    case LB_DOWN:
        m_dwLeftHoldingTime = GetCurTickCount();
        OnLeftButtonDown();
        return 1;
    case LB_UP:
        m_dwLeftHoldingTime = 0;
        OnLeftButtonUp();
        return 1;
    case LB_DCLICK:
        m_dwLeftHoldingTime = 0;
        OnLeftDoubleButtonDown();
        return 1;
    case RB_DOWN:
        m_dwRightHoldingTime = 0;
        OnRightButtonDown();
        return 1;
    case RB_UP:
        m_dwRightHoldingTime = 0;
        OnRightButtonUp();
        return 1;
    case RB_DCLICK:
        m_dwRightHoldingTime = 0;
        OnRightDoubleButtonDown();
        return 1;
    default:
        break;
    }
    // 鼠标左键按住
    if (GetMouse()->IsLeftButtonDown())
    {
        OnLeftButtonHolding();
    }
    // 鼠标右键按住
    if (GetMouse()->IsRightButtonDown())
    {
        OnRightButtonHolding();
    }
    return 1;
}

// 键盘处理
long CGameControl::OnKeyboardEvent()
{
    if(GetGame()->GetIGWFocus())    //如果焦点在IGW上，则不响应快截操作。
        return 0;

    // 有模态对话框弹出
    //IGUIRoot *pRoot = IGUIRoot::GetGUIInstance();
    //if(pRoot && pRoot->GetGUIComp()->GetLock())
    //	return 0;


    // 快捷键捡物品
    if (GetKeyboard()->GetCurrentKey() == DIK_SPACE)
    {
        if (IsCanPickupGoodsCondition())
        {
            bool bIsPic = false;
            for (int i=0; i<9; i++)
            {
                CGoods *pGoods = pRegion->GetShapeGoods(pMainPlayer->GetTileX() + _Pick_Order[i][0], pMainPlayer->GetTileY() + _Pick_Order[i][1]);
                if (pGoods)
                {
                    AutoPickGood(pGoods->GetExID());
                    bIsPic = true;
                    break;
                }
            }
            // 在邻近格子没有可拾取的物品，向外一层的范围查找物品
            if(!bIsPic)
                AutoPickGoodEx();
        }
    }
    //跑、行走方式切换，在无输入框的情况下
    else if (GetKeyboard()->GetCurrentKey() == DIK_R && GetKeyboard()->IsKeyDown(DIK_LCONTROL))
    {
        GetKeyboard()->SetCurrentKey(0);
        if(GetMPActionMode() != MPAM_No)
        {
            //停止自动攻击和自动移动
            GetGame()->GetGameControl()->AutoAttactStop();
            GetGame()->GetGameControl()->AutoMoveStop();
            SetChangingMoveMode(true);
        }
        else
        {
            ChangeMoveMode();
        }
    }
    // 按TAB快捷选怪
    else if(GetActiveWindow() == g_hWnd && pRegion && pMainPlayer && GetKeyboard()->GetCurrentKey() == DIK_TAB)
    { 
        bool bEFlag = GetGame()->GetTabSet()->bEnemyPlayer;			// 敌国玩家
        bool bPFlag = GetGame()->GetTabSet()->bPiratePlayer;		// 海盗玩家
        bool bMFlag = GetGame()->GetTabSet()->bMonster;				// 怪物
        bool bPetFlag = GetGame()->GetTabSet()->bPet;				// 宠物
        if(!bMFlag && !bEFlag && !bPFlag && !bPetFlag && !m_bIsTabTeamMate)		// Tab功能关闭
            return 1;

        long x = pMainPlayer->GetTileX();
        long y = pMainPlayer->GetTileY();
        long lDir = pMainPlayer->GetDir();
        long lType = -1;
        long lOffLen = 1;
        CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
        if(pSetup)
            lOffLen = pSetup->lAreaByTab;
        // 判断搜索条件
        if(((bEFlag || bPFlag) && !bMFlag) || m_bIsTabTeamMate)
            lType = TYPE_PLAYER;
        else if(bMFlag && !bEFlag && !bPFlag && !m_bIsTabTeamMate)
            lType = TYPE_MONSTER;
        else if(bPetFlag)
            lType = TYPE_PET;

        // 根据条件搜索对象
        CShape *pShape = pRegion->GetLeastShape(m_lsSelectedShape,x,y,lDir,lOffLen,lType);
        if(pShape)
        {
            SetSelectedShape(pShape);
            // 关闭菜单
            //CPopMenuPageEx *pMenu = GetGame()->GetCGuiEx()->GetCPopMenuPageEx();
            //if(pMenu && pMenu->IsOpen())
            //    pMenu->Close();
        }
    }
    // 按'F'快捷选择当前选中对象的选择对象
    else if (m_pSelectedShape && GetKeyboard()->GetCurrentKey() == DIK_F)
    {
        // 向服务器发送请求，获得当前选择对象的选择对象
        CMessage msg(MSG_C2S_SHAPE_GETTSELECTEDSHAPE);
        msg.Send();
    }

    // 处理宠物快捷键
    // @todo 王晓轩
    CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
    CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
    if(pPet)
        pPet->ProcessPetHotKey();

    // 快捷键处理
    ProcessHotKey();
    return 0;
}


// 其它处理（自动处理等）
long CGameControl::OnAnotherEvent()
{
    //如果当前没有行为模式,则取出缓存的行为模式执行
    if( GetMPActionMode() == MPAM_No)
        OnNextPlayerActionMode();
    switch(GetMPActionMode())
    {
    case MPAM_AutoMove:
        {
            OnMainPlayerMove();
        }
        break;
    case MPAM_AutoAttack:
        {
            OnMainPlayerAttack();
        }
        break;
    case MPAM_AutoPickGoods:
        {
            OnAutoPickupGoods();
        }
        break;
    }

    return 1;
}

long CGameControl::OnAutoPickupGoods()
{
    CGoods* pGood = dynamic_cast<CGoods*>(pRegion->FindChildObject(TYPE_GOODS, m_PickGoodsID));
    if(pGood)
        PickupGoods(pGood);
    m_PickGoodsID = NULL_GUID;
    SetMPActionMode(MPAM_No);
    return 1;
}

//long CGameControl::OnAutoCollect()
//{
//	CMonster* pMonster = dynamic_cast<CMonster*>(pRegion->FindChildObject(TYPE_MONSTER, m_CollectGoodsID));
//	CCollectionGoods* pCollectGood = dynamic_cast<CCollectionGoods*>(pRegion->FindChildObject(TYPE_COLLECTION, m_CollectGoodsID));
//	if(pCollectGood!=NULL)
//	{
//		DoCollect(pCollectGood);
//	}else if (pMonster!=NULL&&pMonster->IsDied())
//	{
//		DoCollect(pMonster);
//	}
//	m_CollectGoodsID = NULL_GUID;
//	SetMPActionMode(MPAM_No);
//	return 1;
//}

// 是否不允许任何操作
bool CGameControl::IsEveryThingNotToDo()
{
    // 死亡或者有异常状态，不允许任何操作，且终止所有自动控制操作
    if(pMainPlayer->IsDied() || !pMainPlayer->IsInUnControlState())
    {
        SetFollowingObj(0,CGUID::GUID_INVALID);
        SetBufferSkillID(0);		// 取消缓冲技能
        SetAutoSkillID(0);			// 取消锁定技能
        return true;
    }
    return false;
}

// 是否可以操作的状态
bool CGameControl::IsCanControlCondition()
{
    //if (
    //	pMainPlayer->IsPlayerShopOpened()									// 打开开个人商店
    //	|| GetGame()->GetCGuiEx()->GetPlayerShopPageEx()->IsOpen()			// 打开开个人商店
    //	//	|| (GetGame()->GetInterface()->GetPlayerShopPage())->IsActive()		// 个人商店界面
    //	//	|| (GetGame()->GetInterface()->GetShopPage())->IsActive()			// 商店界面
    //	//	|| (GetGame()->GetInterface()->GetEquipUpgradePage())->IsActive()	// 升级装备界面
    //	//	|| (GetGame()->GetInterface()->GetDepotPage())->IsActive()			// 仓库界面
    //	//	|| (GetGame()->GetInterface()->GetTradePage())->IsActive()			// 交易界面
    //	//	|| GetGame()->GetInterface()->GetTradePage()->GetTradeState()		// 交易中
    //	//	|| GetGame()->GetInterface()->GetIncrementShopPage()->IsActive()	// 增值商品中
    //	|| GetGame()->GetCGuiEx()->GetPresentPageEx()->IsOpen()				// 领奖界面
    //	)
    //{
    //	return false;
    //}

    // 不可操作的状态
    if (IsControlAble() == false)
    {
        return false;
    }

    // 处于异常状态
    if (pMainPlayer->IsInUnControlState() == false)
    {
        return false;
    }

    return true;
}

// 是否可以行走的状态
bool CGameControl::IsCanMoveCondition()
{
    // 处于无法控制的动作
    /*if (pMainPlayer->GetAction() == CShape::ACT_ATTACK ||
    pMainPlayer->GetAction() == CShape::ACT_BEATTACK)
    return false;*/

    // 处于无法控制的状态
    //if (
    //	pMainPlayer->IsPlayerShopOpened()								// 打开开个人商店

    // 不可操作的状态,恐惧异常状态
    if (!IsControlAble() || !pMainPlayer->IsInDreadState())
    {
        return false;
    }

    return true;
}

// 是否可以捡起物品的状态
bool CGameControl::IsCanPickupGoodsCondition()
{
    // 处于异常状态
    if (GetGame()->GetMainPlayer()->GetIntonateKey()!=0 ||
        pMainPlayer->IsInUnControlState() == false||pMainPlayer->GetAction()==CShape::ACT_PICK)
    {
        return false;
    }
    return true;
}

// 是否可以使用物品的状态
bool CGameControl::IsCanUseGoodsCondition(DWORD dwGoodsIndex)
{
    // 处于异常状态
    if (!pMainPlayer->IsInUnControlState())
    {
        return false;
    }

    // 不能使用物品
    if (!pMainPlayer->IsCanUseItem())
    {
        return false;
    }

    return true;
}

// 是否可以使用物理攻击技能的状态
bool CGameControl::IsCanUsePhysicsSkillCondition()
{
    // 不可操作的状态
    if (!IsControlAble())
    {
        return false;
    }

    // 处于异常状态
    if (!pMainPlayer->IsInUnControlState())
    {
        return false;
    }

    // 不能使用物理技能
    if (!pMainPlayer->IsCanUseFighterSkill())
    {
        return false;
    }

    return true;
}

// 是否可以使用魔法攻击技能的状态
bool CGameControl::IsCanUseMagicSkillCondition()
{
    // 不可操作的状态
    if (!IsControlAble())
    {
        return false;
    }

    // 处于异常状态
    if (!pMainPlayer->IsInUnControlState())
    {
        return false;
    }

    // 不能使用魔法
    if (!pMainPlayer->IsCanUseRabbSkill())
    {
        return false;
    }

    return true;
}

void CGameControl::GetUpdateXY(float &fX,float &fY)
{
    if(m_fUpdateX == 0 && m_fUpdateY == 0 && pMainPlayer)
    {
        //fX = pMainPlayer->GetTileX();
        //fY = pMainPlayer->GetTileY();
		fX = pMainPlayer->GetPosX();
		fY = pMainPlayer->GetPosY();
        return;
    }	
    fX = m_fUpdateX;
    fY = m_fUpdateY;
}



/*
* 功能: 判断当前动作是否可以移动
* 摘要: -
* 参数: -
* 返回值: 是否可以移动
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.11.11 - lpf
*		增加了仪态动作可以移动的判断条件
*/
bool CGameControl::IsMoveWithCurAction()
{
    if (pMainPlayer->GetAction() == CShape::ACT_STAND || 
        pMainPlayer->GetAction() == CShape::ACT_IDLE || 
        pMainPlayer->GetAction() == CShape::ACT_BEATTACK ||
        pMainPlayer->GetAction() == CShape::ACT_BLOCK ||
        pMainPlayer->GetAction() == CShape::ACT_PARRY ||
        pMainPlayer->GetAction() == CShape::ACT_MISS || 
        pMainPlayer->GetAction() == CShape::ACT_INTONATE ||
        (pMainPlayer->GetAction() >= CShape::ACT_FACEACTION && pMainPlayer->GetAction() <= CShape::ACT_FACEACTIONMAX) ||               //仪态动作移动可以
        pMainPlayer->GetIntonateKey())
    {
        return true;
    }
    return false;
}

// 判断当前动作是否可以攻击
bool CGameControl::IsActWithCurAction()
{
    WORD wAct = pMainPlayer->GetAction();
    if (IsMoveWithCurAction())
    {
        return true;
    }
    // 处于施法动作状态，如果该施法动作的规定时间达到，可以攻击
    if(wAct >= CShape::ACT_PERFORM && wAct <= CShape::ACT_MAX_PERFORM)
    {
        // 规定动作达到
        if(GetGame()->GetMainPlayer()->IsCurSkillActTimeFinished())
            return true;
    }
    return false;
}

// 添加移动过的格子
//返回值:false:表示已经存在,true:不存在该格子
bool	CGameControl::AddMovedCell(long lTileX,long lTileY)
{
    //return TRUE;
    static long lLastAddX = 0;
    static long lLastAddY = 0;
    if(lLastAddX == lTileX && lLastAddY == lTileY)
    {
        return true;
    }
    lLastAddX = lTileX;lLastAddY=lTileY;
    POINT point = {lTileX,lTileY};
    itMovedCell it = m_MovedCell.find(point);
    if(it != m_MovedCell.end())
    {
        if( (*it).second >= 2)
            return false;
        (*it).second++;
    }
    else
    {
        m_MovedCell[point]=1;
    }
    return true;
}

bool CGameControl::IsResetAutoMoveMode()
{
    if(m_bResetAutoMoveMode)
    {
        long lTileX = pMainPlayer->GetTileX();
        long lTileY = pMainPlayer->GetTileY();
        if( pRegion->GetBlock(lTileX, lTileY) != CRegion::BLOCK_NO && 
            pRegion->GetBlock(lTileX, lTileY) != CRegion::BLOCK_AIM)
        {
            m_bResetAutoMoveMode = false;
        }
    }
    return m_bResetAutoMoveMode;
}
// 主角移动到达目标点处理
long CGameControl::OnMainPlayerMove()
{	
    // 1格同步或可行走动作，可移动
    if ( IsMoveWithCurAction() ||	//可行走动作
        pMainPlayer->IsArrive()  ||  //到达新格子中心
        IsResetAutoMoveMode() )//重置移动模式
    {
        //移动测试代码,wq,2008-2-20
        /*DWORD dwCurTime = GetCurTickCount();
        char strInfo[512]="";
        _snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_0),
        dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
        PutStringToFile("MoveTest",strInfo);*/

        long ret = 1;
        //如果要停止移动
        if(!pMainPlayer->IsInUnMoveState() || (GetAutoMoveMode()&AMM_STOP))
        {
            SetAutoMoveMode(AMM_NO);
            ret = 0;
        }

        switch(GetAutoMoveMode())
        {
        case AMM_A_SEARCH:
            {
                ret = AutoMoveAI();
            }
            break;
        case AMM_TO_DESTDIR:
            {
                ret = AutoMoveAIEx();
            }
            break;
        case AMM_FOLLOW_MOUSE:
            {
                ret = FollowMouseMove();
            }
            break;
        case AMM_FOLLOW_OBJECT:
            {
                ret = FollowObjectAI();
            }
            break;
        };

        //设置相关参数
        SetResetAutoMoveMode(false);

        // 王峰：处理玩家移动对 界面的影响

        // 恢复到站立
        if (ret == 0)
        {
            m_MovedCell.clear();
            if (pMainPlayer->GetAction() == CShape::ACT_WALK ||
                pMainPlayer->GetAction() == CShape::ACT_RUN)
            {
                pMainPlayer->SetAction( CShape::ACT_STAND );
                //停止移动
                StopMove();
            }
            //处理跟随结果
            if(GetAutoMoveMode() == AMM_FOLLOW_OBJECT)
                DoFollowResult();
            if(m_lFollowingObjType != TYPE_PLAYER || m_NextMainPlayerActionMode != MPAM_No)
                SetMPActionMode(MPAM_No);
            if(GetChangingMoveMode())
            {
                SetChangingMoveMode(false);
                ChangeMoveMode();
            }
            // 取消在大地图上显示的目标点标示
            /*CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
            if(pMiniMap)
            pMiniMap->SetDestPos();*/
           
        }
    }
    //更新地图界面玩家位置
    FireUIEvent("WorldMap","MAP_EVENT_ONPLAYERMOVE");
    return 0;
}

// 主角攻击的自动处理
CGameControl::eAAMRet CGameControl::OnMainPlayerAttack()
{	
    //-1,停止自动攻击模式, 0:表示继续追击,1:开始攻击
    eAAMRet ret = AAMR_Other;

    long lAAkMode = GetAutoAttackMode()&(0xFFF);
    if(lAAkMode ==  AAM_FIND_ATTACTER)
    {
        ret = AAMR_Trace;
        //同步消息或动作判断
        if( (pMainPlayer->IsArrive()  && GetSendedMoveMsgCount() < 1 ) || IsMoveWithCurAction() )
        {
            DWORD dwCurTime = GetCurTickCount();
            //char strInfo[512]="";
            //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_2),
            //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
            //PutStringToFile("MoveTest",strInfo);

            // 不能移动
            if(!pMainPlayer->IsInUnMoveState() || (GetAutoAttackMode()&AAM_STOP))
                ret = AAMR_Stop;
            else
            {
                ret = AutoAttackMoveAI();
                if(ret == AAMR_Attack)
                {
                    SetAutoAttackMode(AAM_ATTACT);
                    lAAkMode = AAM_ATTACT;
                    if (pMainPlayer->GetAction() == CShape::ACT_WALK ||
                        pMainPlayer->GetAction() == CShape::ACT_RUN)
                    {
                        pMainPlayer->SetAction( CShape::ACT_STAND );
                        //停止移动
                        StopMove();
                    }
                }
            }
        }
    }	

    //攻击模式
    if(lAAkMode == AAM_ATTACT)
    {
        //动作判断
        if( /*GetSendedUseSkillMsgCount() < 1 && 
            */IsActWithCurAction() )
        {
            DWORD dwCurTime = GetCurTickCount();
            //char strInfo[512]="";
            //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_3),
            //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
            //PutStringToFile("MoveTest",strInfo);
            if(GetAutoAttackMode()&AAM_STOP)
                ret = AAMR_Stop;
            else
            {
                ret = AutoLockAttackAI();
                if(ret == AAMR_Trace)
                {
                    SetAutoAttackMode(AAM_FIND_ATTACTER);
                }
            }
        }
    }

    //停止自动模式
    if(ret == AAMR_Stop)
    {
        SetMPActionMode(MPAM_No);
        SetCurAttractShape(NULL);
        if (pMainPlayer->GetAction() == CShape::ACT_WALK ||
            pMainPlayer->GetAction() == CShape::ACT_RUN)
        {
            pMainPlayer->SetAction( CShape::ACT_STAND );
            //停止移动
            StopMove();
        }
        //如果需要改变移动模式
        if(GetChangingMoveMode())
        {
            SetChangingMoveMode(false);
            ChangeMoveMode();
        }

        DWORD dwCurTime = GetCurTickCount();
        //char strInfo[512]="";
        //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_4),
        //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
        //PutStringToFile("MoveTest",strInfo);
    }
    return ret;
}

// 执行缓存的行为模式
long	CGameControl::OnNextPlayerActionMode()
{
    if(m_NextMainPlayerActionMode != MPAM_No)
    {
        m_MainPlayerActionMode = m_NextMainPlayerActionMode;
        m_NextMainPlayerActionMode = MPAM_No;

        if(m_MainPlayerActionMode == MPAM_AutoMove)
        {
            DWORD dwCurTime = GetCurTickCount();
            Log4c::Trace(ROOT_MODULE,FormatText("GT_25",dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY()));//(Time:%d)玩家进入移动模式x:%d,y:%d.
        }
        else if(m_MainPlayerActionMode == MPAM_AutoAttack)
        {
            DWORD dwCurTime = GetCurTickCount();
            //char strInfo[512]="";
            //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_6),
            //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
            //PutStringToFile("MoveTest",strInfo);
        }
    }
    return 1;
}

//	到一个点的自动移动
long CGameControl::AutoMoveAIEx()
{
    long lX = pMainPlayer->GetTileX();
    long lY = pMainPlayer->GetTileY();
    float fPosX = pMainPlayer->GetPosX();
    float fPosY = pMainPlayer->GetPosY();
    long ret = 1;

    // 到达目标坐标，停止自动移动
    if ( abs(fPosX-m_fDestX) <= m_FloatMove->MINIMUS_OP_DISTANCE && abs(fPosY-m_fDestY) <= m_FloatMove->MINIMUS_OP_DISTANCE )
    {
        ret = 0;
    }
    else
    {
        //检查是否两次走过该格子
        //否就继续行走
        if( AddMovedCell(lX,lY))
        {
            float fDestX = pMainPlayer->GetPosX();
            float fDestY = pMainPlayer->GetPosY();
            if (pMainPlayer->IsArrive() == true)
            {
                //对超长直线路径判断
                if (!m_FloatMove->LineMoveList.empty())
                {
                    float tempX = m_FloatMove->LineMoveList.begin()->x;
                    float tempY = m_FloatMove->LineMoveList.begin()->y;
                    MoveToCell(tempX,tempY);
                    m_FloatMove->LineMoveList.pop_front();
                }
                else
                    MoveToCell(m_fDestX,m_fDestY);
            }
            /*if(GetNextMoveDestCell(m_fDestX,m_fDestY,MAX_MOVE_CELLNUM,fDestX,fDestY))
            {
            MoveToCell(fDestX,fDestY);
            }*/
        }
        //是则停止行走
        else
            ret = 0;
    }
    return ret;
}

//得到某个点方向上下一个移动目标点
long  CGameControl::GetNextMoveDestCell(float fX,float fY,long lMaxDistance,float &fDestX,float &fDestY)
{
    float fSX = pMainPlayer->GetPosX();
    float fSY = pMainPlayer->GetPosY();
    float fDir = CShape::GetLineDirF(fSX,fSY,fX,fY);
    float fDistance = pMainPlayer->RealDistance(fX,fY);
    if(0.0f==fDistance)	
        return 0;
    fDistance = min(((float)lMaxDistance),fDistance);
    return GetNextMoveDestCell(fDir,fDistance,fDestX,fDestY);	
}

//得到一个方向上的下一个移动目标点
long	CGameControl::GetNextMoveDestCell(float fRadianDir,float fMaxDistance,float &fDestX,float &fDestY)
{
    //滑动的8个方向加减值
    static int SlipOrder[8] = {0,1,-1,2,-2,3,-3,4};
    //随机的初始滑动正负方向
    static int SlipRandom[] = {1,-1};

    float fSX = pMainPlayer->GetPosX();
    float fSY = pMainPlayer->GetPosY();

    //在初始方向上移动到阻挡为止
    float fCount = 0.0f;
    float fEndX = fSX,fEndY = fSY;
    int nBlockNum = 0;
    while(fCount <= fMaxDistance)
    {
        float fx = fSX + fCount * cosf(fRadianDir);			
        float fy = fSY + fCount * sinf(fRadianDir);
        if( pRegion->GetBlock((long)fx, (long)fy) != CRegion::BLOCK_NO && 
            pRegion->GetBlock((long)fx, (long)fy) != CRegion::BLOCK_AIM)
            nBlockNum++;
        else
        {
            nBlockNum=0;
            fEndX = fx;fEndY=fy;
        }
        //连续有5个点存在阻挡，则表示不能移动通过
        if(nBlockNum >= 1)
        {
            break;
        }
        fCount += 0.1f;
    }
    if( nBlockNum < 1 && ( (long)fSX != (long)fEndX || (long)fSY != (long)fEndY ))
    {
        fDestX=fEndX;
        fDestY= fEndY;
        if(pRegion->GetBlock((long)fDestX, (long)fDestY) == CRegion::BLOCK_UNFLY)
        {
            int x = 8;
        }
        return 1;
    }
    //没有移动成功，则在产生一格的滑动	
    long nRandom=random(2);
    long lDir = CShape::RadianDirTo8Dir(fRadianDir);
    int i=0;
    long lDestX = (long)fSX;
    long lDextY = (long)fSY;
    for(; i<8; i++)
    {
        long lMoveDir = lDir+SlipOrder[i]*SlipRandom[nRandom];
        if(lMoveDir>7)	lMoveDir = lMoveDir%8;
        else if(lMoveDir<0) lMoveDir = lMoveDir+8;
        lDestX = (long)fSX + _dir[lMoveDir][0];
        lDextY = (long)fSY + _dir[lMoveDir][1];
        if( pRegion->GetBlock(lDestX, lDextY) == CRegion::BLOCK_NO ||
            pRegion->GetBlock(lDestX, lDextY) == CRegion::BLOCK_AIM)
        {
            break;
        }
    }
    if(i < 8)
    {
        fDestX = (float)lDestX+0.5f;
        fDestY = (float)lDextY+0.5f;
        return 1;
    }
    return 0;
}
//	自动跟随鼠标移动
long CGameControl::FollowMouseMove()
{
    //计算方向
    POINT pt;
    GetCursorPos(&pt);
    float fdir = pRegion->CalcDirection(pMainPlayer->GetPosX(), pMainPlayer->GetPosY(), pMainPlayer->GetHeight(), pt.x, pt.y);

    float fDestX = pMainPlayer->GetPosX();
    float fDestY = pMainPlayer->GetPosY();	
    if( GetNextMoveDestCell(fdir,MAX_MOVE_CELLNUM,fDestX,fDestY) )
    {
        MoveToCell(fDestX,fDestY);
        return 1;
    }
    return 0;
}



/*
* 功能: 鼠标左键按下处理
* 摘要: -
* 参数: -
* 返回值: 略
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.11.11 - lpf
*		在“吟唱动作或非施法动作时可移动”增加了仪态动作可以移动的判断条件
*/
long CGameControl::OnLeftButtonDown()
{
    CMainPlayerHand* pMainPlayerHand = 0;


    //不能操作
    if (!IsCanControlCondition())
    {
        pMainPlayerHand->SetMainPlayerHandByEffect(0,0);
        return 0;
    }

    //清空手上的技能图标
    if (pMainPlayerHand)
    {
        if(pMainPlayerHand->GetObjectTypeOfMainPlayerHand()==TYPE_SKILL)
        {
            pMainPlayerHand->ClearMainPlayerHand();
            return 0;
        }
        // 手上有范围技能
        else if(pMainPlayerHand->GetAreaSkillID() > 0)
        {
            DWORD dwID = pMainPlayerHand->GetAreaSkillID();
            CPlayer::tagSkill *pSkill = pMainPlayer->GetSkill(dwID);
            if (pSkill)
            {
                if(GetMPActionMode() == MPAM_AutoMove)
                    AutoMoveStop();
                // 达到释放条件
                long x = 0,y = 0;
				/***********************************************************************/
				/* 赵航 fix */				
				// CSkillXml skill;				
				/***********************************************************************/
				SkillComponent* skillcom = new ConcreteSkillComp();			
				ConcreteDecSkillXml skill(skillcom);                // 释放条件满足
                if(skill.StepSend(dwID,pSkill->wLevel,pMainPlayer,NULL,x,y))
                {
                    //缓冲技能
                    SetBufferSkillID(dwID);
                    // 对目标使用技能
                    StartAttack(m_pCurAttractShape);
                    m_stCurAreaSkill.dwSkillID = dwID;
                    pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), m_stCurAreaSkill.fPosX, m_stCurAreaSkill.fPosY);
                }
                pMainPlayerHand->SetMainPlayerHandByEffect(0,0);
                return 0;
            }
        }
    }

    //如果玩家手上有物品,做物品扔掉处理
    if(pMainPlayerHand!=NULL)
    {
        // 玩家眩晕状态下不能丢东西
        if (pMainPlayer->GetAction()!=CShape::ACT_MEGRIM)
        {
            if (pMainPlayerHand->GetPGoodsOfMainPlayerHand())
            {
                // 王峰：锁定的物品不能丢弃
                if( pMainPlayerHand->GetPGoodsOfMainPlayerHand()->IsLocked() )
                {
                    return 0;
                }

                // 王峰：如果是仓库内的物品则不能丢弃
                if( pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lType > DEPOT_CONTAINER_TYPE_BEGIN_VALUE  &&   pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lType < DEPOT_CONTAINER_TYPE_END_VALUE )
                    return 0;

                if( GetGame()->GetSetup()->bTradeDisable)
                {
                    return 1;
                }
                //判断是否是勋章
                if(pMainPlayerHand->GetPGoodsOfMainPlayerHand()->GetProperty()->dwType==GBT_MEDAL)
                {
                    long lType = pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lType;
                    long lpos = pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lPos;
                    //源是否勋章列表
                    if(lType == PEI_MEDAL)
                    {
                        pMainPlayerHand->ClearMainPlayerHand();
                    }
                    else if(lType == PEI_EQUIPMENT)
                    {
                        //GetGame()->GetCGuiEx()->GetCharacterPageEx()->ChangeEquip(0xffffffff,lpos,PEI_MEDAL,PEI_EQUIPMENT);
                        pMainPlayerHand->ClearMainPlayerHand();
                    }
                    return 1;
                }

                // 判断此物品是否是丢弃就会消失的物品
                if (pMainPlayerHand->GetPGoodsOfMainPlayerHand()->GetEffectVal(GAP_PARTICULAR_ATTRIBUTE,1)&16)
                {
                    //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = 2;
                    //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open(GetGame()->GetCGuiEx()->GetItemPageEx()
                    //	,CStringReading::LoadText(208,214),CMessageBoxPageEx::MSG_OK_NO,true);
                    return 0;
                }

                // 王峰：处理冻结物品
                long dwFreezeVal = pMainPlayerHand->GetPGoodsOfMainPlayerHand()->GetEffectVal(GAP_FREEZE,1);
                if (dwFreezeVal==eFT_Frost||dwFreezeVal==eFT_Thawing||dwFreezeVal==eFT_CantThaw)
                {
                    //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open(NULL,CStringReading::LoadText(208,154),CMessageBoxPageEx::MSG_OK,true);
                    return 0;
                }

                // 王峰：处理绑定物品
                long dwBindVal = pMainPlayerHand->GetPGoodsOfMainPlayerHand()->GetEffectVal(GAP_BIND,1);
                if( dwBindVal==eBT_Bind||dwBindVal==eBT_CantUntie||dwBindVal==eBT_GetAndCantUntie||dwBindVal==eBT_GetAndBind )
                {
                    //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->m_lCallerID = 6;
                    //GetGame()->GetCGuiEx()->GetMessageBoxPageEx()->Open(GetGame()->GetCGuiEx()->GetItemPageEx(), CStringReading::LoadText(208,155),CMessageBoxPageEx::MSG_OK_NO, true);
                    return 0;
                }

                long lPos=pMainPlayer->GetTileY()*pRegion->GetWidth()+pMainPlayer->GetTileX();
                DWORD dwAmount = pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lNum;
                long lType = pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lType;
                long lpos = pMainPlayerHand->GetTGoodsOfMainPlayerHand()->lPos;
                pMainPlayerHand->SendPutDownMes(TYPE_PLAYER,pMainPlayer->GetExID(),lType,lpos,TYPE_REGION,
                    pRegion->GetExID(),0,lPos,TYPE_GOODS,pMainPlayerHand->GetPGoodsOfMainPlayerHand()->GetExID(),dwAmount);
                SetMoveAble(false);

                return 1;
            }
            else if (pMainPlayerHand->GetGoodsIndexOfMainPlayerHand()!=0)
            {
                pMainPlayerHand->ClearMainPlayerHand();
            }
        }
    }

    // 当前鼠标指向的对象为空或者有指向物品(点击地面)
    if(m_pMouseShape == NULL || m_pMouseShape == pMainPlayer)
    {
        // 当鼠标处于修理状态时，取消鼠标状态
        if (GetGame()->GetMouseType()==MC_STATE_TRADE)
        {
            //GetGame()->GetCGuiEx()->GetNPCShopPageEx()->SetRepairState(false);
            GetGame()->SetMouseCursor(MC_NORMAL);
            return 0;
        }
        // 取消鼠标状态
        if (GetGame()->GetMouseType()>=MC_TRYON_GOODS)
        {
            GetGame()->SetMouseCursor(MC_NORMAL);
            return 0;
        }
        // 无附加键盘操作
        if((GetKeyboard()->IsKeyDown(DIK_LSHIFT) ==false && GetKeyboard()->IsKeyDown(DIK_RSHIFT) == false) &&
            (GetKeyboard()->IsKeyDown(DIK_SPACE) ==false))
        {
            // 当前鼠标指向的是物品，不处于吟唱状态，拾取物品
            if(m_pMouseShapeGoods)
            {
                CGoods *pGoods = dynamic_cast<CGoods*>(m_pMouseShapeGoods);
                if (pGoods)
                {
                    // 可以拾取
                    if (IsCanPickupGoodsCondition())
                    {
                        // 物品不在玩家周围，跑过去,拾取在跑到物品处后触发
                        if(IsMoveAble() && pMainPlayer->Distance(pGoods) > 1)
                        {
                            FollowObject(pGoods->GetType(),pGoods->GetExID(),1);
                            return 1;
                        }
                        if (PickupGoods(pGoods) == 1)
                        {
                            return 1;
                        }
                    }
                }
            }
            //如果当前行为模式为自动攻击,并且有自动攻击模式，就停止自动攻击
            if(GetMPActionMode() == MPAM_AutoAttack && !( GetAutoAttackMode()&AAM_STOP) )
            {
                //停止自动攻击
                AutoAttactStop();
            }
            //移动
            else
            {
                CPlayer *pPlayer = GetGame()->GetMainPlayer();
                if(pPlayer)
                {
                    // 吟唱动作或非施法动作时可移动
                    if (pPlayer->GetIntonateKey() ||
                        pPlayer->GetAction() < CShape::ACT_PERFORM ||
                        (pPlayer->GetAction() >= CShape::ACT_FACEACTION && pPlayer->GetAction() <= CShape::ACT_FACEACTIONMAX))
                    {
                        // 获得鼠标点击的坐标
                        float fDestX=0.0f, fDestY=0.0f;
                        pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), fDestX, fDestY);
                        if (GetGame()->GetSetup()->bfreecamera)
                        {
                            GetGame()->SetScrollCenterX(fDestX);
                            GetGame()->SetScrollCenterY(fDestY);
                        }
                        else
                        {
                            // 不是点击当前所站的位置
                            if(abs(pPlayer->GetPosX() - fDestX) >= m_FloatMove->MINIMUS_OP_DISTANCE || 
                                abs(pPlayer->GetPosY() - fDestY) >= m_FloatMove->MINIMUS_OP_DISTANCE)
                            {
                                pPlayer->SetCurIntonateTime(0,0);
                                pPlayer->SetIntonateSkillKey(0);
                                AutoMoveToEx(fDestX, fDestY);
                            }
                        }
                        GetGame()->GetRegion()->PlayEffect(1,fDestX,fDestY);
                    }
                }
            }
            GetMouse()->SetCurrentButton(NONE);
        }
    }
    // 鼠标指向对象不为空，且不是点击自己或自己的宠物
    else if(m_pMouseShape != pMainPlayer)
    {
        if (m_pMouseShape->GetType()==TYPE_PET)
        {
            CPet* pet = dynamic_cast<CPet*>(m_pMouseShape);
            if (pet->IsMainPlayerPet())
            {
                return 0;
            }
        }
        // 鼠标点击其他物体身上处理
        if( OnLeftClickShape() )
        {
            SetMoveAble(false);
            return 1;
        }
    }
    // Debug下点击地面  瞬间移动
    if( g_bDebug && 
        GetKeyboard()->IsKeyDown(DIK_SPACE) &&
        (pMainPlayer->GetAction() == CShape::ACT_STAND || pMainPlayer->GetAction() == CShape::ACT_IDLE ||
        pMainPlayer->GetAction() == CShape::ACT_BEATTACK))
    {
        float fMouseX, fMouseY;
        pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), fMouseX, fMouseY);

        CMessage msg(MSG_C2S_SHAPE_SETPOS);
        msg.Add(pMainPlayer->GetType());
        msg.Add(pMainPlayer->GetExID());
        msg.Add(fMouseX);
        msg.Add(fMouseY);
        msg.Send();
        SetMoveAble(false);
        AutoAttactStop();
        AutoMoveStop();
        return 1;
    }

    return 0;
}

// 鼠标左键松开处理
long CGameControl::OnLeftButtonUp()
{
    return 0;
}

// 鼠标左键按住处理
long CGameControl::OnLeftButtonHolding()
{	
    // 如果持续按住鼠标左键，自动跟随鼠标方向移动
    if ( GetMPActionMode() == MPAM_AutoMove &&
        m_dwLeftHoldingTime &&
        GetCurTickCount() - m_dwLeftHoldingTime > 800 )
    {
        m_dwLeftHoldingTime = 0;
        if(m_AutoMoveMode != AMM_FOLLOW_MOUSE)
        {
            m_AutoMoveMode = AMM_FOLLOW_MOUSE;
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
            //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_AUTOMOVE),
            //	D3DCOLOR_ARGB(255,255,255,0));
        }
    }
    return 0;
}

// 鼠标左键双击
long CGameControl::OnLeftDoubleButtonDown()
{
    // 不是玩家
    if (m_pMouseShape && m_pMouseShape != pMainPlayer) 
    {
        // 不是玩家宠物
        if (m_pMouseShape->GetType()==TYPE_PET)
        {
            CPet* pet = dynamic_cast<CPet*>(m_pMouseShape);
            if (pet->IsMainPlayerPet())
            {
                return 0;
            }
        }
        // 玩家处于强制攻击状态，且当前鼠标指向的对象不是目前被选择的对象，返回
        if(!pMainPlayer->IsInDefianceState() && m_pSelectedShape != m_pMouseShape||m_pMouseShape->GetType()==TYPE_COLLECTION)
        {
            return 0;
        }
        //不能操作
        if (!IsCanControlCondition())
            return 0;
        // 转化为攻击模式
        if(GetMPActionMode() == MPAM_AutoMove)
        {
            //停止自动行走
            AutoMoveStop();
        }

        CShape* pAttackShape = NULL;
        SetSelectedShape(m_pMouseShape);
        pAttackShape = m_pMouseShape;
        SetAutoSkillID(pMainPlayer->GetDefaultAttackSkillID());

        // 双击玩家
        if(pAttackShape->GetType() == TYPE_PLAYER)
        {
            CPlayer *pPlayer = dynamic_cast<CPlayer*>(pAttackShape);
            if (pPlayer->IsDied())
            {
                SetAutoSkillID(0);
            }
            // 不是敌对玩家，不是强行攻击
            if( GetGame()->GetMainPlayer()->IsCityWarEnemyFaction( pPlayer->GetFactionID() ) == false &&
                GetGame()->GetMainPlayer()->IsEnemyFaction(pPlayer->GetFactionID()) == false && 
                (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL)) )
            {
                pAttackShape = NULL;
                StartAttack(pAttackShape);
                return 0;
            }
        }
        // 双击monster
        else if(pAttackShape->GetType() == TYPE_MONSTER)
        {
            CMonster *pMonster = dynamic_cast<CMonster*>(pAttackShape);			
            // 死亡
            if(pMonster->IsDied())
            {
                SetAutoSkillID(0);
            }
            // 双击NPC,且不是强行攻击
            if(pMonster->GeteNpcType() == NT_Normal && (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL)))
            {
                // 取消攻击对象和攻击技能
                m_pCurAttractShape = NULL;
                SetAutoSkillID(0);
                if (pMainPlayer->Distance(pMonster) > 5)
                {
                    // 跑过去对话
                    FollowObject(pMonster->GetType(),pMonster->GetExID(),3);
                    return 0;
                }
                else
                {
                    SendTouchNpcMes(pMonster->GetExID());
                    return 0;
                }
            }
        }
        else if (pAttackShape->GetType() == TYPE_PET)
        {
            // 不是玩家自己的宠物
            CPet* pet = dynamic_cast<CPet*>(pAttackShape);
            if (pet->IsDied())
            {
                SetAutoSkillID(0);
            }

            // 重复点击玩家宠物，不是强行攻击，宠物主人不是敌对行会成员，也不是敌国成员，取消攻击状态
            if( pet->GetMaster()->IsCityWarEnemyFaction( pet->GetMaster()->GetFactionID() ) == false 
                && pet->GetMaster()->IsEnemyFaction(pet->GetMaster()->GetFactionID()) == false  
                && (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL)))
            {
                pAttackShape = NULL;
                StartAttack(pAttackShape);
                return 0;
            }
        }

        //开始攻击
        StartAttack(pAttackShape);
    }
    return 0;
}

// 鼠标左健在SHAPE上点下处理
long CGameControl::OnLeftClickShape()
{
    if (m_pMouseShape) 
    {
        // 玩家处于强制攻击状态，且当前鼠标指向的对象不是目前被选择的对象，返回
        if(!pMainPlayer->IsInDefianceState() && m_pSelectedShape != m_pMouseShape||m_pMouseShape->GetType()==TYPE_COLLECTION)
        {
            return 0;
        }

        CShape * pOldSelected = m_pSelectedShape;
        SetSelectedShape(m_pMouseShape);
        //m_pSelectedShape = m_pMouseShape;

        // 如果是重复点击，进行攻击
        if(pOldSelected == m_pSelectedShape)
        {
            if(GetMPActionMode() == MPAM_AutoMove)
            {
                //停止自动行走
                AutoMoveStop();
            }
            StartAttack(m_pSelectedShape);
            m_pCurAttractShape = m_pSelectedShape;
            SetAutoSkillID(pMainPlayer->GetDefaultAttackSkillID());
        }
        // 否则，如果玩家处于静止状态改变玩家方向
        else if(pMainPlayer->GetAction() == CShape::ACT_STAND || pMainPlayer->GetAction() == CShape::ACT_IDLE)
        {
            //long dir = GetLineDir(pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), m_pSelectedShape->GetTileX(), m_pSelectedShape->GetTileY());
            float fRadianDir = pMainPlayer->GetLineDirF(pMainPlayer->GetPosX(),pMainPlayer->GetPosY(),
                m_pSelectedShape->GetPosX(),m_pSelectedShape->GetPosY(),pMainPlayer->GetDirEx());
            if (fRadianDir != pMainPlayer->GetDirEx())
            {
                pMainPlayer->SetDirEx(fRadianDir);
                return 1;
            }
        }
        switch( m_pMouseShape->GetType() )
        {
        case TYPE_PLAYER:
            {
                CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pSelectedShape);
                if (pPlayer==NULL)
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetSelectedShape(NULL);
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                    }
                    return 0;
                }
                if (pPlayer->IsDied())
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetAutoSkillID(0);
                    }
                    return 0;
                }

                // 设置跟随对象，设置攻击对象为空
                if ( GetKeyboard()->IsKeyDown( DIK_LALT ) || GetKeyboard()->IsKeyDown( DIK_RALT ) )
                {
                    m_pCurAttractShape = NULL;
                    SetAutoSkillID(0);

                    string strName = pPlayer->GetName();
                    // 隐藏信息的队友，读取队伍中的信息
                    if(pPlayer->GetIsHideInfo() && pMainPlayer->IsTeammate(pPlayer->GetExID()))
                    {
                        tagWSMemberInfo * stInfo = pMainPlayer->GetWsMemberInfo(pPlayer->GetExID());
                        if(stInfo)
                            strName = stInfo->szName;
                    }
                    //char str[256];
                    //sprintf(str, CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_FOLLOWPLAYER), strName.c_str());
                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,str,D3DCOLOR_ARGB(255,255,255,0));
                    //跟随对象
                    FollowObject(TYPE_PLAYER,pPlayer->GetExID(),1);
                    return 0;
                }

                // 重复点击玩家，不是强行攻击，不是敌对行会成员，也不是敌国成员，取消攻击状态
                if( pOldSelected == m_pSelectedShape && GetGame()->GetMainPlayer()->IsCityWarEnemyFaction( pPlayer->GetFactionID() ) == false &&
                    GetGame()->GetMainPlayer()->IsEnemyFaction(pPlayer->GetFactionID()) == false  && 
                    (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL)))
                {
                    //m_pCurAttractShape = NULL;
                    //SetAutoSkillID(0);
                    //// 移动到目标附近
                    //long x = 0,y = 0;
                    //GetLineFront(x,y,pMainPlayer->GetPosX(),pMainPlayer->GetPosY(),
                    //	m_pSelectedShape->GetPosX(),m_pSelectedShape->GetPosY());
                    //if(x > 0 && y > 0)
                    //	AutoMoveTo(x,y);
                    return 0;
                }
            }
            break;

        case TYPE_MONSTER:
            {
                /*if(GetGame()->GetMouseType()>=MC_TRYON_GOODS)
                {
                GetGame()->SetMouseCursor(MC_NORMAL);
                return 0;
                }*/

                // 处于骑乘时不自动攻击怪物
                if (pMainPlayer->GetHorse()->IsVisible())
                {
                    return 0;
                }

                CMonster *pMonster = dynamic_cast<CMonster*>(m_pMouseShape);
                if (pMonster==NULL)
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetSelectedShape(NULL);
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                    }
                    return 0;
                }
                // 死亡
                if (pMonster->IsDied())
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetSelectedShape(NULL);
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                    }
                    return 0;
                }
                // 重复点NPC,且不是强行攻击
                if( pMonster->GeteNpcType() == NT_Normal && pOldSelected == m_pSelectedShape 
                    && (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL))
                    )
                {
                    // 取消攻击对象和攻击技能
                    m_pCurAttractShape = NULL;
                    SetAutoSkillID(0);
                    if (pMainPlayer->Distance(pMonster) > 5)
                    {
                        // 跑过去对话
                        FollowObject(pMonster->GetType(),pMonster->GetExID(),3);
                        return 0;
                    }
                    else
                    {
                        SendTouchNpcMes(pMonster->GetExID());
                        return 0;
                    }
                }
                return 0;
            }
            break;

        case TYPE_BUILD:
        case TYPE_CITYGATE:
            {
                if(GetGame()->GetMouseType()>=MC_TRYON_GOODS)
                {
                    GetGame()->SetMouseCursor(MC_NORMAL);
                    return 0;
                }
                // 建筑物被消灭
                if ( ((CMoveShape*)m_pMouseShape)->IsDied())
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetSelectedShape(NULL);
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                    }				
                    return 1;
                }
                return 0;
            }
            break;

        case TYPE_PET:
            {
                // 不是玩家自己的宠物
                CPet* pet = dynamic_cast<CPet*>(m_pMouseShape);
                if (pet == NULL|| pet->IsMainPlayerPet())
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetSelectedShape(NULL);
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                    }
                    return 0;
                }

                if (pet->IsDied())
                {
                    if(pOldSelected == m_pSelectedShape)
                    {
                        SetAutoSkillID(0);
                    }
                    return 0;
                }

                // 重复点击玩家宠物，不是强行攻击，宠物主人不是敌对行会成员，也不是敌国成员，取消攻击状态
                if( pOldSelected == m_pSelectedShape 
                    && pet->GetMaster()->IsCityWarEnemyFaction( pet->GetMaster()->GetFactionID() ) == false 
                    && pet->GetMaster()->IsEnemyFaction(pet->GetMaster()->GetFactionID()) == false  
                    && (!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL)))
                {
                    return 0;
                }
            }
            break;

            // 不可攻击的对象
        default:
            {
                m_pCurAttractShape = NULL;
                SetAutoSkillID(0);
            }
            break;
        }
    }
    return 1;
}

// 鼠标右键按下处理
long CGameControl::OnRightButtonDown()
{
    // 手上有状态,清除
    CMainPlayerHand *pHand = 0;
    if(pHand)
    {
        if(pHand->GetAreaSkillID() > 0 || pHand->GetSkillOfMainPlayerHand() > 0)
            pHand->ClearMainPlayerHand();
    }
    m_pPressRBtnShape = NULL;	
    // 点自己或不能操作
    if( m_pMouseShape == pMainPlayer || IsCanControlCondition()==false)
        return 0;
    // 鼠标点击其他物体身上处理
    if (m_pMouseShape)
    {
        // 玩家处于强制攻击状态，且当前鼠标指向的对象不是目前被选择的对象，返回
        if(!pMainPlayer->IsInDefianceState() && m_pSelectedShape != m_pMouseShape)
        {
            return 0;
        }
        m_pPressRBtnShape = m_pMouseShape;
        //OnRightClickShape();
    }
    return 0;
}

// 鼠标右键松开处理
long CGameControl::OnRightButtonUp()
{
    // 如果按下和放开都在同一个对象上，执行点击操作
    if(m_pMouseShape && m_pMouseShape == m_pPressRBtnShape)
        OnRightClickShape();
    return 0;
}

// 鼠标右键按住处理
long CGameControl::OnRightButtonHolding()
{
    return 0;
}

// 鼠标右键双击
long CGameControl::OnRightDoubleButtonDown()
{
    return 0;
}

// 鼠标右健在SHAPE上点下处理
long CGameControl::OnRightClickShape()
{
    if (m_pMouseShape) 
    {
        //m_pSelectedShape = m_pMouseShape;
        SetSelectedShape(m_pMouseShape);
        if(m_pSelectedShape && pMainPlayer->GetAction() == CShape::ACT_STAND || pMainPlayer->GetAction() == CShape::ACT_IDLE)
        {
            float fRadianDir = pMainPlayer->GetLineDirF(pMainPlayer->GetPosX(),pMainPlayer->GetPosY(),
                m_pSelectedShape->GetPosX(),m_pSelectedShape->GetPosY(),pMainPlayer->GetDirEx());
            if (fRadianDir != pMainPlayer->GetDirEx())
            {
                // 转方向
                pMainPlayer->SetDirEx(fRadianDir);
            }
        }
        // 右键点击别的玩家
        if( m_pMouseShape->GetType() == TYPE_PLAYER )
        {
            // 查看状态，取消攻击对象和自动攻击技能
            if( GetKeyboard()->IsKeyDown( DIK_LCONTROL ) || GetKeyboard()->IsKeyDown( DIK_RCONTROL ) )
            {
                SetMoveAble(false);
                //GetGame()->GetInterface()->GetOtherPlayerPopMenuPage()->SwitchVisibility();
                long mx = GetMouse()->GetMouseX();
                long my = GetMouse()->GetMouseY();
                //CPlayer* pMoveShape = dynamic_cast<CPlayer*>(m_pMouseShape);
                //GetGame()->GetCGuiEx()->GetLookPageEx()->SetSelectPlayer(pMoveShape);
                //GetGame()->GetCGuiEx()->GetLookPageEx()->Open();
                return 1;
            }
            // 设置私聊对象
            if ( GetKeyboard()->IsKeyDown( DIK_LALT ) || GetKeyboard()->IsKeyDown( DIK_RALT ) )
            {
                string strName = m_pMouseShape->GetName();
                SetMoveAble(false);
                return 1;
            }
            // 开启对方的个人商店
            if (((CPlayer*)m_pMouseShape)->IsPlayerShopOpened())
            {
                float fDistance = (float)pMainPlayer->Distance(m_pMouseShape);
                if (fDistance < 6)
                {
                    CMessage msg(MSG_C2S_PLAYERSHOP_LOOKGOODS);
                    msg.Add(((CPlayer*)m_pMouseShape)->GetPlayerShopSessionID());
                    msg.Add(((CPlayer*)m_pMouseShape)->GetExID());
                    msg.Send();
                    AutoMoveStop();
					StopMove();
                    return 1;
                }
            }
            //------------------------07.8.5(liuke添加)----------------------------
            if(((CPlayer*)m_pMouseShape)->IsRecruitingTeam())
            {
                // 玩家有队伍，不能点其它玩家的招募信息
                if(!GetGame()->GetMainPlayer()->GetTeamMateInfo().empty())
                {
                    //char str[256]="";
                    //// 本队队长招募
                    //if(m_pMouseShape->GetExID() == GetGame()->GetMainPlayer()->GetCaptainID())
                    //	sprintf_s(str,"%s",CStringReading::LoadText(eIDS_TEAM,eIDS_TEAM_INTHISTEAM));
                    //else
                    //	sprintf_s(str,"%s",CStringReading::LoadText(eIDS_TEAM,eIDS_TEAM_HASTEAM));
                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,str,D3DCOLOR_ARGB(255,255,0,0));
                    return 0;
                }
                /*CRecruitBoxPageEx *pRecruitBox = GetGame()->GetCGuiEx()->GetRecruitBoxPageEx();
                CMessageBoxPageEx *pMessageBox = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
                if(pRecruitBox && pMessageBox)
                {
                pRecruitBox->SetPlayerGUID(m_pMouseShape->GetExID());
                pMessageBox->Open(pRecruitBox,CStringReading::LoadText(eIDS_TEAM,eIDS_TEAM_ISJOINTEAM),CMessageBoxPageEx::MSG_OK_NO,true);
                }*/
                SetMoveAble(false);
                return 1;
            }
            // 有攻击目标直接切换
            if(m_pCurAttractShape)
                StartAttack(m_pSelectedShape);
            //-----------------------------------------------------------------
        }
        // 右键点击怪物（或NPC）
        else if(m_pMouseShape->GetType() == TYPE_MONSTER)
        {
            CMonster *pMonster = dynamic_cast<CMonster*>(m_pMouseShape);
            if (pMonster!=NULL)
            {
                // 死亡
                if(pMonster->IsDied())
                {
                    if (((CMoveShape*)m_pMouseShape)->GetCollectGoodsID()!=0)
                    {
                        SetFollowingObj(0,CGUID::GUID_INVALID);
                        SetBufferSkillID(0);		// 取消缓冲技能
                        //m_pSelectedShape = m_pMouseShape;
                        // 取消攻击对象和攻击技能
                        m_pCurAttractShape = NULL;
                        SetAutoSkillID(0);
                        CCollectionGoods* pCollectGoods = (CCollectionGoods*)m_pMouseShape;
                        if (pMainPlayer->Distance(pCollectGoods) > 1)
                        {
                            // 跑过去采集
                            FollowObject(m_pMouseShape->GetType(),m_pMouseShape->GetExID(),1);
                            return 1;
                        }
                        else
                        {
                            DoCollect(pCollectGoods);
                            return 1;
                        }
                        //AutoCollect(pCollectGoods->GetExID());
                    }else 
                        return 1;
                }
                // 右键点击NPC,且不是强行攻击，过去对话
                //if(pMonster->GeteNpcType() == NT_Normal)
                if(pMonster->IsCanTalk())
                {
                    SetFollowingObj(0,CGUID::GUID_INVALID);
                    SetBufferSkillID(0);		// 取消缓冲技能
                    //m_pSelectedShape = m_pMouseShape;
                    // 取消攻击对象和攻击技能
                    m_pCurAttractShape = NULL;
                    SetAutoSkillID(0);
                    if (pMainPlayer->Distance(pMonster) > 5)
                    {
                        // 跑过去对话
                        FollowObject(pMonster->GetType(),pMonster->GetExID(),3);
                        return 1;
                    }
                    else
                    {
                        SendTouchNpcMes(pMonster->GetExID());
                        return 1;
                    }
                }
                // 有攻击目标直接切换
                if(m_pCurAttractShape)
                    StartAttack(m_pSelectedShape);
            }
        }
        else if (m_pMouseShape->GetType() == TYPE_COLLECTION)
        {
            CCollectionGoods* pCollectGoods = (CCollectionGoods*)m_pMouseShape;
            if (pMainPlayer->Distance(m_pMouseShape) > 1)
            {
                // 跑过去对话
                FollowObject(m_pMouseShape->GetType(),m_pMouseShape->GetExID(),1);
                return 1;
            }
            else
            {
                DoCollect(pCollectGoods);
                return 1;
            }
            //AutoCollect(pCollectGoods->GetExID());
            //DoCollect(pCollectGoods);
        }
        else if (m_pMouseShape->GetType() == TYPE_PET)
        {
            //CPet *pPet = dynamic_cast<CPet*>(m_pMouseShape);
            //if (pPet && pPet->IsMainPlayerPet())
            //{
            //	CPopMenuPageEx *pMenu = GetGame()->GetCGuiEx()->GetCPopMenuPageEx();
            //	if (pMenu)
            //	{
            //		pMenu->SetMenuType(CPopMenuPageEx::PET_RIGHTMOUSE_POPMENU);
            //		pMenu->SetObjType( CPopMenuPageEx::PET_MENU );
            //		pMenu->SetObject( (void*)pPet );
            //		long x = GetMouse()->GetMouseX();
            //		long y = GetMouse()->GetMouseY();
            //		pMenu->SetMenuPos(x+5,y);
            //		pMenu->OpenMenu(CPopMenuPageEx::PET_RIGHTMOUSE_POPMENU);
            //	}
            //}
        }
    }
    return 0;
}

// 发送点击NPC的消息
void CGameControl::SendTouchNpcMes(CGUID NpcGUID)
{
    CMonster *pMonster = dynamic_cast<CMonster *>(pRegion->FindChildObject(TYPE_MONSTER,NpcGUID));
    if(pMonster)
    {
        // 死亡，可剥皮
        if(pMonster->IsDied() && pMonster->GetCollectGoodsID()!=0)
        {
            CCollectionGoods* pCollectGoods = (CCollectionGoods*)pMonster;
            if(pCollectGoods)
            {
                DoCollect(pCollectGoods);
                return;
            }
        }
        // 将点击的NPC设置新的方向，为面向玩家
        if(pMainPlayer && pRegion)
        {
            float fRadianDir = pMonster->GetLineDirF(pMonster->GetPosX(),pMonster->GetPosY(),pMainPlayer->GetPosX(),pMainPlayer->GetPosY());
            pMonster->SetDirEx(fRadianDir);
        }
    }
    //CNPCShopPageEx* pNPCShop = GetGame()->GetCGuiEx()->GetNPCShopPageEx();
    //CPlayerShopPageEx* pPlayerShop = GetGame()->GetCGuiEx()->GetPlayerShopPageEx();
    //CPlayerTradePageEx* pPlayerTrade = GetGame()->GetCGuiEx()->GetPlayerTradePageEx();
    //if (pNPCShop!=NULL&&pPlayerShop!=NULL&&pPlayerTrade!=NULL&&
    //	(pNPCShop->IsOpen()||pPlayerShop->IsOpen()||pPlayerTrade->IsOpen()))
    //{
    //	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
    //		CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_CANTTOUCHNPC),
    //		D3DCOLOR_ARGB(255,255,0,0));
    //	return;
    //}
    CMessage msg(MSG_C2S_PLAYER_TOUCHNPC);
    msg.Add(NpcGUID);
    msg.Send();
    GetGame()->SetLastTouchNpcID(NpcGUID);
    // 如果是邮箱，保存guid
    //if(pMonster && pMonster->GetOrigName() && strcmp(pMonster->GetOrigName(),"10000Email")==0)
    //	GetGame()->GetCGuiEx()->GetMailPageEx()->SetMailBoxGUID(NpcGUID);
}

void CGameControl::DoCollect(CMoveShape* pMoveShape)
{
    if (pMoveShape->GetType()==TYPE_COLLECTION&&pMoveShape->IsDied()) return;
    //CCollectionGoods* pCollectGoods = dynamic_cast<CCollectionGoods*>(pMoveShape);
    const CCollectionLayout* pCollectLayout = CDeputyOccuSystemClient::GetCollectionLayout(pMoveShape->GetCollectGoodsID());
    if (pMoveShape!=NULL&&pCollectLayout!=NULL&&pCollectLayout->GetOccuID()!=0)
    {
        if (!pMoveShape->IsBeingCollected()&&pMoveShape->IsCanBeCollected())
        {
            bool bShowResults = true;
            DWORD dwCollectType = CDeputyOccuSystemClient::OB_Collection;
            const list<tgConditionGoods> pConditionList = pCollectLayout->GetConditionList();
            if (pCollectLayout->GetOccuID()!=DOccu_Other&&pCollectLayout->GetOccuID()!=pMainPlayer->GetAssOccu())
            {
                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOOCCU),
                //	D3DCOLOR_ARGB(255,255,0,0));
                //PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOOCCU));
                bShowResults = false;
            }
            // 任务条件检测
            else if (pCollectLayout->GetOccuID()==DOccu_Other)
            {
                dwCollectType = CDeputyOccuSystemClient::OB_TaskCollection;
                const vector<tagQuestGoods>& QustList = pCollectLayout->GetRoleInfo();
                if (!QustList.empty())
                {
                    vector<tagQuestGoods>::const_iterator QustIter = QustList.begin();
                    for (;QustIter!=QustList.end();QustIter++)
                    {
                        if (QustIter->nQuestId!=-1)
                        {
                            CQuestQueue* pQueueList = pMainPlayer->GetRoleQueue();
                            if (pQueueList!=NULL)
                            {
                                if (pQueueList->GetRecordByID(QustIter->nQuestId)==NULL||
                                    pQueueList->GetQuestStepNum(QustIter->nQuestId)!=QustIter->dwRoleStep)
                                {
                                    continue;

                                    //bShowResults = false;
                                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                                    //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUEST),
                                    //	D3DCOLOR_ARGB(255,255,0,0));
                                }
                                else if (pQueueList->GetRecordByID(QustIter->nQuestId)!=NULL&&
                                    pQueueList->GetQuestStepNum(QustIter->nQuestId)==QustIter->dwRoleStep)
                                {
                                    break;

                                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                                    //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUESTSTEP),
                                    //	D3DCOLOR_ARGB(255,255,0,0));
                                    //bShowResults = false;
                                }
                            }
                            else
                            {
                                continue;

                                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                                //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUEST),
                                //	D3DCOLOR_ARGB(255,255,0,0));
                                //bShowResults = false;
                            }
                        }
                        else
                            break;
                    }
                    if (QustIter==QustList.end())
                    {
                        bShowResults = false;
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                        //	CStringReading::LoadText(eIDS_CONTROL,45),
                        //	D3DCOLOR_ARGB(255,255,0,0));
                    }
                    else if (QustIter!=QustList.end())
                    {
                        bShowResults = true;
                    }
                }
            }
            // 物品条件检测
            if (bShowResults&&pConditionList.size()!=0)
            {
                list<tgConditionGoods>::const_iterator conIter = pConditionList.begin();
                for (;conIter!=pConditionList.end();conIter++)
                {
                    long lGoodNum = pMainPlayer->GetGoodsAmount(conIter->cGoodsName);
                    if (conIter->iNumType==0&&lGoodNum<conIter->iNum||
                        conIter->iNumType==1&&lGoodNum!=conIter->iNum||
                        conIter->iNumType==2&&lGoodNum>=conIter->iNum)
                    {
                        bShowResults = false;
                        WORD wChoseIndex = 0;
                        switch(conIter->iNumType)
                        {
                        case 0:
                            wChoseIndex = 6;
                            break;
                        case 1:
                            wChoseIndex = 7;
                            break;
                        case 2:
                            wChoseIndex = 8;
                            break;
                        }
                        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                        //	CStringReading::LoadText(205,wChoseIndex),
                        //	D3DCOLOR_ARGB(255,255,0,0));
                        break;
                    }
                }
            }
            if (bShowResults)
            {
                UseOccuSkill(dwCollectType,(CShape*)pMoveShape);
            }
        }
        else if (pMoveShape->IsBeingCollected())
        {
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
            //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_BEINGCOLLECTED),
            //	D3DCOLOR_ARGB(255,255,0,0));
        }
        else if (!pMoveShape->IsCanBeCollected())
        {
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
            //	CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_CANTBECOLLECTED),
            //	D3DCOLOR_ARGB(255,255,0,0));
        }
    }
}

// 向指定方向移动一步,可以跑一格
//dir:方向
//action:动作(run or walk)
//bDis:移动距离,跑的时候最大是2,行走是1
long CGameControl::MoveDir(CMoveShape* pShape, long dir,short nDis)
{
    WORD action = CShape::ACT_WALK;
    if(pShape->GetIsRun())
        action = CShape::ACT_RUN;

    //行走只能一步
    if(action == CShape::ACT_WALK)	nDis = 1;
    // 不能移动状态
    if (IsCanMoveCondition()==false)
        return 0;

    long lOrDir		= dir;
    long x			= pShape->GetTileX();
    long y			= pShape->GetTileY();
    long newx1		= x;
    long newy1		= y;
    long newx2		= x;
    long newy2		= y;

    int i=0;
    for(; i<8; i++)
    {
        newx1 = x + _dir[_slip_order[dir][i]][0];
        newy1 = y + _dir[_slip_order[dir][i]][1];
        if( pRegion->GetBlock(newx1, newy1) == CRegion::BLOCK_NO ||
            pRegion->GetBlock(newx1, newy1) == CRegion::BLOCK_AIM)
        {
            dir = pShape->GetDestDir(x,y, newx1, newy1);
            if( dir != lOrDir && nDis == 2 )
            {
                nDis = 1;
            }
            break;
        }
    }
    //没有找到，返回
    if(i == 9)	return 0;

    if( action == CShape::ACT_RUN && nDis == 2)
    {
        newx2 = newx1 + _dir[dir][0];
        newy2 = newy1 + _dir[dir][1];
        if( pRegion->GetBlock(newx2, newy2) != CRegion::BLOCK_NO &&
            pRegion->GetBlock(newx2, newy2) != CRegion::BLOCK_AIM )
        {
            nDis = 1;
        }
    }

    //移动测试代码,wq,2008-2-20
    DWORD dwCurTime = GetCurTickCount();
    //char strInfo[512]="";
    //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_8),
    //								dwCurTime,	x,		y,	action,		dir,	nDis,newx1,		newy1,	newx2,		newy2);
    //PutStringToFile("MoveTest",strInfo);
    ////////////////////////////////////////
    ////////////////////////////////////////wqend

    pShape->SetAction(action);
    pShape->SetDir((CShape::eDIR)dir);

    if( nDis == 1 )
    {
        pShape->SetDestXY(newx1+0.5f, newy1+0.5f);
    }
    else// nDis == 2 
    {
        pShape->SetDestXY(newx2+0.5f, newy2+0.5f);
    }

    SetControlAble(false);
    // 设置开始移动的时间
    pShape->SetIsArrive(false);
    SetSendedMoveMsgCount(GetSendedMoveMsgCount()+1);
    return 1;

}

//向某方向移动函数
//fdir:角度
long CGameControl::MoveToCell(float fDestX,float fDestY)
{
    //移动次数
    static long lMoveCellCount = 0;
    //上次统计的移动验证时间
    static DWORD dwLastMoveValiTime = 0;
    static const DWORD dwMaxValiNum = 40;
    // 不能移动状态
    if (IsCanMoveCondition()==false)
        return 0;
    float fx = pMainPlayer->GetPosX();
    float fy = pMainPlayer->GetPosY();

    float fRadianDir = GetRadianDir(fx,fy,fDestX,fDestY);

    pMainPlayer->SetDirEx(fRadianDir);
    pMainPlayer->SetDestXY(fDestX,fDestY);
    pMainPlayer->SetIsArrive(false);
    //得出移动时间
    float fTotalDistance = pMainPlayer->RealDistance(fDestX,fDestY);
    float fMoveTime = fTotalDistance/pMainPlayer->GetSpeed();
    pMainPlayer->SetMoveOffset((fDestX-fx)/fMoveTime, (fDestY-fy)/fMoveTime);
    DWORD dwCurTime = GetCurTickCount();
    pMainPlayer->SetEndMoveTime(dwCurTime+(DWORD)fMoveTime);	
    if(pMainPlayer->GetAction() != CShape::ACT_WALK &&
        pMainPlayer->GetAction() != CShape::ACT_RUN)
    {
        //如果现在的动作不是行走动作,重置上次的更新时间
        pMainPlayer->SetLastUpdatePosTime(dwCurTime);
    }

    //设置更新位置
    SetUpdateXY(fx,fy);
    WORD action = CShape::ACT_WALK;
    if(pMainPlayer->GetIsRun())
        action = CShape::ACT_RUN;
    if(pMainPlayer->GetAction() != action)
    {
        pMainPlayer->SetAction(action);
    }

    //移动频率验证
    lMoveCellCount++;
    //每10次操作统计一次
    if(lMoveCellCount>dwMaxValiNum)
    {
        if( (dwCurTime-dwLastMoveValiTime) < (float)dwMaxValiNum/pMainPlayer->GetSpeed()*0.7f )
        {
            //CMessageBoxPageEx *pMsgPage = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
            //pMsgPage->Open(pMsgPage,CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_MOVEHIGHFREQ),CMessageBoxPageEx::MSG_OK,true);
        }
        lMoveCellCount = 0;
        dwLastMoveValiTime = dwCurTime;
    }

    static long lLastSX = 0,lLastSY = 0;
    static long lLastDX = 0,lLastDY = 0;
    if(lLastSX != (long)fx || lLastSY != (long)fy ||
        lLastDX != (long)fDestX || lLastDY != (long)fDestY)
    {
        lLastSX = (long)fx;lLastSY = (long)fy;
        lLastDX = (long)fDestX; lLastDY = (long)fDestY;
        m_dwMoveMsgCounter++;
        long lCount = 5;
        CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
        if(pSetup)
            lCount = pSetup->lMoveMsgCount;
        if(GetSendedMoveMsgCount() < lCount)
        {
            CMessage msg(MSG_C2S_SHAPE_MOVE);
            msg.Add(fx);
            msg.Add(fy);
            msg.Add((BYTE)0);
            msg.Add(fDestX);
            msg.Add(fDestY);
            msg.Send();
            SetSendedMoveMsgCount(GetSendedMoveMsgCount() + 1);

            //sprintf_s<256>(strMoveInfo,"move msg sended %d times\n", m_dwMoveMsgCounter);
            //OutputDebugString(strMoveInfo);
            //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
        }

        //char strInfo[256]="";
        //_snprintf(strInfo,256,"SendMoveMsg,curTime:%d,sx:%f,sy:%f,dx:%f,dy:%f",timeGetTime(),fx,fy,fDestX,fDestY);
        //PutStringToFile("MoveTest",strInfo);

#ifdef _CLENTTOTEST_
        CMessage msgtot(MSG_C2T_SHAPE_MOVE);
        msgtot.Add((CGUID)CGUID::GUID_INVALID);
        msgtot.Add(fx);
        msgtot.Add(fy);
        msgtot.Add(fDestX);
        msgtot.Add(fDestY);
        msgtot.SendMemToTest();
#endif
    }
    return 1;
}


long	CGameControl::MoveToCellByHotKey(float fDestX,float fDestY)
{
    if (IsCanMoveCondition()==false)
        return 0;
    float fx = pMainPlayer->GetPosX();
    float fy = pMainPlayer->GetPosY();

    float fRadianDir = GetRadianDir(fx,fy,fDestX,fDestY);
    ////设置新的方向
    pMainPlayer->SetDirEx(fRadianDir);
    pMainPlayer->SetDestXY(fDestX,fDestY);

    pMainPlayer->SetIsArrive(false);
    //得出移动时间
    float fTotalDistance = pMainPlayer->RealDistance(fDestX,fDestY);
    DWORD dwMoveTime = (DWORD)(fTotalDistance/pMainPlayer->GetSpeed());
    DWORD dwCurTime = GetCurTickCount();
    pMainPlayer->SetEndMoveTime(dwCurTime+dwMoveTime);	
    //设置更新位置
    SetUpdateXY(fx,fy);
    WORD action = CShape::ACT_WALK;
    if(pMainPlayer->GetIsRun())
        action = CShape::ACT_RUN;
    if(pMainPlayer->GetAction() != action)
    {
        pMainPlayer->SetAction(action);
    }
    static long lLastSX = 0,lLastSY = 0;
    static long lLastDX = 0,lLastDY = 0;
    if(lLastSX != (long)fx || lLastSY != (long)fy ||
        lLastDX != (long)fDestX || lLastDY != (long)fDestY)
    {
        lLastSX = (long)fx;lLastSY = (long)fy;
        lLastDX = (long)fDestX; lLastDY = (long)fDestY;
        m_dwMoveMsgCounter++;
        CMessage msg(MSG_C2S_SHAPE_MOVE);
        msg.Add(fx);
        msg.Add(fy);
        msg.Add(fDestX);
        msg.Add(fDestY);	
        msg.Send();
        Log4c::Trace(ROOT_MODULE,"SendMoveMsg,curTime:%d,sx:%f,sy:%f,dx:%f,dy:%f",timeGetTime(),fx,fy,fDestX,fDestY);

#ifdef _CLENTTOTEST_
        CMessage msgtot(MSG_C2T_SHAPE_MOVE);
        msgtot.Add((CGUID)CGUID::GUID_INVALID);
        msgtot.Add(fx);
        msgtot.Add(fy);
        msgtot.Add(fDestX);
        msgtot.Add(fDestY);
        msgtot.SendMemToTest();
#endif
    }
    return 1;
}

//停止移动
long CGameControl::StopMove()
{
    CMessage msg(MSG_C2S_SHAPE_MOVE);
    float fStopX = pMainPlayer->GetPosX();
    float fStopY = pMainPlayer->GetPosY();
    msg.Add(fStopX);
    msg.Add(fStopY);
    msg.Add((BYTE)1);
    msg.Send();
    //OutputDebugString("stop moved!\n");
    //
    //sprintf_s<256>(strMoveInfo,"停止点: X:%f Y:%f\n", fStopX,fStopY);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //DWORD elaspseTime = timeGetTime() - dwBegionMoveTime;
    //if(m_FloatMove->bIsBeelineMove)
    //    sprintf_s(strMoveInfo,256,"移动开始点到停止点移动花费时间:%f秒\n"
    //    ,elaspseTime/1000.f);
    //else
    //    sprintf_s(strMoveInfo,256,"第%d个转向点到停止点移动花费时间:%f秒\n",
    //    m_dwMoveMsgCounter-1,elaspseTime/1000.f);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //sprintf_s<256>(strMoveInfo,"---------------------------------------------\n");
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
#ifdef _CLENTTOTEST_
    CMessage msgtot(MSG_C2T_SHAPE_STOPMOVE);
    msgtot.Add((CGUID)CGUID::GUID_INVALID);
    msgtot.Add(pMainPlayer->GetPosX());
    msgtot.Add(pMainPlayer->GetPosY());
    msgtot.SendMemToTest();
#endif
    return 1;
}


// 捡物品
// 返回: 1表示成功  0表示失败
long CGameControl::PickupGoods(CGoods* pGoods)
{
    // 玩家在骑乘状态下不能拣东西
    if (pMainPlayer->IsRide())
    {
        //if(GetGame()->GetCGuiEx()->GetMainBarPageEx())
        //	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadText(206,75),0xffffffff);
        return 0;
    }
    // 玩家眩晕状态下不能拣东西
    if (pMainPlayer->GetAction()==CShape::ACT_MEGRIM)
    {
        //if(GetGame()->GetCGuiEx()->GetMainBarPageEx())
        //	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadText(206,75),0xffffffff);
        return 0;
    }
    if (pMainPlayer->GetAction()==CShape::ACT_PICK)
    {
        return 0;
    }
    // 判斷玩家是否在物品上面
    long PlayerCellX = pMainPlayer->GetTileX();
    long PlayerCellY = pMainPlayer->GetTileY();
    long GoodsCellX = pGoods->GetTileX();
    long GoodsCellY = pGoods->GetTileY();
    if ( abs(PlayerCellX - GoodsCellX) <= 2 && abs(PlayerCellY - GoodsCellY) <= 2)
    {
        // 捡钱
        if (pGoods->GetProperty()->dwType==GBT_MONEY||pGoods->GetProperty()->dwType==GBT_SILVERMONEY)
        {
            DWORD dwAmount = pGoods->GetNum();
            if ((pGoods->GetProperty()->dwType==GBT_MONEY&&pMainPlayer->GetMoney()+dwAmount>2000000000)||
                (pGoods->GetProperty()->dwType==GBT_SILVERMONEY&&pMainPlayer->GetSilverMoney()+dwAmount>2000000000))
            {
                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadText(208,212),D3DCOLOR_ARGB(255,255,0,0));
                return 0;
            }
            long lPos = pGoods->GetTileY()*pRegion->GetWidth()+pGoods->GetTileX();
            CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
            msg.Add((LONG)TYPE_REGION);
            msg.Add(pRegion->GetExID());
            msg.Add((LONG)0);
            msg.Add((LONG)lPos);
            msg.Add((LONG)TYPE_PLAYER);
            msg.Add(pMainPlayer->GetExID());
            if (pGoods->GetProperty()->dwType==GBT_MONEY)
            {
                msg.Add((LONG)PEI_WALLET);
            }else if (pGoods->GetProperty()->dwType==GBT_SILVERMONEY)
            {
                msg.Add((LONG)PEI_SILVERWALLET);
            }
            msg.Add((LONG)0);
            msg.Add((LONG)TYPE_GOODS);
            msg.Add(pGoods->GetExID());
            msg.Add((LONG)dwAmount);
            msg.Send();
        }
        else
        {
            //背包里面空间不足//不可累加的物品
            if(pMainPlayer->IsAllPackageFull())
            {
                if(!CGoods::isGoodsCanAccumulate(pGoods))
                {
                    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,CStringReading::LoadText(200,37),
                    //	D3DCOLOR_ARGB(255,255,0,0));
                    return 0;
                }
                else
                {
                    WORD ContainerID = pMainPlayer->FindContainerIDOfGoods(pGoods);
                    if (ContainerID == -1) return 0;
                    DWORD dwAmount=pGoods->GetNum();
                    long lPos=pGoods->GetTileY()*pRegion->GetWidth()+pGoods->GetTileX();
                    CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
                    msg.Add((LONG)TYPE_REGION);
                    msg.Add(pRegion->GetExID());
                    msg.Add((LONG)0);
                    msg.Add((LONG)lPos);
                    msg.Add((LONG)TYPE_PLAYER);
                    msg.Add(pMainPlayer->GetExID());
                    msg.Add((LONG)ContainerID);
                    msg.Add((DWORD)0xffffffff);
                    msg.Add((LONG)TYPE_GOODS);
                    msg.Add(pGoods->GetExID());
                    msg.Add((LONG)dwAmount);
                    msg.Send();
                }
            }
            else
            {
                //WORD lExtendID = pMainPlayer->GetNotFullPackage();
                DWORD dwAmount=pGoods->GetNum();
                long lPos=pGoods->GetTileY()*pRegion->GetWidth()+pGoods->GetTileX();
                CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
                msg.Add((LONG)TYPE_REGION);
                msg.Add(pRegion->GetExID());
                msg.Add((LONG)0);
                msg.Add((LONG)lPos);
                msg.Add((LONG)TYPE_PLAYER);
                msg.Add(pMainPlayer->GetExID());
                msg.Add((LONG)1);
                msg.Add((DWORD)0xffffffff);
                msg.Add((LONG)TYPE_GOODS);
                msg.Add(pGoods->GetExID());
                msg.Add((LONG)dwAmount);
                msg.Send();
            }
        }
        //转到面向要拾取的物品方向
        long dir = GetLineDir(pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), pGoods->GetTileX(), pGoods->GetTileY());
        if (dir != pMainPlayer->GetDir())
        {
            pMainPlayer->SetDir((CShape::eDIR)dir);
            // 转方向
            CMessage msg(MSG_C2S_SHAPE_CHANGEDIR);
            msg.Add((BYTE)dir);
            msg.Add(pMainPlayer->GetDirEx());
            msg.Send();
        }
        pMainPlayer->SetAction(CShape::ACT_PICK);
        return 1;
    }
    return 0;
}

//20090701 处理按键移动
void CGameControl::ProcessMoveKey()
{
    DWORD dwMoveStyle = MOVE_NONE;
    static bool bIsInMoveState = false;
    CPlayer *pPlayer = GetGame()->GetMainPlayer();
    DWORD dwCurTime = GetCurTickCount();
    if (GetGame()->GetKeyboard()->IsKeyDown(DIK_W))
        dwMoveStyle = MOVE_FORWARD;
    if (GetGame()->GetKeyboard()->IsKeyDown(DIK_D))
        dwMoveStyle |= MOVE_RIGHT;
    if (GetGame()->GetKeyboard()->IsKeyDown(DIK_A))
        dwMoveStyle |= MOVE_LEFT;
    if (GetGame()->GetKeyboard()->IsKeyDown(DIK_S))
        dwMoveStyle |= MOVE_BACK;
    DWORD lMoveDir = -1; 
    switch (dwMoveStyle)
    {
    case MOVE_FORWARD:
        lMoveDir = CShape::DIR_UP;
        break;
    case MOVE_LEFT:
        lMoveDir = CShape::DIR_LEFT;
        break;
    case MOVE_RIGHT:
        lMoveDir = CShape::DIR_RIGHT;
        break;
    case MOVE_BACK:
        lMoveDir = CShape::DIR_DOWN;
        break;
    case MOVE_LEFT_FORWARD:
        lMoveDir = CShape::DIR_LEFTUP;
        break;
    case MOVE_RIGHT_FORWARD:
        lMoveDir = CShape::DIR_RIGHTUP;
        break;
    case MOVE_LEFT_BACK:
        lMoveDir = CShape::DIR_LEFTDOWN;
        break;
    case MOVE_RIGHT_BACK:
        lMoveDir = CShape::DIR_RIGHTDOWN;
        break;
    }

    static long lDestX = (long)pPlayer->GetDestX();
    static long lDestY = (long)pPlayer->GetDestY();
    if (lMoveDir != -1)
    {
        if (pPlayer->IsArrive())
        {
            //if (bIsInMoveState)
            //	lMoveDir = CShape::DIR_UP;
            pPlayer->GetPosFromCameraView((CShape::eDIR)lMoveDir,lDestX,lDestY);

            BYTE bBlock = GetGame()->GetRegion()->GetCell(lDestX,lDestY)->bBlock;
            if (bBlock != CRegion::BLOCK_CANFLY || bBlock != CRegion::BLOCK_UNFLY)
            {
                MoveToCellByHotKey((float)lDestX,(float)lDestY);
                pPlayer->SetLastUpdatePosTime(dwCurTime);
                bIsInMoveState = true;
            }
        }
    }
    else if (bIsInMoveState)
    {
        if (pPlayer->GetAction() == CShape::ACT_RUN || pPlayer->GetAction() == CShape::ACT_WALK)
        {
            pPlayer->SetAction(CShape::ACT_STAND);
            StopMove();
            pPlayer->SetIsArrive(true);
        }
        bIsInMoveState = false;
    }
}

bool CGameControl::IsPhaseBlock(float xNew,float yNew)
{
    if ( !pMainPlayer)  return false;

    /// 相位外未做
    if ( pMainPlayer->GetPhaseID() == NULL_GUID )  return false;

    /// 相位内阻挡
    float dLen = Distance(xNew, yNew , pMainPlayer->GetPhasePlace().x_point , pMainPlayer->GetPhasePlace().y_point );
    /// 不能走出范围1外  
    if ( dLen >= pMainPlayer->GetPhasePlace().a_size)
    {
         return true;
    }
    return false;
}
// 处理快捷键操作
long CGameControl::ProcessHotKey()
{
    //处理WASD移动 add by nikai
    //ProcessMoveKey();
    /////////////
    //使用快捷健
    unsigned char Keycode = GetKeyboard()->GetCurrentKey();
    if(Keycode == 0)
        return 0;

    int nFButton=0;
    switch(Keycode)
    {
    case DIK_F1: nFButton=1;break;
    case DIK_F2: nFButton=2;break;
    case DIK_F3: nFButton=3;break;
    case DIK_F4: nFButton=4;break;
    case DIK_F5: nFButton=5;break;
    case DIK_F6: nFButton=6;break;
    case DIK_F7: nFButton=7;break;
    case DIK_F8: nFButton=8;break;
    case DIK_F9: nFButton=9;break;
    case DIK_F10: nFButton=10;break;
    case DIK_F11: nFButton=11;break;
    case DIK_F12: nFButton=12;break;
        /////////////////////////////////////////////////
        // zhaohang  2010/5/19 
        // 恢复1~+快捷键
        /////////////////////////////////////////////////
    case DIK_1: nFButton=13;break;
    case DIK_2: nFButton=14;break;
    case DIK_3: nFButton=15;break;
    case DIK_4: nFButton=16;break;
    case DIK_5: nFButton=17;break;
    case DIK_6: nFButton=18;break;
    case DIK_7: nFButton=19;break;
    case DIK_8: nFButton=20;break;
    case DIK_9: nFButton=21;break;
    case DIK_0: nFButton=22;break;
    case DIK_MINUS: nFButton=23;break;
    case DIK_EQUALS: nFButton=24;break;

    default: break;
    }

    // 当前不存在交点空间，可执行1~+的快捷键（07.9.21刘科修改）
    /*CComponent *pComp = IGUIRoot::GetGUIInstance()->GetFocusComp();
    if(pComp == NULL)
    {
    switch(Keycode)
    {
    case DIK_1: nFButton=13;break;
    case DIK_2: nFButton=14;break;
    case DIK_3: nFButton=15;break;
    case DIK_4: nFButton=16;break;
    case DIK_5: nFButton=17;break;
    case DIK_6: nFButton=18;break;
    case DIK_7: nFButton=19;break;
    case DIK_8: nFButton=20;break;
    case DIK_9: nFButton=21;break;
    case DIK_0: nFButton=22;break;
    case DIK_MINUS: nFButton=23;break;
    case DIK_EQUALS: nFButton=24;break;
    default:break;
    }
    }*/

    if (nFButton!=0)
    {
        nFButton--;

        DWORD dwVal=pMainPlayer->GetHotKey(nFButton);
        // 宠物相关代码注释 Bugs 2008-9-12
        //unsigned char pPetKey[6] = {DIK_1,DIK_2,DIK_3,DIK_4,DIK_5,DIK_6};
        if ((dwVal&0x80000000)==0)	// 物品
        {
            if (IsCanUseGoodsCondition(dwVal))
            {
                pMainPlayer->UseItem(dwVal);
            }
        }
        else	// 技能
        {
            dwVal &= 0x7fffffff;
            CMainPlayerHand* pHand = 0;//GetGame()->GetCGuiEx()->GetPMainPlayerHand();
            // 使用坐骑技能，手上不能的物品不能是已装备的坐骑物品
            if(dwVal >= SKILL_RIDE_START && dwVal <= SKILL_RIDE_END)
            {
                // 未装备坐骑，提示失败信息
                if( pMainPlayer->GetEquip(CPlayer::EQUIP_RIDER) == NULL )
                {
                    GetInst(MsgEventManager).PushEvent( Msg_Ok , AppFrame::GetText("Skill_33") );   //未装备坐骑无法进入乘骑状态！
                    return false;
                }
                if(pHand && pHand->GetPGoodsOfMainPlayerHand() == pMainPlayer->GetEquip(CPlayer::EQUIP_RIDER))
                    return false;
            }
            // 更新相应的快捷键
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->UseHotKey(nFButton,dwVal);
            // 被动技能无法使用
            CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(dwVal);
            if (!pSkill)
                return false;
            // 被动技能
            if (pSkill->dwSkillType == CSkillListXml::SKILLTYPE_PASSIVE)
            {
                /*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
                CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_PASSSKILL),
                D3DCOLOR_ARGB(255,255,0,0));*/
                return false;
            }
            // 玩家身上找到这个技能的属性
            CPlayer::tagSkill *pS = pMainPlayer->GetSkill(dwVal);
            // 范围待放魔法，不处于冷却状态
            if(pHand && pS && pS->dwCoolDownStartTime <= 0)
            {
                pHand->SetMainPlayerHandByEffect(0,0);
                if(pSkill->dwEffectID > 0)
                {
                    pHand->SetMainPlayerHandByEffect(dwVal,pSkill->dwEffectID);
                    return false;
                }
            }
            // 目标
            CMoveShape *pAttactObj = NULL;
            if(m_pSelectedShape)
                pAttactObj = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(m_pSelectedShape->GetType(), m_pSelectedShape->GetExID()));
            if(!pAttactObj && m_pCurAttractShape)
                pAttactObj = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(m_pCurAttractShape->GetType(), m_pCurAttractShape->GetExID()));
            // 如果需要锁定目标且当前无锁定目标，将攻击目标设为自己
            if (NULL == pAttactObj && pSkill->bLockTarget)
            {
                pAttactObj = GetGame()->GetMainPlayer();
            }
            if(!pS)
                return false;
            // 锁定目标技能，非连续攻击技能，同时按下‘~’键，将释放目标设置成自己，且停止目前的自动攻击
            if(!pSkill->bLockSkill && pSkill->bLockTarget && GetKeyboard()->IsKeyDown(DIK_GRAVE))
            {
                AutoAttactStop();
                pAttactObj = GetGame()->GetMainPlayer();
            }
            // 如果技能有动作，将该技能设置成缓冲技能
            if(pSkill->bHasAct)
            {
                if(GetMPActionMode() == MPAM_AutoMove)
                    AutoMoveStop();
                // 达到释放条件
                long x = 0,y = 0;
                if(pAttactObj)
                {
                    x = pAttactObj->GetTileX();
                    y = pAttactObj->GetTileY();
                }
              
				/***********************************************************************/	
				/* 赵航 fix */				
				// CSkillXml skill;				
				/***********************************************************************/	
				SkillComponent* skillcom = new ConcreteSkillComp();;				
				CSkillXml skill(skillcom);                // 释放条件满足
                if(skill.StepSend(dwVal,pS->wLevel,pMainPlayer,pAttactObj,x,y))
                {
                    //缓冲技能
                    SetBufferSkillID(dwVal);
                    // 对目标使用技能
                    StartAttack(pAttactObj);
                }
                else
                    pS->dwUseingStartTime = 0;
            }
            // 无动作的技能直接释放
            else if(IsActWithCurAction() || 
                pMainPlayer->GetAction() == CShape::ACT_RUN ||
                pMainPlayer->GetAction() == CShape::ACT_WALK)
            {
				/***********************************************************************/				/* zhaohang fix */				/***********************************************************************/                if(!StartSkill(dwVal,pAttactObj) && pSkill->bLockSkill)
                    pS->dwUseingStartTime = 0;
            }
            SetLastPressHotKey(GetKeyboard()->GetCurrentKey());
            return true;
        }
    }
    return false;
}

// 追击当前正在被攻击对象的AI
CGameControl::eAAMRet CGameControl::AutoAttackMoveAI()
{
    if(NULL == m_pCurAttractShape)	return AAMR_Stop;
    // 锁定目标技能，用来判断和锁定目标的距离使用
    // 有缓冲技能的话先释放缓冲技能
    if(GetBufferSkillID() != 0)
        m_dwCurSkillID = GetBufferSkillID();
    if(m_dwCurSkillID == 0)
        return AAMR_Stop;
    // 找到技能信息
    CPlayer::tagSkill * pSkill = pMainPlayer->GetSkill(m_dwCurSkillID);
    if (pSkill == NULL)
        return AAMR_Stop;
    CSkillListXml::tagSkill *pS = CSkillListXml::GetProperty(m_dwCurSkillID);
    if (pS == NULL)
        return AAMR_Stop;
    // 如果无锁定目标,直接返回为攻击模式
    if(!pS->bLockTarget)
        return AAMR_Attack;
    long lDesX = m_pCurAttractShape->GetTileX();
    long lDesY = m_pCurAttractShape->GetTileY();
    CMoveShape *pShape = dynamic_cast<CMoveShape *>(m_pCurAttractShape);
    if(pShape)
    {
        lDesX = pShape->GetTileX();
        lDesY = pShape->GetTileY();
    }
    // 追踪
    // 计算攻击目标的有效范围
    long lDistance = pMainPlayer->RealDistance(pShape);//Distance(pMainPlayer->GetTileX(),pMainPlayer->GetTileY(),lDesX,lDesY);
    float fSkillRangeMax = pSkill->fAffectRangeMax;
    // 攻击距离合法，返回攻击模式
    if (lDistance >= pSkill->fAffectRangeMin && lDistance <= fSkillRangeMax)
    {
        return AAMR_Attack;
    }

    long lDestX = pMainPlayer->GetTileX();
    long lDestY = pMainPlayer->GetTileY();
    //float fDir = CShape::GetLineDirF(m_pCurAttractShape->GetPosX(),m_pCurAttractShape->GetPosY()
    //	,pMainPlayer->GetPosX(),pMainPlayer->GetPosY());
    //CShape::eDIR eDir = pMainPlayer->RadianDirTo8Dir(fDir);
    long lDir = GetLineDir(m_pCurAttractShape->GetTileX(),
		m_pCurAttractShape->GetTileY(),
		pMainPlayer->GetTileX(),
		pMainPlayer->GetTileY());
    //得到目标周围的空格子
    CMoveShape* pAttackShape = dynamic_cast<CMoveShape*>(m_pCurAttractShape);
    if(pAttackShape)
        pRegion->GetLeastShapesCellFromCircularSpace((int)pAttackShape->GetDestX(),
		(int)pAttackShape->GetDestY(),
        pAttackShape->GetFigure(CShape::DIR_RIGHT),
		pAttackShape->GetFigure(CShape::DIR_UP),
        fSkillRangeMax,
		lDir,
		lDestX,
		lDestY);
    //取得下一步移动的位置

    float fDestX = pMainPlayer->GetPosX();
    float fDestY = pMainPlayer->GetPosY();			
    if(GetNextMoveDestCell(lDestX+0.5f,lDestY+0.5f,MAX_MOVE_CELLNUM,fDestX,fDestY))
    {
        MoveToCell(fDestX,fDestY);
    }
    else
        return AAMR_Stop;
    return AAMR_Trace;
}


//锁定技能攻击AI
//返回：-1停止自动攻击模式 0追击 1攻击
CGameControl::eAAMRet CGameControl::AutoLockAttackAI()
{
    // 锁定目标技能，用来判断和锁定目标的距离使用
    m_dwCurSkillID = GetBufferSkillID();
    if(m_dwCurSkillID == 0 && GetAutoSkillID())
        m_dwCurSkillID = GetAutoSkillID();

    if(m_dwCurSkillID == 0)
        return AAMR_Stop;
    static long lUseSkillTime = 0;
    static DWORD time = timeGetTime();
    if(timeGetTime() - time < 1000)
        ++lUseSkillTime;
    else
    {
        time = timeGetTime();
        lUseSkillTime = 0;
        if(g_bDebug)
        {
            Log4c::Trace(ROOT_MODULE,"*******SKILL_USE:%d,时间:%d,已超过一秒，计数清0******",m_dwCurSkillID,time);
        }
    }
    long lCount = 5;
    CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
    if(pSetup)
        lCount = pSetup->lUseSkillTime;
    if(lUseSkillTime > lCount)
        return AAMR_Attack;
    if(g_bDebug)
    {
        Log4c::Trace(ROOT_MODULE,"*******SKILL_USE:%d,一秒钟中的第%d次,时间:%d*******",m_dwCurSkillID,lUseSkillTime,timeGetTime());
    }
    if ( m_dwCurSkillID == SKILL_BASE_BUFF )
        m_dwCurSkillID = GetGame()->GetMainPlayer()->GetDefaultAttackSkillID();

    // 追踪
    CPlayer::tagSkill * pSkill = pMainPlayer->GetSkill(m_dwCurSkillID);
    if (pSkill == NULL)
    {
        return AddSkillByGoods(m_dwCurSkillID);
    }
    CSkillListXml::tagSkill *pS = CSkillListXml::GetProperty(m_dwCurSkillID);
    if (pS == NULL)
        return AAMR_Stop;
    // 处于吟唱状态，吟唱完毕继续攻击
    if(GetGame()->GetMainPlayer()->GetIntonateKey()!=0)
    {
        DWORD dwID = GetGame()->GetMainPlayer()->GetIntonateState();
        // 坐骑的吟唱，返回停止
        if(dwID >= SKILL_RIDE_START && dwID <= SKILL_RIDE_END)
            return AAMR_Stop;
        return AAMR_Attack;
    }
    // 技能冷却时间未到
    if(pSkill->dwCoolDownStartTime > 0)
    {
        // 该技能不是连续攻击技能，返回并停止攻击
        if(GetAutoSkillID() != m_dwCurSkillID)
        {
            /*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
            CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_COOLTIME),
            D3DCOLOR_ARGB(255,255,0,0));*/
            // 如果该技能是缓冲技能，清空缓冲技能
            if(GetBufferSkillID() == m_dwCurSkillID)
                SetBufferSkillID(0);
            if(GetAutoSkillID() > 0)
                return AAMR_Attack;
            return AAMR_Stop;
        }
        // 连续攻击
        else
        {
            SetBufferSkillID(GetAutoSkillID());
            return AAMR_Attack;
        }
    }
    // 该技能是物理技能，但目前又不能使用物理技能，返回
    if(pS->dwSkillUseType == 1 && !IsCanUsePhysicsSkillCondition())
        return AAMR_Stop;
    // 该技能是魔法技能，但目前又不能使用魔法技能，返回
    if(pS->dwSkillUseType == 2 && !IsCanUseMagicSkillCondition())
        return AAMR_Stop;
    // 该技能不需要锁定目标
    if(!pS->bLockTarget)
    {
        // 施放失败
        if(!StartSkill(m_dwCurSkillID, NULL))
        {
            if(m_dwCurSkillID == GetBufferSkillID())
                SetBufferSkillID(0);
            else if(m_dwCurSkillID == GetAutoSkillID())
                SetAutoSkillID(0);
        }
        return AAMR_Attack;
    }
    // 有攻击目标
    else if( m_pCurAttractShape && m_pCurAttractShape->GetExID()!=CGUID::GUID_INVALID && m_pCurAttractShape->GetType()!=0)
    {
        // 追击攻击目标
        CMoveShape* pAttactObj = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(m_pCurAttractShape->GetType(), m_pCurAttractShape->GetExID()));
        if(pAttactObj)
        {
            // 目标死亡，且该技能不能对已死亡的对象使用
            if(pAttactObj->IsDied() && !pS->bIsUseToDiedObj)
            {
                SetAutoSkillID(0);
                SetBufferSkillID(0);
                return AAMR_Stop;
            }
            // 不是对自己释放技能，判断距离合法性
            if(pAttactObj != pMainPlayer)
            {
                // 计算攻击目标的有效范围
                long lDistance = pMainPlayer->RealDistance(pAttactObj);//Distance(pMainPlayer->GetTileX(),pMainPlayer->GetTileY(),pAttactObj->GetTileX(),pAttactObj->GetTileY());
                float fSkillRangeMax = pSkill->fAffectRangeMax;
                float fSkillRangeMin = pSkill->fAffectRangeMin;
                if(fSkillRangeMax != 0 || fSkillRangeMin != 0)
                {
                    //在最大,最小追踪范围外
                    if (lDistance > fSkillRangeMax || lDistance < fSkillRangeMin)
                    {
                        // 追踪
                        return AAMR_Trace;
                    }
                }
            }
            // 释放技能
            if(!StartSkill(m_dwCurSkillID, pAttactObj))
            {
                if(m_dwCurSkillID == GetBufferSkillID())
                    SetBufferSkillID(0);
                else if(m_dwCurSkillID == GetAutoSkillID())
                    SetAutoSkillID(0);
            }
            /*char strInfo[256]="";
            _snprintf(strInfo,256,"DistanceMsg:%d,MaxDis:%d,MinDis:%d,sx:%d,sy:%d,dx:%d,dy:%d",lDistance,wSkillRangeMax,pSkill->wAffectRangeMin,
            pMainPlayer->GetTileX(),pMainPlayer->GetTileY(),pAttactObj->GetTileX(),pAttactObj->GetTileY());
            PutStringToFile("MoveTest",strInfo);*/
            return AAMR_Attack;
        }
    }
    return AAMR_Stop;
}

// 使用副职业技能
bool CGameControl::UseOccuSkill(DWORD dwIndex,CShape* pTagShape)
{
    //if (pMainPlayer->GetAction() != CShape::ACT_STAND )
    //{
    //	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOTUSESKILLEX));
    //	PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOTUSESKILLEX));
    //	return false;
    //}
    // 处于吟唱状态
    if(pMainPlayer->GetIntonateKey()!=0)
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_INTTIME));
        //return false;
    }
    //CPlayerTradePageEx* pTradePage = GetGame()->GetCGuiEx()->GetPlayerTradePageEx();
    //if (pTradePage!=NULL&&pTradePage->IsOpen())
    //{
    //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOTCOLLECT));
    //PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOTCOLLECT));
    //return false;
    //}
    if(pTagShape==NULL)
    {
        Log4c::Trace(ROOT_MODULE,GetText("GC_5"));  //目标空！
        return false;
    }
    pMainPlayer->SetToolsID(dwIndex,((CCollectionGoods*)pTagShape)->GetActionID());
    CMessage msg(MSG_C2S_DOCCUSKILL_USE);
    msg.Add(dwIndex);
    if (dwIndex==CDeputyOccuSystemClient::OB_Facture)
    {
        msg.Add((DWORD)0);
        msg.Add(CGUID::GUID_INVALID);
        msg.Add((DWORD)1);
    }else if(dwIndex==CDeputyOccuSystemClient::OB_Collection||
        dwIndex==CDeputyOccuSystemClient::OB_TaskCollection)
    {
        msg.Add((DWORD)pTagShape->GetType());
        msg.Add(pTagShape->GetExID());
        msg.Add((DWORD)0);
    }
    msg.Send();
    Log4c::Trace(ROOT_MODULE,GetText("GC_6"));  //消息发送成功！
    return true;
}

// 检测一个由物品触发的技能
CGameControl::eAAMRet CGameControl::AddSkillByGoods(DWORD m_dwCurSkillID)
{
    if (pMainPlayer->GetAction() != CShape::ACT_IDLE && 
        pMainPlayer->GetAction() != CShape::ACT_STAND &&
        pMainPlayer->GetAction() != CShape::ACT_RUN &&
        pMainPlayer->GetAction() != CShape::ACT_WALK &&
        pMainPlayer->GetAction() != CShape::ACT_BEATTACK &&
        pMainPlayer->GetAction() != CShape::ACT_BLOCK &&
        pMainPlayer->GetAction() != CShape::ACT_PARRY &&
        pMainPlayer->GetAction() != CShape::ACT_MISS &&
        !IsActWithCurAction())
    {
        return AAMR_Attack;
    }

    // 如果该技能为缓冲技能,设置缓冲技能为0
    if(m_dwCurSkillID == GetBufferSkillID())
        SetBufferSkillID(0);

    CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(m_dwCurSkillID);
    if (pSkill == NULL)
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NOSKILL));
        return AAMR_Stop;
    }

    // 处于吟唱状态
    if(GetGame()->GetMainPlayer()->GetIntonateKey()!=0)
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_INTTIME));
        return AAMR_Attack;
    }

    // 消息发送需要的数据
    long x = 0,y = 0;
    long lType = 0;
    CGUID guid = CGUID::GUID_INVALID;
    CMoveShape * pTagShape = NULL;
    // 技能需要锁定目标,且攻击目标存在
    if(pSkill->bLockTarget && m_pCurAttractShape)
    {
        pTagShape = (CMoveShape *)m_pCurAttractShape;
        lType = m_pCurAttractShape->GetType();
        guid = m_pCurAttractShape->GetExID();
        x = m_pCurAttractShape->GetTileX();
        y = m_pCurAttractShape->GetTileY();
    }
    // 否则如果是不需要锁定目标的技能
    else if(!pSkill->bLockTarget)
    {
        float lMouseX = 0.0f, lMouseY = 0.0f;
        pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), lMouseX, lMouseY);
        x = (long)lMouseX;
        y = (long)lMouseY;
    }
    // 需要锁定目标，目标又不存在，返回失败
    else
        return AAMR_Stop;

    CMessage msg(MSG_C2S_SKILL_USE);
    msg.Add(m_dwCurSkillID);
    msg.Add( lType );
    msg.Add( guid );
    msg.Add( x );
    msg.Add( y );
    msg.Add((BYTE)1);	
    msg.Add((long)m_lSkillGoodsExtendID);
    msg.Add((long)m_lSkillGoodsPos);

    //msg.Add( m_dwGoodsIndexForSkill );
    msg.Send();

    //CGoods* pGoods = GetGame()->GetMainPlayer()->GetGoodsByPosition(m_lSkillGoodsExtendID,m_lSkillGoodsPos);
    //if (pGoods!=NULL)
    //{
    //	CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
    //	pGoods->Lock();
    //	if (pItemPage!=NULL)
    //	{
    //		pItemPage->UpdateGoodsShow(m_lSkillGoodsExtendID,(int)m_lSkillGoodsPos);
    //	}
    //}

    m_lSkillGoodsExtendID = 0;
    m_lSkillGoodsPos = -1;
    //m_dwGoodsIndexForSkill = 0;

    SetSendedUseSkillMsgCount(GetSendedUseSkillMsgCount()+1);

    return AAMR_Stop;
}

// 对指定目标发起技能
bool CGameControl::StartSkill(DWORD dwIndex, CShape* pAttackShape)
{
    /*if (pMainPlayer->GetAction() != CShape::ACT_IDLE && 
    pMainPlayer->GetAction() != CShape::ACT_STAND &&
    pMainPlayer->GetAction() != CShape::ACT_RUN &&
    pMainPlayer->GetAction() != CShape::ACT_WALK &&
    pMainPlayer->GetAction() != CShape::ACT_BEATTACK &&
    pMainPlayer->GetAction() != CShape::ACT_BLOCK &&
    pMainPlayer->GetAction() != CShape::ACT_PARRY &&
    pMainPlayer->GetAction() != CShape::ACT_MISS &&
    !IsActWithCurAction())
    {
    return false;
    }*/

    // 要发射的技能
    DWORD dwSkillID = dwIndex;

    // 如果该技能为缓冲技能,设置缓冲技能为0
    //if(dwSkillID == GetBufferSkillID())
    //	SetBufferSkillID(0);

    // 确定基本攻击技能
    if(dwIndex >= SKILL_BASE_BUFF && dwIndex <= SKILL_BASE_BLADEFIST)
        dwSkillID = GetGame()->GetMainPlayer()->GetDefaultAttackSkillID();


    CPlayer::tagSkill *pS = pMainPlayer->GetSkill(dwSkillID);
    if(pS == NULL)
        return false;

    CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(dwSkillID);
    if (pSkill == NULL)
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NOSKILL));
        return false;
    }
    // 该技能是物理技能，但目前又不能使用物理技能，返回
    if(pSkill->dwSkillUseType == 1 && !IsCanUsePhysicsSkillCondition())
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_CANNOTPHY));
        return false;
    }
    // 该技能是魔法技能，但目前又不能使用魔法技能，返回
    if(pSkill->dwSkillUseType == 2 && !IsCanUseMagicSkillCondition())
    {
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddChatWords(CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_CANNOTMAGIC));
        return false;
    }
    // 技能冷却时间未到
    if(pS->dwCoolDownStartTime > 0)
    {
        if(GetAutoSkillID() != m_dwCurSkillID)
        {
            /*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
            CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_COOLTIME),
            D3DCOLOR_ARGB(255,255,0,0));*/
            // 如果该技能是缓冲技能，清空缓冲技能
            if(GetBufferSkillID() == m_dwCurSkillID)
                SetBufferSkillID(0);
        }
        // 连续攻击
        else
            SetBufferSkillID(SKILL_BASE_BUFF);
        return false;
    }
    // 消息发送需要的数据
    long x = 0,y = 0;
    long lType = 0;
    CGUID guid = CGUID::GUID_INVALID;
    CMoveShape * pTagShape = NULL;
    // 技能需要锁定目标,且攻击目标存在
    if(pSkill->bLockTarget && pAttackShape)
    {
        pTagShape = (CMoveShape *)pAttackShape;
        lType = pAttackShape->GetType();
        guid = pAttackShape->GetExID();
        x = pAttackShape->GetTileX();
        y = pAttackShape->GetTileY();
    }
    // 否则如果是不需要锁定目标的技能
    else if(!pSkill->bLockTarget)
    {
        // 释放的是当前的范围技能
        if(pSkill->dwSkillID == m_stCurAreaSkill.dwSkillID)
        {
            x = (long)m_stCurAreaSkill.fPosX;
            y = (long)m_stCurAreaSkill.fPosY;
            m_stCurAreaSkill.dwSkillID = 0;
        }
        else
        {
            float lMouseX = 0.0f, lMouseY = 0.0f;
            pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), lMouseX, lMouseY);
            x = (long)lMouseX;
            y = (long)lMouseY;
        }
    }
    // 需要锁定目标，目标又不存在，返回失败
    else
        return false;
    // 判断该技能是否可以对目标使用
    bool bIsUse = false;
    
	/***********************************************************************/	
	/* 赵航 fix */	
	//CSkillXml *m_pSkill = new CSkillXml();	
	/***********************************************************************/
	ConcreteSkillComp* skillcom = new ConcreteSkillComp();	
	ConcreteDecSkillXml* m_pSkill = new ConcreteDecSkillXml(skillcom);   
	if(m_pSkill->StepSend(dwSkillID,pS->wLevel,pMainPlayer,pTagShape,x,y))
    {
        // 使用的是连续攻击技能,且传入的技能不是基本缓存技能
        if (pSkill->bLockSkill && dwIndex != SKILL_BASE_BUFF)
        {
            // 该连续攻击技能不是之前的，替换连续攻击技能
            if(dwSkillID != GetAutoSkillID())
            {
                SetAutoSkillID(dwSkillID);
            }
        }
        long lCount = 3;
        CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
        if(pSetup)
            lCount = pSetup->lUseSkillMsgCount;
        // 在未收到服务器响应时的累计发送次数
        if(GetSendedUseSkillMsgCount() < lCount)
        {
            // 光环类技能且该技能正在使用，发送结束消息
            if(pMainPlayer->IsHaveSkillByIndex(dwSkillID) && !pSkill->bLockSkill && pSkill->dwSkillState != 0)
            {
                CMessage msg(MSG_C2S_SKILL_USE_END);
                msg.Add((LONG)-1);
                msg.Add((LONG)dwSkillID);
                msg.Send();
            }
            else
            {
                float lMouseX = 0.0f, lMouseY = 0.0f;
                pRegion->CalcMousePoint(GetMouse()->GetMouseX(), GetMouse()->GetMouseY(), lMouseX, lMouseY);
                CMessage msg(MSG_C2S_SKILL_USE);
                msg.Add(dwSkillID);
                msg.Add( lType );
                msg.Add( guid );
                msg.Add( x );
                msg.Add( y );
                msg.Add((BYTE)0);
                msg.Send();
                if(g_bDebug)
                {
                    Log4c::Warn(ROOT_MODULE,"========SKILL_USE:%d,第%d次=========",dwSkillID,GetSendedUseSkillMsgCount());
                }
            }
            SetSendedUseSkillMsgCount(GetSendedUseSkillMsgCount()+1);
            // 如果该技能为缓冲技能,设置缓冲技能为0
            if(dwIndex == GetBufferSkillID())
                SetBufferSkillID(0);
        }
        bIsUse = true;
    }
    SAFEDELETE(m_pSkill);
    return bIsUse;
}

// 对指定目标发起技能
bool CGameControl::StartSkill(DWORD dwIndex, long lType, const CGUID& ID)
{
    CMoveShape* pShape = (CMoveShape*)pRegion->FindChildObject(lType, ID);
    return StartSkill(dwIndex,pShape);
}

//////////////////////////////////////////////////////////////////////////
//	自动跑步 / 自动跟随
//////////////////////////////////////////////////////////////////////////

// 跟随
long CGameControl::FollowObjectAI()
{
    CShape* pShape = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lFollowingObjType, m_FollowingObjID));
    if (pShape)
    {
        if(m_fStartDestX != pShape->GetTileX() || m_fStartDestY != pShape->GetTileY())
        {
            long lDestX = pMainPlayer->GetTileX();
            long lDestY = pMainPlayer->GetTileY();
            // 和目标距离已经在最大允许范围之内，跟踪结束
            if ( abs(lDestX-pShape->GetTileX()) <= m_lFollowMaxDistance && 
                abs(lDestY-pShape->GetTileY()) <= m_lFollowMaxDistance )
            {
                return 0;
            }
            long lDir = GetLineDir(pMainPlayer->GetTileX(),pMainPlayer->GetTileY()
                ,pShape->GetTileX(),pShape->GetTileY());
            pRegion->GetLeastShapesCellFromSpace(pShape->GetTileX(),pShape->GetTileY(),
                0,0,m_lFollowMaxDistance,lDir,lDestX,lDestY);

            m_fDestX = lDestX + 0.5f;
            m_fDestY = lDestY + 0.5f;
            m_fStartDestX = pShape->GetPosX();
            m_fStartDestY = pShape->GetPosY();
        }
        long lX = pMainPlayer->GetTileX();
        long lY = pMainPlayer->GetTileY();
        //如果目标点有Shape,并且和Shape之间的距离为1是就停止移动
        // 到达目标坐标，停止自动移动
        if ( abs(lX-(long)m_fDestX) < 1 && abs(lY-(long)m_fDestY) < 1 )
        {
            return 0;
        }
        float fDestX = pMainPlayer->GetPosX();
        float fDestY = pMainPlayer->GetPosY();			
        if(GetNextMoveDestCell(m_fDestX,m_fDestY,MAX_MOVE_CELLNUM,fDestX,fDestY))
        {
            MoveToCell(fDestX,fDestY);
            return 1;
        }
    }
    return 0;
}

//处理跟随的结果
void CGameControl::DoFollowResult()
{
    CShape* pShape = dynamic_cast<CShape*>(pRegion->FindChildObject(m_lFollowingObjType, m_FollowingObjID));
    if (pShape)
    {
        switch(pShape->GetType())
        {
        case TYPE_MONSTER:
            {
                SendTouchNpcMes(pShape->GetExID());
            }
            break;
        case TYPE_GOODS:
            {
                PickupGoods(dynamic_cast<CGoods*>(pShape));
            }
            break;
        case TYPE_COLLECTION:
            {
                DoCollect(dynamic_cast<CCollectionGoods*>(pShape));
            }
            break;
        case TYPE_PLAYER:
            break;
        }
    }
    else	// 跟踪目标丢失，取消跟踪
        SetFollowingObj(0,CGUID::GUID_INVALID);
}
//////////////////////////////////////////////////////////////////////////
//	自动寻路
//////////////////////////////////////////////////////////////////////////

// 自动行走(A*)
void CGameControl::AutoMoveTo(float DestX, float DestY)
{
    /// 判断相位阻挡
    const float fDestX = DestX;
    const float fDestY = DestY;
    if ( GetFollowType() && m_FollowDestX == fDestX && m_FollowDestY == fDestY )
    {
        return ;
    }
    if( IsPhaseBlock(fDestX,fDestY) )
        return ;
    const float fPosX = pMainPlayer->GetPosX();
    const float fPosY = pMainPlayer->GetPosY();
    if (abs(fPosX - fDestX) <= m_FloatMove->MINIMUS_OP_DISTANCE &&
        abs(fPosY - fDestY) <= m_FloatMove->MINIMUS_OP_DISTANCE)
    {
        return;
    }
    pMainPlayer->SetIsArrive(false);
    dwBegionMoveTime = timeGetTime();
    m_FollowDestX = fDestX;
    m_FollowDestY = fDestY;
    GetFindPath()->GetPathList()->clear();
    // 清空之前走过的格子记录
    m_MovedCell.clear();
    //如果但前处于自动攻击模式中，则先停止自动攻击
    if(GetMPActionMode() == MPAM_AutoAttack)
    {
        //停止自动攻击
        AutoAttactStop();
    }

    //移动行为模式
    if(GetMPActionMode() != MPAM_AutoMove)
        SetMPNexActionMode(MPAM_AutoMove);

    // 开始移动(站立或被攻击、闪避、格挡、招架状态下可移动)
    m_fDestX = fDestX;
    m_fDestY = fDestY;
    DWORD dwOld = timeGetTime();
    if(m_AutoMoveMode == AMM_A_SEARCH)
    {
        GetFindPath()->AddPathPos(pRegion->GetResourceID(),
			pMainPlayer->GetTileX(), 
			pMainPlayer->GetTileY(), 
			(long)fDestX, 
			(long)fDestY);
        // 寻路
        //GetFindPath()->Find(pMainPlayer->GetDestX(), pMainPlayer->GetDestY(), lDestX, lDestY);
        if(GetFindPath()->GetPathList()->size() > 0)
            GetFindPath()->GetPathList()->pop_front();	// 去掉第一个点
    }
    else
    {
        m_AutoMoveMode= AMM_A_SEARCH;
        // 寻路
        GetFindPath()->AddPathPos(pRegion->GetResourceID(),
			pMainPlayer->GetTileX(), 
			pMainPlayer->GetTileY(),
			(long)fDestX, 
			(long)fDestY);
        //GetFindPath()->Find(pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), lDestX, lDestY);
        if(GetFindPath()->GetPathList()->size() > 0)
            GetFindPath()->GetPathList()->pop_front();	// 去掉第一个点,因为就是自己站的点
    }

    //sprintf_s<256>(strMoveInfo,"---------------------------------------------\n");
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //sprintf_s<256>(strMoveInfo,"鼠标点击点 X:%f Y:%f\n", fDestX,fDestY);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //sprintf_s<256>(strMoveInfo,"当前位置 X:%f Y:%f\n", fPosX,fPosY);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    m_FloatMove->UpdateA_MoveList(GetFindPath(),fPosX,fPosY,fDestX,fDestY);
    m_dwMoveMsgCounter = 0;
    if (!m_FloatMove->A_MoveList.empty())
    {
        CFloatMove::tagMovPos firstStep;
        firstStep.x = m_FloatMove->A_MoveList.begin()->x;
        firstStep.y = m_FloatMove->A_MoveList.begin()->y;
        m_FloatMove->bIsPreviousMoveByA_Star = true;
        m_FloatMove->UpdatelinearMoveList(fPosX,fPosY,firstStep.x,firstStep.y);
        if(!m_FloatMove->LineMoveList.empty())
        {
           firstStep.x = m_FloatMove->LineMoveList.begin()->x;
           firstStep.y = m_FloatMove->LineMoveList.begin()->y;
           m_FloatMove->LineMoveList.pop_front();
           m_FloatMove->bIsPreviousMoveByA_Star = false;
           //sprintf_s(strMoveInfo,256,"直线拆分表中点X:%f Y:%f\n",firstStep.x,firstStep.y);
           //OutputDebugString(strMoveInfo);
           //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
        }
        
        MoveToCell(firstStep.x, firstStep.y);
        //if (m_FloatMove->bIsBeelineMove)
        //    sprintf_s(strMoveInfo,256,"目标点 X: %f Y: %f\n", firstStep.x, firstStep.y);
        //else
        //    sprintf_s(strMoveInfo,256,"第%d个转向点 X:%f Y:%f \n", m_dwMoveMsgCounter, firstStep.x, firstStep.y);
        //OutputDebugString(strMoveInfo);
        //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
    }
    else
        return ;

    SetFollowingObj(0,CGUID::GUID_INVALID);
    // 设置在大地图上显示的目标点标示
    CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
    if(pMiniMap)
    {
        pMiniMap->SetDestPos((int)fDestX,(int)fDestY);
    }
    // 记录自动移动的信息
    //CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
    //if(pMainPage && pMainPlayer)
    //	pMainPage->SetMoveInfo(pMainPlayer->GetRegionID(),lDestX,lDestY);
}

// 开始自动行走,直接向目标方向移动
void CGameControl::AutoMoveToEx(float fDestX, float fDestY)
{
    if( IsPhaseBlock(fDestX,fDestY) )
        return ;
    const long lTileX = pMainPlayer->GetTileX();
    const long lTileY = pMainPlayer->GetTileY();
    const long lDestX = (long)fDestX;
    const long lDestY = (long)fDestY;
    const float fPosX = pMainPlayer->GetPosX();
    const float fPosY = pMainPlayer->GetPosY();

    
    //sprintf_s<256>(strMoveInfo,"---------------------------------------------\n");
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //sprintf_s<256>(strMoveInfo,"鼠标点击点 X:%f Y:%f\n", fDestX,fDestY);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

    //sprintf_s<256>(strMoveInfo,"当前位置 X:%f Y:%f\n", fPosX,fPosY);
    //OutputDebugString(strMoveInfo);
    //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
    /////////////////////////////////////////////////////////////////////////////////////
    long lX = lDestX;
    long lY = lDestY;
    long lDir = - 1;

    //由于途中有阻挡而导致调整目标点位置 标志
    bool bBlockedToAdjustDestPos = false;
    if( /*pRegion->GetCellObjNum(lX,lY) > 0 ||*/ 
        (pRegion->GetBlock(lX,lY) != CRegion::BLOCK_NO && 
        pRegion->GetBlock(lX,lY) != CRegion::BLOCK_AIM &&
        pRegion->GetBlock(lX,lY) != CRegion::BLOCK_SHAPE))
    {
        lDir = GetLineDir(lTileX,lTileY,lDestX,lDestY);
        pRegion->GetLeastShapesCellFromCircularSpace(lDestX,lDestY,0,0,1,lDir,lX,lY);
        bBlockedToAdjustDestPos = true;
    }
    //检查源点坐标和目标点坐标的直线上是否有阻挡,如果有阻挡则设置目标点为 源点到阻挡点最近的坐标

    list<POINT> listPos;
    long lDistance = Distance(lTileX, lTileY, lDestX, lDestY);
    GetLinePointToDest(listPos, lTileX, lTileY, lDestX, lDestY,lDistance);
   
    list<POINT>::iterator	listIter = listPos.begin();
    BYTE bBlock = 0;
    POINT PreViousPos;  //上一个移动点的位置
    PreViousPos.x = lTileX;
    PreViousPos.y = lTileY;
    for(;listIter != listPos.end(); ++listIter)
    {
        POINT& pos = *listIter;
        bBlock = pRegion->GetBlock(pos.x, pos.y);
        if( !(bBlock == CRegion::BLOCK_AIM ||
            bBlock == CRegion::BLOCK_NO) )
        {
            //存在阻挡，在此阻挡格子周围找一合法格子作为目的点坐标
            lDir = GetLineDir(lTileX,lTileY,pos.x,pos.y);
            pRegion->GetLeastShapesCellFromCircularSpace(pos.x,pos.y,0,0,1,lDir,lX,lY);
            bBlockedToAdjustDestPos = true;
            break;
        }
        else
        {
            PreViousPos.x = pos.x;
            PreViousPos.y = pos.y;
        }
    }
    if (bBlockedToAdjustDestPos)
    {
        m_fDestX = PreViousPos.x + 0.5f;
        m_fDestY = PreViousPos.y + 0.5f;
    }
    else
    {
        m_fDestX = fDestX;
        m_fDestY = fDestY;
    }

    if (abs(fPosX - m_fDestX) <= m_FloatMove->MINIMUS_OP_DISTANCE &&
        abs(fPosY - m_fDestY) <= m_FloatMove->MINIMUS_OP_DISTANCE)
    {
        return;
    }

    pMainPlayer->SetIsArrive(false);
    m_FloatMove->LineMoveList.clear();
    dwBegionMoveTime = timeGetTime();
    m_dwMoveMsgCounter = 0;

    //AutoMoveTo(fDestX,fDestY);
    m_FloatMove->UpdatelinearMoveList(fPosX,fPosY,m_fDestX,m_fDestY);
    if (!m_FloatMove->LineMoveList.empty())
    {
        float fTempX = m_FloatMove->LineMoveList.begin()->x;
        float fTempY = m_FloatMove->LineMoveList.begin()->y;
        MoveToCell(fTempX,fTempY);
        m_FloatMove->LineMoveList.pop_front();
    }
    else
        MoveToCell(m_fDestX,m_fDestY);
    // 清空之前走过的格子记录
    m_MovedCell.clear();
    // 清空A*寻路路径
    CRegionList::GetRegionPathList().clear();
    m_lTargetRegionID = 0;
    if(m_AutoMoveMode == AMM_A_SEARCH)
    {
        // 自动移动图标闪动
        //CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
        //if(pMainPage)
        //	pMainPage->MovePicFlick(true);
    }	
    //移动行为模式
    if(GetMPActionMode() != MPAM_AutoMove)
        SetMPNexActionMode(MPAM_AutoMove);
    else//重置移动模式
        SetResetAutoMoveMode(true);
    m_AutoMoveMode = AMM_TO_DESTDIR;
    SetFollowingObj(0,CGUID::GUID_INVALID);
}

// 目标采集物的GUID
//void CGameControl::AutoCollect(const CGUID& guCollectID)
//{
//	m_CollectGoodsID = guCollectID;
//}

// 自动拾取物品
void CGameControl::AutoPickGood(const CGUID& guGoodID)
{
    //移动行为模式
    if(GetMPActionMode() == MPAM_AutoMove)
        AutoMoveStop();
    else if(GetMPActionMode() == MPAM_AutoAttack && !( GetAutoAttackMode()&AAM_STOP) )
        AutoAttactStop();
    //移动行为模式
    if(GetMPActionMode() != MPAM_AutoPickGoods)
        SetMPNexActionMode(MPAM_AutoPickGoods);
    m_PickGoodsID = guGoodID;
    return;
}

// 跟随一个目标对象
void CGameControl::FollowObject(long lType,const CGUID& guid,long lMaxDistance)
{
    // 不能移动
    if(!IsCanMoveCondition())
        return;
    CShape* pShape = dynamic_cast<CShape*>(pRegion->FindChildObject(lType, guid));
    if (NULL == pShape)	return;

    SetFollowingObj(lType,guid);
    m_lFollowMaxDistance = lMaxDistance;
    //移动行为模式
    if(GetMPActionMode() != MPAM_AutoMove)
        SetMPNexActionMode(MPAM_AutoMove);
    else//重置移动模式
        SetResetAutoMoveMode(true);
    m_AutoMoveMode = AMM_FOLLOW_OBJECT;


    long lDestX = pMainPlayer->GetTileX();
    long lDestY = pMainPlayer->GetTileY();
    long lDir = GetLineDir(pMainPlayer->GetTileX(),pMainPlayer->GetTileY()
        ,pShape->GetTileX(),pShape->GetTileY());
    pRegion->GetLeastShapesCellFromSpace(pShape->GetTileX(),pShape->GetTileY(),
        0,0,lMaxDistance,lDir,lDestX,lDestY);

    m_fDestX = lDestX + 0.5f;
    m_fDestY = lDestY + 0.5f;
    m_fStartDestX = pShape->GetPosX();
    m_fStartDestY = pShape->GetPosY();
}

// 开始攻击并设置攻击模式
void CGameControl::StartAttack(CShape* pShape)
{
    //if(pShape != NULL)
    //{
    SetCurAttractShape(pShape);
    long x=0,y=0;
    string str="";
    if(pShape)
    {
        x = pShape->GetTileX();
        y = pShape->GetTileY();
        str = pShape->GetName();
    }
    //移动测试代码,wq,2008-2-20
    DWORD dwCurTime = GetCurTickCount();
    //char strInfo[512]="";
    //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_11),
    //	dwCurTime,str.c_str(),pMainPlayer->GetTileX(), pMainPlayer->GetTileY(),x,y);
    //PutStringToFile("MoveTest",strInfo);

    if(GetMPActionMode() != MPAM_AutoAttack)
    {
        SetMPNexActionMode(MPAM_AutoAttack);
        m_AutoAttactMode = AAM_ATTACT;
    }
    //}

}

// 过程中间被中断后继续自动行走
long CGameControl::ContinueMoveTo(float fDestX, float fDestY)
{
    //所有路径点都走完了，目标点还是有阻挡，移动停止
    //if(pRegion->GetBlock(lDestX, lDestY) && GetFindPath()->GetPathPosList().empty())
    //	return 0;

    const long lDestX = (long)fDestX;
    const long lDestY = (long)fDestY;

    // 寻路
    GetFindPath()->AddPathPos(pRegion->GetResourceID(),pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), lDestX, lDestY);
    // 去掉第一个点,因为就是自己站的点
    if(GetFindPath()->GetPathList()->size() > 0)
    {
        GetFindPath()->GetPathList()->pop_front();
        float fPosX = pMainPlayer->GetPosX();
        float fPosY = pMainPlayer->GetPosY();
        m_FloatMove->UpdateA_MoveList(GetFindPath(),fPosX,fPosY,fDestX,fDestY);
    }
    if(GetFindPath()->GetPathList()->size() == 0)
    {
        m_FloatMove->A_MoveList.clear();
        return 0;
    }
    return 1;
}

//===========================08.5.8liuke=============================
// 跨场景后的继续寻路
void CGameControl::ContinueMoveToOtherRegion()
{
    pMainPlayer = GetGame()->GetMainPlayer();
    pRegion = GetGame()->GetRegion();
    if(!pRegion || !pMainPlayer)
        return;
    // 已经到达最后一个目标场景
    if ( GetTargetRegionID() != 0 && GetTargetRegionID() == pMainPlayer->GetRegionID() )
    {
        if(m_lTPosX != 0 && m_lTPosY != 0)
        {
            //移动行为模式
            if(GetMPActionMode() != MPAM_AutoMove)
                SetMPNexActionMode(MPAM_AutoMove);
            m_AutoMoveMode= AMM_A_SEARCH;
            m_fDestX = (float)m_lTPosX;
            m_fDestY = (float)m_lTPosY;
            GetFindPath()->AddPathPos(pRegion->GetResourceID(),pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), m_lTPosX, m_lTPosY);
            CRegionList::GetRegionPathList().clear();
        }
    }
    // 场景节点列表存在
    else if (!CRegionList::GetRegionPathList().empty())
    {
        //移动行为模式
        if(GetMPActionMode() != MPAM_AutoMove)
            SetMPNexActionMode(MPAM_AutoMove);
        m_AutoMoveMode= AMM_A_SEARCH;
        // 场景错乱，停止移动
        CRegionList::tagRegionPathInfo stPathInfo = CRegionList::GetRegionPathList().front();
        if(stPathInfo.lRegionID != pMainPlayer->GetRegionID())
        {
            SetMPActionMode(MPAM_AutoMove);
            AutoMoveStop();
            SetMPNexActionMode(MPAM_No);
            // 设置在大地图上显示的目标点标示
            CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
            if(pMiniMap)
                pMiniMap->SetDestPos();
            return;
        }
        // 取出第一个场景节点的目地坐标信息
        else
        {
            CRegionList::GetRegionPathList().erase(CRegionList::GetRegionPathList().begin());
            if (!CRegionList::GetRegionPathList().empty())
            {
                // 寻路
                CRegionList::tagRegionPathInfo stPathInfo = CRegionList::GetRegionPathList().front();
                m_fDestX = (float)stPathInfo.stPos.x;
                m_fDestY = (float)stPathInfo.stPos.y;
                GetFindPath()->AddPathPos(pRegion->GetResourceID(),pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), (long)m_fDestX, (long)m_fDestY);
            }
            else
            {
                SetMPActionMode(MPAM_AutoMove);
                AutoMoveStop();
                SetMPNexActionMode(MPAM_No);
                CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
                if(pMiniMap)
                    pMiniMap->SetDestPos();
                return;
            }
        }
    }
    //// 已经到达最后一个目标场景
    //else if ( GetTargetRegionID() != 0 && GetTargetRegionID() == pMainPlayer->GetRegionID() )
    //{
    //	if(m_lTPosX != 0 && m_lTPosY != 0)
    //	{
    //		//移动行为模式
    //		if(GetMPActionMode() != MPAM_AutoMove)
    //			SetMPNexActionMode(MPAM_AutoMove);
    //		m_AutoMoveMode= AMM_A_SEARCH;
    //		m_fDestX = m_lTPosX;
    //		m_fDestY = m_lTPosY;
    //		GetFindPath()->AddPathPos(pRegion->GetResourceID(),pMainPlayer->GetTileX(), pMainPlayer->GetTileY(), m_fDestX, m_fDestY);
    //	}
    //}
    else
        return;
    // 设置在大地图上显示的目标点标示
    CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
    if(pMiniMap)
    {
        pMiniMap->SetDestPos((int)m_fDestX,(int)m_fDestY);
    }
}
//===================================================================

void CGameControl::AutoMoveStop()
{
    if(m_MainPlayerActionMode == MPAM_AutoMove)
    {
        m_AutoMoveMode = m_AutoMoveMode|AMM_STOP;
        CRegionList::GetRegionPathList().clear();
        m_lTargetRegionID = 0;

        DWORD dwCurTime = GetCurTickCount();
        // 取消在大地图上显示的目标点标示
        /*CMiniMap *pMiniMap = GetGame()->GetRegion()->GetMiniMap();
        if(pMiniMap)
        pMiniMap->SetDestPos();*/
        //CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
        //if(pMainPage)
        //	pMainPage->MovePicFlick(true);
        //char strInfo[512]="";
        //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_12),
        //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
        //PutStringToFile("MoveTest",strInfo);
    }
}

// 终止自动攻击
void CGameControl::AutoAttactStop()
{
    if(m_MainPlayerActionMode == MPAM_AutoAttack)
    {
        m_AutoAttactMode = m_AutoAttactMode|AAM_STOP;
        SetAutoSkillID(0);
        SetBufferSkillID(0);

        //DWORD dwCurTime = GetCurTickCount();
        //char strInfo[512]="";
        //_snprintf(strInfo,512,CStringReading::LoadText(eIDS_CONTROL,eIDS_MOVETEST_13),
        //	dwCurTime,pMainPlayer->GetTileX(), pMainPlayer->GetTileY());
        //PutStringToFile("MoveTest",strInfo);
    }
}

// 终止自动拾取物品
void CGameControl::AutoPickGoodStop()
{
    if(m_MainPlayerActionMode == MPAM_AutoPickGoods)
    {
        m_PickGoodsID = NULL_GUID;
    }
}

// 处理A*寻路的自动移动AI
long CGameControl::AutoMoveAI()
{
    // 已经到目标点了
    const long lX = pMainPlayer->GetTileX();
    const long lY = pMainPlayer->GetTileY();
    const long lDestX = (long)pMainPlayer->GetDestX();
    const long lDestY = (long)pMainPlayer->GetDestY();
    const float fPosX = pMainPlayer->GetPosX();
    const float fPosy = pMainPlayer->GetPosY();

    // 寻路的目标场景等于当前玩家所在场景,或目标场景为0（表示不存在跨场景移动），且已到达目标点
    long lCurRegion = GetTargetRegionID();
    if (abs(fPosX-m_fDestX) <= 2.f && abs(fPosy-m_fDestY) <= 2.f)
    {
        // 记录自动移动的信息
        //CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
        //if(pMainPage)
        //{
        //	CMainBarPageEx::stMoveInfo stInfo = pMainPage->GetMoveInfo();
        //	if(stInfo.lTarRegionID == pMainPlayer->GetRegionID())
        //		pMainPage->SetMoveInfo(0,0,0);
        //}
        // 目标点有monster
        CMonster* pMonster = dynamic_cast<CMonster*>(pRegion->GetShape((long)m_fDestX, (long)m_fDestY));
        if (pMonster)
        {
            // 是npc，自动点击对话
            if(pMonster->IsCanTalk())
            {
                SendTouchNpcMes(pMonster->GetExID());
            }
            return 0;
        }
        else
        {
            // 目标点有物品，拾取
            CGoods* pGoods = dynamic_cast<CGoods*>(pRegion->GetShape((long)m_fDestX, (long)m_fDestY));
            if (pGoods)
            {
                PickupGoods(pGoods);
                return 0;
            }

        }
    }
    //更新当前转向移动点表
    CFloatMove::tagMovPos CurDestPoint;

    if (pMainPlayer->IsArrive())
    {
        //到达后先处理直线方向上的拆分点。
        if(!m_FloatMove->LineMoveList.empty())
        {
            CurDestPoint.x = m_FloatMove->LineMoveList.begin()->x;
            CurDestPoint.y = m_FloatMove->LineMoveList.begin()->y;
            
            //sprintf_s(strMoveInfo,256,"直线拆分表中点X:%f Y:%f\n",CurDestPoint.x,CurDestPoint.y);
            //OutputDebugString(strMoveInfo);
            //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

            MoveToCell(CurDestPoint.x,CurDestPoint.y);    
            dwBegionMoveTime = timeGetTime();
            m_FloatMove->LineMoveList.pop_front();
            m_FloatMove->bIsPreviousMoveByA_Star = false;
        }
        else
        {
            if (m_FloatMove->bIsPreviousMoveByA_Star)
                m_FloatMove->A_MoveList.pop_front();
            DWORD elaspseTime = timeGetTime() - dwBegionMoveTime;
            if(!m_FloatMove->A_MoveList.empty())
            {
                CurDestPoint.x = m_FloatMove->A_MoveList.begin()->x;
                CurDestPoint.y = m_FloatMove->A_MoveList.begin()->y;
                m_FloatMove->bIsPreviousMoveByA_Star = true;
                m_FloatMove->UpdatelinearMoveList(fPosX,fPosy,CurDestPoint.x,CurDestPoint.y);

                //if (m_dwMoveMsgCounter == 2)
                //    sprintf_s(strMoveInfo,256,"从开始移动到第%d个转向点移动花费时间:%f秒\n",m_dwMoveMsgCounter-1,elaspseTime/1000.f);
                //else if(m_dwMoveMsgCounter>2)
                //    sprintf_s(strMoveInfo,256,"第%d个转向点到第%d个转向点移动花费时间:%f秒\n",
                //    m_dwMoveMsgCounter-1-1,m_dwMoveMsgCounter-1,elaspseTime/1000.f);
                if (!m_FloatMove->LineMoveList.empty())
                {
                    CurDestPoint.x = m_FloatMove->LineMoveList.begin()->x;
                    CurDestPoint.y = m_FloatMove->LineMoveList.begin()->y;
                    m_FloatMove->LineMoveList.pop_front();
                    m_FloatMove->bIsPreviousMoveByA_Star = false;
                    //sprintf_s(strMoveInfo,256,"直线拆分表中点X:%f Y:%f\n",CurDestPoint.x,CurDestPoint.y);
                    
                }
                MoveToCell(CurDestPoint.x,CurDestPoint.y);    
                dwBegionMoveTime = timeGetTime();

                //OutputDebugString(strMoveInfo);
                //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);

                //if(m_FloatMove->A_MoveList.size() > 1)
                //    sprintf_s(strMoveInfo,256,"第%d个转向点 X:%f Y:%f \n", m_dwMoveMsgCounter,CurDestPoint.x,CurDestPoint.y);
                //else
                //    sprintf_s(strMoveInfo,256,"目的地点 X:%f Y:%f \n", CurDestPoint.x,CurDestPoint.y);
                //OutputDebugString(strMoveInfo);
                //PutStrToFileSaveToDir("log","MoveDebug",strMoveInfo);
            }
            else
            {
                //到达目标 停止移动
                return 0;
            }
        }
    }
    return 1;

    // 按路径走
    //if (GetFindPath()->GetPathList()->size())
    //{
    //	// 计算第一步方向
    //	list<CFindPath::stPos>::iterator it = GetFindPath()->GetPathList()->begin();
    //	long lDX = (*it).x;
    //	long lDY = (*it).y;

    //	// 阻断了，重新寻路
    //	if (pRegion->GetBlock(lDX, lDY) != CRegion::BLOCK_NO 
    //		&& pRegion->GetBlock(lDX, lDY) != CRegion::BLOCK_AIM)
    //	{
    //		//如果就是目标格子，停止移动
    //		ContinueMoveTo(m_fDestX, m_fDestY);
    //		return 1;
    //	}

    //	int dir = GetLineDir(lX, lY, lDX, lDY);
    //       int dir2 = dir;
    //	GetFindPath()->GetPathList()->pop_front();
    //	
    //	long lDX2 = lDX;
    //	long lDY2 = lDY;
    //	// 计算第二步方向
    //	if (GetFindPath()->GetPathList()->size())
    //	{
    //		list<CFindPath::stPos>::iterator it2 = GetFindPath()->GetPathList()->begin();
    //		lDX2 = (*it2).x;
    //		lDY2 = (*it2).y;
    //		dir2 = GetLineDir(lDX, lDY, lDX2, lDY2);

    //		// 方向相同
    //		if (dir == dir2)
    //		{
    //			// 阻断了，重新寻路
    //			if (pRegion->GetBlock(lDX2, lDY2) != CRegion::BLOCK_NO 
    //				&& pRegion->GetBlock(lDX2, lDY2) != CRegion::BLOCK_AIM)
    //			{				
    //				ContinueMoveTo(m_fDestX, m_fDestY);
    //				return 1;
    //			}
    //			GetFindPath()->GetPathList()->pop_front();
    //		}
    //	}

    //	if(dir == dir2)
    //		MoveToCell(lDX2+0.5f,lDY2+0.5f);
    //	else
    //		MoveToCell(lDX+0.5f,lDY+0.5f);
    //	pMainPlayer->m_lStopCount = 0;
    //}
    //else	// 路径走完了,还未到达目标,继续寻路
    //{
    //	// 如果目标点是不可行走的，停止移动和寻路
    //	if(ContinueMoveTo(m_fDestX, m_fDestY) == 0)
    //	{
    //		// 记录自动移动的信息
    //		//CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
    //		//if(pMainPage)
    //		//	pMainPage->SetMoveInfo(0,0,0);
    //		return 0;
    //	}
    //}
    return 1;
}



CFindPath* CGameControl::GetFindPath()
{
    return pRegion->GetFindPath();
}

void CGameControl::SetFollowingObj(long lType,const CGUID& guid)
{
    m_lFollowingObjType = lType;
    m_FollowingObjID = guid;
    m_fStartDestX = 0;
    m_fStartDestY = 0;
}

// 取消选中的目标
void CGameControl::CancelSelectedTarget()
{
    SetSelectedShape(m_pSelectedShape);
}

// 设置鼠标当前选择的对象
void CGameControl::SetSelectedShape(CShape* pShape,bool bIsCToS)
{
    //如果是GM隐身状态，则不选中
    if(pShape && pShape->GetType() == TYPE_PLAYER && ((CPlayer*)pShape)->GetIsGMConcealment() )
        return;
    //如果是陷阱或者饰宠，则不选中
    if(pShape && pShape->GetType() == TYPE_PET)
    {
        CPet *pPet = (CPet*)pShape;
        //if (pPet->GetPetType() == PET_TYPE_TRAP || pPet->GetPetType() == PET_TYPE_ORNA)
        //	return;
    }

    // 与之前选中对象相同，直接返回
    if(m_pSelectedShape == pShape)
        return;
    m_pSelectedShape = pShape;
    if(pShape)
    {
        m_lSelectedType = pShape->GetType();
        m_guidSelected = pShape->GetExID();
    }
    else
    {
        m_lSelectedType = 0;
        m_guidSelected = CGUID::GUID_INVALID;
        ClearSelectedList();
    }
    if(bIsCToS)
    {
        if(m_dwSelectedShapeStartTime == 0)
            m_dwSelectedShapeStartTime = timeGetTime();
    }
    else
        m_dwSelectedShapeStartTime = 0;

}

bool CGameControl::GetIntonateBreak()
{
    bool bBreak = m_bIntonateBreak;
    m_bIntonateBreak = false;
    return bBreak;
}

// 查找指定场景下的指定位置到目标场景的指定位置的路径
bool CGameControl::FindPathToOtherRegion(long lSourRegion, long lSourx, long lSoury, long lTargRegion, long lTargx, long lTargy)
{
    // 相同场景，立即返回
    if(lSourRegion == lTargRegion)
        return false;
    m_lTargetRegionID = lTargRegion;
    if(!CRegionList::GetRegionPathList().empty())
    {
        m_lSPosX = lSourx;
        m_lSPosY = lSoury;
        m_lTPosX = lTargx;
        m_lTPosY = lTargy;
        // 获取第一个场景节点信息
        CRegionList::tagRegionPathInfo stPathInfo = CRegionList::GetRegionPathList().front();
        // 寻找要到达该场景，所要经过的路径点
        AutoMoveTo((float)stPathInfo.stPos.x,(float)stPathInfo.stPos.y);
        //CRegionList::GetRegionPathList().erase(CRegionList::GetRegionPathList().begin());
        // 记录自动移动的信息
        //CMainBarPageEx * pMainPage = GetGame()->GetCGuiEx()->GetMainBarPageEx();
        //if(pMainPage)
        //	pMainPage->SetMoveInfo(lTargRegion,lTargx,lTargy);
        return true;
    }
    return false;
}

//发送消息改变移动模式
void CGameControl::ChangeMoveMode()
{
    CMessage msg(MSG_C2S_SHAPE_CHANGEMOVEMODE);
    msg.Add(static_cast<char>(!pMainPlayer->GetIsRun()));
    msg.Send();		
}

// 设置自动释放技能的ID
void CGameControl::SetAutoSkillID(DWORD dw)
{
    CPlayer *pPlayer = GetGame()->GetMainPlayer();
    if(pPlayer == NULL || m_dwAutoSkillID == dw)
        return;
    CPlayer::tagSkill *ptagS = NULL;
    // 之前的自动施放技能快捷栏上光圈绘制的开始时间设置为0，表示不绘制
    if(m_dwAutoSkillID > 0)
    {
        ptagS = pPlayer->GetSkill(m_dwAutoSkillID);
        if(ptagS)
            ptagS->dwUseingStartTime = 0;
    }
    m_dwAutoSkillID = dw;
    // 设置当前的自动施放技能快捷栏上光圈绘制的开始时间
    if(m_dwAutoSkillID > 0)
    {
        ptagS = pPlayer->GetSkill(m_dwAutoSkillID);
        if(ptagS)
            ptagS->dwUseingStartTime = timeGetTime();
    }
}
// 设置缓冲技能ID
void CGameControl::SetBufferSkillID(DWORD dw)
{
    CPlayer *pPlayer = GetGame()->GetMainPlayer();
    if(pPlayer == NULL || m_dwBufferSkillID == dw)
        return;
    CPlayer::tagSkill *ptagS = NULL;
    // 之前的自动施放技能快捷栏上光圈绘制的开始时间设置为0，表示不绘制
    if(m_dwBufferSkillID > 0 && m_dwBufferSkillID != m_dwAutoSkillID)
    {
        if(m_dwBufferSkillID != m_dwAutoSkillID && dw == SKILL_BASE_BUFF)
            return;
        ptagS = pPlayer->GetSkill(m_dwBufferSkillID);
        if(ptagS)
            ptagS->dwUseingStartTime = 0;
    }
    m_dwBufferSkillID = dw;
}

// 自动拾取玩家外圈的物品（需要自动移动到物品处拾取）
void CGameControl::AutoPickGoodEx()
{
    if(pMainPlayer)
    {
        // 玩家为中心点的第二层坐标
        static int PickOrder[16][2] = 
        {
            // 从左边开始，顺时针排列
            -2,0,	-2,-1,	-2,-2,
            -1,-2,	0,-2,	1,-2,	2,-2,
            2,-1,	2,0,	2,1,	2,2,
            1,2,	0,2,	-1,2,	-2,2,
            -2,1
        };
        for (int i=0; i<16; i++)
        {
            CGoods *pGoods = pRegion->GetShapeGoods(pMainPlayer->GetTileX() + PickOrder[i][0], pMainPlayer->GetTileY() + PickOrder[i][1]);
            if (pGoods)
            {
                // 跑过去,拾取在跑到物品处后触发
                //FollowObject(pGoods->GetType(),pGoods->GetExID(),1);
                AutoPickGood(pGoods->GetExID());
                return;
            }
        }
    }	
}

// 设置和获得是否锁定队友选择
void CGameControl::SetIsTabTeamMate(bool bLock)
{
    //CMainBarPageEx * pMainbar = GetGame()->GetCGuiEx()->GetMainBarPageEx();
    //if(pMainbar)
    //{
    //	// 从不锁定队友切换到锁定队友，全部设置失效
    //	if(m_bIsTabTeamMate != bLock && bLock)
    //	{
    //		pMainbar->SetTabItem(CMainBarPageEx::eTab_Enemy,false);
    //		pMainbar->SetTabItem(CMainBarPageEx::eTab_Pirate,false);
    //		pMainbar->SetTabItem(CMainBarPageEx::eTab_Monster,false);
    //	}
    //	m_bIsTabTeamMate = bLock;
    //	// 根据标记改变Tab按钮描述信息
    //	pMainbar->SetTabText(m_bIsTabTeamMate);
    //}
}


//定时pingServer功能
void CGameControl::PingServer()
{
    if(!pRegion || !pMainPlayer)	return;

    DWORD dwCurTime = timeGetTime();
    if(dwCurTime - m_dwLastPingTime > 60*1000*10)
    {
        m_dwLastPingTime = dwCurTime;
        DWORD dwCRC32 = 0;
        //CCrc32Static::DataCrc32(&m_dwLocalIP,sizeof(DWORD),dwCRC32Value);
        CGUID guid = pRegion->GetExID();
        CCrc32Static::DataCrc32(&guid,sizeof(CGUID),dwCRC32);
        DWORD dwValue = pMainPlayer->GetHotKey(0);
        CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCRC32);
        dwValue = pMainPlayer->GetHotKey(12);
        CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCRC32);

        CMessage msg(MSG_C2S_OTHER_PING);		
        msg.Add(dwCRC32);
        msg.Send();
    }
}



//浮点移动功能----------------------------------
CFloatMove::CFloatMove():bIsBeelineMove(true),bIsPreviousMoveByA_Star(false),
MINIMUS_OP_DISTANCE(CGlobeSetup::GetSetup()->fminimumOpDistance),
MAX_LINE_DISTANCE(CGlobeSetup::GetSetup()->fLineMaxDistanceAllowToMove)
{
    
}

CFloatMove::~CFloatMove()
{

}

//直线移动时超过最大步长的拆分处理
void CFloatMove::UpdatelinearMoveList(const float fPosX,const float fPosY,
                          const float fDestX, const float fDestY)
{
    if (fDestX == 0 && fDestY == 0)
        return;
    //清除上一次路径点
    LineMoveList.clear();
    float lDistance = Distance(fPosX,fPosY,fDestX,fDestY);
    if(lDistance < MAX_LINE_DISTANCE)
        return;
    const float sinLine = (fDestY-fPosY) / lDistance;
    const float cosLine = (fDestX-fPosX) / lDistance;
    int i = 1;
    tagMovPos PrePos;
    PrePos.x = fPosX;
    PrePos.y = fPosY;
    while(lDistance >= MAX_LINE_DISTANCE )
    {
        tagMovPos newPos;
        newPos.x = fPosX + i* (MAX_LINE_DISTANCE-0.5f) * cosLine;
        newPos.y = fPosY + i* (MAX_LINE_DISTANCE-0.5f) * sinLine;
        lDistance = Distance(PrePos.x,PrePos.y,newPos.x,newPos.y);
        PrePos.x = newPos.x;
        PrePos.y = newPos.y;
        LineMoveList.push_back(newPos);
        
        i++;
        lDistance = Distance(newPos.x,newPos.y,fDestX,fDestY);
    }
}

//转换A*点路劲为 转向线段路径
void CFloatMove::UpdateA_MoveList(CFindPath *pFindPath,const float fPosX,
                                  const float fPosY,const float fDestX,
                                  const float fDestY)
{
    A_MoveList.clear();
    list<CFindPath::stPos> *pPathlist = pFindPath->GetPathList();
    if(pPathlist->empty())
    {
        if(abs(fPosX - fDestX) < MINIMUS_OP_DISTANCE &&
            abs(fPosY - fDestY) < MINIMUS_OP_DISTANCE)
            return;
        else
        {
            //一个格子内的移动
            if (abs(fPosX - fDestX) >= MINIMUS_OP_DISTANCE ||
                abs(fPosY - fDestY) >= MINIMUS_OP_DISTANCE)
            {
                tagMovPos movePos;
                movePos.x = fDestX;
                movePos.y = fDestY;
                A_MoveList.push_back(movePos);
                bIsBeelineMove = true;
            }
            return;
        }
    }

    CFindPath::stPos PreviousPos,CurPos;
    POINT FirstPos;
    long lTileX = (long)fPosX;
    long lTileY = (long)fPosY;

    list<CFindPath::stPos>::iterator pathIter = pPathlist->begin();
    FirstPos.x = pathIter->x;
    FirstPos.y = pathIter->y;

    //起始位置玩家当前站立点
    PreviousPos.x = lTileX;
    PreviousPos.y = lTileY;
    //初始方向
    long PreviousDir = GetLineDir(lTileX, lTileY, FirstPos.x, FirstPos.y);

    for (long i = 0;pathIter != pPathlist->end();++pathIter,i++)
    {
        CurPos.x = pathIter->x;
        CurPos.y = pathIter->y;
        //如果更上一个移动格子方向不一致 则记录转向点
        long CurDir = GetLineDir(PreviousPos.x, PreviousPos.y, CurPos.x, CurPos.y);
        tagMovPos movePos;
        if (PreviousDir != CurDir)
        {    
            movePos.x = PreviousPos.x + 0.5f;
            movePos.y = PreviousPos.y + 0.5f;
            A_MoveList.push_back(movePos);//这里记录每一个拐点
            PreviousDir = CurDir; 
        }
        if(i == pPathlist->size()-1)//最后目标点
        {
            movePos.x = fDestX;
            movePos.y = fDestY;
            A_MoveList.push_back(movePos);
        }
        PreviousPos.x = CurPos.x;
        PreviousPos.y = CurPos.y;
    }
    if (A_MoveList.size()>1)
        bIsBeelineMove = false;
    else
        bIsBeelineMove = true;
}