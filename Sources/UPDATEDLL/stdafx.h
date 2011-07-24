// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__465EAB58_4316_4ADF_A9C0_C901BF969BE1__INCLUDED_)
#define AFX_STDAFX_H__465EAB58_4316_4ADF_A9C0_C901BF969BE1__INCLUDED_


#ifndef BEGIN_INTERFACE

#ifdef __KS_VC_COMPATIBILITY_GCC__
#ifndef BEGIN_INTERFACE
   #define BEGIN_INTERFACE virtual void __KS_VC_COMPATIBILITY_GCC__a() {} virtual void __KS_VC_COMPATIBILITY_GCC__b() {}
#endif
#else   // __KAV_VC_COMP_GCC__
#ifndef BEGIN_INTERFACE
   #define BEGIN_INTERFACE
#endif
#endif  // __KAV_VC_COMP_GCC__

#endif  // BEGIN_INTERFACE

#ifndef END_INTERFACE
#define END_INTERFACE
#endif

#ifndef KSIDL_DEFINE

#ifdef _XP_LINUX_

// Add Header file for Linux 

#else   //_XP_LINUX_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef WIN32_LEAN_AND_MEAN
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#endif

// TODO: reference additional headers your program requires here
#ifdef _XP_WIN32_MFC_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxsock.h>		// MFC socket extensions
#else  // !_XP_WIN32_MFC_

#include "rpc.h"
#include "rpcndr.h"
#include "windows.h"
#include "ole2.h"

#endif  // _XP_WIN32_MFC_

#endif  // _XP_LINUX_

#endif // KSIDL_DEFINE

#include "KSCOMDef.h"
#include "KAVPublic.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__465EAB58_4316_4ADF_A9C0_C901BF969BE1__INCLUDED_)
