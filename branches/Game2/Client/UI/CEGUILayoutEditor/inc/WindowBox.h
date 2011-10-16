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

#ifndef _WINDOW_BOX_H_
#define _WINDOW_BOX_H_

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

/** The resize box exists of 8 points.*/
#define NUM_RESIZE_POINTS 8

/** Locations of the points.*/
#define RESIZE_POINT_WN 0   // West-North
#define RESIZE_POINT_N  1   // North
#define RESIZE_POINT_NE 2   // North-East
#define RESIZE_POINT_E  3   // East
#define RESIZE_POINT_ES 4   // East-South
#define RESIZE_POINT_S  5   // South
#define RESIZE_POINT_SW 6   // South-West
#define RESIZE_POINT_W  7   // West

/** The resize box has 4 cardinal directions.*/
#define NUM_RESIZE_DIRS 4

/** Locations of the directions.*/
#define RESIZE_DIR_N  0   // North
#define RESIZE_DIR_E  1   // East
#define RESIZE_DIR_S  2   // South
#define RESIZE_DIR_W  3   // West

/** Value indicating that the mouse is not hovering on a resize point.*/
#define RESIZE_POINT_NONE -1

//////////////////////////////////////////////////////////////////////////
// WINDOWBOX
//////////////////////////////////////////////////////////////////////////

/** Class which wraps a selected Window. Provides for resizing and repositioning of that window,
* besides it holds whether the window's properties may be updated (the 'locked' member.)*/
class WindowBox
{

public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor, accepts a window and a setting whether it can be resized.*/
    WindowBox(CEGUI::Window* aWindow, bool aResizable = true);

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/

	// Operator to help STL::list::remove
	bool operator==(WindowBox& aBox);
    
    /** Re-positions the resize-points. Used when the resizer is initialized or when the 
    * contained GuiElement has been resized.*/
    void Reset();

    /** Returns the window to which this resizer is attached.*/
    CEGUI::Window* GetWindow()
    {
        return m_boxedWindow;
    }

    /** Checks if the mouse is within a resize point. If so, it returns the point's index (0-7).*/
    int GetPointAtPosition(const CEGUI::Point& point) const;

    /** Returns an absolute rectangle for the given point.
     *
     * @param iIndex the point (0-7).
     * @return absolute rectangle.
     */
    wxRect GetPointRect(const int anIndex) const;

    /** Returns a point for the given side.
     * If udimOrigin is set we actually return the point corresponding
     * to the udim position with 0 scale.  That point is of particular
     * significance because it is the point we 'offset' from.
     *
     * @param anDir the point (0-3).
     * @param udimOrigin if true, finds the udim origin position
     * @return absolute point.
     */
    wxPoint GetSidePoint(const int anDir, const bool udimOrigin) const;

    /** Returns an absolute rectangle for the boxed Window.
     *
     * @return absolute rectangle.
     */
    wxRect GetWindowRect() const;

    /** Applies a new rectangle to the boxed window, and then updates the box itself.*/
    void SetNewWindowArea(const CEGUI::URect &rect);
    void SetNewWindowArea(int x, int y, int width, int height);

    /** Applies a new position to the boxed window, and then updates the box itself.*/
    void SetNewWindowPosition(const CEGUI::UVector2& newPosition);

    /** Locks or unlocks the boxed window from allowing its properties to be changed.*/
    void Lock(bool lock = true)
    {
        m_locked = lock;
    }

    /** Returns the lock state of the boxed window.*/
    bool IsLocked()
    {
        return m_locked;
    }

	/** Returns the resize state of the boxed window.*/
    bool IsResizable()
    {
        return m_resizable;
    }
private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** The windows around which this resize box resides.*/
    CEGUI::Window*    m_boxedWindow ;

    /** 8 CEGUI rectangles with absolute and unclipped positions and sizes per point.*/
    CEGUI::Rect       m_resizePoints [NUM_RESIZE_POINTS] ;

    /** Holds whether the boxed window is locked. When true, its position and size can not be changed.
     * in the future, each window should be wrapped in this box so that locking information can also be remembered,
     * when a window is not selected anymore. Then add methods for "setSelected" and such.*/
    bool			m_locked;
	bool			m_resizable;	// False when selected but not resizable. Happens when a parent is selected.
};

#endif // _WINDOW_BOX_H_
