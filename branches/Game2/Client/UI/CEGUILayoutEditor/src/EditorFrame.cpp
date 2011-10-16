//////////////////////////////////////////////////////////////////////////////
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
#include "DialogMain.h"
#include "DialogAlignCtrl.h"
#include "DialogGrid.h"
#include "CEGUIDefaultResourceProvider.h"
#include "Config.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/choicdlg.h>

// Toolbar icons
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/delete.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/unlock.xpm"
#include "bitmaps/lock.xpm"

#include "FalProgressBarEx.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define ASSERT_DOCUMENT_MSG wxT("The m_document member is NULL!")

//////////////////////////////////////////////////////////////////////////
// EDITORFRAME IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(EditorFrame, wxDocParentFrame)

//////////////////////////////////////////////////////////////////////////
// EDITORFRAME EVENT TABLE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
BEGIN_EVENT_TABLE(EditorFrame, wxDocParentFrame)

EVT_SIZE(EditorFrame::OnResize)

EVT_MENU(ID_OPEN_CUSTOM, EditorFrame::OnOpenCustom)

// Dialog options
EVT_MENU(ID_VIEW_SHOW_HIDE_DIALOG, EditorFrame::OnShowOrHideDialog)
// Resizers
EVT_MENU(ID_VIEW_640x480, EditorFrame::On640x480)
EVT_MENU(ID_VIEW_800x600, EditorFrame::On800x600)
EVT_MENU(ID_VIEW_1024x786, EditorFrame::On1024x768)
EVT_MENU(ID_VIEW_1280x800, EditorFrame::On1280x800)
//
EVT_MENU(ID_VIEW_FULLSCREEN, EditorFrame::OnFullScreen)
//
EVT_UPDATE_UI(wxID_CUT, EditorFrame::OnUpdateEditCut)
EVT_MENU(wxID_CUT, EditorFrame::OnEditCut)
//
EVT_UPDATE_UI(wxID_COPY, EditorFrame::OnUpdateEditCopy)
EVT_MENU(wxID_COPY, EditorFrame::OnEditCopy)
//
EVT_UPDATE_UI(wxID_PASTE, EditorFrame::OnUpdateEditPaste)
EVT_MENU(wxID_PASTE, EditorFrame::OnEditPaste)
//
EVT_UPDATE_UI(wxID_DELETE, EditorFrame::OnUpdateEditDelete)
EVT_MENU(wxID_DELETE, EditorFrame::OnEditDelete)
//
EVT_UPDATE_UI(wxID_SELECTALL, EditorFrame::OnUpdateSelectAll) 
EVT_MENU(wxID_SELECTALL, EditorFrame::OnSelectAll)
//
EVT_UPDATE_UI(ID_VIEW_SET_BACKGROUND, EditorFrame::OnUpdateSetBackground)
EVT_MENU(ID_VIEW_SET_BACKGROUND, EditorFrame::OnSetBackground)
//
EVT_UPDATE_UI(ID_VIEW_SHOW_BACKGROUND, EditorFrame::OnUpdateShowBackground)
EVT_MENU(ID_VIEW_SHOW_BACKGROUND, EditorFrame::OnShowBackground)
EVT_MENU(ID_VIEW_SET_GRID, EditorFrame::OnSetGrid)
EVT_MENU(ID_VIEW_SET_FONT, EditorFrame::OnSetFont)
// Enables/disables entire alignment submenu
EVT_UPDATE_UI(ID_ALIGN, EditorFrame::OnUpdateAlign)
EVT_MENU(ID_ALIGN_LEFT, EditorFrame::OnAlignLeft)
EVT_MENU(ID_ALIGN_RIGHT, EditorFrame::OnAlignRight)
EVT_MENU(ID_ALIGN_TOP, EditorFrame::OnAlignTop)
EVT_MENU(ID_ALIGN_BOTTOM, EditorFrame::OnAlignBottom)
EVT_MENU(ID_ALIGN_WIDTH, EditorFrame::OnAlignWidth)
EVT_MENU(ID_ALIGN_HEIGHT, EditorFrame::OnAlignHeight)
/*2010.9.3 nikai */
EVT_MENU(ID_ALIGN_PADDING, EditorFrame::OnAlignPadding)

EVT_MENU(ID_SKINS, EditorFrame::OnLayoutSkin)
EVT_UPDATE_UI(ID_SKINS, EditorFrame::OnUpdateLayoutSkin)
EVT_UPDATE_UI(ID_LOCK_SELECTION, EditorFrame::OnUpdateLockSelection)
EVT_MENU(ID_LOCK_SELECTION, EditorFrame::OnLockSelection)

END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// EDITORFRAME FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
EditorFrame::EditorFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, const long type):
m_GUISystem(wx_static_cast(CEGUI::System*, NULL)),
m_GUIRenderer(wx_static_cast(CEGUI::OpenGLRenderer*, NULL)),
m_dialogMain(wx_static_cast(DialogMain*, NULL)),
m_renderCanvas(wx_static_cast(EditorCanvas*, NULL)),
m_document(wx_static_cast(EditorDocument*, NULL)),
mEditMenu(wx_static_cast(wxMenu*, NULL)),
wxDocParentFrame(manager, frame, id, title, pos, size, type)
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
EditorFrame::~EditorFrame()
{
    // Swallow any exceptions found to prevent the destructor from emitting them.
    // NOTE: An exception should never leave a destructor as it may yield
    // undefined behavior or even lead to program abnormal termination 
    // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
    try
    {
        // Windows exception handling specific cleanup
        // NOTE: We need to process this here and not in the CELayoutEditor destructor, because
        // by then the Logger singleton no longer exists and it will not be possible to log
        // this info.
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        // Verify if there's any "eaten" (caught but not processed in a try/catch block) 
        // exception to log
        // NOTE: Please note that "eaten" exceptions might only manifest themselves much later
        // than when they first appeared since they are only detected when a second exception
        // is thrown (or at program shutdown - which is the case here).
#if CHECK_EATEN_EXCEPTIONS
        if(!ExceptionManager::GetInstancePtr()->IsExceptionProcessed())
            // NOTE: Always use LogSilentError here, because we're exiting the application (perhaps even in an unstable state) and there's no need to bother the user
            LogSilentError(wxT("An \"eaten\" exception (caught but not processed/logged) was found.\n\n\"EATEN\" EXCEPTION INFO:%s"), ExceptionManager::GetInstancePtr()->GetExceptionVEHInfo());
#endif // CHECK_EATEN_EXCEPTIONS

        // Unregister VEH exception handler
        ExceptionManager::GetInstancePtr()->UnRegisterExceptionHandler();
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

        // Free member pointers
        if(m_dialogMain)
        {
            (void)m_dialogMain->Destroy();
            delete m_dialogMain;
        }
        //释放自定义渲染器
        CEGUI::WindowRendererFactory *pFactory = CEGUI::WindowRendererManager::getSingleton().getFactory("Falagard/ProgressBarEx");
        delete pFactory;
        if( m_GUISystem != NULL)
        {
            m_GUISystem->destroy();
            m_GUISystem = NULL;
            if(m_GUIRenderer != NULL)
            {
                CEGUI::OpenGLRenderer::destroy(*m_GUIRenderer);
                m_GUIRenderer = NULL;
            }
        }
        if(m_renderCanvas)
            delete m_renderCanvas;

        // Zero out the member pointers
        // NOTE: All of these will be freed internally by wxWidgets
        m_document = wx_static_cast(EditorDocument*, NULL);
        mEditMenu = wx_static_cast(wxMenu*, NULL);
    }
    // Add exception to log
    LOG_EXCEPTION_SAFE("EditorFrame::~EditorFrame", "Failed to destroy editor frame", "Failed to destroy editor frame", "Failed to destroy editor frame");    
}

//------------------------------------------------------------------------
void EditorFrame::Setup()
{
    // Should be enough to fix this bug: http://www.cegui.org.uk/mantis/view.php?id=80
    wxWindowBase::SetSizeHints(320, 200);

    // Add a menu bar, a toolbar and a statusbar
    AttachMenubar();
    AttachToolbar();
    (void)wxFrameBase::CreateStatusBar();

    // Don't do earlier, because we need the toolbar and statusbar in the calculations!
    RestoreIniSize();

    if (m_dialogMain == NULL)
    {
        // Create the main dialog
        // The dialog registers itself to document event so all important
        // changes will automatically be propagated to it.
        m_dialogMain = new DialogMain(this);
    }
}

//------------------------------------------------------------------------
void EditorFrame::AttachMenubar()
{
    // Make a menubar (note that the Edit menu is a member!)
    wxMenu* const fileMenu = new wxMenu;
    mEditMenu = new wxMenu;
    wxMenu* const alignSubmenu = new wxMenu;
    wxMenu* const viewMenu = new wxMenu;
    wxMenu* const helpMenu = new wxMenu;
    wxMenuBar* const menubar = new wxMenuBar;

    // File items

    // Many of these ID's (the ones which start with "wxID") are already mapped
    // internally, so you won't see them in the EVENT_TABLE.
    (void)fileMenu->Append(wxID_NEW, wxT("&New...\tCtrl+N"), wxT("Create a new document."));
    (void)fileMenu->Append(ID_OPEN_CUSTOM, wxT("&Open...\tCtrl+O"), wxT("Open an existing document."));
    (void)fileMenu->Append(wxID_CLOSE, wxT("&Close"), wxT("Close the active document."));
    (void)fileMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"), wxT("Save the active document."));
    (void)fileMenu->Append(wxID_SAVEAS, wxT("Save &As..."), wxT("Save the active document with a new name."));
    (void)fileMenu->AppendSeparator();
    (void)fileMenu->Append(wxID_EXIT, wxT("E&xit\tCtrl+Q"), wxT("Quit the application; prompts to save document."));

    // Edit items
    (void)mEditMenu->Append(wxID_UNDO, wxT("&Undo\tCtrl+Z"), wxT("Undo the last action."));
    (void)mEditMenu->Append(wxID_REDO, wxT("&Redo\tCtrl+Y"), wxT("Redo the previously undone action."));
    (void)mEditMenu->AppendSeparator();
    (void)mEditMenu->Append(wxID_CUT, wxT("&Cut\tCtrl+X"), wxT("Cut the current selection."));
    (void)mEditMenu->Append(wxID_COPY, wxT("&Copy\tCtrl+C"), wxT("Copy the current selection."));
    (void)mEditMenu->Append(wxID_PASTE, wxT("&Paste\tCtrl+V"), wxT("Paste the latest cut or copied selection."));
    (void)mEditMenu->Append(wxID_DELETE, wxT("&Delete\tDel"), wxT("Delete the current selection."));
    (void)mEditMenu->AppendSeparator();
    (void)mEditMenu->Append(wxID_SELECTALL, wxT("Select &All \tCtrl+A"), wxT("Select all windows in the layout."));

    (void)mEditMenu->AppendSeparator();

    // Align items
    // NOTE: No need to free the custodial pointer alignSubmenu since
    // it will be handled internally by wxWidgets wxMenuBase::Append()
    (void)alignSubmenu->Append(ID_ALIGN_LEFT, wxT("&Left"), wxT("Give entire selection the same X1 value."));
    (void)alignSubmenu->Append(ID_ALIGN_RIGHT, wxT("&Right"), wxT("Give entire selection the same X2 value."));
    (void)alignSubmenu->Append(ID_ALIGN_TOP, wxT("&Top"), wxT("Give entire selection the same Y1 value."));
    (void)alignSubmenu->Append(ID_ALIGN_BOTTOM, wxT("&Bottom"), wxT("Give entire selection the same Y2 value."));
    (void)alignSubmenu->AppendSeparator();
    (void)alignSubmenu->Append(ID_ALIGN_WIDTH, wxT("&Width"), wxT("Give entire selection the same width."));
    (void)alignSubmenu->Append(ID_ALIGN_HEIGHT, wxT("&Height"), wxT("Give entire selection the same height."));
    (void)alignSubmenu->Append(ID_ALIGN_PADDING, wxT("&Align Space Between"), wxT("Give entire selection the same Space Between."));
    (void)mEditMenu->Append(ID_ALIGN, wxT("Align"), alignSubmenu);

    (void)mEditMenu->Append(ID_SKINS, wxT("Apply Layout Skin..."), wxT("Applies a skin to the entire layout."));

    // Dialog options
    (void)viewMenu->Append(ID_VIEW_SHOW_HIDE_DIALOG, wxT("Toggle dialog"), wxT("Toggles the main dialog through minimize and restore."));
    (void)viewMenu->AppendSeparator();
    // View items
    (void)viewMenu->Append(ID_VIEW_640x480, wxT("Make exactly 640 x 480"), wxT("Convenience resizer for 640 x 480."));
    (void)viewMenu->Append(ID_VIEW_800x600, wxT("Make exactly 800 x 600"), wxT("Convenience resizer for 800 x 600."));
    (void)viewMenu->Append(ID_VIEW_1024x786, wxT("Make exactly 1024 x 768"), wxT("Convenience resizer for 1024 x 768."));
    (void)viewMenu->Append(ID_VIEW_1280x800, wxT("Make exactly 1280 x 800"), wxT("Convenience resizer for 1280 x 800 (wide)."));
    (void)viewMenu->Append(ID_VIEW_FULLSCREEN, wxT("Make fullscreen"), wxT("Convenience resizer to fullscreen."));
    (void)viewMenu->AppendSeparator();
    (void)viewMenu->Append(ID_VIEW_SET_BACKGROUND, wxT("Set background..."));
    (void)viewMenu->AppendCheckItem(ID_VIEW_SHOW_BACKGROUND, wxT("Show background..."));
    (void)viewMenu->AppendSeparator();
    (void)viewMenu->Append(ID_VIEW_SET_GRID, wxT("Set &grid...\tCtrl+G"), wxT("Set grid size and visibility."));
    (void)viewMenu->Append(ID_VIEW_SET_FONT, wxT("Set default &font...\tCtrl+F"), wxT("Set default font from now on."));

    // Help items
    (void)helpMenu->Append(wxID_ABOUT, wxT("&About"), wxT("Display program information, version number and copyright."));

    // Fill the menubar
    // NOTE: No need to free the custodial pointers fileMenu, viewMenu 
    // and helpMenu since they will be handled internally by wxWidgets wxMenuBar::Append()
    (void)menubar->Append(fileMenu, wxT("&File"));
    (void)menubar->Append(mEditMenu, wxT("&Edit"));
    (void)menubar->Append(viewMenu, wxT("&View"));
    (void)menubar->Append(helpMenu, wxT("&Help"));

    // Associate the menu bar with the frame
    // NOTE: No need to free the custodial pointer menubar since
    // it will be handled internally by wxWidgets wxFrameBase::SetMenuBar()
    wxFrameBase::SetMenuBar(menubar);
}

//------------------------------------------------------------------------
void EditorFrame::AttachToolbar()
{
    // Create toolbar with icons-only
    wxToolBar* const toolBar = CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER, wxID_ANY);
    //
    (void)toolBar->AddTool(wxID_NEW, wxBitmap (new_xpm), wxT("New")); 
    (void)toolBar->AddTool(ID_OPEN_CUSTOM, wxBitmap (open_xpm), wxT("Open"));
    (void)toolBar->AddTool(wxID_SAVE, wxBitmap (save_xpm), wxT("Save"));
    (void)toolBar->AddSeparator();
    (void)toolBar->AddTool(wxID_CUT, wxBitmap (cut_xpm), wxT("Cut"));
    (void)toolBar->AddTool(wxID_COPY, wxBitmap (copy_xpm), wxT("Copy"));
    (void)toolBar->AddTool(wxID_PASTE, wxBitmap (paste_xpm), wxT("Paste"));
    (void)toolBar->AddTool(wxID_DELETE, wxBitmap (delete_xpm), wxT("Delete"));
    (void)toolBar->AddSeparator();

    (void)toolBar->AddTool(ID_LOCK_SELECTION, wxBitmap (lock_xpm), wxT("Lock Selection"));
    (void)toolBar->AddSeparator();

    (void)toolBar->AddTool(wxID_ABOUT, wxBitmap (help_xpm), wxT("About"));
    // Don't forget this one!
    (void)toolBar->Realize();

    // Assign toolbar
    // NOTE: No need to free the custodial pointer toolbar since
    // it will be handled internally by wxWidgets wxFrameBase::SetToolBar()
    wxFrameBase::SetToolBar(toolBar);    
}

//------------------------------------------------------------------------
void EditorFrame::SetDocument(EditorDocument* document)
{
    m_document = document;

    // Always set the (new) document
    if (m_dialogMain)
    {
        m_dialogMain->SetDocument (document);
    }

    // Tell the document's command processor where it can find the Undo and Redo items to update
    // (enable/disable) accordingly.
    if (m_document)
    {
        m_document->GetCommandProcessor()->SetEditMenu(mEditMenu);
    }
}

//------------------------------------------------------------------------
EditorCanvas *EditorFrame::AttachCanvas(wxView *view)
{
    int width, height;
    GetClientSize(&width, &height);

    // Create the GLCanvas. This approach provides MDI support maybe in the future
    //EditorCanvas* const canvas = new EditorCanvas(view, this, GetClientAreaOrigin(), wxDefaultSize);
    EditorCanvas* const canvas = new EditorCanvas(view, this, wxPoint(0,0), wxDefaultSize);

    // Tell it about the initial size
    //canvas->changeSize(width + BORDER_SIZE, height + BORDER_SIZE);
    canvas->changeSize(width, height);
    canvas->ClearBackground();

    // Create the CEGUI renderable
    InitializeCEGUI();

    // Copy into member
    m_renderCanvas = canvas;

    return canvas;
}

//------------------------------------------------------------------------
wxString EditorFrame::GetCEGUIDataRoot()
{
    Options* const opt =  Options::GetInstancePtr();
    wxString appResourceDir(wxStandardPaths::Get().GetResourcesDir());

    if (!appResourceDir.empty())
        return appResourceDir;
    // are data directories are specified in the INI config file?
    wxString anIniPath(opt->GetPathToLookNFeels()); // Just pick one folder to check this.
    if (!anIniPath.empty())
        return wxString();

    // see if there is a 'datafiles' directory in the resources location
    if (wxDir::Exists(appResourceDir + wxT("/datafiles")))
        return appResourceDir + wxT("/datafiles");

    // Can't establish where the files might be; ask the user instead
    // Start at the resources location
    const wxString defaultPath(appResourceDir);
    // Don't allow creating of a new folder.
    wxDirDialog dialog(this, wxT("Select your \"Datafiles\" folder once."),
        defaultPath, (wxDEFAULT_DIALOG_STYLE & 
        ~wxDD_NEW_DIR_BUTTON));

    if (dialog.ShowModal() == wxID_OK)
        return dialog.GetPath();
    else
        return wxString();
}

//------------------------------------------------------------------------
void EditorFrame::InitializeCEGUI()
{
    // Setup support for TGA,JPG and PNG
    CEGUI::System::setDefaultImageCodecName("SILLYImageCodec");
    //CEGUI::OpenGLRenderer::setDefaultImageCodecName("TGAImageCodec");

    m_GUIRenderer = &CEGUI::OpenGLRenderer::create(CEGUI::OpenGLRenderer::TTT_AUTO);
    if(m_GUIRenderer)
        m_GUISystem	= &CEGUI::System::create(*m_GUIRenderer);

    //CEGUI::Logger::getSingleton().setLoggingLevel (CEGUI::Informative);
    InitializePaths();

    // Loads schemes and fonts (no other resources yet).
    LoadData();
}

//------------------------------------------------------------------------
void EditorFrame::InitializePaths()
{
    // Default to internal defaults
    CEGUI::Font::setDefaultResourceGroup("configs");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");

    CEGUI::DefaultResourceProvider* const rp = 
        wx_static_cast(CEGUI::DefaultResourceProvider*, CEGUI::System::getSingleton().getResourceProvider());

    // Get the settings
    Options* const opt =  Options::GetInstancePtr();

    wxString dataRoot(GetCEGUIDataRoot());
    if (!dataRoot.empty())
    {
        CEGUI::Logger::getSingletonPtr()->logEvent(StringHelper::ToCEGUIString(wxT("Set \"Datafiles\" folder to \"") + dataRoot + wxT("\".")));
        opt->SetPathToConfigs(dataRoot + wxT("/datafiles") + wxT("/configs/"));
        opt->SetPathToFonts(dataRoot + wxT("/datafiles") + wxT("/fonts/"));
        opt->SetPathToImagesets(dataRoot + wxT("/datafiles") + wxT("/imagesets/"));
        opt->SetPathToLookNFeels(dataRoot + wxT("/datafiles") +wxT("/looknfeel/"));
        opt->SetPathToScripts(dataRoot + wxT("/datafiles") + wxT("/lua_scripts/"));
        opt->SetPathToSchemes(dataRoot  + wxT("/datafiles") + wxT("/schemes/"));
        opt->SetPathToLayouts(dataRoot  + wxT("/datafiles") + wxT("/layouts/"));
    }		

    // Paths set (also used in case user canceled out the dialog).
    if (opt->GetPathToLookNFeels().empty())	// Just pick one folder to check this.
    {
        LogError (wxT("EditorFrame::InitializeCEGUI - Need to quit: you either canceled the Datafiles browser, or have an incorrect INI file.\nCheck your CELayoutEditor.ini file for the 'XXXPaths=' fields."));
        (void)Close(true);    // No "veto"
    }
    // Calling these methods means that you can load resources without specifying the full path to them. CEGUI
    // will look for the resources in the given paths.
	rp->setResourceGroupDirectory("layouts", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToLayouts())));
    rp->setResourceGroupDirectory("schemes", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToSchemes())));
    rp->setResourceGroupDirectory("imagesets", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToImagesets())));
    rp->setResourceGroupDirectory("fonts", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToFonts())));
    rp->setResourceGroupDirectory("looknfeel", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToLookNFeels())));
    rp->setResourceGroupDirectory("lua_scripts", wx_static_cast(const char*, wxConvLibc.cWX2MB(opt->GetPathToScripts())));
}

//------------------------------------------------------------------------
bool EditorFrame::GetFileNames(const wxString& path, const wxString& wildcard, wxArrayString& filenames) const
{
	filenames.clear();
	if (!path.IsEmpty())
	{	// Find all layouts
		wxDir::GetAllFiles(path, &filenames, wildcard, wxDIR_FILES);
	}
	return filenames.GetCount() > 0;
}

//------------------------------------------------------------------------
void EditorFrame::LoadData() const
{
    wxArrayString	filenames;
    size_t			index;
    Options* const opt =  Options::GetInstancePtr();

	// Load all ".scheme" files.
	wxString path = opt->GetPathToSchemes();
	if (GetFileNames(path, wxT("*.scheme"), filenames))
	{
		for (index=0; index<filenames.GetCount(); index++)
		{
			try
			{
				// Dalfy: i added the resource folder to avoid defaulting.
                CEGUI::String schemeName = StringHelper::ToCEGUIString(wxFileName(filenames[index]).GetFullName());
				(void)CEGUI::SchemeManager::getSingleton().create(schemeName);
			}
			catch (const CEGUI::AlreadyExistsException& guiException)
			{
				// This exception always seems to occur at the moment (for every datafile structure processing), 
				// so we're explicitly not showing it to the user (as it gets very annoying). However, we still want
				// to log the exception info composed of log basic info (description, location, message), line and 
				// file info (CEGUI exception specific) and VEH info if under Windows (stack trace, exception code & flags)
				LogDebug(ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("EditorFrame::LoadData"), wxT("Unexpected Error"), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));
			}
			// If something goes wrong, show user
			SHOW_EXCEPTION("EditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
		}
	}
	else
    {
         LogWarning(wxT("Warning: no schemes were found. Loading of layouts will most likely fail now."));
    }
    //addby nikai  2010.10.22  自定义进度条渲染器
    CEGUI::WindowRendererManager::addFactory<CEGUI::TplWindowRendererFactory<CEGUI::FalagardProgressBarEx> >();

    // Load all ".font" files (may have already been includes in a scheme file)
	path = opt->GetPathToFonts();
	if (GetFileNames(path, wxT("*.font"), filenames))
	{
		for (index=0; index<filenames.GetCount(); index++)
		{
			try
			{
				// Dalfy: i added the resource folder to avoid defaulting.
				(void)CEGUI::FontManager::getSingleton().create(StringHelper::ToCEGUIString(wxFileName(filenames[index]).GetFullName()));
			}
			catch (const CEGUI::AlreadyExistsException&)
			{	// Obvious when already included
			}
			// If something goes wrong, show user
			SHOW_EXCEPTION("EditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
		}
	}
	else
    {
        LogWarning(wxT("Warning: no fonts were found. Loading of layouts will most likely fail now."));
    }

	// Load all ".imageset" files (may have already been includes in a scheme file)
	path = opt->GetPathToImagesets();
	if (GetFileNames(path, wxT("*.imageset"), filenames))
	{
		for (index=0; index<filenames.GetCount(); index++)
		{
			try
			{
				// Dalfy: i added the resource folder to avoid defaulting.
				(void)CEGUI::ImagesetManager::getSingleton().create(StringHelper::ToCEGUIString(wxFileName(filenames[index]).GetFullName()));
			}
			catch (const CEGUI::AlreadyExistsException&)
			{	// Obvious when already included
			}
			// If something goes wrong, show user
			SHOW_EXCEPTION("EditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
		}
	}
	else
    {
        LogWarning(wxT("Warning: no imagesets were found. Loading of layouts will most likely fail now."));
    }

	// Load all ".looknfeel" files (may have already been includes in a scheme file)
	path = opt->GetPathToLookNFeels();
	if (GetFileNames(path, wxT("*.looknfeel"), filenames))
	{
		for (index=0; index<filenames.GetCount(); index++)
		{
			try
			{
				// Dalfy: i added the resource folder to avoid defaulting.
				(void)CEGUI::WidgetLookManager::getSingleton().parseLookNFeelSpecification(StringHelper::ToCEGUIString(wxFileName(filenames[index]).GetFullName()));
			}
			catch (const CEGUI::AlreadyExistsException&)
			{	// Obvious when already included
			}
			// If something goes wrong, show user
			SHOW_EXCEPTION("EditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
		}
	}
	else
    {
        LogWarning(wxT("Warning: no imagesets were found. Loading of layouts will most likely fail now."));
    }
}

//------------------------------------------------------------------------
void EditorFrame::OnResize(wxSizeEvent& WXUNUSED(event)) 
{
    // We need the client size
    int clientWidth, clientHeight;
    GetClientSize(&clientWidth, &clientHeight);

    if (m_document)
    {   // Recalculate the resize boxes
		m_document->GetSelection()->Update();
    }

    if (m_renderCanvas)
    {
        // Apply settings to our GLCanvase
        m_renderCanvas->changeSize(clientWidth, clientHeight);
    }

    if (m_GUIRenderer)
    {
        // Apply to renderer and store in INI
        m_GUIRenderer->setDisplaySize (CEGUI::Size(wx_static_cast(float, clientWidth) , wx_static_cast(float, clientHeight)));
        Options::GetInstancePtr()->SetCurrentResolution(clientWidth, clientHeight);
    }
}

//------------------------------------------------------------------------
void EditorFrame::RestoreIniSize() 
{
    // Load the previously stored resolution
    int lastWidth;
    int lastHeight;
    Options::GetInstancePtr()->GetCurrentResolution(&lastWidth, &lastHeight);

    // The OnResize handler will apply the setting to the renderer and INI file
    SetClientSize(lastWidth, lastHeight);
}

//------------------------------------------------------------------------
void EditorFrame::Refresh(bool eraseBackground, const wxRect* rect)
{
    if (m_document)
    {   // Recalculate the resize boxes
		m_document->GetSelection()->Update();
    }
    if (m_renderCanvas)
    {
        m_renderCanvas->Refresh(eraseBackground, rect);
        m_renderCanvas->Update();
    }
    // Call superclass
    wxFrame::Refresh(eraseBackground, rect);
}

//------------------------------------------------------------------------
void EditorFrame::OnOpenCustom(wxCommandEvent& event)
{
    wxArrayString	filenames;
	wxArrayString	choices;
    size_t			index;

	// Call helper
	Options* const opt =  Options::GetInstancePtr();
	wxString path = opt->GetPathToLayouts();
	// We accept .layout and .xml
	if (GetFileNames(path, wxT("*.layout"), filenames))
    {
		for (index=0; index<filenames.GetCount(); index++)
		{
			choices.Add(wxFileName(filenames[index]).GetFullName());
		}
	}
	if (GetFileNames(path, wxT("*.xml"), filenames))
    {
		for (index=0; index<filenames.GetCount(); index++)
		{
			choices.Add(wxFileName(filenames[index]).GetFullName());
		}
	}
	if (choices.GetCount() > 0)
	{	// Sort, because we appended from two arrays
		choices.Sort();
		wxSingleChoiceDialog chooser(this, wxT("Select from: ") + path, wxT("Please select a layout"), choices);
		if (chooser.ShowModal() == wxID_OK)
		{
			// Cegui knows where to find layouts, so the filename is enough
			wxGetApp().GetDocManager()->CreateDocument(chooser.GetStringSelection(), wxDOC_SILENT);
		}
	}	
	else
    {
		LogWarning(wxT("Warning: no layouts are found in: ") + path);
    }
}

//------------------------------------------------------------------------
void EditorFrame::OnShowOrHideDialog(wxCommandEvent& WXUNUSED(event)) 
{
    if (m_dialogMain)
	{	// Toggle by minimizing or maximizing the dialog
		m_dialogMain->Iconize(!m_dialogMain->IsIconized());
	}
}

//------------------------------------------------------------------------
void EditorFrame::On640x480(wxCommandEvent& WXUNUSED(event)) 
{
    Resize(640, 480);
}

//------------------------------------------------------------------------
void EditorFrame::On800x600(wxCommandEvent& WXUNUSED(event)) 
{
    Resize(800, 600);
}

//------------------------------------------------------------------------
void EditorFrame::On1024x768(wxCommandEvent& WXUNUSED(event))
{
    Resize(1024, 786);
}

//------------------------------------------------------------------------
void EditorFrame::On1280x800(wxCommandEvent& WXUNUSED(event))
{
    Resize(1280, 800); // Widescreen
}

void EditorFrame::Resize(int aWidth, int aHeight)
{
	// The OnResize handler will apply the setting to the renderer and INI file
	SetClientSize(aWidth, aHeight);
    EditorFrame::Refresh();
}

//------------------------------------------------------------------------
void EditorFrame::OnFullScreen(wxCommandEvent& WXUNUSED(event))
{
    (void)wxFrame::ShowFullScreen(true);
    EditorFrame::Refresh();
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateEditCut(wxUpdateUIEvent& event) 
{
    event.Enable (m_document && m_document->GetSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnEditCut(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    m_document->CutSelection();
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateEditCopy(wxUpdateUIEvent& event) 
{
    event.Enable (m_document && m_document->GetSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnEditCopy(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    m_document->CopySelection();
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateEditPaste(wxUpdateUIEvent& event) 
{
    event.Enable (m_document && m_document->GetCopiedSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnEditPaste(wxCommandEvent& WXUNUSED(event)) 
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    m_document->PasteCopiedSelection();
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateEditDelete(wxUpdateUIEvent& event) 
{	
    event.Enable (m_document && m_document->GetSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnEditDelete(wxCommandEvent& WXUNUSED(event)) 
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    m_document->DeleteSelection() ;
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateSetBackground(wxUpdateUIEvent& event) 
{
    // Must have a document
    event.Enable (m_document != NULL && m_document->IsBackgroundAvailable()) ;
}

//------------------------------------------------------------------------
void EditorFrame::OnSetBackground(wxCommandEvent& WXUNUSED(event)) 
{
    // Validations
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

    // Restore directory before returning otherwise all file-access
    // will happen in this folder, including saving of layouts and such.
    const wxString currentDirectory = wxGetCwd();
    // Make sure to only include extensions, which are supported by CEGUI!
    wxFileDialog dialog(this, wxT("Open a background image"), currentDirectory, wxT(""), wxT("Layout files (*.tga;*.jpg;*.png)|*.tga;*.jpg;*.png"), wxOPEN);
    if (dialog.ShowModal() == wxID_OK)
    {
        m_document->SetBackgroundImage(dialog.GetPath());
    }
    (void)wxSetWorkingDirectory(currentDirectory);
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateShowBackground(wxUpdateUIEvent& event) 
{
    // You don't need to have an open document in order to show or hide backgrounds
    //event.Enable (m_renderCanvas && m_renderCanvas->m_background.isVisible()) ;
    event.Check (m_renderCanvas && Options::GetInstancePtr()->IsBackgroundVisible());
    event.Enable (Options::GetInstancePtr()->GetCurrentBackground() != wxT("")) ;
}

//------------------------------------------------------------------------
void EditorFrame::OnShowBackground(wxCommandEvent& event) 
{
    // Validations
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

    m_document->SetBackgroundVisibility(event.IsChecked());
}

//------------------------------------------------------------------------
void EditorFrame::OnSetGrid(wxCommandEvent& WXUNUSED(event))
{
    // Prompts the user with the grid dialog
    // We don't need the return value, since the dialog updates the Options itself
    (void)(new DialogGrid (this))->ShowModal();
    EditorFrame::Refresh();
}

//------------------------------------------------------------------------
void EditorFrame::OnSetFont(wxCommandEvent& WXUNUSED(event))
{
    // Prompts the user with the available fonts to choose one from.
    wxArrayString	fonts;	// Duplicate code from DialogMain, needs refactoring(?)

    CEGUI::FontManager::FontIterator fontIt = CEGUI::FontManager::getSingletonPtr()->getIterator();
    while (!fontIt.isAtEnd())
    {
        (void)fonts.Add (StringHelper::ToWXString(fontIt.getCurrentKey()));
        ++fontIt;
    }

    // Found any?
    if (fonts.GetCount() > 0)
    {
        wxSingleChoiceDialog fontChooser (this, wxT("Apply this font to new windows:"), wxT("Set default font"), fonts);
        if (fontChooser.ShowModal() == wxID_OK)
        {
            // Overwrite previous value
            Options::GetInstancePtr()->SetDefaultFont(fontChooser.GetStringSelection());
        }
    }
    else
    {
        LogWarning (wxT("No fonts available to choose from."));
    }
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateAlign(wxUpdateUIEvent& event) 
{
    event.Enable (m_document && m_document->GetSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignLeft(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignLeft);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignRight(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignRight);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignTop(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignTop);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignBottom(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignBottom);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignWidth(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignWidth);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnAlignHeight(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignHeight);
    m_document->Modify(true); 
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateSelectAll(wxUpdateUIEvent& event) 
{
    event.Enable (m_document != NULL);
}

//------------------------------------------------------------------------
void EditorFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
    m_document->SelectAll();
} 

//------------------------------------------------------------------------
void EditorFrame::OnLayoutSkin(wxCommandEvent& WXUNUSED(event))
{
    // Validations
    wxASSERT_MSG(m_dialogMain != NULL, wxT("No layout is currently loaded!"));

	// Create a dialog that lists the registered Look'N'Feels and that asks the user to choose
    // one of the options. 
    const wxString skin = wxGetSingleChoice(StringHelper::ToWXString("Choose skin that you want to apply to entire layout:"), StringHelper::ToWXString("Skin chooser"), m_dialogMain->GetAvailableLooks());

    // Apply chosen skin to ALL the widgets in the current layout
    if(skin && skin.Length() > 0)
        m_dialogMain->SetWidgetsLook(StringHelper::ToCEGUIString(skin));
} 

//------------------------------------------------------------------------
void EditorFrame::OnUpdateLayoutSkin(wxUpdateUIEvent& event) 
{
    // Only enable the update layout skin when a document is open
    event.Enable (m_document != NULL);
}

//------------------------------------------------------------------------
void EditorFrame::OnUpdateLockSelection(wxUpdateUIEvent& event) 
{
    // Only enable the lock/unlock selection icon when the user has a valid widget selection
    event.Enable (m_document && m_document->GetSelection()->Size() > 0);
}

//------------------------------------------------------------------------
void EditorFrame::OnLockSelection(wxCommandEvent& WXUNUSED(event))
{
    // Validations
    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

    // Change the current lock selection status and update the lock/unlock 
    // toolbar icon to reflect this
    m_document->SwitchLockedSelection();
    UpdateLockedSelectionIcon(m_document);
}

//------------------------------------------------------------------------
void EditorFrame::UpdateLockedSelectionIcon(const EditorDocument* const document) const
{
    // Do we have a valid document to analyze?
    if(!document)
        return;

    // Obtain toolbar
    wxToolBar* const toolBar = this->GetToolBar();

    // Is the current user selection locked?
    if(document->GetSelectWndisLocked())//document->GetLockedSelection())
    {
        // Selection locked, so change to locked icon
        toolBar->SetToolNormalBitmap(ID_LOCK_SELECTION, wxBitmap (lock_xpm));
        toolBar->SetToolShortHelp(ID_LOCK_SELECTION, wxT("Unlock Selection"));
    }
    else
    {
        // Selection unlocked, so change to unlocked icon
        toolBar->SetToolNormalBitmap(ID_LOCK_SELECTION, wxBitmap (unlock_xpm));
        toolBar->SetToolShortHelp(ID_LOCK_SELECTION, wxT("Lock Selection"));
    }
}

void EditorFrame::OnAlignPadding(wxCommandEvent& event)
{
    m_dialogAlign = new DialogAlignCtrl(this);

    m_dialogAlign->SetDocument(m_document);
    // Present dialog to user (modal)
    m_dialogAlign->ShowModal ();
    
    (void)m_dialogAlign->Destroy();
    delete m_dialogAlign;
}
