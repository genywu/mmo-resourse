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

#ifndef _EDITOR_FRAME_H_
#define _EDITOR_FRAME_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////
// TODO: Get pch workin on gcc to remove these
#include <wx/event.h>
#include <wx/docview.h>
#include <CEGUISystem.h>
#include <RendererModules\OpenGL\CEGUIOpenGLRenderer.h>

//////////////////////////////////////////////////////////////////////////
// FORWARD REFERENCES
//////////////////////////////////////////////////////////////////////////

// Forwarding
class EditorDocument;
class EditorCanvas;
class DialogMain;
class DialogAlignCtrl;
//////////////////////////////////////////////////////////////////////////
// EDITORFRAME
//////////////////////////////////////////////////////////////////////////

class EditorFrame : public wxDocParentFrame
{
    DECLARE_CLASS(EditorFrame)
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor. Attaches an OpenGL compatible canvas.*/
    EditorFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long type);

    /** Destructor.*/
    ~EditorFrame();

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Separate method so we can call it after the entire frame has been initialized,
    * because for the canvas it must be visible, which doesn't look that nice when
    * creating the frame.*/
    EditorCanvas *AttachCanvas(wxView *view);

    /** Retrieves the current OpenGL render canvas.
     * @access public 
     * @qualifier const
     * @return EditorCanvas* Current OpenGL render canvas
     */
    EditorCanvas* GetCanvas() const
    {
        return m_renderCanvas;
    }

    /** Receives the document from the attached view for accessing by the menu-events.*/
    void SetDocument(EditorDocument* document);

    /** Overloaded. We manually refresh the canvas when this is requested.*/
    virtual void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);

    /** we can get the DialoMain, because we need to get the currently selected window */
    DialogMain *GetDialogMain(){return m_dialogMain;};

    /** Toggles between the open and close locked icons depending on the current lock status.
     * @access public 
     * @qualifier none
     * @param document Document which contains the current lock status
     * @return void
     */
    void UpdateLockedSelectionIcon(const EditorDocument* const document) const;
  
private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Defines the menu items. The doc-view items (open, save, exit, cut/copy/paste) are provided
     * by wxWidgets and therefore not found here. Note that Undo and Redo items are updated by wx itself!*/
    enum {
        ID_OPEN_CUSTOM,		// We open from a listbox, not a file dialog
        ID_ALIGN,        // Sub-menu ID does nothing itself.
        ID_ALIGN_LEFT,
        ID_ALIGN_RIGHT,
        ID_ALIGN_TOP,
        ID_ALIGN_BOTTOM,
        ID_ALIGN_WIDTH,
        ID_ALIGN_HEIGHT,
        ID_ALIGN_PADDING,    //Align Space between
        // Handy dialog stuff
		ID_VIEW_SHOW_HIDE_DIALOG,
		//ID_VIEW_MIN_MAX_DIALOG,
		// View stuff
        ID_VIEW_640x480,
        ID_VIEW_800x600,
        ID_VIEW_1024x786,
		ID_VIEW_1280x800, // Wide
        ID_VIEW_FULLSCREEN,	// This setting is not saved to INI since it's mostly used temporarily
        ID_VIEW_SET_BACKGROUND,
        ID_VIEW_SHOW_BACKGROUND,
        ID_VIEW_SET_GRID,
        ID_VIEW_SET_FONT,
        ID_SKINS,
        ID_LOCK_SELECTION
    };

    /** Pointer to CEGUI singleton.*/
    CEGUI::System*			m_GUISystem;

    /** Pointer to CEGUI renderer.*/
    CEGUI::OpenGLRenderer*	m_GUIRenderer;

    /** The dialog which provides access to all properties of a current selection
     * in the document. It is created once, and reset when a new document is opened.
     */
    DialogMain*				m_dialogMain;

    /** OpenGL rendering canvas.*/
    EditorCanvas*			m_renderCanvas;
    EditorDocument*			m_document;

    /** We store the edit menu, to pass to the CommandProcessor.*/
    wxMenu*					mEditMenu;

    /*  设置行列间距的对话框*/
    DialogAlignCtrl *       m_dialogAlign;
    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Attaches a menubar to the frame.*/
    void AttachMenubar();

    /** Attaches a toolbar to the frame.*/
    void AttachToolbar();

    /** Applies size from ini file.*/
    void RestoreIniSize();

    /** Handled wxSizeEvent. Propagate the new resolution to the gui renderer.
    */
    void OnResize(wxSizeEvent& event);

    /** Initializes the CEGUI core + renderer, loads available "looks" and
     * sets a default font to use. We don't cleanup the frame when new view is spawned,
     * so this is a good place to put the CEGUI functionality.*/
    void InitializeCEGUI();
    
    /** Return a wxString that holds the root directory where the CEGUI data
     * files can be found.  It will use the following options when trying to
     * establish where it can find some data files:
     * - If the INI file has paths specified, automatically use those
     * (returning an empty wxString).
     * - Check for a 'datafiles' directory in the resources location.
     * (returning the full path to the found datafiles directory)
     * - Ask the user to specify the datafiles location.
     * (returning the selected directory, or an empty string if user cancelled
     * the dialog).
     */ 
    wxString GetCEGUIDataRoot();

	/** Points the editor to a valid folder with CEGUI datafiles.*/
	void InitializePaths();

    /** Loads available .scheme and .font files from cegui's datafiles directory.
     */
    void LoadData() const;

    /** General initialization method. Should be called after the constructor.
     * @access private 
     * @qualifier none
     * @return void
     */
    void Setup();

	bool GetFileNames(const wxString& path, const wxString& wildcard, wxArrayString& filenames) const;

    /** We listen to the Edit events (including the Align part) of the menubar.*/
    DECLARE_EVENT_TABLE();

	/** Prompts the user with a list of layouts.*/
	void OnOpenCustom(wxCommandEvent& event);

	// Convenience canvas sizers
	void Resize(int aWidth, int aHeight);
	
	// Some dialog options, since nice docking is not available in default wxWidgets
	void OnShowOrHideDialog(wxCommandEvent& event);
	//void OnMinOrMaxDialog(wxCommandEvent& event);
	
	// Resulutions
    void On640x480(wxCommandEvent& event);
    void On800x600(wxCommandEvent& event);
    void On1024x768(wxCommandEvent& event);
    void On1280x800(wxCommandEvent& event);
	//
    void OnFullScreen(wxCommandEvent& event);
    //
    void OnUpdateEditCut(wxUpdateUIEvent& event);
    void OnEditCut(wxCommandEvent& event);
    //
    void OnUpdateEditCopy(wxUpdateUIEvent& event);
    void OnEditCopy(wxCommandEvent& event);
    //
    void OnUpdateEditPaste(wxUpdateUIEvent& event);
    void OnEditPaste(wxCommandEvent& event);
	//
    void OnUpdateEditDelete(wxUpdateUIEvent& event);
    void OnEditDelete(wxCommandEvent& event);
	//
	void OnUpdateSelectAll(wxUpdateUIEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    //
    void OnUpdateSetBackground(wxUpdateUIEvent& event);
    void OnSetBackground(wxCommandEvent& event);
	//
    void OnUpdateShowBackground(wxUpdateUIEvent& event);
    void OnShowBackground(wxCommandEvent& event);
	//
    void OnSetGrid(wxCommandEvent& event);
    void OnSetFont(wxCommandEvent& event);

    // The Align item is a "root". So we don't need separate enable/disables per sub-item
    void OnUpdateAlign(wxUpdateUIEvent& event);
    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);
    void OnAlignTop(wxCommandEvent& event);
    void OnAlignBottom(wxCommandEvent& event);
    void OnAlignWidth(wxCommandEvent& event);
    void OnAlignHeight(wxCommandEvent& event);
    
    /*2010.9.3 nikai*/
    void OnAlignPadding(wxCommandEvent& event);

    /** Processes the event raised when the user chooses the menu option 'Apply Layout Skin'.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnLayoutSkin(wxCommandEvent& event);

    /** Processes the event raised when the application updates the 'Apply Layout Skin' menu option
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnUpdateLayoutSkin(wxUpdateUIEvent& event);

    /** Processes the event raised when the application updates the lock/unlock toolbar icon.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnUpdateLockSelection(wxUpdateUIEvent& event);

    /** Processes the event raised when the user chooses the lock/unlock toolbar icon.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnLockSelection(wxCommandEvent& event);
};

#endif // _EDITOR_FRAME_H_
