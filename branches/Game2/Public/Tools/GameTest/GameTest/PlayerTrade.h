#pragma once
class CTestInfo;
class CGameClient;

#include "afxcmn.h"
#include "..\Utility\GUID.h"
#include "..\App\Goods.h"

// CPlayerTrade 对话框

class CPlayerTrade : public CDialog
{
	DECLARE_DYNAMIC(CPlayerTrade)
///////////////////////////////////////////////////////////////////////
		//************交易*******************
public:
	enum tagPriceType
	{
		PT_Money,			// 货币
		PT_YuanBao,			// 元宝
	};

	enum tagUpdateObject
	{
		Trade_You,			//更新对象为对方玩家
		Trade_Me,			//更新对象为自己
	};
	struct tagTradeGoods 
	{
		long lNum;			// 物品的数量
		long lPos;			// 物品的位置
		DWORD dwIconId;		// 物品的图标ID
		CGUID guid;			// 物品的GUID
	};
	bool OpenTradePage(const CGUID& SessionID,const CGUID& MyID,long lMPlugID,const CGUID& YourID,long lYPlugID);		// 打开交易界面
	void OpenTradeDialog(CGameClient* client);
	const CGUID& GetSessionID()		{return m_SessionID;}					// 返回会话ID
	long GetMyPlugID()				{return m_lMPlugID;}					// 返回玩家自己的PlugID
	const CGUID& GetYouID()			{return m_YourID;}						// 返回交易对方的ID
private:
	static const WORD GRIDNUM;		//交易页面的交易栏位数
	/***************
	逻辑处理所需数据
	****************/
	CGUID		m_SessionID;		//每次会话的ID
	CGUID		m_MyID;				//我的玩家ID
	CGUID		m_YourID;			//对方玩家ID
	long		m_lMPlugID;			//自己在此次会话中的ID
	long		m_lYPlugID;			//对方在此次会话中的ID
	bool		m_bMeIsLocked;		//我方锁定状态标志
	bool		m_bYouIsLocked;		//对方锁定状态标志
	long		m_Yuanbao;			//我方元宝
	long		m_Golds;			//我方金币
	WORD		m_CoinType;			//货币类型

	map<int,tagTradeGoods> m_MyTradeGoodsList;				//我方要交易的物品列表
	map<int,CGoods> m_YourTradeGoodesList;					//对方要交易的物品列表
	///********************************
//////////////////////////////////////////////////////////////////////////////////
public:
	CPlayerTrade(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayerTrade();

// 对话框数据
	enum { IDD = IDD_DIALOG_Trade };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool bMyLock;
	bool bYouLock;
	// 对方栏位
	CListCtrl m_YouTradeList;
	// 自己栏位
	CListCtrl m_MyTradeList;
	afx_msg void OnBnClickedChangetrade();
	afx_msg void OnBnClickedReadytrade();
	afx_msg void OnBnClickedTrade();
};
