#include "stdafx.h"
#include "../UIDef.h"
#include "TeamDef.h"
#include "TeamSetup.h"
#include "../../GameClient/ClientApp/Team/Team.h"

bool PopupTeamCaptainMenu(const CEGUI::EventArgs& e);

///使用纯代码方式创建界面
CEGUI::Window* AddTeamMember();
///
bool AdjustTeamPageSize(uint showCnt,CEGUI::Window* wnd);

///更新队伍的显示
bool UpdateTeamList(const CEGUI::EventArgs& e);

///增加队伍成员
CEGUI::Window* AddTeamMember();
///弹出右键菜单
bool PopupTeamCaptainMenu(const CEGUI::EventArgs& e);

CEGUI::Window* InitTeam()
{
	TeamSetup& tsini = GetInst(TeamSetup);
	tsini.ResetCurItemNum();
	if(!tsini.ReadSetup())
	{
		return NULL;
	}
	CEGUI::WindowManager& wndMgr = GetWndMgr();
	CEGUI::Window* root = wndMgr.getWindow("Root");
	CEGUI::Window* pageWnd = wndMgr.createWindow("GUISheet",TEAM_PAGE_NAME);
	root->addChildWindow(pageWnd);
	pageWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetPagePos().d_x),
		cegui_absdim(tsini.GetPagePos().d_y))
		);
	pageWnd->setSize(CEGUI::UVector2(cegui_absdim(0),cegui_absdim(0)));
	//事件注册
	pageWnd->subscribeEvent(EVENT_OPEN,
		CEGUI::Event::Subscriber(OnShown));
	pageWnd->subscribeEvent(EVENT_CLOSE,
		CEGUI::Event::Subscriber(OnHide));
	pageWnd->subscribeEvent(EVENT_SWITCH,
		CEGUI::Event::Subscriber(OnSwitch));

	pageWnd->subscribeEvent("UpdateTeam",CEGUI::Event::Subscriber(UpdateTeamList));
	for(int i = 0 ;  i < tsini.GetMaxItem() ; ++i)
		AddTeamMember();
	AdjustTeamPageSize(tsini.GetMaxItem(),pageWnd);
	//初始化不可见
	pageWnd->setVisible(false);
	pageWnd->notifyScreenAreaChanged(true);
	return pageWnd;
}

CEGUI::Window* AddTeamMember()
{
	TeamSetup& tsini = GetInst(TeamSetup);
	CEGUI::WindowManager& wndMgr = GetWndMgr();
	CEGUI::Window* pageWnd = wndMgr.getWindow(TEAM_PAGE_NAME);
	if(!pageWnd)
		return NULL;
	//AdjustTeamPageSize(tsini.GetCurItemNum()+1,pageWnd);
	/////////////////////////////////
	//添加ITem项，窗口层次结构如下
	// +TeamPageWnd
	// ----+ItemWnd
	//      -------+LevelWnd
	//      -------+HeadWnd
	//      -------+HpWnd
	//      -------+MpWnd
	//      -------+PlayerNameWnd
	////////////////////////////////////
	uint index = tsini.GetCurItemNum();
	//ItemWnd
	char ItemName[256];
	sprintf(ItemName,tsini.GetItemWndName(),index);
	CEGUI::GUISheet* itemWnd = WGUISheet(wndMgr.createWindow(tsini.GetItemWndType(),ItemName));
	itemWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	pageWnd->addChildWindow(itemWnd);
	itemWnd->setSize(tsini.GetItemSize());
	float y = ( tsini.GetItemHeight() + tsini.GetInterval()) * index;
	float x = tsini.GetPagePos().d_x;
	itemWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(0),
		cegui_absdim(y))
		);
	SetBackGroundImage(itemWnd,
		TEAM_HEAD_IMAGESET,
		TEAM_HEAD_PATH,
		"name.tga",false);
	//LevelWnd
	char LevelWnd[256];
	sprintf(LevelWnd,tsini.GetLevelWndName(),index);
	CEGUI::Window* levelWnd = wndMgr.createWindow(tsini.GetLevelWndType(),LevelWnd);
	itemWnd->addChildWindow(levelWnd);
	levelWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	levelWnd->setSize(tsini.GetLevelSize());
	levelWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetLevelPos().d_x ),
		cegui_absdim(tsini.GetLevelPos().d_y ))
		);

	//HeadWnd
	char HeadName[256];
	sprintf(HeadName,tsini.GetHeadWndName(),index);
	CEGUI::Window* headWnd = wndMgr.createWindow(tsini.GetHeadWndType(),HeadName);
	headWnd->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
		CEGUI::Event::Subscriber(PopupTeamCaptainMenu));//右键点击队伍界面头像,弹出菜单
	headWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	itemWnd->addChildWindow(headWnd);
	headWnd->setSize(tsini.GetHeadSize());
	headWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetHeadPos().d_x ),
		cegui_absdim(tsini.GetHeadPos().d_y ))
		);

	//HpWnd
	char hpName[256];
	sprintf(hpName,tsini.GetHPWndName(),index);
	CEGUI::Window* HpWnd = wndMgr.createWindow(tsini.GetHPWndType(),hpName);
	HpWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	itemWnd->addChildWindow(HpWnd);
	HpWnd->setSize(tsini.GetHpSize());
	HpWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetHpPos().d_x ),
		cegui_absdim(tsini.GetHpPos().d_y ))
		);

	//MpWnd
	char mpName[256];
	sprintf(mpName,tsini.GetMpWndName(),index);
	CEGUI::Window* MpWnd = wndMgr.createWindow(tsini.GetHPWndType(),mpName);
	MpWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	itemWnd->addChildWindow(MpWnd);
	MpWnd->setSize(tsini.GetMpSize());
	MpWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetMpPOs().d_x ),
		cegui_absdim(tsini.GetMpPOs().d_y ))
		);

	//PlayerNameWnd
	char player[256];
	sprintf(player,tsini.GetPlayerWndName(),index);
	CEGUI::Window* playerWnd = wndMgr.createWindow(tsini.GetPlayerWndType(),player);
	playerWnd->setID(index);//绑定ID，由ID关联队伍玩家索引
	itemWnd->addChildWindow(playerWnd);
	playerWnd->setSize(tsini.GetPlayerSize());
	playerWnd->setPosition(CEGUI::UVector2(
		cegui_absdim(tsini.GetPlayerPos().d_x ),
		cegui_absdim(tsini.GetPlayerPos().d_y ))
		);

	//增加Item计数
	tsini.CurItemNumAdded();
	return pageWnd;
}
bool AdjustTeamPageSize(uint showCnt,CEGUI::Window* wnd)
{
	//调整pagewnd的大小
	if(!wnd)
		return false;
	TeamSetup& tsini = GetInst(TeamSetup);
	CEGUI::UDim u1 = cegui_absdim(tsini.GetItemWidth());
	CEGUI::UDim u2 = cegui_absdim( (tsini.GetItemHeight() + tsini.GetInterval()) * 
		showCnt - tsini.GetInterval());
	CEGUI::UVector2 v2 = CEGUI::UVector2(u1,u2);
	wnd->setSize(v2);
	uint min = min(showCnt,wnd->getChildCount());
	for( uint i = 0 ; i < min ; ++i)
	{
		CEGUI::Window* itemWnd = wnd->getChildAtIdx(i);
#ifdef _DEBUG
		const char* showName = itemWnd->getName().c_str();
#endif
		if(itemWnd)
		{
			itemWnd->setVisible(true);
		}
	}
	//让其余child不可见
	for(uint i = showCnt ; i < wnd->getChildCount() ; ++i)
	{
		CEGUI::Window* itemWnd = wnd->getChildAtIdx(i);
#ifdef _DEBUG
		const char* name = itemWnd->getName().c_str();
#endif
		if(itemWnd)
		{
			itemWnd->setVisible(false);
		}
	}
	if(showCnt == GetInst(TeamData).GetSelMember())
	{
		FireUIEvent(TEAM_CAPTAINMENU_NAME,EVENT_CLOSE);
	}
	wnd->notifyScreenAreaChanged(true);
	return true;
}
bool UpdateTeamList(const CEGUI::EventArgs& e)
{
	TeamData& td = GetInst(TeamData);
	TeamSetup& tsini = GetInst(TeamSetup);

	CEGUI::Window* Wnd = WEArgs(e).window;

	CEGUI::ProgressBar* hpWnd = NULL;
	CEGUI::ProgressBar* mpWnd = NULL;
	CEGUI::Window*		playerNameWnd = NULL;
	CEGUI::Window*		LevelWnd = NULL;
	char format[256];
	char hpName[256]="";
	char mpName[256]="";
	char playerName[256]="";
	char level[256]="";
	TeamData::tagTeam* sT = NULL;
	uint max = min((uint)td.GetTeamInfo().size(),(uint)tsini.GetMaxItem());
	for (uint i= 0 ; i < max ; ++i)
	{
		sT = &td.GetTeamInfo()[i];

		//hp
		sprintf(hpName,tsini.GetHPWndName(),i);
		hpWnd = WProgress(Wnd->getChildRecursive(hpName));
		if(hpWnd)
		{
			sprintf(format,"%d/%d",sT->dwHp,sT->dwMaxHp);
			hpWnd->setText(format);
			float curVal = 0.0;
			if( sT->dwMaxHp != 0)
			{
				curVal = (float) sT->dwHp / (float) sT->dwMaxHp;
			}
			hpWnd->setProgress(curVal);
		}
		//mp
		sprintf(mpName,tsini.GetMpWndName(),i);
		mpWnd = WProgress(Wnd->getChildRecursive(mpName));
		if( mpWnd )
		{
			sprintf(format,"%d/%d",sT->dwMp,sT->dwMaxMp);
			mpWnd->setText(format);
			float curVal = 0.0;
			if( sT->dwMaxMp != 0)
			{
				curVal = (float) sT->dwMp / (float) sT->dwMaxMp;
			}
			mpWnd->setProgress(curVal);
		}
		//playerName
		sprintf(playerName,tsini.GetPlayerWndName(),i);
		playerNameWnd = Wnd->getChildRecursive(playerName);
		if(playerNameWnd)
		{
			playerNameWnd->setText(sT->strPlayerName.c_str());
		}
		//Levle
		sprintf(level,tsini.GetLevelWndName(),i);
		LevelWnd = Wnd->getChildRecursive(level);
		if(LevelWnd)
		{
			LevelWnd->setText(CEGUI::PropertyHelper::intToString(sT->lLev));
		}
	}
	Wnd->setVisible(true);
	AdjustTeamPageSize(max,Wnd);
	return true;
}

bool PopupTeamCaptainMenu(const CEGUI::EventArgs& e)
{
	TeamData& td = GetInst(TeamData);
	CEGUI::Window* wnd = WEArgs(e).window;
	td.SetSelMember(wnd->getID());
	FireUIEvent(TEAM_CAPTAINMENU_NAME,EVENT_OPEN);
	return true;
}