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

#ifndef _DIALOG_GRID_H_
#define _DIALOG_GRID_H_

//////////////////////////////////////////////////////////////////////////
// DIALOGGRID
//////////////////////////////////////////////////////////////////////////

/** This dialog prompts the user with Grid settings. It shows the current settings,
* and allows for modifying them. It reads & stores information through the COptions class,
* so they end up in the INI file.
*/
class DialogGrid: public wxDialog
{
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor, accepts the parent frame.
     * @access public 
     * @qualifier none
     * @param parent Grid's parent frame
     * @return void
     */
    DialogGrid (wxWindow* parent);
    
    /** Destructor.
     * @access public 
     * @qualifier none
     * @return  void
     * @remarks No cleanup is needed because all allocated member variables 
     * will be handled by wxWidgets.
     */
    ~DialogGrid() {};

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** To update the grid's visibility state.*/
    wxCheckBox*	m_visible;

    /** To update the snap type.*/
    wxRadioBox*	m_snapModeChoice;

    /** To update the grid's size.*/
    wxTextCtrl*	m_size;

    // Control ID(s)
    enum {
        ID_OK_BUTTON = wxID_HIGHEST,
        ID_CANCEL_BUTTON
    };

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Called during construction to represent the current grid setup.*/
    void ShowCurrentSettings();

    /** Initializes the dialog.
     * @access private 
     * @qualifier none
     * @return void
     */
    void Setup();

    /** Stores the (new) settings and closes the dialog.*/
    void OnOk(wxCommandEvent& event);

    /** Discards the (changed) settings and closes the dialog.*/
    void OnCancel(wxCommandEvent& event);

    // Event handling stuff
    DECLARE_EVENT_TABLE();
};

#endif // _DIALOG_GRID_H_
