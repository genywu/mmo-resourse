#include "stdafx.h"
#include "GoodsTree.h"
#include "../GoodsModelManager.h"
#include "../../CEGUIEngine/CEGUIManager.h"

namespace UIClass
{
	const unsigned int GoodsTree::s_TreeID = 1;
	const unsigned int GoodsTree::s_ListRowNum = 15;
	UIData::GoodsOperate GoodsTree::s_GoodsOP;
#define IMAGES_FILE_NAME		"TaharezLook"
#define BRUSH_NAME				"TextSelectionBrush"
#define MULTILISTSEL_BRUSH_NAME "MultiListSelectionBrush"
#define MODEL_NAME				"\\model"
	GoodsTree::GoodsTree()
	{
		m_GoodsTree = NULL;
		m_RootItem = NULL;
		m_arrListItem = NULL;
	}

	GoodsTree::~GoodsTree()
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(m_RootWindow);
		//如果采用一次填充所有数据方式
		if(m_arrListItem !=NULL)
			delete [] m_arrListItem;
		for(size_t size = 0 ;size < m_ChildRootItem.size(); ++size)
		{
			if(m_ChildRootItem[size] != NULL)
				delete m_ChildRootItem[size];
		}
		m_ChildRootItem.clear();
	}
	bool GoodsTree::Init()
	{
		WindowManager& WndMgr = WindowManager::getSingleton();
		m_RootWindow = WndMgr.loadWindowLayout("GoodsTree.layout");
		//设置为手动销毁
		m_RootWindow->setDestroyedByParent(false);
		//挂载到CEGUImanager创建的底版（根窗口上）
		CEGUIManager::GetInstance()->GetRootWindow()->addChildWindow(m_RootWindow);

		//Tree的初始化
		m_GoodsTree = (Tree*)m_RootWindow->getChild(s_TreeID);
		m_GoodsTree->initialise();
		//是否存数据库Combox条目的初始化
		InitIsDBCombobox();
		//物品类型条目的初始化
		InitGoodTypeCombobox();
		//攻击是是否混音Combobox条目的初始化
		InitIsSoundSwitchCombobox();
		//附加属性Combobox条目初始化
		s_GoodsOP.LoadGoodsItemTypeData();//先要读取文件
		InitAddAttrCombobox();
		//初始化附加属性值的Combobox(是否有效，是否隐藏)
		InitAddAttrValueSelCombobox();
		//初始化套装属性Combobox（套装名称)
		InitSuitAttrCombobox();
		//注册事件响应
		//.............
		m_GoodsTree->subscribeEvent(Tree::EventSelectionChanged,Event::Subscriber(&GoodsTree::OnBranchSelectChanged,this));//单击TreeItem事件

		MultiColumnList* mclbox = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/AddProperty/MultiColumnList"));
		mclbox->subscribeEvent(MultiColumnList::EventSelectionChanged,Event::Subscriber(&GoodsTree::OnAddAttrItemSelChanged,this));//附加属性条目选中事件

		mclbox = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/GroupBoxSuitAtrr/MultiColumnList"));
		mclbox->subscribeEvent(MultiColumnList::EventSelectionChanged,Event::Subscriber(&GoodsTree::OnSuitAttrItemSelChanged,this));//套装属性条目选中事件
		//添加RootItem到Tree
		m_RootItem = AddItemToTree(String("AllGoods"));
		
		if(s_GoodsOP.LoadGoodsListData() == true) //读取Goods数据，并填充到TreeItem
		{
			size_t GoodsTotalNum = s_GoodsOP.GetGoodsTotalNum();

			//如果使用一次填充所有数据，则在堆上分配多行列表框的单元格
			//m_arrListItem = new MyListItem[s_ListRowNum * GoodsTotalNum];

			for(size_t size = 0; size < GoodsTotalNum ; ++size)
			{
				const UIData::tagGoodAttr *GoodAttr = s_GoodsOP.GetGoodAttrByIndex(size);
				if(GoodAttr != NULL)
				{
					TreeItem* Item = AddItemToRootItem(GoodAttr->strOrigName);
					if(Item != NULL)
						m_ChildRootItem.push_back(Item);
					//添加所有Item到MultiList中
					//SetItemContentInMulList(GoodAttr,(CEGUI::uint)size);
				}
			}
		}
		m_RootItem->toggleIsOpen();//默认展开Tree
		return true;
	}

	void GoodsTree::InitIsDBCombobox()
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
	void GoodsTree::InitGoodTypeCombobox()
	{
		//物品类型条目的初始化
		Combobox* cbbo = GetCombobox("GoodsTreeFrame/BaseProperty/CombboxGoodType");
		ListboxTextItem* itm = NULL;
		for(int i = 0 ; i < UIData::gGoodsTypeNum ; ++i)
		{
			itm = new ListboxTextItem(s_GoodsOP.GetGoodsType(i),i);
			itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
			cbbo->addItem(itm);
		}
	}
	void GoodsTree::InitIsSoundSwitchCombobox()
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

	void GoodsTree::InitAddAttrCombobox()
	{
		//附加属性
		Combobox* cbbo = GetCombobox("GoodsTreeFrame/AddProperty/GroupBox");
		ListboxTextItem* itm;
		for(size_t i = 0 ; i < s_GoodsOP.GetGoodItemTypeSize(); ++i)
		{
			itm = new ListboxTextItem(s_GoodsOP.GetGoodItemType(i),(CEGUI::uint)i);
			itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
			cbbo->addItem(itm);
		}
	}
	void GoodsTree::InitAddAttrValueSelCombobox()
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
	void GoodsTree::InitSuitAttrCombobox()
	{
		//套装属性
		Combobox* cbbo = GetCombobox("GoodsTreeFrame/GroupBoxSuitAtrr/CombBoxSuitAttr");
		ListboxTextItem* itm;
		for(size_t i = 0 ; i < s_GoodsOP.GetGoodItemTypeSize(); ++i)
		{
			itm = new ListboxTextItem(s_GoodsOP.GetGoodItemType(i),(CEGUI::uint)i);
			itm->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
			cbbo->addItem(itm);
		}
	}
	TreeItem* GoodsTree::AddItemToTree(String& ItemStr)
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

	TreeItem* GoodsTree::AddItemToRootItem(const String& ItemStr)
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
	void GoodsTree::SetItemContentInMulList(const UIData::tagGoodAttr* pGoodAttr,CEGUI::uint RowIdx)
	{
		if(pGoodAttr!=NULL)
		{
			MultiColumnList* mclbox = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/MultiColumnList"));
			//清空行
			mclbox->resetList();
			//设置每一行的值
			mclbox->addRow();

			/*m_arrListItem[s_ListRowNum* RowIdx].setText(PropertyHelper::intToString(pGoodAttr->dwIndex));
			mclbox->setItem( &m_arrListItem[s_ListRowNum* RowIdx],0,RowIdx);*/
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwIndex)),0,RowIdx);
			mclbox->setItem(new MyListItem(CEGUI::String(pGoodAttr->strOrigName)),1,RowIdx);
			mclbox->setItem(new MyListItem(CEGUI::String(pGoodAttr->strName)),2,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::boolToString(pGoodAttr->bSave)),3,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwIconId)),4,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwGroundId)),5,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwEquipID)),6,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwValue)),7,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSilverValue)),8,RowIdx);
			mclbox->setItem(new MyListItem(s_GoodsOP.GetGoodsType(pGoodAttr->dwType)),9,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSound)),10,RowIdx);
			mclbox->setItem(new MyListItem(pGoodAttr->strContent),11,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSoundID1)),12,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(pGoodAttr->dwSoundID2)),13,RowIdx);
			mclbox->setItem(new MyListItem(PropertyHelper::boolToString(pGoodAttr->bSoundSwitch)),14,RowIdx);

			//mclbox->setProperty("Font","Chinese");
		}
	}

	bool GoodsTree::OnBranchSelectChanged(const EventArgs& args)
	{
		const TreeEventArgs& treeArgs = static_cast<const TreeEventArgs&>(args);
		TreeItem *selItem = treeArgs.treeItem;
		if(selItem)
		{
			const String &str = selItem->getTextVisual();
			const UIData::tagGoodAttr *goodAttr = s_GoodsOP.GetGoodAttrByOgriName(str);
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
				String strModelGrapID = PropertyHelper::intToString(goodAttr->dwIconId);
				string strModelName = strModelGrapID.c_str();
				strModelName.insert(0,"\\");
				strModelName += MODEL_NAME;
				GoodsModelManager::GetInstance()->OpenModel(strModelName.c_str(),false);
			}
		}
		return true;
	}

	bool GoodsTree::OnAddAttrItemSelChanged(const EventArgs& args)
	{
		//处理附加属性条目选中消息
		MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/AddProperty/MultiColumnList"));
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

	bool GoodsTree::OnSuitAttrItemSelChanged(const EventArgs& args)
	{
		//处理套装属性条目选中消息
		MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/GroupBoxSuitAtrr/MultiColumnList"));
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

	void GoodsTree::UpdateBaseAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
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
			pEditBox->setText("你好");
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
	void GoodsTree::UpdateClientAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
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
		MultiLineEditbox* muleditbox = static_cast<MultiLineEditbox*>(WindowManager::getSingleton().getWindow("GoodsTreeFrame/ClientProperty/Content"));
		muleditbox->setText(pGoodAttr->strContent);
	}

	void GoodsTree::UpdateAddAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
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
			mclbox->setItem(new MyListItem(s_GoodsOP.GetGoodItemType(addAttr.wType)),0,(CEGUI::uint)size);
			mclbox->setItem(new MyListItem(PropertyHelper::boolToString(addAttr.bEnable)),1,(CEGUI::uint)size);
			mclbox->setItem(new MyListItem(PropertyHelper::boolToString(addAttr.bHide)),2,(CEGUI::uint)size);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(addAttr.lValue1)),3,(CEGUI::uint)size);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(addAttr.lValue2)),4,(CEGUI::uint)size);
		}
	}

	void GoodsTree::UpdateSuitAttrDisplay(const UIData::tagGoodAttr* pGoodAttr)
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
			mclbox->setItem(new MyListItem(s_GoodsOP.GetGoodItemType(suitAttr.wType)),0,i);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.lValue1)),1,i);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.lValue2)),2,i);
			mclbox->setItem(new MyListItem(PropertyHelper::intToString(suitAttr.wSuitNum)),3,i);
		}
	}
	Editbox* GoodsTree::GetEditbox(const char *WindowName)
	{
		return static_cast<Editbox*>(WindowManager::getSingleton().getWindow(WindowName));
	}
	Combobox* GoodsTree::GetCombobox(const char *WindowName)
	{
		return static_cast<Combobox*>(WindowManager::getSingleton().getWindow(WindowName));
	}
}