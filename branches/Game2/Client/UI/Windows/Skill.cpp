#include "stdafx.h"
#include "MainbarPreDef.h"
#include "../UIDef.h"
#include "SkillDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/MainPlayerHand.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/Skills/SkillListXml.h"
#include "../../GameClient/ClientApp/Skills/SkillLogicSys.h"
#include "../../../Public/Common/OccupSkillList.h"
#include "../../../Public/Common/SkillRelated.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
extern uint GetDCIndexInMainbar(CEGUI::Window* dc);

//得到技能描述控件的索引
uint GetSkillDescIndexInSkillTemplate(CEGUI::Window* dc);
//由技能ID来设置DragContainer和其子控件StaticImage的显示(修改图标和设置控件ID与技能ID关联)
void SetDCIconBySkillID(uint skillID,CEGUI::Window* dragContainer);

//修改技能界面的Icon的文字说明
void SetSkillIconDesc(uint skillID,CEGUI::Window* descWnd,uint SkillLevel = 1);

CEGUI::Window* InitSkill();
//设置初始化界面状态
void SetInitializedSkillTemplateUIProperty(CEGUI::Window* PageWnd);

//事件处理函数注册集合
void SubscriberSkillTemplateEvent(CEGUI::Window* PageWnd);

//技能图标拖动释放
bool OnSkillIconDragDropped(const CEGUI::EventArgs& e);

//技能图标拖动离开控件
bool OnSkillIconDragDropLeaves(const CEGUI::EventArgs& e);

////技能图标拖动进入mainbar控件
//bool OnSkillIconDragDropEnters(const CEGUI::EventArgs& e);

bool OnPageChanged(const CEGUI::EventArgs& e);
//添加已会技能
bool OnAddCanUseSkill(const CEGUI::EventArgs& e);

bool OnListboxSelChanged(const CEGUI::EventArgs& e);

////点击升级按钮
//bool OnUpdateSkillLvBtnClicked(const CEGUI::EventArgs& e);


//更新Tab下某一选项的显示
void UpdateTabContent(CEGUI::Window* selTab,size_t idxInTabCtl);


//更新界面右侧的已会招式图和文本描述,参数startPos/endPos 是玩家招式列表里的 开始/结束 位置
void UpdateCanUseSkill(const uint startPos,const uint endPos);

//根据选中的ListItem类型更新已会招式UI，被UpdateCanUseSkill()调用
void UpdateCanUseSkillByType(const uint startPos,const uint endPos,uint listitemID);

// 被UpdateCanUseskillByType()调用
void UpdateRightUI(const DWORD dwSkillID,const uint indexWnd,uint skillLevel = 1);

//清空已会技能UI的显示数据
void ClearCanUseSkill();

//更新升级某个Tab下的某个listItem里的技能消耗数据的显示
void UpdateCustOnLvUp();

//更新界面中间的图标
void UpdateCentreMainIcon();

//获取TabContent下的listbox窗口
CEGUI::Listbox* GetListBoxInCurSelTabContent();

//获取当前tabCtrl下选中的tabContent
CEGUI::Window* GetCurSelTabContent();

typedef std::list<COccupSkill::tagOccupSkill*>listOccStudySkill;
//获取当前玩家职业的已会技能列表,用sSkill返回值
void GetOccupStudySkill(listOccStudySkill& sSkill);

CEGUI::TabControl* GetTabControl();

//获取当前在ListBox中被选中条目索引
size_t GetCurSelItemInListBox(const CEGUI::String &Lisboxname,CEGUI::Window& LbParentWnd);

//处理mainbar快捷键和技能Icon的交换
void ProcessMainbarDCToSkillTemplate(CEGUI::Window* targetWnd,CEGUI::Window* sourceWnd);

//处理界面内的互相交换
void ProcessInternalDragDropped(CEGUI::Window* targetWnd,CEGUI::Window* sourceWnd);

CEGUI::Window* InitSkill()
{
#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("SkillLayout.log");
#endif
	
	CEGUI::Window* wnd = LoadUI(SKILL_PAGE_NAME);

#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("CEGUI.log",true);
#endif
	//界面初始状态设置
	SetInitializedSkillTemplateUIProperty(wnd);
	//事件注册
	SubscriberSkillTemplateEvent(wnd);

	//页面显示初始化,因为0-10为基本技能，所以加上一页的列表数目
	UpdateCanUseSkill(0,17);
	return wnd;
}
/////////////////////////////////////////////////
// zhaohang  2010/5/17 
// 界面状态初始化
void SetInitializedSkillTemplateUIProperty(CEGUI::Window* PageWnd)
{
	if(!PageWnd)
		return;
	PageWnd->setVisible(false);//这样设置是为了使第一次打开窗口时，使窗口可见
	//调整SkillIcon的大小
	for(int i = 0 ; i < (int)SKILL_PAGE_SIZE ; ++i)
	{
		char wndname[256];
		sprintf(wndname,"Skill/StaticText/StaticImage%d/DragContainer",i);
		CEGUI::Window* wnd = PageWnd->getChildRecursive(wndname);
		if(wnd)
		{
			wnd->setSize(CEGUI::UVector2(cegui_absdim(SKILL_ICON_SIZE + 0),cegui_absdim(SKILL_ICON_SIZE + 0)));
			wnd->setPosition(CEGUI::UVector2(cegui_absdim(5),cegui_absdim(5)));
		}
		sprintf(wndname,"Skill/StaticText/StaticImage%d",i);
		wnd = PageWnd->getChildRecursive(wndname);
		if(wnd)
		{
			wnd->setSize(CEGUI::UVector2(cegui_absdim(SKILL_ICON_SIZE + 10),cegui_absdim(SKILL_ICON_SIZE + 10)));
		}
	}
	ClearCanUseSkill();//清空已会技能
}
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// zhaohang  2010/5/17 
// 事件处理函数注册集合
void SubscriberSkillTemplateEvent(CEGUI::Window* PageWnd)
{
	if(!PageWnd)
		return ;
	PageWnd->subscribeEvent("AddCanUseSkill",CEGUI::Event::Subscriber(OnAddCanUseSkill));//添加已会技能

	CEGUI::PushButton* btn = WPushButton(PageWnd->getChildRecursive("Skill/PrePage"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnPageChanged));//已会技能翻页

	btn = WPushButton(PageWnd->getChildRecursive("Skill/NextPage"));
	btn ->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnPageChanged));//已会技能翻页

	btn = WPushButton(PageWnd->getChildRecursive("Skill/BottomText/BtnUpLv"));
	//btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnUpdateSkillLvBtnClicked));//技能升级按钮点击

	CEGUI::TabControl* tbc = GetTabControl();
	size_t TabPageNum = tbc->getTabCount();

	for(size_t i = 0 ; i < TabPageNum ; ++i)
	{
		char listboxname[256];
		sprintf(listboxname,"Skill/Tab/Tab %d/ListBox",i);
		CEGUI::Listbox* lb = WListBox(tbc->getChildRecursive(listboxname));
		if(lb)
		{
			//listBox selectItemChangeed
			lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(OnListboxSelChanged));
			CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("Skill_64")));
			item->setID(0);
			item->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			item->setSelected(true);
			lb->addItem(item);
		}
	}
	for(size_t i = 0 ; i < TabPageNum ; ++i)
	{
		char listboxname[256];
		sprintf(listboxname,"Skill/Tab/Tab %d/ListBox",i);
		CEGUI::Listbox* lb = WListBox(tbc->getChildRecursive(listboxname));
		if(lb)
		{
			//listBox selectItemChangeed
			lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(OnListboxSelChanged));
			CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("Skill_63")));
			item->setID(1);
			item->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			//item->setSelected(true);
			lb->addItem(item);
		}
	}

	//DragContainer 事件 
	char itemName[256];
	for( int i = 0 ; i < (int)SKILL_PAGE_SIZE ; ++i)
	{
		//组合名称
		sprintf(itemName,"Skill/StaticText/StaticImage%d",i);
		CEGUI::Window* dragItem = PageWnd->getChildRecursive(itemName);
		if(dragItem)
		{
			//dragItem->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,CEGUI::Event::Subscriber(OnSkillIconDragDropped));//拖拽释放
			//dragItem->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves,CEGUI::Event::Subscriber(OnSkillIconDragDropLeaves));//拖拽离开技能窗口
			//dragItem->subscribeEvent(CEGUI::Window::EventDragDropItemEnters,CEGUI::Event::Subscriber(OnSkillIconDragDropEnters));//拖拽进入mainbar窗口
		}
		//拖放到技能上已经有了DragItem的栏上处理(实际就是一个交换）
		sprintf(itemName,"Skill/StaticText/StaticImage%d/DragContainer",i);
		dragItem = PageWnd->getChildRecursive(itemName);
		if(dragItem)
		{
			dragItem ->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,CEGUI::Event::Subscriber(OnSkillIconDragDropped));//item互换
			dragItem->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves,CEGUI::Event::Subscriber(OnSkillIconDragDropLeaves));//mainbar的Dc拖拽离开事件
			//dragItem->subscribeEvent(CEGUI::Window::EventDragDropItemEnters,CEGUI::Event::Subscriber(OnSkillIconDragDropEnters));//拖拽进入mainbar窗口
		}
	}
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
// zhaohang  2010/5/17 
// 新加入技能图标拖拽相关事件处理

//技能图标拖动离开窗口了
bool OnSkillIconDragDropLeaves(const CEGUI::EventArgs& e)
{
	CEGUI::Window* targetDC = WEArgs(e).window;
	CEGUI::DragContainer* sourceDC = static_cast<const CEGUI::DragDropEventArgs&>(e).dragDropItem;
#ifdef _DEBUG
	const char* name = targetDC->getName().c_str();
#endif
	if(sourceDC->getName().find("MainBarPage") != CEGUI::String::npos)
	{
		CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
		if(pHand)
		{
			pHand->SetMainPlayerHandBySkill(sourceDC->getID());
		}
	}
	return true;
}

//技能图标拖动释放了
bool OnSkillIconDragDropped(const CEGUI::EventArgs& e)
{
	CEGUI::Window* targetWnd= WEArgs(e).window;
	CEGUI::DragContainer* dropWnd = static_cast<const CEGUI::DragDropEventArgs&>(e).dragDropItem;
#ifdef _DEBUG
	const char* tagetWndName = targetWnd->getName().c_str();
	const char* dragWndName = dropWnd->getName().c_str();
#endif 
	//if(dropWnd->getName().find(SKILL_PAGE_NAME) != CEGUI::String::npos)//根据名称判断是界面内Icon的交换
	//	//ProcessInternalDragDropped(targetWnd,dropWnd);//不做界面内交换处理
	/////////////////////////////////////////////////
	// zhaohang  2010/5/19 
	// 处理mianbar上技能拖到技能界面
	if( dropWnd->getName().find("MainBarPage") != CEGUI::String::npos)
		ProcessMainbarDCToSkillTemplate(targetWnd,dropWnd);
	/////////////////////////////////////////////////
	return true;
}


/////////////////////////////////////////////////

/////////////////////////////////////////////////
// zhaohang  2010/4/27 
// 事件响应函数
bool OnListboxSelChanged(const CEGUI::EventArgs& e)
{
	//如果选中的是所有技能，则因为0-10为基本技能，所以结束位置为加上一页的列表数目
	UpdateCanUseSkill(0,17);
	return true;
}

//bool OnUpdateSkillLvBtnClicked(const CEGUI::EventArgs& e)
//{
//	return true;
//}
bool OnPageChanged(const CEGUI::EventArgs& e)
{
	//const char* name = NULL;
	ClearCanUseSkill();
	CPlayer* player = GetGame()->GetMainPlayer();
	std::list<CPlayer::tagSkill>* skilllist = &player->GetSkillList();

	CEGUI::PushButton* btn = WPushButton(WEArgs(e).window);
	CEGUI::String prepgbtn = "Skill/PrePage" ;
	CEGUI::String nextpgbtn = "Skill/NextPage";
	CEGUI::String btnName = btn->getName();
	//计算页索引
	static int pageIdx = 0;
	int totalPage = (int)((float)skilllist->size() / SKILL_PAGE_SIZE );
	btnName == prepgbtn ?  pageIdx-- : (btnName == nextpgbtn ? pageIdx++ : pageIdx=0);
	if(pageIdx < 0 )
		pageIdx = 0;
	else if(pageIdx > totalPage)
		pageIdx = totalPage;
	UpdateCanUseSkill( int(pageIdx * SKILL_PAGE_SIZE),int((pageIdx + 1 ) * SKILL_PAGE_SIZE));
	return true;
}

bool OnAddCanUseSkill(const CEGUI::EventArgs& e)
{
//	CEGUI::Listbox* lb = WListBox(WEArgs(e).window);
//#ifdef _DEBUG
//	const char* name = lb->getName().c_str();
//#endif
	UpdateCanUseSkill(0,7);
	return false;
}
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// zhaohang  2010/4/27 
// 更新数据函数
void UpdateCentreMainIcon()
{
	CEGUI::Window* tabct = GetCurSelTabContent();
	if(!tabct)
		return;
}

void UpdateCustOnLvUp(CEGUI::Window* tabctlContent /* = NULL */)
{

}
void ClearCanUseSkill()
{
	for(float i= 0 ; i < SKILL_PAGE_SIZE ; ++i)
	{
		//SKILLIcon 控件名称
		char si[256];
		//SkiLLDes 控件名称
		char st[256];
		//SkillDragcontainer名称
		char dc[256];
		sprintf(si,"Skill/StaticText/StaticImage%d/DragContainer/sImage",(int)i);
		sprintf(st,"Skill/StaticText/StaticText%d",(int)i);
		sprintf(dc,"Skill/StaticText/StaticImage%d/DragContainer",(int)i);
		CEGUI::Window* siWnd = GetWndMgr().getWindow(si);
		siWnd->setProperty("Image","");
		siWnd->setID(0);
		CEGUI::Window* stWnd = GetWndMgr().getWindow(st);
		stWnd->setText("");
		stWnd->setID(0);
		CEGUI::Window* dcWnd = GetWndMgr().getWindow(dc);
		dcWnd->setID(0);
		dcWnd->setTooltipText("");
	}
}
void UpdateCanUseSkillByType(const uint startPos,const uint endPos,uint listitemID)
{
	if( listitemID == 0 )
	{
		listOccStudySkill os;
		GetOccupStudySkill(os);//获得当前职业下的已学技能
		//判断范围是否合法
		if(startPos >= os.size() || startPos >= endPos)
		{
//#ifdef _DEBUG
//			throw CEGUI::InvalidRequestException("UpdateCanUseSkillPage is out of range");
//#endif
			return;
		}
		//结束位置取较小值
		const size_t tempEndPos = min(endPos,os.size());
		//迭代器指向开始位置
		listOccStudySkill::iterator skilliter = os.begin();
		for(size_t i = 0 ; i <startPos ; ++i)
			++skilliter;
		for(uint i = 0 ; i < tempEndPos - startPos ; ++i ,++skilliter)
		{
			DWORD dwSkillID = (*skilliter)->lSkillID;
			UpdateRightUI(dwSkillID,i);
		}

	}
	else if( listitemID == 1)
	{
		//获取玩家技能列表
		CPlayer* player = GetGame()->GetMainPlayer();
		std::list<CPlayer::tagSkill>& skilllist = player->GetSkillList();
		//开始位置是否合法
		if(startPos >= skilllist.size() || startPos >= endPos )
		{
//#ifdef _DEBUG
//			throw CEGUI::InvalidRequestException("UpdateCanUseSkillPage is out of range");
//#endif
			return;
		}
		//结束位置取较小的
		const size_t tempEndPos = min(endPos,skilllist.size());
		std::list<CPlayer::tagSkill>::iterator skilliter = skilllist.begin();
		//迭代器指向开始位置
		for(size_t i = 0 ; i < startPos; ++i)
		{
			++skilliter;
		}
		int wndIdx = 0;
		for(uint i = 0 ; i < tempEndPos - startPos ; ++i,++skilliter)
		{
			DWORD dwSkillID = skilliter->dwID;
			// 属于基本攻击类技能
			if(dwSkillID <= SKILL_BASE_BLADEFIST)
			{
				dwSkillID = player->GetDefaultAttackSkillID();
				UpdateRightUI(dwSkillID,wndIdx,skilliter->wLevel);
				continue;
			}
			else{
				UpdateRightUI(dwSkillID,++wndIdx,skilliter->wLevel);
			}
		}
	}
}
void UpdateRightUI(const DWORD dwSkillID,const uint indexWnd,uint SkillLevel)
{
	char staticimage[256];
	char statictext[256];
	//char imagesetname[256];

	//组合窗口名称
	sprintf(staticimage,"Skill/StaticText/StaticImage%d/DragContainer",indexWnd);
	CEGUI::Window* dcWnd = GetWndMgr().getWindow(staticimage);
	//设置DCIcon并关联ID
	SetDCIconBySkillID(dwSkillID,dcWnd);

	//组合窗口名称
	sprintf(statictext,"Skill/StaticText/StaticText%d",indexWnd);
	CEGUI::Window* wndText = GetWndMgr().getWindow(statictext);
	//修改技能说明并关联ID
	SetSkillIconDesc(dwSkillID,wndText,SkillLevel);
}
void UpdateCanUseSkill(const uint startPos,const uint endPos)
{
	//清空已会SkillUI显示
	ClearCanUseSkill();

	CEGUI::Listbox* lb = GetListBoxInCurSelTabContent();
	if(!lb)
		return;
	CEGUI::ListboxItem* lbi = lb->getFirstSelectedItem();
	if(!lbi)
		return;
	uint id = lbi->getID();
	UpdateCanUseSkillByType(startPos,endPos,id);
}


void UpdateTabContent(CEGUI::Window* selTab,size_t idxInTabCtl)
{
	if(!selTab)
		return;
	char listboxname[256];
	sprintf(listboxname,"Skill/Tab/Tab %d/ListBox",idxInTabCtl+1);
	size_t selitem = GetCurSelItemInListBox(listboxname,*selTab);
}



/////////////////////////////////////////////////
// zhaohang  2010/4/27 
// 辅助函数
CEGUI::Listbox* GetListBoxInCurSelTabContent()
{
	CEGUI::TabControl* tabctrl  = GetTabControl();
	CEGUI::Window* tabcte = GetCurSelTabContent();
	char listbox[256];
	sprintf(listbox,"Skill/Tab/Tab %d/ListBox",tabctrl->getSelectedTabIndex());
	return WListBox(tabcte->getChildRecursive(listbox));
}
CEGUI::Window* GetCurSelTabContent()
{
	CEGUI::TabControl* tbc = GetTabControl();
	if(!tbc)
		return NULL;
	return tbc->getTabContentsAtIndex(tbc->getSelectedTabIndex());
}
void GetOccupStudySkill(listOccStudySkill& sSkill)
{
	sSkill.clear();
	CPlayer* player = GetGame()->GetMainPlayer();
	//获取玩家职业
	eOccupation eOccu = player->GetOccupation();
#ifdef _DEBUG
	const char* occuName = player->GetPlayerOccuType(eOccu);
#endif
	//根据职业获取对应技能列表
	COccupSkill *pOccupSkill = GetGame()->GetOccupSkillList()->GetOccupSkill(eOccu);
	//获取职业下面的技能数目
	size_t opslNum = pOccupSkill->GetOccupSkillCount();
	//遍历职业对应的技能是否在玩家已会技能列表里
	for(size_t size = 0; size < opslNum ; ++size)
	{
		uint ID = pOccupSkill->GetOccupSkill()[size]->lSkillID;
		if(ID > 0 && player->CheckSkill(ID) > 0 ) //有
		{
			sSkill.push_back(pOccupSkill->GetOccupSkill()[size]);
		}
	}
}
CEGUI::TabControl* GetTabControl()
{
	return WTabControl(GetWndMgr().getWindow("Skill/Tab"));
}
size_t GetCurSelItemInListBox(const CEGUI::String &Lisboxname,CEGUI::Window& LbParentWnd)
{
	CEGUI::Listbox* lb = WListBox(LbParentWnd.getChildRecursive(Lisboxname));
	if(!lb)
		return CEGUI::String::npos;
#ifdef _DEBUG
	const char* name = lb->getName().c_str();
#endif
	CEGUI::ListboxItem* lbi = lb->getFirstSelectedItem();
	if(lbi)
	{
#ifdef _DEBUG
		name = lbi->getText().c_str();
		CEGUI::uint ID =lbi->getID();
#endif
		return lb->getItemIndex(lbi);
	}
	else
		return lb->getItemCount();
}

/////////////////////////////////////////////////


/////////////////////////////////////////////////
// zhaohang  2010/5/20
// 
void ProcessMainbarDCToSkillTemplate(CEGUI::Window* targetWnd,CEGUI::Window* sourceWnd)
{
	if(!targetWnd || !sourceWnd)
		return;
	//清空mainbar的DC
	SetDCIconBySkillID(0,sourceWnd);
	//修改mainbar的快捷键
	// 设置快捷键
	uint hotKeyIdc = GetDCIndexInMainbar(sourceWnd);
	SkillLogicSys& SkillLS = GetInst(SkillLogicSys);
	SkillLS.Send_Set_Skill_HotKey(hotKeyIdc+SHORTCUT_COUNT,0);//0~11->F1~F12,所以加12
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();//手上的释放
	if(pHand)
	{
		pHand->SetMainPlayerHandBySkill(sourceWnd->getID());
	}
}
void ProcessInternalDragDropped(CEGUI::Window* targetWnd,CEGUI::Window* sourceWnd)
{
	if(!targetWnd || !sourceWnd)
		return;
	uint targetID = targetWnd->getID();
	uint sourceID = sourceWnd->getID();
	//交换
	SetDCIconBySkillID(targetID,sourceWnd);
	SetDCIconBySkillID(sourceID,targetWnd);
	//交换后修改技能文字说明
}
/////////////////////////////////////////////////
// zhaohang  2010/5/20 
// 
//由技能ID来设置DragContainer和其子控件StaticImage的显示(修改图标和设置控件ID与技能ID关联)
void SetDCIconBySkillID(uint skillID,CEGUI::Window* dragContainer)
{
	if(!dragContainer)
		return;
	char imagesetname[256];
	sprintf(imagesetname,SKILL_PREFIXID,skillID);
	CEGUI::String imagename = CEGUI::PropertyHelper::intToString(skillID)+".jpg";
	CEGUI::GUISheet* childImg = WGUISheet(dragContainer->getChildAtIdx(0));
	if(!childImg)
		return;
	if(skillID == 0)
	{
		childImg->setProperty("Image","");
		dragContainer->setTooltipText("");
	}
	else
	{
		SetBackGroundImage(childImg,imagesetname,SKILL_ICON_PATH,imagename.c_str());
		CSkillListXml::tagSkill* xmlSkill = CSkillListXml::GetProperty(skillID);
		if(!xmlSkill)
        {
            MsgEventManager& mgr = GetInst(MsgEventManager);
			char strTemp[256];
			sprintf(strTemp,AppFrame::GetText("SDCC_63"),skillID);
            mgr.PushEvent(Msg_Ok,strTemp);
            return;
        }
		CEGUI::String dest = xmlSkill->strSkillDesc.c_str();
		dragContainer->setTooltipText(dest);
	}
	//修改关联ID
	childImg->setID(skillID);
	dragContainer->setID(skillID);
}

//修改技能界面的Icon的文字说明
void SetSkillIconDesc(uint skillID,CEGUI::Window* descWnd,uint SkillLevel)
{
	if(!descWnd)
		return;
	CSkillListXml::tagSkill* xmlSkill = CSkillListXml::GetProperty(skillID);
	if(!xmlSkill)
		return;
	CEGUI::String dest = ToCEGUIString(xmlSkill->strSkillName.c_str());
	dest += CEGUI::PropertyHelper::intToString((skillID));
	descWnd->setText(dest);

	descWnd->setID(skillID);//控件ID和技能ID关联
}


/////////////////////////////////////////////////