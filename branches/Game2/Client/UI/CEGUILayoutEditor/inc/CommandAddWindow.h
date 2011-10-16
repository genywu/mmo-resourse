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

#ifndef _COMMAND_ADD_WINDOW_H_
#define _COMMAND_ADD_WINDOW_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "EditorDocument.h"

//////////////////////////////////////////////////////////////////////////
// COMMANDADDWINDOW
//////////////////////////////////////////////////////////////////////////

/** This command adds a window in the "do" and "redo" mode, and destroys it
* in the "undo" mode.
*/
class CommandAddWindow : public wxCommand
{
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor.
     * @param aDocument the document on which to apply the command.
     * @param aParent the parent window to which the new window must be added.
     * @param aType the type of the new window, e.g. "Button".
     * @param aName the name of the new window, e.g. "HealthBar".
     */
    CommandAddWindow (EditorDocument* aDocument, CEGUI::Window* aParent, const CEGUI::String& aType, const CEGUI::String& aName);

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** "Does" or "Re-does" the command.*/
    bool Do();

    /** "Un-does" the command.*/
    bool Undo();

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Stored constructor parameter.*/
    EditorDocument*			mDocument;

    /** Stored constructor parameter.*/
    CEGUI::Window*			mParent;

    /** Stored constructor parameter.*/
    CEGUI::String				mType;

    /** Stored constructor parameter.*/
    CEGUI::String				mName;

    /** Stores the parent's name, so we can check whether it still exists,
    * without running into a null-pointer error when the parent is deleted.*/
    CEGUI::String				mParentName;

    /** Stores the created window for Undoing.*/
    CEGUI::Window*			mCreatedWindow;

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Block default construction not possible.*/
    CommandAddWindow();  
};

#endif // _COMMAND_ADD_WINDOW_H_
