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

// Avoids the "identifier was truncated to '255' characters in the debug information" warning,
// which occurs when converting between the strings sometimes.
#pragma warning (disable:4786)

#ifndef _PCH_
#define _PCH_

// Generic includes
#include "Options.h"

// CEGUI core + renderer includes
#include "CEGUI.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"

// Standard wxWidget includes
#include "wx/wx.h"
// More specific wxWidgets includes
#include "wx/glcanvas.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"

#include "wx/tokenzr.h"

// Utility class for conversion between CEGUI- and wxWidgets strings
#include "StringHelper.h"

#endif // _PCH_
