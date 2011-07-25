// AlarmClockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AlarmClockDlg.h"
#include "AlarmClock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAlarmClockDlg 对话框

DWORD WINAPI CallBackFun(LPVOID lpThreadParameter)
{
    CAlarmClockDlg* pCAlarmClockDlg = (CAlarmClockDlg*)lpThreadParameter;
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    pCAlarmClockDlg->m_IsExit = FALSE;
    INT64 ftSrc;
    INT64 ftDes;
    SYSTEMTIME st;
    while (pCAlarmClockDlg->m_bRun)
    {
        ::GetLocalTime(&st);
        // 仅一次
        for (UINT i = 0; i < pCAlarmClockDlg->m_OnceClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_OnceClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc)
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_OnceClockReg[i], i);
                break;
            }
        }

        // 在每天
        for (UINT i = 0; i < pCAlarmClockDlg->m_EverydayClockRegCount; i++)
        {
            pCAlarmClockDlg->m_EverydayClockReg[i].m_ClueOnTime.wYear       = st.wYear;
            pCAlarmClockDlg->m_EverydayClockReg[i].m_ClueOnTime.wMonth      = st.wMonth;
            pCAlarmClockDlg->m_EverydayClockReg[i].m_ClueOnTime.wDay        = st.wDay;
            pCAlarmClockDlg->m_EverydayClockReg[i].m_ClueOnTime.wDayOfWeek  = st.wDayOfWeek;
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_EverydayClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_EverydayClockReg[i], i);
                break;
            }
        }

        // 仅周日
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week0ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week0ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 0 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week0ClockReg[i], i);
                break;
            }
        }

        // 仅周一
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week1ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week1ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 1 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week1ClockReg[i], i);
                break;
            }
        }

        // 仅周二
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week2ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week2ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 2 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week2ClockReg[i], i);
                break;
            }
        }

        // 仅周三
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week3ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week3ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 3 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week3ClockReg[i], i);
                break;
            }
        }

        // 仅周四
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week4ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week4ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 4 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week4ClockReg[i], i);
                break;
            }
        }

        // 仅周五
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week5ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week5ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 5 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week5ClockReg[i], i);
                break;
            }
        }

        // 仅周六
        for (UINT i = 0; i < pCAlarmClockDlg->m_Week6ClockRegCount; i++)
        {
            ::SystemTimeToFileTime(&pCAlarmClockDlg->m_Week6ClockReg[i].m_ClueOnTime, (FILETIME*)&ftSrc);
            ::SystemTimeToFileTime(&st, (FILETIME*)&ftDes);
            if (ftDes >= ftSrc && st.wDayOfWeek == 6 && ftDes <= (ftSrc + 10000000))
            {
                pCAlarmClockDlg->OnGetToTime(&pCAlarmClockDlg->m_Week6ClockReg[i], i);
                break;
            }
        }

        ::SleepEx(300, TRUE);
    }

    pCAlarmClockDlg->m_IsExit = TRUE;
    return 0;
}

CAlarmClockDlg::CAlarmClockDlg(CWnd* pParent /*=NULL*/)
	: CDialog                   (CAlarmClockDlg::IDD, pParent)
    , m_RegCount                (0)
    , m_OnceClockRegCount       (0)
    , m_EverydayClockRegCount   (0)
    , m_Week0ClockRegCount      (0)
    , m_Week1ClockRegCount      (0)
    , m_Week2ClockRegCount      (0)
    , m_Week3ClockRegCount      (0)
    , m_Week4ClockRegCount      (0)
    , m_Week5ClockRegCount      (0)
    , m_Week6ClockRegCount      (0)
    , m_pAddDlg                 (NULL)
    , m_pLookDlg                (NULL)
    , m_bRun                    (FALSE)
    , m_IsExit                  (FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAlarmClockDlg::~CAlarmClockDlg(void)
{
    if (m_pAddDlg)
    {
        delete m_pAddDlg;
    }
    if (m_pLookDlg)
    {
        delete m_pLookDlg;
    }
}

void CAlarmClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAlarmClockDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(MS_WM_NOTIFYICON, OnTrayNotification)

	//}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_COMMAND(ID_MENU_SHOW, &CAlarmClockDlg::OnMenuShow)
    ON_COMMAND(ID_MENU_EXIT, &CAlarmClockDlg::OnMenuExit)
    ON_BN_CLICKED(IDC_BUTTON1, &CAlarmClockDlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON3, &CAlarmClockDlg::OnBnClickedButtonLook)
END_MESSAGE_MAP()


// CAlarmClockDlg 消息处理程序

BOOL CAlarmClockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize            = sizeof(m_nid);                //设置结构大小
    m_nid.hWnd              = m_hWnd;                       //设置图标对应的窗口
    m_nid.hIcon             = m_hIcon;
    m_nid.uFlags            = NIF_ICON|NIF_MESSAGE|NIF_TIP;
    m_nid.uCallbackMessage  = MS_WM_NOTIFYICON;             //应用程序定义的回调消息ID
    m_nid.uID               = IDR_MAINFRAME;
    m_nid.uVersion          = NOTIFYICON_VERSION; 
    _tcscpy_s(m_nid.szTip, _T("敏思定时闹钟！"));

    if (!::Shell_NotifyIcon(NIM_ADD, &m_nid))               //增加系统托盘图标
    {
        this->MessageBox(_T("添加系统托盘图标失败！"));
    }

    DWORD dwCountByte = sizeof(m_RegCount);
    DWORD dwRegByte = sizeof(S_CLOCK_REGISTER);
    HANDLE hFile = ::CreateFile(_T("AlarmClock"), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        hFile = ::CreateFile(_T("AlarmClock"), FILE_READ_DATA, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
        ::WriteFile(hFile, &m_RegCount, dwCountByte, &dwCountByte, NULL);
    }

    ::SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    ::ReadFile(hFile, &m_RegCount, dwCountByte, &dwCountByte, NULL);
    S_CLOCK_REGISTER TempClockReg;
    for (UINT i = 0; i < m_RegCount; i++)
    {
        ::ReadFile(hFile, &TempClockReg, dwRegByte, &dwRegByte, NULL);
        switch (TempClockReg.m_ClockType)
        {
        case E_CLOCK_TYPE_ONCE:         m_OnceClockReg[m_OnceClockRegCount++]           = TempClockReg; break;
        case E_CLOCK_TYPE_EVERYDAY:     m_EverydayClockReg[m_EverydayClockRegCount++]   = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_0:  m_Week0ClockReg[m_Week0ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_1:  m_Week1ClockReg[m_Week2ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_2:  m_Week2ClockReg[m_Week3ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_3:  m_Week3ClockReg[m_Week3ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_4:  m_Week4ClockReg[m_Week4ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_5:  m_Week5ClockReg[m_Week5ClockRegCount++]         = TempClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_6:  m_Week6ClockReg[m_Week6ClockRegCount++]         = TempClockReg; break;
        }
    }
    ::CloseHandle(hFile);

    m_CMsSound.Init(FALSE);
    m_CMsSound.Open(0, "Sound/PlaySound", TRUE);

    m_bRun = TRUE;
    ::CreateThread(NULL, 0, CallBackFun, this, 0, NULL);
    // 加载菜单
    m_Menu.LoadMenu(IDR_MENU);

    this->ShowBallute(_T("启动成功！"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAlarmClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAlarmClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAlarmClockDlg::OnDestroy()
{
    CDialog::OnDestroy();
    if (!::Shell_NotifyIcon(NIM_DELETE, &m_nid))                 //删除系统托盘图标
    {
        this->MessageBox(_T("删除系统托盘图标失败！"));
    }
}

LRESULT CAlarmClockDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
    switch (lParam)
    {
    case WM_RBUTTONUP:
        {
            POINT pt;
            ::GetCursorPos(&pt); 
            CMenu* pMenu = m_Menu.GetSubMenu(0);
            pMenu->TrackPopupMenu(TPM_RIGHTBUTTON|TPM_LEFTALIGN, pt.x, pt.y, this);
        }break;
    case WM_LBUTTONUP:
        {
            if (m_CMsSound.IsPlaying(0))
            {
                m_CMsSound.Stop(0);
            }
            this->ShowWindow(SW_SHOW);
        }break;
    }
    return 0;
}

void CAlarmClockDlg::OnClose()
{
    this->ShowWindow(SW_HIDE);
    this->ShowBallute(_T("窗口已经隐藏！\n显示窗口请点击这里！\n右键退出！"));
}

void CAlarmClockDlg::OnMenuShow()
{
    if (m_CMsSound.IsPlaying(0))
    {
        m_CMsSound.Stop(0);
    }
    this->ShowWindow(SW_SHOW);
}

void CAlarmClockDlg::OnMenuExit()
{
    m_bRun = FALSE;
    while (!m_IsExit)
    {
        ::SleepEx(100, TRUE);
    }

    if ((HWND)m_pAddDlg)
    {
        m_pAddDlg->EndDialog(0);
    }
    if ((HWND)m_pLookDlg)
    {
        m_pLookDlg->EndDialog(0);
    }

    DWORD dwCountByte = sizeof(m_RegCount);
    DWORD dwRegByte = sizeof(S_CLOCK_REGISTER);
    HANDLE hFile = ::CreateFile(_T("AlarmClock"), FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        ::DeleteFile(_T("AlarmClock"));
        hFile = ::CreateFile(_T("AlarmClock"), FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
    }

    m_RegCount = 0;
    ::SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    ::WriteFile(hFile, &m_RegCount, dwCountByte, &dwCountByte, NULL);

    // 仅一次
    for (UINT i = 0; i < m_OnceClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_OnceClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 在每天
    for (UINT i = 0; i < m_EverydayClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_EverydayClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周日
    for (UINT i = 0; i < m_Week0ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week0ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周一
    for (UINT i = 0; i < m_Week1ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week1ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周二
    for (UINT i = 0; i < m_Week2ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week2ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周三
    for (UINT i = 0; i < m_Week3ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week3ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周四
    for (UINT i = 0; i < m_Week4ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week4ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周五
    for (UINT i = 0; i < m_Week5ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week5ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    // 仅周六
    for (UINT i = 0; i < m_Week6ClockRegCount; i++)
    {
        m_RegCount++;
        ::WriteFile(hFile, &m_Week6ClockReg[i], dwRegByte, &dwRegByte, NULL);
    }

    ::SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    ::WriteFile(hFile, &m_RegCount, dwCountByte, &dwCountByte, NULL);

    ::CloseHandle(hFile);
    this->EndDialog(0);
}

BOOL CAlarmClockDlg::ShowBallute(TCHAR* pText)
{
    if (pText)
    {
        m_nid.uFlags            = NIF_INFO;
        m_nid.uTimeout          = 10000;
        m_nid.dwInfoFlags       = NIIF_INFO;
        _tcscpy_s(m_nid.szInfoTitle, _T("敏思闹钟！"));
        _tcscpy_s(m_nid.szInfo, pText);
        return ::Shell_NotifyIcon(NIM_MODIFY, &m_nid);
    }
    return FALSE;
}

void CAlarmClockDlg::OnBnClickedButtonAdd()
{
    S_CLOCK_REGISTER ClockReg;

    if (!m_pAddDlg)
    {
        m_pAddDlg = new AlarmClockAddDlg(&ClockReg);
    }
    if (0 == m_pAddDlg->DoModal())
    {
        switch (ClockReg.m_ClockType)
        {
        case E_CLOCK_TYPE_ONCE: m_OnceClockReg[m_OnceClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_EVERYDAY: m_EverydayClockReg[m_EverydayClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_0: m_Week0ClockReg[m_Week0ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_1: m_Week1ClockReg[m_Week1ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_2: m_Week2ClockReg[m_Week2ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_3: m_Week3ClockReg[m_Week3ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_4: m_Week4ClockReg[m_Week4ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_5: m_Week5ClockReg[m_Week5ClockRegCount++] = ClockReg; break;
        case E_CLOCK_TYPE_ONLY_WEEK_6: m_Week6ClockReg[m_Week6ClockRegCount++] = ClockReg; break;
        }
    }
    delete m_pAddDlg;
    m_pAddDlg = NULL;
}

void CAlarmClockDlg::OnBnClickedButtonLook()
{
    if (!m_pLookDlg)
    {
        m_pLookDlg = new AlarmClockLookDlg;
    }
    m_pLookDlg->DoModal();
    delete m_pLookDlg;
    m_pLookDlg = NULL;
}

void CAlarmClockDlg::OnGetToTime(S_CLOCK_REGISTER* pRegister, UINT uIndex)
{
    S_CLOCK_REGISTER cr;
    memcpy(&cr, pRegister, sizeof(cr));
    switch(pRegister->m_ClockType)
    {
    case E_CLOCK_TYPE_ONCE:
        {
            memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_OnceClockRegCount - 1) - uIndex));
            m_OnceClockRegCount--;
        }break;
    case E_CLOCK_TYPE_EVERYDAY:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_EverydayClockRegCount - 1) - uIndex));
            //m_EverydayClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_0:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week0ClockRegCount - 1) - uIndex));
            //m_Week0ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_1:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week1ClockRegCount - 1) - uIndex));
            //m_Week1ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_2:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week2ClockRegCount - 1) - uIndex));
            //m_Week2ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_3:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week3ClockRegCount - 1) - uIndex));
            //m_Week3ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_4:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week4ClockRegCount - 1) - uIndex));
            //m_Week4ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_5:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week5ClockRegCount - 1) - uIndex));
            //m_Week5ClockRegCount--;
        }break;
    case E_CLOCK_TYPE_ONLY_WEEK_6:
        {
            //memcpy(pRegister, (pRegister + 1), sizeof(S_CLOCK_REGISTER) * ((m_Week6ClockRegCount - 1) - uIndex));
            //m_Week6ClockRegCount--;
        }break;
    }

    m_CMsSound.Play(0, FALSE);
    m_CMsSound.SetVolume(0, VOLUME_MAX_VALUE);
    this->ShowBallute(cr.m_szClueOnText);
    ::MessageBox(NULL, cr.m_szClueOnText, _T("闹铃"), MB_OK);
}
