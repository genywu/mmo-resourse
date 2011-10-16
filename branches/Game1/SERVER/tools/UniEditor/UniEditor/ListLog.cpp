// ListLog.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditor.h"
#include "ListLog.h"
#include "UniEditorDlg.h"
#include "TextLog.h"
// CListLog

IMPLEMENT_DYNAMIC(CListLog, CListCtrl)

CString CListLog::szType[10] = {_T("错误"), _T("添加"), _T("删除"), _T("修改"), _T("保存"), _T("警告"), _T("XX")};

CListLog::CListLog()
{
	GetLocalTime(&m_sysTime);
	m_szTime.Format(_T("%02u:%02u:%02u"), m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

	ifstream ifs;
	ifs.open(_T("ColorSet_Log.ini"), ios_base::in);

	// 从文件读取颜色配置
	int i = 0;
	if(ifs.is_open())
	{
		string str;
		int r, g, b;
		while(TRUE == CUniEditorApp::ReadTo(_T("("), ifs))
		{
			ifs >> r >> g >> b;
			m_clText[i] = RGB(r, g, b);
			++i;
		}
	}

	// 采用默认颜色配置
	//enum { LOG_ERROR, LOG_ADD, LOG_DEL, LOG_MOD, LOG_SAVE, LOG_WARNING, LOG_XX };
	if(i != 6)
	{
		m_clText[LOG_ERROR]			= RGB(255, 0, 0);		// 错误
		m_clText[LOG_ADD]			= RGB(128, 0, 255);		// 添加
		m_clText[LOG_DEL]			= RGB(128, 128, 128);	// 删除
		m_clText[LOG_MOD]			= RGB(0, 155, 155);		// 修改
		m_clText[LOG_SAVE]			= RGB(0, 255, 0);		// 保存
		m_clText[LOG_WARNING]		= RGB(255, 128, 0);		// 警告
	}

	ifs.close();
}

CListLog::~CListLog()
{
}

BOOL CListLog::InitialList()
{
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;	//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;		//网格线（只适用与report风格的listctrl）
	SetExtendedStyle(dwStyle);			//设置扩展风格

	DeleteAllItems();
	InsertColumn(0, _T("时间"), LVCFMT_LEFT, 56);
	InsertColumn(1, _T("类型"), LVCFMT_LEFT, 38);
	InsertColumn(2, _T("详细信息"), LVCFMT_LEFT, 148);

	InsertItem(0, m_szTime);
	SetItemText(0, 1, _T("打开"));
	CTextLog::WriteLog("修改日志显示列表初始化成功");
	return TRUE;
}

BOOL CListLog::AddLog(const UINT uType, const CString &szInfo)
{
	static 	time_t t1( CTime::GetCurrentTime().GetTime() );
	static	int i(0);
	time_t t2( CTime::GetCurrentTime().GetTime() );

	if (++i > 300 && t2-t1 < 1)
	{
		MessageBox(_T("无法正常读取当前场景文件"), _T("错误"));
		return FALSE;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	m_szTime.Format(_T("%02u:%02u:%02u"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	int nCount = GetItemCount();
	if(!CUniEditorDlg::m_bCtrl)
	{
		InsertItem(nCount, m_szTime);
		SetItemText(nCount, 1, szType[uType]);
		SetItemText(nCount, 2, szInfo);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CListLog, CListCtrl)
	//ON_NOTIFY ( NM_CUSTOMDRAW, IDC_LIST_LOG, OnCustomdrawMyList )
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdraw )
END_MESSAGE_MAP()

// CListLog 消息处理程序

void CListLog::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
	}
	else
	{	// loading code
	}
}

void CListLog::OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult )
{
	// Take the default processing unless we set this to something else below.
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = 0;    
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.
		// We'll cycle the colors through red, green, and light blue.

		CString szLogItem = GetItemText((int)pLVCD->nmcd.dwItemSpec, 1);
		int i=0;
		for (; i<6; i++)
		{
			if (szType[i] == szLogItem)
			{
				break;
			}
		}

		if (i<6)
		{
			pLVCD->clrText = m_clText[i];
		}
		else
		{
			pLVCD->clrText = RGB(0, 0, 0);
		}

		//if ( ( 1 == pLVCD->iSubItem )
		//	crText = RGB(255,0,0);
		//else if ( (pLVCD->nmcd.dwItemSpec % 3) == 1 )
		//	crText = RGB(0,255,0);
		//else
		//	crText = RGB(128,128,255);
		//// Store the color back in the NMLVCUSTOMDRAW struct.
		//pLVCD->clrText = crText;
		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
	}
}