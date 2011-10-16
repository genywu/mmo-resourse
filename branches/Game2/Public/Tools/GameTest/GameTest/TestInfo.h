//=============================================================================
/**
 *  file: TestInfo.h
 *
 *  Brief:管理游戏中的数据显示信息
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#pragma once

#include "RegionInfo.h"
#include "PlayerInfo.h"
#include "TreeGameClient.h"
#include "MonsterInfo.h"
#include "SkillInfo.h"
#include "RegionGoodsInfo.h"

class CTestInfo : public CDialog
{
	DECLARE_DYNAMIC(CTestInfo)

public:
	enum eChatType
	{
		CHAT_NORMAL = 0,		// 同屏聊天
		CHAT_REGION,			// 同场景聊天
		CHAT_FACTION,			// 帮会聊天
		CHAT_UNION,				// 联盟聊天
		CHAT_TEAM,				// 队伍聊天
		CHAT_PRIVATE,			// 两人私聊
		CHAT_COUNTRY,			// 国家聊天
		CHAT_WOLRD,				// 世界聊天
		CHAT_BOARDCAST,			// 广播
		GM_CMD,					// GM命令
		CHAT_KING_COUNTRY,		// 国王国家喊话
		CHAT_MINISTER_COUNTRY,	// 大臣国家喊话
		RUN_SCRIPT,				// 运行脚本
		RUN_HELP,				// 帮助

		TYPE_NUM,
	};

public:
	CTestInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTestInfo();

	enum { IDD = IDD_PROPPAGE_GAMEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

private:
	long m_lTimerID;
	BOOL	m_bCheckOne;

	long m_lMaxContNum;
	long m_lMinContNum;


	CStatic m_ShowAmountCtrl;
	CGameClient*	m_pSelGameClient;
	CRegionInfo				m_RegionInfo;
	CPlayerInfo				m_PlayerInfo;
	CMonsterInfo			m_MonsterInfo;
	CSkillInfo				m_SkillInfo;
	CTreeGameClient			m_ctrGameClients;
	CRegionGoodsInfo		m_RegionGoods;
	CEdit m_edInputBox;
	CButton m_CheckOne;
	CString	m_strScriptName;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnEnChangeEditOutputInfo();
	afx_msg void OnRunScript();				//执行脚本
	void ReleaseTimer()	{ KillTimer(m_lTimerID); m_lTimerID = -1; }
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CTreeGameClient* GetTreeGameClient()	{return &m_ctrGameClients;}

	void OnTreeSelect();
	void OnSelectRegion();
	void ResetSelGameClient(CGameClient* pGameClient);
	void OutputGameInfo(const char* strInfo);
	void OutputThreadInfo(const char* info);
	void UpdateDisplayInfo(CGameClient* pGameClient);


	CGameClient*  GetSelGameClient()		{return m_pSelGameClient;}
	CMonsterInfo* GetMonsterInfo()			{return &m_MonsterInfo;}
	CRegionGoodsInfo* GetRegionGoodsInfo()	{return &m_RegionGoods;}
	CPlayerInfo* GetPlayerInfo()			{return &m_PlayerInfo;}
	CString m_StrInputBox;
	CEdit m_edOutputBox;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	const CGUID* GetPlayerAttributeToGUID();
	long GetPlayerAttributeToSocketID();
	const char* GetPlayerAttributeToName();
	const CString GetInputBoxInfo();	

	long GetMaxConnect() { return m_lMaxContNum; }
	long GetMinConnect() { return m_lMinContNum; }

};
