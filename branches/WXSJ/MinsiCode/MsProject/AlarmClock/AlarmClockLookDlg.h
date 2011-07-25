#pragma once


// AlarmClockLookDlg 对话框

class AlarmClockLookDlg : public CDialog
{
	DECLARE_DYNAMIC(AlarmClockLookDlg)

public:
	AlarmClockLookDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AlarmClockLookDlg();

// 对话框数据
	enum { IDD = IDD_ALARMCLOCK_DIALOG_LOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
