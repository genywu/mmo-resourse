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

#ifndef _SELECTION_H_
#define _SELECTION_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <list>
#include <vector>
#include "WindowBox.h"

//////////////////////////////////////////////////////////////////////////
// SELECTION
//////////////////////////////////////////////////////////////////////////

/** This class maintains one ore more selected GUI windows, which the user can then modify as a 
 * group. Selected windows get "boxed" in @link=WindowBox}s, which provide a 
 * Powerpoint-like way to drag and resize the windows. An example of a class which uses a Selection to modify 
 * window properties, is the {@link=EditorDocument} class.
 */
class Selection 
{
public:

    /************************************************************************
     * PUBLIC TYPEDEFS
     ************************************************************************/
    
    /** Helper types.*/
    typedef std::list<WindowBox>	Boxes;

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Returns the number of selected windows.*/
    size_t Size() const;

    /** Selects the given window, when it is not selected yet.
     * @param aWindow the window to add to the selection.
     */
    bool Select(CEGUI::Window* aWindow);

	/** Selects all windows, starting with the given window. Only the given
	 * window will be resizable and such. Children will get a non-resizable 'box'.
	 * @param aWindow the window which is the start of our SelectAll process.
	*/
	bool SelectAll(CEGUI::Window* aWindow);

	/** Removes the given window from current selection.
     * @param aWindow the element to remove from the selection and resizers.
     */
    void Unselect(const CEGUI::Window* const aWindow);

    /** Returns whether the given window is currently selected.
     * @param aWindow the window which should be checked for being selected.
     */
    bool IsSelected(const CEGUI::Window* const aWindow);

    /** Unselects the entire current selection.*/
    void ClearSelection () ;

	/** Refreshes the boxes.*/
	void Update();

    /** @Returns the collection of ALL selected windows.*/
    Boxes& GetAllBoxes() 
    {
        return m_allBoxes;
    }

	/** Returns the collection of Boxes whos positions may be updated. To avoid dragging
	both parents and children, cause it would have a double effect on the children.*/
	Boxes& GetMoveableBoxes()
	{
		return m_moveableBoxes;
	}

    /** @Returns the box of the given window. Or NULL.*/
    WindowBox* FindBoxFor(const CEGUI::Window* const aWindow);

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Current (boxed) selection. Cleared when another overlay is being activated, or when
     * another element is being selected without shift or ctrl being pressed.*/
    Boxes m_allBoxes;	// Contains all selected windows, used for copy & pasting
	Boxes m_moveableBoxes;	// Only contains windows which may be moved (parents)

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/

	/** Selects children of the given window (recursive)*/
	void SelectChildren(const CEGUI::Window* aWindow);

	/** Unselects children of the given window (recursive)*/
	void UnselectChildren(const CEGUI::Window* aWindow);

	/** Clears the given windowBox which stored the given window, from the given container.*/
	bool DeleteWindowBox(Boxes& aBoxes, const CEGUI::Window* aWindow);

	/** Recalculates the resize boxes.*/
	void ResetBoxes(Boxes& aBoxes);
    
	/** Returns whether the given element is a parent or a child of one of the selected elements. Needed
     * to avoid problems when a parent and its child are 'dragged' at the same time. The child would get double
     * changes, because the parent forwards it already...
     *
     * @param aWindow the window to check.
     */
    bool IsParentOrChild(const CEGUI::Window* const aWindow);    
};

#endif // _SELECTION_H_
