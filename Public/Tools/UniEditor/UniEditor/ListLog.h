#pragma once


// CListLog

enum { LOG_ERROR, LOG_ADD, LOG_DEL, LOG_MOD, LOG_SAVE, LOG_WARNING, LOG_XX };

class CListLog : public CListCtrl
{
	DECLARE_DYNAMIC(CListLog)

public:
	CListLog();
	virtual ~CListLog();

	BOOL InitialList();												// 初始化列表控件
	BOOL AddLog(const UINT uType, const CString &szInfo);			// 增加日志

protected:
	DECLARE_MESSAGE_MAP()
	static CString szType[10];

protected:
	SYSTEMTIME	m_sysTime;
	CString		m_szTime;
	CString		m_szType;
	CString		m_szInfo;
	COLORREF	m_clText[10];

public:
	virtual void Serialize(CArchive& ar);
	afx_msg void OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult );
};


