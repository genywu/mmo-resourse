///////////////////////////////////////////////////////////////////////////////
//  For project details and authors, refer to "CELayoutEditor.cpp".
//
//  This file is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//  To view the licence online, go to: http://www.gnu.org/copyleft/gpl.html
////////////////////////////////////////////////////////////////////////////////

#ifndef _BUILD_CHECKS_H_
#define _BUILD_CHECKS_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Config.h"

//////////////////////////////////////////////////////////////////////////
// VISUAL STUDIO BUILD CHECKS
//////////////////////////////////////////////////////////////////////////

// Check if current VS version is supported
// NOTE:  The non-existence of _MSC_VER is a clear indication Visual Studio is not being 
// used. Therefore, we can safely compare the version directly, since for other compilers
// this will be ignored
#if defined(_MSC_VER) && _MSC_VER < VISUAL_STUDIO_7_1 
    #if _MSC_VER == VISUAL_STUDIO_7_0
        COMPILE_WARN("VC++7.0 (VS2002) contains some serious bugs. It is highly recommended users upgrade to VS2003 or VS2005!")
    #else
        COMPILE_WARN("VC++6 (or lower) support has been discontinued. You might run into unexpected problems. An upgrade to VS2003 or VS2005 is highly recommended!")
    #endif // _MSC_VER == VISUAL_STUDIO_7_0
#endif // _MSC_VER < VISUAL_STUDIO_7_1 

//////////////////////////////////////////////////////////////////////////
// WXWIDGETS BUILD CHECKS
//////////////////////////////////////////////////////////////////////////

// Verify wxWidgets version
#if !wxCHECK_VERSION(2,8,0)
    COMPILE_WARN("CELayoutEditor expects wxWidgets 2.8.* or higher. Lower versions of the library might conduct to errors!")
#endif // !wxCHECK_VERSION(2,8,0)

// Make sure OpenGL canvas is being used
#if !defined(wxUSE_GLCANVAS) || wxUSE_GLCANVAS != 1
    #error "CELayoutEditor requires wxWidgets OpenGL support (wxUSE_GLCANVAS == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_GLCANVAS) || wxUSE_GLCANVAS != 1

// Make sure exceptions are being used
#if !defined(wxUSE_EXCEPTIONS) || wxUSE_EXCEPTIONS != 1
    #error "CELayoutEditor requires wxWidgets exception support (wxUSE_EXCEPTIONS == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_EXCEPTIONS) || wxUSE_EXCEPTIONS != 1

// Make sure the about box is being used
#if !defined(wxUSE_ABOUTDLG) || wxUSE_ABOUTDLG != 1
#error "CELayoutEditor requires wxWidgets about box support (wxUSE_ABOUTDLG == 1). Check readme.txt for more info."
#endif // !defined(wxUSE_ABOUTDLG) || wxUSE_ABOUTDLG != 1

#endif // _BUILD_CHECKS_H_
