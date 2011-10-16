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

#include <wx/filename.h>
#include <wx/colordlg.h>

#include "CELayoutEditor.h"
#include "EditorDocument.h"
#include "DialogMain.h"
#include "SelectionMover.h"
#include "CEGUIDefaultResourceProvider.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define NULL_WINDOW_MSG wxT("Window may not be NULL here!")
//#define DELETE_ROOT_WARNING wxT("You can't delete the 'root' window.")
#define NO_BACKGROUND_WARNING wxT("There is currently no mapping loaded for a StaticImage. Therefore it is not possible to set a background image.")
#define ADD_WINDOW_HELPER 4

//////////////////////////////////////////////////////////////////////////
// EDITORDOCUMENT IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

// wxWidgets document/view stuff
IMPLEMENT_DYNAMIC_CLASS(EditorDocument, wxDocument)

//////////////////////////////////////////////////////////////////////////
// EDITORDOCUMENT FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// EditorDocument construction/destruction
EditorDocument::EditorDocument() :
	m_activeLayout(wx_static_cast(CEGUI::Window*, NULL)),
	m_background(wx_static_cast(CEGUI::Window*, NULL)),

	m_observers(),

	m_hoveredResizePoint(RESIZE_POINT_NONE),
	m_previousHoveredResizePoint(RESIZE_POINT_NONE),
	m_mousePressed(false),

	// Not zero to avoid division exceptions
	m_width(1),
	m_height(1),

	m_mouseX(0),
	m_mouseY(0),
	m_previousMouseX(0),
	m_previousMouseY(0),
	m_hoveredWindow(wx_static_cast(CEGUI::Window*, NULL)),

	m_selection(),
	m_copiedSelection(),

	// Initially, no layout widgets are selected, so we have nothing to lock. 
	//m_lockedSelection(false),
	wxDocument()
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
 
    // Clean up selection + resizers
    m_selection.ClearSelection();

    // Initially, no layout widgets are selected, so we have nothing to lock. 
    wxGetApp().GetMainFrame()->UpdateLockedSelectionIcon(this);
}

//------------------------------------------------------------------------
/** Handles the wxID_OPEN menu event.
* The system will check whether modifications were made and give the option to save the
* current layout first (if any).
*/
bool EditorDocument::OnOpenDocument(const wxString& fullPathName)
{
	Options* const options =  Options::GetInstancePtr();
    wxFileName layoutFile(fullPathName);
	// wx needs the full path
	wxString path = options->GetPathToLayouts();
    wxString fileName = layoutFile.GetName()+ L"." + layoutFile.GetExt();
	wxString fullPath = path + fileName;

    if ( fileName.empty() || !wxDocument::OnOpenDocument( fullPath ) )
        return false;

    // Cleanup any current windows to avoid name clashes
	m_activeLayout = NULL;
    CEGUI::WindowManager::getSingletonPtr()->destroyAllWindows();

    try 
    {
        // Layout will get loaded from the set path (see INI file)
        CEGUI::Window* const layout = CEGUI::WindowManager::getSingleton().loadWindowLayout(StringHelper::ToCEGUIString(fileName));
        SetActiveLayout (layout);
        UpdateWndMap(layout);
        // Tell observers. Disabled because the view is already notified.
        for (size_t i = 0; i < m_observers.size(); ++i) 
        {
            m_observers [i]->LayoutOpened(layout);
        }

        // Store this setting for application reload
        options->SetCurrentLayout(fullPath);

        SetFilename(fullPath, true);
        SetTitle(fullPath); // no path
        return true;
    }
    catch (CEGUI::Exception& guiException) 
    {   
        // Cleanup so far to avoid name clashes for correctly loaded windows
        CEGUI::WindowManager::getSingletonPtr()->destroyAllWindows ();
        
        // Show exception info, composed of basic info (description, location, message), line and 
        // file info (CEGUI exception specific) and VEH info if under Windows (stack trace, exception code & flags)
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("EditorDocument::OnOpenDocument"), wxT("Unexpected Error"), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));

        return false;
    }
}

//------------------------------------------------------------------------
// Handles the wxID_CLOSE menu event.
bool EditorDocument::OnCloseDocument ()
{
    // Reset any selection related stuff
    m_hoveredResizePoint = RESIZE_POINT_NONE;
    m_previousHoveredResizePoint = RESIZE_POINT_NONE;
    m_hoveredWindow = NULL;
    m_selection.ClearSelection();

    // Cleanup current windows to avoid name clashes
    CEGUI::WindowManager::getSingletonPtr()->destroyAllWindows ();

    // Store this setting for application reload
    // COptions::getInstancePtr()->setCurrentLayout("");

    SetFilename(wxT(""), true);
    return true;
}
void EditorDocument::UpdateWndMap(CEGUI::Window *wnd)
{
    mapWndLock.clear();
    AppendWndMap(wnd);
}

void EditorDocument::AppendWndMap(CEGUI::Window *wnd)
{
    mapWndLock[wnd] = false;
    int childCount = wnd->getChildCount();
    if (wnd->getChildCount())
    {
        for (int i=0;i<childCount;i++)
        {
            CEGUI::Window *pChildWnd = wnd->getChildAtIdx(i);
            AppendWndMap(pChildWnd);
        }
    }
}
//------------------------------------------------------------------------
// Handles the wxID_NEW menu event.
bool EditorDocument::OnNewDocument () 
{
    wxDocument::OnNewDocument();
	
	CEGUI::Window* layout = NULL; // Might override with a root

	// Does the user want a fullscreen root?
	wxMessageDialog dlg(NULL, wxT("Do you want to start with a fullscreen root window?"), wxT("Add default root?"),
		wxYES_NO);
	if (dlg.ShowModal() == wxID_YES)
	{	// Start with fullscreen root
		try 
		{    
			layout = CEGUI::WindowManager::getSingletonPtr()->createWindow ("DefaultGUISheet", "Root");
		}
		// If something goes wrong, show user and return false
		SHOW_EXCEPTION_RETURN("EditorDocument::OnNewDocument", "Error", "Unexpected standard exception while adding new document", "Unexpected non-standard exception while adding new document", false);
	}

	SetActiveLayout (layout);

	// Tell observers
	for (size_t i = 0; i < m_observers.size(); ++i) 
	{
		m_observers [i]->LayoutStarted(layout);
	}

    EditorDocument::Modify(true);
    return true;
}

//------------------------------------------------------------------------
CEGUI::Window* EditorDocument::AddWindow (CEGUI::Window* aParentWindow, const CEGUI::String& aType, CEGUI::String aName) 
{
    if( aName.empty() )
    {   // Pick the type without the look.
        aName = aType.substr( aType.find_last_of("/") + 1 );
    }
    // Lookup the given name and keep numbering until unique.
    CEGUI::String newName = aName;
    int nCounter = 1;

    while ( CEGUI::WindowManager::getSingleton().isWindowPresent(newName) )
    {
        wxChar p_chNamePostfix[100];
        (void)wxSnprintf (p_chNamePostfix, 100, wxT("%d"), nCounter++);
        newName = aName + StringHelper::ToCEGUIString(p_chNamePostfix); 
    }

    try 
    {   
        if (!aParentWindow) 
		{
			LogDebug(wxT("EditorDocument::addWindow - create first window: '%s'(%s)."),
				StringHelper::ToWXString(newName).c_str(), StringHelper::ToWXString(aType).c_str());
		}
		else
		{	// Print info about parent and newly to create window.
			LogDebug(wxT("EditorDocument::addWindow - parent: '%s'(%s), new child: '%s'(%s)."),
				StringHelper::ToWXString(aParentWindow->getName()).c_str(), StringHelper::ToWXString(aParentWindow->getType()).c_str(), StringHelper::ToWXString(newName).c_str(), StringHelper::ToWXString(aType).c_str());
		}

		// Create the window
		CEGUI::Window* newWindow = CEGUI::WindowManager::getSingleton().createWindow (aType, aName);
		InitNewWindow(newWindow, aParentWindow);
		
		// When no parent is given, the new window will become the first window (root)
		if (!aParentWindow)
		{
			SetActiveLayout(newWindow);
		}
		else
		{
			aParentWindow->addChildWindow(newWindow);
		}

        // Tell observers
        for (size_t i = 0; i < m_observers.size(); ++i) 
        {
            m_observers [i]->WindowAdded(newWindow);
        }

        // Select it
        SelectWindow (newWindow);

        //设置新增window 锁状态
        AppendWndMap(newWindow);
        EditorDocument::Modify(true);
        return newWindow;
    }
    // If something goes wrong, show user and return NULL
    SHOW_EXCEPTION_RETURN("EditorDocument::AddWindow", "Error", "Unexpected standard exception while adding new window", "Unexpected non-standard exception while adding new window", NULL);
}

//------------------------------------------------------------------------
void EditorDocument::InitNewWindow (CEGUI::Window* aNewWindow, CEGUI::Window* aParentWindow)
{
    // Some initializing. TabControls get default pre-population
    if (aNewWindow->testClassName("TabControl"))
    {
        InitTabControl(aNewWindow);
    }
    else if (aNewWindow->testClassName("Menubar"))
    {
        //InitMenubar(aNewWindow);
		// If we add a MenuBar, we need to position and size it for a bit.
		aNewWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f), CEGUI::UDim(0.12f, 0.0f)));
		aNewWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, 0.0f), CEGUI::UDim(0.1f, 0.0f)));
    }
	// Some things can only be done with a parent (already checked in the AddWindowDlg)
	if (aParentWindow)
	{	// Menus need some specific handling...
		if (aNewWindow->testClassName("MenuItem"))
		{	// Just put some text in the caption
			aNewWindow->setText("MenuItem");
			if (aParentWindow->testClassName("MenuItem"))
			{   // We never explicitly add PopupMenus; when the parent is a MenuItem, we have two options:
				// 1. When the parent item does not have a PopupMenu yet, we automagically add a PopupMenu and the new item
				// 2. When the parent item already has a PopupMenu, we add the new item to that popupmenu
				CEGUI::PopupMenu* popup = NULL;
				// (This cast to MenuItem is safe now)
				if (!(wx_static_cast(CEGUI::MenuItem*, aParentWindow))->getPopupMenu())
				{   // Create popup menu for this item
					const CEGUI::String look = aNewWindow->getLookNFeel().substr(0, aNewWindow->getLookNFeel().find_first_of("/"));
					popup = wx_static_cast(CEGUI::PopupMenu*, CEGUI::WindowManager::getSingleton().createWindow(look + CEGUI::String("/PopupMenu"), aParentWindow->getName() + "/AutoPopup"));
					(wx_static_cast(CEGUI::MenuItem*, aParentWindow))->setPopupMenu(popup);
				}
				else
				{   // Retrieve popup menu for this item
					popup = (wx_static_cast(CEGUI::MenuItem*, aParentWindow))->getPopupMenu();
				}
				// Tricky part: because we put the new window into a helper popup menu, we don't have to add to too
				// the parent as well!
				popup->addChildWindow(aNewWindow);
			}
		}
		// Same for tab pages: we default them to 1,1 (relative)
		// DefaultGUISheet gets classname "DefaultWindow"!
		if (aNewWindow->testClassName("DefaultWindow") && aParentWindow->testClassName("TabControl"))
		{
			aNewWindow->setText("Tab");
			aNewWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, 0.0f), CEGUI::UDim(1.0f, 0.0f)));
		}
		// Except for menuitems (whos size depens on the text), we create a default position and size.
		else if (!aNewWindow->testClassName("MenuItem"))
		{   // Set initial (relative) position in the usual top-left to bottom-right apprauch
			// Make the size a bit fair to the parent size, and wrap around maximum ADD_WINDOW_HELPER
			const float newStartXY = 0.2f + ((aParentWindow->getChildCount() ? aParentWindow->getChildCount() : 1) % ADD_WINDOW_HELPER) * 0.1f;

			// No metrics mode anymore
			aNewWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(newStartXY, 0.0f), CEGUI::UDim(newStartXY, 0.0f)));
			aNewWindow->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f / ADD_WINDOW_HELPER, 0.0f), CEGUI::UDim(1.0f / ADD_WINDOW_HELPER, 0.0f)));
		}
	}
	else
	{	// No parent, set a position
		aNewWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2f, 0.0f), CEGUI::UDim(0.2f, 0.0f)));
		aNewWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0.0f), CEGUI::UDim(0.25f, 0.0f)));
	}
	
	// Apply the default font (if set)
	const wxString defaultFont = Options::GetInstancePtr()->GetDefaultFont();
	if (defaultFont != wxT(""))
	{
		aNewWindow->setProperty("Font", StringHelper::ToCEGUIString(defaultFont));
		if (aNewWindow->isPropertyPresent("TitlebarFont"))
		{
			aNewWindow->setProperty("TitlebarFont", StringHelper::ToCEGUIString(defaultFont));
		}
	}
}

//------------------------------------------------------------------------
void EditorDocument::InitTabControl (CEGUI::Window* aTabControl)
{
    // Validations
    wxASSERT_MSG(aTabControl != NULL, wxT("Supplied tab control is NULL"));

    #define TAB_COUNT 4

    const wxChar* const tabCaptions[TAB_COUNT] = {wxT("Global"), wxT("Team"), wxT("Friends"), wxT("Admin")};
    wxChar tabIndex [10];
    // Simulate a chat window with mutiple channels.
    if (aTabControl->testClassName("TabControl"))
    {
        //LogDebug (wxT("Tabcontrol: '%s'(%s)"), StringHelper::ToWXString(aTabControl->getName()), StringHelper::ToWXString(aTabControl->getType()));
        CEGUI::Window *newTab;
        const CEGUI::String controlName = aTabControl->getName();

        //CEGUI::String look = aTabControl->getLookNFeel().substr(0, aTabControl->getLookNFeel().find("/"));
        for (int i=0; i<TAB_COUNT; i++)
        {
            // Just call our helper method
            (void)wxSprintf(tabIndex, wxT("/Tab %d"), i+1);
			// Create the new tab
			newTab = CEGUI::WindowManager::getSingleton().createWindow ("DefaultWindow", controlName + StringHelper::ToCEGUIString(tabIndex));
			InitNewWindow(newTab, aTabControl);
            newTab->setText(StringHelper::ToCEGUIString(tabCaptions[i]));    // Overwrite default caption.
			aTabControl->addChildWindow(newTab);
        }
    }
}

//------------------------------------------------------------------------
void EditorDocument::InitMenubar (CEGUI::Window* aMenubar)
{
    // Validations
/*    wxASSERT_MSG(aMenubar != NULL, wxT("Supplied menu bar is NULL"));

    #define ITEM_COUNT 17

    const wxChar* const menuCaptions[ITEM_COUNT] = {wxT("_root_"), wxT("File"), wxT("New"), wxT("Open"), wxT("Close"), wxT("Save"), wxT("Exit"),
        wxT("_root_"), wxT("Edit"), wxT("Cut"), wxT("Copy"), wxT("Paste"), wxT("Delete"),
        wxT("_root_"), wxT("Help"), wxT("Contents..."), wxT("About...")};
    wxChar itemIndex [10];
    // Simulate a simple menu
    if (aMenubar->testClassName("Menubar"))
    {
        CEGUI::Window* currentRoot = NULL;
        CEGUI::Window* newItem = NULL;
        const CEGUI::String controlName = aMenubar->getName();
        const CEGUI::String look = aMenubar->getLookNFeel().substr(0, aMenubar->getLookNFeel().find("/"));
        int i=0;
        while (i<ITEM_COUNT)
        {
            (void)wxSprintf(itemIndex, wxT("/Item%d"), i+1);
            if(wxStrcmp(menuCaptions[i], wxT("_root_")) == 0)
            {   
                // New root item
				currentRoot = CEGUI::WindowManager::getSingleton().createWindow (look + CEGUI::String ("/MenuItem"), controlName + StringHelper::ToCEGUIString(itemIndex));
				InitNewWindow(currentRoot, aMenubar);
                aMenubar->addChildWindow(newItem);
				newItem = currentRoot;
                i++;    // Don't use _root_ as captions.
            }
            else
            {   // Add to current root item
                if(currentRoot)
				{
					newItem = CEGUI::WindowManager::getSingleton().createWindow (look + CEGUI::String ("/MenuItem"), currentRoot->getName() + StringHelper::ToCEGUIString(itemIndex));
					InitNewWindow(newItem, currentRoot);
					currentRoot->addChildWindow(newItem);
				}
                else
                {
                    // Something went wrong
                    LogError(wxT("EditorDocument::InitMenubar - Root window not found!"));
                    return;
                }
            }

            // Safety-Check: If a root item is set as the last menu caption (which 
            // would be a mistake), it could lead to an out-of-bounds access.
            if(i<ITEM_COUNT)
                newItem->setText(StringHelper::ToCEGUIString(menuCaptions[i]));
            i++;    // next
        }
    }*/
}

//------------------------------------------------------------------------
bool EditorDocument::RemoveWindow (CEGUI::Window* aWindow, const bool notifyListeners)
{
    // You may not remove the root window, otherwise many other stuff might go wrong ;-)
/*    if (aWindow == m_activeLayout)
    {
        LogWarning (DELETE_ROOT_WARNING);
        // Don't delete or unselect then.
        return false;
    }*/

    // Remove froms selection too (if selected)
    m_selection.Unselect(aWindow);

	// (Allows NULL)
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow (aWindow);

	if (aWindow == m_activeLayout)
	{
		SetActiveLayout(NULL);
	}
	if (notifyListeners)
	{
		// Tell listeners
		for (size_t i = 0; i < m_observers.size(); ++i) 
		{
			m_observers [i]->WindowsRemoved();
		}
	}

    EditorDocument::Modify(true);
    return true;
}

//------------------------------------------------------------------------
CEGUI::String EditorDocument::FindFirstMapping(const CEGUI::String& aType) const
{
    // The looks will be nodes; its widgets become leaves. Test each widget for passing the filter.
    CEGUI::WindowFactoryManager::FalagardMappingIterator falIt = CEGUI::WindowFactoryManager::getSingletonPtr()->getFalagardMappingIterator();
    // Build the map of all looks 
    for(; !falIt.isAtEnd() ; ++falIt)
    {
        if (falIt.getCurrentKey().find(aType) != CEGUI::String::npos)
        {
            return falIt.getCurrentKey();
        }
    }
    return "";
}

//------------------------------------------------------------------------
bool EditorDocument::IsBackgroundAvailable() const
{
    return (m_background != NULL);
}

//------------------------------------------------------------------------
bool EditorDocument::InitBackground()
{
    // First call?
    if (!m_background)
    {
		try 
		{
			// Use the first mapping available
			const CEGUI::String staticImageType = FindFirstMapping("StaticImage");
			if (!staticImageType.empty())
			{	
				LogDebug (wxT("Create background with: ") + StringHelper::ToWXString(staticImageType));
				m_background = CEGUI::WindowManager::getSingletonPtr()->createWindow(staticImageType, BACKGROUND_WIDGET_NAME);
				// Disable frame and standard background
				m_background->setProperty("FrameEnabled", "false");
				m_background->setProperty("BackgroundEnabled", "false");
				// Apply last stored settings
				SetBackgroundImage(Options::GetInstancePtr()->GetCurrentBackground());
			}
			else
			{   // Stub
				m_background = CEGUI::WindowManager::getSingletonPtr()->createWindow("DefaultWindow", BACKGROUND_WIDGET_NAME);
				LogWarning(NO_BACKGROUND_WARNING);
			}
			// Set position and size
			m_background->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
			m_background->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
		}
		// If something goes wrong, show user
		SHOW_EXCEPTION("EditorDocument::InitBackground", "Error", "Unexpected standard exception while creating background widget", "Unexpected non-standard exception while creating background widget");
    }
    return IsBackgroundAvailable();
}

//------------------------------------------------------------------------
void EditorDocument::SetBackgroundImage(const wxString& fullpath)
{   // Only process when a file is given
    if (!fullpath.IsEmpty())
    {   // Remove current imageset. This call just silently bails out when not found.
		CEGUI::ImagesetManager::getSingleton().destroy("BackgroundImage");
		CEGUI::DefaultResourceProvider* const rp = wx_static_cast(CEGUI::DefaultResourceProvider*, CEGUI::System::getSingleton().getResourceProvider());
		const CEGUI::String currentImagesetsDirectory = rp->getResourceGroupDirectory("imagesets");
		try 
        {   	
			// Just to be sure.
            if (InitBackground())
            {   // Okay, behold... because ImageSets get loaded from the predefined resource group,
                // we change it for the moment.
                wxFileName filename(fullpath);
                const wxString path = filename.GetPath() + wxT("/");
                const wxString file = filename.GetFullName();
                rp->setResourceGroupDirectory("imagesets", StringHelper::ToCEGUIString(path));
                (void)CEGUI::ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", StringHelper::ToCEGUIString(file));
                // Set or change the background image ("full_image" is the default name generated by cegui).
                m_background->setProperty("Image", "set:BackgroundImage image:full_image");
                // Store this setting (full path this time, because this method will be called for it)
                Options::GetInstancePtr()->SetCurrentBackground(fullpath);
                // Only use INI setting when the value is equal. Otherwise assume visibility of (newly) set image.
				Options* pOptions = Options::GetInstancePtr();
				if (fullpath == pOptions->GetCurrentBackground())
				{
					SetBackgroundVisibility(pOptions->IsBackgroundVisible());
				}
				else
				{	// Assume True for newly set background filename
					SetBackgroundVisibility(true);
				}
            }
        }
        // If something goes wrong, show user
        SHOW_EXCEPTION("EditorDocument::SetBackgroundImage", "Error", "Unexpected standard exception while setting background image", "Unexpected non-standard exception while setting background image");

        // Restore (also after an exception!)
        rp->setResourceGroupDirectory("imagesets", currentImagesetsDirectory);
    }
}

//------------------------------------------------------------------------
void EditorDocument::SetBackgroundVisibility(bool visible) 
{
    // Validations
    wxASSERT_MSG(m_background != NULL, wxT("Background is empty or invalid!"));

	if (m_background)
	{
		if(visible)
		{
			m_background->show();
		}
		else
		{
			m_background->hide();
		}
	    // Store this setting
		Options::GetInstancePtr()->SetBackgroundVisible(visible);
	}
}

//------------------------------------------------------------------------
void EditorDocument::SetActiveLayout (CEGUI::Window* aLayout) 
{
	// Store & tell Cegui
	m_activeLayout = aLayout;
	(void)CEGUI::System::getSingletonPtr()->setGUISheet (m_activeLayout);

	// Good moment to init the background, if not done yet. Note that the background is not the
	// layout's parent (anymore).
	(void)InitBackground();
}

//------------------------------------------------------------------------
void EditorDocument::DeactivateCurrentLayout () 
{
    if (m_activeLayout) 
    {
        m_activeLayout = NULL;
    }
}

//------------------------------------------------------------------------
void EditorDocument::AddObserver (IDocumentObserver* anObserver) 
{
    m_observers.push_back (anObserver);
}

//------------------------------------------------------------------------
void EditorDocument::Reset () 
{
    m_hoveredResizePoint = RESIZE_POINT_NONE;
    m_previousHoveredResizePoint = RESIZE_POINT_NONE;
    m_hoveredWindow = NULL;
    m_activeLayout = NULL;
    m_background = NULL;
    m_mousePressed = false;
    m_mouseX = 0;
    m_mouseY = 0;
    m_previousMouseX = 0;
    m_previousMouseY = 0;

    // Not zero to avoid devision exceptions
    m_width = 1;
    m_height = 1;

    // Clean up selection + resizers
    m_selection.ClearSelection();

    // Initially, no layout widgets are selected, so we have nothing to lock. 
    //m_lockedSelection = false;
    wxGetApp().GetMainFrame()->UpdateLockedSelectionIcon(this);

    mapWndLock.clear();
}

//------------------------------------------------------------------------
void EditorDocument::HandleMouseMoved (int aMouseX, int aMouseY) 
{
    COMPILE_TODO("view->GetSelection(); or something.")
    SelectionMover mover (&m_selection);

    wxChar statusInfo [100];
    bool bChanges = false;
    float pixelDeltaX, pixelDeltaY ;

    // Copy parameters into members
    m_mouseX = aMouseX ;
    m_mouseY = aMouseY ;

    // Minimum status bar text
    (void)wxSnprintf (statusInfo, 100, wxT("[%d, %d][%3.2f, %3.2f]"), aMouseX, aMouseY, wx_static_cast(double, aMouseX) / m_width, wx_static_cast(double, aMouseY) / m_height);


    // Only proceed when the mouse is pressed
    if (m_mousePressed) 
    {
        // Calculate both value-type, so the code below can choose,
        // based on the metric modes of the elements.
        (void)wxSnprintf (statusInfo, 100, wxT("%s - dragging"), statusInfo) ;

        // Calculate pixel differences with the previous mouse position.
        pixelDeltaX = wx_static_cast(float, m_mouseX - m_previousMouseX);
        pixelDeltaY = wx_static_cast(float, m_mouseY - m_previousMouseY);

        if (m_hoveredResizePoint != RESIZE_POINT_NONE)
        {	
            if(GetSelectWndisLocked() == false)
            {
                // The mouse is pressed on a resize point; resize each selected window accordingly.
                switch (m_hoveredResizePoint) 
                {
                case RESIZE_POINT_WN:
                    // Upper left
                    mover.UpdateWindowAreas(pixelDeltaX, pixelDeltaY, 0.0f, 0.0f);
                    break;
                case RESIZE_POINT_N:
                    // Upper middle
                    mover.UpdateWindowAreas(0.0f, pixelDeltaY, 0.0f, 0.0f);
                    break;
                case RESIZE_POINT_NE:
                    // Upper right
                    mover.UpdateWindowAreas(0.0f, pixelDeltaY, pixelDeltaX, 0.0f);
                    break;
                case RESIZE_POINT_E:
                    // Middle right
                    mover.UpdateWindowAreas(0.0f, 0.0f, pixelDeltaX, 0.0f);
                    break;
                case RESIZE_POINT_ES:
                    // Bottom right
                    mover.UpdateWindowAreas(0.0f, 0.0f, pixelDeltaX, pixelDeltaY);
                    break;
                case RESIZE_POINT_S:
                    // Bottom middle
                    mover.UpdateWindowAreas(0.0f, 0.0f, 0.0f, pixelDeltaY);
                    break;
                case RESIZE_POINT_SW:
                    // Bottom left
                    mover.UpdateWindowAreas(pixelDeltaX, 0.0f, 0.0f, pixelDeltaY);
                    break;
                case RESIZE_POINT_W:
                    // Middle left
                    mover.UpdateWindowAreas(pixelDeltaX, 0.0f, 0.0f, 0.0f);
                    break;
                default:
                    break;
                }
                bChanges = true ;
            }
        }
        else if (m_hoveredWindow)
        {	
            // The mouse is pressed inside a window; drag entire selection.
            // We drag, so the size doesn't change. Therefore we pass the same two parameters for the right bottom values.
            if(isLocked(m_hoveredWindow) == false)
            {
                mover.UpdateWindowAreas(pixelDeltaX, pixelDeltaY, pixelDeltaX, pixelDeltaY);
                bChanges = true;
            }
        }

        if (bChanges) 
        {
            EditorDocument::Modify(true);
        }
    }

    else 
    {	// Mouse not pressed; find hovered resize point or window
        if (m_activeLayout) 
        {
			Selection::Boxes& boxes = m_selection.GetAllBoxes();
            for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
            {        
                m_hoveredResizePoint = boxIt->GetPointAtPosition (CEGUI::Point (wx_static_cast(float, m_mouseX), wx_static_cast(float, m_mouseY)));
                if (m_hoveredResizePoint != RESIZE_POINT_NONE)
                {
                    break;
                }
            }
        
            if (m_hoveredResizePoint == RESIZE_POINT_NONE)
            {
                // Didn't find resize point, try window
                m_hoveredWindow = m_activeLayout->getChildAtPosition(CEGUI::Point(wx_static_cast(float, m_mouseX), wx_static_cast(float, m_mouseY)));
            }

			// Still null? Check if the mouse is in the root window
			if (!m_hoveredWindow && m_activeLayout == CEGUI::System::getSingletonPtr()->getGUISheet())
			{
				CEGUI::Rect pixelRc = m_activeLayout->getOuterRectClipper();
				if (pixelRc.isPointInRect(CEGUI::Point(wx_static_cast(float, m_mouseX), wx_static_cast(float, m_mouseY))))
				{
					m_hoveredWindow = m_activeLayout;
				}
			}
        }
    }

    // Hovering is exclusive; either a resize point or a window. Never both.
    if (m_hoveredResizePoint != RESIZE_POINT_NONE)
    {
        m_hoveredWindow = NULL;
        (void)wxSnprintf (statusInfo, 100, wxT("%s ResizePoint: %d"), statusInfo, m_hoveredResizePoint);
    }
    else if (m_hoveredWindow != NULL) 
    {
        m_hoveredResizePoint = RESIZE_POINT_NONE;
        static CEGUI::String hoverWindowName = m_hoveredWindow->getName();
        if (hoverWindowName != m_hoveredWindow->getName())
        {
            hoverWindowName = m_hoveredWindow->getName();
            UpdateAllViews();
        }
        (void)wxSnprintf (statusInfo, 100, wxT("%s Window: %s"), statusInfo, StringHelper::ToWXString(m_hoveredWindow->getName()).c_str());
        // Selectable?
        if (m_hoveredWindow->getName().find("__auto_") != CEGUI::String::npos || m_hoveredWindow->getName().find("__TabPane__") != CEGUI::String::npos)
        {
            // No, add to status info
            (void)wxSnprintf (statusInfo, 100, wxT("%s (not selectable)"), statusInfo);
        }
    }

    UpdateStatusBar (statusInfo);
    UpdateCursor();

    // Reset previous values
    m_previousMouseX = m_mouseX ;
    m_previousMouseY = m_mouseY ;
}

//------------------------------------------------------------------------
void EditorDocument::HandleMousePressed (const int WXUNUSED(aMouseX), const int WXUNUSED(aMouseY), const bool aMustReset) 
{
    // An overlay must be open
    if (!m_activeLayout) 
    {
        return ;
    }
    if (m_hoveredWindow) 
    {
        LogDebug ( wxT("EditorDocument::HandleMousePressed - '%s'"), StringHelper::ToWXString(m_hoveredWindow->getName()).c_str());
        // Helper methods
        if (!PressedTabButton() && !PressedMenuItem())
        {
            while (m_hoveredWindow->getName().find("__auto_") != CEGUI::String::npos || 
                m_hoveredWindow->getName().find("__TabPane__") != CEGUI::String::npos)
            {   // Current window is an __auto_, goto parent (if any)
                if (m_hoveredWindow->getParent())
                {
                    m_hoveredWindow = m_hoveredWindow->getParent();
                }
            }
        }
        LogDebug (wxT("EditorDocument::HandleMousePressed - pressed: '%s'"), StringHelper::ToWXString(m_hoveredWindow->getName()).c_str());
        // Call helper method
        SelectWindow (m_hoveredWindow, aMustReset);
        UpdateAllViews();
    }
    else
    {
		COMPILE_TODO("Move this to a seperate helper");
        //HandleColoursUpdate(aMustReset);
    }
    // Save this state for during mouse movements
    m_mousePressed = true;
}

//------------------------------------------------------------------------
// tuan.kuranes@free.Fr
bool EditorDocument::PressedTabButton()
{
    // Validations
    wxASSERT_MSG(m_hoveredWindow != NULL, wxT("Hovered window is NULL"));
    
    // Find tab window (parent parent of a tab button)
    // Then find the tab pane corresponding to the button clicked.
    // Hide Other TabPane, show Clicked tabpane.
    const CEGUI::String &name = m_hoveredWindow->getName();    // For a tab button, this becomes "__auto_btn<MyPageName>".

    // Here we test whether a tab button was selected. Selecting a different page happens in the same way
    // as with a run-time tabcontrol.
    const size_t posInString = name.find("__auto_btn");
    if (posInString != CEGUI::String::npos && 
        m_hoveredWindow->getParent() &&     // For a tab button, parent is a <MyTabControlName>__auto_TabPane__Buttons.
        m_hoveredWindow->getParent()->getName().find("__auto_TabPane__Buttons") != CEGUI::String::npos)
    {
        LogDebug ( wxT("You clicked a tab button."));
        m_hoveredWindow = m_hoveredWindow->getParent()->getParent();    // This is <MyTabControlName>
        if (m_hoveredWindow)
        {
            LogDebug (wxT("The tab button is part of: '%s'"), StringHelper::ToWXString(m_hoveredWindow->getName()).c_str());
            const unsigned int hoveredWindowChildCount = m_hoveredWindow->getChildCount();

            CEGUI::Window *newWin = 0;

            // We have "walked up" the TabControl; now find <MyTabControlName>__auto_TabPane__,
            // which contains the content pages which correspond to the buttons.
            for (size_t i = 0; i < hoveredWindowChildCount; i++)
            {
                newWin = m_hoveredWindow->getChildAtIdx(i);
                if (newWin->getName().find("__auto_TabPane__Buttons")!= CEGUI::String::npos)
                    continue;
                if (newWin->getName().find("__auto_TabPane_")!= CEGUI::String::npos)
                {   // Found it!
                    m_hoveredWindow = newWin;
                    break;
                }
            }
            if (m_hoveredWindow == newWin && newWin)
            {   
                // then find the tab pane corresponding to the button clicked.
                LogDebug (wxT("Name: ") + StringHelper::ToWXString(name));
                //const CEGUI::String &typeName = m_hoveredWindow->getType();
                const unsigned int size = m_hoveredWindow->getChildCount();   // Child count is the same as the child count of TabPane__buttons.
                CEGUI::String autoButton ("__auto_btn");
                const size_t endAutoPos = name.rfind(autoButton) + autoButton.length();
                // size_t endAutoPos = autoPos + String("__auto_btn").size();
                
                const CEGUI::String searchedTabName = name.substr(endAutoPos);   // The button name minus "__auto_btn" is the content pane we need.
                const char* const strToFind = searchedTabName.c_str();
                CEGUI::Window *searchedTab = 0;
                
                for (size_t i = 0; i < size; i++)
                {
                    searchedTab = newWin->getChildAtIdx(i);
                    const CEGUI::String &curTabName = searchedTab->getName();
                    if (curTabName.find(strToFind) != CEGUI::String::npos)
                    {   // Found it!
                        m_hoveredWindow = searchedTab;
                        LogDebug ( wxT("Clicked tab: '%s'"), StringHelper::ToWXString(searchedTab->getType()).c_str());
                    }
                    searchedTab->setVisible (false);
                }
                // Others TabPane are now hidden; show Clicked tabpane.
                m_hoveredWindow->setVisible (true);
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------
bool EditorDocument::PressedMenuItem()
{
    // Validations
    wxASSERT_MSG(m_hoveredWindow != NULL, wxT("Hovered Window is NULL"));

    if (m_hoveredWindow->testClassName("MenuItem"))
    {   // When this item is a root item, meaning that it is a direct child of the Menubar, 
        // the Menubar itself takes care of hiding the previous one :-)
        // (The menu item will check for a NULL situation.)
        (wx_static_cast(CEGUI::MenuItem*, m_hoveredWindow))->openPopupMenu();
        return true;
    }        
    return false;
}

//------------------------------------------------------------------------
void EditorDocument::HandleColoursUpdate(const bool shiftPressed) const
{
    // currently selected (CEGUI)Window
    CEGUI::Window* const win = wxGetApp().GetMainFrame()->GetDialogMain()->GetCurrentWindow();
    if (win != NULL && shiftPressed && m_hoveredResizePoint != -1 && win->isPropertyPresent("TextColours"))
    {
        // get the color rect
        CEGUI::ColourRect rect = CEGUI::PropertyHelper::stringToColourRect(win->getProperty("TextColours"));

        wxColourData cd;
        cd.SetChooseFull(true);

        // and add it to the dialog's custom colors
        cd.SetCustomColour(0, wxColour(
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getRed() * 255),
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getGreen() * 255), 
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getBlue() * 255)));
        cd.SetCustomColour(1, wxColour(
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getRed() * 255),
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getGreen() * 255), 
            wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getBlue() * 255)));
        cd.SetCustomColour(2, wxColour(
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getRed() * 255),
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getGreen() * 255), 
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getBlue() * 255)));
        cd.SetCustomColour(3, wxColour(
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getRed() * 255),
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getGreen() * 255), 
            wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getBlue() * 255)));

        // set the default color
        switch (m_hoveredResizePoint) 
        {
        case RESIZE_POINT_WN:
            // Upper left
            cd.SetColour(wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getGreen() * 255), 
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getBlue() * 255)));
            break;
        case RESIZE_POINT_NE:
            // Upper right
            cd.SetColour(wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getGreen() * 255), 
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_right.getBlue() * 255)));
            break;
        case RESIZE_POINT_ES:
            // Bottom right
            cd.SetColour(wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getGreen() * 255), 
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_right.getBlue() * 255)));
            break;
        case RESIZE_POINT_SW:
            // Bottom left
            cd.SetColour(wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getGreen() * 255), 
                wx_static_cast(wxColourBase::ChannelType, rect.d_bottom_left.getBlue() * 255)));
            break;
        default:
            // default we get from the Upper left of the rect
            cd.SetColour(wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getGreen() * 255), 
                wx_static_cast(wxColourBase::ChannelType, rect.d_top_left.getBlue() * 255)));
            break;
        }
        wxColourDialog dlg(wxGetApp().GetMainFrame(), &cd);
        // show the color dialog
        if (dlg.ShowModal() != wxID_OK)
            return;

        wxColour &clr = dlg.GetColourData().GetColour();
        // and convert the data
        CEGUI::colour cl;
        cl.setRed(clr.Red() / 255.0f);
        cl.setGreen(clr.Green() / 255.0f);
        cl.setBlue(clr.Blue() / 255.0f);

        // set up the alpha, then switch the color
        switch (m_hoveredResizePoint) 
        {
        case RESIZE_POINT_WN:
            // Upper left
            cl.setAlpha(rect.d_top_left.getAlpha());
            rect.d_top_left = cl;
            break;
        case RESIZE_POINT_NE:
            // Upper right
            cl.setAlpha(rect.d_top_right.getAlpha());
            rect.d_top_right = cl;
            break;
        case RESIZE_POINT_ES:
            // Bottom right
            cl.setAlpha(rect.d_bottom_right.getAlpha());
            rect.d_bottom_right = cl;
            break;
        case RESIZE_POINT_SW:
            // Bottom left
            cl.setAlpha(rect.d_bottom_left.getAlpha());
            rect.d_bottom_left = cl;
            break;
        default:
            rect = cl;
            break;
        }
        // finally we apply the whole color rect, and refresh
        win->setProperty("TextColours", CEGUI::PropertyHelper::colourRectToString(rect));
        wxGetApp().GetMainFrame()->Refresh();
    }
}

//------------------------------------------------------------------------
void EditorDocument::HandleMouseReleased () 
{
    const bool mustNotify = (m_hoveredResizePoint != RESIZE_POINT_NONE || m_hoveredWindow);
    m_mousePressed = false ;
    
    if(GetSelectWndisLocked())
        return;
    m_hoveredResizePoint = RESIZE_POINT_NONE;
    m_hoveredWindow = NULL ;
    if (mustNotify)
    {
        for (size_t i = 0; i < m_observers.size(); ++i) 
        {
            m_observers [i]->SelectionDraggedOrResized();
        }
        UpdateAllViews();
    }
}

//------------------------------------------------------------------------
void EditorDocument::SelectWindow (CEGUI::Window* aWindow, const bool aMustReset)
{
    // Is the current selection locked? If so, we can't make changes.
  //  if(m_lockedSelection)
  //  {
  //      // Verify if there is anything currently selected and if so ignore the operation (locked). 
  //      // Otherwise, remove the lock.
  //      if(m_selection.Size() == 0)
  //      {
  //          // The user unselected the last selection element (either by a cut, unselect or delete
  //          // operation). We need to remove the lock and update the toolbar lock icon to its
  //          // unlock state
  //          m_lockedSelection = false;
  //          wxGetApp().GetMainFrame()->UpdateLockedSelectionIcon(this);
  //      }
  //      else
		//{	// The current selection is locked, so we return immediately
  //          return;
		//}
  //  }
    
    // Validations
	if (aWindow == NULL)
	{
		LogError(wxT("Cannot select a NULL Window. Please contact support."));
		return;
	}

	// Simulate common selection behaviour:
	//  When something is selected, and LCtrl is not pressed, undo the entire current selection
	if (aMustReset)
	{
		m_selection.ClearSelection();
	}

    // Either the new window or the first window (when the given window was unselected)
    CEGUI::Window* newSelectedWindow = NULL;

    // Add (first) window.
    if (m_selection.Select (aWindow))
    {          
        newSelectedWindow = aWindow;
        // Feedback. Mantis ticket #34
        UpdateStatusBar (wxT("Selected a '%s'"), StringHelper::ToWXString(aWindow->getType()).c_str());
        // Lock?
        if (RequiresLock(aWindow))
        {	// Block changing of position and size.
            m_selection.FindBoxFor(aWindow)->Lock();
        }

    }
    else
    {	// We unselected the window. Use the first selection for the notification.
        if (m_selection.Size() > 0)
        {	// Ptr to window inside the box
            newSelectedWindow = m_selection.GetAllBoxes().begin()->GetWindow();
        }
        // else stays NULL
        UpdateStatusBar (wxT("Unselected '%s'"), StringHelper::ToWXString(aWindow->getType()).c_str());
    }
    wxGetApp().GetMainFrame()->UpdateLockedSelectionIcon(this);
    ////如果被锁定了 则返回
    //if(isLocked(aWindow))
    //    return;

    // Tell observers about the selection
    for (size_t i = 0; i < m_observers.size(); ++i) 
    {
        m_observers [i]->WindowSelected (newSelectedWindow);
    }


    // Do we have a valid new selection?
    // NOTE: This can be null if the user unselected the top/last window
    if(newSelectedWindow)
    {
        LogDebug (wxT("EditorDocument::SelectWindow - Newselected: '%s'"), StringHelper::ToWXString(newSelectedWindow->getName()).c_str());

        // tuan.kuranes@free.fr
        // if selecting from dialog tree
        // an invisible item, put it back to visible
        if (!newSelectedWindow->isVisible ())
        {
            CEGUI::Window *currWindow = newSelectedWindow;
            while (currWindow && !currWindow->isVisible())
            {          
                // find tab window (parent of current window)
                if (currWindow->getParent() && 
					currWindow->getParent()->getName().find("__auto_TabPane__") != CEGUI::String::npos)
                {
                    // hide brother tab
                    unsigned int size = currWindow->getParent()->getChildCount();
                    for (size_t parentChild = 0; parentChild < size; parentChild++)
                    {
                        currWindow->getParent()->getChildAtIdx(parentChild)->setVisible(false);
                    }

                    // show all tab children 
                    size = currWindow->getChildCount();
                    for (size_t child = 0; child < size; child++)
                    {
                        currWindow->getChildAtIdx(child)->setVisible(true);
                    }              
                }
                currWindow->setVisible(true);
                currWindow = currWindow->getParent();              
            } // while
        } // if 
    } // if

	m_selection.Update();
    wxDocument::UpdateAllViews();
}

//------------------------------------------------------------------------
void EditorDocument::SelectAll()
{
	// Not when locked
    //if(!m_lockedSelection)
	//{	
		// [quote by Van] "Don't get smarter then your users" [/quote]
		// So a Select All is a select all, including the root sheet and such.
		// (Which actually makes sence in case the user want to move everythin into a fresh layout)
		CEGUI::Window* root = CEGUI::System::getSingletonPtr()->getGUISheet();
		if (root)
		{
			m_selection.SelectAll(root);
		}

		EditorDocument::Modify(true);
	//}
} 

//------------------------------------------------------------------------
bool EditorDocument::RequiresLock (const CEGUI::Window* const aWindow) const
{
    // Validations
    wxASSERT_MSG(aWindow != NULL, wxT("Supplied window is NULL"));

    //LogDebug ( wxT("MenuItem: %d"), (int) aWindow->testClassName("MenuItem"));
    //LogDebug ( wxT("DefaultWindow: %d"), (int) aWindow->testClassName("DefaultWindow"));
    //LogDebug ( wxT("ParentType: %s"), StringHelper::ToWXString(aWindow->getParent()->getType()));
    return (aWindow->testClassName("MenuItem") ||
        // Little hacky to the tabcontrol
        (aWindow->testClassName("DefaultWindow") && aWindow->getParent() && aWindow->getParent()->getType().find("TabContentPane") != CEGUI::String::npos));
}

//------------------------------------------------------------------------
void EditorDocument::UpdateStatusBar (const wxChar *format, ...) const
{
    wxChar        buffer[1024] ;

    // Initialize variable arguments list
    va_list arguments;
    va_start(arguments, format);
    // Create the status bar text
    (void)wxVsnprintf (buffer, sizeof(buffer), format, arguments) ;
    // Cleanup arguments.
    va_end(arguments);

    wxGetApp().GetMainFrame()->GetStatusBar()->SetStatusText(buffer);
}

//------------------------------------------------------------------------
void EditorDocument::UpdateCursor() 
{
    wxWindow* const frame = wxGetApp().GetMainFrame();
    if (m_hoveredResizePoint != m_previousHoveredResizePoint)
    {
        //LogDebug(wxT("Updating cursor."));
        switch (m_hoveredResizePoint) 
        {
        case RESIZE_POINT_WN:
            // Upper left
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case RESIZE_POINT_N:
            // Upper middle
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case RESIZE_POINT_NE:
            // Upper right
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case RESIZE_POINT_E:
            // Middle right
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case RESIZE_POINT_ES:
            // Bottom right
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case RESIZE_POINT_S:
            // Bottom middle
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case RESIZE_POINT_SW:
            // Bottom left
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case RESIZE_POINT_W:
            // Middle left
            (void)frame->SetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        default:
            // Default cursor
            (void)frame->SetCursor(wxCursor(wxCURSOR_ARROW));
            break;
        }
    }
    m_previousHoveredResizePoint = m_hoveredResizePoint;
}

//-----------------------------------------------------------------------
void EditorDocument::GetRelevantProperties(RelevantProperties* commonProperties, RelevantProperties* nonCommonProperties, const bool onlyCommon) 
{
    // Validations
    wxASSERT_MSG(commonProperties != NULL, wxT("Supplied common properties are NULL"));
    wxASSERT_MSG(nonCommonProperties != NULL, wxT("Supplied non common properties are NULL"));

    // Make sure no properties exist initially (safety-check)
    commonProperties->clear();
    nonCommonProperties->clear();

    // Obtain the first window in the current selection
    Selection::Boxes::iterator boxIt = m_selection.GetAllBoxes().begin();
    const CEGUI::Window *currentWindow = (*boxIt).GetWindow();

    // No window exists, so the user hasn't selected anything (shouldn't really happen)
    if(!currentWindow)
        return;

    try
    {
        // Iterate through all the properties of the first window in order to mark them
        // as common properties for starters
        // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and 
        // PropertySet (it would if we used getIterator()) thus avoiding useless casts
        CEGUI::PropertySet::Iterator propertyIterator = currentWindow->getPropertyIterator();
        for(; !propertyIterator.isAtEnd() ; ++propertyIterator)
        {
            // Obtain current property name and store it
            // NOTE: We store each property in the common properties map initially because we start
            // by assuming that every property present in the first element in the selection
            // will be common to EVERY other element in the selection. Of course, if we find this assumption
            // to be erroneous later on, we delete it from the common map and optionally move the 
            // affected property to the non common properties map (if onlyCommon parameter 
            // is set to false). 
            // This method also guarantees that single selection is treated in the same way as 
            // multi-selection since every property in a single selection will be common (only one 
            // widget selected!)
            const CEGUI::String propertyName = propertyIterator.getCurrentKey();
            (void)commonProperties->insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));
        }

        // Scan the rest of the selection (if it exists)
        for (; boxIt != m_selection.GetAllBoxes().end(); ++boxIt)
        {
            // Obtain current widget in the selection
            currentWindow = (*boxIt).GetWindow();

            // NOTE: Must be done before the other iteration
            // Are we to process only the common properties in the current selection?
            if(!onlyCommon)
            {
                // No, so we need to add all the new properties of the current window
                // that are not present in the first window to the non common property map      
                // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and 
                // PropertySet (it would if we used getIterator()) thus avoiding useless casts
                propertyIterator = currentWindow->getPropertyIterator();
                for(; !propertyIterator.isAtEnd() ; ++propertyIterator)
                {
                    // Obtain current property name
                    const CEGUI::String propertyName = propertyIterator.getCurrentKey();

                    // If the property isn't present in the common property map, we add
                    // it to the non-common property map since there is at least one 
                    // window (the first one) that doesn't contain it
                    const RelevantPropertiesIterator propIter = commonProperties->find(propertyName);
                    if (propIter == commonProperties->end())
                        (void)nonCommonProperties->insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));
                }

                // The other task that remains for us to do is to check the non 
                // common property map and for every property whose value is not
                // common between all elements in the selection, mark it as empty.
                // NOTE: If the property isn't present in a window, it isn't
                // considered. This effectively means that if we have 3 windows
                // selected, and the first and third one contain property X with
                // value Y, and the second one doesn't contain it, the displayed
                // value with still be Y. If the third window had value Z instead,
                // the displayed value would be empty
                for(RelevantPropertiesIterator iter = nonCommonProperties->begin(); iter != nonCommonProperties->end();)
                {
                    // Obtain non-common property name
                    const CEGUI::String propertyName = (*iter).first;

                    // Does this property exist in this window? If not, ignore and move to
                    // the next one
                    if(currentWindow->isPropertyPresent(propertyName))
                    {
                        // Property exists, so we need to make sure its value (stored in the non-common
                        // property map) is the same as the one assigned to the current window
                        const CEGUI::String propertyValue = (*iter).second;
                        const CEGUI::String currentWindowPropertyValue = currentWindow->getProperty(propertyName);
                        if(propertyValue != currentWindowPropertyValue)
                            // Values are different, so set property as empty
                            (*iter).second = "";
                    }

                    // Move on to the next property
                    ++iter;
                }
            }

            // We now need to iterate through the common properties of the first window in the selection 
            // stored above in order to perform two different actions:
            // 1) Every property in the common property map that is not present in the current
            // window is no longer common and must be removed from the common map. If we
            // are processing all properties (onlyCommon parameter is set to false), we also
            // need to move it to the non common map before deleting it.
            // 2) For properties that are present but whose value is not common (i.e. not exactly
            // the same in all windows that contain it), we need to assign them as empty.
            for(RelevantPropertiesIterator iter = commonProperties->begin(); iter != commonProperties->end();)
            {
                // Obtain property name
                const CEGUI::String propertyName = (*iter).first;

                // Is the property present in the current window?
                if(!currentWindow->isPropertyPresent(propertyName))
                {
                    // If we are processing all properties, we need to add the current property
                    // to the non-common properties map before deleting it from the common 
                    //properties one
                    if(!onlyCommon)
                        (void)nonCommonProperties->insert(std::make_pair(propertyName, (*iter).second));

                    // Property is not common to ALL elements in the selection, so delete it
                    // from the common property map
                    // NOTE: The erase() function in associative containers doesn't invalidate
                    // any iterators except those that point to elements being erased (that's
                    // also true for 'std::list' and 'std::set' but not 'std::vector'). For 
                    // this reason, we don't really need map::erase() to return an iterator
                    (void)commonProperties->erase(iter++);
                }
                else
                {
                    // Property exists, so we need to make sure its value (stored in the common
                    // property map) is the same as the one assigned to the current window
                    const CEGUI::String propertyValue = (*iter).second;
                    const CEGUI::String currentWindowPropertyValue = currentWindow->getProperty(propertyName);
                    if(propertyValue != currentWindowPropertyValue)
                         // Values are different, so set property as empty
                        (*iter).second = "";

                    // Move on to next property
                    ++iter;
                }
            }
        }
    }
    // Ouch, something went wrong
    SHOW_EXCEPTION("EditorDocument::GetRelevantProperties", "Error", "Unexpected standard exception while getting relevant properties", "Unexpected non-standard exception while getting relevant properties");
}

//------------------------------------------------------------------------
bool EditorDocument::SetProperty(const CEGUI::String& aName, const CEGUI::String& newValue, const bool allowEmpty) 
{
    bool bChanges = false ;

    // This check is added because the main dialog does quite some calls with empty values,
    // for example when launching the app (initializing the editboxes).
    if (newValue == "" && !allowEmpty)
    {
        return true;
    }

    Selection::Boxes& boxes = m_selection.GetAllBoxes();
    for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {
        // Does this window have the given property?
        if (boxIt->GetWindow()->isPropertyPresent(aName)) 
        {
            // Yes, is it different? (this check is done to avoid a SetModified when a window is only selected).
            if (boxIt->GetWindow()->getProperty(aName) != newValue)
            {
                // Yes, set the new value
                LogDebug (wxT("'%s' set to '%s'"),StringHelper::ToWXString(aName).c_str(), StringHelper::ToWXString(newValue).c_str());
                boxIt->GetWindow()->setProperty(aName, newValue);
                boxIt->GetWindow()->invalidate(true);
                // In case it was a position- or size value.
                boxIt->Reset();
                bChanges = true ;
            }
        }
    }
    if (bChanges) 
    {  
        EditorDocument::Modify(true);
    }

    return bChanges;
}

//------------------------------------------------------------------------
bool EditorDocument::OnSaveDocument (const wxString& filePath)
{
    // Validations
    wxASSERT_MSG(m_activeLayout != NULL, wxT("Active layout is NULL and can't be saved!"));

    // Pre-save
    OnPreSave();

    std::ofstream file ;
    file.open (wxConvLibc.cWX2MB(filePath), std::ios::out) ;
    if (!file.is_open ()) 
    {
        LogError (wxT("EditorDocument::OnSaveDocument - Error: could not open file for saving. Is it read-only?")) ;
        return false;
    }

    try 
    {
        // Save it, now using CEGUI itself to do it! :-)
        CEGUI::WindowManager::getSingletonPtr()->writeWindowLayoutToStream(*m_activeLayout, file);
		// Store this setting for application reload
		wxString fileName = wxFileName(filePath).GetFullName();
        Options::GetInstancePtr()->SetCurrentLayout(fileName);
    }
    // If something goes wrong, show user
    SHOW_EXCEPTION("EditorDocument::OnSaveDocument", "Error", "Unexpected standard exception while saving document", "Unexpected non-standard exception while saving document");

    file.close () ;

    // Tell the document it's "clean" again
    EditorDocument::Modify(false);
    return true;
}

//------------------------------------------------------------------------
void EditorDocument::OnPreSave() const
{
    const CEGUI::WindowManager* const winMgr = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::WindowManager::WindowIterator windowIt = winMgr->getIterator();
    while (!windowIt.isAtEnd()) 
    {
        CEGUI::Window* const window = windowIt.getCurrentValue();
        //COMPILE_HACK("This is a hack to overcome the problem that CEGUI writes the __auto_* windows. Needs to be patched into CEGUI itself, but after RC2")
        if (window->getName().find("__auto_") != CEGUI::String::npos)
        {   
            // COMPILE_HACK("A hack within a hack; a so-called 'nested hack'. Otherwise ScrollablePane does not save its children.")
            if (window->getName().find("__auto_container__") == CEGUI::String::npos)
            {
                window->setWritingXMLAllowed(false);   // __auto_*
                LogDebug (wxT("Surpress writing of: '%s'(%s)"), StringHelper::ToWXString(window->getName()).c_str(), StringHelper::ToWXString(window->getType()).c_str());
            }
        }
        
        windowIt++;
    }
}

//------------------------------------------------------------------------
void EditorDocument::SaveFromWindow(const CEGUI::Window* const aWindow, const wxString& fileName) const
{
    // Validations
    wxASSERT_MSG(aWindow != NULL, wxT("Supplied window is NULL"));

    std::ofstream file ;
    file.open (wxConvLibc.cWX2MB(fileName), std::ios::out) ;
    if (!file.is_open ()) 
    {
        LogError (wxT("EditorDocument::saveFromWindow - Error: could not open file for saving. Is it read-only?")) ;
    }

    // Save it, now using CEGUI itself to do it! :-)
    CEGUI::WindowManager::getSingletonPtr()->writeWindowLayoutToStream(*aWindow, file);

    file.close ();
}

//------------------------------------------------------------------------
void EditorDocument::CutSelection()
{
    // Same as copy
    CopySelection();

    // Remove the selection
    DeleteSelection();
}

//------------------------------------------------------------------------
void EditorDocument::CopySelection()
{
    // Undo previous result.
    m_copiedSelection.Reset();

    // Use the moveables as a base for our recursion
    Selection::Boxes& boxes = m_selection.GetMoveableBoxes();
    for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
    {
        m_copiedSelection.Copy(boxIt->GetWindow());
    }
}

//------------------------------------------------------------------------
void EditorDocument::PasteCopiedSelection()
{
    // Paste the selection on the first selected window (if any) or the layout root.
    if (m_selection.Size()) 
    {
        m_copiedSelection.Paste(m_selection.GetAllBoxes().begin()->GetWindow());
    }
    else 
    {
        m_copiedSelection.Paste(m_activeLayout);
    }
	// Common selection behaviour: after a paste, nothing is selected anymore
    m_selection.ClearSelection();
    // Tell observers
    for (size_t i = 0; i < m_observers.size(); ++i) 
    {
        m_observers [i]->WindowAdded(NULL);
    }
}

//------------------------------------------------------------------------
void EditorDocument::DeleteSelection()
{
    // Don't use an iterator loop, since the removeWindow method may unselect windows
    // and thereby invalidate the iteration(!)
    while (m_selection.GetMoveableBoxes().size() > 0)
    {
        // Delete the top-most windows. Only notify on the last one.
        if (!RemoveWindow(m_selection.GetMoveableBoxes().begin()->GetWindow(), m_selection.GetMoveableBoxes().size() == 1))
        {
            break;
        }
    }

	// Don't select anything by default anymore
    /*if (m_activeLayout)
    {
        SelectWindow(m_activeLayout);
    }*/
}

//------------------------------------------------------------------------
void EditorDocument::Modify(bool modify)
{
    wxDocument::Modify(modify);
    wxDocument::UpdateAllViews();
}

//获取当前窗口是否处于锁定状态
bool EditorDocument::isLocked(CEGUI::Window *wnd) 
{
    windowLock::iterator iter = mapWndLock.find(wnd);
    if(iter != mapWndLock.end())
    {
        return iter->second;
    }
    
    return false;
}


//切换当前window锁定状态
void EditorDocument::SwitchWindowLock(CEGUI::Window *wnd)
{
    windowLock::iterator iter = mapWndLock.find(wnd);
    if(iter != mapWndLock.end())
    {
        bool curValue = iter->second;
        mapWndLock[wnd] = !curValue;
        for (size_t i = 0; i < m_observers.size(); ++i) 
        {
            ((DialogMain*)m_observers [i])->UpdateTreeImage(wnd);
        }
    }
    else
        assert(0);
}

const bool EditorDocument::GetSelectWndisLocked() const
{
    EditorDocument * pDocument = const_cast<EditorDocument*>(this);
    CEGUI::Window *pSelectWnd = NULL;
    if(pDocument->GetSelection()->GetMoveableBoxes().size())
    {
        pSelectWnd = pDocument->GetSelection()->GetMoveableBoxes().begin()->GetWindow();
    }
    if(pSelectWnd)
        return const_cast<EditorDocument*>(this)->isLocked(pSelectWnd);
    else
        return false;
}

void EditorDocument::SwitchLockedSelection()
{
    long boxSize = m_selection.GetAllBoxes().size();
    if (boxSize)
    {
        Selection::Boxes::iterator iter = m_selection.GetAllBoxes().begin();
        for (;iter != m_selection.GetAllBoxes().end(); ++iter)
        {
            CEGUI::Window *pSelectWnd = iter->GetWindow();
            SwitchWindowLock(pSelectWnd);
            iter->Lock(!iter->IsLocked());
        }
    }
}