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

#include "SelectionMover.h"
#include "CELayoutEditor.h"

//////////////////////////////////////////////////////////////////////////
// SELECTION MOVER FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
SelectionMover::SelectionMover(Selection *s)
: m_selection(s)
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
}

//------------------------------------------------------------------------
void SelectionMover::AlignSelection(const Alignment alignment)
{
    // Validations
    wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

    if(m_selection->Size() <= 1) 
    {
        // Should not happen because of the disabled menu/toolbar item in this case
        LogWarning(wxT("You must select more than one window to align!"));
        return;
    }

    // The first selected window is the one to match. This is for example how Visual Studio's 
    // dialog editor works as well.
    Selection::Boxes::iterator boxIt = m_selection->GetMoveableBoxes().begin();

    CEGUI::Window *current = boxIt->GetWindow();

    const CEGUI::URect rect = current->getArea();
    ++boxIt;

    for(; boxIt != m_selection->GetMoveableBoxes().end(); ++boxIt) {
        // Deny when it is blocked
        if (boxIt->IsLocked())
            continue;
        
        current = boxIt->GetWindow();
        
        switch(alignment)
        {
        case AlignLeft:
            current->setPosition(CEGUI::UVector2(rect.d_min.d_x, current->getPosition().d_y));
            break;
        case AlignRight:
            current->setPosition(CEGUI::UVector2(rect.d_max.d_x - current->getWidth(), current->getPosition().d_y));
            break;
        case AlignTop:
            current->setPosition(CEGUI::UVector2(current->getPosition().d_x, rect.d_min.d_y));
            break;
        case AlignBottom:
            current->setPosition(CEGUI::UVector2(current->getPosition().d_x, rect.d_max.d_y - current->getHeight()));
            break;
        case AlignWidth:
            // The svn diff for this block will be big; no clue what the previous code
            // was doing there..
            current->setWidth(rect.getWidth());
            break;
        case AlignHeight:
            // The svn diff for this block will be big; no clue what the previous code
            // was doing there..
            current->setHeight(rect.getHeight());
            break;
        default:
            LogError(wxT("SelectionMover::AlignSelection - Unrecognized align option (%d)"), alignment);
            return;
        }
    }
    // Request for a repaint
    wxGetApp().GetMainFrame()->Refresh();
}

//nikai 2010.9.3
void SelectionMover::AlignSpaceBetween(int PaddingValue, const PaddingAlign al)
{
    // Validations
    wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

    if(m_selection->Size() <= 1) 
    {
        // Should not happen because of the disabled menu/toolbar item in this case
        LogWarning(wxT("You must select more than one window to align!"));
        return;
    }
    // The first selected window is the one to match. This is for example how Visual Studio's 
    // dialog editor works as well.
    Selection::Boxes::iterator boxIt = m_selection->GetMoveableBoxes().begin();

    const CEGUI::Window *current = boxIt->GetWindow();
            
    CEGUI::Window *pParentWin = current->getParent();
    if(NULL == pParentWin)
    {
        LogWarning(wxT("You can't operate the root window!"));
        wxMessageBox(L"You can't operate the root window!");
        return;
    }
    const long ParentX = CEGUI::CoordConverter::windowToScreenX(*pParentWin,0);
    const long ParentY = CEGUI::CoordConverter::windowToScreenY(*pParentWin,0);
    const long parentWidth = pParentWin->getPixelSize().d_width;
    const long parentHeight = pParentWin->getPixelSize().d_height;

    const CEGUI::URect rect = current->getArea();
    ++boxIt;
    
    for(; boxIt != m_selection->GetMoveableBoxes().end(); ++boxIt) {
        // Deny when it is blocked
        if (boxIt->IsLocked())
        {
            wxMessageBox(L"all the select window ¡¯s can't be locked£¡");
            return;
        }
        CEGUI::Window* curWin = boxIt->GetWindow();
        if (pParentWin != curWin->getParent())
        {
            wxMessageBox(L"all the select window ¡¯s parent must be same£¡");
            return;
        }
    }
    //»Ö¸´µü´úÆ÷Î»ÖÃ
    //boxIt = m_selection->GetMoveableBoxes().begin(); 
    //++boxIt;
    //float lWidth  = current->getPixelSize().d_width;
    //float lHeight = current->getPixelSize().d_height;
    //float RightPos = CEGUI::CoordConverter::windowToScreenX(*current,0)+lWidth;
    //float BottomPos = CEGUI::CoordConverter::windowToScreenY(*current,0)+lHeight;
    //
    //for(; boxIt != m_selection->GetMoveableBoxes().end(); ++boxIt)
    //{
    //    CEGUI::Window* curWin = boxIt->GetWindow();
    //    CEGUI::UDim xPos = curWin->getXPosition();
    //    CEGUI::UDim yPos = curWin->getYPosition();
    //    switch(al)
    //    {
    //    case HorzPadding:
    //        {
    //            float newXPixel = RightPos + PaddingValue;
    //            float xscale = (newXPixel-ParentX)/parentWidth;
    //            xPos = CEGUI::UDim(xscale,0);
    //            break;
    //        }
    //    case VertPadding:
    //        {
    //            float newYPixel = BottomPos + PaddingValue;
    //            float yscale = (newYPixel-ParentY)/parentHeight;
    //            yPos = CEGUI::UDim(yscale,0);
    //            break;
    //        }
    //    default:
    //        LogError(wxT("SelectionMover::AlignSelection - Unrecognized align option (%d)"), al);
    //        return;
    //    }
    //    curWin->setPosition(CEGUI::UVector2(xPos,yPos));
    //    lWidth  = curWin->getPixelSize().d_width;
    //    lHeight = curWin->getPixelSize().d_height;
    //    RightPos = CEGUI::CoordConverter::windowToScreenX(*curWin,0)+lWidth;
    //    BottomPos = CEGUI::CoordConverter::windowToScreenY(*curWin,0)+lHeight;
    //}
    //// Request for a repaint
    //wxGetApp().GetMainFrame()->Refresh();
    boxIt = m_selection->GetMoveableBoxes().begin(); 
    ++boxIt;
    float newPixelWidth  = current->getPixelSize().d_width;
    float newPixelHeight = current->getPixelSize().d_height;
    float RightPos = CEGUI::CoordConverter::windowToScreenX(*current,0)+newPixelWidth;
    float BottomPos = CEGUI::CoordConverter::windowToScreenY(*current,0)+newPixelHeight;

    for(; boxIt != m_selection->GetMoveableBoxes().end(); ++boxIt)
    {
        CEGUI::Window* curWin = boxIt->GetWindow();
        float fLeft = CEGUI::CoordConverter::windowToScreenX(*curWin,0);
        float fTop  = CEGUI::CoordConverter::windowToScreenY(*curWin,0);
        newPixelWidth  = curWin->getPixelSize().d_width;
        newPixelHeight = curWin->getPixelSize().d_height;
        float fRight  = fLeft  + newPixelWidth;
        float fBottom = fTop + newPixelHeight;

        CEGUI::URect newWindowArea = curWin->getArea();
        float sLeft = 0.0f,sTop = 0.0f,sRight = 0.0f, sBottom = 0.0f;
        switch(al)
        {
        case HorzPadding:
            {
                float newXPixel = RightPos + PaddingValue;
                sRight = sLeft = newXPixel - fLeft;
                break;
            }
        case VertPadding:
            {
                float newYPixel = BottomPos + PaddingValue;
                sBottom = sTop = newYPixel - fTop;
                break;
            }
        default:
            LogError(wxT("SelectionMover::AlignSelection - Unrecognized align option (%d)"), al);
            return;
        }

        MoveUsingScale(curWin,newWindowArea,sLeft,sTop,sRight,sBottom);
        curWin->setArea(newWindowArea);
        fLeft = CEGUI::CoordConverter::windowToScreenX(*curWin,0);
        fTop  = CEGUI::CoordConverter::windowToScreenY(*curWin,0);
        RightPos  = fLeft + newPixelWidth;
        BottomPos = fTop  + newPixelHeight;
    }
    // Request for a repaint
    wxGetApp().GetMainFrame()->Refresh();
}
//------------------------------------------------------------------------
void SelectionMover::MoveUsingScale(const CEGUI::Window* const window, CEGUI::URect& windowArea, const float left, const float top, const float right, const float bottom) const
{
    // Validations
    wxASSERT_MSG(window != NULL, wxT("Supplied window is NULL"));

    // Calculate relative versions of the given values
    const float renderWidth = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_width;
    const float renderHeight = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_height;

    // We need to know how much 1 pixel means relatively for the current resolution
    float scaleDeltaLeft = left / renderWidth;
    float scaleDeltaTop = top / renderHeight;
    float scaleDeltaRight = right / renderWidth;
    float scaleDeltaBottom = bottom / renderHeight;

    CEGUI::Rect windowRect;
    if (window->getParent())
    {
        windowRect = CEGUI::CoordConverter::windowToScreen(*window->getParent(), windowArea);   // Pixels
    }
    else
    {
        windowRect = CEGUI::CoordConverter::windowToScreen(*window, windowArea);   // Pixels
    }


    // Avoid negative widths and heights
    if (windowRect.getWidth() + (right - left) > 1.0f && 
        windowRect.getHeight() + (bottom - top) > 1.0f)
    {
        const CEGUI::Window* parent = window->getParent();
        while (parent)
        {
            // Update relative values when a parent is set.
            // When a parent is set, make the difference relatively larger.
            if (parent->getWidth().d_scale)
                scaleDeltaLeft /= parent->getWidth().d_scale;
            if (parent->getHeight().d_scale)
                scaleDeltaTop /= parent->getHeight().d_scale;
            if (parent->getWidth().d_scale)
                scaleDeltaRight /= parent->getWidth().d_scale;
            if (parent->getHeight().d_scale)
                scaleDeltaBottom /= parent->getHeight().d_scale;
            // next
            parent = parent->getParent();
        }

        // In UDim land, there is no metrics mode anymore.
        // Only update the 'scale' part. The 'offset' part can only
        // be changed from the property grid in the main dialog.

        // Left
        windowArea.d_min.d_x.d_scale += scaleDeltaLeft;

        // Top
        windowArea.d_min.d_y.d_scale += scaleDeltaTop;

        // Right
        windowArea.d_max.d_x.d_scale += scaleDeltaRight;

        // Bottom
        windowArea.d_max.d_y.d_scale += scaleDeltaBottom;
    }
}

//------------------------------------------------------------------------
void SelectionMover::MoveUsingOffset(CEGUI::URect& windowArea, const float left, const float top, const float right, const float bottom) const
{
    windowArea.d_min.d_x.d_offset += left;
    windowArea.d_min.d_y.d_offset += top;
    windowArea.d_max.d_x.d_offset += right;
    windowArea.d_max.d_y.d_offset += bottom;
}

//------------------------------------------------------------------------
void SelectionMover::UpdateWindowAreas(float left, float top, float right, float bottom)
{
    // Validations
    wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

    if (m_selection->Size() < 1)
    {	// Nothing to move
        return;
    }
    //LogDebug (wxT("UpdateWindowAreas - left: %f, top: %f, right: %f, bottom: %f"),
    //    left, top, right, bottom);

    //LogDebug (wxT("UpdateWindowAreas - renderWidth: %f, renderHeight: %f"),
    //  renderWidth, renderHeight);

    // We only move the moveable windows. These will automatically update their children
	Selection::Boxes& boxes = m_selection->GetMoveableBoxes();
    for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {   // Deny when it is blocked
        if (boxIt->IsLocked())
            continue;

        // Handy pointer
        const CEGUI::Window* const window = boxIt->GetWindow();

        // Get the entire rectangle in one call.
        CEGUI::URect windowArea = window->getArea();    // Relative

        static bool autoMovement = true;
        if (autoMovement)
        {
            // Automatically use either scale or offset, depending which is dominant
            float sleft = 0.0f;
            float sright = 0.0f;
            float stop = 0.0f;
            float sbottom = 0.0f;
            float aleft = 0.0f;
            float aright = 0.0f;
            float atop = 0.0f;
            float abottom = 0.0f;
            // If offset being used, move absolutely
            if (windowArea.d_min.d_x.d_offset == 0)		sleft = left;		else	aleft = left;
            if (windowArea.d_max.d_x.d_offset == 0)		sright = right;		else	aright = right;
            if (windowArea.d_min.d_y.d_offset == 0)		stop = top;			else	atop = top;
            if (windowArea.d_max.d_y.d_offset == 0)		sbottom = bottom;	else	abottom = bottom;

            MoveUsingScale(window, windowArea, sleft, stop, sright, sbottom);
            MoveUsingOffset(windowArea, aleft, atop, aright, abottom);

            // Update via wrapper, so it updates the resize box as well.
            boxIt->SetNewWindowArea(windowArea);
        }
        else 
        {
            // Scale movement
            MoveUsingScale(window, windowArea, left, top, right, bottom);
            // Update via wrapper, so it updates the resize box as well.
            boxIt->SetNewWindowArea(windowArea);
        }    
    }
	// Update all boxes
	m_selection->Update();
}

//------------------------------------------------------------------------
void SelectionMover::SnapToGrid()
{
    // Validations
    wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

    const int step = Options::GetInstancePtr()->GetGridSize();

    // (grid does not have to be visible in order to be able to snap)
    if (m_selection->Size() < 1 || step < 1)
    {	// Nothing to snap
        return;
    }

    // Check with current settings
    const int snapMode = Options::GetInstancePtr()->GetSnapMode();
    const bool snapX = (snapMode == SNAP_X || snapMode == SNAP_XY);
    const bool snapY = (snapMode == SNAP_Y || snapMode == SNAP_XY);

    // Calculate relative versions of the given values
    const float renderWidth = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_width;
    const float renderHeight = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_height; 

    // Loop all windows and use the enum to check what direction to move to
	Selection::Boxes& boxes = m_selection->GetMoveableBoxes();
    for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {   // Handy pointer
        const CEGUI::Window* const window = boxIt->GetWindow();

        // "Floor" values.
        const CEGUI::Rect absoluteRect = window->getUnclippedOuterRect();
        CEGUI::UVector2 relativePosition = window->getPosition();

        // Will eventually be replaced by unified grid modifications.
        if (snapX)
        {
            const int deltaX = wx_static_cast(int, absoluteRect.d_left) % step;
            // Apply this pixel value to the scale(relative) value.
            relativePosition.d_x.d_scale -= deltaX / renderWidth;
        }
        if (snapY)
        {
            const int deltaY = wx_static_cast(int, absoluteRect.d_top) % step;
            // Apply this pixel value to the scale(relative) value.
            relativePosition.d_y.d_scale -= deltaY / renderHeight;
        }

        // Update via the wrapper, so it can update the resize box as well.
        boxIt->SetNewWindowPosition(relativePosition);
    }
	// Update all boxes
	m_selection->Update();
}
