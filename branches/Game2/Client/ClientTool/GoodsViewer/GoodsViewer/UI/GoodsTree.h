/**
* @filename:GoodsTree.h
* @date: 2010/2/4
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 定义GoodsTree窗口对应相关的操作,新定义的界面类都要从BaseUIClass继承而来
*/
#include "../../CEGUIEngine/BaseUIClass.h"
#include "../GoodsData/GoodsOperate.h"

namespace UIClass
{
	class MyListItem:public CEGUI::ListboxTextItem
	{
	public:
		MyListItem(const CEGUI::String& text = ""):ListboxTextItem(text)
		{
			setSelectionBrushImage("TaharezLook","MultiListSelectionBrush");
		}
	};
	class GoodsTree:public BaseUIClass
	{
	public:
		/// @brief
		/// 构造函数
		/// @param
		/// 
		/// @return
		/// 
		GoodsTree() ;

		/// @brief
		/// 析构函数
		/// @param
		/// 
		/// @return
		/// 
		~GoodsTree() ;

		/// @brief
		/// 重写的初始化函数, 定制自己的UI操作,这里从文件中读取了Goods的数据
		/// @param
		/// 
		/// @return
		/// 
		bool Init();

		
		/// @brief
		/// 增加Item到RootItem
		/// @param *ItemStr
		/// 条目内容
		/// @return TreeItem*
		/// 返回指向当前加入的TreeItem指针
		TreeItem* AddItemToRootItem(const String& ItemStr);

		/// @brief
		/// 增加ItemContent到MultiList
		/// @param const UIData::tagGoodAttr*
		/// @param RowIdx
		/// 加入的行号
		/// @return
		/// 
		void SetItemContentInMulList(const UIData::tagGoodAttr* pGoodAttr,CEGUI::uint RowIdx);

		/// @brief
		/// 处理BranchOpened
		/// @param args
		/// 
		/// @return
		/// 
		bool HandleEventBranchOpened(const EventArgs& args);

		/// @brief
		/// 处理树形框的选中消息
		/// @param args
		/// 
		/// @return
		/// 
		bool OnBranchSelectChanged(const EventArgs& args);

		/// @brief
		/// 处理附加属性条目选中消息
		/// @param args
		/// 
		/// @return bool
		/// 
		bool OnAddAttrItemSelChanged(const EventArgs& args);
		
		/// @brief
		/// 处理套装属性条目选中消息
		/// @param args
		/// 
		/// @return bool
		/// 
		bool OnSuitAttrItemSelChanged(const EventArgs& args);
	protected:
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
	private:
		/// @brief
		/// 增加树形框的Item
		/// @param &ItemStr
		/// 条目内容
		/// @return TreeItem*
		/// 返回指向当前加入TreeItem的指针
		TreeItem* AddItemToTree(String& ItemStr);

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
		/// 辅助函数，获取指定名称的Editbox控件
		/// @param WindowName
		/// 窗口名称
		/// @return Editbox*
		/// 返回指定名称的Editbox的指针
		Editbox* GetEditbox(const char *WindowName);

		/// @brief
		/// 辅助函数，获取指定名称的Combobox控件
		/// @param WindowName
		/// 窗口名称
		/// @return Combobox*
		/// 返回指定名称的Combobox的指针
		Combobox* GetCombobox(const char *WindowName);

		Tree* m_GoodsTree;//指向界面Tree的指针
		TreeItem* m_RootItem;//指向界面Tree的根Item
		std::vector<TreeItem*>m_ChildRootItem;//保存RootItem的孩子，在析构中，进行内存释放
		MyListItem* m_arrListItem;//指针数组,指向多行列表框的单元格
		static const unsigned int s_TreeID;
		static const unsigned int s_ListRowNum;//列数目
		static UIData::GoodsOperate s_GoodsOP;///<数据逻辑操作类
	};
}