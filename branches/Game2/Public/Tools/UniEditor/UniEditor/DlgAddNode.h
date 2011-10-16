#pragma once
#include "afxwin.h"
#include "RegionTree.h"

// CDlgAddNode 对话框

typedef list<HTREEITEM>				LISTITEM;
typedef list<HTREEITEM>::iterator	ITRITEM;

class CDlgAddNode : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddNode)

public:
	CDlgAddNode(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddNode();

	const inline LISTITEM GetListItem()const {return m_listItem;}

// 对话框数据
	enum { IDD = IDD_ADDNODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	enum { RADIO_MONSTER, RADIO_NPC, RADIO_MONINFO };

	LISTREGION		m_RegionList;
	CListBox		m_listRegion;
	CListBox		m_listRegionAdd;
	CListBox		m_listMonster;
	CListBox		m_listMonsterAdd;
	int				m_uRadio;
	CWnd			*m_pWnd;
	CRegionTree		*m_pRegionTree;
	CMonster		*m_pMonster;
	HTREEITEM		m_hItemFirstRegion;
	LISTITEM		m_listItem;
public:

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnLbnDblclkListregion();
public:
	afx_msg void OnLbnDblclkListregionadd();
public:
	afx_msg void OnBnClickedButadd();
public:
	afx_msg void OnBnClickedButdel();
public:
	afx_msg void OnLbnDblclkListmonster();
public:
	afx_msg void OnLbnDblclkListmonsteradd();
public:
	afx_msg void OnBnClickedButaddM();
public:
	afx_msg void OnBnClickedButdelM();
protected:
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
