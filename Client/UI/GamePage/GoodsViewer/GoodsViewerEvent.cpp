#include "stdafx.h"
#include "GoodsViewerEvent.h"
#include "../../GamePage.h"
#include "../../../Engine/Render.h"
#include "GoodsRender.h"
using namespace CEGUI;

GVEvent::GVEvent()
{
	m_wnd = NULL ;
	m_RootItem = NULL;
	m_GoodsTree = NULL;

	m_xMove = 0;
	m_yMove = 0;
	m_zMove = 0;

	m_xRot  = 0;
	m_yRot  = 0; 
	m_zRot  = 0;
}

GVEvent::~GVEvent()
{
	m_wnd = NULL;
	m_RootItem = NULL;
	m_GoodsTree = NULL;
	/*for(size_t size = 0 ;size < m_ChildRootItem.size(); ++size)
	{
		if(m_ChildRootItem[size] != NULL)
			delete m_ChildRootItem[size];
	}*/
	m_ChildRootItem.clear();
}

void GVEvent::OnPageLoad(GamePage *pPage)
{
	pPage->LoadPageWindow();
	
	//得到Tree控件指针
	m_wnd = pPage->GetPageWindow();
	m_GoodsTree = static_cast<CEGUI::Tree*>(m_wnd->getChildRecursive("GoodsTreeFrame/Tree"));
	if(m_GoodsTree)
		m_GoodsTree->initialise();
	//是否存数据库Combox条目的初始化
	InitIsDBCombobox();
	//物品类型条目的初始化
	InitGoodTypeCombobox();
	//攻击是是否混音Combobox条目的初始化
	InitIsSoundSwitchCombobox();
	//附加属性Combobox条目初始化
	m_GoodsOP.LoadGoodsItemTypeData("data/itemtype1.dat");//先要读取itemtype.dat文件
	InitAddAttrCombobox();
	//初始化附加属性值的Combobox(是否有效，是否隐藏)
	InitAddAttrValueSelCombobox();
	//初始化套装属性Combobox（套装名称)
	InitSuitAttrCombobox();

	//设置模型控制滚动条位置
	ReSetScrollPos();
	//注册事件响应
	//.............
	m_GoodsTree->subscribeEvent(CEGUI::Tree::EventSelectionChanged,CEGUI::Event::Subscriber(&GVEvent::OnBranchSelectChanged,this));//单击TreeItem事件

	CEGUI::MultiColumnList* mclbox = static_cast<CEGUI::MultiColumnList*>(m_wnd->getChildRecursive("GoodsTreeFrame/AddProperty/MultiColumnList"));
	mclbox->subscribeEvent(MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(&GVEvent::OnAddAttrItemSelChanged,this));//附加属性条目选中事件

	mclbox = static_cast<CEGUI::MultiColumnList*>(m_wnd->getChildRecursive("GoodsTreeFrame/GroupBoxSuitAtrr/MultiColumnList"));
	mclbox->subscribeEvent(MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(&GVEvent::OnSuitAttrItemSelChanged,this));//套装属性条目选中事件

	/////////////////////////////////////////////////
	// zhaohang  2010/3/18 
	// 新的物品观察控制注册事件响应
	/////////////////////////////////////////////////
	m_wnd->getChild("GoodsTreeFrame/Scal")->subscribeEvent(Scrollbar::EventScrollPositionChanged,CEGUI::Event::Subscriber(&GVEvent::HandScal,this));
	m_wnd->getChild("GoodsTreeFrame/XMove")->subscribeEvent(Scrollbar::EventScrollPositionChanged,Event::Subscriber(&GVEvent::HandXMove,this));
	m_wnd->getChild("GoodsTreeFrame/YMove")->subscribeEvent(Scrollbar::EventScrollPositionChanged,Event::Subscriber(&GVEvent::HandYMove,this));
	m_wnd->getChild("GoodsTreeFrame/XRot")->subscribeEvent(Scrollbar::EventScrollPositionChanged,Event::Subscriber(&GVEvent::HandXRot,this));
	m_wnd->getChild("GoodsTreeFrame/YRot")->subscribeEvent(Scrollbar::EventScrollPositionChanged,Event::Subscriber(&GVEvent::HandYRot,this));
	m_wnd->getChild("GoodsTreeFrame/ZRot")->subscribeEvent(Scrollbar::EventScrollPositionChanged,Event::Subscriber(&GVEvent::HandZRot,this));
	//添加RootItem到Tree
	m_RootItem = AddItemToTree("AllGoods");

	if(m_GoodsOP.LoadGoodsListData("data/goodslist1.dat") == true) //读取Goods数据，并填充到TreeItem
	{
		size_t GoodsTotalNum = m_GoodsOP.GetGoodsTotalNum();
		for(size_t size = 0; size < GoodsTotalNum ; ++size)
		{
			const UIData::tagGoodAttr *GoodAttr = m_GoodsOP.GetGoodAttrByIndex(size);
			if(GoodAttr != NULL)
			{
				CEGUI::TreeItem* Item = AddItemToRootItem(GoodAttr->strOrigName.c_str());
				if(Item != NULL)
					m_ChildRootItem.push_back(Item);
			}
		}
	}
	m_RootItem->toggleIsOpen();//默认展开Tree
}

void GVEvent::OnPageOpen(GamePage *pPage)
{

}

void GVEvent::OnPageClose(GamePage *pPage)
{

}

void GVEvent::OnPageUpdate(GamePage *pPage)
{

}

void GVEvent::OnPageRelease(GamePage *pPage)
{

}


/////////////////////////////////////////////////
// zhaohang  2010/3/15 
//控件数据初始化function和辅助function
void GVEvent::InitIsDBCombobox()
{
	//是否存数据库Combox条目的初始化
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombBoxIsDB");
	ListboxTextItem* itm;
	itm = new ListboxTextItem("True",0);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("False",1);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
}

void GVEvent::InitGoodTypeCombobox()
{
	//物品类型条目的初始化
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombboxGoodType");
	ListboxTextItem* itm = NULL;
	for(int i = 0 ; i < UIData::gGoodsTypeNum ; ++i)
	{
		itm = new ListboxTextItem(m_GoodsOP.GetGoodsType(i).c_str(),i);
		itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
		cbbo->addItem(itm);
	}
}

void GVEvent::InitIsSoundSwitchCombobox()
{
	//攻击时是否混音
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombBoxIsDB1");
	ListboxTextItem* itm;
	itm = new ListboxTextItem("True",0);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("False",1);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
}

void GVEvent::InitAddAttrCombobox()
{
	//附加属性
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox");
	ListboxTextItem* itm;
	for(size_t i = 0 ; i < m_GoodsOP.GetGoodItemTypeSize(); ++i)
	{
		itm = new ListboxTextItem(m_GoodsOP.GetGoodItemType(i).c_str(),(CEGUI::uint)i);
		itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
		cbbo->addItem(itm);
	}
}

void GVEvent::InitAddAttrValueSelCombobox()
{
	//是否有效
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox1");
	ListboxTextItem* itm;
	itm = new ListboxTextItem("True",0);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("False",1);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
	//是否隐藏
	cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox11");
	itm = new ListboxTextItem("True",0);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("False",1);
	itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
	cbbo->addItem(itm);
}

void GVEvent::InitSuitAttrCombobox()
{
	//套装属性
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/GroupBoxSuitAtrr/CombBoxSuitAttr");
	ListboxTextItem* itm;
	for(size_t i = 0 ; i < m_GoodsOP.GetGoodItemTypeSize(); ++i)
	{
		itm = new ListboxTextItem(m_GoodsOP.GetGoodItemType(i).c_str(),(CEGUI::uint)i);
		itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
		cbbo->addItem(itm);
	}
}

void GVEvent::ReSetScrollPos()
{
	Scrollbar *sb = static_cast<Scrollbar*>(m_wnd->getChild("GoodsTreeFrame/XMove"));
	DWORD width = render::Interface::GetInstance()->GetWndWidth();
	sb->setDocumentSize((float)width);
	sb->setScrollPosition(float(width/2 + DEFAULT_XTRANS));

	sb = static_cast<Scrollbar*>(m_wnd->getChild("GoodsTreeFrame/YMove"));
	DWORD height = render::Interface::GetInstance()->GetWndHeight();
	sb->setDocumentSize((float)height);
	sb->setScrollPosition(float(height/2 + DEFAULT_YTRANS));

	sb = static_cast<Scrollbar*>(m_wnd->getChild("GoodsTreeFrame/XRot"));
	sb->setDocumentSize(360);
	sb->setScrollPosition(180);

	sb = static_cast<Scrollbar*>(m_wnd->getChild("GoodsTreeFrame/YRot"));
	sb->setDocumentSize(360);
	sb->setScrollPosition(180);

	sb = static_cast<Scrollbar*>(m_wnd->getChild("GoodsTreeFrame/ZRot"));
	sb->setDocumentSize(360);
	sb->setScrollPosition(180);
}
CEGUI::TreeItem* GVEvent::AddItemToTree(const CEGUI::String& ItemStr)
{
	if(m_GoodsTree != NULL)
	{
		TreeItem *Item = new TreeItem(ItemStr);
		Item->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		m_GoodsTree->addItem(Item);
		return Item;
	}
	else
		return NULL;
}

CEGUI::TreeItem* GVEvent::AddItemToRootItem(const CEGUI::String& ItemStr)
{
	if(m_RootItem != NULL)
	{
		TreeItem *Item = new TreeItem(ItemStr);
		Item->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		m_RootItem->addItem(Item);
		return Item;
	}
	else
		return NULL;
}

CEGUI::Editbox* GVEvent::GetEditbox(const char *WindowName)
{
	return static_cast<CEGUI::Editbox*>(m_wnd->getChildRecursive(WindowName));
}

CEGUI::Combobox* GVEvent::GetCombobox(const char *WindowName)
{
	return static_cast<CEGUI::Combobox*>(m_wnd->getChildRecursive(WindowName));
}
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// zhaohang  2010/3/15 
// GoodsViewer界面控件的事件响应函数

bool GVEvent::OnBranchSelectChanged(const CEGUI::EventArgs &args)
{
	const TreeEventArgs& treeArgs = static_cast<const TreeEventArgs&>(args);
	TreeItem *selItem = treeArgs.treeItem;
	if(selItem)
	{
		const String &str = selItem->getTextVisual();
		const UIData::tagGoodAttr *goodAttr = m_GoodsOP.GetGoodAttrByOgriName(str);
		if(goodAttr)
		{
			SetItemContentInMulList(goodAttr,0);
			//更新基本属性
			UpdateBaseAttrDisplay(goodAttr);
			//更新客户端属性
			UpdateClientAttrDisplay(goodAttr);
			//更新附加属性
			UpdateAddAttrDisplay(goodAttr);
			//更新套装属性
			UpdateSuitAttrDisplay(goodAttr);
			//更新模型控制滚动条
			ReSetScrollPos();
			String strModelGrapID = PropertyHelper::intToString(goodAttr->dwIconId);
			string strModelName = strModelGrapID.c_str();
			strModelName.insert(0,"\\");
			GetInst(GoodsRender).LoadModel(goodAttr->dwIconId);
		}
	}
	return true;
}

bool GVEvent::OnAddAttrItemSelChanged(const CEGUI::EventArgs &args)
{
	//处理附加属性条目选中消息
	MultiColumnList* mcl = static_cast<MultiColumnList*>(m_wnd->getChildRecursive("GoodsTreeFrame/AddProperty/MultiColumnList"));
	MyListItem* lti = static_cast<MyListItem*>(mcl->getFirstSelectedItem());
	if(!lti)
	{
		//更新对应的控件
		//是否有效
		Combobox* cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox1");
		cbbo->setItemSelectState(1,false);
		cbbo->setItemSelectState((size_t)0,false);
		//是否隐藏
		cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox11");
		cbbo->setItemSelectState(1,false);
		cbbo->setItemSelectState((size_t)0,false);
		//值1
		Editbox* edbox = GetEditbox("GoodsTreeFrame/AddProperty/EditBoxValue1");
		edbox->setText("");
		//值2
		edbox = GetEditbox("GoodsTreeFrame/AddProperty/EditBoxValue11");
		edbox->setText("");
		return false;
	}
	//获取选中的附加属性名称
	const String &strAttrName = lti->getText();
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	//获取选中的附加属性是否有效
	const String &strEnable = lti->getText();
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	//是否隐藏
	const String &strHide = lti->getText();
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	//值1
	const String &strValue1 = lti->getText();
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	//值2
	const String &strValue2 = lti->getText();

	//更新对应的控件
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox1");
	bool b = PropertyHelper::stringToBool(strEnable);
	//是否有效
	if( b == false)
		cbbo->setItemSelectState(1,true);
	else
		cbbo->setItemSelectState((size_t)0,true);
	b = PropertyHelper::stringToBool(strHide);
	cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox11");
	//是否隐藏
	if(b == false)
		cbbo->setItemSelectState(1,true);
	else
		cbbo->setItemSelectState((size_t)0,true);
	//值1
	Editbox* edbox = GetEditbox("GoodsTreeFrame/AddProperty/EditBoxValue1");
	edbox->setText(strValue1);
	//值2
	edbox = GetEditbox("GoodsTreeFrame/AddProperty/EditBoxValue11");
	edbox->setText(strValue2);
	return true;
}

bool GVEvent::OnSuitAttrItemSelChanged(const CEGUI::EventArgs &args)
{
	//处理套装属性条目选中消息
	MultiColumnList* mcl = static_cast<MultiColumnList*>(m_wnd->getChildRecursive("GoodsTreeFrame/GroupBoxSuitAtrr/MultiColumnList"));
	MyListItem* lti = static_cast<MyListItem*>(mcl->getFirstSelectedItem());
	if(!lti)
	{
		//更新对应的控件
		//值1
		Editbox* edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox");
		edbox->setText("");
		//值2
		edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox1");
		edbox->setText("");
		//套装件数
		edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox2");
		edbox->setText("");
		return false;
	}
	//获取选中的套装属性名称
	const String &strAttrName = lti->getText();
	//值1
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	const String &strValue1 = lti->getText();
	//值2
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	const String &strValue2 = lti->getText();
	//套装件数
	lti = static_cast<MyListItem*>(mcl->getNextSelected(lti));
	const String &strSuitNum = lti->getText();

	//更新对应的控件
	//值1
	Editbox* edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox");
	edbox->setText(strValue1);
	//值2
	edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox1");
	edbox->setText(strValue2);
	//套装件数
	edbox = GetEditbox("GoodsTreeFrame/GroupBoxSuitAtrr/EditBox2");
	edbox->setText(strSuitNum);
	return true;
}

bool GVEvent::HandScal(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	float scal = sb->getScrollPosition() / sb->getDocumentSize() * DEFAULT_SCALE ;
	GetInst(GoodsRender).ResetScal(scal);
	return true;
}
bool GVEvent::HandXMove(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	static float xoldPos= DEFAULT_XTRANS;
	float curmove = sb->getScrollPosition() - xoldPos;
	xoldPos = sb->getScrollPosition();
	GetInst(GoodsRender).SetTranAdd(curmove,0,0);
	return true;
}

bool GVEvent::HandYMove(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	static float yoldPos= DEFAULT_YTRANS;
	float curmove = sb->getScrollPosition() - yoldPos;
	yoldPos = sb->getScrollPosition();
	GetInst(GoodsRender).SetTranAdd(0,curmove,0);
	return true;
}

bool GVEvent::HandXRot(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	static float oldpos = 180;
	float curval = (int)sb->getScrollPosition() - oldpos;
	oldpos = sb->getScrollPosition();
	GetInst(GoodsRender).ReSetRotAngle(D3DXToRadian(curval),D3DXToRadian(0),D3DXToRadian(0));
	return true;
}

bool GVEvent::HandYRot(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	static float oldpos = 180;
	float curval = sb->getScrollPosition() - oldpos;
	oldpos = sb->getScrollPosition();
	GetInst(GoodsRender).ReSetRotAngle(D3DXToRadian(0),D3DXToRadian(curval),D3DXToRadian(0));
	return true;
}

bool GVEvent::HandZRot(const CEGUI::EventArgs& args)
{
	Scrollbar *sb = static_cast<Scrollbar*>( 
		static_cast<const WindowEventArgs&> (args).window);
	static float oldpos = 180;
	float curval = sb->getScrollPosition() - oldpos;
	oldpos = sb->getScrollPosition();
	GetInst(GoodsRender).ReSetRotAngle(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(curval));
	return true;
}
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// zhaohang  2010/3/16 
// 控件数据显示更新 
void GVEvent::SetItemContentInMulList(const UIData::tagGoodAttr *pGoodAttr, CEGUI::uint RowIdx)
{
	if(pGoodAttr!=NULL)
	{
		MultiColumnList* mclbox = static_cast<MultiColumnList*>(m_wnd->getChildRecursive("GoodsTreeFrame/MultiColumnList"));
		//清空行
		mclbox->resetList();
		//设置每一行的值
		mclbox->addRow();
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwIndex)),0,RowIdx);
		mclbox->setItem(new MyListItem(pGoodAttr->strOrigName.c_str()),1,RowIdx);
		mclbox->setItem(new MyListItem(pGoodAttr->strName.c_str()),2,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::boolToString(pGoodAttr->bSave)),3,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwIconId)),4,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwGroundId)),5,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwEquipID)),6,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwValue)),7,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSilverValue)),8,RowIdx);
		mclbox->setItem(new MyListItem(m_GoodsOP.GetGoodsType(pGoodAttr->dwType).c_str()),9,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSound)),10,RowIdx);
		mclbox->setItem(new MyListItem(pGoodAttr->strContent.c_str()),11,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSoundID1)),12,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSoundID2)),13,RowIdx);
		mclbox->setItem(new MyListItem(PropertyHelper::boolToString(pGoodAttr->bSoundSwitch)),14,RowIdx);
	}
}

void GVEvent::UpdateBaseAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
{
	if(!pGoodAttr)
		return;
	//物品ID
	Editbox *pEditBox = GetEditbox("GoodsTreeFrame/BaseProperty/EditIndex");
	if(pEditBox)
	{
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwIndex));
	}
	//物品名称
	pEditBox = GetEditbox("GoodsTreeFrame/BaseProperty/EditDisName");
	if(pEditBox)
	{
		//std::wstring wstr());
		pEditBox->setText(pGoodAttr->strName.c_str());
	}
	//物品价格
	pEditBox = GetEditbox("GoodsTreeFrame/BaseProperty/EditPrice");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwValue));
	//物品原始名称
	pEditBox = GetEditbox("GoodsTreeFrame/BaseProperty/EditOgriName");
	if(pEditBox)
		pEditBox->setText(pGoodAttr->strOrigName.c_str());
	//物品是否存数据库
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombBoxIsDB");
	if(pGoodAttr->bSave == false)
		cbbo->setItemSelectState(1,true);
	else
		cbbo->setItemSelectState(size_t(0),true);
	//物品类型
	cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombboxGoodType");
	cbbo->setItemSelectState(pGoodAttr->dwType,true);
}

void GVEvent::UpdateClientAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
{
	if(!pGoodAttr)
		return;
	//客户端界面图形ID
	Editbox* pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwIconId));
	//落地图形ID
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID1");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwGroundId));
	//换装图形ID
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID2");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwEquipID));
	//拾取声音ID
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID3");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwSound));
	//挥动弱势伤害声音ID
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID4");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwSoundID1));
	//特殊击中被击中声音ID
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID5");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwSoundID2));
	//武器动作类型
	pEditBox = GetEditbox("GoodsTreeFrame/ClientProperty/EditGraID51");
	if(pEditBox)
		pEditBox->setText(PropertyHelper::intToString(pGoodAttr->dwWeaponActType));
	//攻击时是否混音
	Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombBoxIsDB1");
	if(pGoodAttr->bSoundSwitch == false)
		cbbo->setItemSelectState(1,true);
	else
		cbbo->setItemSelectState(size_t(0),true);
	//描述
	MultiLineEditbox* muleditbox = static_cast<MultiLineEditbox*>(m_wnd->getChildRecursive("GoodsTreeFrame/ClientProperty/Content"));
	muleditbox->setText(pGoodAttr->strContent.c_str());
}

void GVEvent::UpdateAddAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
{
	//更新物品附加属性显示
	if(!pGoodAttr)
		return;
	MultiColumnList* mclbox = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/AddProperty/MultiColumnList"));
	if(!mclbox)
		return;
	//清空行
	mclbox->resetList();
	for(size_t size = 0 ; size < pGoodAttr->vecAddAttr.size(); ++size)
	{
		//设置每一行的值
		mclbox->addRow();
		const UIData::tagAddAttr &addAttr = pGoodAttr->vecAddAttr[size];
		mclbox->setItem(new MyListItem(m_GoodsOP.GetGoodItemType(addAttr.wType).c_str()),0,(CEGUI::uint)size);
		mclbox->setItem(new MyListItem(PropertyHelper::boolToString(addAttr.bEnable)),1,(CEGUI::uint)size);
		mclbox->setItem(new MyListItem(PropertyHelper::boolToString(addAttr.bHide)),2,(CEGUI::uint)size);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(addAttr.lValue1)),3,(CEGUI::uint)size);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(addAttr.lValue2)),4,(CEGUI::uint)size);
	}
}

void GVEvent::UpdateSuitAttrDisplay(const UIData::tagGoodAttr *pGoodAttr)
{
	//更新套装属性
	if(!pGoodAttr)
		return;
	MultiColumnList* mclbox = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/GroupBoxSuitAtrr/MultiColumnList"));
	if(!mclbox)
		return;
	//清空行
	mclbox->resetList();
	for(CEGUI::uint i = 0 ; i < pGoodAttr->vecSuitAttr.size() ; ++i)
	{
		//设置每一行的值
		mclbox->addRow();
		const UIData::tagSuitAttr& suitAttr = pGoodAttr->vecSuitAttr[i];
		mclbox->setItem(new MyListItem(m_GoodsOP.GetGoodItemType(suitAttr.wType).c_str()),0,i);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.lValue1)),1,i);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.lValue2)),2,i);
		mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.wSuitNum)),3,i);
	}
}
/////////////////////////////////////////////////