#pragma  once
#include "../../PageEvent.h"
#include "GoodsOperate.h"
#include "../../UIDef.h"


class MyListItem:public CEGUI::ListboxTextItem
{
public:
	MyListItem(const CEGUI::String& text = ""):ListboxTextItem(text)
	{
		setSelectionBrushImage("TaharezLook","MultiListSelectionBrush");
	}
};

class GVEvent:public PageEvent
{
public:
	GVEvent();
	GVEvent(EVENTTYPE type){ SetEventType(type) ; }
	~GVEvent();

	void OnPageLoad(GamePage *pPage);
	void OnPageOpen(GamePage *pPage);
	void OnPageClose(GamePage *pPage);
	void OnPageUpdate(GamePage *pPage);
	void OnPageRelease(GamePage *pPage);

	/// @brief
	/// 增加Item到RootItem
	/// @param *ItemStr
	/// 条目内容
	/// @return TreeItem*
	/// 返回指向当前加入的TreeItem指针
	CEGUI::TreeItem* AddItemToRootItem(const CEGUI::String& ItemStr);

	/////////////////////////////////////////////////
	// zhaohang  2010/3/15 
	// 控件的事件响应函数

	/// @brief
	/// 处理树形框的选中消息
	/// @param args
	/// 
	/// @return
	/// 
	bool OnBranchSelectChanged(const CEGUI::EventArgs& args);

	/// @brief
	/// 处理附加属性条目选中消息
	/// @param args
	/// 
	/// @return bool
	/// 
	bool OnAddAttrItemSelChanged(const CEGUI::EventArgs& args);

	/// @brief
	/// 处理套装属性条目选中消息
	/// @param args
	/// 
	/// @return bool
	/// 
	bool OnSuitAttrItemSelChanged(const CEGUI::EventArgs& args);
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	// zhaohang  2010/3/17 
	// 物品模型控制响应
	/////////////////////////////////////////////////
	//bool OnScalAdd(const CEGUI::EventArgs& args);
	//bool OnScalMinus(const CEGUI::EventArgs& args);
	//bool OnXMinus(const CEGUI::EventArgs& args);
	//bool OnXAdd(const CEGUI::EventArgs& args);
	//bool OnYMinus(const CEGUI::EventArgs& args);
	//bool OnYAdd(const CEGUI::EventArgs& args);

	//bool OnLeftRot(const CEGUI::EventArgs& args);
	//bool OnRightRot(const CEGUI::EventArgs& args);
	//bool OnUpRot(const CEGUI::EventArgs& args);
	//bool OnDownRot(const CEGUI::EventArgs& args);

	/////////////////////////////////////////////////
	// zhaohang  2010/3/18 
	// 新的控制模型函数
	bool HandScal(const CEGUI::EventArgs& args);
	bool HandXMove(const CEGUI::EventArgs& args);
	bool HandYMove(const CEGUI::EventArgs& args);
	bool HandXRot(const CEGUI::EventArgs& args);
	bool HandYRot(const CEGUI::EventArgs& args);
	bool HandZRot(const CEGUI::EventArgs& args);
	/////////////////////////////////////////////////
protected:
	
private:
	/// @brief
	/// //是否存数据库Combobox条目的初始化
	/// @param
	/// 
	/// @return void
	/// 
	void InitIsDBCombobox();
	/// @brief
	/// //物品类型Combobox条目的初始化
	/// @param void
	/// 
	/// @return void
	/// 
	void InitGoodTypeCombobox();

	/// @brief
	/// 攻击时是否混音Combobox条目的初始化
	/// @param
	/// 
	/// @return
	/// 
	void InitIsSoundSwitchCombobox();

	/// @brief
	/// 初始化附加属性Combobox条目
	/// @param
	/// 
	/// @return
	/// 
	void InitAddAttrCombobox();

	/// @brief
	/// 初始化附加属性值的Combobox(是否有效，是否隐藏)
	/// @param void
	/// 
	/// @return void
	/// 
	void InitAddAttrValueSelCombobox();

	/// @brief
	/// 初始化套装属性Combobox（套装名称)
	/// @param
	/// 
	/// @return
	/// 
	void InitSuitAttrCombobox();

	/// @brief
	/// 
	/// @param
	/// 
	/// @return
	/// 
	void ReSetScrollPos();
	/// @brief
	/// 辅助函数，获取指定名称的Editbox控件
	/// @param WindowName
	/// 窗口名称
	/// @return Editbox*
	/// 返回指定名称的Editbox的指针
	CEGUI::Editbox* GetEditbox(const char *WindowName);

	/// @brief
	/// 辅助函数，获取指定名称的Combobox控件
	/// @param WindowName
	/// 窗口名称
	/// @return Combobox*
	/// 返回指定名称的Combobox的指针
	CEGUI::Combobox* GetCombobox(const char *WindowName);

	/// @brief
	/// 增加树形框的Item
	/// @param &ItemStr
	/// 条目内容
	/// @return TreeItem*
	/// 返回指向当前加入TreeItem的指针
	CEGUI::TreeItem* AddItemToTree(const CEGUI::String& ItemStr);

	/////////////////////////////////////////////////
	// zhaohang  2010/3/16 
	// 控件数据显示更新function

	/// @brief
	/// 增加ItemContent到MultiList
	/// @param const UIData::tagGoodAttr*
	/// @param RowIdx
	/// 加入的行号
	/// @return
	/// 
	void SetItemContentInMulList(const UIData::tagGoodAttr* pGoodAttr,CEGUI::uint RowIdx);

	/// @brief
	/// 更新基本属性数据的显示
	/// @param pGoodAttr
	/// 单个物品的属性指针
	/// @return
	/// void
	void UpdateBaseAttrDisplay(const UIData::tagGoodAttr* pGoodAttr);

	/// @brief
	/// 更新物品客户端特有属性数据的显示
	/// @param pGoodAttr
	/// 单个物品属性的指针
	/// @return
	/// void 
	void UpdateClientAttrDisplay(const UIData::tagGoodAttr* pGoodAttr);

	/// @brief
	/// 更新物品附加属性数据的显示
	/// @param pGoodAttr
	/// 单个物品属性的指针
	/// @return
	/// void
	void UpdateAddAttrDisplay(const UIData::tagGoodAttr* pGoodAttr);

	/// @brief
	/// 更新物品套装属性数据的显示
	/// @param pGoodAttr
	/// 单个物品属性的指针
	/// @return
	/// void
	void UpdateSuitAttrDisplay(const UIData::tagGoodAttr* pGoodAttr);
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	// zhaohang  2010/3/15 
	CEGUI::Window* m_wnd;//GoodsViewer界面的根窗口
	CEGUI::Tree* m_GoodsTree;  //指向界面Tree的指针
	CEGUI::TreeItem* m_RootItem;//指向界面Tree的根Item
	std::vector<CEGUI::TreeItem*>m_ChildRootItem;//保存RootItem的孩子，在析构中，进行内存释放

	UIData::GoodsOperate m_GoodsOP;//物品数据操作类,使用动态内存分配方式，释放放在OnPageRelease() function中

	int m_xMove;
	int m_yMove;
	int m_zMove;
	int m_xRot;
	int m_yRot;
	int m_zRot;
	/////////////////////////////////////////////////
	
};