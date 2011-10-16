// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BB9CC6D5_27C9_11D3_ACCC_00104B247756__INCLUDED_)
#define AFX_STDAFX_H__BB9CC6D5_27C9_11D3_ACCC_00104B247756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>
#include <afxpriv.h>
#include <urlmon.h>
#include <AFXINET.H>

#include <time.h>
#include <mmsystem.h>
#include <process.h>
#include <assert.h>

#include <deque>

using namespace std;

extern CWnd * wnd;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BB9CC6D5_27C9_11D3_ACCC_00104B247756__INCLUDED_)
