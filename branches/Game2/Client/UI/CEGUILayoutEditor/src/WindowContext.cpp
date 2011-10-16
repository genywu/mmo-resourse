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

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "WindowContext.h"

//////////////////////////////////////////////////////////////////////////
// WINDOWCONTEXT FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
WindowContext::WindowContext (CEGUI::Window* pWindow) :
m_pWindow(pWindow)
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
}

//------------------------------------------------------------------------
void WindowContext::SetWindow (CEGUI::Window* pWindow)
{
    m_pWindow = pWindow;
}

//------------------------------------------------------------------------
bool WindowContext::AcceptsWindowAsChild() const
{
    // Validations
    wxASSERT_MSG(m_pWindow != NULL, wxT("Window member is NULL"));

    const CEGUI::String strWindowType = m_pWindow->getType();

    // These require different parent / child handling.
    // The current type must not be equal to the checks below
    // Because of the "find" instead of exact matches, it works for different
    // looknfeels, e.g. both "TaharezLook/Combobox" and "Windowslook/Combobox".
    return strWindowType.find("Combobox") == CEGUI::String::npos &&
        strWindowType.find("ComboDropList") == CEGUI::String::npos &&
        strWindowType.find("ListHeader") == CEGUI::String::npos &&
        strWindowType.find("Combobox") == CEGUI::String::npos &&
        strWindowType.find("ListBox") == CEGUI::String::npos &&
        strWindowType.find("MultiColumnList");
}
