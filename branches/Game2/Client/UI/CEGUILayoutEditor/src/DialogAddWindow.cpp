







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
#include "DialogAddWindow.h"
#include "Config.h"

#include <map>

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define BORDER            5

//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

#if wxCHECK_VERSION(2,8,0)
    typedef wxTreeItemIdValue cookieType;
#else
    typedef long cookieType;
#endif

//////////////////////////////////////////////////////////////////////////
// DIALOGADDWINDOW FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DialogAddWindow, wxDialog)
// Button-click handler(s)
EVT_TREE_SEL_CHANGED  (ID_WIDGET_TREE, DialogAddWindow::OnSelchangedWidgetTree)
EVT_BUTTON            (wxID_OK, DialogAddWindow::OnOK)
EVT_BUTTON            (wxID_CANCEL, DialogAddWindow::OnCancel)
EVT_RADIOBOX		  (ID_FILTER_BOX, DialogAddWindow::OnFilterBoxSel) 
END_EVENT_TABLE()

//------------------------------------------------------------------------
// Shared
wxString DialogAddWindow::m_previousLooknfeel;

//////////////////////////////////////////////////////////////////////////
// DIALOGADDWINDOW FUNCTIONS
//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------
DialogAddWindow::DialogAddWindow(wxWindow* aParentDialog, const CEGUI::Window* const aParentWindow) :
m_windowName(),
m_windowType(),

// Put the elements to NULL, so we can block events during initialization
m_editName(wx_static_cast(wxTextCtrl*, NULL)),
m_widgetTree(wx_static_cast(wxTreeCtrl*, NULL)),

// Initially we assume a filtered display (only those that are not dependent on parents) 
// of available widgets
m_treeFilter(true), 

// Store parent window for later use (may be NULL!)
m_parentWindow(aParentWindow),

// Put the elements to NULL, so we can block events during initialization
m_filterBox(wx_static_cast(wxRadioBox*, NULL)),
m_okButton(wx_static_cast(wxButton*, NULL)),
wxDialog(aParentDialog, wxID_ANY, wxT("Add Window"), wxDefaultPosition, wxSize(350, 700))
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
void DialogAddWindow::Setup()
{
    // All "groups". Put the parent's name + type in the text field, when a parent is set
    wxStaticBox* const parentGroup = new wxStaticBox(this, wxID_ANY, 
		m_parentWindow ? StringHelper::ToWXString(m_parentWindow->getName() + "(" + m_parentWindow->getType() +  "):")
		: wxT("No parent window"));

    // All static fields
    wxStaticText* const staticName = new wxStaticText(this, wxID_ANY, wxT("The name of the new window:"));
    wxStaticBox* const staticWidgets = new wxStaticBox(this, wxID_ANY, wxT("Available widgets for current parent:"));

    // All edit fields. Default the name to "parentName/"
    m_editName = new wxTextCtrl(this, wxID_ANY, 
		m_parentWindow ? StringHelper::ToWXString(m_parentWindow->getName() + "/")
		: wxT(""));

    // All widgets
    m_widgetTree = new wxTreeCtrl(this, ID_WIDGET_TREE);

    // Size the components vertically
    wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

    // Sizes the tree - this one may be expanded
    // NOTE: No need to free the custodial pointer parentGroup since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const nameSizer = new wxStaticBoxSizer(parentGroup, wxVERTICAL);

    // NOTE: No need to free the custodial pointer staticName since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)nameSizer->Add(staticName, 0, wxEXPAND | wxALL, BORDER);
    (void)nameSizer->Add(m_editName, 0, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer (may not stretch)
    // NOTE: No need to free the custodial pointer nameSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(nameSizer, 0, wxEXPAND | wxALL, BORDER);

    //------------------------------ FILTER BOX -------------------------------

    // Create filter box that will allow the user to switch between displaying all existing widget
    // types or only those that don't require a parent for effective use
    const wxString options[] = {wxT("Show All"), wxT("Show Filtered")};
    m_filterBox = new wxRadioBox(this, ID_FILTER_BOX, wxT("Parent Filter:"), wxDefaultPosition, wxDefaultSize, 2, options);

    // Use entire sizer
    wxBoxSizer* const filterSizer = new wxBoxSizer(wxVERTICAL);
    (void)filterSizer->Add(m_filterBox, 0, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer (may stretch)
    // NOTE: No need to free the custodial pointer filterSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(filterSizer, 0, wxEXPAND | wxCENTER, BORDER);

    // Sizes the tree - this one may be expanded
    // NOTE: No need to free the custodial pointer staticWidgets since
    // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
    wxStaticBoxSizer* const treeSizer = new wxStaticBoxSizer(staticWidgets, wxVERTICAL);

    // Add filter warning to tree sizer
    // NOTE: No need to free the custodial pointer filterWarning since
    // it will be handled internally by wxWidgets wxSizer::Add()
    wxStaticText* const filterWarning = new wxStaticText(this, wxID_ANY, wxT("Widgets that require a parent are blue colored."));
    (void)treeSizer->Add(filterWarning, 0, wxEXPAND | wxALL, BORDER);
    (void)treeSizer->Add(m_widgetTree, 1, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer (may not stretch)
    // NOTE: No need to free the custodial pointer treeSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(treeSizer, 1, wxEXPAND | wxALL, BORDER);

    wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_okButton = new wxButton(this, wxID_OK, wxT("Ok"));
    // Disable until a widget gets selected.
    (void)m_okButton->Enable(false);
    (void)buttonSizer->Add(m_okButton, 0, wxEXPAND | wxALL, BORDER);
    (void)buttonSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxEXPAND | wxALL, BORDER);

    // Add to the root sizer (may stretch)
    // NOTE: No need to free the custodial pointer buttonSizer since
    // it will be handled internally by wxWidgets wxSizer::Add()
    (void)rootSizer->Add(buttonSizer, 0, wxEXPAND | wxCENTER, BORDER);

    // Initially we assume a filtered display (only those that are not dependent on parents) 
    // of available widgets
    m_filterBox->SetSelection(m_treeFilter);

    // Apply root sizer
    // NOTE: No need to free the custodial pointer rootSizer since
    // it will be handled internally by wxWidgets wxWindowBase::SetSizer()
    SetSizer(rootSizer);

    // Build up the widget list to choose from
    InitWidgetTree(m_parentWindow);

    // NB: We should not call 'Show' on a wxDialog we later call ShowModal for.
    //(void)wxDialog::Show(true);

    // Somehow the initial name value gets selected by default, which is annoying cause if you start typing, you overwrite the selection.
    // Therefore we pull this trick, required after Show.
    m_editName->SetFocus();
    m_editName->SetInsertionPointEnd();
}

//------------------------------------------------------------------------
void DialogAddWindow::InitWidgetTree(const CEGUI::Window* const aParentWindow)
{
    // Validations
    wxASSERT_MSG(m_widgetTree != NULL, wxT("Widget tree invalid or NULL"));

    const CEGUI::String separator("/");
    std::map<CEGUI::String, wxTreeItemId> looks;
    std::map<CEGUI::String, wxTreeItemId>::iterator lookIter;

    // If the tree is already populated (probably by a previous 
    // call to this function), clear it so that we avoid duplicate data and
    // errors because of duplicate root nodes (only one per tree allowed)
    if(!m_widgetTree->IsEmpty())
        m_widgetTree->DeleteAllItems();

    // Build up the widget tree. (Not recursive.)

    // Tree-root; not a cegui thing.
    const wxTreeItemId rootNode = m_widgetTree->AddRoot(wxT("Widgets"));

    // Set root node as bold
    m_widgetTree->SetItemBold(rootNode, true);

    // Independent of looknfeel, therefore only added once.
    if (Filter ("DefaultWindow", aParentWindow))
    {
        (void)m_widgetTree->AppendItem(rootNode, wxT("DefaultWindow"));
    }
    if (Filter ("DefaultGUISheet", aParentWindow))
    {
        (void)m_widgetTree->AppendItem(rootNode, wxT("DefaultGUISheet"));
    }
    if (Filter ("DragContainer", aParentWindow))
    {
        (void)m_widgetTree->AppendItem(rootNode, wxT("DragContainer"));
    }

    // The looks will be nodes; its widgets become leaves. Test each widget for passing the filter.
    CEGUI::WindowFactoryManager::FalagardMappingIterator falIt = CEGUI::WindowFactoryManager::getSingletonPtr()->getFalagardMappingIterator();
	// Look node to open (if set)
	wxTreeItemId looknfeelNode;
    // Build the map of all looks 
    for(; !falIt.isAtEnd() ; ++falIt)
    {
        const CEGUI::String currentFal = falIt.getCurrentKey();
        std::string Fal = currentFal.c_str();
        LogDebug (wxT("Current mapping: ") + StringHelper::ToWXString(currentFal));
        const CEGUI::String::size_type pos = currentFal.find(separator);
        CEGUI::String look(currentFal, 0, pos);
        CEGUI::String widget(currentFal, pos + 1);
        // Put entire block within this statement, to avoid empty look nodes (not likely to happen though).
        if (Filter(widget, aParentWindow))
        {
            lookIter = looks.find(look);
            if (lookIter == looks.end())
            {	// Branch not available yet; add it.
				wxString label = StringHelper::ToWXString(look);
                const wxTreeItemId node = m_widgetTree->AppendItem(rootNode, label);

                // Skin nodes are bold, as opposed to widget type (leaf) nodes that are normal
                m_widgetTree->SetItemBold(node, true);

				// Expand when equal to previous
				if (label == m_previousLooknfeel)
				{
					looknfeelNode = node;
				}

                looks[look] = node;
                //LogDebug (wxT("Adding widget look ") + StringHelper::ToWXString(look));
            }

            // Widgets that require a parent appear as blue items in the widget tree
            if(RequiresParent(widget))
                m_widgetTree->SetItemTextColour(m_widgetTree->AppendItem(looks[look], StringHelper::ToWXString(widget)), wxColour(0,0,255));
            else
                (void)m_widgetTree->AppendItem(looks[look], StringHelper::ToWXString(widget));

            //LogDebug (wxT("Adding widget ") + StringHelper::ToWXString(widget) + wxT(" to look ") + StringHelper::ToWXString(look));
        }
    }

    // Alphabetically sort the children of each look
    for(lookIter = looks.begin(); lookIter != looks.end(); ++lookIter)
	{
        m_widgetTree->SortChildren((*lookIter).second);
	}

    // Always expand the root
    m_widgetTree->Expand(rootNode);

    // Expand the last-used look. If only one look, expand that.
	if (looknfeelNode.IsOk())
	{
		m_widgetTree->Expand(looknfeelNode);
	}
    wxTreeItemId lookItem;
    if (looks.size() == 1)
    {
        lookItem = (*looks.begin()).second;
        m_widgetTree->Expand(lookItem);
    }
    // If only one widget, select it. This is convenient when heavy filtering applies, for example
    // when only a MenuItem may be added.
    cookieType cookie;
    wxTreeItemId firstChild;
    if (lookItem.IsOk() && m_widgetTree->GetChildrenCount(lookItem) == 1)
    {
        firstChild = m_widgetTree->GetFirstChild(lookItem, cookie);
    }
    else if (m_widgetTree->GetChildrenCount(rootNode) == 1)
    {
        firstChild = m_widgetTree->GetFirstChild(rootNode, cookie);
    }
    //
    if (firstChild.IsOk())
    {
        m_widgetTree->SelectItem(firstChild);
    }
}

//------------------------------------------------------------------------
void DialogAddWindow::OnSelchangedWidgetTree(wxTreeEvent& event)
{
    // Validations
    wxASSERT_MSG(m_widgetTree != NULL, wxT("Widget tree invalid or NULL"));
    wxASSERT_MSG(m_okButton != NULL, wxT("OK Button invalid or NULL"));

    const wxTreeItemId node = event.GetItem();
    // If the element has no children, it is a "real" element, and not a node.
    cookieType cookie;
    const wxTreeItemId child = m_widgetTree->GetFirstChild(node, cookie);
    if (!child.IsOk())
    {   // We have reach an end, traverse back and build up the name.
        m_windowType = m_widgetTree->GetItemText(node);
        wxTreeItemId parent = m_widgetTree->GetItemParent(node);
		// Update 'previous' in case user hits OK
		if (parent && parent != m_widgetTree->GetRootItem())
		{
			m_previousLooknfeel = m_widgetTree->GetItemText(parent);
		}
        while (parent && parent != m_widgetTree->GetRootItem())
        {
            (void)m_windowType.insert(0, wxT("/"));
            (void)m_windowType.insert(0, m_widgetTree->GetItemText(parent));
            // Up
            parent = m_widgetTree->GetItemParent(parent);
        }
        //LogDebug (wxT("Selected possible widget type '%s'"),  m_windowType);
    }
    else
    {
        // Reset
        m_windowType.Empty();
    }
    // May only "ok" when a widget (a leave) is selected. (No scheme node).
    (void)m_okButton->Enable(!m_windowType.IsEmpty());
}

//------------------------------------------------------------------------
bool DialogAddWindow::Filter(const CEGUI::String& aWidget, const CEGUI::Window* const aParentWindow) const
{
    // Parent may be NULL
	if (!aParentWindow)
	{	// Allow alot everything since the new window is the first one
		return !RequiresParent (aWidget);
	}
    
	// When the parent is a MenuBar or a PopupMenu, we may only add MenuItems
    if (aParentWindow->testClassName("Menubar")/* || aParentWindow->testClassName("PopupMenu")*/)
    {
        return (aWidget == "MenuItem");
    }
    // We never explicitly add PopupMenus
    // When the parent is a MenuItem, we have two options:
    // 1. When the item does not have a PopupMenu yet, we may only add a PopupMenu
    // 2. When the item already has a PopupMenu, we may only add a MenuItem. This works in combination with
    //  DialogMain::InitLayoutTree (the PopupMenu skipping part) and the EditorDocument::AddNewWindow 
    // (the step-into PopupMenu part).
    if (aParentWindow->testClassName("MenuItem"))
    {
        /*if (!((MenuItem*)aParentWindow)->getPopupMenu())
        {
        return (aWidget == "PopupMenu");
        }*/
        return (aWidget == "MenuItem");
    }
    // When the parent is a TabControl, we may only add Default windows
    if (aParentWindow->testClassName("TabControl"))
    {
        //return (aWidget == "DefaultGUISheet");
        return (aWidget == "DefaultWindow");
    }

    // Are we to filter the displayed widgets by their parent requirement?
    if(m_treeFilter)
        // Last test: only allow widgets who make sense on their own.
        return !RequiresParent (aWidget);
    else
        // No parent filter required here
        return true;
}

//------------------------------------------------------------------------
bool DialogAddWindow::RequiresParent(const CEGUI::String& windowType) const
{
    if (windowType.find("ComboDropList") != CEGUI::String::npos ||
        windowType.find("ComboEditBox") != CEGUI::String::npos ||
        //windowType.find("HorizontalScrollbar") != CEGUI::String::npos ||
        windowType.find("HorizontalScrollbarThumb") != CEGUI::String::npos ||
        windowType.find("LargeVerticalScrollbar") != CEGUI::String::npos ||
        windowType.find("LargeVerticalScrollbarThumb") != CEGUI::String::npos ||
        windowType.find("MenuItem") != CEGUI::String::npos ||
        windowType.find("PopupMenu") != CEGUI::String::npos ||
        windowType.find("SliderThumb") != CEGUI::String::npos ||
        windowType.find("TabButton") != CEGUI::String::npos ||
        windowType.find("TabButtonPane") != CEGUI::String::npos ||
        windowType.find("TabContentPane") != CEGUI::String::npos ||   // Auto window for a TabControl (?)
        windowType.find("Tooltip") != CEGUI::String::npos ||
        //windowType.find("VerticalScrollbar") != CEGUI::String::npos ||
        windowType.find("VerticalScrollbarThumb") != CEGUI::String::npos ||
        windowType.find("VUMeter") != CEGUI::String::npos)
    {
        //LogDebug (wxT("%s requires a parent."), StringHelper::ToWXString(windowType));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
void DialogAddWindow::OnOK( wxCommandEvent& WXUNUSED(event) ) 
{
    // Validations
    wxASSERT_MSG(m_editName != NULL, wxT("Edit name invalid or NULL"));

    // Copy the values from the controls (for some reason doesn't work when closed)
    m_windowName = m_editName->GetValue();
    // We already got the type in the tree handler
    // Pass to superclass
#if wxCHECK_VERSION(2,8,0)
    // new way of returning
    AcceptAndClose();
#else
    wxDialog::OnOK(event);
    // Hide to pass control back to caller
    Show(false);
#endif
}

//------------------------------------------------------------------------
void DialogAddWindow::OnCancel( wxCommandEvent& WXUNUSED(event) ) 
{
#if wxCHECK_VERSION(2,8,0)
    // this is no longer supported
    EndDialog(wxID_CANCEL);
#else
    wxDialog::OnCancel(event);
    Show(false);
#endif
}

//------------------------------------------------------------------------
void DialogAddWindow::OnFilterBoxSel(wxCommandEvent& event) 
{
    // Check type of filter selected
    // NOTE: The comparison is used to avoid the annoying warning C4800 
    //       int' : forcing value to bool 'true' or 'false' (performance warning)
    m_treeFilter = (event.GetInt()!=0); 

    // Rebuild widget tree to take in account the current filter
    InitWidgetTree(m_parentWindow);
}
