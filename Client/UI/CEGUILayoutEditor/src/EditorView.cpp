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

#include "EditorView.h"
#include "CELayoutEditor.h"
#include "EditorCanvas.h"

//////////////////////////////////////////////////////////////////////////
// EDITORVIEW IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(EditorView, wxView)

//////////////////////////////////////////////////////////////////////////
// EDITORVIEW FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
EditorView::EditorView() :
m_frame(wx_static_cast(EditorFrame*, NULL)),
m_renderCanvas(wx_static_cast(EditorCanvas*, NULL)),
wxView()
{ 
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
}

//------------------------------------------------------------------------
bool EditorView::OnCreate(wxDocument *document, long WXUNUSED(flags) )
{
    // Single-window mode
    m_frame = wxGetApp().GetMainFrame();
    m_renderCanvas = m_frame->GetCanvas();
    m_renderCanvas->setView (this);

    // Associate the appropriate frame with this view.
    SetFrame(m_frame);

    // Tell the frame about the document
    m_frame->SetDocument(wx_static_cast(EditorDocument*, document));

    UpdateFrameTitle();

    // Make sure the document manager knows that this is the
    // current view.
    wxView::Activate(true);

    return true;
}

//------------------------------------------------------------------------
bool EditorView::OnClose(bool WXUNUSED(deleteWindow))
{
    if (!GetDocument()->Close())
    {
        return false;
    }

    if(m_renderCanvas)
    {
        m_renderCanvas->ClearBackground();
        m_renderCanvas->setView (wx_static_cast(wxView*, NULL));
        m_renderCanvas = wx_static_cast(EditorCanvas*, NULL);
    }
    SetFrame(wx_static_cast(wxFrame*, NULL));

    // Tell the frame about the document
    if(m_frame)
    {
        m_frame->SetDocument(wx_static_cast(EditorDocument*, NULL));
        m_frame = wx_static_cast(EditorFrame*, NULL);
    }

    wxView::Activate(false);

    UpdateFrameTitle();

    return true;
}

//------------------------------------------------------------------------
void EditorView::OnUpdate(wxView* WXUNUSED(sender), wxObject* WXUNUSED(hint))
{
    if (m_frame && m_frame->IsShown() && m_renderCanvas)
    {
        //LogDebug(wxT("Needs repaint."));
        m_renderCanvas->Refresh(false);
        m_renderCanvas->Render();
    }
}

//------------------------------------------------------------------------
void EditorView::OnChangeFilename()
{
    UpdateFrameTitle();
}

//------------------------------------------------------------------------
void EditorView::UpdateFrameTitle()
{

    wxString title (wxTheApp->GetAppName());

    if (GetDocument())
    {
        title << wxT(" - [") << GetDocument()->GetFilename() << wxT("]");
    }
    if (m_frame)
    {
        m_frame->SetTitle(title);
    }
}

//------------------------------------------------------------------------
void EditorView::OnDraw(wxDC* WXUNUSED(dc))
{
}
