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

#ifndef _SELECTION_MOVER_H_
#define _SELECTION_MOVER_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Selection.h"

//////////////////////////////////////////////////////////////////////////
// SELECTIONMOVER
//////////////////////////////////////////////////////////////////////////

/** This class provides several operation to modify a selection of windows.*/
class SelectionMover
{
public:

    /************************************************************************
     * PUBLIC ENUMS
     ************************************************************************/
    
    enum Alignment { AlignLeft, AlignRight, AlignTop, AlignBottom, AlignWidth, 
        AlignHeight
    };
    enum PaddingAlign{HorzPadding,VertPadding};
    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    SelectionMover(Selection *);

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    void AlignSelection(const Alignment alignment);

    //2010.9.3 nikai 
    void AlignSpaceBetween(int PaddingValue, const PaddingAlign al);

    /** Updates each window area in the current selection. It only updates the 'scales'; the 'offsets' can only be changed
     * in the property grid in the main dialog.*/
    void UpdateWindowAreas(float left, float top, float right, float bottom);

    /** Snaps the current selection to the current grid. The grid does not have to be visible.
     */
    void SnapToGrid();

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    /** Move using scale
     */
    void MoveUsingScale(const CEGUI::Window* const window, CEGUI::URect& windowArea, const float left, const float top, const float right, const float bottom) const;

    /** Move using offset
     */
    void MoveUsingOffset(CEGUI::URect& windowArea, const float left, const float top, const float right, const float bottom) const;
    
    Selection *m_selection;
};

#endif // _SELECTION_MOVER_H_
