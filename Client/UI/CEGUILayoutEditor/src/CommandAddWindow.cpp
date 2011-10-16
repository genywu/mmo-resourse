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

#include "CELayoutEditor.h"
#include "CommandAddWindow.h"

//////////////////////////////////////////////////////////////////////////
// COMMANDADDWINDOW FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
CommandAddWindow::CommandAddWindow (EditorDocument* aDocument, CEGUI::Window* aParent, const CEGUI::String& aType, const CEGUI::String& aName) :
// Just copy all parameters and wait for "Do".
mDocument(aDocument),
mParent(aParent),
mType(aType),
mName(aName),

// Store for checking existence
mParentName(aParent != NULL ? aParent->getName() : ""),

// Nothing created yet
mCreatedWindow(wx_static_cast(CEGUI::Window *, NULL)),

// This command can be undone
wxCommand(true, wxT("Add Window"))
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Validations
    wxASSERT_MSG(aDocument != NULL, wxT("Supplied document is NULL"));
}

//------------------------------------------------------------------------
bool CommandAddWindow::Do() 
{
    //LogDebug(wxT("== CommandAddWindow::Do/Redo =="));
    //LogDebug(wxT(" Parent: %s"), StringHelper::ToWXString(mParentName));
    //LogDebug(wxT(" Type: %s"), StringHelper::ToWXString(mType));
    //LogDebug(wxT(" Name: %s"), StringHelper::ToWXString(mName));

    // In case of re-do, only possible when the parent is still in the layout(!)
    if (CEGUI::WindowManager::getSingleton().isWindowPresent(mParentName))
    {
        // Validations
        wxASSERT_MSG(mDocument != NULL, wxT("Document member is NULL"));

        // Add the window with the given parameters
        mCreatedWindow = mDocument->AddWindow(mParent, mType, mName);
        return (mCreatedWindow != NULL);
    }

    // Could not (re)do
    LogWarning (wxT("Failed to (re)do - original parent does not exist anymore."));
    return false;
}

//------------------------------------------------------------------------
bool CommandAddWindow::Undo() 
{
    // Delete the window which was created during "Do".
    if (mCreatedWindow)
    {
        // Validations
        wxASSERT_MSG(mDocument != NULL, wxT("Document member is NULL"));

        // Remove + notify
        (void)mDocument->RemoveWindow (mCreatedWindow, true);
        mCreatedWindow = NULL;
        return true;
    }

    return false;
}
