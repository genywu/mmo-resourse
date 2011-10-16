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
#include "EditorCanvas.h"
#include "SelectionMover.h"

//////////////////////////////////////////////////////////////////////////
// EDITORCANVAS EVENT TABLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
BEGIN_EVENT_TABLE(EditorCanvas, wxGLCanvas)
EVT_PAINT				(EditorCanvas::OnPaint)
EVT_MOTION				(EditorCanvas::OnMouseMotion)
EVT_LEFT_DOWN			(EditorCanvas::OnLeftDown)
EVT_LEFT_UP				(EditorCanvas::OnLeftUp)
EVT_KEY_DOWN			(EditorCanvas::OnKeyDown)

END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// EDITORCANVAS FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
EditorCanvas::EditorCanvas(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size) :

// Store view
m_view(v),

// Initial (invalid) values
mCurrentWidth(-1),
mCurrentHeight(-1),
mLastMouseX(-1),
mLastMouseY(-1),

wxGLCanvas(frame, wx_static_cast(wxGLCanvas*, NULL), -1, pos, size, wxNO_BORDER )
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Init one-time-only stuff
    SetCurrent();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    // Set the clear color to a grey value
    glClearColor (0.75f, 0.75f, 0.75f, 1.0f);

    // Depth buffer setup
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

//------------------------------------------------------------------------
void EditorCanvas::changeSize(int newWidth, int newHeight)
{
    // Tell the canvas!
    SetSize(0, 0, newWidth, newHeight);

    mCurrentWidth = newWidth;
    mCurrentHeight = newHeight;

    // Tell openGL
    glViewport(0, 0, wx_static_cast(GLsizei, newWidth), wx_static_cast(GLsizei, newHeight));

    // Calculate aspect ratio
    gluPerspective(45.0,wx_static_cast(GLdouble, newWidth)/wx_static_cast(GLdouble, newHeight),0.1,100.0);
}

//------------------------------------------------------------------------
void EditorCanvas::OnMouseMotion(wxMouseEvent& event)
{
    // Check this, because we might get this event during document closing
    if (m_view && m_view->GetDocument() && (event.GetX() != mLastMouseX || event.GetY() != mLastMouseY))
    {
        //logDebug(wxT("Mouse move accepted"));

        EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());

        document->SetDimensions(mCurrentWidth, mCurrentHeight);

        // Update
        mLastMouseX = event.GetX();
        mLastMouseY = event.GetY();

        CEGUI::System::getSingleton().injectMousePosition(mLastMouseX,mLastMouseY);
        document->HandleMouseMoved (mLastMouseX, mLastMouseY) ;
    }
}

//------------------------------------------------------------------------
void EditorCanvas::OnLeftDown(wxMouseEvent& event)
{
    // Check this, because we might get this event during document closing
    if (m_view && m_view->GetDocument())
    {
        CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
        EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());
        document->HandleMousePressed (event.GetX(), event.GetY(), !event.ControlDown()) ;
    }
}

//------------------------------------------------------------------------
void EditorCanvas::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
    // Check this, because we might get this event during document closing
    if (m_view && m_view->GetDocument())
    {
        CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
        EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());
        document->HandleMouseReleased () ;
    }
}

//------------------------------------------------------------------------
void EditorCanvas::Render()
{
    if (m_view && m_view->GetDocument())
    {
        SetCurrent();

        // clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Prepare 2D operations
        Set2DMode();

        // CEGUI rendering
        RenderLayout();		

        // Resizer rendering
        DrawResizers();

        glFlush();    
        SwapBuffers();
    }
}

//------------------------------------------------------------------------
void EditorCanvas::Set2DMode() const
{
    // Prepare 2D operations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Prepare for 2D drawing
    glPointSize(1.0f);
    glLineWidth(1.0f);
}

//------------------------------------------------------------------------
void EditorCanvas::DrawGrid() const
{
    // Only when required/relevant to do
    const bool gridVisible = Options::GetInstancePtr()->IsGridVisible();
    const int stepSize = Options::GetInstancePtr()->GetGridSize();
    if (gridVisible && stepSize > 1)
    {
		int stepsX, stepsY;
        if (mCurrentHeight % stepSize == 0)
		{
            stepsY = mCurrentHeight / stepSize;
		}
        else
		{
            stepsY = (mCurrentHeight / stepSize) + 1;
		}
        if (mCurrentWidth % stepSize == 0)
		{
            stepsX = mCurrentWidth / stepSize;
		}
        else
		{
            stepsX = (mCurrentWidth / stepSize) + 1;
		}

		// Lines
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
        //glColor4f (0.9f, 0.9f, 0.9f, 0.5f);
		glColor4f (0.0f, 0.0f, 0.5f, 0.5f);
        glBegin(GL_LINES);

		// Horz
		for (int y=0; y<stepsY; y++)
		{
			GLfloat yTopAndBottom = 1.0f - ( (wx_static_cast(GLfloat, (y * stepSize) + 1) / mCurrentHeight) * 2.0f);

			// Two points form a line
			glVertex2f(-1.0, yTopAndBottom);
			glVertex2f(1.0, yTopAndBottom);
		}

		// Vert
		for (int x=0; x<stepsX; x++)
		{
			GLfloat xLeftRight = -1.0f + ( (wx_static_cast(GLfloat, x * stepSize) / mCurrentWidth) * 2.0f);

			// Two points form a line
			glVertex2f(xLeftRight, -1.0);
			glVertex2f(xLeftRight, 1.0);
		}

        glEnd(); // GL_LINES;
        glDisable(GL_BLEND);
    }
}

//------------------------------------------------------------------------
void EditorCanvas::DrawResizers() const
{
    // Validations
    wxASSERT_MSG(m_view != NULL, wxT("View must exist in order to draw resizers"));

    GLfloat topLeftX, topLeftY, bottomRightX, bottomRightY;

    //logDebug(wxT("Painting resizers."));

    EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());

    Selection::Boxes& boxes = document->GetSelection()->GetAllBoxes();
    for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {
        // Draw a rectangle around the window. Don't use DrawRectangle because it fills
        // the area. We don't want that, just lines.
        const wxRect winRc = boxIt->GetWindowRect();

        // Non-filled RED rectangle
        glColor3f (1.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
		// Little pixel tweaking to nicely surround the window
        topLeftX = -1.0f + ( (wx_static_cast(GLfloat, winRc.x - 1) / mCurrentWidth) * 2.0f);
        topLeftY = 1.0f - ( (wx_static_cast(GLfloat, winRc.y) / mCurrentHeight) * 2.0f);
        bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, winRc.GetRight()) / mCurrentWidth) * 2.0f);
        bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, winRc.GetBottom() + 1) / mCurrentHeight) * 2.0f);

        glVertex2f(topLeftX, topLeftY);
        glVertex2f(bottomRightX, topLeftY);
        glVertex2f(bottomRightX, bottomRightY);
        glVertex2f(topLeftX, bottomRightY);
        glEnd();	// GL_LINE_LOOP

        // When the window is not locked, draw the eight points as white rectangles with a red border.
        if (!boxIt->IsLocked() && boxIt->IsResizable())
        {
            for (int i = 0; i < NUM_RESIZE_POINTS; ++i)
            {
                const wxRect pointRc = boxIt->GetPointRect(i);

                // WHITE filled rectangle for the inside
                glColor3f (1.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                topLeftX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.x) / mCurrentWidth) * 2.0f);
                topLeftY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.y) / mCurrentHeight) * 2.0f);
                bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.GetRight()) / mCurrentWidth) * 2.0f);
                bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.GetBottom()) / mCurrentHeight) * 2.0f);

                glVertex2f(topLeftX, topLeftY);
                glVertex2f(bottomRightX, topLeftY);
                glVertex2f(bottomRightX, bottomRightY);
                glVertex2f(topLeftX, bottomRightY);
                glEnd();	// GL_QUADS

                // RED surrounding rectangle for the border (just draw over the filled rectangle)
                glColor3f (1.0f, 0.0f, 0.0f);
                glBegin(GL_LINE_LOOP);
                topLeftX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.x) / mCurrentWidth) * 2.0f);
                topLeftY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.y) / mCurrentHeight) * 2.0f);
                bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.GetRight()) / mCurrentWidth) * 2.0f);
                bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.GetBottom()+1) / mCurrentHeight) * 2.0f);

                glVertex2f(topLeftX, topLeftY);
                glVertex2f(bottomRightX, topLeftY);
                glVertex2f(bottomRightX, bottomRightY);
                glVertex2f(topLeftX, bottomRightY);
                glEnd();	// GL_LINE_LOOP

                // logDebug(wxT("Draw rectangle at %f, %f, %f, %f"), x0, y0, x1, y1);
            }
        }

        // Lines to udim 'origin'
        for (int dir=0; dir<NUM_RESIZE_DIRS; ++dir)
        {
            const wxPoint boxPos = boxIt->GetSidePoint(dir, false);
            const wxPoint basePos = boxIt->GetSidePoint(dir, true);

            // Make sure side points are valid
            if( (boxPos.x == -1 && boxPos.y == -1) || (basePos.x == -1 && basePos.y == -1))
                return;

            topLeftX = -1.0f + ( (wx_static_cast(GLfloat, basePos.x) / mCurrentWidth) * 2.0f);
            topLeftY = 1.0f - ( (wx_static_cast(GLfloat, basePos.y) / mCurrentHeight) * 2.0f);
            bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, boxPos.x) / mCurrentWidth) * 2.0f);
            bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, boxPos.y) / mCurrentHeight) * 2.0f);

            glColor3f (0.0f, 1.0f, 0.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(topLeftX, topLeftY);
            glVertex2f(bottomRightX, bottomRightY);
            glEnd();
        }
    }
}

//------------------------------------------------------------------------
void EditorCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // We need to do this to avoid a flood of paint events
    wxPaintDC paintDC(this);

    Render();
}

//------------------------------------------------------------------------
void EditorCanvas::RenderLayout() const
{
    if (CEGUI::System::getSingletonPtr() != NULL)
    {
        //logDebug(wxT("Painting layout."));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CEGUI::System::getSingleton().getRenderer())
        {
           // CEGUI::System::getSingleton().getRenderer()->setQueueingEnabled(false);
            if (CEGUI::WindowManager::getSingleton().isWindowPresent(BACKGROUND_WIDGET_NAME))
            {
                CEGUI::WindowManager::getSingleton().getWindow(BACKGROUND_WIDGET_NAME)->render();
            }
           // CEGUI::System::getSingleton().getRenderer()->setQueueingEnabled(true);
        }

        // Grid rendering
        DrawGrid();

        // Render the current layout
        CEGUI::System::getSingleton().renderGUI();
    }
}

//------------------------------------------------------------------------
void EditorCanvas::OnKeyDown(wxKeyEvent& event)
{
    // Make sure no key is processed if the view hasn't been
    // initialized yet
    if(!m_view)
        return;

    int pixelDeltaX = 0, pixelDeltaY = 0;
    const int key = event.GetKeyCode();

    // Restore fullscreen mode
    if (event.GetKeyCode() == WXK_ESCAPE && wxGetApp().GetMainFrame()->IsFullScreen())
    {
        (void)wxGetApp().GetMainFrame()->ShowFullScreen(false);
        wxGetApp().GetMainFrame()->Refresh();
    }

    if (key == 'g' || key == 'G')	// 'g' for "grid" ('s' is already used)
    {
        SelectionMover((wx_static_cast(EditorDocument*, m_view->GetDocument()))->GetSelection()).SnapToGrid();
        m_view->GetDocument()->Modify(true);
        // Done
        return;
    }
    if (key == 'w' || key == 'W' || key == WXK_UP)
    {
        pixelDeltaY = -1;
    }
    else if (key == 's' || key == 'S' || key == WXK_DOWN)
    {
        pixelDeltaY = 1;
    }
    else if (key == 'a' || key == 'A' || key == WXK_LEFT)
    {
        pixelDeltaX = -1;
    }
    else if (key == 'd' || key == 'D' || key == WXK_RIGHT)
    {
        pixelDeltaX = 1;
    }

    // Was a relevant selection mover key pressed?
    if (pixelDeltaX || pixelDeltaY)
    {
        // Control key is used to change selection by keys to small steps (when you're near your target location.)
        if (!event.ControlDown())
        {
            pixelDeltaX *= Options::GetInstancePtr()->GetBigKeyStep();
            pixelDeltaY *= Options::GetInstancePtr()->GetBigKeyStep();
        }

        SelectionMover((wx_static_cast(EditorDocument*, m_view->GetDocument()))->GetSelection()).UpdateWindowAreas(wx_static_cast(float, pixelDeltaX), wx_static_cast(float, pixelDeltaY), wx_static_cast(float, pixelDeltaX), wx_static_cast(float, pixelDeltaY));
        m_view->GetDocument()->Modify(true);
    }
}
