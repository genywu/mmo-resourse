#include "stdafx.h"
#include "../../GamePage.h"
#include "PackageEvent.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/Other/PicList.h"
#include "../../../GameClient/ClientApp/MainPlayerHand.h"
#include "../Package/PackagePage.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../../Public/Common/MsgType.h"
#include "../../../../Public/Common/DepotDef.h"
#include "../../../GameClient/ClientApp/Goods/Goods.h"
#include "../../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../../GameClient/ClientApp/TradeSystem/PlayerTrade.h"
#include "../../UIDef.h"
#include "../ChatList/ChatListPage.h"
#include "../../../GameClient/GameControl.h"
#include "../../../GameClient/ClientApp/Mail/Mail.h"
using namespace mail;
#include "../../../GameClient/ClientApp/Mail/MailDef.h"
#include "../../../GameClient/ClientApp/GameDesp/gamedespparser.h"

extern CEGUI::Window* GetWindow(const CEGUI::String& name);

namespace PackageMsgBox
{
    struct tagChangeEquipMSG 
    {
        int nIndex;         //目标栏位
        int nSPos;          //源栏位
        int nExtendID;      //目标容器ID
        int nSExtendID;     //来源容器ID
    };
}

void  ChangeEquipMsgBox(MsgEvent& event,eMsgResult UserId)
{
    PackageMsgBox::tagChangeEquipMSG *pEquipMSG = 
        (PackageMsgBox::tagChangeEquipMSG*)event.pData;
    GetInst(PackagePage).GetPackageEvent()->SendChangeEquipMSG(
        pEquipMSG->nIndex,pEquipMSG->nSPos,pEquipMSG->nExtendID,pEquipMSG->nSExtendID);
    SAFE_DELETE(pEquipMSG);
}

PackageEvent::PackageEvent(EVENTTYPE type)
{
    SetEventType(type);
}

PackageEvent::~PackageEvent()
{

}

void PackageEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    CEGUI::FrameWindow *pPageWindow = static_cast<CEGUI::FrameWindow*>(pPage->GetPageWindow());
    CEGUI::DefaultWindow *pPageBackWindow = static_cast<CEGUI::DefaultWindow*>(pPageWindow->getChild("Package/backgrond"));
	CEGUI::PushButton* pOpenPlayerShopBtn = static_cast<CEGUI::PushButton*>(pPageBackWindow->getChild("Package/backgrond/OpenPlayerShop"));
   
    //更新背包物品
    for(int i=0;i<GRIDITEM;i++)
    {
        SetUpdatePos(i);
        OnPageUpdate(pPage);
        CEGUI::GUISheet *pContainer = static_cast<CEGUI::GUISheet*>(pPageBackWindow->getChildAtIdx(i));
        CEGUI::DragContainer *pDrageWin = static_cast<CEGUI::DragContainer*>(pContainer->getChildAtIdx(0));
        try
        {
            // subscribe the handler.
            pDrageWin->subscribeEvent(
                CEGUI::Window::EventDragDropItemDropped,
                CEGUI::Event::Subscriber(&PackageEvent::handle_GoodsDropped, this));
            pDrageWin->subscribeEvent(CEGUI::DragContainer::EventDragStarted,
                CEGUI::Event::Subscriber(&PackageEvent::handle_GoodsPickUp, this));

            pDrageWin->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
                CEGUI::Event::Subscriber(&PackageEvent::ProcessMouseRBtnDown,this));

            pDrageWin->subscribeEvent(CEGUI::Window::EventMouseEnters,
                CEGUI::Event::Subscriber(&PackageEvent::OnMouseEnter,this));

            pDrageWin->subscribeEvent(CEGUI::Window::EventMouseLeaves,
                CEGUI::Event::Subscriber(&PackageEvent::OnMouseLeave,this));
            
        }
        // if something goes wrong, log the issue but do not bomb!
        catch(CEGUI::Exception&)
        {}
    }
    
    pPageWindow->subscribeEvent(
        CEGUI::FrameWindow::EventCloseClicked,
        CEGUI::Event::Subscriber(&PackageEvent::handle_PageClose,this));

	pOpenPlayerShopBtn->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&PackageEvent::handle_OpenPlayerShop,this));

    pPageWindow->subscribeEvent("UpdateMoney",CEGUI::Event::Subscriber(&PackageEvent::updateMoney,this));
    
}

void PackageEvent::OnPageOpen(GamePage *pPage)
{
    CEGUI::FrameWindow *pPageWindow = static_cast<CEGUI::FrameWindow*>(pPage->GetPageWindow());
    CEGUI::DefaultWindow *pPageBackWindow = static_cast<CEGUI::DefaultWindow*>(pPageWindow->getChild("Package/backgrond"));
    const CEGUI::Image &pBackImage = SetBackGroundImage(pPageBackWindow,"Package","datafiles/imagesets/Package","backpack.tga");
    FireUIEvent("PackagePage","UpdateMoney");
}

void PackageEvent::OnPageClose(GamePage *pPage)
{

}

void PackageEvent::OnPageUpdate(GamePage *pPage)
{
    CEGUI::FrameWindow *pPageWindow = static_cast<CEGUI::FrameWindow*>(pPage->GetPageWindow());
    CEGUI::DefaultWindow *pPageBackWindow = static_cast<CEGUI::DefaultWindow*>(pPageWindow->getChild("Package/backgrond"));
    //找到需要更新的DrageContainer控件
    CEGUI::GUISheet *pContainer = static_cast<CEGUI::GUISheet*>(pPageBackWindow->getChildAtIdx(m_UpdateGoodsPos));
    CEGUI::DragContainer *pCurDrag = static_cast<CEGUI::DragContainer*>(pContainer->getChildAtIdx(0));
    CEGUI::GUISheet *pImageComp = static_cast<CEGUI::GUISheet*>(pCurDrag->getChildAtIdx(0));
    
    pContainer->setProperty("FrameEnabled", "false");
    pContainer->setProperty("BackgroundEnabled", "false");
    pImageComp->setProperty("FrameEnabled", "false");
    pImageComp->setProperty("BackgroundEnabled", "false");
    pImageComp->setProperty("MousePassThroughEnabled","true");
    pImageComp->setProperty("Text","");
    
    CPlayer *pPlayer = GetGame()->GetMainPlayer();
    CPlayer::tagGoods *pPlayerGoods = pPlayer->GetGoodsOnPlayer(PEI_PACKET,(WORD)m_UpdateGoodsPos);
    
    DWORD dwIconId = 0;
    CGoods *pGoods = NULL;
    char strImageFilePath[128] = "";
    char strImageFileName[128] = "";
    if(pPlayerGoods)
    {
        pGoods = pPlayerGoods->pGoods;
        assert(pGoods->GetProperty()!=NULL);
        if (pGoods->GetProperty()!=NULL)
        {
            dwIconId = pGoods->GetProperty()->dwIconId;
        }
        if(dwIconId != 0)
        {
            // 获得当前背包栏对应的物品图标数据，并将该图标设置成对应背包组件的额外图片。
            const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON, dwIconId);
            
            if(strIconPath)
            {
                string strQuality = "";
                long lColor = 0;
                pGoods->GetGoodsQuality(strQuality,lColor);
                if (!strQuality.empty())
                {
                    GetFileName(strQuality.c_str(),strImageFileName);
                    const CEGUI::Image &pQuilityImage = SetBackGroundImage(pContainer,"Quality","datafiles/imagesets/Public",strImageFileName);
                    
                }
                else
                {
                    pContainer->setProperty("Image","");
                }
                GetFilePath(strIconPath,strImageFilePath);
                GetFileName(strIconPath,strImageFileName);
                CEGUI::String strImagesetName = "GoodIcon/";
                strImagesetName += strImageFileName;
                const CEGUI::Image &pGoodIcon = SetBackGroundImage(pImageComp,strImagesetName.c_str(),strImageFilePath,strImageFileName);
                float width  = pGoodIcon.getSourceTextureArea().getWidth();
                float height = pGoodIcon.getSourceTextureArea().getHeight();
                pContainer->setSize(CEGUI::UVector2(cegui_absdim(width),cegui_absdim(height)));
                pCurDrag->setSize(CEGUI::UVector2(cegui_absdim(width),cegui_absdim(height)));               
            }
            else
            {
                pImageComp->setProperty("Image","");
            }
            
            // 当物品数大于1的时候才显示数量
            if(pPlayerGoods->lNum > 1)
            {
                char strGoodsNum[128];
                sprintf_s(strGoodsNum,"%d",pPlayerGoods->lNum);
                pImageComp->setText(strGoodsNum);
            }
            else
                pImageComp->setText("");
        }
    }
    else
    {
        pImageComp->setProperty("Image","");
        pImageComp->setProperty("Text","");
        pContainer->setProperty("Image","");
    }
}

void PackageEvent::OnPageRelease(GamePage *pPage)
{

}

bool PackageEvent::handle_GoodsDropped(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    CMainPlayerHand *pHand = GetGame()->GetMainPlayerHand();
    CGUID guid = pMainPlayer->GetExID();
    CPlayer::tagGoods* pstGoods = pHand->GetTGoodsOfMainPlayerHand();
    if (pstGoods->lNum == 0)
        return false;
    // cast the args to the 'real' type so we can get access to extra fields
    const DragDropEventArgs& dd_args =
        static_cast<const DragDropEventArgs&>(args);
    //得到当前正在拖拽的物品容器
    DragContainer *pCurDrag = dd_args.dragDropItem;
    //得到当前被拖拽物品来自那个栏位容器
    //CEGUI::Window *pPickUpContainer = pCurDrag->getParent();
    //放下后，被换到手上的DrageItem
    CEGUI::Window *pReplaceGoods = dd_args.window;
    //放入的槽
    CEGUI::Window *pPutContainer = dd_args.window->getParent();
    //获取企图放置的格子容器的孩子数量，如果为0，表示这个格子是空的，否则就要物品交换
    long lChildCount = (long)dd_args.window->getChildCount();
    if (pPutContainer->getName() == "Package/backgrond")
    {
        //放在了原来的格子里面
        pHand->ClearMainPlayerHand();
        return false;
    }
    else if(lChildCount == 1 && pPutContainer->getParent()->getName() == "Package/backgrond")    //发生交换物品位置
    {
        if(pCurDrag == pReplaceGoods) //放回原来的格子里。
        {
            pHand->ClearMainPlayerHand();
            return true;
        }
        //放置刚才拿起的物品
        //pPutContainer->addChildWindow(pCurDrag);
        //pCurDrag->setPosition(uvector2(udim(0, 0),UDim(0, 0)));
        
        //放置原来格子中的物品到刚才拿走物品的那个格子里面
        //pPickUpContainer->addChildWindow(pReplaceGoods);
        //pPutContainer->removeChildWindow(pReplaceGoods);
        
        //交换物品图标并设置数量
        //源图片容器
        CEGUI::Window *pSrcImageComp = pCurDrag->getChildAtIdx(0);
        //目的图片容器
        CEGUI::Window *pDestImageComp = pReplaceGoods->getChildAtIdx(0);
        
        CEGUI::String SrcGoodImage = pSrcImageComp->getProperty("Image");
        CEGUI::String DestGoodImage = pDestImageComp->getProperty("Image");
        //获取数量
        CEGUI::String srcGoodsNum  = pSrcImageComp->getText();
        CEGUI::String DestGoodsNum = pDestImageComp->getText();
        //是否能交换物品
        bool bCanSwap = true;
        bool bBind = false;
        CGoods* pGoodsInItem = GetGame()->GetMainPlayerHand()->GetPGoodsOfMainPlayerHand();
        if (pGoodsInItem)
        {
            if( pstGoods->lType > DEPOT_CONTAINER_TYPE_BEGIN_VALUE  && pstGoods->lType < DEPOT_CONTAINER_TYPE_END_VALUE )
            {
                if( pGoodsInItem->GetEffectVal( GAP_CANNOT_SAVE_DEPOT, 1 ) )
                    bCanSwap = false;
            }

            //
            if( PEI_EQUIPMENT == pstGoods->lType )
            {
                DWORD dwBind = pGoodsInItem->GetEffectVal(GAP_BIND, 1);
                if( eBT_DressAndCantUntie == dwBind  ||  eBT_DressAndBind == dwBind )
                {
                    bBind = true;
                }
            }
        }
        if( bCanSwap )
        {
            
            CEGUI::FrameWindow *pPageWindow = static_cast<CEGUI::FrameWindow*>(GetInst(PackagePage).GetPageWindow());
            CEGUI::DefaultWindow *pPageBackWindow = static_cast<CEGUI::DefaultWindow*>(pPageWindow->getChild("Package/backgrond"));
            long lpos = GetIndexByChild(pPageBackWindow,pPutContainer);
           
            // 是否是 装备后绑定
            if( bBind )
            {
                //弹出消息框提示装备后绑定
                MsgEvent bindEvent;
                bindEvent.pContext = AppFrame::GetText("Goods_310");    //该物品装备后将被绑定，是否继续？
                PackageMsgBox::tagChangeEquipMSG *pMSGData = new PackageMsgBox::tagChangeEquipMSG;
                pMSGData->nIndex       = lpos;
                pMSGData->nSPos        = pstGoods->lPos;
                pMSGData->nExtendID    = PEI_PACKET;
                pMSGData->nSExtendID   = pstGoods->lType;
                bindEvent.pData = pMSGData;
                bindEvent.bMode = false;
                bindEvent.pEventDef.FunTable = ChangeEquipMsgBox;
                bindEvent.pEventDef.BoxType = Msg_YesNo;
                GetInst(MsgEventManager).PushEvent(bindEvent);
            }
            else
            {
                 //设置图标
                pDestImageComp->setProperty("Image",SrcGoodImage);
                pSrcImageComp->setProperty("Image",DestGoodImage);
                //设置数量
                pDestImageComp->setText(srcGoodsNum);
                pSrcImageComp->setText(DestGoodsNum);
                long count = pstGoods->lNum;
                const CGUID &sguid = pstGoods->pGoods->GetExID();
                SendPutDownMes(TYPE_PLAYER,guid,pstGoods->lType,pstGoods->lPos,TYPE_PLAYER,guid,PEI_PACKET,lpos,TYPE_GOODS,sguid,count);
                pHand->ClearMainPlayerHand();
            }
        }
        else
        {
            // "该物品禁止放入仓库"
            TCHAR	szPrompt[ 512 ];
            wsprintf( szPrompt, AppFrame::GetText("Depot_35")   //特殊物品：【%s】 不允许放入银行！
                , pGoodsInItem->GetName() );
            GetInst(MsgEventManager).PushEvent(Msg_Ok,szPrompt);
            
        }
        //if ()
        //{
        //   
        //} 
        //else //可合并的物品则需要设置目的容器 清空原容器
        //{
        //    //设置图标
        //    pDestImageComp->setProperty("Image",SrcGoodImage);
        //    pSrcImageComp->setProperty("Image","");
        //    //设置数量
        //    long lNewNum = CEGUI::PropertyHelper::stringToInt(srcGoodsNum)
        //        + CEGUI::PropertyHelper
        //    pDestImageComp->setText(srcGoodsNum+);
        //    pSrcImageComp->setText("");
        //}
        //pPickUpContainer->notifyScreenAreaChanged();

          
    }

    return true;
}

bool PackageEvent::handle_GoodsPickUp(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;
    CMainPlayerHand *pHand = GetGame()->GetMainPlayerHand();
    const WindowEventArgs& dd_args = static_cast<const WindowEventArgs&>(args);
    //得到当前正在拖拽的物品容器
    DragContainer *pCurDrag = static_cast<DragContainer*>(dd_args.window);
    long pos = GetCurGridPos(pCurDrag);
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    CPlayer::tagGoods *pPlayerGoods = pMainPlayer->GetGoodsOnPlayer(PEI_PACKET,(WORD)pos);
    if(pPlayerGoods)
    {
        pHand->SetMainPlayerHandByGoods(pPlayerGoods->bChange,pPlayerGoods->lNum,pos,pPlayerGoods->pGoods,PEI_PACKET);
    }
    else
    {
        pHand->ClearMainPlayerHand();
    }
    return true;
}


void PackageEvent::SendPutDownMes(long SorceType/*源类型*/,const CGUID& SorceGUID/*源对象的GUID*/,long lSorceObjectExtendID,long lSorceObjectPos,
                                     long DesType,const CGUID& DesGUID,long lDesObjectExtendID,long lDesObjectPos,long lObjectType,const CGUID& ObjectGUID,long lObjectNum)
{
    if (g_bDebug)
    {
        /// 用于输出发送物品移动的信息
        //char strTitle[64] = "";
        //if (GetGame()->GetCGuiEx()->GetItemPageEx()->GetFinishBagState())
        //{
        //    strcpy_s(strTitle,"--客户端向服务器发送的（整理）物品移动信息--");
        //}else
        //    strcpy_s(strTitle,"--客户端向服务器发送的物品的普通的移动信息--");
        //char strPrintInfor[512] = "";
        //char strGoodsGUID[64] = "";
        //ObjectGUID.tostring(strGoodsGUID);
        //sprintf_s(strPrintInfor,"%s\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%d\n",
        //    strTitle,
        //    "物品原来所在的容器ID:",lSorceObjectExtendID,
        //    "物品在原来容器中的位置:",lSorceObjectPos,
        //    "物品要移动到的目标容器ID:",lDesObjectExtendID,
        //    "物品要移动到目标容器的位置:",lDesObjectPos,
        //    "本次操作的物品的GUID:",strGoodsGUID,
        //    "本次操作的物品数量:",lObjectNum);
        //PutStrToFileSaveToDir( "log/GoodsMoveLog", "记录物品的移动过程", strPrintInfor, false );
        //////////////////////////////
    }

    CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
    msg.Add((LONG)SorceType);
    msg.Add(SorceGUID);
    msg.Add((LONG)lSorceObjectExtendID);
    msg.Add((LONG)lSorceObjectPos);
    msg.Add((LONG)DesType);
    msg.Add(DesGUID);
    msg.Add( lDesObjectExtendID);
    msg.Add( lDesObjectPos );
    msg.Add((LONG)lObjectType);
    msg.Add(ObjectGUID);
    msg.Add((LONG)lObjectNum);
    msg.Send();
}

bool PackageEvent::handle_PageClose(const CEGUI::EventArgs& args)
{
    GetInst(PackagePage).Close();
    return true;
}

bool PackageEvent::handle_OpenPlayerShop(const CEGUI::EventArgs& args)
{
	if(GetGame()->GetMainPlayer()->GetState() == CShape::STATE_DIED)
		return false;

	// 世界地图打开时不能开启个人商店

	// 制作页面打开时不能开启个人商店

	// 打开个人商店前要先关闭增值商店

	// 招募状态不能开店
	if(GetGame()->GetMainPlayer()->IsRecruitingTeam())
	{
        GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Shop_12"),
            0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		return false;
	}

	// 战斗状态不能开店
	if (GetGame()->GetMainPlayer()->GetState()==CShape::STATE_FIGHT)
	{
		GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Shop_17"), 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		return false;
	}
	/// 判断玩家周围5*5的范围内是否有别的玩家已经在开店了，如果有则不容许开店
	if (GetGame()->GetMainPlayer()->CheckIsAnyBodyShopOpenedOnRange(2))
	{
		GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Shop_16"), 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
		return false;
	}

	//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetMoveInfo(0,0,0);
	GetGame()->GetGameControl()->AutoMoveStop();

	CMessage msg(MSG_C2S_PLAYERSHOP_CREATE);
	msg.Send((CClient*)GetGame()->GetNetClient());

	//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
	return true;
}

bool PackageEvent::OnRBClickGoodsForOtherUIPages(CPlayer::tagGoods* pstGoods)
{
	// 如果没有物品则返回，如果处于锁定状态也返回
	if(!pstGoods  ||  !pstGoods->pGoods)
		return false;

	// 缓冲物品，防止外界修改
	CPlayer::tagGoods stGoods = *pstGoods;
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	if (pHand!=NULL)
	{
		CGoods* pGoods = pHand->GetPGoodsOfMainPlayerHand();
		if (pGoods!=NULL&&pGoods->GetExID()==stGoods.pGoods->GetExID())
		{
			pGoods->SetHaveShadowState(false);
			pHand->DelShadowStateOfGoods();
			pHand->ClearMainPlayerHand();
		}
	}

	//add by shixi 20100921
	if ( GetWindow(PERSONAL_MAIL)->isVisible() )
	{
		GetInst(CMail).AddGoods( &stGoods );
		return true;
	}


	// 打开交易界面，处理右键点击物品
	if (GetWindow("PlayerTrade")->isVisible())
	{
		GetInst(PlayerTrade).OnInputGoods(&stGoods);
		return true;
	}
	return false;
}

bool PackageEvent::ProcessMouseRBtnDown(const CEGUI::EventArgs &args)
{
    CMainPlayerHand *pHand = GetGame()->GetMainPlayerHand();
    if (pHand && pHand->GetGoodsIndexOfMainPlayerHand()!=0) 
        return true;
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    if (pMainPlayer->IsDied()) return true;
    
    const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
    CEGUI::DragContainer *pDrug = static_cast<CEGUI::DragContainer*>(e_mouse.window);
    //鼠标右键操作
    
    if(e_mouse.button == CEGUI::RightButton)
    {
        long opPos = GetCurGridPos(pDrug);
        if(opPos == -1)
            return false;
        CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
        long nSExtendId = -1;
        long nIndex = -1;
        long nExtendId = -1;
        if (opPos>= 0 && opPos<20)  //主背包范围内
        {
            nSExtendId = PEI_PACKET;
        }
        CPlayer::tagGoods *ptagGoods = pMainPlayer->GetGoodsOnPlayer((WORD)nSExtendId,(WORD)opPos);
        if(ptagGoods)
        {
			//modify by shixi
			if(ptagGoods->pGoods->IsLocked())
			{
				return false;
			}
            CGoods *pGoods = ptagGoods->pGoods;
            if (pGoods == NULL && !pMainPlayer->GetGoodsUseState((WORD)pGoods->GetIndex()))
                return false;

			if (OnRBClickGoodsForOtherUIPages(ptagGoods))
				return true;

            // 原栏位有物品,且已记录了原栏位的位置和容器类型
            if(pGoods != NULL && opPos != -1)
            {
                
                if(nSExtendId != -1)
                {
                    CGoodsList::tagGoods2* pstGoods = CGoodsList::GetProperty(pGoods->GetIndex());
                    if(!pstGoods)
                        return false;

                    DWORD dwType = pstGoods->BaseProperty.dwType;
					if (dwType==GBT_CONSUMABLE)
					{
						pMainPlayer->UseItem(pGoods->GetIndex(),nSExtendId,opPos);
						return true;
					}
					else if (dwType==GBT_EQUIPMENT)
					{
						nIndex = (long)pGoods->GetEquipType();
						nExtendId = PEI_EQUIPMENT;
					}
					else if (dwType==GBT_SUBPACK)
					{
						if (pGoods->GetEffectVal(GAP_PACK_TYPE,1)==2)
						{
							CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
							if (pHand!=NULL)
							{
								pHand->SendPutDownMes(TYPE_PLAYER,pMainPlayer->GetExID(),
									nSExtendId,opPos,TYPE_PLAYER,pMainPlayer->GetExID(),
									PEI_BUSINESPACKFIELD,0,TYPE_GOODS,pGoods->GetExID(),
									pGoods->GetNum());
							}
							return true;
						}
					}
                }

                if (nIndex != -1 && nExtendId != -1)
                {
                    WORD wAction = pMainPlayer->GetAction();
                    if(wAction>=CShape::ACT_MEGRIM) return false;

                    // 是否是 装备后绑定
                    DWORD dwBind = pGoods->GetEffectVal(GAP_BIND, 1);
                    if( eBT_DressAndCantUntie == dwBind  ||  eBT_DressAndBind == dwBind )
                    {   
                        MsgEvent bindEvent;
                        bindEvent.pContext = AppFrame::GetText("Goods_310");    //该物品装备后将被绑定，是否继续？
                        PackageMsgBox::tagChangeEquipMSG *pMSGData = new PackageMsgBox::tagChangeEquipMSG;
                        pMSGData->nIndex       = nIndex;
                        pMSGData->nSPos        = opPos;
                        pMSGData->nExtendID    = nExtendId;
                        pMSGData->nSExtendID   = nSExtendId;
                        bindEvent.pData = pMSGData;
                        bindEvent.bMode = false;
                        bindEvent.pEventDef.FunTable = ChangeEquipMsgBox;
                        bindEvent.pEventDef.BoxType = Msg_YesNo;
                        GetInst(MsgEventManager).PushEvent(bindEvent);
                    }
                    else
                    {
                        SendChangeEquipMSG(nIndex,opPos,nExtendId,nSExtendId);
                    }
                }
            }
            return true;
        }
    }
    return true;
}


/* 
*  交换装备，一般在右键点击装备后触发
*  参数：nIndex表示目标栏位，nExtendId表示目标容器；
*        nSPos表示源栏位，nSExtendId表示源目标容器。
*/
void PackageEvent::SendChangeEquipMSG(int nIndex,int nSPos,int nExtendID,int nSExtendID)
{
    DWORD dwAmount=1;
    CPlayer::tagGoods *pstGoods=NULL;
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    if (pMainPlayer == NULL)
        return;
    // 原栏位位置的物品
    if(nSExtendID==PEI_PACKET)
        pstGoods= pMainPlayer->GetGoodsOnPlayer(PEI_PACKET,nSPos);
    //else if(nSExtendID>=PEI_SUBPACK)
    //    pstGoods= GetCoteData(nSExtendID-PEI_PACK1,nSPos);
    if(!pstGoods)	return;
    if(!pstGoods->pGoods)	return;
    dwAmount=pstGoods->pGoods->GetNum();
    if(pstGoods==NULL || pstGoods->pGoods==NULL)return;

    CMessage msg(MSG_C2S_CONTAINER_OBJECT_MOVE);
    msg.Add((LONG)TYPE_PLAYER);
    msg.Add(pMainPlayer->GetExID());
    msg.Add((LONG)nSExtendID);
    msg.Add((LONG)nSPos);
    msg.Add((LONG)TYPE_PLAYER);
    msg.Add(pMainPlayer->GetExID());
    msg.Add((LONG)nExtendID);
    msg.Add((LONG)nIndex);
    msg.Add((LONG)TYPE_GOODS);
    msg.Add(pstGoods->pGoods->GetExID());
    msg.Add((LONG)dwAmount);
    msg.Send();
}

//得到当前操作的的格子索引
long PackageEvent::GetCurGridPos(CEGUI::DragContainer *pDrag)
{
    
    CEGUI::FrameWindow *pPageWindow = static_cast<CEGUI::FrameWindow*>(GetInst(PackagePage).GetPageWindow());
    CEGUI::DefaultWindow *pPageBackWindow = static_cast<CEGUI::DefaultWindow*>(pPageWindow->getChild("Package/backgrond"));
    
    return GetIndexByChild(pPageBackWindow,pDrag->getParent());    
}

bool PackageEvent::updateMoney(const CEGUI::EventArgs & args)
{
    CPlayer *pMainplayer = GetGame()->GetMainPlayer();
    DWORD dwSilver = pMainplayer->GetSilverMoney(); //银币
    DWORD dwGold   = pMainplayer->GetMoney();       //金币
    CEGUI::Window *st_silver = GetWndMgr().getWindow("Package/backgrond/st_silver");
    st_silver->setText(CEGUI::PropertyHelper::intToString(dwSilver));
    CEGUI::Window *st_gold = GetWndMgr().getWindow("Package/backgrond/st_Gold");
    st_gold->setText(CEGUI::PropertyHelper::intToString(dwGold));
    return false;
}

bool PackageEvent::OnMouseEnter(const CEGUI::EventArgs & args)
{
    const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
    static CEGUI::Window *pLastWin = 0;
    CEGUI::Window *pHoverWin = e_mouse.window;
    if(!pHoverWin)
        return false;
    if (pHoverWin != pLastWin)
    {    
        pLastWin = pHoverWin;
        CEGUI::Window *pPageWindow = GetInst(PackagePage).GetPageWindow();
        CEGUI::Window *pPageBackWindow = pPageWindow->getChild("Package/backgrond");
        m_UpdateGoodsPos = GetIndexByChild(pPageBackWindow,e_mouse.window->getParent());
        CEGUI::String WndName = pHoverWin->getParent()->getName() + "\n";
        OutputDebugString(WndName.c_str());
        GetGame()->GetGameDespParser()->SetSourceUI(GameDespParser::SI_PACKAGE);
        FireUIEvent("GoodsDesp","updateDesp");
    }
    OutputDebugString("打开——物品描述界面\n");
    CGoods* pMouseGoods = GetGame()->GetMainPlayer()->GetGoodsByPosition(PEI_PACKET,m_UpdateGoodsPos);
    if(pMouseGoods)
        GetWndMgr().getWindow("GoodsDesp")->setVisible(true);
    //FireUIEvent("GoodsDesp",EVENT_OPEN);
    return true;
}

bool PackageEvent::OnMouseLeave(const CEGUI::EventArgs & args)
{
    OutputDebugString("关闭——物品描述界面\n");
    GetWndMgr().getWindow("GoodsDesp")->setVisible(false);
    //FireUIEvent("GoodsDesp",EVENT_CLOSE);
    return true;
}