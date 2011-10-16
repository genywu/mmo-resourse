#include "StdAfx.h"
#include "../UIDef.h"
#include "QuestDef.h"
//下面这个include可以删掉，暂时添加是为了有自动提示。
#include "../CEGUI/cegui/include/elements/CEGUITree.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/QuestSystem/QuestSystem.h"

CEGUI::Window* InitQuest();

bool OnQuestTabSelectChanged(const CEGUI::EventArgs &args);         //改变任务类型标签的选中项目

bool OnNormalQuestTreeSelectChanged(const CEGUI::EventArgs &args);  //改变普通任务树的选中项目
bool OnNormalQuestTreeAdded(const CEGUI::EventArgs &args);          //普通任务树添加一项
bool OnNormalQuestTreeDeleted(const CEGUI::EventArgs &args);        //普通任务树删除一项

bool OnRemoveQuestButtonClicked(const CEGUI::EventArgs& args);      //点击"放弃任务"按钮
bool OnShareQuestButtonClicked(const CEGUI::EventArgs& args);       //点击"共享任务"按钮
bool OnDrawAwardButtonClicked(const CEGUI::EventArgs& args);        //点击"领取奖励"按钮

bool AddQuest_impl(CEGUI::Tree* tree, CQuestRecord* questRecord);

bool OnUpdateTeamQuestList(const CEGUI::EventArgs& e);			    //更新队伍任务显示
bool OnUpdateTeamQuestRunning(const CEGUI::EventArgs& e);	        //更新队伍正在进行任务的显示

bool OnOpenQuestUI(const CEGUI::EventArgs& Args);                   //打开任务UI
bool OnUpdateQuestSelectText(const CEGUI::EventArgs& Args);         //
bool OnClearQuestTreeSelectText(const CEGUI::EventArgs & Args);     //清空任务显示描述文本

namespace  QuestUI
{
   static CQuestRecord*  g_CurrRecord = NULL;
}

CEGUI::Window* InitQuest()
{
    CEGUI::Window* wnd = LoadUI(QUEST_PAGE_NAME);

    CEGUI::Window* tabCtrl;
    CEGUI::Window* defWnd;
    CEGUI::Window* tree;
    CEGUI::Window* btn;
    wnd->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenQuestUI));
    tabCtrl = wnd->getChild("Quest/Tab");
    tabCtrl->subscribeEvent(CEGUI::TabControl::EventSelectionChanged,
        CEGUI::SubscriberSlot(OnQuestTabSelectChanged));

    //"Quest/Tab/Normal"是"Quest/Tab"的子节点的子节点，所以要递归
    defWnd = tabCtrl->getChildRecursive("Quest/Tab/Normal");
    tree = defWnd->getChild("Quest/Tab/Normal/Tree");
    tree->subscribeEvent(CEGUI::Tree::EventSelectionChanged,
        CEGUI::SubscriberSlot(OnNormalQuestTreeSelectChanged));

    tabCtrl->subscribeEvent("UQuestText",CEGUI::SubscriberSlot(OnUpdateQuestSelectText));

    tree->subscribeEvent("AddNormalQuest",
        CEGUI::SubscriberSlot(OnNormalQuestTreeAdded));

    tree->subscribeEvent("DeleteNormalQuest",
        CEGUI::SubscriberSlot(OnNormalQuestTreeDeleted));

    //其他标签
    //...   

    btn = wnd->getChild("Quest/Delete");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnRemoveQuestButtonClicked));

    btn = wnd->getChild("Quest/Share");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnShareQuestButtonClicked));

    btn = wnd->getChild("Quest/Draw");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnDrawAwardButtonClicked));

    //必须设置viaible为false，才能在第一次按快捷键"Q"就弹出界面
    wnd->setVisible(false);
	/////////////////////////////////////////////////
	// zhaohang  2010/6/29 
	// 队伍任务函数绑定
	wnd->subscribeEvent(QUEST_TEAMFUNC_UPDATE,CEGUI::SubscriberSlot(OnUpdateTeamQuestList));
	wnd->subscribeEvent(QUEST_TEAMFUNC_UPDATERUNNING,CEGUI::SubscriberSlot(OnUpdateTeamQuestRunning));
	////////////////////////////////////////////////
    return wnd;
}

bool OnOpenQuestUI(const CEGUI::EventArgs& Args)
{
    //初始化数据
    ////玩家有队伍,发送一次请求队伍任务更新
    //QuestMsgMgr& qmm = GetInst(QuestMsgMgr);
    //qmm.Send_TeamQuestUpdate(0);
    CEGUI::Window * tabCtrl = GetWndMgr().getWindow("Quest/Tab");
    CEGUI::Window * defWnd = tabCtrl->getChildRecursive("Quest/Tab/Normal");
    CEGUI::Window * tree = defWnd->getChild("Quest/Tab/Normal/Tree");

    static_cast<CEGUI::Tree*>(tree)->resetList();

    CPlayer* player = GetGame()->GetMainPlayer();
    std::list<CQuestRecord*>* quests = player->GetRoleQueue()->GetQuestRecordList();
    for (std::list<CQuestRecord*>::iterator it = quests->begin();
        it != quests->end(); ++it)
    {
        CQuestRecord* questRecord = *it;
        if (NULL == questRecord)
            continue;

        AddQuest_impl(dynamic_cast<CEGUI::Tree*>(tree), questRecord);
    }
    //第一次打开界面，显示index为0的标签项
    if (dynamic_cast<CEGUI::TabControl*>(tabCtrl)->getTabCount() > 0)
    {
        dynamic_cast<CEGUI::TabControl*>(tabCtrl)->setSelectedTabAtIndex(0);
        OnQuestTabSelectChanged(CEGUI::WindowEventArgs(tabCtrl));
    }
    return true;
}

//点击“放弃任务”按钮
bool OnRemoveQuestButtonClicked(const CEGUI::EventArgs& args)
{
    //找到选中的任务
    CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(GetWndMgr().getWindow("Quest/Tab/Normal/Tree"));
    if (NULL == tree)
        return true;

    CEGUI::TreeItem* item = tree->getFirstSelectedItem();
    if (NULL == item)
        return true;

    //向服务器发消息请求删除该任务
    uint questId = static_cast<CQuestRecord*>(item->getUserData())->GetQuestID();
    GetInst(QuestMsgMgr).Send_Disband(questId);

    return true;
}


//点击“共享任务”按钮
//暂时不能组队
bool OnShareQuestButtonClicked(const CEGUI::EventArgs& args)
{
    //找到选中的任务
    CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(GetWndMgr().getWindow("Quest/Tab/Normal/Tree"));
    if (NULL == tree)
        return true;

    CEGUI::TreeItem* item = tree->getFirstSelectedItem();
    if (NULL == item)
        return true;

    //向服务器发消息请求共享该任务
	//......
	uint questId = static_cast<CQuestRecord*>(item->getUserData())->GetQuestID();
	GetInst(QuestMsgMgr).Send_Shared(questId);
    return true;
}


//点击“领取奖励”按钮
//感觉这个按钮不应该在任务界面出现，应该在与NPC对话时领取。
bool OnDrawAwardButtonClicked(const CEGUI::EventArgs& args)
{
    return true;
}


//普通任务树删除一项
bool OnNormalQuestTreeDeleted(const CEGUI::EventArgs &args)
{
    //找到任务树
    CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(WEArgs(args).window);
    if (NULL == tree)
        return true;

    //找到待删除的任务
    CQuestRecord* questRecord = CQuestSystem::GetTempQuestRecord();
    if( QuestUI::g_CurrRecord == questRecord )
         QuestUI::g_CurrRecord = NULL;

    CEGUI::TreeItem* item = tree->findFirstItemWithID(questRecord->GetQuestID());

    //删除该任务
    tree->removeItem(item);
	//清空任务具体描述
	CEGUI::Window* wnd = tree->getParent()->getChildRecursive(1);
	if(wnd)
		wnd->setText("");

    static CEGUI::Window * TextWindow = GetWndMgr().getWindow("Quest/Tab/Normal/Text");
    TextWindow->setText( "" );
    
    return true;
}


//普通任务树添加一项
bool OnNormalQuestTreeAdded(const CEGUI::EventArgs& args)
{
    //找到“普通任务”树
    CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(WEArgs(args).window);
    if (NULL == tree)
        return true;

    //找到待添加的任务
    CQuestRecord* questRecord = CQuestSystem::GetTempQuestRecord();

    //添加到任务树上
    AddQuest_impl(tree, questRecord);

    return true;
}

bool OnUpdateQuestSelectText(const CEGUI::EventArgs& Args)
{
    if ( !QuestUI::g_CurrRecord )
        return false;

    CQuestRecord* questRecord = QuestUI::g_CurrRecord;
    long questId = questRecord->GetQuestID();
    const CQuest* quest = CQuestSystem::GetQuest(questId);

    std::string  QuestDes;

    /// 任务描述 
    QuestDes = AppFrame::LoadQuestTextRes(questId,eQuestDes); 
    /// 任务目标
    const CQuestTrace * trace =quest->GetStep(questRecord->GetQuestStep())->GetQuestTraceHead();
    while (  trace !=NULL )
    {
        char str[64];
        long ltraceNum = 0 ;
        string strID = trace->GetGoodsStrID();
        // 如果该追踪的目标有原始名，表示需要在玩家背包查找该物品数量，然后设置给该追踪项数量。
        if(!strID.empty())
        {
            CGoodsList::tagGoods2* ptgGoods = CGoodsList::GetProperty(strID.c_str());
            if(ptgGoods)
                ltraceNum = GetGame()->GetMainPlayer()->GetGoodsAmount(ptgGoods->BaseProperty.dwIndex);
        }
        // 已完成数
        else
            ltraceNum =  questRecord->GetParam( trace->GetParam() );
        QuestDes += AppFrame::GetText(trace->GetDescIndex()) ;// CStringReading::LoadText( QuestRet(questId),trace->GetDescIndex() );
        sprintf_s(str,"     %d/%d   %s",ltraceNum,trace->GetSum(),ltraceNum == trace->GetSum()?"<完成>":"");
        QuestDes += str;     
        QuestDes += "[BR][BR]";
        trace = trace->next;
    }

    /// 任务奖励
    QuestDes+= CQuestEncourMan::GetAwardText(questId);

    static CEGUI::Window * TextWindow = GetWndMgr().getWindow("Quest/Tab/Normal/Text");

    TextWindow->setText( ToCEGUIString(QuestDes.c_str()) );
    return true;

}

//改变普通任务树的选中项目
bool OnNormalQuestTreeSelectChanged(const CEGUI::EventArgs& args)
{
    //得到当前选择的项
    const CEGUI::TreeEventArgs& treeArgs = static_cast<const CEGUI::TreeEventArgs&>(args);
    CEGUI::TreeItem* item = treeArgs.treeItem;
    if (NULL == item)
        return true;

    CQuestRecord* questRecord = (CQuestRecord*)item->getUserData();
    if (NULL == questRecord)
        return true;

    QuestUI::g_CurrRecord = questRecord;

    long questId = questRecord->GetQuestID();
    const CQuest* quest = CQuestSystem::GetQuest(questId);

    std::string  QuestDes;

    /// 任务描述   //CStringReading::LoadText( QuestRet(questId),quest->GetStep(questRecord->GetQuestStep())->GetText(5));
    QuestDes = AppFrame::LoadQuestTextRes(questId,eQuestDes); //%s
    /// 任务目标
    const CQuestTrace * trace =quest->GetStep(questRecord->GetQuestStep())->GetQuestTraceHead();
    while (  trace !=NULL )
    {
        char str[64];
        long ltraceNum = 0 ;
        string strID = trace->GetGoodsStrID();
        // 如果该追踪的目标有原始名，表示需要在玩家背包查找该物品数量，然后设置给该追踪项数量。
        if(!strID.empty())
        {
            CGoodsList::tagGoods2* ptgGoods = CGoodsList::GetProperty(strID.c_str());
            if(ptgGoods)
                ltraceNum = GetGame()->GetMainPlayer()->GetGoodsAmount(ptgGoods->BaseProperty.dwIndex);
        }
        // 已完成数
        else
           ltraceNum =  questRecord->GetParam( trace->GetParam() );
        QuestDes += AppFrame::GetText(trace->GetDescIndex()) ;// CStringReading::LoadText( QuestRet(questId),trace->GetDescIndex() );
        sprintf_s(str,"     %d/%d   %s",ltraceNum,trace->GetSum(),ltraceNum == trace->GetSum()?"<完成>":"");
        QuestDes += str;     
        QuestDes += "[BR][BR]";
        trace = trace->next;
    }

    /// 任务奖励
    QuestDes+= CQuestEncourMan::GetAwardText(questId);

    static CEGUI::Window * TextWindow = GetWndMgr().getWindow("Quest/Tab/Normal/Text");

    TextWindow->setText( ToCEGUIString(QuestDes.c_str()) );
    return true;
}

bool OnClearQuestTreeSelectText(const CEGUI::EventArgs & Args)
{
    static CEGUI::Window * TextWindow = GetWndMgr().getWindow("Quest/Tab/Normal/Text");

    TextWindow->setText( "" );

    QuestUI::g_CurrRecord = NULL;
    return true;
}


//改变任务类型标签的选中项目
bool OnQuestTabSelectChanged(const CEGUI::EventArgs& args)
{
    CEGUI::TabControl* tab = dynamic_cast<CEGUI::TabControl*>(WEArgs(args).window);
    if (NULL == tab)
        return true;
    size_t idx = tab->getSelectedTabIndex();
    CEGUI::Window* wnd = tab->getTabContentsAtIndex(idx);

    if (wnd->getName() == "Quest/Tab/Normal")
    {
        CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(wnd->getChild("Quest/Tab/Normal/Tree"));
        if (NULL == tree)
            return true;

        tree->show();
        return true;
    }
	else if ( wnd->getName() == "Quest/Tab/Team")
	{
		CEGUI::Tree* tree = dynamic_cast<CEGUI::Tree*>(wnd->getChild("Quest/Tab/Team/Tree"));
		if(NULL == tree)
			return true;
		//tree->show();
		return true;
	}
    else
    {
        return false;
    }
}


//在任务树上添加一条任务记录
bool AddQuest_impl(CEGUI::Tree* tree, CQuestRecord* questRecord)
{
    if (NULL == tree || NULL == questRecord)
        return false;

    long questId = questRecord->GetQuestID();
    const CQuest* quest = CQuestSystem::GetQuest(questId);
    const char* name = quest->GetAttribute().GetQuestName();
    CEGUI::TreeItem* item = new CEGUI::TreeItem( ToCEGUIString(name) );
    tree->addItem(item);
    item->setID(questId);
    item->setUserData(questRecord);
    item->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
    return true;
}

bool OnUpdateTeamQuestList(const CEGUI::EventArgs& e)
{
	return true;
}
bool OnUpdateTeamQuestRunning(const CEGUI::EventArgs& e)
{
	return true;
}