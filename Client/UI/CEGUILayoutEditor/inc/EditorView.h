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

#ifndef _EDITOR_VIEW_H_
#define _EDITOR_VIEW_H_

//////////////////////////////////////////////////////////////////////////
// FORWARD REFERENCES
//////////////////////////////////////////////////////////////////////////

class EditorFrame;
class EditorCanvas;

//////////////////////////////////////////////////////////////////////////
// EDITORVIEW
//////////////////////////////////////////////////////////////////////////

/** A "view" on a "document". Note that views are not created when the application 
 * starts, but only when the user chooses "file->new" or "file->open".
 * Also note that the dialog is not a real view.
 */
class EditorView: public wxView
{
    // Needed for the doc/view manager
    DECLARE_DYNAMIC_CLASS(EditorView)

public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor.*/
    EditorView();

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** The view is attached to a frame.*/
    EditorFrame  *m_frame;

    /** OpenGL rendering canvas.*/
    EditorCanvas *m_renderCanvas;

    /************************************************************************
     * OVERRIDDEN PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Overridden because the base class is a Window. We don't use it here.*/
    void OnDraw(wxDC *dc);

    /** Notifies us that a new document has been created, either through
     * File->New or File->Open.*/
    bool OnCreate(wxDocument *doc, long flags);

    /** Notifies that the user closes the current document. Either by an explicit 
     * closing, or through File->New or File->Open.
     */
    bool OnClose(bool deleteWindow = true);

    /** Notification of the UpdateAllViews call in a document.
     */
    void OnUpdate(wxView* sender, wxObject* hint);

    /** Called by the document when the document's name changes, e.g. on Save or New.*/
    void OnChangeFilename();

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Updates the mainframe's title bar according to the currently open document.*/
    void UpdateFrameTitle();
};

#endif // _EDITOR_VIEW_H_
