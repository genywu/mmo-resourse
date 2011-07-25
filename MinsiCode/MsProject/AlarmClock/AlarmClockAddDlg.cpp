// AlarmClockAddDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmClock.h"
#include "AlarmClockDlg.h"
#include "AlarmClockAddDlg.h"


// AlarmClockAddDlg 对话框

IMPLEMENT_DYNAMIC(AlarmClockAddDlg, CDialog)

AlarmClockAddDlg::AlarmClockAddDlg(S_CLOCK_REGISTER* pClockReg, CWnd* pParent /*=NULL*/)
	: CDialog       (AlarmClockAddDlg::IDD, pParent)
    , m_pClockReg   (pClockReg)
{

}

AlarmClockAddDlg::~AlarmClockAddDlg()
{
}

void AlarmClockAddDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RADIO1, m_ButtonOnce);
    DDX_Control(pDX, IDC_RADIO2, m_ButtonEveryDay);
    DDX_Control(pDX, IDC_RADIO3, m_ButtonWeek0);
    DDX_Control(pDX, IDC_RADIO4, m_ButtonWeek1);
    DDX_Control(pDX, IDC_RADIO5, m_ButtonWeek2);
    DDX_Control(pDX, IDC_RADIO6, m_ButtonWeek3);
    DDX_Control(pDX, IDC_RADIO7, m_ButtonWeek4);
    DDX_Control(pDX, IDC_RADIO8, m_ButtonWeek5);
    DDX_Control(pDX, IDC_RADIO9, m_ButtonWeek6);
    DDX_Control(pDX, IDC_RADIO10, m_ButtonClueNormal);
    DDX_Control(pDX, IDC_RADIO11, m_ButtonClueAlong);
    DDX_Control(pDX, IDC_RADIO12, m_ButtonClueInterval);
    DDX_Control(pDX, IDC_EDIT1, m_EditText);
    DDX_Control(pDX, IDC_EDIT2, m_EditInterval);
    DDX_Control(pDX, IDC_EDIT3, m_EditTime);
    DDX_Control(pDX, IDC_DATETIMEPICKER3, m_Date);
    DDX_Control(pDX, IDC_DATETIMEPICKER2, m_Time);
}


BEGIN_MESSAGE_MAP(AlarmClockAddDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &AlarmClockAddDlg::OnBnClickedButtonEnterAdd)
    ON_BN_CLICKED(IDC_RADIO13, &AlarmClockAddDlg::OnBnClickedRadio13)
    ON_BN_CLICKED(IDC_RADIO14, &AlarmClockAddDlg::OnBnClickedRadio14)
    ON_BN_CLICKED(IDC_BUTTON3, &AlarmClockAddDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// AlarmClockAddDlg 消息处理程序

void AlarmClockAddDlg::OnBnClickedButtonEnterAdd()
{
    TCHAR szBuff[128] = {0};
    m_EditText.GetWindowText(szBuff, 128);
    if (
            (
                (BST_CHECKED == m_ButtonOnce.GetState()) || 
                (BST_CHECKED == m_ButtonEveryDay.GetState()) || 
                (BST_CHECKED == m_ButtonWeek0.GetState()) || 
                (BST_CHECKED == m_ButtonWeek1.GetState()) || 
                (BST_CHECKED == m_ButtonWeek2.GetState()) || 
                (BST_CHECKED == m_ButtonWeek3.GetState()) || 
                (BST_CHECKED == m_ButtonWeek4.GetState()) || 
                (BST_CHECKED == m_ButtonWeek5.GetState()) || 
                (BST_CHECKED == m_ButtonWeek6.GetState())
            )&&
            (
                (BST_CHECKED == m_ButtonClueNormal.GetState()) ||
                (BST_CHECKED == m_ButtonClueAlong.GetState()) ||
                (BST_CHECKED == m_ButtonClueInterval.GetState())
            )&&
            (
                _tcsclen(szBuff)
            )&&
            (
                (BST_CHECKED == ((CButton*)(this->GetDlgItem(IDC_RADIO13)))->GetState()) ||
                (BST_CHECKED == ((CButton*)(this->GetDlgItem(IDC_RADIO14)))->GetState())
            )
        )
    {
        if (BST_CHECKED == m_ButtonOnce.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONCE;
        }
        else if (BST_CHECKED == m_ButtonEveryDay.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_EVERYDAY;
        }
        else if (BST_CHECKED == m_ButtonWeek0.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_0;
        }
        else if (BST_CHECKED == m_ButtonWeek1.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_1;
        }
        else if (BST_CHECKED == m_ButtonWeek2.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_2;
        }
        else if (BST_CHECKED == m_ButtonWeek3.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_3;
        }
        else if (BST_CHECKED == m_ButtonWeek4.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_4;
        }
        else if (BST_CHECKED == m_ButtonWeek5.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_5;
        }
        else if (BST_CHECKED == m_ButtonWeek6.GetState())
        {
            m_pClockReg->m_ClockType = E_CLOCK_TYPE_ONLY_WEEK_6;
        }

        if (BST_CHECKED == m_ButtonClueNormal.GetState())
        {
            m_pClockReg->m_ClueType = E_CLUE_TYPE_NORMAL;
        }
        else if (BST_CHECKED == m_ButtonClueAlong.GetState())
        {
            m_pClockReg->m_ClueType = E_CLUE_TYPE_ALONG;
        }
        else if (BST_CHECKED == m_ButtonClueInterval.GetState())
        {
            m_pClockReg->m_ClueType = E_CLUE_TYPE_INTERVAL;
        }

        if (BST_CHECKED == ((CButton*)(this->GetDlgItem(IDC_RADIO14)))->GetState())
        {
            SYSTEMTIME st;
            m_Date.GetTime(&st);
            m_pClockReg->m_ClueOnTime.wYear     = st.wYear;
            m_pClockReg->m_ClueOnTime.wMonth    = st.wMonth;
            m_pClockReg->m_ClueOnTime.wDay      = st.wDay;
            m_pClockReg->m_ClueOnTime.wDayOfWeek= st.wDayOfWeek;
            m_Time.GetTime(&st);
            m_pClockReg->m_ClueOnTime.wHour     = st.wHour;
            m_pClockReg->m_ClueOnTime.wMinute   = st.wMinute;
            m_pClockReg->m_ClueOnTime.wSecond   = st.wSecond;
            m_pClockReg->m_ClueOnTime.wMilliseconds = st.wMilliseconds;
        }
        else if (BST_CHECKED == ((CButton*)(this->GetDlgItem(IDC_RADIO13)))->GetState())
        {
            INT64 nTime = 0;
            CString Str_Day, Str_Hour, Str_Minute, Str_Second;
            INT64 nDay, nHour, nMinute, nSecond;
            ((CComboBox*)(this->GetDlgItem(IDC_COMBO1)))->GetLBText(((CComboBox*)(this->GetDlgItem(IDC_COMBO1)))->GetCurSel(), Str_Day);
            ((CComboBox*)(this->GetDlgItem(IDC_COMBO2)))->GetLBText(((CComboBox*)(this->GetDlgItem(IDC_COMBO2)))->GetCurSel(), Str_Hour);
            ((CComboBox*)(this->GetDlgItem(IDC_COMBO3)))->GetLBText(((CComboBox*)(this->GetDlgItem(IDC_COMBO3)))->GetCurSel(), Str_Minute);
            ((CComboBox*)(this->GetDlgItem(IDC_COMBO4)))->GetLBText(((CComboBox*)(this->GetDlgItem(IDC_COMBO4)))->GetCurSel(), Str_Second);
            nDay = _ttoi(Str_Day.GetString());
            nHour = _ttoi(Str_Hour.GetString());
            nMinute = _ttoi(Str_Minute.GetString());
            nSecond = _ttoi(Str_Second.GetString());
            ::GetLocalTime(&m_pClockReg->m_ClueOnTime);
            ::SystemTimeToFileTime(&m_pClockReg->m_ClueOnTime, (FILETIME*)&nTime);
            nTime += ((((nDay * 24 + nHour) * 60) + nMinute) * 60 + nSecond) * 1000 * 10000;
            ::FileTimeToSystemTime((FILETIME*)&nTime, &m_pClockReg->m_ClueOnTime);
        }

        m_EditText.GetWindowText(m_pClockReg->m_szClueOnText, 128);

        m_EditText.GetWindowText(szBuff, 10);
        m_pClockReg->m_IntervalLong = _ttoi(szBuff);

        m_EditTime.GetWindowText(szBuff, 10);
        m_pClockReg->m_MaxClueTime = _ttoi(szBuff);

        m_pClockReg->m_ClueLong = 10000;

        this->EndDialog(0);
    }
    else
    {
        this->MessageBox(_T("闹钟类型与提示信息等必须填写完整！"));
    }
}

void AlarmClockAddDlg::OnBnClickedRadio13()
{
    this->GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_DATETIMEPICKER3)->EnableWindow(FALSE);

    this->GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
    this->GetDlgItem(IDC_COMBO2)->EnableWindow(TRUE);
    this->GetDlgItem(IDC_COMBO3)->EnableWindow(TRUE);
    this->GetDlgItem(IDC_COMBO4)->EnableWindow(TRUE);


    ((CComboBox*)(this->GetDlgItem(IDC_COMBO1)))->SetCurSel(0);
    ((CComboBox*)(this->GetDlgItem(IDC_COMBO2)))->SetCurSel(0);
    ((CComboBox*)(this->GetDlgItem(IDC_COMBO3)))->SetCurSel(0);
    ((CComboBox*)(this->GetDlgItem(IDC_COMBO4)))->SetCurSel(0);
}

void AlarmClockAddDlg::OnBnClickedRadio14()
{
    this->GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_COMBO3)->EnableWindow(FALSE);
    this->GetDlgItem(IDC_COMBO4)->EnableWindow(FALSE);

    this->GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(TRUE);
    this->GetDlgItem(IDC_DATETIMEPICKER3)->EnableWindow(TRUE);
}

void AlarmClockAddDlg::OnBnClickedButton3()
{
    this->EndDialog(-1);
}
