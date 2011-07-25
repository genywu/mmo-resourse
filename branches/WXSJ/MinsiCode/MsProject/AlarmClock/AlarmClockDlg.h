// AlarmClockDlg.h : 头文件
//

#pragma once
#include "resource.h"
#include "AlarmClockAddDlg.h"
#include "AlarmClockLookDlg.h"
#include "MsSound.h"

// CAlarmClockDlg 对话框
class CAlarmClockDlg : public CDialog
{
// 构造
public:
    CAlarmClockDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CAlarmClockDlg(void);

// 对话框数据
	enum { IDD = IDD_ALARMCLOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
    friend DWORD WINAPI CallBackFun(LPVOID lpThreadParameter);

// 实现
protected:
	HICON               m_hIcon;                // 图标句柄
    NOTIFYICONDATA      m_nid;                  // 托盘信息
    CMenu               m_Menu;                 // 托盘菜单
    UINT                m_RegCount;             // 提示记录个数

    AlarmClockAddDlg*   m_pAddDlg;
    AlarmClockLookDlg*  m_pLookDlg;

    S_CLOCK_REGISTER    m_OnceClockReg[MAX_CLOCK_COUNT];    // 一次提示记录数组
    UINT                m_OnceClockRegCount;                // 一次提示记录个数

    S_CLOCK_REGISTER    m_EverydayClockReg[MAX_CLOCK_COUNT];// 每天提示记录数组
    UINT                m_EverydayClockRegCount;            // 每天提示记录个数

    S_CLOCK_REGISTER    m_Week0ClockReg[MAX_CLOCK_COUNT];   // 周日提示记录数组
    UINT                m_Week0ClockRegCount;               // 周日提示记录个数

    S_CLOCK_REGISTER    m_Week1ClockReg[MAX_CLOCK_COUNT];   // 周一提示记录数组
    UINT                m_Week1ClockRegCount;               // 周一提示记录个数

    S_CLOCK_REGISTER    m_Week2ClockReg[MAX_CLOCK_COUNT];   // 周二提示记录数组
    UINT                m_Week2ClockRegCount;               // 周二提示记录个数

    S_CLOCK_REGISTER    m_Week3ClockReg[MAX_CLOCK_COUNT];   // 周三提示记录数组
    UINT                m_Week3ClockRegCount;               // 周三提示记录个数

    S_CLOCK_REGISTER    m_Week4ClockReg[MAX_CLOCK_COUNT];   // 周四提示记录数组
    UINT                m_Week4ClockRegCount;               // 周四提示记录个数

    S_CLOCK_REGISTER    m_Week5ClockReg[MAX_CLOCK_COUNT];   // 周五提示记录数组
    UINT                m_Week5ClockRegCount;               // 周五提示记录个数

    S_CLOCK_REGISTER    m_Week6ClockReg[MAX_CLOCK_COUNT];   // 周六提示记录数组
    UINT                m_Week6ClockRegCount;               // 周六提示记录个数

    BOOL                m_bRun;
    BOOL                m_IsExit;

    CMsSound            m_CMsSound;
	// 生成的消息映射函数
    BOOL            ShowBallute(TCHAR* pText);
    void            OnGetToTime(S_CLOCK_REGISTER* pRegister, UINT uIndex);
    virtual BOOL    OnInitDialog();
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void    OnDestroy();
    afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
    afx_msg void    OnClose();
    afx_msg void    OnMenuShow();
    afx_msg void    OnMenuExit();
    afx_msg void    OnBnClickedButtonAdd();
    afx_msg void    OnBnClickedButtonLook();
    DECLARE_MESSAGE_MAP()
};
