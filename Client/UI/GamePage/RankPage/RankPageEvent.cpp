#include "stdafx.h"
#include "RankPageEvent.h"
#include "RankPage.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/RankSystem/RankSystem.h"
#include "../../UIDef.h"


RankPageEvent::RankPageEvent()
{
	m_wnd = NULL;
	SetDefaultCurRankID();
}

RankPageEvent::~RankPageEvent()
{

}

RankPageEvent::RankPageEvent(EVENTTYPE type)
{
	m_wnd = NULL;
	SetDefaultCurRankID();
	SetEventType(type);
}

void RankPageEvent::OnPageLoad(GamePage *pPage)
{
	 //加载UI
	pPage->LoadPageWindow();
	//保存UI根窗口
	m_wnd = pPage->GetPageWindow();
	CEGUI::DefaultWindow* wnd = static_cast<CEGUI::DefaultWindow*>(m_wnd->getChildRecursive("RankWnd/Editbox11"));
	//保存页数信息字符串
	m_strPageText = wnd->getText();
	/////////////////////////////////////////////////
	// zhaohang  2010/3/29 
	// 新的界面对应的修改
	 //初始化菜单
	 InitMenu();
	 //初始化各个MultiColumnList header
	 //InitMultiList();
	 UpdateMulitListHeader(m_curRankID);
	 //注册事件响应函数
	 SubscribeEvent();
	/////////////////////////////////////////////////

}

void RankPageEvent::OnPageOpen(GamePage *pPage)
{

}

void RankPageEvent::OnPageClose(GamePage *pPage)
{

}
void RankPageEvent::OnPageUpdate(GamePage *pPage)
{
	//更新页数信息
	UpdatePageNumInfo();
	RankMsgProc& msg = GetInst(RankMsgProc);
	UpdateMulitListHeader(msg.GetRankID());
	UpdateMulitListData(msg.GetRankID());
}
void RankPageEvent::OnPageRelease(GamePage *pPage)
{
	m_wnd = NULL;
}

 
void RankPageEvent::SubscribeEvent()
{
	/////////////////////////////////////////////////
	// zhaohang  2010/3/29 
	//注册查询上一页函数
	CEGUI::PushButton* pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/page"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlQueryPrePage,this));
	//注册查询下一页函数
	pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/page1"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlQueryNextPage,this));
	//注册查询我的排名
	pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/Query2"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlMyRank,this));
	//注册查询排名
	pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/Query"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlQueryRank,this));
	//查询名称
	pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/Query1"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlQueryName,this));

	//注册关闭按钮函数
	pbt = static_cast<CEGUI::PushButton*>(m_wnd->getChildRecursive("RankWnd/Query3"));
	pbt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&RankPageEvent::HandlClose,this));
	//TabControl事件注册
	CEGUI::TabControl* tbc = static_cast<CEGUI::TabControl*>(m_wnd->getChildRecursive("RankWnd/Tab"));
	tbc->subscribeEvent(CEGUI::TabControl::EventSelectionChanged,CEGUI::Event::Subscriber(&RankPageEvent::HandlTabSelChanged,this));
	//listboxItem selchanged 事件
	char temp[256];
	RankQueryConfig& rqf = GetInst(RankQueryConfig);
	for(size_t i=1; i <= rqf.GetMainMenuSize() ; ++i)
	{
		sprintf(temp,"RankWnd/Tab/%d/ListBox",i);
		CEGUI::Listbox* lb = GetListBox(temp);		
		if(lb)
			lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(&RankPageEvent::HandlListBoxSelChange,this));
	}
	/////////////////////////////////////////////////
}

bool RankPageEvent::HandlQueryRank(const CEGUI::EventArgs& e)
{
	CEGUI::Editbox* ed = GetEditBox("RankWnd/Editbox");
	if(ed)
	{
		RankMsgProc& rmp = GetInst(RankMsgProc);
		RankQueryConfig& rqc = GetInst(RankQueryConfig);
		rmp.Query(RT_RANK,m_curRankID,"","",CEGUI::PropertyHelper::stringToInt(ed->getText()));
	}
	return true;
}

bool RankPageEvent::HandlQueryName(const CEGUI::EventArgs& e)
{
	CEGUI::Editbox* ed = GetEditBox("RankWnd/Editbox1");
	if(ed)
	{
		RankMsgProc& rmp = GetInst(RankMsgProc);
		RankQueryConfig& rqc = GetInst(RankQueryConfig);
		CPlayer* pPlayer = GetGame()->GetMainPlayer();
		if(!pPlayer)
			return false;
		rmp.Query(RT_OWNER,m_curRankID,ed->getText().c_str(),
			rqc.GetSubMenuQueryContent(GetMainMenuIndexByRankID(m_curRankID),GetIndexInListBoxByRankID(m_curRankID)).c_str());
	}
	return true;
}

bool RankPageEvent::HandlQueryNextPage(const CEGUI::EventArgs& e)
{
	RankMsgProc& rmp = GetInst(RankMsgProc);
	RankQueryConfig& rqc = GetInst(RankQueryConfig);
	rmp.Query(RT_OTHER,m_curRankID,"",
		rqc.GetSubMenuQueryContent(GetMainMenuIndexByRankID(m_curRankID),GetIndexInListBoxByRankID(m_curRankID)).c_str(),
		rmp.GetCurPage()+1);
	return true;
}

bool RankPageEvent::HandlQueryPrePage(const CEGUI::EventArgs& e)
{
	RankMsgProc& rmp = GetInst(RankMsgProc);
	RankQueryConfig& rqc = GetInst(RankQueryConfig);
	rmp.Query(RT_OTHER,m_curRankID,"",
		rqc.GetSubMenuQueryContent(GetMainMenuIndexByRankID(m_curRankID),GetIndexInListBoxByRankID(m_curRankID)).c_str(),rmp.GetCurPage()-1);
	return true;
}

bool RankPageEvent::HandlMyRank(const CEGUI::EventArgs& e)
{
	RankMsgProc& msg = GetInst(RankMsgProc);
	RankQueryConfig& rqc = GetInst(RankQueryConfig);
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(!pPlayer)
		return false;
	//发送查询信息
	msg.Query(RT_OWNER,m_curRankID,pPlayer->GetName(),
		rqc.GetSubMenuQueryContent(GetMainMenuIndexByRankID(m_curRankID),GetIndexInListBoxByRankID(m_curRankID)).c_str());
	return true;
}

bool RankPageEvent::HandlListBoxSelChange(const CEGUI::EventArgs& e)
{
	CEGUI::Listbox* lbox = static_cast<CEGUI::Listbox*>(
		static_cast<const CEGUI::WindowEventArgs&>(e).window);
	CEGUI::ListboxItem* selitm = lbox->getFirstSelectedItem();
	if(selitm)
	{
		m_curRankID = selitm->getID();
		UpdateMulitListHeader(m_curRankID);
		UpdateMulitListData(m_curRankID);
	}
	return true;
}

bool RankPageEvent::HandlTabSelChanged(const CEGUI::EventArgs& e)
{
	CEGUI::TabControl* tbc = static_cast<CEGUI::TabControl*>(m_wnd->getChildRecursive("RankWnd/Tab"));
	char temp[256];
	sprintf(temp,"RankWnd/Tab/%d/ListBox",tbc->getSelectedTabIndex()+1);
	CEGUI::Listbox* lb = GetListBox(temp);
	if(!lb)
		return false;
	CEGUI::ListboxItem* lbi = lb->getFirstSelectedItem();
	if(!lbi)
		return false;
	//子菜单ID
	m_curRankID = lbi->getID(); 
	UpdateMulitListHeader(m_curRankID);
	UpdateMulitListData(m_curRankID);
	return true;
}

bool RankPageEvent::HandlClose(const CEGUI::EventArgs& e)
{
	GetInst(RankPage).Close();
	return true;
}

bool RankPageEvent::InitMenu()
{
	//读取菜单查询配置(菜单ID，和查询内容）
	RankQueryConfig &rqf = GetInst(RankQueryConfig);
	if(!rqf.ReadConfig())
	{
		throw CEGUI::InvalidRequestException("ERROR: File setup/RankSet.ini not found!");
		return false;
	}
	//添加菜单到对应的Tab控件
	for(size_t size = 0 ; size <  rqf.GetMainMenuSize(); ++size)
	{
		char tempLbName[256];
		//设置tab的lable
		sprintf(tempLbName,"RankWnd/Tab/%d",size+1);
		CEGUI::DefaultWindow* dw = static_cast<CEGUI::DefaultWindow*>(m_wnd->getChildRecursive(tempLbName));
		if(dw)
			//dw->setText(rqf.GetMainMenuName((uint)size));
			SetText(dw,rqf.GetMainMenuName((uint)size));
		sprintf(tempLbName,"RankWnd/Tab/%d/ListBox",size+1);
		CEGUI::Listbox* lb = GetListBox(tempLbName);
		if(lb)
		{
			for(size_t sz = 0 ; sz < rqf.GetSubMenuSize(size) ; ++sz)
			{
				CEGUI::ListboxTextItem* lbti = new CEGUI::ListboxTextItem(ToCEGUIString(rqf.GetSubMenuName(size,sz).c_str()));
				lbti->setSelectionColours(CEGUI::colour(255,255,0,255));
				lbti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
				lbti->setID(rqf.GetSubMenuID((uint)size,(uint)sz));
				if(sz == GetIndexInListBoxByRankID(m_curRankID))
					lbti->setSelected(true);
				lb->addItem(lbti);
			}
		}
	}
	return true;
}
/////////////////////////////////////////////////
// zhaohang  2010/3/26 
// 数据显示更新部分
void RankPageEvent::UpdatePageNumInfo()
{
	CEGUI::Editbox* ed = GetEditBox("RankWnd/Editbox11");
	if(!ed)
		return;
	char szText[256];
	RankMsgProc& msgPrc = GetInst(RankMsgProc);
	sprintf(szText,m_strPageText.c_str(),msgPrc.GetCurPage(),msgPrc.GetCurTotalPage());
	ed->setText(szText);
}

void RankPageEvent::UpdateMulitListHeader(uint rankID)
{
	RankMsgProc& msg = GetInst(RankMsgProc);
	RankQueryConfig& rqc = GetInst(RankQueryConfig);
	uint mainMenuID = GetMainMenuIDByRankID(rankID);
	char temp[256];
	sprintf(temp,"RankWnd/Tab/%d/MultiColumnList",mainMenuID);
	CEGUI::MultiColumnList* mcl = GetMultiColumnList(temp);
	//先移除列信息
	if(mcl)
	{
		uint colNum = mcl->getColumnCount();
		for(uint i = 0 ; i < colNum ; ++i)
			mcl->removeColumnWithID(i);
		//加入列表头
		for(size_t i = 0 ; i < rqc.GetSubMenuRowSize(GetMainMenuIndexByRankID(rankID),GetIndexInListBoxByRankID(rankID)) ; ++i)
		{
			std::string str = rqc.GetSubMenuRowHeader(GetMainMenuIndexByRankID(rankID),GetIndexInListBoxByRankID(rankID),i);
			mcl->addColumn(ToCEGUIString(str.c_str()),(uint)i,cegui_absdim(80));
		}
	}
}

void RankPageEvent::UpdateMulitListData(uint rankID)
{
	RankMsgProc& msg = GetInst(RankMsgProc);
	RankQueryConfig& rqc = GetInst(RankQueryConfig);
	uint mainMenuID = GetMainMenuIDByRankID(rankID);
	tagRankInfo& info = msg.GetRankInfo();

	//先移除所有row
	char temp[256];
	sprintf(temp,"RankWnd/Tab/%d/MultiColumnList",mainMenuID);
	CEGUI::MultiColumnList* mcl = GetMultiColumnList(temp);
	if(!mcl)
		return;
	mcl->resetList();
	mcl->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
	if(rankID != msg.GetRankID())//更新的目标rankID必须和消息的rankID匹配
		return;
	//一次性添加Row
	size_t rowSize = info.vecColData.size();
	for(size_t rs = 0 ; rs < rowSize ; ++rs)
		mcl->addRow();
	//先列后行，减少列的查询次数
	//有多少列
	//根据RankID查找对应的显示的列数配置
	size_t colsz = rqc.GetSubMenuRowSize(GetMainMenuIndexByRankID(rankID),GetIndexInListBoxByRankID(rankID));
	//记录当前排名和之前的排名，用于计算升降
	std::vector<long>curRank;
	long oldRank = 0;
	for(size_t csz = 0 ; csz < colsz ; ++csz)
	{
		//根据配置的显示列对应的关键字查找数据填充到MultiColumnList
		const std::string& strKey = rqc.GetSubMenuRowDataHeader(GetMainMenuIndexByRankID(rankID),GetIndexInListBoxByRankID(rankID),csz);
		//寻找表列在Col Vector中的位置
		long colpos = info.FindColPos(strKey.c_str());
		if(colpos >= 0)//有对应关键字的数据
		{
			//目前有三种类型，定义三种类型的临时变量
			std::string strData;
			long		lData = 0;
			float		fData = 0.0f;
			//填充数据,按列-行 来填充
			for(size_t rsz = 0 ; rsz < rowSize ; ++rsz)
			{
				//取得某行的数据
				CTableManager* pData = info.vecColData[rsz];
				//取得某行某列的数据
				CDataObject* pdo = pData->GetColumnData(colpos);
				//根据类型获取值
				const tagColInfo& tInfo = info.vecColInfo[colpos];
				switch(tInfo.dt)
				{
				case CT_BYTE:
				case CT_STRING:
					{
						strData = pdo->GetStringValue();
						CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(ToCEGUIString(strData.c_str()));
						/***********************************************************************/
						/* zhaohang fix 2010-9-3
						/* 修改设置图片方式
						/***********************************************************************/
						item->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
						mcl->setItem(item,(uint)csz,(uint)rsz);
					}
					break;
				case CT_LONG:
					lData = pdo->GetLongValue();
					//如果关键字是curRank或oldrank (关键字全是小写),就记录排名，用于计算升降
					if(strcmp(strKey.c_str(),"currank")==0)
					{
						curRank.push_back(lData);
						CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(curRank[rsz]));
						//另一种设置图片方式
						item->setSelectionBrushImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush"));
						mcl->setItem(item,(uint)csz,(uint)rsz);
					}
					else if(strcmp(strKey.c_str(),"oldrank")==0) //如果是之前排名就转化为升降值显示
					{
						oldRank = lData;
						long minus = oldRank - curRank[rsz];
						CEGUI::String str;
						if(minus > 0)
						{
							str = FormatText(CEGUI::PropertyHelper::intToString(minus),"[colour FFFF0000]");
						}
						else if(minus < 0 && oldRank !=0)
						{
							str = FormatText(CEGUI::PropertyHelper::intToString(minus),"[colour FF00FF00]");
						}
						else
							str = FormatText(CEGUI::String("-"),"[colour FFFFFF00]");
						CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(ToCEGUIString(str.c_str()));
						item->setSelectionBrushImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush"));
						mcl->setItem(item,(uint)csz,(uint)rsz);
					}
					break;
				case CT_FLOAT:
					pdo->GetBufValue((void*)&fData,sizeof(float));
					CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::floatToString(fData));
					item->setSelectionBrushImage(&CEGUI::ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush"));
					mcl->setItem(item,(uint)csz,(uint)rsz);
					break;
				}//switch
			}//for(size_t rsz = 0 ; rsz < rowSize ; ++rsz)
		}//if(colpos >= 0)
	}//for(size_t csz = 0 ; csz < colsz ; ++csz)
}

CEGUI::Listbox* RankPageEvent::GetListBox(const char* lbName)
{
	if(!lbName)
		return NULL;
	return static_cast<CEGUI::Listbox*>(m_wnd->getChildRecursive(lbName));
}
CEGUI::MultiColumnList* RankPageEvent::GetMultiColumnList(const char* Name)
{
	if(!Name)
		return NULL;
	return static_cast<CEGUI::MultiColumnList*>(m_wnd->getChildRecursive(Name));
}
CEGUI::Editbox* RankPageEvent::GetEditBox(const char* name)
{
	if(!name)
		return NULL;
	return static_cast<CEGUI::Editbox*>(m_wnd->getChildRecursive(name));
}

uint RankPageEvent::GetMainMenuIDByRankID(uint rankID)
{
	return rankID / 10 ;
}

uint RankPageEvent::GetMainMenuIndexByRankID(uint rankID)
{
	return GetMainMenuIDByRankID(rankID) - 1;
}

uint RankPageEvent::GetIndexInListBoxByRankID(uint rankID)
{
	return rankID % 10 - 1;
}

CEGUI::String& RankPageEvent::FormatText(CEGUI::String& str,const char* colour /* ="[colour FFFFFFFF]"  */)
{
	return str.insert(0,colour,strlen(colour));
}
/////////////////////////////////////////////////