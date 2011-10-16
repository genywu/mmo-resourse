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
#include "WindowBox.h"
#include "CELayoutEditor.h"

//////////////////////////////////////////////////////////////////////////
// WINDOWBOX CONSTRUCTORS/DESTRUCTORS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
WindowBox::WindowBox (CEGUI::Window* aWindow, bool aResizable) :
m_boxedWindow(aWindow),
m_resizable(aResizable),
m_locked(false)
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Validations
    wxASSERT_MSG(aWindow != NULL, wxT("NULL windows cannot be boxed."));

	LogDebug(wxT("Boxing '%s'"), StringHelper::ToWXString(m_boxedWindow->getName()).c_str());

    // Calculate the point positions
    Reset ();
}

//////////////////////////////////////////////////////////////////////////
// WINDOWBOX FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
bool WindowBox::operator==(WindowBox& aBox)
{
	return (m_boxedWindow == aBox.m_boxedWindow);
}

//------------------------------------------------------------------------
void WindowBox::Reset () 
{
    m_locked = false;
    // Find the amount to convert from relative- to screen coordinates
    // Do this by finding the parent of the element (until no parent)
    const CEGUI::Rect winRc = m_boxedWindow->getUnclippedOuterRect();

    // Make boxes 3x3 and always in pixels
    //int width = 7;
    //int height = 7;
    const int paddX = 3;
    const int paddY = 3;

    // Position the "TopLeft" resizer
    m_resizePoints [RESIZE_POINT_WN].d_left = winRc.d_left - paddX;
    m_resizePoints [RESIZE_POINT_WN].d_top = winRc.d_top - paddY;
    // Position the "Top" resizer
    m_resizePoints [RESIZE_POINT_N].d_left = winRc.d_left + winRc.getWidth() / 2 - paddX;
    m_resizePoints [RESIZE_POINT_N].d_top = winRc.d_top - paddY;
    // Position the "TopRight" resizer
    m_resizePoints [RESIZE_POINT_NE].d_left = winRc.d_left + winRc.getWidth() - paddX;
    m_resizePoints [RESIZE_POINT_NE].d_top = winRc.d_top - paddY;
    // Position the "Right" resizer
    m_resizePoints [RESIZE_POINT_E].d_left = winRc.d_left + winRc.getWidth() - paddX;
    m_resizePoints [RESIZE_POINT_E].d_top = winRc.d_top + winRc.getHeight() / 2 - paddY;
    // Position the "BottomRight" resizer
    m_resizePoints [RESIZE_POINT_ES].d_left = winRc.d_left + winRc.getWidth() - paddX;
    m_resizePoints [RESIZE_POINT_ES].d_top = winRc.d_top + winRc.getHeight() - paddY;
    // Position the "Bottom" resizer
    m_resizePoints [RESIZE_POINT_S].d_left = winRc.d_left + winRc.getWidth() / 2 - paddX;
    m_resizePoints [RESIZE_POINT_S].d_top = winRc.d_top + winRc.getHeight() - paddY;
    // Position the "BottomLeft" resizer
    m_resizePoints [RESIZE_POINT_SW].d_left = winRc.d_left - paddX;
    m_resizePoints [RESIZE_POINT_SW].d_top = winRc.d_top + winRc.getHeight() - paddY;
    // Position the "Left" resizer
    m_resizePoints [RESIZE_POINT_W].d_left = winRc.d_left - paddX;
    m_resizePoints [RESIZE_POINT_W].d_top = winRc.d_top + winRc.getHeight() / 2 - paddY;

    for (int i = 0; i < NUM_RESIZE_POINTS; ++i) 
    {
        m_resizePoints [i].d_right = m_resizePoints [i].d_left + (paddX * 2.0f) ;
        m_resizePoints [i].d_bottom = m_resizePoints [i].d_top + (paddY * 2.0f) ;
    }
}

//------------------------------------------------------------------------
wxRect WindowBox::GetPointRect (const int anIndex) const 
{
    const CEGUI::Rect rc = m_resizePoints [anIndex];
    return wxRect (wxPoint(wx_static_cast(int, rc.d_left), wx_static_cast(int, rc.d_top)), wxPoint(wx_static_cast(int, rc.d_right), wx_static_cast(int, rc.d_bottom)));
}

//------------------------------------------------------------------------
wxPoint WindowBox::GetSidePoint(const int anDir, const bool udimOrigin) const
{
    wxASSERT_MSG(m_boxedWindow != NULL, wxT("Boxed Window must exist"));
    CEGUI::UVector2 boxPos;
    CEGUI::UDim hwidth;
    CEGUI::UDim hheight;
    switch (anDir)
    {
    case RESIZE_DIR_N:
        boxPos.d_x = m_boxedWindow->getXPosition();
        boxPos.d_y = m_boxedWindow->getYPosition();
        boxPos.d_x += m_boxedWindow->getWidth()/CEGUI::UDim(2.0f,2.0f);
        if (udimOrigin)
        {
            boxPos.d_y.d_offset = 0;
        }
        break;
    case RESIZE_DIR_W:
        boxPos.d_x = m_boxedWindow->getXPosition();
        boxPos.d_y = m_boxedWindow->getYPosition();
        boxPos.d_y += m_boxedWindow->getHeight()/CEGUI::UDim(2.0f,2.0f);
        if (udimOrigin)
        {
            boxPos.d_x.d_offset = 0;
        }
        break;
    case RESIZE_DIR_S:
        boxPos.d_x = m_boxedWindow->getXPosition();
        boxPos.d_y = m_boxedWindow->getYPosition();
        boxPos.d_x += m_boxedWindow->getWidth()/CEGUI::UDim(2.0f,2.0f);
        boxPos.d_y += m_boxedWindow->getHeight();
        if (udimOrigin)
        {
            boxPos.d_y.d_offset = 0;
        }
        break;
    case RESIZE_DIR_E:
        boxPos.d_x = m_boxedWindow->getXPosition();
        boxPos.d_y = m_boxedWindow->getYPosition();
        boxPos.d_x += m_boxedWindow->getWidth();
        boxPos.d_y += m_boxedWindow->getHeight()/CEGUI::UDim(2.0f,2.0f);
        if (udimOrigin)
        {
            boxPos.d_x.d_offset = 0;
        }
        break;
    default:
        LogError(wxT("Invalid resize direction (%d) supplied"), anDir);
        return wxPoint(-1, -1);
    };
    // Now find it
    CEGUI::Vector2 absPos;
    if (m_boxedWindow->getParent())
    {
        absPos = CEGUI::CoordConverter::windowToScreen(*m_boxedWindow->getParent(), boxPos);
    }
    else
    {
        // hmm what to do here?
        absPos = CEGUI::CoordConverter::windowToScreen(*m_boxedWindow, boxPos);
    }
    return wxPoint(wx_static_cast(int, absPos.d_x), wx_static_cast(int, absPos.d_y));
}

//------------------------------------------------------------------------
wxRect WindowBox::GetWindowRect() const
{
    const CEGUI::Rect rc = m_boxedWindow->getUnclippedOuterRect();
    return wxRect (wxPoint(wx_static_cast(int, rc.d_left), wx_static_cast(int, rc.d_top)), wxPoint(wx_static_cast(int, rc.d_right), wx_static_cast(int, rc.d_bottom)));
}

//------------------------------------------------------------------------
int WindowBox::GetPointAtPosition (const CEGUI::Point& aPoint) const
{
    // If the window is locked or not resizable, we just cheat the Document
    if (m_locked || !m_resizable)
    {
        return RESIZE_POINT_NONE;
    }
    for (int i = 0; i < NUM_RESIZE_POINTS; ++i) 
    {
        if (m_resizePoints [i].isPointInRect (aPoint))
        {
            return i ;
        }    
    }
    return RESIZE_POINT_NONE;
}

void WindowBox::SetNewWindowArea(const CEGUI::URect &rect)
{
    m_boxedWindow->setArea(rect);
    // Update the resizer positions
    Reset();
}
//------------------------------------------------------------------------
void WindowBox::SetNewWindowArea(int x, int y, int width, int height)
{
    // Apply it to the CEGUI window
    CEGUI::Vector2 pos(x,y);
    CEGUI::Vector2 UIPOS = CEGUI::CoordConverter::screenToWindow(*m_boxedWindow,pos);
    
    
    
    // Update the resizer positions
    Reset();
}

//------------------------------------------------------------------------
void WindowBox::SetNewWindowPosition(const CEGUI::UVector2& newPosition)
{
    // Apply it to the CEGUI window
    m_boxedWindow->setPosition(newPosition);
    // Update the resizer positions
    Reset();
}
