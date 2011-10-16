#if !defined(AFX_TREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_)
#define AFX_TREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TreeListCtrl.h : header file
//

/*
	Code written by Gary Andrew Lewis
	Please include these comments in your code if you wish to 
	use this control.  Thank you!
	
	Any questions then email: realm_123@hotmail.com 
*/

#include "TreeListHeaderCtrl.h"

struct SSortType
{
	int nCol;
	BOOL bAscending;
};

class CTLItem
{
//private:
public:
	CString m_itemString;

public:
	CTLItem();
	CTLItem(CTLItem &copyItem);
	DWORD itemData;

	char m_cEnding;

	// visual attributes
	BOOL m_Bold;
	COLORREF m_Color;

	// editable attributes
	BOOL m_bIsEditable;
	BOOL m_bHasCombo;

	//是否是文件夹
	BOOL m_bIsFolder;

	// m_nSub is zero-based
	CString GetItemString() { return m_itemString; };
	CString GetSubstring(int m_nSub);
	CString GetItemText() { return GetSubstring(0); };
	BOOL IsEditable() { return m_bIsEditable; }; 
	BOOL HasCombo() { return m_bHasCombo; };
	BOOL IsFolder()	{return m_bIsFolder;}
	
	void SetSubstring(int m_nSub, CString m_sText);
	void InsertItem(CString m_sText) { SetSubstring(0, m_sText); };
	void SetEditable(BOOL bEditable);
	void SetCombo(BOOL bCombo);
	void SetFolder(BOOL bFolder);
};

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window

#define UWM_TV_CHECKBOX    WM_APP   

class CTreeListCtrl : public CTreeCtrl
{
// Construction
public:
	CTreeListCtrl();

// Attributes
private:	
	int m_nColumns;
	int m_nColumnsWidth;
	int m_nItems;
	CDWordArray	m_dwaItems;
	CStringArray TreeLayout;
	UINT	m_nHoverTimerID;
	CArray<CTLItem*,CTLItem*> MemItemArray;
	COLORREF m_wndColor;
	CEdit *pEdit;
	CComboBox *pCombo;

public:
	CTreeListHeaderCtrl m_wndHeader;
	CFont m_headerFont;
	CImageList m_cImageList;
	CImageList m_StatusImageList;
	
	int m_nOffset;

	enum CheckState{ NOSTATE = 0, UNCHECKED, CHECKED, CHILD_CHECKED, 
		SELF_AND_CHILD_CHECKED };
	enum CheckType{ CHECK, UNCHECK, TOGGLE, REFRESH };




// Operations
public:

	int GetColumnsNum() { return m_nColumns; };
	int GetColumnsWidth() { return m_nColumnsWidth; };
	int GetItemCount() { return m_nItems; };
	void RecalcColumnsWidth();
	
	void ResetVertScrollBar();

	HTREEITEM GetTreeItem(int nItem);
	int GetListItem(HTREEITEM hItem);

	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	int GetColumnWidth(int nCol);
	int GetColumnAlign(int nCol);

	BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
	DWORD GetItemData(HTREEITEM hItem) const;

	HTREEITEM GetItemByText(CString& str);
	HTREEITEM FindChildItemByText(HTREEITEM htParentItem,CString& str);

	CString GetItemText( HTREEITEM hItem, int nSubItem = 0 );
	CString GetItemText( int nItem, int nSubItem );
	CString GetItemTextWithParent(HTREEITEM hItem, int nSubItem=0);

	HTREEITEM InsertItem( LPCTSTR lpszItem, BOOL bFolder,int nImage, int nSelectedImage, BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, BOOL bFolder,BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, BOOL bFolder,int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE);
	HTREEITEM InsertItemByStr(LPCTSTR lpszItem,int nImage,int nSelectedImage,BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);

	HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);

	BOOL DeleteSubItems( HTREEITEM hItem );
	BOOL DeleteItem( HTREEITEM hItem );
	BOOL DeleteItem( int nItem );

	void MemDeleteAllItems(HTREEITEM hParent);
	BOOL DeleteAllItems();

	BOOL SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem );
	BOOL SetItemFolder(HTREEITEM hItem,BOOL bFolder);

	BOOL SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate = TRUE);
	BOOL SetItemBold( HTREEITEM hItem, BOOL m_Bold = TRUE, BOOL m_bInvalidate = TRUE );

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	BOOL SortItems( int nCol, BOOL bAscending, HTREEITEM low);

	void DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrl)	
	//}}AFX_VIRTUAL

// Implementation
public:	
	void ShowEditComboBoxes(POINT point);
	void DeleteBoxes();
	void SetCombo(HTREEITEM hItem, BOOL bHasCombo);
	void SetEditable(HTREEITEM hItem,BOOL bIsEditable);
	void SetGridStyle(UINT style);
	COLORREF GetGridColor();
	void SetGridColor(COLORREF color);
	BOOL HasGridLines();
	void SetGridLines(BOOL hasLines);
	void SetBackgroundColor(COLORREF color);
	int WhichCheck(HTREEITEM hItem);
	void SetChecked(HTREEITEM hItem,int Checked);
	void UpdateDisplay();
	BOOL LocatePickLists(CString CustNum,BOOL HighlightLast,int ReturnToPickID);
	CString GetNextLine(CString &message);
	CString Return_SelectedCustNum();
	int Return_SelectedPick();	
	BOOL IsInPickingList(CString csItem,CString OrdNum);
	virtual ~CTreeListCtrl();

	void AddNumber(HTREEITEM htItem,int iCol,int iSize);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();			
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);		
	afx_msg void OnTreelist2refresh();		
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg LRESULT OnTvCheckbox(WPARAM wp, LPARAM lp);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);			
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnEndComboedit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:		
	BOOL m_bPressedTick;
	BOOL m_bHasGridLines;
	COLORREF m_GridColor;
	UINT m_GridStyle;
	HTREEITEM m_selectedItem;
	
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_)
