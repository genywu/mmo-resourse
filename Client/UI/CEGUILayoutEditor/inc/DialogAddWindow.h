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

#ifndef _DIALOG_ADD_WINDOW_H_
#define _DIALOG_ADD_WINDOW_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <wx/treectrl.h>
#include <map> 

//////////////////////////////////////////////////////////////////////////
// DIALOGADDWINDOW
//////////////////////////////////////////////////////////////////////////

/** This dialog lets the user select a Window type to add. The window will be added as a 
* child of the selected Layout root or Window in the browse tree. (@see DialogMain).
*/
class DialogAddWindow : public wxDialog
{

public:
    
    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /* Constructor.*/
    DialogAddWindow(wxWindow* aParentDialog, const CEGUI::Window* const aParentWindow);

    /** Destructor.
    * @access public 
    * @qualifier none
    * @return  void
    * @remarks No cleanup is needed because all allocated member variables 
    * will be handled by wxWidgets.
    */
    ~DialogAddWindow() {};

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Returns the chosen new window name.
     * @access public 
     * @qualifier none
     * @return wxString& Chosen window name
     */
    wxString& GetWindowName()
    {
        return m_windowName;
    }

    /** Returns the chosen new window type.
     * @access public 
     * @qualifier none
     * @return wxString& Chosen window type
     */
    wxString& GetWindowType()
    {
        return m_windowType;
    }
    
private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Set in the OK handler, so the main dialog can see which new name was typed.*/
    wxString m_windowName;

    /** Set in the OK handler, so the main dialog can see which new type was chosen.*/
    wxString m_windowType;

    /** Allows for accepting the new window's name.*/
    wxTextCtrl* m_editName;

    /** Allows for selecting a widget type.*/
    wxTreeCtrl* m_widgetTree;

    /** Indicates whether the tree will display all items (false) or only those that
    * are not dependent on parents (true).*/
    bool m_treeFilter;

	/** Shared member to allow us to ppen the previously used looknfeel. */
	static wxString	m_previousLooknfeel;

    /** Holds the parent window of this dialog.*/
    const CEGUI::Window* m_parentWindow;

    /** Allows the user to specify the type of desired filter in the widget tree.*/
    wxRadioBox* m_filterBox;

    /** Keep reference so we can disable it on no valid widget selection.*/
    wxButton*   m_okButton;
    
    // Control ID(s) for the window widgets
    enum {
        ID_SET_AS_DEFAULT = wxID_HIGHEST,
        ID_WIDGET_TREE,
        ID_FILTER_BOX,  
    };

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Returns whether the given window type requires a parent, and is therefor irrelevant to add on its own.*/
    bool RequiresParent(const CEGUI::String& windowType) const;

    /** Returns the current selected window look. Doesn't have to be the same as the one in COptions!*/
    int getWindowLookIndex();

    /** Builds up a tree with all available looks and their widgets.*/
    void InitWidgetTree(const CEGUI::Window* const aParentWindow);

    /** Returns whether the given widget can/may be added to the given parent.*/
    bool Filter(const CEGUI::String& aWidget, const CEGUI::Window* const aParentWindow) const;

    /** General initialization method. Should be called after the constructor.
     * @access private 
     * @qualifier none
     * @return void
     */
    void Setup();

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnSelchangedWidgetTree(wxTreeEvent& event);

    /** Processes the event raised when the user changes the filter radio box selection.
    * @access private 
    * @qualifier none
    * @param event Object that holds the event data.
    * @return void
    */
    void OnFilterBoxSel(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();
};

#endif // _DIALOG_ADD_WINDOW_H_
