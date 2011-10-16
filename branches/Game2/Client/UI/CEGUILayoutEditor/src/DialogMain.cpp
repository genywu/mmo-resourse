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

#include "Config.h"
#include "CELayoutEditor.h"
#include "DialogMain.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "falagard/CEGUIFalEnums.h"
#include "CommandAddWindow.h"
#include "../../wxWidgets/include/wx/msw/private.h"

// One day when maybe wxWidgets-only containers and such are used,
// it can be removed. For the moment i didn't want to rebuild with the
// USE_STD_IOSTREAM define.
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define BORDER            5

#define ASSERT_DOCUMENT_MSG wxT("The m_document member is NULL!")

// This dialog is resizable, but not smaller then this.
#define MINIMUM_WIDTH 500
#define MINIMUM_HEIGHT 1000

// Grid column widths
static const int COLUMN_ROW_NUMBER_WIDTH = 40;
static const int COLUMN_CUT_PADDING = 30;
// The value column gets the remainder

// We use a fake property to update a window's name.
#define NAME        wxT("Name")

// The UDim setup. These names form our other "fake" properties; the ones who do not actually
// exist in the CEGUI system. But the string versions are rather difficult and user-unfriendly
// to edit, because of their format {{scale,offset}}. So we query the current window
// for its area and query these values as normal floats, which are easier to edit :-)
#define POS_X          wxT("X")
#define POS_Y          wxT("Y")
#define WIDTH          wxT("Pixel Width")
#define HEIGHT         wxT("Pixel Height")

#define FAKE_PROPERTY_COUNT 5

// Color used to display the "fake" property values
#define FAKE_PROPERTY_COLOR wxColour(36,113,82)

// Color used to display the non common "real" property values (i.e. properties 
// that appear in some but not all elements in the current user selection)
#define NON_COMMON_PROPERTY_COLOR wxColour(60, 69, 155)

//////////////////////////////////////////////////////////////////////////
// DIALOG MAIN EVENT TABLE
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DialogMain, wxDialog)
// Button-click handler(s)
EVT_BUTTON            (ID_ADD_WINDOW, DialogMain::OnAddWindow)
EVT_BUTTON            (ID_SAVE_SELECTION, DialogMain::OnSaveSelection)
EVT_TREE_SEL_CHANGED  (ID_LAYOUT_TREE, DialogMain::OnSelchangedLayoutTree)
EVT_TEXT              (ID_TEXT, DialogMain::OnChangeText)
EVT_COMBOBOX		  (ID_SKINS, DialogMain::OnChangeSkin)
EVT_RADIOBOX		  (ID_SKINS_DISPLAY, DialogMain::OnDisplayModeSelection) 

// Grid handlers
EVT_GRID_SELECT_CELL(DialogMain::OnGridCellSelected)
EVT_GRID_CELL_CHANGE(DialogMain::OnGridCellChange)
EVT_GRID_COL_SIZE(DialogMain::OnGridColSize)
EVT_SIZE(DialogMain::OnResize)

END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// DIALOG MAIN CONSTRUCTORS/DESTRUCTORS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
DialogMain::DialogMain(wxWindow* parent) 
: 
// Put the elements to NULL, so we can block events during initialization
m_document(wx_static_cast(EditorDocument *, NULL)),
m_currentRoot(wx_static_cast(CEGUI::Window*, NULL)),
m_currentWindow(wx_static_cast(CEGUI::Window*, NULL)),
m_tree(wx_static_cast(wxTreeCtrl*, NULL)),
m_boldItemWindow(NULL),
m_addWindow(wx_static_cast(wxButton*, NULL)),
m_saveSelection(wx_static_cast(wxButton*, NULL)),
m_editText(wx_static_cast(wxTextCtrl*, NULL)),
m_widgetLooks(),
m_availableLooks(),
m_typeText(wx_static_cast(wxStaticText*, NULL)),
m_skinList(wx_static_cast(wxComboBox*, NULL)),
m_displayModeRadio(wx_static_cast(wxRadioBox*, NULL)),

// By default, display all existing skins and properties
m_displayOnlyCommon(false),	

// Put the elements to NULL, so we can block events during initialization
m_grid(wx_static_cast(wxGrid*, NULL)),
m_propertySizer(wx_static_cast(wxStaticBoxSizer*, NULL)),
m_fonts(wx_static_cast(wxString*, NULL)),
m_fontCount(0),
m_updating(false),

m_supportedProperties(),
m_treeItemMapping(),
m_lastSelectedRow(-1),
m_lastSelectedWindow(wx_static_cast(CEGUI::Window*, NULL)),
wxDialog(parent, wxID_ANY, wxT("Main Dialog"), wxDefaultPosition, wxSize(MINIMUM_WIDTH, MINIMUM_HEIGHT), 
		 (wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX) | wxRESIZE_BORDER | wxMINIMIZE_BOX /* Minimize/restore */ ) // Cannot be closed
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

//------------------------------------------------------------------------
DialogMain::~DialogMain()
{
    // Swallow any exceptions found to prevent the destructor from emitting them.
    // NOTE: An exception should never leave a destructor as it may yield
    // undefined behavior or even lead to program abnormal termination 
    // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
    try
    {
        m_fontCount = 0;
        delete[] m_fonts;

        // Zero out the member pointers
        // NOTE: All of these will be freed internally by wxWidgets
        m_skinList = wx_static_cast(wxComboBox*, NULL);
        m_propertySizer = wx_static_cast(wxStaticBoxSizer*, NULL);
        m_document = wx_static_cast(EditorDocument*, NULL);
        m_currentRoot = wx_static_cast(CEGUI::Window*, NULL);
        m_displayModeRadio = wx_static_cast(wxRadioBox*, NULL);
        m_grid = wx_static_cast(wxGrid*, NULL);
        m_currentWindow = wx_static_cast(CEGUI::Window*, NULL);
        m_tree = wx_static_cast(wxTreeCtrl*, NULL);
        m_TreeImageList = wx_static_cast(wxImageList*, NULL);
        m_addWindow = wx_static_cast(wxButton*, NULL);
        m_saveSelection = wx_static_cast(wxButton*, NULL);
        m_editText = wx_static_cast(wxTextCtrl*, NULL);
        m_typeText = wx_static_cast(wxStaticText*, NULL);
    }
    // Add exception to log
    LOG_EXCEPTION_SAFE("DialogMain::~DialogMain", "Failed to destroy main dialog", "Failed to destroy main dialog", "Failed to destroy main dialog");
}

//////////////////////////////////////////////////////////////////////////
// DIALOG MAIN PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * SETUP
 ************************************************************************/

//-----------------------------------------------------------------------
void DialogMain::Setup()
{
    // We can resize the dialog, but keep the given dimension as the minimum
    wxWindowBase::SetSizeHints(MINIMUM_WIDTH, MINIMUM_HEIGHT);

    // All "groups"
    wxStaticBox* const treeGroup = new wxStaticBox(this, wxID_ANY, wxT("Browse layout:"));
    wxStaticBox* const propertyGroup = new wxStaticBox(this, wxID_ANY, wxT("Available properties in the current selection:"));

    // The layout browse tree
    // NB: This must, must, MUST! be created after the static box that will be
    // used for the box sizer we add the tree to.
    m_tree = new wxTreeCtrl(this, ID_LAYOUT_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_VARIABLE_ROW_HEIGHT | wxTR_HAS_BUTTONS | wxTR_MULTIPLE);

    wxImageList *m_TreeImageList = new wxImageList(16,16);
    wxBitmap image(wxT("lock.bmp"),wxBITMAP_TYPE_BMP);
    m_lockIcon.CopyFromBitmap(image);
    m_TreeImageList->Add(m_lockIcon);
    m_tree->AssignImageList(m_TreeImageList);
    
    // Button(s)
    m_addWindow = new wxButton(this, ID_ADD_WINDOW, wxT("Add window"));
    //m_LoadLayout = new wxButton(this, ID_LOAD_LAYOUT, wxT("Add layout"));
    m_saveSelection = new wxButton(this, ID_SAVE_SELECTION, wxT("Save current selection"));
    // Wait until a window gets selected
    (void)m_saveSelection->Enable(false);


    // Size the components vertically
    wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

    // Sizes the tree - this one may be expanded
    // NOTE: No need to free the custodial pointer treeGroup since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const treeSizer = new wxStaticBoxSizer(treeGroup, wxVERTICAL);
    (void)treeSizer->Add(m_tree, 1, wxEXPAND | wxALL, BORDER);
    
    wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    // Add the button  (may not stretch)
    (void)buttonSizer->Add(m_addWindow, 0, wxALL, BORDER);
    //buttonSizer->Add(m_LoadLayout, 0, wxALL, BORDER);
    (void)buttonSizer->Add(m_saveSelection, 0, wxALL, BORDER);

    // NOTE: No need to free the custodial pointer buttonSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)treeSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer (may stretch)
    // NOTE: No need to free the custodial pointer treeSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(treeSizer, 1, wxEXPAND | wxALL, BORDER);

    //------------------------------ CONTENT GROUP -------------------------------

    // Create a framed static text for displaying the currently selected widget text
    wxStaticBox* const contentGroup = new wxStaticBox(this, wxID_ANY, wxT("Text:"));
    m_editText = new wxTextCtrl(this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2);

    // The content (or caption) property
    // NOTE: No need to free the custodial pointer contentGroup since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const contentSizer = new wxStaticBoxSizer(contentGroup, wxHORIZONTAL);
    // Use entire sizer
    (void)contentSizer->Add(m_editText, 1, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
    // NOTE: No need to free the custodial pointer contentSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(contentSizer, 0, wxEXPAND | wxALL, BORDER);

    //------------------------------ TYPE GROUP -------------------------------

    // Create a framed static text for displaying the type of the currently selected widget
    wxStaticBox* const typeGroup = new wxStaticBox(this, wxID_ANY, wxT("Type:"));
    m_typeText = new wxStaticText(this, wxID_ANY, wxT("") );
    wxStaticBoxSizer* const typeSizer = new wxStaticBoxSizer(typeGroup, wxHORIZONTAL);

    // Use entire sizer
    (void)typeSizer->Add(m_typeText, 1, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
    // NOTE: No need to free the custodial pointer typeSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(typeSizer, 0, wxEXPAND | wxALL, BORDER);

    //------------------------------ DISPLAY GROUP -------------------------------

    // Create display sizer
    wxBoxSizer* const displaySizer = new wxBoxSizer(wxVERTICAL);

    // Create radio box that will allow the user to switch between multi-selection All and
    // common modes
    const wxString options[] = {wxT("Show All"), wxT("Show Only Common")};
    m_displayModeRadio = new wxRadioBox(this, ID_SKINS_DISPLAY, wxT("Multi-Selection Display Mode For Skins And Properties:"), wxDefaultPosition, wxDefaultSize, 2, options);

    // Use entire sizer
    (void)displaySizer->Add(m_displayModeRadio, 0, wxEXPAND | wxALL, 0);

    // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
    // NOTE: No need to free the custodial pointer displaySizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(displaySizer, 0, wxEXPAND | wxALL, BORDER);

    // No widget selection initially, so disable the multi-selection display mode radio box
    (void)m_displayModeRadio->Enable(false);

    //------------------------------ SKIN GROUP -------------------------------

    // Create framed skin dropdown, that will hold the available skins for the current widget selection
    wxStaticBox* const skinGroup = new wxStaticBox(this, wxID_ANY, wxT("Skin:"));	
    m_skinList = new wxComboBox(this, ID_SKINS);

    // NOTE: No need to free the custodial pointer skinGroup since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const skinSizer = new wxStaticBoxSizer(skinGroup, wxVERTICAL);

    // Use entire sizer
    (void)skinSizer->Add(m_skinList, 0, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
    // NOTE: No need to free the custodial pointer skinSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(skinSizer, 0, wxEXPAND | wxALL, BORDER);

    // No skin selected initially
    (void)m_skinList->Enable(false);

    //------------------------------ PROPERTY GRID -------------------------------

    // NOTE: No need to free the custodial pointer propertyGroup since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    m_propertySizer = new wxStaticBoxSizer(propertyGroup, wxHORIZONTAL);
    CreateGrid();
    (void)m_propertySizer->Add(m_grid, 1, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
    (void)rootSizer->Add(m_propertySizer, 1, wxEXPAND | wxALL, BORDER);

    // NOTE: No need to free the custodial pointer rootSizer since
    // it will be handled internally by wxWidgets wxWindowBase::SetSizer()
    SetSizer(rootSizer);

    // Initially, reset the entire dialog. (Does a bit more than required here, but doesn't matter)
    Reset();

    CreateSupportedPropertyMap();

    (void)wxDialog::SetEscapeId(wxID_NONE);
    (void)wxDialog::Show(true);
}

/************************************************************************
 * PROPERTY GRID
 ************************************************************************/

//------------------------------------------------------------------------
void DialogMain::ClearGrid()
{
    if(m_grid)
    {
        const int oldRowCount = m_grid->GetRows();
        if (oldRowCount > 0)
        {
            (void)m_grid->DeleteRows(0, oldRowCount);
        }
    }
}

//------------------------------------------------------------------------
void DialogMain::CreateSupportedPropertyMap()
{
    // Read settings from options
    const wxString& properties = Options::GetInstancePtr()->GetSupportedProperties();

    // Parse setting - this parsing should eventually be moved in COption
    wxStringTokenizer semicolonTokenizer(properties, wxT(";"));
    while(semicolonTokenizer.HasMoreTokens())
    {
        // token hold something like:  propertyName,propertyType
        const wxString token = semicolonTokenizer.GetNextToken();

        wxStringTokenizer commaTokenizer(token, wxT(",")); // Second tokenizer

        const wxString propertyName = commaTokenizer.GetNextToken();
        const wxString propertyType = commaTokenizer.GetNextToken();

        if (propertyName.IsEmpty() || propertyType.IsEmpty())
        {
            continue; // Ignore this entry
        }

        // Associate a mnemonic named present in the COption string to our PropertyType enumeration.
        struct PropertyMnemonic
        {
            wxString     mnemonic;
            PropertyType type;
        } mnemonics[] =
        {
            { wxT("bool")               ,  BOOL_TYPE             },
            { wxT("float")              ,  FLOAT_TYPE            },
            { wxT("font")               ,  FONT_TYPE             },
            { wxT("horz_align")         ,  HORZ_ALIGN_TYPE       },
            { wxT("horz_text_format")   ,  HORZ_TEXT_FORMAT_TYPE },
            { wxT("text")               ,  TEXT_TYPE             },
            { wxT("vert_align")         ,  VERT_ALIGN_TYPE       },
            { wxT("vert_text_format")   ,  VERT_TEXT_FORMAT_TYPE },
            { wxT("tab_positon")        ,  TABPANE_POS           },
        };

        // For every mnemonic type, compare with the current COption entry we are parsing
        // Note: Mnemonics that we don't know about will be silently discarded
        for (size_t i = 0; i < sizeof mnemonics / sizeof mnemonics[0]; ++i)
        {
            PropertyMnemonic& M = mnemonics[i]; // The current PropertyMnemonic we are trying to match

            // If the item we are processing match
            if (propertyType == M.mnemonic)
            {
                (void)m_supportedProperties.insert(std::make_pair(StringHelper::ToCEGUIString(propertyName), M.type)); // Add to the supported properties list
                break;
            }
        }
    }
}

//------------------------------------------------------------------------
void DialogMain::CreateGrid()
{
    m_grid = new wxGrid(this, ID_GRID);
    (void)m_grid->CreateGrid( 0, 2 );

    m_grid->SetColLabelValue(COLUMN_NAME, wxT("Property"));
    m_grid->SetColLabelValue(COLUMN_VALUE, wxT("Value"));

    UpdateColumnWidths();
}

//------------------------------------------------------------------------
DialogMain::PropertyType DialogMain::GetPropertyType(const CEGUI::String& propertyName)
{
    const DialogMain::SupportedPropertiesType::iterator propertyIt = m_supportedProperties.find(propertyName);
    if (propertyIt != m_supportedProperties.end())
    {
        return propertyIt->second;
    }

    // An upsupported property was given
    return UNSUPPORTED;
}

//------------------------------------------------------------------------
void DialogMain::UpdateColumnWidths()
{
    if (m_propertySizer)
    {
        // Validations
        wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));

		int width, unused;
		m_grid->GetSize(&width, &unused);
		// Only refresh when different
        const int leftColSize = m_grid->GetColSize(COLUMN_NAME);
		int targetWidth = (width - leftColSize)-(COLUMN_ROW_NUMBER_WIDTH + COLUMN_CUT_PADDING);
		if (targetWidth > 0 && targetWidth != m_grid->GetColSize(COLUMN_VALUE))
		{
			// Reset the column widths so that the entire view is being used. Horizontal scrollbars should
			// never occur, and it's not possible to make the right column smaller. Is this wanted? Don't think so...
			m_grid->SetRowLabelSize(COLUMN_ROW_NUMBER_WIDTH);
			m_grid->SetColSize(COLUMN_VALUE, targetWidth);
			m_grid->ForceRefresh();
		}
    }
}

//------------------------------------------------------------------------
void DialogMain::UpdateProperties(bool mapSkins)
{
    ClearGrid();
    if (m_document == NULL || m_document->GetSelection()->Size() == 0)
    {
        // The user unselected the last selection element, so we need to update the
        // selection info (text, skin, etc.) to reflect this before taking our leave
        UpdateSelectionInfo();
        return;
    }

    // Adds the Udim  and name stuff
    AddFakeProperties();

    // Add the CEGUI propertySet stuff
    AddRealProperties(mapSkins);

	UpdateColumnWidths();

    m_grid->AdjustScrollbars();
}

//------------------------------------------------------------------------
void DialogMain::AddFakeProperties()
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Can't add fake properties with empty document"));
    wxASSERT_MSG(m_currentWindow != NULL, wxT("Can't add fake properties with empty window"));

    // Only set common short-cuts when no more then 1 window is selected. Otherwise each
    // window in the selection would get the same value! Effectively making them invisible (right
    // behind the first one) or giving all the same text!
    const bool multiSelect = m_document->GetSelection()->Size() > 1;
    // Only allow changes to position and size when the window is not locked (even when it's just one window selected).
    const bool locked = m_document->GetSelection()->FindBoxFor(m_currentWindow)->IsLocked();
    //logDebug (wxT("%s locked? :%d"), StringHelper::ToWXString(m_currentWindow->getName()), locked);

    // Now pick the elements we need one by one
    // NOTE: If multi-selection exists, the property value is only displayed if 
    // ALL windows in the selection contain the same property value (common value). 
    // Otherwise, the grid entry will appear empty to mark that not 
    // all selected widgets contain the same value for that property.
    // NOTE2: We don't use property lookup for the fake ones. Just add floating 
    // points for the Scales, and integers for the Offsets. Note that you 
    // cannot pre-allocate these grids only once, because that causes Refcount 
    // asserts on cleanup...
    
    AddPropertyRow(new wxGridCellTextEditor(),   NAME,   GetFakePropertyCommonValue(NAME),  multiSelect,          FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), POS_X,GetFakePropertyCommonValue(POS_X),   multiSelect|| locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), POS_Y,GetFakePropertyCommonValue(POS_Y),   multiSelect|| locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), WIDTH,GetFakePropertyCommonValue(WIDTH),   multiSelect|| locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), HEIGHT,GetFakePropertyCommonValue(HEIGHT), multiSelect|| locked, FAKE_PROPERTY_COLOR);
 /*   AddPropertyRow(new wxGridCellFloatEditor(),  LEFT_SCALE, GetFakePropertyCommonValue(LEFT_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), LEFT_OFFSET, GetFakePropertyCommonValue(LEFT_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellFloatEditor(), TOP_SCALE, GetFakePropertyCommonValue(TOP_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), TOP_OFFSET, GetFakePropertyCommonValue(TOP_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellFloatEditor(), RIGHT_SCALE, GetFakePropertyCommonValue(RIGHT_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), RIGHT_OFFSET, GetFakePropertyCommonValue(RIGHT_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellFloatEditor(), BOTTOM_SCALE, GetFakePropertyCommonValue(BOTTOM_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
    AddPropertyRow(new wxGridCellNumberEditor(), BOTTOM_OFFSET, GetFakePropertyCommonValue(BOTTOM_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);*/
    
}

//------------------------------------------------------------------------
wxString DialogMain::GetFakePropertyValue(const wxString& propertyName, const CEGUI::Window* const currentWindow) const
{
    // Validations
    wxASSERT_MSG(currentWindow != NULL, wxT("Supplied window is NULL!"));

    // Check type of "fake" property and return appropriate value
    // NOTE: Unlike the "real" properties, which vary according to the widget type,
    // the "fake" properties are always the same, are always present and are 
    // common to all widgets.
    if(propertyName == NAME)
        return StringHelper::ToWXString(currentWindow->getName());
    else
    {
        // Get the entire rectangle in one call.
        const CEGUI::URect windowArea = currentWindow->getArea();
        float fScreenX = CEGUI::CoordConverter::windowToScreenX(*currentWindow,0);
        float fScreenY = CEGUI::CoordConverter::windowToScreenY(*currentWindow,0);
        // Scales appear as floats, offsets as floats disguised as integers
        if (propertyName == POS_X)
            return wxString::Format(wxT("%.0f"), fScreenX);
        else if(propertyName == POS_Y)
            return wxString::Format(wxT("%.0f"), fScreenY);
        else if(propertyName == WIDTH)
            return wxString::Format(wxT("%.0f"), currentWindow->getPixelSize().d_width);
        else if(propertyName == HEIGHT)
            return wxString::Format(wxT("%.0f"), currentWindow->getPixelSize().d_height);
/*        else if(propertyName == LEFT_SCALE)
            return wxString::Format(wxT("%f"), windowArea.d_min.d_x.d_scale);
        else if(propertyName == LEFT_OFFSET)
            return wxString::Format(wxT("%.0f"), windowArea.d_min.d_x.d_offset);
        else if(propertyName == TOP_SCALE)
            return wxString::Format(wxT("%f"), windowArea.d_min.d_y.d_scale);
        else if(propertyName == TOP_OFFSET)
            return wxString::Format(wxT("%.0f"), windowArea.d_min.d_y.d_offset);
        else if(propertyName == RIGHT_SCALE)
            return wxString::Format(wxT("%f"), windowArea.d_max.d_x.d_scale);
        else if(propertyName == RIGHT_OFFSET)
            return wxString::Format(wxT("%.0f"), windowArea.d_max.d_x.d_offset);
        else if(propertyName == BOTTOM_SCALE)
            return wxString::Format(wxT("%f"), windowArea.d_max.d_y.d_scale);
        else if(propertyName == BOTTOM_OFFSET)
            return wxString::Format(wxT("%.0f"), windowArea.d_max.d_y.d_offset);     */   
        else
        {
            LogError(wxT("DialogMain::GetFakePropertyValue - Unknown fake property supplied!"));
            return wxT("");
        }
    }
}

//------------------------------------------------------------------------
wxString DialogMain::GetFakePropertyCommonValue(const wxString& propertyName) const
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Document member is NULL!"));

    // The first window in the selection will be our comparison basis
    const CEGUI::Window *currentWindow = m_currentWindow;
    wxString firstPropertyValue = wxT("");

    // Make sure we have a valid window
    if(currentWindow)
    {
        // Obtain "fake" property value
        firstPropertyValue = GetFakePropertyValue(propertyName, currentWindow);

        // Verify that selection is valid (safety-check)
        if(m_document->GetSelection())
        {
            // Scan entire user selection
			Selection::Boxes& boxes = m_document->GetSelection()->GetAllBoxes();
			for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
            {
                // If the current "fake" property value isn't the same as the stored property
                // value for the first window in the selection, the value isn't common
                // to ALL widgets in the selection, so it will appear as empty in the
                // property grid
                currentWindow = (*boxIt).GetWindow();
                if(GetFakePropertyValue(propertyName, currentWindow) != firstPropertyValue)
                    return wxT("");
            }
        }
    }

    // Property value is common to ALL widgets in the selection, so we can use it
    // for display purposes
    return firstPropertyValue;
}

//------------------------------------------------------------------------
void DialogMain::AddRealProperties(const bool mapSkins)
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Can't add real properties with empty document"));

    // If desired, we can map the possible skins for the current user widget selection
    // NOTE: This is not always wanted. For instance, when the Look'N'Feel of a 
    // selection is modified, we want to keep the skins as is since the 
    // selection itself did not change and thus the possible skins remain the same
    if(mapSkins)
        MapWidgetSelectionPossibleSkins();

    // Update text, type and displayed skin info
    UpdateSelectionInfo();

    // Updates the relevant properties by calling the document for the most actual set.
    EditorDocument::RelevantProperties commonProperties, nonCommonProperties;
    m_document->GetRelevantProperties(&commonProperties, &nonCommonProperties, m_displayOnlyCommon);

    // Display common properties to all elements in the selection
    // NOTE: These properties are always shown independently of the current display mode (common or all)
    EditorDocument::RelevantPropertiesIterator current = commonProperties.begin();
    for(; current != commonProperties.end(); ++current)
    {
        // Add a new property to the property grid. Pass PropertyName, PropertyValue
        LogTrace(wxT("DialogMain::AddRealProperties - Common Property '") + StringHelper::ToWXString((*current).first) + wxT("' = ") + StringHelper::ToWXString((*current).second));
        AddPropertyRow((*current).first, (*current).second);
    }
    // Do the same for the non common properties, if the display mode allows it ('Display All' only)
    if(!m_displayOnlyCommon)
    {
        for(current = nonCommonProperties.begin(); current != nonCommonProperties.end(); ++current)
        {
            // Add a new property to the property grid. Pass PropertyName, PropertyValue
            // NOTE: Non common properties' values appear in blueish color
            LogTrace(wxT("DialogMain::AddRealProperties - NonCommon Property '") + StringHelper::ToWXString((*current).first) + wxT("' = ") + StringHelper::ToWXString((*current).second));
            AddPropertyRow((*current).first, (*current).second, NON_COMMON_PROPERTY_COLOR);            
        }
    }
}

//------------------------------------------------------------------------
void DialogMain::AddPropertyRow(wxGridCellEditor* editor, const wxString& propertyName, const wxString& propertyValue, bool valueReadOnly /* = true */, const wxColour& textColor/* =*wxBLACK */, const wxColour& backgroundColor /* =*wxWHITE */, const wxColour& lockedBackgroundColor /* =*wxLIGHT_GREY */)
{
    // Validations
    wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));

    // Add a row, and store the editor + set initial value
    (void)m_grid->AppendRows(1, true);
    const int newRowIndex = m_grid->GetRows() - 1;

    // Set the specified editor
    m_grid->SetCellEditor(newRowIndex, COLUMN_VALUE, editor);

    // Make the index zero-based(!)
    m_grid->SetCellValue(newRowIndex, COLUMN_NAME, propertyName);
    m_grid->SetReadOnly(newRowIndex, COLUMN_NAME, true);
    m_grid->SetCellValue(newRowIndex, COLUMN_VALUE, propertyValue);
    m_grid->SetReadOnly(newRowIndex, COLUMN_VALUE, valueReadOnly);	// Depends

    // Set the background colors accordingly
    m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_NAME, backgroundColor);
    if (valueReadOnly)
        m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_VALUE, lockedBackgroundColor);
    else
        m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_VALUE, backgroundColor);

    // Change text color (only affects value column)
    m_grid->SetCellTextColour(newRowIndex, COLUMN_VALUE, textColor);

    // Set the correct font (normal if property is at default value in all elements in
    // the current selection, bold otherwise)
    SetPropertyValueFont(newRowIndex, propertyName);
}

//------------------------------------------------------------------------
void DialogMain::AddPropertyRow(const CEGUI::String& propertyName, const CEGUI::String& propertyValue, const wxColour& textColor/* =*wxBLACK */, const wxColour& backgroundColor /* =*wxWHITE */, const wxColour& lockedBackgroundColor /* =*wxLIGHT_GREY */)
{
    // Get the editor for the given property (float, bool, or font)
    wxGridCellEditor* const cellEditor = GetPropertyEditor(propertyName);

    // Supported?
    if (cellEditor)
    {
        AddPropertyRow(cellEditor, StringHelper::ToWXString(propertyName), StringHelper::ToWXString(propertyValue), false, textColor, backgroundColor, lockedBackgroundColor);
    }
}

//------------------------------------------------------------------------
wxGridCellEditor* DialogMain::GetPropertyEditor(const CEGUI::String& propertyName)
{
    switch(GetPropertyType(propertyName))
    {
    case DialogMain::FLOAT_TYPE:
        // Ignores non-floating point input
        return new wxGridCellFloatEditor();
    case DialogMain::BOOL_TYPE:
        {
            static const int BOOL_CHOICE_COUNT = 2;
            const wxString boolChoices[BOOL_CHOICE_COUNT] = { wxT("False"), wxT("True") };

            // Creates a little combobox with two options: True and False.
            return new wxGridCellChoiceEditor(BOOL_CHOICE_COUNT, boolChoices);
        }
    case DialogMain::FONT_TYPE:
        // Creates a combobox with all font names
        FindAvailableFonts();
        return new wxGridCellChoiceEditor(m_fontCount, m_fonts);
    case DialogMain::TEXT_TYPE:
        return new wxGridCellTextEditor();

    case DialogMain::VERT_ALIGN_TYPE:
        {
            static const int VERT_CHOICE_COUNT = 3;
            const wxString vertChoices[VERT_CHOICE_COUNT] = { wxT("Top"), wxT("Centre"), wxT("Bottom") };

            return new wxGridCellChoiceEditor(VERT_CHOICE_COUNT, vertChoices);
        }
    case DialogMain::HORZ_ALIGN_TYPE:
        {
            static const int HORZ_CHOICE_COUNT = 3;
            const wxString horzChoices[HORZ_CHOICE_COUNT] = { wxT("Left"), wxT("Centre"), wxT("Right") };

            return new wxGridCellChoiceEditor(HORZ_CHOICE_COUNT, horzChoices);
        }
    case DialogMain::TABPANE_POS:
        {
            static const int TAB_POS_COUNT = 2;
            const wxString tabChoices[TAB_POS_COUNT] = { wxT("Top"), wxT("Bottom") };
            return new wxGridCellChoiceEditor(TAB_POS_COUNT, tabChoices);
        }
    case DialogMain::VERT_TEXT_FORMAT_TYPE:
        {
            static const int VERT_TEXT_FORMAT_CHOICE_COUNT = 3;
            const wxString vert_formatChoices[VERT_TEXT_FORMAT_CHOICE_COUNT] = { wxT("TopAligned"), wxT("VertCentred"), wxT("BottomAligned") };

            /** Because the image's vertical formatting named as the text's, we need 2 vector one for the texts
            and one for the images. If the currently selected window has the "Image" property, we return with
            the image formatting properties otherwise its a text.

            Same for the horz formatting.
            */
            static const int VERT_IMAGE_FORMAT_CHOICE_COUNT = 5;
            const wxString vert_image_formatChoices[VERT_IMAGE_FORMAT_CHOICE_COUNT] = { 
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::vertFormatToString(CEGUI::VF_TOP_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::vertFormatToString(CEGUI::VF_CENTRE_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::vertFormatToString(CEGUI::VF_BOTTOM_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::vertFormatToString(CEGUI::VF_STRETCHED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::vertFormatToString(CEGUI::VF_TILED))};

                if (m_currentWindow->isPropertyPresent("Image"))
                    return new wxGridCellChoiceEditor(VERT_IMAGE_FORMAT_CHOICE_COUNT, vert_image_formatChoices);
                else
                    return new wxGridCellChoiceEditor(VERT_TEXT_FORMAT_CHOICE_COUNT, vert_formatChoices);
        }
    case DialogMain::HORZ_TEXT_FORMAT_TYPE:
        {
            static const int HORZ_TEXT_FORMAT_CHOICE_COUNT = 8;
            const wxString horz_formatChoices[HORZ_TEXT_FORMAT_CHOICE_COUNT] = { wxT("LeftAligned"), wxT("RightAligned"), wxT("HorzCentred"),
                wxT("Justified"), wxT("WordWrapLeftAligned"), wxT("WordWrapRightAligned"), wxT("WordWrapCentred"), wxT("WordWrapJustified") };

            static const int HORZ_IMAGE_FORMAT_CHOICE_COUNT = 5;
            const wxString horz_image_formatChoices[HORZ_IMAGE_FORMAT_CHOICE_COUNT] = { 
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::horzFormatToString(CEGUI::HF_LEFT_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::horzFormatToString(CEGUI::HF_CENTRE_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::horzFormatToString(CEGUI::HF_RIGHT_ALIGNED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::horzFormatToString(CEGUI::HF_STRETCHED)),
                StringHelper::ToWXString(CEGUI::FalagardXMLHelper::horzFormatToString(CEGUI::HF_TILED))};

                if (m_currentWindow->isPropertyPresent("Image"))
                    return new wxGridCellChoiceEditor(HORZ_IMAGE_FORMAT_CHOICE_COUNT, horz_image_formatChoices);
                else
                    return new wxGridCellChoiceEditor(HORZ_TEXT_FORMAT_CHOICE_COUNT, horz_formatChoices);
        }
    default:
        // Unsupported
        //logDebug (wxT("No support for property '%s'"), StringHelper::ToWXString(propertyName));
        return NULL;
    }
}

//------------------------------------------------------------------------
void DialogMain::SetPropertyValueFont(int propertyRow, const wxString& propertyName, const bool forceRefresh /* = false */)
{
    // If the property is at its default value in ALL the windows in the current
    // user selection (widgets where the property doesn't exist are ignored), the
    // employed font will be normal weight. Otherwise, if at least one widget contains
    // the property with another value than the default one, the font will be bold.
    // NOTE: Please note that if we have multi-selection and the property contains
    // different values in the selected windows, the text will be empty (this is 
    // done elsewhere - GetRelevantProperties) and the type of font will 
    // have no impact since no text will be displayed. This is only relevant for single selection or 
    // for multi-selection where the property has the same value for all 
    // elements in it (be it the default or other value).
    wxFont font(*wxNORMAL_FONT);
    if(IsPropertyDefaultInSelection(propertyName))
        font.SetWeight(wxFONTWEIGHT_NORMAL);
    else
        font.SetWeight(wxFONTWEIGHT_BOLD);
    m_grid->SetCellFont(propertyRow, COLUMN_VALUE, font);

    // Force grid refresh (if so desired)
    if(forceRefresh)
        m_grid->ForceRefresh();
}

//------------------------------------------------------------------------
bool DialogMain::IsPropertyDefaultInSelection(const wxString& propertyName)
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));
    wxASSERT_MSG(!propertyName.IsEmpty(), wxT("Supplied property name is empty!"));

    // Make sure an user selection exists
    if(m_document->GetSelection())
    {
        // Scan entire user selection
        Selection::Boxes& boxes = m_document->GetSelection()->GetAllBoxes();
		for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {
            // Obtain currently selected widget
            const CEGUI::Window* const currentWindow = (*boxIt).GetWindow();

            // Check if the property is at its default value (if it exists)
            if(currentWindow->isPropertyPresent(StringHelper::ToCEGUIString(propertyName)) && 
                !currentWindow->isPropertyDefault(StringHelper::ToCEGUIString(propertyName)))
                return false;         
        }
    }

    // Supplied property is at its default value
    // NOTE: We're also returning true if the selection doesn't exist
    return true;
}

//------------------------------------------------------------------------
void DialogMain::ApplyFakePropertyChange()
{
    // Validations
    wxASSERT_MSG(m_grid != NULL, wxT("Can't apply fake properties on empty grid"));
    wxASSERT_MSG(m_lastSelectedWindow != NULL, wxT("Can't apply fake properties on empty window"));
    wxASSERT_MSG(m_document != NULL, wxT("Can't apply fake properties on empty document"));
    wxASSERT_MSG(m_tree != NULL, wxT("Can't apply fake properties on empty tree"));

    // Just update the entire 'area rect'.
    CEGUI::URect newWindowArea;

    // The fake properties have row indexes 0 to 9
    // First one is the name. Only start this process when actually modified.
	if (m_lastSelectedRow == 0)
	{
		const CEGUI::String newName = StringHelper::ToCEGUIString(m_grid->GetCellValue(m_lastSelectedRow, COLUMN_VALUE));
		LogDebug (wxT("New name: '%s'"), StringHelper::ToWXString(newName).c_str());
		LogDebug (wxT("Current name: '%s'"), StringHelper::ToWXString(m_lastSelectedWindow->getName()).c_str());
		if (newName != m_lastSelectedWindow->getName())
		{
			try
			{
				CEGUI::WindowManager::getSingletonPtr()->renameWindow(m_lastSelectedWindow, newName);
				//m_lastSelectedWindow->rename(newName);
				// Update the tree item too
				const std::map<CEGUI::Window*, wxTreeItemId>::iterator it = m_treeItemMapping.find(m_lastSelectedWindow);
				if (it != m_treeItemMapping.end())
				{
					m_tree->SetItemText(it->second, m_grid->GetCellValue(m_lastSelectedRow, COLUMN_VALUE));
				}
			}
			// Add exception to log
			LOG_EXCEPTION("DialogMain::ApplyFakePropertyChange", "Error", "Unexpected standard exception while changing fake property", "Unexpected non-standard exception while changing fake property");
		}
	}
	else
	{
        wxString ctrlName = m_grid->GetCellValue(0,COLUMN_VALUE);
        if(ctrlName.compare(L"") == 0)
            return;
        //新设置的X,Y屏幕坐标
        long newX = StringHelper::WXStringToFloat(m_grid->GetCellValue(1, COLUMN_VALUE));
        long newY = StringHelper::WXStringToFloat(m_grid->GetCellValue(2, COLUMN_VALUE));
        long newPixelWidth  = StringHelper::WXStringToFloat(m_grid->GetCellValue(3, COLUMN_VALUE));
        long newPixelHeight = StringHelper::WXStringToFloat(m_grid->GetCellValue(4, COLUMN_VALUE));
        
        //获取像素宽高度
        long lOldX      = CEGUI::CoordConverter::windowToScreenX(*m_currentWindow,0);
        long lOldY      = CEGUI::CoordConverter::windowToScreenY(*m_currentWindow,0);
        long lOldWidth  = m_currentWindow->getPixelSize().d_width;
        long lOldHeight = m_currentWindow->getPixelSize().d_height;
		
        float ParentX = 0.0f;
        float ParentY = 0.0f;
        float ParentWidth  = m_document->GetWidth();
        float ParentHeight = m_document->GetHeight();
        CEGUI::URect  ParentRect(CEGUI::UDim(0,0),CEGUI::UDim(0,0),CEGUI::UDim(1,0),CEGUI::UDim(1,0));
        if(m_currentWindow->getParent())
        {
            ParentX = CEGUI::CoordConverter::windowToScreenX(*m_currentWindow->getParent(),0);
            ParentY = CEGUI::CoordConverter::windowToScreenY(*m_currentWindow->getParent(),0);
            ParentWidth = m_currentWindow->getParent()->getPixelSize().d_width;
            ParentHeight = m_currentWindow->getParent()->getPixelSize().d_height;
            ParentRect = m_currentWindow->getParent()->getArea();
        }

        long sLeft   = newX - lOldX;
        long sTop    = newY - lOldY;
        long sRight  = lOldX + newPixelWidth - lOldX - lOldWidth;
        long sBottom = lOldY + newPixelHeight - lOldY - lOldHeight;
        if (abs(sLeft) || abs(sTop) || abs(sRight) || abs(sBottom))
        {
            m_document->Modify(true);
            //SelectionMover mover(m_document->GetSelection());
            //mover.UpdateWindowAreas(sLeft, sTop, sRight, sBottom);
            //newWindowArea = m_currentWindow->getArea();
		    newWindowArea.d_min.d_x.d_scale = float(newX - ParentX) / float(ParentWidth);
            newWindowArea.d_min.d_y.d_scale = float(newY - ParentY) / float(ParentHeight);
            newWindowArea.d_max.d_x.d_scale = float(newX - ParentX + newPixelWidth) / float(ParentWidth);
            newWindowArea.d_max.d_y.d_scale = float(newY - ParentY + newPixelHeight) / float(ParentHeight);
            
            newWindowArea.d_min.d_x.d_offset = 0;
            newWindowArea.d_min.d_y.d_offset = 0;
            newWindowArea.d_max.d_x.d_offset = 0;
            newWindowArea.d_max.d_y.d_offset = 0;
            // Apply it to the CEGUI window
		    WindowBox* const box = m_document->GetSelection()->FindBoxFor(m_lastSelectedWindow);
		    if (box)
		    {
			    box->SetNewWindowArea(newWindowArea);
		    }
		    else
		    {	// Not the same window. Update area anyway!
			    m_lastSelectedWindow->setArea(newWindowArea);
		    }
        }
	}
    // Await a new selection
    //m_lastSelectedWindow = NULL;
    //m_lastSelectedRow = -1;
    //CEGUI::ProgressBar *probar = static_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("Login/ProgressBar"));
    //const CEGUI::WidgetLookFeel & lookNFeel = CEGUI::WidgetLookManager::getSingleton().getWidgetLook(probar->getLookNFeel());
    //const CEGUI::ImagerySection & ImaSec = lookNFeel.getImagerySection("frame");
}

/***********..*************************************************************
 * MISC PRIVATE FUNCTIONS
 ************************************************************************/

//------------------------------------------------------------------------
void DialogMain::Reset() 
{
    // Cleanup
    ClearTree();
    ClearGrid();

    (void)m_addWindow->Disable();
    (void)m_editText->Disable();

    m_currentRoot = NULL;
    m_currentWindow = NULL;

	m_lastSelectedWindow = NULL;
	m_lastSelectedRow = -1;
}

//------------------------------------------------------------------------
void DialogMain::ClearTree()
{
    if(m_tree)
    {
        m_tree->DeleteAllItems();
        m_treeItemMapping.clear();
    }
}

//------------------------------------------------------------------------
void DialogMain::HighlightCurrentWindow()
{
    // Validations
    wxASSERT_MSG(m_tree != NULL, wxT("Tree object is NULL"));
    wxASSERT_MSG(m_addWindow != NULL, wxT("Add Window button is NULL"));

    // Select + highlight (make bold) new item  
    const std::map<CEGUI::Window*, wxTreeItemId>::iterator it = m_treeItemMapping.find (m_currentWindow);
    if (it != m_treeItemMapping.end())
    {
        // Un-bold the current item
        //if (m_boldItem.IsOk())
        const std::map<CEGUI::Window*, wxTreeItemId>::iterator boldItemIt =
            m_treeItemMapping.find(m_boldItemWindow);
        if (boldItemIt != m_treeItemMapping.end())
        {
            //m_tree->SetItemBold(m_boldItem, false);
            m_tree->SetItemBold(boldItemIt->second, false);
        }

        // Bold and show new item
        m_tree->SelectItem(it->second) ;
        m_tree->EnsureVisible (it->second) ;
        m_tree->Expand(it->second) ;
        m_tree->SetItemBold(it->second, true);
        if(m_document->isLocked (m_currentWindow))
            m_tree->SetItemImage(it->second,0);
        else
            m_tree->SetItemImage(it->second,-1);
        // Store for un-bolding later on
        //m_boldItem = it->second;
        m_boldItemWindow = it->first;

        // Enable the Add Element button
        (void)m_addWindow->Enable() ;
    }
}

//------------------------------------------------------------------------
void DialogMain::FindAvailableFonts () 
{
    wxArrayString	fonts;

    // Only do this once (assuming at least one default font)
    if (m_fonts == NULL)
    {
        CEGUI::FontManager::FontIterator fontIt = CEGUI::FontManager::getSingletonPtr ()->getIterator () ;
        while (!fontIt.isAtEnd())
        {
            LogDebug (wxT("PropertyGrid::FindAvailableFonts - Found font: %s."), StringHelper::ToWXString(fontIt.getCurrentKey()).c_str());
            (void)fonts.Add (StringHelper::ToWXString(fontIt.getCurrentKey())) ;
            ++fontIt;
        }

        // Found any?
        if (fonts.GetCount() > 0)
        {
            m_fontCount = fonts.GetCount();
            m_fonts = new wxString[m_fontCount];
            if (m_fonts)
            {
                for (size_t i=0; i<m_fontCount; i++)
                {
                    m_fonts[i] = fonts[i];
                }
            }
        }
        else
        {
            LogError (wxT("PropertyGrid::FindAvailableFonts - Serious memory problems while allocating %d strings."), m_fontCount);
        }
    }
}

//------------------------------------------------------------------------
void DialogMain::InitLayoutTree (CEGUI::Window* window, const wxTreeItemId& parent, const bool addThis) 
{
    // Validations 
    wxASSERT_MSG(m_tree != NULL, wxT("Tree object member is NULL"));
    wxASSERT_MSG(window != NULL, wxT("Supplied window is NULL"));

    wxTreeItemId node;
    // Either add a new node, or add to the parent. The latter allows us to 'skip' helper widgets,
    // such as _TabPane_'s.
    if (addThis)
    {
        node = m_tree->AppendItem(parent, StringHelper::ToWXString(window->getName())) ;
        // Cache the window / treeItem pair for for "bolding" and "un-bolding".
        AddTreeItemMapping (window, node) ;
    }
    else
    {
        node = parent;
    }

    // Add its children
    for (size_t i = 0; i < window->getChildCount(); ++i) 
    {	// tuan.kuranes@free.fr
        // even auto element may have editable properties (name)
        // others should be detected when edited.
        // Auto- or Tab-helpers should not have their own node. They get skipped, but their
        // children may be added. Same for PopupMenus, which are also skipped. 
        // (See the filtering in DialogAddWindow for more info).
        if (window->getChildAtIdx (i)->getName().find(wx_static_cast(CEGUI::utf8*, wx_static_cast(void*, wx_const_cast(char *, "__auto_")))) == CEGUI::String::npos && 
            window->getChildAtIdx (i)->getName().find("__TabPane__") == CEGUI::String::npos &&
            !window->getChildAtIdx (i)->testClassName("PopupMenu"))
        {    
            // Just an ordinary widget; branch may be added
            InitLayoutTree (window->getChildAtIdx (i), node) ;
        }
        // Does helper widget has children?
        else if (window->getChildAtIdx (i)->getChildCount () > 0)
        {	
            // Yes: probably a _TabPane_ or _MenuBar_; branch may be added, but re-use the last node,
            // effectively skipping the helper parent.
            InitLayoutTree (window->getChildAtIdx (i), node, false) ;
        }
        else
        {
            LogTrace (wxT("initLayoutTree - stopped expanding at '%s'."), StringHelper::ToWXString(window->getChildAtIdx (i)->getName()).c_str());
        }
    }
}
/* OLD VERSION

void DialogMain::InitLayoutTree (CEGUI::Window* window, wxTreeItemId& parent, bool addThis) 
{
wxTreeItemId node = m_tree->AppendItem(parent, StringHelper::ToWXString(window->getName())) ;

// Cach the window / treeItem pair for for "bolding" and "un-bolding".
AddTreeItemMapping (window, node) ;

// Add it's children
for (size_t i = 0; i < window->getChildCount(); ++i) 
{    
// Deny automatically added elements.
//if (window->getChildAtIdx (i)->getName().find("__auto_") == String::npos && window->getChildAtIdx (i)->getName().find("__TabPane__") == String::npos)
{      
// This branche may be added
InitLayoutTree (window->getChildAtIdx (i), node) ;
}
//else 
//{
//LogTrace (wxT("initLayoutTree - denied (cut of) %s."), StringHelper::ToWXString(window->getChildAtIdx (i)->getName()));
//}
}
}*/

void DialogMain::UpdateTreeImage(CEGUI::Window * pCurWnd)
{
    const std::map<CEGUI::Window*, wxTreeItemId>::iterator it = m_treeItemMapping.find (pCurWnd);
    if (it != m_treeItemMapping.end())
    {
        if(m_document->isLocked (pCurWnd))
            m_tree->SetItemImage(it->second,0);
        else
            m_tree->SetItemImage(it->second,-1);
    }
}
//------------------------------------------------------------------------
void DialogMain::AddTreeItemMapping (CEGUI::Window* pWindow, const wxTreeItemId& treeItem) 
{
    // Validations
    wxASSERT_MSG(pWindow != NULL, wxT("Supplied window is NULL"));

    (void)m_treeItemMapping.insert (std::make_pair (pWindow, treeItem)) ;
    LogTrace (wxT("Cache element/tree pair for '%s'"), StringHelper::ToWXString(pWindow->getName()).c_str()) ;
}

//------------------------------------------------------------------------
CEGUI::Window* DialogMain::FindWindow (const wxTreeItemId& treeItem)
{
    CEGUI::Window* window;
    std::map<CEGUI::Window*, wxTreeItemId>::iterator it ;
    for (it = m_treeItemMapping.begin(); it != m_treeItemMapping.end (); ++it)
    {
        if (it->second == treeItem) 
        {
            window = it->first;
            //logDebug (wxT("DialogMain::findWindow - Selected window: %s"), StringHelper::ToWXString(window->getName()));
            return window;
        }
    }

    // Can happen when the user right-clicks somewhere in the tree
    return NULL;
}

//------------------------------------------------------------------------
void DialogMain::SyncTree()
{
    // Validations
    wxASSERT_MSG(m_tree != NULL, wxT("Tree object is NULL"));

    m_treeItemMapping.clear();
    m_tree->DeleteAllItems();

    // Build up a new tree?
    if (m_currentRoot)
    {
        const wxTreeItemId parent = m_tree->GetRootItem();
        // Add a parent, pass the root
        InitLayoutTree (m_currentRoot, parent);

        // Update the visibility and bold-ness state
        HighlightCurrentWindow();
    }
}

//------------------------------------------------------------------------
bool DialogMain::WidgetLookExists(const CEGUI::String& widget, const CEGUI::String& look)
{
    // Locate an iterator to the first pair object (widget/look) associated with the widget
    WidgetLookHash::iterator widgetLook =  m_widgetLooks.find(widget);
    if(widgetLook == m_widgetLooks.end())
        return false; // No looks associated with widget, so return immediately

    // Get an iterator to the element that is one past the last look associated with widget
    const WidgetLookHash::iterator lastWidgetLook = m_widgetLooks.upper_bound(widget);

    // For each element in the sequence [widget, lastWidgetElement)
    for ( ; widgetLook != lastWidgetLook; ++widgetLook)
    {
        // Is this the look we are looking for?
        if((*widgetLook).second == look)
            return true;
    }

    // Widget look not found
    return false;
}

//------------------------------------------------------------------------
DialogMain::WidgetLooks DialogMain::GetWidgetLooks(const CEGUI::String& widget)
{
    WidgetLooks widgetLooks;

    //Locate an iterator to the first pair object (widget/look) associated with the widget
    WidgetLookHash::iterator widgetLook =  m_widgetLooks.find(widget);
    if(widgetLook == m_widgetLooks.end())
        return widgetLooks; // No looks associated with widget, so return immediately

    // Get an iterator to the element that is one past the last look associated with widget
    const WidgetLookHash::iterator lastWidgetLook = m_widgetLooks.upper_bound(widget);

    // For each element in the sequence [widget, lastWidgetElement)
    for ( ; widgetLook != lastWidgetLook; widgetLook++)
        widgetLooks.push_back((*widgetLook).second);

    // Return existing looks for supplied widget
    return widgetLooks;
}


//------------------------------------------------------------------------
DialogMain::LookAndWidget DialogMain::GetLookAndWidgetFromType(CEGUI::Window *window) const
{
    LookAndWidget result;
    const CEGUI::String separator("/");
    CEGUI::String::size_type pos;

    // Store current window
    result.window = window;
    if(!window)
        return result;

    // Obtain look and widget kind from the supplied window type
    const CEGUI::String type = window->getType();
    pos = type.find(separator);
    CEGUI::String look(type, 0, pos);
    CEGUI::String widget(type, pos + 1);

    // Store and return result
    result.look = look;
    result.widget = widget;
    return result;
}

//------------------------------------------------------------------------
void DialogMain::MapWidgetSelectionPossibleSkins()
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Document object is NULL"));

    WidgetLooks widgetLooks;
    const CEGUI::String separator("/");
    LookAndWidget currentLookAndWidget;

    // Check if we have a single or multi selection
    if (m_document->GetSelection()->Size() == 1)
    {
        // Single selection, so obtain and sort available looks for the selected widget
        currentLookAndWidget = GetLookAndWidgetFromType(m_currentWindow);
        widgetLooks = GetWidgetLooks(currentLookAndWidget.widget);
        widgetLooks.sort();
    }
    else
    {
        // Multi-selection, so we need to check the available looks for all the widgets
        // in the current selection. Let's start by retrieving the first one in the selection
        Selection::Boxes::iterator boxIt = m_document->GetSelection()->GetAllBoxes().begin();
        CEGUI::Window *currentWindow = (*boxIt).GetWindow();

        // Obtain available looks for the first widget in the selection
        currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);
        widgetLooks = GetWidgetLooks(currentLookAndWidget.widget);

        // Scan the rest of the selection
        for (; boxIt != m_document->GetSelection()->GetAllBoxes().end(); ++boxIt)
        {
            // Obtain current widget in the selection
            currentWindow = (*boxIt).GetWindow();

            // Obtain available looks for the current widget in the selection
            currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);
            WidgetLooks currentWidgetLooks = GetWidgetLooks(currentLookAndWidget.widget);

            // Check currently chosen skin display type
            if(m_displayOnlyCommon)
                // The user has chosen to only display skins that are common to ALL the 
                // elements in the selection. Therefore, we eliminate from the global widgetLooks list
                // all those that are not present in the current widget look list. In the end, 
                // widgetLooks will only hold the common skins.
                (void)widgetLooks.erase(std::remove_if( widgetLooks.begin(), widgetLooks.end(), NotIn(currentWidgetLooks)), widgetLooks.end());
            else
            {
                // The user has chosen to display all skins, even if they do not apply to all the widgets
                // in the selection. Therefore, we add the current widget look list to the global
                // widgetLooks list (we'll deal with the duplicates later on)
                COMPILE_TODO("This is is not the best approach (in other words, lazy). We should only insert non-duplicate elements instead of inserting everything and then removing it later on with a erase/unique combo. But performance is not really critical here.")
                widgetLooks.splice(widgetLooks.end(), currentWidgetLooks);
            }
        }

        // Sort the available looks of the widgets in the selection
        widgetLooks.sort();

        // If we are displaying all the skins, we need to eliminate the duplicate entries
        if(!m_displayOnlyCommon)
        {
            // Eliminate all duplicates
            // NOTE: Elements need to be sorted before calling this, otherwise the unique function 
            // (since it only processes consecutive duplicates) will have problems
            COMPILE_TODO("Lazy approach. See TODO on the splice method() above for more info")
            (void)widgetLooks.erase(std::unique(widgetLooks.begin(), widgetLooks.end()), widgetLooks.end());
        }
    }

    // We're going to repopulate the skin dropdown, so let's start by clearing it
    m_skinList->Clear();

    // Scan the available looks discovered earlier on
    CEGUI::WindowFactoryManager& wfMgr = CEGUI::WindowFactoryManager::getSingleton();
    WidgetLooks::iterator widgetIter = widgetLooks.begin();
    for(;widgetIter != widgetLooks.end(); widgetIter++)
    {
        bool valid = true;
        if(m_displayOnlyCommon)
        {
            // (Safety check) Make sure that the current look can be applied to each widget in the selection
            // NOTE: No problems should occur here, since the possible looks were already mapped at the
            // start of the application
			Selection::Boxes& boxes = m_document->GetSelection()->GetAllBoxes();
			for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
            {
                // Build a look type based on the current widget and the current look
                CEGUI::Window* const currentWindow = (*boxIt).GetWindow();
                currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);
                const CEGUI::String lookType = (*widgetIter) + separator + currentLookAndWidget.widget;

                // Make sure that the type is valid one
                // NOTE: This should never fail
                if (!wfMgr.isFalagardMappedType(lookType))
                    valid = false;
            }
        }

        // Make sure we are allowed to proceed
        if (valid)
        {
            // Add look to the skin dropdown
            m_skinList->AppendString(StringHelper::ToWXString(*widgetIter));

            // If we are doing a single selection, check if the current look is the one
            // that is currently mapped on the selected widget. If so, select it.
            COMPILE_TODO("For multi-selection, also select value if all share the same skin")
            if(m_document->GetSelection()->Size() == 1)
            {
                // Select look if it's the one currently mapped
                // NOTE: No worries here, since this is a single selection, so the
                // currentLookAndWidget struct will hold the look and widget we desire
                if(*widgetIter == currentLookAndWidget.look)
                    m_skinList->SetValue(StringHelper::ToWXString(*widgetIter));
            } // if
        } // if
    } // for

    // Should we allow the user to choose a skin based on the current selection?
    // NOTE: When in multi-selection mode, we want to keep the dropdown enabled 
    // even when only a single skin is available because not all selections might have
    // that skin applied
    if( (m_skinList->GetCount() > 1 && m_document->GetSelection()->Size() == 1) ||
        (m_skinList->GetCount() > 0 && m_document->GetSelection()->Size() > 1))
        (void)m_skinList->Enable(true);
    else
        (void)m_skinList->Enable(false);
}

//------------------------------------------------------------------------
void DialogMain::UpdateSelectionInfo()
{
    // Validations
    wxASSERT_MSG(m_skinList != NULL, wxT("Skin list wasn't properly initialized"));
    wxASSERT_MSG(m_typeText != NULL, wxT("Type text wasn't properly initialized"));
    wxASSERT_MSG(m_editText != NULL, wxT("Edit text wasn't properly initialized"));
    wxASSERT_MSG(m_displayModeRadio != NULL, wxT("Skin display radio wasn't properly initialized"));

    // The displayed info is totally dependent on the selection count, so let's start
    // by checking that
    if (m_document && m_document->GetSelection()->Size() == 1)
    {
        // Validations
        wxASSERT_MSG(m_currentWindow != NULL, wxT("Selection count is 1 and current window is empty"));
        
        // Single selection : Display currently selected widget type, text and skin and
        // allow the user to change the last two
        const LookAndWidget currentLookAndWidget = GetLookAndWidgetFromType(m_currentWindow);
        m_skinList->SetValue(StringHelper::ToWXString(currentLookAndWidget.look));
        m_typeText->SetLabel(StringHelper::ToWXString(m_currentWindow->getType()));
		// Mute OnChange events
		m_updating = true;
        if(m_currentWindow->isPropertyPresent("Text"))
		{	
            m_editText->SetValue(StringHelper::ToWXString(m_currentWindow->getProperty("Text")));
		}
        else
		{
            m_editText->SetValue(wxT("Property \"Text\" not supported in current selection"));
		}
		m_updating = false;
        (void)m_displayModeRadio->Enable(false);
        (void)m_skinList->Enable();
        (void)m_editText->Enable();
    }
    else if (m_document && m_document->GetSelection()->Size() > 1)
    {
        // Multi-selection : Display common info about the selected elements
        // NOTE: For the moment, type and text don't support multi-selection, so 
        // let's inform the user and leave it at that
        COMPILE_TODO("Allow scrollable text area which displays the names of all the types in the selection")
        m_typeText->SetLabel(wxT("Multi-selection detected. Not yet supported."));
        //m_editText->SetValue(wxT("Multi-selection detected. Not yet supported."));
        (void)m_displayModeRadio->Enable(true);
        (void)m_skinList->Enable();
        //(void)m_editText->Disable();
    }
    else
    {
        // No selection : Clear all info and prevent user iteration
        m_typeText->SetLabel(wxT(""));
        m_editText->SetValue(wxT(""));
        m_skinList->Clear();
        (void)m_skinList->Disable();
        (void)m_displayModeRadio->Enable(false);
        (void)m_editText->Disable();
    }
}

//////////////////////////////////////////////////////////////////////////
// DIALOG MAIN PROTECTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * EVENTS
 ************************************************************************/

//------------------------------------------------------------------------
// Do nothing. This effectively refuses to close the dialog when Escape is pressed.
void DialogMain::OnCancel(wxCommandEvent& WXUNUSED(event))
{
}

//------------------------------------------------------------------------
// Do nothing. This effectively refuses to close the dialog when Enter is pressed.
void DialogMain::OnOK(wxCommandEvent& WXUNUSED(event))
{
}

//------------------------------------------------------------------------
void DialogMain::OnAddWindow(wxCommandEvent& WXUNUSED(event)) 
{
    try
    {
		// If we don't have a current window, just pick the root.
		if (!m_currentWindow )
		{
			m_currentWindow = CEGUI::System::getSingletonPtr()->getGUISheet();
		}
		
		bool allowed = false;
		if (m_currentWindow )
		{	// If we still don't have a current window, the new one will be the first one
			// Determine the context of the current window
			WindowContext windowContext (m_currentWindow);
			allowed = windowContext.AcceptsWindowAsChild();
		}
		else
		{	// Will be the first one, so no limitations yet
			allowed = true;
		}
        if (allowed)
        {    
            // Create new 'Add Window' Dialog
            DialogAddWindow *addWindow = new DialogAddWindow(this, m_currentWindow);

            // Present dialog to user (modal)
            if (addWindow->ShowModal () == wxID_OK)
            {
                wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
                // Add to Document via a command. This allows Undo and Redo.
                //(void)m_document->GetCommandProcessor()->Submit(
                //    new CommandAddWindow(m_document, m_currentWindow, StringHelper::ToCEGUIString(addWindow->GetWindowType()), StringHelper::ToCEGUIString(addWindow->GetWindowName())));
				m_document->AddWindow(m_currentWindow, StringHelper::ToCEGUIString(addWindow->GetWindowType()), StringHelper::ToCEGUIString(addWindow->GetWindowName()));
            }

            (void)addWindow->Destroy();
            delete addWindow;
        }
        else
        {
            LogWarning(wxT("This widget does not support children via XML.\nSee the CEGUI API for programmatically adding children to this widget."));
        }
    }
    // If something goes wrong, show user
    SHOW_EXCEPTION("DialogMain::OnAddWindow", "Error", "Unexpected standard exception while adding window", "Unexpected non-standard exception while adding window");
}

//------------------------------------------------------------------------
void DialogMain::OnSaveSelection(wxCommandEvent& WXUNUSED(event))
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

    // Restore directory before returning otherwise all file-access
    // will happen in this folder, including saving of layouts and such.
    const wxString currentDirectory = wxGetCwd();

    wxFileDialog dialog(this, wxT("Open a layout"), currentDirectory, wxT(""), wxT("Layout files (*.xml;*.layout;*.*)|*.xml;*.layout;*.*"), wxSAVE);
    if (dialog.ShowModal() == wxID_OK)
    {
        m_document->SaveFromWindow(m_currentWindow,dialog.GetPath());
    }

    (void)wxSetWorkingDirectory(currentDirectory);
    //LogWarning(wxT("This feature is not enabled yet. Working on it..."));
}

//------------------------------------------------------------------------
void DialogMain::OnSelchangedLayoutTree(wxTreeEvent& event) 
{
    CEGUI::Window* selectedWindow;
    // Resolve the treeItem->window mapping

    // Because this event also gets generated when we call m_tree->SelectItem in the
    // HightlighWindow method, we "if" on the return value to deny a second call.
    selectedWindow = FindWindow (event.GetItem());

    // Deny first event.
    if (selectedWindow != m_currentWindow && selectedWindow != NULL)
    {
        m_currentWindow = selectedWindow;
        // This will eventually get the "elementSelected" method being called.
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        bool isCtrlDown = wxIsCtrlDown();
        m_document->SelectWindow (m_currentWindow,!isCtrlDown);

        // Enable the Add Element button
        (void)m_addWindow->Enable() ;
    }
}

//------------------------------------------------------------------------
void DialogMain::OnChangeText(wxCommandEvent& WXUNUSED(event))
{
	if (m_updating)
		return;

    // Validations
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    wxASSERT_MSG(m_editText != NULL, wxT("Edit text object is NULL"));

    // Prevent any changes in multi-selection
    if (m_document->GetSelection()->Size() == 1)
        // Allow empty string!
        (void)m_document->SetProperty ("Text", StringHelper::ToCEGUIString(m_editText->GetValue()), true);
}

//------------------------------------------------------------------------
void DialogMain::OnGridCellSelected(wxGridEvent& event)
{
    // Validations
    wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));
    wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

	if (event.GetCol() == (int)COLUMN_VALUE)
	{
		// Obtain grid row the user just selected
		m_lastSelectedRow = event.GetRow();
		m_lastSelectedWindow = m_currentWindow;
		if (m_lastSelectedWindow)
		{
			LogDebug (wxT("You selected a row in: '%s'"), StringHelper::ToWXString(m_lastSelectedWindow->getName()).c_str());
		}
	}
	event.Skip(); // continue
}

//------------------------------------------------------------------------
void DialogMain::OnGridCellChange(wxGridEvent& event)
{
    if(m_currentWindow && m_document->isLocked(m_currentWindow) == false)
    {
        long lSaveRow = event.GetRow();
	    SaveCurrentProperty(lSaveRow);
    }
}

//------------------------------------------------------------------------
void DialogMain::SaveCurrentProperty(long lSaveRow)
{
    // Validations
    wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));
    wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

    if (m_lastSelectedWindow != NULL && lSaveRow >= 0)
    {
        // Is it a real property or a fake one?
        // NOTE: Fake properties have indices in the range [0, FAKE_PROPERTY_COUNT]
        //if (m_lastSelectedRow < FAKE_PROPERTY_COUNT)
        //{
        //    // Non existing property, handle manually
        //    ApplyFakePropertyChange();
        //}
        ApplyFakePropertyChange();
        // "Real" CEGUI property, set it directly.
        const wxString propertyName = m_grid->GetCellValue(lSaveRow, COLUMN_NAME);
        if (m_lastSelectedWindow && m_lastSelectedWindow->isPropertyPresent(StringHelper::ToCEGUIString(propertyName)))
        {
            const wxString propertyValue = m_grid->GetCellValue(lSaveRow, COLUMN_VALUE);
            (void)m_document->SetProperty(StringHelper::ToCEGUIString(propertyName), StringHelper::ToCEGUIString(propertyValue));

            // Set the correct font (normal if property is at default value in all elements in
            // the current selection, bold otherwise)
            SetPropertyValueFont(lSaveRow, propertyName, true);
        }
        // Not via the Document, so refresh the frame ourselves
        wxGetApp().GetMainFrame()->Refresh();
    }
}

//------------------------------------------------------------------------
void DialogMain::OnGridColSize(wxGridSizeEvent& WXUNUSED(event))
{
	// Re-arrange columns within current space
    UpdateColumnWidths();
}

//------------------------------------------------------------------------
void DialogMain::OnResize(wxSizeEvent& event)
{
    UpdateColumnWidths();
    // Let the parent handle this event as well(!)
    event.Skip();
}

//------------------------------------------------------------------------
void DialogMain::OnChangeSkin(wxCommandEvent& WXUNUSED(event))
{
    // Apply new Look'N'Feel to current widget selection
    SetWidgetsLook(m_document->GetSelection(), StringHelper::ToCEGUIString(m_skinList->GetValue()));
}

//------------------------------------------------------------------------
void DialogMain::OnDisplayModeSelection(wxCommandEvent& event) 
{
    // Check new skin and property display type
    // NOTE: The comparison is used to avoid the annoying warning C4800 
    //       int' : forcing value to bool 'true' or 'false' (performance warning)
    m_displayOnlyCommon = (event.GetInt()!=0); 

    // Remap the available skins for the current selection to take in account the new display type
    MapWidgetSelectionPossibleSkins();

    // Do the same for the property panel
    UpdateProperties();
}

//////////////////////////////////////////////////////////////////////////
// DIALOG MAIN PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * MISC PUBLIC FUNCTIONS
 ************************************************************************/

//------------------------------------------------------------------------
void DialogMain::SetDocument (EditorDocument* document) 
{
    m_document = document;
    if (m_document)
    {
        m_document->AddObserver(this);
    }
    Reset();
}

//------------------------------------------------------------------------
void DialogMain::WindowSelected (CEGUI::Window* aWindow) 
{
    // Validations
    wxASSERT_MSG(m_saveSelection != NULL, wxT("Save selection button is NULL"));

    m_currentWindow = aWindow;
    if (m_currentWindow)
    {
        m_currentWindow->moveToFront();
        (void)m_saveSelection->Enable(true);
    }
    else
    {
        (void)m_saveSelection->Enable(false);
    }

    // Refresh the property set for the current selection
    UpdateProperties();

    HighlightCurrentWindow();

    //debug
#ifdef _DEBUG
    {
        WCHAR strTextOut[512];
        wsprintf(strTextOut,L"min_dx_scaleX: %f \n",m_currentWindow->getArea().d_min.d_x.d_scale);
        OutputDebugStr(strTextOut);
        wsprintf(strTextOut,L"min_dy_scaleY: %f \n",m_currentWindow->getArea().d_min.d_y.d_scale);
        OutputDebugStr(strTextOut);
        wsprintf(strTextOut,L"max_dx_scaleX: %f \n",m_currentWindow->getArea().d_max.d_x.d_scale);
        OutputDebugStr(strTextOut);
        wsprintf(strTextOut,L"max_dy_scaleY: %f \n",m_currentWindow->getArea().d_max.d_y.d_scale);
        OutputDebugStr(strTextOut);
    }
#endif
}

//------------------------------------------------------------------------
void DialogMain::LayoutOpened (CEGUI::Window* aRoot) 
{
    m_currentRoot = aRoot;
    m_currentWindow = aRoot;
    SyncTree();

    // Can add windows then
    (void)m_addWindow->Enable();
}

//------------------------------------------------------------------------
void DialogMain::LayoutClosed () 
{
    Reset();

    SyncTree();

    // Cannot add then
    (void)m_addWindow->Disable();
}

//------------------------------------------------------------------------
void DialogMain::LayoutStarted (CEGUI::Window* aRoot) 
{
    m_currentRoot = aRoot;
    m_currentWindow = aRoot;
    SyncTree();

    // Can add windows then
    (void)m_addWindow->Enable();
}

//------------------------------------------------------------------------
void DialogMain::LayoutModified (CEGUI::Window* WXUNUSED(aRoot)) 
{
}

//------------------------------------------------------------------------
void DialogMain::WindowAdded (CEGUI::Window* aWindow) 
{
    if (aWindow) 
    {
        m_currentWindow = aWindow;
		if (m_currentWindow == CEGUI::System::getSingletonPtr()->getGUISheet())
		{
			m_currentRoot = m_currentWindow;
		}
    }
    SyncTree();
}

//------------------------------------------------------------------------
void DialogMain::WindowsRemoved () 
{
    m_currentWindow = NULL;
	// Did the root got removed?
	if (CEGUI::System::getSingletonPtr()->getGUISheet() == NULL)
	{
		m_currentRoot = NULL;
	}
    SyncTree();
}

//------------------------------------------------------------------------
void DialogMain::SelectionDraggedOrResized()
{
    LogDebug(wxT("DialogMain::SelectionDraggedOrResized"));
    // Refresh the property set for the current selection
    UpdateProperties();
}


//------------------------------------------------------------------------
void DialogMain::StoreWidgetLooks(const bool clearBeforeHand)
{
    // Clear the widget look hash multimap if so desired
    if(clearBeforeHand)
        m_widgetLooks.clear();

    // Clear previous available looks
    // NOTE: We always need to clear this, since we want unique entries here
    m_availableLooks.Clear();

    // Scan all existing falagard mappings
    CEGUI::WindowFactoryManager::FalagardMappingIterator falIt = CEGUI::WindowFactoryManager::getSingletonPtr()->getFalagardMappingIterator();
    const CEGUI::String separator("/");
    for(; !falIt.isAtEnd() ; ++falIt)
    {
        // Obtain current falagard mapping (which corresponds to the widget type)
        const CEGUI::String currentFal = falIt.getCurrentKey();

        // Retrieve the widget look and the widget kind
        const CEGUI::String::size_type pos = currentFal.find(separator);
        CEGUI::String look(currentFal, 0, pos);
        CEGUI::String widget(currentFal, pos + 1);

        // Mark look as a possible one for the widget, if this hasn't already been done in the past
        if(!WidgetLookExists(widget,look))
            (void)m_widgetLooks.insert(std::make_pair(widget, look));

        // We also want to store this as a global available Look'N'Feel for later use (e.g. for 
        // instance, for the set layout skin operation). But we need to avoid duplicate entries.
        StringHelper::WXPushBackUnique(m_availableLooks, StringHelper::ToWXString(look));
    }

    // Sort the looks for easier user interaction
    m_availableLooks.Sort();
}

//------------------------------------------------------------------------
void DialogMain::SetWidgetsLook(Selection* currentSelection, const CEGUI::String& newLook)
{
    // Validations
    wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));
    if(!currentSelection)
        return;

    // Apply desired Look'N'Feel to all widgets currently selected
	Selection::Boxes& boxes = currentSelection->GetAllBoxes();
	for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {
        // Apply new Look'N'Feel to widget
        SetWidgetLook((*boxIt).GetWindow(), newLook, false);
    }

    // Refresh the main frame so that the Look'N'Feel changes are immediately visible
    wxGetApp().GetMainFrame()->Refresh();

    // Update current selection displayed properties and info, making sure the skin mapping is not
    // rebuilt since this operation did not affect the selection itself
    UpdateProperties(false);
}

//------------------------------------------------------------------------
void DialogMain::SetWidgetLook(CEGUI::Window *currentWindow, const CEGUI::String& newLook, const bool refresh)
{
    COMPILE_FIXME("Problem with static image... check if renderer problem")

    // Validations
    wxASSERT_MSG(currentWindow != NULL, wxT("Supplied current window is NULL"));

    CEGUI::WindowFactoryManager& wfMgr = CEGUI::WindowFactoryManager::getSingleton();
    const CEGUI::String separator("/");

    // Obtain current window type (in format look/widget)
    const CEGUI::String currentType = currentWindow->getType();

    // Retrieve the widget look and the widget kind
    const CEGUI::String::size_type pos = currentType.find(separator);
    CEGUI::String look(currentType, 0, pos);
    CEGUI::String widget(currentType, pos + 1);

    // Build the new desired type (in format look/widget)
    const CEGUI::String newType = newLook + separator + widget;

    // If no type change is detected, leave
    if(newType == currentType)
        return;

    // Is this a valid mapped type? (safety-check)
    if (wfMgr.isFalagardMappedType(newType))
    {
        // Obtain falagard mapping for type
        const CEGUI::WindowFactoryManager::FalagardWindowMapping& fwm = wfMgr.getFalagardMappingForType(newType);

        // Retrieve and store the currently assigned widget properties that are editable, so 
        // we that can restore them after setting the new Look'N'Feel. This ensures 
        // that changes made by the user are maintained.
        // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and PropertySet (it 
        // would if we used getIterator()) thus avoiding useless casts
        typedef std::map<CEGUI::String, CEGUI::String> PropMap; 
        PropMap originalProps;
        CEGUI::PropertySet::Iterator oit = currentWindow->getPropertyIterator();
        for(; !oit.isAtEnd() ; ++oit)
        {
            // Obtain current property name
            const CEGUI::String propertyName = (*oit)->getName();

            // Check if this property is editable (i.e. supported for user edition inside 
            // the CELayoutEditor). If so, we want to store it for later reference
            if(this->GetPropertyType(propertyName) != UNSUPPORTED)
                (void)originalProps.insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));

            // We also want to keep some properties, that while not directly editable by the user,
            // are affected by the new Look'N'Feel
            COMPILE_TODO("This is not very elegant, and needs to be changed as soon as the application supports more editable properties")
            else if (propertyName == "TextColours" || propertyName == "Image" ||
            propertyName == "MouseCursorImage" || propertyName == "EWSizingCursorImage" ||
            propertyName == "NSSizingCursorImage" || propertyName == "NESWSizingCursorImage" ||
            propertyName == "NWSESizingCursorImage" || propertyName == "ImageColours" ||
            propertyName == "WantsMultiClickEvents")
                (void)originalProps.insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));
        }

        // Assign the new Look'N'Feel to the widget	
        // NOTE: This calls the SetLookNFeel() function internally and updates
        // the falagard type variable. This variable would not be updated if we were 
        // to only call 
        // currentWindow->setProperty("LookNFeel", fwm.d_lookName); 
        // or 
        // currentWindow->setLookNFeel(fwm.d_lookName); 
        try
        {            
            currentWindow->setFalagardType(fwm.d_lookName, fwm.d_rendererType);
        }
        // If something goes wrong, show user and return
        SHOW_EXCEPTION_RETURN("DialogMain::SetWidgetLook", "Error", "Unexpected standard exception while setting widget look", "Unexpected non-standard exception while setting widget look",);

        // Time to restore the original editable properties
        for(PropMap::iterator it = originalProps.begin(); it != originalProps.end(); it++)
        {
            // Obtain current property name
            const CEGUI::String propertyName = (*it).first;

            // Although the Look'N'Feel and WindowRender properties are not 
            // currently editable, we want to REALLY make sure they are not processed here (safety-check)
            if(propertyName != "LookNFeel" && propertyName != "WindowRenderer")
            {
                // Make sure this property is still valid with the new Look'N'Feel
                if(currentWindow->isPropertyPresent(propertyName))
                {
                    // Restore property
                    currentWindow->setProperty(propertyName, (*it).second);

                    // If the property wasn't correctly set (i.e. because the value
                    // is not valid), we want to remove it to prevent garbage
                    // NOTE: This is mostly for the MouseCursorImage property, whose
                    // set() implementation (see CEGUIWindowProperties.cpp) prevents
                    // empty entries.
                    if(currentWindow->getProperty(propertyName) != (*it).second)
                    {
                        COMPILE_TODO("This is not very elegant, and needs to be changed as soon as the application supports more editable properties")
                            if (propertyName == "Image" ||
                                propertyName == "MouseCursorImage" || propertyName == "EWSizingCursorImage" ||
                                propertyName == "NSSizingCursorImage" || propertyName == "NESWSizingCursorImage" ||
                                propertyName == "NWSESizingCursorImage")
                                currentWindow->removeProperty(propertyName);
                    } // if
                } // if
            } // if
        } // for
    } // if

    // Do we need to refresh the screen?
    if(refresh)
        wxGetApp().GetMainFrame()->Refresh();	
}

//------------------------------------------------------------------------
void DialogMain::SetWidgetsLook(const CEGUI::String& newLook)
{
    // Apply desired Look'N'Feel to all widgets in the layout
    CEGUI::WindowManager::WindowIterator windowIt = CEGUI::WindowManager::getSingleton().getIterator();
    while (!windowIt.isAtEnd())
    {
        // Apply new Look'N'Feel to widget and move on to the next one
        SetWidgetLook(windowIt.getCurrentValue(), newLook, false);
        ++windowIt;
    }

    // Refresh the main frame so that the Look'N'Feel changes are immediately visible
    wxGetApp().GetMainFrame()->Refresh();

    // Update current selection displayed properties and info, making sure the skin mapping is not
    // rebuilt since this operation did not affect the selection itself
    UpdateProperties(false);
}
