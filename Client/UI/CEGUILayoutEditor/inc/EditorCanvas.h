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

#ifndef _EDITOR_CANVAS_H_
#define _EDITOR_CANVAS_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "EditorDocument.h"

#include "wx/glcanvas.h"

//////////////////////////////////////////////////////////////////////////
// EDITORCANVAS
//////////////////////////////////////////////////////////////////////////

/** Provides OpenGL output. This canvas is the actual visible part of our "view"
* on the "document". The CEGUI system must be initialized when rendering, which is verified.
*/
class EditorCanvas : public wxGLCanvas
{

public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor.*/
    EditorCanvas(wxView *view, wxFrame *frame, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Updates the openGL viewport setup. Called by the frame when it gets resized.*/
    void changeSize(int newWidth, int newHeight);

    void setView (wxView* aView)
    {
        m_view = aView;
    }

    /** Forces a re-draw.*/
    void Render();

private:
    
    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** The view to which we are attached.*/
    wxView *m_view;

    /** Current canvas width. Used a lot so it's cached.*/
    int mCurrentWidth;

    /** Current canvas height. Used a lot so it's cached.*/
    int mCurrentHeight;

    /** Last known mouseX position.*/
    long mLastMouseX;

    /** Last known mouseY position.*/
    long mLastMouseY;

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Renders the current layout.*/
    void RenderLayout() const;

    /** Prepares for 2D openGL rendering.*/
    void Set2DMode() const;

    /** Draws the grid, if visible.*/
    void DrawGrid() const;

    /** Draws the current resizes.*/
    void DrawResizers() const;

    /** Event handler: notifies us that the canvas needs a repaint. We render the current layout.*/
    void OnPaint(wxPaintEvent& event);

    /** Event handler: we handle mouse input here.*/
    void OnMouseMotion(wxMouseEvent& event);

    /** Event handler: we handle mouse input here.*/
    void OnLeftDown(wxMouseEvent& event);

    /** Event handler: we handle mouse input here.*/
    void OnLeftUp(wxMouseEvent& event);

    /** Event handler: handles key-down events. We use it to move the current selection.*/
    void OnKeyDown(wxKeyEvent& event);

    // Event handling stuff
    DECLARE_EVENT_TABLE()
};

#endif // _EDITOR_CANVAS_H_
