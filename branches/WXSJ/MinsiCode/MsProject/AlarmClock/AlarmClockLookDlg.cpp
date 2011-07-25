// AlarmClockLookDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmClock.h"
#include "AlarmClockLookDlg.h"


// AlarmClockLookDlg 对话框

IMPLEMENT_DYNAMIC(AlarmClockLookDlg, CDialog)

AlarmClockLookDlg::AlarmClockLookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AlarmClockLookDlg::IDD, pParent)
{

}

AlarmClockLookDlg::~AlarmClockLookDlg()
{
}

void AlarmClockLookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AlarmClockLookDlg, CDialog)
END_MESSAGE_MAP()


// AlarmClockLookDlg 消息处理程序
