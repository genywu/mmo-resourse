/**
* @filename:RankPageEvent.h
* @date: 2010/3/23
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 排行榜UI逻辑响应
*/
#pragma  once
#include "../../PageEvent.h"

///排行榜UI事件类
class RankPageEvent:public PageEvent
{
public:
	///
	RankPageEvent();
	///
	RankPageEvent(EVENTTYPE type);
	///
	~RankPageEvent();
	///
	void OnPageLoad(GamePage *pPage);
	///
	void OnPageOpen(GamePage *pPage);
	///
	void OnPageClose(GamePage *pPage);
	///
	void OnPageUpdate(GamePage *pPage);
	///
	void OnPageRelease(GamePage *pPage);

private:
	/// @brief
	/// 初始化各菜单Item
	bool InitMenu();

	/// @brief
	/// 设置菜单项子菜单Item的默认选中项
	void SetDefaultCurRankID(uint curSelRankID = 11 )
	{
		m_curRankID = curSelRankID;
	}

	/// @brief
	/// 注册界面控件的事件响应函数
	void SubscribeEvent();

	/// @brief
	/// 处理查询排名
	bool HandlQueryRank(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理查询名称
	bool HandlQueryName(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理向后翻页
	bool HandlQueryNextPage(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理向前翻页
	bool HandlQueryPrePage(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理查询我的排名
	bool HandlMyRank(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理主菜单下子菜单选中项更改事件
	bool HandlListBoxSelChange(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理关闭界面按钮点击事件
	bool HandlClose(const CEGUI::EventArgs& e);

	/// @brief
	/// 处理Tab控件选中变化函数
	bool HandlTabSelChanged(const CEGUI::EventArgs& e);

	/// @brief
	/// 更新页数信息
	void UpdatePageNumInfo();
	/// @brief
	/// 根据rankID（子菜单ID）更新MultiColumnList要显示的列表头
	void UpdateMulitListHeader(uint rankID);

	/// @brief
	/// 更新多行列表框显示内容
	/// @param rankID
	/// 由rankID来决定更新哪一项子菜单查询的信息
	void UpdateMulitListData(uint rankID);

	/////////////////////////////////////////////////
	// zhaohang  2010/3/29 
	// 辅助函数
	/// @brief
	/// 根据名称获取指定的ListBox指针
	/// @param lbName
	/// listbox的名称
	/// @return Listbox*
	/// 返回指定的ListBox的指针
	CEGUI::Listbox* GetListBox(const char* lbName);

	/// @brief
	/// 根据名称获取MultiColumnList的指针
	/// @param Name
	/// 指定的MultiColumnList的名称
	/// @return MultiColumnList*
	/// 返回指定名称的MultiColumnList*
	CEGUI::MultiColumnList* GetMultiColumnList(const char* Name);

	/// @brief
	/// 获取指定名称的Editbox
	CEGUI::Editbox* GetEditBox(const char* name);

	/// @brief
	/// 由查询ID获取主菜单ID
	/// @param rankID
	/// 查询的ID，也可以叫子菜单ID
	/// @return uint
	/// 返回主菜单的ID
	uint GetMainMenuIDByRankID(uint rankID);

	/// @brief
	/// 获取主菜单的索引
	/// @param rankID
	/// 查询的ID，也可以叫子菜单ID
	/// @return uint
	/// 返回主菜单的索引
	uint GetMainMenuIndexByRankID(uint rankID);

	/// @brief
	/// 由查询ID返回子菜单的在ListBox中的索引
	/// @param rankID
	/// 查询的ID，也可以叫子菜单ID
	/// @return uint
	/// 子菜单的在ListBox中的索引
	uint GetIndexInListBoxByRankID(uint rankID);

	/// @brief
	/// 格式化字符串显示的颜色
	/// @param str
	/// 传入的字符串引用
	/// @param const char* colour
	/// 格式化的颜色字符串
	/// @return CEGUI::String&
	/// 返回格式化好后字符串的引用
	CEGUI::String& FormatText(CEGUI::String& str,const char* colour = "[colour='FFFFFFFF']");
	/////////////////////////////////////////////////

	///排行榜UI根窗口
	CEGUI::Window* m_wnd;
	//std::map<uint,std::vector<CEGUI::ListboxTextItem*>>m_mapLbti;//存储所有的子菜单组

	uint m_curRankID;///当前选中的子菜单ID即服务端定义的rankID

	CEGUI::String m_strPageText;///存储页数信息的字符串  "当前第 %d/%d 页"
};

