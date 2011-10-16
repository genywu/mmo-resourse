// TreeToolTipCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeToolTipCtrl.h"
#include "PackageToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeToolTipCtrl

CTreeToolTipCtrl::CTreeToolTipCtrl()
{
}

CTreeToolTipCtrl::~CTreeToolTipCtrl()
{
}


BEGIN_MESSAGE_MAP(CTreeToolTipCtrl, CToolTipCtrl)
	//{{AFX_MSG_MAP(CTreeToolTipCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeToolTipCtrl message handlers

void CTreeToolTipCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPackageToolDoc * pDocs;		// Pointer to My Pack Document
	// Gets a pointer to document template
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pgDocTemplate=AfxGetApp()->GetNextDocTemplate(pos);
	// Gets a pointer to document
	pos=pgDocTemplate->GetFirstDocPosition();
	CDocument* pDoc=pgDocTemplate->GetNextDoc(pos);			
	// assign Document to Pack Doc
	pDocs=(CPackageToolDoc*)pDoc;			
	
	Pop();

	CToolTipCtrl::OnLButtonDown(nFlags, point);
}

