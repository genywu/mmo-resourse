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
#include "DialogGrid.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define BORDER            5
#define DEFAULT_SIZE			25

#define ASSERT_INFO wxT("Dialog not correctly constructed!")

//////////////////////////////////////////////////////////////////////////
// DIALOG GRID EVENT TABLE
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DialogGrid, wxDialog)
EVT_BUTTON( ID_OK_BUTTON,	DialogGrid::OnOk)
EVT_BUTTON( ID_CANCEL_BUTTON,	DialogGrid::OnCancel)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// DIALOGGRID FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
DialogGrid::DialogGrid( wxWindow* parent): 
m_visible(wx_static_cast(wxCheckBox*, NULL)),
m_snapModeChoice(wx_static_cast(wxRadioBox*, NULL)),
m_size(wx_static_cast(wxTextCtrl*, NULL)),
wxDialog(parent, wxID_ANY, wxT("Grid settings"), wxDefaultPosition, wxSize(300, 255))
{

    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Initialize dialog
    Setup();
}

//-----------------------------------------------------------------------
void DialogGrid::Setup()
{
    // Add the dialog's controls
    wxStaticBox* const mainGroup = new wxStaticBox(this, wxID_ANY, wxT("Update settings:"));
    m_visible = new wxCheckBox(this, wxID_ANY, wxT("grid visible") );

    // Snap options
    const wxString options[] = {wxT("Snap X"), wxT("Snap Y"), wxT("Snap both")};
    m_snapModeChoice = new wxRadioBox(this, wxID_ANY, wxT("Snap mode:"), wxDefaultPosition, wxDefaultSize, 3, options);

    wxStaticText* const size = new wxStaticText(this, wxID_ANY, wxT("Size in pixels:") );
    // Validate for positive number
    m_size = new wxTextCtrl(this, wxID_ANY);

    wxButton* const ok = new wxButton(this, ID_OK_BUTTON, wxT("&OK") );
    wxButton* const cancel = new wxButton(this, ID_CANCEL_BUTTON, wxT("&Cancel") );

    // Mark 'OK' button as the default one (i.e. activated when user presses enter)
    ok->SetDefault();

    // Layout the dialog vertically
    wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

    // Put components -except for the buttons- inside a group
    // NOTE: No need to free the custodial pointer mainGroup 
    // since it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const settingsSizer = new wxStaticBoxSizer(mainGroup, wxVERTICAL);

    // These use entire row
    (void)settingsSizer->Add(m_visible, 0, wxALL, BORDER);
    (void)settingsSizer->Add(m_snapModeChoice, 0, wxALL, BORDER);

    // These components share one row
    // NOTE: No need to free the custodial pointers size, visibleSizer 
    // and settingsSizer since they will be handled internally by 
    // wxWidgets wxSizer::Add()
    wxBoxSizer* const visibleSizer = new wxBoxSizer(wxHORIZONTAL);
    (void)visibleSizer->Add(size, 1, wxALL, BORDER); // within their space, they may stretch
    (void)visibleSizer->Add(m_size, 1, wxALL, BORDER);
    (void)settingsSizer->Add(visibleSizer, 0, wxEXPAND | wxALL, BORDER);
    (void)rootSizer->Add(settingsSizer, 1, wxEXPAND | wxALL, BORDER);

    // The buttons share one row as well
    // NOTE: No need to free the custodial pointers ok, cancel and buttonSizer 
    // since they will be handled internally by wxWidgets wxSizer::Add()
    wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    (void)buttonSizer->Add(ok, 0, wxALL, BORDER);
    (void)buttonSizer->Add(cancel, 0, wxALL, BORDER);
    (void)rootSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, BORDER);

    // Apply root sizer
    // NOTE: No need to free the custodial pointer rootSizer 
    // since it will be handled internally by wxWidgets wxWindowBase::SetSizer()
    SetSizer(rootSizer);

    // Represent current settings
    ShowCurrentSettings();

    // Center dialog
    Centre();
}

//------------------------------------------------------------------------
void DialogGrid::ShowCurrentSettings()
{
    // Validations
    wxASSERT_MSG(m_visible != NULL, ASSERT_INFO);
    wxASSERT_MSG(m_snapModeChoice, ASSERT_INFO);
    wxASSERT_MSG(m_size, ASSERT_INFO);

    m_visible->SetValue(Options::GetInstancePtr()->IsGridVisible());
    m_snapModeChoice->SetSelection(Options::GetInstancePtr()->GetSnapMode());
    m_size->SetValue(wxString::Format(wxT("%d"), Options::GetInstancePtr()->GetGridSize()));
}

//------------------------------------------------------------------------
void DialogGrid::OnOk( wxCommandEvent& WXUNUSED(event) ) 
{
    // Validations
    wxASSERT_MSG(m_visible, ASSERT_INFO);
    wxASSERT_MSG(m_snapModeChoice, ASSERT_INFO);
    wxASSERT_MSG(m_size, ASSERT_INFO);

    Options::GetInstancePtr()->SetGridVisible(m_visible->IsChecked());
    Options::GetInstancePtr()->SetSnapMode(m_snapModeChoice->GetSelection());
    int newSize = atoi(wxConvLibc.cWX2MB(m_size->GetValue()));
    if (newSize <= 0)
    {
        // Default
        newSize = DEFAULT_SIZE;
        LogWarning (wxT("Grid size defaulted to %d because of invalid input."), newSize);
    }
    Options::GetInstancePtr()->SetGridSize(newSize);
    (void)Close();
}

//------------------------------------------------------------------------
void DialogGrid::OnCancel( wxCommandEvent& WXUNUSED(event) ) 
{
    (void)Close();
}
