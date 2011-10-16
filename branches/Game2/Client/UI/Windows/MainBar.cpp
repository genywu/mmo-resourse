#include "stdafx.h"
#include "MainbarPreDef.h"
#include "SkillDef.h"
#include "../UIDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/MainPlayerHand.h"
#include "../../GameClient/ClientApp/Skills/SkillListXml.h"
#include "../../GameClient/ClientApp/Skills/SkillLogicSys.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../ui/UIManager.h"
#include "../../ui/UIExtendComponent/AnimEffectIcon.h"

extern void SubscriberRootEvent();

//由技能ID来设置DragContainer和其子控件StaticImage的显示(修改图标和设置控件ID与技能ID关联)
extern void SetDCIconBySkillID(uint skillID,CEGUI::Window* dragContainer);

//mainbar的事件注册
void SubscriberMainBarEvent(CEGUI::Window* pageWnd);

//界面的初始化状态设置
void SetInitializedMainBarUIProperty(CEGUI::Window* pageWnd);

//拖拽图标到mainbar上面的事件处理
bool OnMainBarDragContainerDropped(const CEGUI::EventArgs& e);

//技能拖图标到mainbar上面，技能DragContainerLeaves事件
bool OnDCLeavesFromSkillToMainbar(const CEGUI::EventArgs& e);

//根据dc的名称获得在mainbar中的索引
uint GetDCIndexInMainbar(CEGUI::Window* dc);

//根据dc的名称获得在mainbar中的索引
uint GetDCIndexInMainbar(CEGUI::Window* dc)
{
	if(!dc)
		return 0;
	for(int i = 0 ; i < SHORTCUT_COUNT ; ++i)
	{
		char dcWndName[256];
		sprintf(dcWndName,"MainBarPage/StaticImage%d/DragContainer",i);
		CEGUI::String wndName = dcWndName;
		if(dc->getName()== wndName)
			return i;
	}
	return 0;
}

void SubscriberMainBarEvent(CEGUI::Window* pageWnd)
{
	if(!pageWnd)
		return ;
	//注册Root界面事件
	SubscriberRootEvent();
	//创建冷却效果的图像集
    CEGUI::Imageset * coolImageSet = NULL;

    //设置资源文件路劲
    //CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
    //    (CEGUI::System::getSingleton().getResourceProvider());
    //rp->setResourceGroupDirectory(UIRes, "datafiles/imagesets");
    
    if(CEGUI::ImagesetManager::getSingleton().isDefined("CoolDown") == false)
        coolImageSet = &CEGUI::ImagesetManager::getSingleton().create("CoolDownEffect.imageset","");
    else
        coolImageSet = &CEGUI::ImagesetManager::getSingleton().get("CoolDown");
    //注册DragItem拖放到快捷键上的处理
	char dcWndName[256];
	for(int i = 0 ; i < SHORTCUT_COUNT ; ++i)
	{
		sprintf(dcWndName,"MainBarPage/StaticImage%d/DragContainer",i);
		CEGUI::Window* simgWnd = pageWnd->getChildRecursive(dcWndName);
		if(simgWnd)
		{
			//DragContainer拖动到上面并释放
			simgWnd->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,CEGUI::Event::Subscriber(OnMainBarDragContainerDropped));
			//技能ICont拖离原界面到mainbar
			simgWnd->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves,CEGUI::Event::Subscriber(OnDCLeavesFromSkillToMainbar));
		}
        sprintf(dcWndName,"MainBarPage/StaticImage%d/CoolImage%d",i,i);
        //sprintf(dcWndName,"MainBarPage/StaticImage%d/DragContainer/sImage",i);
        CEGUI::Window* coolWnd = pageWnd->getChildRecursive(dcWndName);
        AnimEffectIcon *pAnimIcon = new AnimEffectIcon(coolWnd,*coolImageSet);
        UIManager::AddAnimIcon(coolWnd,pAnimIcon);
	}
}

bool OnMainBarDragContainerDropped(const CEGUI::EventArgs& e)
{
	CEGUI::Window* dragTargetWnd = WEArgs(e).window;
	CEGUI::Window* dropWnd = static_cast<const CEGUI::DragDropEventArgs&>(e).dragDropItem;
#ifdef _DEBUG
	const char* targetName = dragTargetWnd->getName().c_str();
	const char* dropName = dropWnd->getName().c_str();
#endif
	/////////////////////////////////////////////////
	// zhaohang  2010/5/18 
	// 	//处理其他界面DragContainer拖动到mainbar的DragContainer上
	/////////////////////////////////////////////////
	uint targetID = dragTargetWnd->getID();
	uint dropID = dropWnd->getID();
	CEGUI::String dropWndName = dropWnd->getName();
	SkillLogicSys& SkillLS = GetInst(SkillLogicSys);

	//判断是否是拖技能到mainbar上面
	if(dropWndName.find(SKILL_PAGE_NAME) != CEGUI::String::npos)//是
	{
		//修改图标和关联ID
		SetDCIconBySkillID(dropID,dragTargetWnd);
		/////////////////////////////////////////////////
		// zhaohang  2010/5/18 
		// 设置快捷键
		uint hotKeyIdx = GetDCIndexInMainbar(dragTargetWnd);
		SkillLS.Send_Set_Skill_HotKey(hotKeyIdx+SHORTCUT_COUNT,dropID);//0~11->F1~F12,所以加12
		CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
		if(pHand)
			pHand->ClearMainPlayerHand();//拖放成功了，所以要释放手上所拿图标
	}
	/////////////////////////////////////////////////
	// zhaohang  2010/5/18 
	// 处理mainbar的互相交换
	if(dropWndName.find("MainBarPage") != CEGUI::String::npos)
	{
		SetDCIconBySkillID(dropID,dragTargetWnd);
		SetDCIconBySkillID(targetID,dropWnd);
		//处理快捷键交换
		uint hotkeyIndx = GetDCIndexInMainbar(dragTargetWnd);
		SkillLS.Send_Set_Skill_HotKey(hotkeyIndx+SHORTCUT_COUNT,dropID);
		hotkeyIndx = GetDCIndexInMainbar(dropWnd);
		SkillLS.Send_Set_Skill_HotKey(hotkeyIndx+SHORTCUT_COUNT,targetID);
	}
	/////////////////////////////////////////////////
	return true;
}

//技能Icon拖到mainbar，DC离开技能界面
bool OnDCLeavesFromSkillToMainbar(const CEGUI::EventArgs& e)
{
	CEGUI::Window* targetDC = WEArgs(e).window;
	CEGUI::Window* sourceDC = static_cast<const CEGUI::DragDropEventArgs&>(e).dragDropItem;
#ifdef _DEBUG
	const char* targetName = targetDC->getName().c_str();
	const char* dropName = sourceDC->getName().c_str();
#endif
	//判断源Dc是否是技能ICon
	CEGUI::String dropWndname = sourceDC->getName();
	if(dropWndname.find(SKILL_PAGE_NAME)== CEGUI::String::npos)
		return false;
	//是的情况处理
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();//获得手
	if(pHand)
	{
		pHand->SetMainPlayerHandBySkill(sourceDC->getID());//设置手上所拿为技能
	}
	return true;
}

void SetInitializedMainBarUIProperty(CEGUI::Window* pageWnd)
{
	if(!pageWnd)
		return;
	CPlayer* player = GetGame()->GetMainPlayer();
	//1~= 快捷栏的初始化 (图标显示）
	for(int i = 0 ; i < SHORTCUT_COUNT ; ++i)
	{
		uint hotkey = player->GetHotKey(i + SHORTCUT_COUNT);
		if(hotkey!=0)//有对应热键
		{
			// 快捷物品
			if ( ( hotkey & 0x80000000)==0 )
			{
				continue;
			}
			else//技能
			{
				uint skillID = hotkey & 0x7fffffff;
				CPlayer::tagSkill* skill = player->GetSkill(skillID);
				if(!skill)
					continue;
				//判断指定的技能在职业下是否可用
				if( CSkillListXml::OccuJudgeStudy(skillID,player->GetOccupation()) == false)
				{
					//MessageBox(g_hWnd,"当前技能不在玩家职业下，技能不可使用","Error",MB_OK);
					char strTemp[256];
					sprintf(strTemp,AppFrame::GetText("SDCC_65"),skillID);
					GetInst(MsgEventManager).PushEvent(Msg_Ok,strTemp);
				}
				//处理图标显示
				char dcWndName[256];
				sprintf(dcWndName,"MainBarPage/StaticImage%d/DragContainer",i);
				CEGUI::Window* dc = pageWnd->getChildRecursive(dcWndName);
				SetDCIconBySkillID(skillID,dc);
			}
		}
	}
}

void UpdateHotKeyCoolDownEffect(long lSkillID)
{
   CPlayer::tagSkill *ptagS = GetGame()->GetMainPlayer()->GetSkill(lSkillID);
   CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
   static char uiName[128] = "";
   DWORD dwReTime = 0;
   int   iHotPos = -1;
   for (int i = 0; i< 12; i++)
   {
       //获得数据
       DWORD dwHotKey = pMainPlayer->GetHotKey(i+12);
       
       if(dwHotKey != 0)
       {
           // 快捷物品
           if ( (dwHotKey & 0x80000000)==0 )
           {
               // 调整物品冷却动画图标
               dwReTime = pMainPlayer->GetGoodsRestoreTime((WORD)dwHotKey);
               iHotPos = i;
               break;
           }
           else
           {
               DWORD wSkillID = dwHotKey&0x7fffffff;
               if(wSkillID == lSkillID)
               {
                   iHotPos  = i;
                   dwReTime = ptagS->dwRestoreTime;
                   break;
               }
           }
       }
   }
   if (dwReTime != 0 && iHotPos != -1)
   {
       sprintf_s<128>(uiName,"MainBarPage/StaticImage%d/CoolImage%d",iHotPos,iHotPos);
       CEGUI::Window *pCoolUI = CEGUI::WindowManager::getSingleton().getWindow(uiName);
       UIManager::GetAnimIcon(pCoolUI)->BeginAnimateEffect(dwReTime);
   }
}