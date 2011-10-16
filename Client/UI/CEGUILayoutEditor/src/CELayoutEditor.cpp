// Doxygen lines:
//

/*! \mainpage Welcome to the CELayoutEditor, a layour editor for the CEGUI library. 
* The CEGUI library can be found here: http://www.cegui.org.uk/
*
* This product is written with wxWidgets: http://www.wxwidgets.org/
* The layout editor's current version requires wxWidgets 2.8.* or higher.
*
* For latest information about installation and such, please refer to the wiki documentation:
* http://www.cegui.org.uk/wiki/index.php/The_%22official%22_layout_editor
*
* @author Patrick Kooman (Scriptkid) - Main application
* @author Sam Jansen (http://bung.homeunix.net/bung/) - Alignment class
* @author Paul D Turner (CrazyEddie) - Linux makefiles and Unicode arrangements
* @author Olivier Delannoy (Dalfy) - Migration to 0.5 support
* @author "Xirnohn" - patch including tooltip support and color setup for widgets
* @author "Tuan Kuranes" - patch including tabcontrol support
* @author Frederico Jeronimo (fjeronimo) - Major patches; looknfeel switch, selection (un)locking, 
* exception handling, lint, build checks, "About" box, bug fixes and so on. For details see:
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2766
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2783
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2796
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2811
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2821
* http://www.cegui.org.uk/phpBB2/viewtopic.php?t=2825
* 
* Copyright (C) The CEGUI team. 
*/

////////////////////////////////////////////////////////////////////////////////
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

#include <gtest.h>
#include "pch.h"

#include "BuildChecks.h"

#include "CELayoutEditor.h"
#include "EditorFrame.h"
#include "EditorDocument.h"
#include "EditorView.h"
#include "DialogMain.h"
#include "DialogAbout.h"

#include "CEGUIDefaultLogger.h"

#include "wx/filename.h"
#include "wx/ffile.h"
#include "wx/splash.h"
#include "wx/aboutdlg.h"
#include "wx/stdpaths.h"

#include "bitmaps/icon32.xpm"
#include "bitmaps/icon48.xpm"

//////////////////////////////////////////////////////////////////////////
// MESSAGE BOXES
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
int CELEMessageBox(const wxString& text, const wxString& title, long style)
{
#if USE_MODAL_MESSAGE_BOXES
    return ModalMessageBox(text, title, style, true);
#else
    return SafeMessageBox(text, title, style);
#endif // USE_MODAL_MESSAGE_BOXES
}

//------------------------------------------------------------------------
int SafeMessageBox(const wxString& text, const wxString& title, long style)
{
    // Check OS
    #ifdef __WINDOWS__
        // In windows we display a native message box which is safe to call
        // even if the application crashed
        return ::MessageBox(NULL, text, title, wx_static_cast(UINT, style));
    #else
        // We're not in Windows, so print message to stderr 
        wxFprintf(stderr, _T("%s: %s\n"), title.c_str(), text.c_str());
        fflush(stderr);
        return CELE_MB_RESULT_CANCEL;
    #endif
}

//------------------------------------------------------------------------
int ModalMessageBox(const wxString& text, const wxString& title, long style, const bool safe)
{
    // Disable all windows in the application
    // NOTE: This is done in the class's constructor
    wxWindowDisabler *disableAllWindows = new wxWindowDisabler();

    // Display message box in normal (use wxWidgets) or safe mode (use native) and wait 
    // for user input
    int retValue;
    if(safe)
        retValue = SafeMessageBox(text, title, style);
    else
        retValue = wxMessageBox(text, title, style);

    // The user closed the message box, so we can reactivate all the windows
    // NOTE: This is done in the class's destructor
    delete disableAllWindows;
    disableAllWindows = NULL;

    // Restore focus to the main frame (if active). This is necessary, 
    // otherwise the application could appear minimized.
    // NOTE: If this function is called during startup or shutdown,
    // the main frame might not be available, so we need to check for it.
    EditorFrame* const mainFrame = wxGetApp().GetMainFrame();
    if(mainFrame)
        mainFrame->SetFocus(); 

    // Return user input
    return retValue;
}

//////////////////////////////////////////////////////////////////////////
// CELAYOUTEDITOR IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
IMPLEMENT_APP(CELayoutEditor)

//////////////////////////////////////////////////////////////////////////
// CELAYOUTEDITOR EVENT TABLE
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CELayoutEditor, wxApp)
EVT_MENU(wxID_ABOUT, CELayoutEditor::OnAbout)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// CELAYOUTEDITOR FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
CELayoutEditor::CELayoutEditor() :
m_docManager(wx_static_cast(wxDocManager*, NULL)),
m_mainFrame(wx_static_cast(EditorFrame*, NULL)),
m_aboutBox(wx_static_cast(DialogAbout*, NULL)),
wxApp()
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
}

//-----------------------------------------------------------------------
bool CELayoutEditor::OnInit(void)
{
    // Initialize CelayoutEditor logger (uses CEGUI logger singleton)
    // IMPORTANT NOTE: This needs to be done before anything else (including registering
    // VEH exception handlers) because from now on many operations will lead to logging
    // and if the logger isn't initialized properly the editor might survive on its own if run
    // inside the debugger (that will automatically initialize the logger singleton itself) 
    // but it will always fail outside (which is the normal scenario).
    InitLogger();

    // Log some platform info
    //LogDebug(wxT("Default Gui font: %s, %d"), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFaceName(), wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT ).GetPointSize());

    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        // Register VEH exception handler that will be responsible for obtaining
        // the thrown exceptions (both CPP & win32 fatal ones) stack traces, codes and flags
        // NOTE: Only applicable on windows platforms
        ExceptionManager::GetInstancePtr()->RegisterExceptionHandler();
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

    #if wxUSE_ON_FATAL_EXCEPTION
        // Make sure that we enable fatal exceptions in debug and under VS. Fatal exceptions (also 
        // known as general protection faults under Windows or segmentation violations in the 
        // Unix world) will be caught and passed to CELayoutEditor::OnFatalException. By default, 
        // i.e. before this function is called, they will be handled in the normal way 
        // which usually just means that the application will be terminated.
        (void)wxHandleFatalExceptions(true);
    #endif // wxUSE_ON_FATAL_EXCEPTION

    // Force the application to exit when we delete the top-level frame (i.e. GetTopWindow()).
    // NOTE: This allows us to kill the application gracefully when an unhandled exception occurs
    SetExitOnFrameDelete(true);

    // Load all required image handlers
    // NOTE: We could have called wxInitAllImageHandlers(), which is more convenient,
    // but the size of the executable would grow due to all the installed handlers 
    // (most of which are not needed in CELayoutEditor anyway).
    InitImageHandlers();

    // If desired, display the CElayoutEditor logo (for a brief period) before loading the application
    #if DISPLAY_SPLASH_SCREEN
        DisplaySplashScreen();
    #endif // DISPLAY_SPLASH_SCREEN

    // Pre-load the ini file
    (void)Options::GetInstancePtr();

    // Create a document manager
    m_docManager = new wxDocManager;

    // Create templates relating drawing documents to their views
    new wxDocTemplate(m_docManager, wxT("Crazy Eddie's GUI layouts"), wxT("*.layout"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    new wxDocTemplate(m_docManager, wxT("Xml files"), wxT("*.xml"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    new wxDocTemplate(m_docManager, wxT("All files"), wxT("*.*"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    // If we've only got one window, we only get to edit one document at a time.
    m_docManager->SetMaxDocsOpen(1);
   
    // Initialize the application's main frame (with OpenGL support)
    return InitMainFrame();
}

//-----------------------------------------------------------------------
void CELayoutEditor::InitLogger() const
{
    // Instantiate logger first 
    if (!CEGUI::Logger::getSingletonPtr())
        new CEGUI::DefaultLogger();
    
    // Assign the log filename
    // NOTE: The file will be output to the current directory (where the executable is located)
    CEGUI::Logger::getSingleton().setLogFilename("CELayoutEditor.log", false);

    // Set informative logging level
    CEGUI::Logger::getSingleton().setLoggingLevel (CEGUI::Informative);
}


//-----------------------------------------------------------------------
void CELayoutEditor::InitImageHandlers() const
{
    // Make sure we are able to process PNG images (used for our CELayouteditor splash for instance)
    // NOTE: For the moment, only the splash screen requires PNG, so we only load the handler
    // if needed. Change this if more situations require the handler.
    #if DISPLAY_SPLASH_SCREEN
        wxImage::AddHandler(new wxPNGHandler);
    #endif // DISPLAY_SPLASH_SCREEN
}

#if DISPLAY_SPLASH_SCREEN
    //-----------------------------------------------------------------------
    void CELayoutEditor::DisplaySplashScreen()
    {
        // Load splash image into memory (art made by Frederico Martins - frederico.martins@ignite-games.com)	
        // NOTE: An error box will be presented to the user if the splash image isn't found
        wxBitmap bitmap;
        const bool loadSplash = bitmap.LoadFile(wxStandardPaths::Get().GetResourcesDir() + wxT("/splash.bmp"), wxBITMAP_TYPE_BMP);

        // Leave if image wasn't found or is invalid
        // NOTE: No further processing is necessary since the user will already be shown an error box
        if(!loadSplash)
            return;

        // Create splash screen using the previously loaded image
        // NOTE: We want to make sure no timeout is assigned, since we'll manage its duration manually. This
        // saves us from deriving a new class from wxSplashScreen and overriding the OnCloseWindow() method
        // to ensure that the application will only continue loading after the splash screen is destroyed.
        wxSplashScreen* const splashScreen = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT, -1, NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);

        // Yield control to pending messages in the windowing system
        // NOTE: wxYield(), although still frequently used, has been deemed obsolete and wxWidgets 
        // documentation (2.8.6) recommends using Yield() instead
        (void)wxApp::Yield();

        // Wait a while, then remove the splash screen. Then, wait a little bit more to allow the screen
        // to fade correctly.
        CELE_Sleep(SPLASH_SCREEN_DURATION);
        splashScreen->Hide(); // added to fix deadlock on Mac
        delete splashScreen;
        CELE_Sleep(SPLASH_SCREEN_FADE_OUT_DURATION);
    }
#endif // DISPLAY_SPLASH_SCREEN

//-----------------------------------------------------------------------
bool CELayoutEditor::InitMainFrame()
{
    // Validations
    wxASSERT_MSG(m_docManager != NULL, wxT("Document Manager wasn't properly initialized"));

    try 
    {
        // Create Single-Document main frame, just look default position and dimensions
        m_mainFrame = new EditorFrame(m_docManager, wx_static_cast(wxFrame*, NULL), wxID_ANY, wxT("CELayoutEditor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);

        // Set the application icon (art made by Frederico Martins - frederico.martins@ignite-games.com)
        // NOTE: Icon needs to be 32x32
        // NOTE2: If using Visual Studio, the file icon will be a different image (48x48)
        // and it will be added automatically as a project resource (icon48.ico). In 
        // other systems, the file (icon32.xpm) is used as both the application and file icon.
        m_mainFrame->SetIcon(wxIcon(icon32_xpm)); 
        
        // Prepare for the rendering canvas
        // We need to show the frame otherwise the CEGUI OpenGL renderer won't 
        // be able to initialize, because the OpenGL viewport is not valid then.
        m_mainFrame->Centre(wxBOTH);
        (void)m_mainFrame->Show(true);
              
        // This needs to be in the try-block!
        // NOTE: This operation is quite SLOW!
        (void)m_mainFrame->AttachCanvas(wx_static_cast(wxView*, NULL));
        SetTopWindow(m_mainFrame);

        // When a current layout was set, load it
        const wxString layout = Options::GetInstancePtr()->GetCurrentLayout();
        if (layout != wxT(""))
            (void)m_docManager->CreateDocument(layout, wxDOC_SILENT);

        // Create the application's "About" box
        // NOTE: Hidden for the moment
        // NOTE2: This needs to be called AFTER the main frame has been created,
        // because it will be added as a child window of the main frame (in order 
        // to prevent cleanup problems)
        CreateAboutBox();

        // Process all existing widget types and for each of them store their possible Look'N'Feels
        // for later use. Also, keep a global stash with all existing Look'N'Feels (skins).
        // NOTE: This needs to be done after the OpenGL canvas is attached, because we need to have
        // the falagard types mapped in the system. Therefore, it can't be done inside the main dialog
        // constructor.
        // NOTE2: This is only done once. This means that if the user adds new skins or modifies skins while
        // the editor is running, those changes won't be detected and the application will need to be 
        // restarted in order to recognize them. 
		if (GetMainFrame()->GetDialogMain())
		{
			GetMainFrame()->GetDialogMain()->StoreWidgetLooks();
		}
        
        // No problems found
        return true;
    }
    // If something goes wrong, log and show user
    SHOW_EXCEPTION_RETURN("CELayoutEditor::InitMainFrame", "Fatal error, application will terminate", "Fatal error, application will terminate", "Fatal error, application will terminate", false);
}

//------------------------------------------------------------------------
void CELayoutEditor::CreateAboutBox()
{
    // Create dialog info container (which will be used to fill the "About" 
    // box contents
    DialogAboutInfo info;

    // Fill base information (name/version/description/copyright)
    info.SetName(wxT("CELayoutEditor"));

    info.SetVersion(wxString::Format(wxT("%d.%d.%d"), CE_LAYOUT_EDITOR_MAJOR_VERSION, CE_LAYOUT_EDITOR_MINOR_VERSION, CE_LAYOUT_EDITOR_RELEASE_NUMBER));
    info.SetDescription(wxT("Thanks for using the \"official\" editor for CEGUI layouts."));
    info.SetCopyright(wxT("(C) 2005-2008 CEGUI Team"));

    // Add main contacts for application inquiries
    /*const wxString contacts[] = { wxT("Patrick Kooman"), wxT("Frederico Jeronimo") };
    const wxString contactsEmail[] = { wxT("pkooman@gmail.com"), wxT("frederico.jeronimo@ignite-games.com") };
    const wxString contactsNicks[] = { wxT("Scriptkid"), wxT("fjeronimo") };*/
	const wxString contacts[] = { wxT("CEGUI Team") };
    const wxString contactsEmail[] = { wxT("team@cegui.org.uk") };
    const wxString contactsNicks[] = { wxT("Team") };
    const wxString contactsDescs[] = { wxT(""), wxT("") };
    info.SetContacts(wxArrayString(WXSIZEOF(contacts), contacts), wxArrayString(WXSIZEOF(contactsEmail), contactsEmail), wxArrayString(WXSIZEOF(contactsNicks), contactsNicks), wxArrayString(WXSIZEOF(contactsDescs), contactsDescs));

    // Add CELayoutEditor icon (art made by Frederico Martins - frederico.martins@ignite-games.com)
    // NOTE: Unlike the application icon (which must be 32x32), the 
    // "About" box one is 48x48
    info.SetIcon(wxIcon(icon48_xpm));
    info.SetWebSite(wxT("http://crayzedsgui.sourceforge.net/"), wxT("CEGUI homepage"));

    // Add licence information (inserted inside a collapsible pane)
    info.SetLicence(wxString::FromAscii(
        "This application is free software; you can redistribute it and/or"
        "\nmodify it under the terms of the GNU General Public License"
        "\nas published by the Free Software Foundation; either version 2"
        "\nof the License, or (at your option) any later version."
        "\n"
        "\nThis software is distributed in the hope that it will be useful,"
        "\nbut WITHOUT ANY WARRANTY; without even the implied warranty of"
        "\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the"
        "\nGNU General Public License for more details."
        "\n"
        "\nYou should have received a copy of the GNU General Public License"
        "\nalong with this program; if not, write to the Free Software"
        "\nFoundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA."
        "\n"
        "\nTo view the licence online, go to: http://www.gnu.org/copyleft/gpl.html"
        ));

    // Set 'Built With' (third party applications used in the construction of CELayoutEditor) info
    info.SetBuiltWith(wxString::Format(wxT("CEGUI %d.%d.%d\nwxWidgets %d.%d.%d"), CEGUI_VERSION_MAJOR, CEGUI_VERSION_MINOR, CEGUI_VERSION_PATCH, wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER));

    // Add developers (inserted inside a collapsible pane)
    const wxString developers[] = { wxT("Patrick Kooman"), 
        wxT("Frederico Jeronimo"),
        wxT("Paul D Turner"),
        wxT("Olivier Delannoy"),
        wxT("\"Tuan Kuranes\""),
        wxT("\"Xirnohn\""),
        wxT("Sam Jansen")};
    const wxString developersEmails[] = { wxT("pkooman@gmail.com"), 
        wxT("frederico.jeronimo@ignite-games.com"),
        wxT(""),
        wxT("dalfy@users.sourceforge.net"),
        wxT("tuan.kuranes@free.fr"),
        wxT(""),
        wxT("")};
    const wxString developersNicks[] = { wxT("Scriptkid"), 
        wxT("fjeronimo"),
        wxT("CrazyEddie"),
        wxT("Dalfy"),
        wxT(""),
        wxT(""),
        wxT("")};
    const wxString developersDescs[] = { wxT("Main application."), 
        wxT("Major patches: looknfeel switch, selection (un)locking, exception handling, etc."),
        wxT("Linux makefiles and Unicode arrangements."),       
        wxT("Migration to 0.5 support."),
        wxT("Patch including tabcontrol support."),
        wxT("Patch including tooltip support and color setup for widgets."),
        wxT("Alignment class.")};
    info.SetDevelopers(wxArrayString(WXSIZEOF(developers), developers), wxArrayString(WXSIZEOF(developersEmails), developersEmails),wxArrayString(WXSIZEOF(developersNicks), developersNicks), wxArrayString(WXSIZEOF(developersDescs), developersDescs));

    // Add artists (inserted inside a collapsible pane)
    const wxString artists[] = { wxT("Frederico Martins")};
    const wxString artistsEmails[] = { wxT("innervortex@hotmail.com")};
    const wxString artistsNicks[] = { wxT("")};
    const wxString artistsDescs[] = { wxT("Splash screen and application icons.")};
    info.SetArtists(wxArrayString(WXSIZEOF(artists), artists), wxArrayString(WXSIZEOF(artistsEmails), artistsEmails), wxArrayString(WXSIZEOF(artistsNicks), artistsNicks), wxArrayString(WXSIZEOF(artistsDescs), artistsDescs));

    // Add acknowledgments (inserted inside a collapsible pane)
    info.AddAcknowledgment(wxT("Roger Orr"), wxT("rogero@howzatt.demon.co.uk"), wxT(""), wxT("Invaluable exception handling tips."));

    // Add hyper links (inserted inside a collapsible pane)
    const wxString hyperLinks[] = { wxT("http://sourceforge.net/projects/crayzedsgui"),
        wxT("http://www.cegui.org.uk/phpBB2/index.php"),
        wxT("http://www.cegui.org.uk/mantis/my_view_page.php")};
    const wxString hyperLinksDescs[] = { wxT("CEGUI Sourceforge Project"), 
        wxT("CEGUI Forums"),
        wxT("CEGUI Mantis")};
    info.SetHyperLinks(wxArrayString(WXSIZEOF(hyperLinks), hyperLinks), wxArrayString(WXSIZEOF(hyperLinksDescs), hyperLinksDescs));

    // Add footnote
    // NOTE: For the moment, it displays the application edition's name (based on 
    // European comics characters)
    info.SetFootnote(wxT("Corto Maltese edition"));

    // Populate "About" box with previous info
    m_aboutBox = new DialogAbout(info);
}

//-----------------------------------------------------------------------
int CELayoutEditor::OnExit()
{
    // Cleanup
    PrepareForExit();
    return 0;
}

//------------------------------------------------------------------------
void CELayoutEditor::PrepareForExit() const
{
    // Cleanup
    // NOTE: The main frame and its child the "About" box are
    // deleted automatically and internally by wxWidgets before
    // this call, so we must avoid deleting them again (which would lead
    // to a fatal error)
    if(m_docManager)
        delete m_docManager;   
    delete Options::GetInstancePtr();
    delete ExceptionManager::GetInstancePtr();
    delete CEGUI::Logger::getSingletonPtr();
}

//-----------------------------------------------------------------------
void CELayoutEditor::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    if(m_aboutBox)
    {
        // Center "About" box (necessary because we have a single "About" box 
        // and we want to ignore any positioning that was done in the past) and show 
        // it to user (modal)
        m_aboutBox->Center();
        (void)m_aboutBox->ShowModal();
    }
}

//-----------------------------------------------------------------------
void CELayoutEditor::SetModificationStateAllDocuments(bool modificationState) const
{
    // Validations
    wxASSERT_MSG(m_docManager != NULL, wxT("Document Manager wasn't properly initialized"));

    try
    {
        // Obtain a list of all currently open documents
        wxList openDocuments = m_docManager->GetDocuments();

        // Iterate previous list
        wxList::iterator iter;
        for (iter = openDocuments.begin(); iter != openDocuments.end(); ++iter)
        {
            // Obtain current document
            wxDocument* const currentDoc = wx_static_cast(wxDocument*, *iter);

            // If the document's current state doesn't match the desired one,
            // change it
            if(currentDoc && (currentDoc->IsModified() != modificationState))
                currentDoc->Modify(modificationState);
        }
    }
    // If something goes wrong, log and show user
    SHOW_EXCEPTION("CELayoutEditor::SetModificationStateAllDocuments", "Error", "Unexpected standard exception while setting modification state", "Unexpected non-standard exception while setting modification state");
}

//-----------------------------------------------------------------------
void CELayoutEditor::OnUnhandledException()
{
    // Log unhandled exception (unfortunately, exception type is lost by now)
    LogSilentError(wxT("CELayoutEditor::OnUnhandledException - Unhandled exception caught, program will terminate."));

    // Show the user the unhandled exception if stipulated
    #if SHOW_UNHANDLED
        // Exception type is lost by now, so inform user that an unhandled exception
        // occurred and exit the application. Depending on current configuration,
        // we use modal or modeless message boxes.
        (void)CELEMessageBox(wxT("CELayoutEditor::OnUnhandledException - Unhandled exception caught, program will terminate."),
                             wxT("CELayoutEditor Unhandled Exception"),
                             CELE_MB_TYPE_OK | CELE_MB_ICON_STOP | CELE_MB_SYSTEMMODAL);
    #endif // SHOW_UNHANDLED
}

//-----------------------------------------------------------------------
void CELayoutEditor::OnFatalException()
{
    // Check if wxWidgets was built with debug report support
    #if wxUSE_DEBUGREPORT
        // We have debug reports, so so inform the user of the fatal error and the possibility of a
        // debug report generation
        // NOTE: Unlike unhandled exceptions which can be customized, fatal exceptions 
        // are ALWAYS shown to user
        (void)CELEMessageBox(wxT("CELayoutEditor::OnFatalException - CELayoutEditor has encountered a fatal exception and needs to terminate. A debug report will be generated if you desire."),
                             wxT("CELayoutEditor Fatal Exception"),
                             CELE_MB_TYPE_OK | CELE_MB_ICON_STOP | CELE_MB_SYSTEMMODAL);

        // Create a debug report (with the current system state, stack trace, log file and configuration file) that
        // the user can save or ignore
        GenerateDebugReport(wxDebugReport::Context_Exception);
    #else
        // No debug report, so inform user of fatal error and exit application
        // NOTE: Unlike unhandled exceptions which can be customized, fatal exceptions 
        // are ALWAYS shown to user
        (void)CELEMessageBox(wxT("CELayoutEditor::OnFatalException - CELayoutEditor has encountered a fatal exception and needs to terminate. We're sorry for the inconvenience!"),
                             wxT("CELayoutEditor Fatal Exception"),
                             CELE_MB_TYPE_OK | CELE_MB_ICON_STOP | CELE_MB_SYSTEMMODAL);
    #endif // wxUSE_DEBUGREPORT    
}

//-----------------------------------------------------------------------
#if wxUSE_DEBUGREPORT
void CELayoutEditor::GenerateDebugReport(wxDebugReport::Context ctx, const bool showPreview) const
{
    // Create a debug report with the program's current state. All the files
    // that we'll be added to it will be compressed into a single .ZIP file.
    wxDebugReportCompress *report = new wxDebugReportCompress;

    // Adds all available information to the report. Currently this includes a 
    // text (XML) file describing the process context (system info + stack trace) 
    // and, under Win32, a minidump file.
    report->AddAll(ctx);

    // Create a timestamp file which contains the date and time of the crash.
    wxFileName fn(report->GetDirectory(), wxT("timestamp.my"));
    wxFFile file(fn.GetFullPath(), wxT("w"));
    if ( file.IsOpened() )
    {
        const wxDateTime dt = wxDateTime::Now();
        (void)file.Write(dt.FormatISODate() + wxT(' ') + dt.FormatISOTime());
        (void)file.Close();
    }

    // Add timestamp file to debug report
    report->AddFile(fn.GetFullName(), wxT("timestamp of this report"));

    // We also add the current log file (CELayoutEditor.log) and configuration 
    // file (CELayoutEditor.ini) to the debug report
    #ifdef __WINDOWS__
        report->AddFile(wxGetCwd() + wxT("\\CELayoutEditor.log"), wxT("CELayoutEditor log file"));
        report->AddFile(wxGetCwd() + wxT("\\CELayoutEditor.ini"), wxT("CELayoutEditor configuration file"));
    #else
        report->AddFile(wxGetCwd() + wxT("/CELayoutEditor.log"), wxT("CELayoutEditor log file"));
        report->AddFile(wxGetCwd() + wxT("/CELayoutEditor.ini"), wxT("CELayoutEditor configuration file"));
    #endif // __WINDOWS__

    // Display a preview window if so desired to the let the user review the report and 
    // decide what to do (delete files and/or add notes). The user can also abort the 
    // report generation by pressing the cancel button. If no preview is required, the report 
    // will be created automatically
    bool generateReport = true;
    if(showPreview && !wxDebugReportPreviewStd().Show(*report))
        generateReport = false;

    // Did the user allow the report or are we to generate it automatically?
    if(generateReport)
    {
        // Save all report data to disk
        if (report->Process())
        {
            // Inform user that operation was successful
            // NOTE: Some exceptions, like some access violations, prevent somehow the log
            // message from executing correctly and thus no log is written to disk. The report
            // is correctly generated and saved though.
            LogDebug(wxT("Report generated in \"%s\"."), report->GetCompressedFileName().c_str());
            report->Reset();
        }
        else
            // Ops, something went wrong
            LogError(wxT("CELayoutEditor::GenerateDebugReport - Failed to generate debug report!"));
    }

    // The report is no longer needed
    delete report;
}
#endif

//-----------------------------------------------------------------------
void CELayoutEditor::HandleEvent(wxEvtHandler *handler,wxEventFunction func, wxEvent& event) const
{
    try 
    {
        // Call function of supplied event handler with event as parameter
        wxApp::HandleEvent(handler, func, event);       
    }
    catch(CEGUI::Exception& guiException)
    {
        // We found a CEGUI exception, so deal with it
        ProcessUnhandledException(wxT("CEGUI"), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine());
    }
    catch(std::exception& ex)\
    {
        // We found a standard exception, so deal with it
        ProcessUnhandledException(wxT("standard"), StringHelper::ToWXString(ex.what()));
    }
    catch ( ... )
    {
        // We found a non-standard exception, so deal with it
        ProcessUnhandledException(wxT("non-standard"), wxT("Exception message not available!"));
    }
}

//-----------------------------------------------------------------------
void CELayoutEditor::ClearAllEvents() const
{
    // Clear all event tables
    wxEventHashTable::ClearAll();

    // Destroy pending events
    delete wxPendingEvents;
    wxPendingEvents = NULL;

    // Check if we are using threads (probably so)
    #if wxUSE_THREADS
        // Remove and destroy the pending events locker
        delete wxPendingEventsLocker;
        wxPendingEventsLocker = NULL;

        // Check if we use validators
        #if wxUSE_VALIDATORS
            // If we don't do the following, we get an apparent memory leak.
            (wx_static_cast(wxEvtHandler&, wx_const_cast(wxValidator&, wxDefaultValidator))).ClearEventLocker();
        #endif // wxUSE_VALIDATORS
    #endif // wxUSE_THREADS
}

//-----------------------------------------------------------------------
// NOTE: We cannot use debug reports here because they are only meant for fatal exceptions...
void CELayoutEditor::ProcessUnhandledException(const wxString& exceptionType, const wxString& exceptionText, const wxString& exceptionFileName, const int exceptionLine) const
{
    // Disable all windows in the application
    // NOTE: This is done in the class's constructor
    wxWindowDisabler *disableAllWindows = new wxWindowDisabler();

    // Check if we are to exit the application when an unhandled exception is encountered
    // NOTE: This setting only has any impact in debug builds and should only
    // be used when debugging a specific problem. If we're doing a release
    // build, the application ALWAYS terminates (this avoids endless loops problems that
    // could occur otherwise).
    #if TERMINATE_ON_UNHANDLED || !defined(_DEBUG)   
        // Eliminate all application events (current and pending) since we don't want
        // any interference now that we are quitting the program
        ClearAllEvents();

        // Obtain exception info, composed of basic info (description, location, message), line and 
        // file info (for CEGUI exceptions) and VEH info if under Windows (stack trace, exception code, type & flags)
        const wxString exceptionInfo = ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("CELayoutEditor::ProcessUnhandledException"), wxString::Format(wxT("CELayoutEditor has encountered an unhandled %s exception and needs to terminate."), exceptionType.c_str()), exceptionText, exceptionFileName, exceptionLine);

        // Log exception
        LogSilentError(exceptionInfo);

        // Display exception to user if so desired (using modal or modeless message boxes)
        #if SHOW_UNHANDLED
            (void)CELEMessageBox(exceptionInfo,
                                 wxT("CELayoutEditor Unhandled Exception"),
                                 CELE_MB_TYPE_OK | CELE_MB_ICON_STOP | CELE_MB_SYSTEMMODAL);
        #endif // SHOW_UNHANDLED

        // If we need to prevent the saving of the file, make sure all open documents have the 
        // modify flag set to false
        // NOTE: Be careful if you allow the user to decide if the file should be saved since some 
        // exceptions might create an unstable system state that might invalidate the file save,
        // thus corrupting the layout file (rare).
        #if PREVENT_SAVE_ON_UNHANDLED
            SetModificationStateAllDocuments(false);
        #endif // PREVENT_SAVE_ON_UNHANDLED

        // Close the top window
        // NOTE: Since we are setting the SetExitOnFrameDelete(true) in 
        // the CELayoutEditor::OnInit() function, this operation will
        // gracefully cleanup the application before exit
        (void)this->GetTopWindow()->Close();

        // Perform some cleanup (safety-check)
        PrepareForExit();

        // Reactivate all the windows
        // NOTE: This is done in the class's destructor
        delete disableAllWindows;
        disableAllWindows = NULL;

        // Exit the application with a failure code
        exit(EXIT_FAILURE);
    #else

        // Obtain exception info, composed of basic info (description, location, message), line and 
        // file info (for CEGUI exceptions) and VEH info if under Windows (stack trace, exception code, type & flags)
        const wxString exceptionInfo = ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("CELayoutEditor::ProcessUnhandledException"), wxString::Format(wxT("CELayoutEditor has encountered an unhandled %s exception."), exceptionType), exceptionText, exceptionFileName, exceptionLine);

        // The application will keep on running, so inform the user about it (or just 
        // log the exception)
        #if SHOW_UNHANDLED
            LogError(exceptionInfo);
        #else
            LogSilentError(exceptionInfo);
        #endif // SHOW_UNHANDLED

        // Reactivate all the windows
        // NOTE: This is done in the class's destructor
        delete disableAllWindows;
        disableAllWindows = NULL;
    #endif // TERMINATE_ON_UNHANDLED
}

//////////////////////////////////////////////////////////////////////////
// WXLOG WRAPPERS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
void LogError(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Display error message to user in a modal or modeless message box (with an error icon) 
    #if USE_MODAL_MESSAGE_BOXES
        // Create a truly modal message box with an error icon to display the message 
        (void)ModalMessageBox(msg, wxT("CELayoutEditor Error"), wxOK | wxICON_ERROR);
    #else
        // Again, let wxWidgets process the error message and pop up a message box (with an error icon) 
        // to inform the user about it (default behavior). Works in both Debug and Release builds.
        // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
        // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
        // to pass the variable arguments to it.
        wxVLogError(szFormat, argptr);
    #endif // USE_MODAL_MESSAGE_BOXES

    // Insert previous string into log as an error message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Errors);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogSilentError(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Insert previous string into log as an error message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    // NOTE2: We are only logging to our own log file and ignoring the wxLog*
    // family of functions, since we want this to be silent
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Errors);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogDebug(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Again, we let wxWidgets process the debug message.  In its default behavior it only does anything 
    // at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
    // to nothing in release mode (otherwise). 
    // NOTE: Under Windows, you must either run the program under debugger (e.g. in Visual 
    // Studio the message is output to the console output window) or use a 3rd party 
    // program such as DebugView to actually see the debug output. Under Unix the 
    // output goes to stderr so you don't need any tools to see it.
    // NOTE2: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
    // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
    // to pass the variable arguments to it.
    wxVLogDebug(szFormat, argptr);

    // Insert previous string into log as a standard message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Standard);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogSilentDebug(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Insert previous string into log as a standard message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    // NOTE2: We are only logging to our own log file and ignoring the wxLog*
    // family of functions, since we want this to be silent
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Standard);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogWarning(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Display warning message to user in a modal or modeless message box (with an information icon) 
    #if USE_MODAL_MESSAGE_BOXES
        // Create a truly modal message box with an information icon to display the message 
        (void)ModalMessageBox(msg, wxT("CELayoutEditor Warning"), wxOK | wxICON_EXCLAMATION);
    #else
        // Again, let wxWidgets process the warning message and pop up a message box (with 
        // OK button and information icon) to inform the user about it (default behavior). 
        // Works in both Debug and Release builds.
        // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
        // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
        // to pass the variable arguments to it.
        wxVLogWarning(szFormat, argptr);
    #endif // USE_MODAL_MESSAGE_BOXES

    // Insert previous string into log as an warning message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Warnings);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogSilentWarning(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Insert previous string into log as an warning message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    // NOTE2: We are only logging to our own log file and ignoring the wxLog*
    // family of functions, since we want this to be silent
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Warnings);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogMessage(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Display normal message to user in a modal or modeless message box (with an information icon) 
    #if USE_MODAL_MESSAGE_BOXES
        // Create a truly modal message box with an information icon to display the message 
        (void)ModalMessageBox(msg, wxT("CELayoutEditor Info"), wxOK | wxICON_INFORMATION);
    #else
        // Again, let wxWidgets process the normal message and pop up a message box (with OK 
        // button and information icon) to inform the user about it (default behavior). Works 
        // in both Debug and Release builds.
        // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
        // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
        // to pass the variable arguments to it.
        wxVLogMessage(szFormat, argptr);
    #endif // USE_MODAL_MESSAGE_BOXES

    // Insert previous string into log as an informative message (CELayoutEditor.log)
    // NOTE: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Informative);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogTrace(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Let wxWidgets process the trace message.  In its default behavior it only does anything 
    // at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
    // to nothing in release mode (otherwise). Behavior is the same as for debug messages 
    // (see LogDebug).
    // NOTE: Under Windows, you must either run the program under debugger (e.g. in Visual 
    // Studio the message is output to the console output window) or use a 3rd party 
    // program such as DebugView to actually see the debug output. Under Unix the 
    // output goes to stderr so you don't need any tools to see it.
    // NOTE2: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
    // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
    // to pass the variable arguments to it.
    // NOTE3: Unconditional log trace has been suppressed in recent wxWidgets versions, so
    // we use the log debug variant which is equivalent
    wxVLogDebug(szFormat, argptr);

    // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Insert previous string into log as a insane message (CELayoutEditor.log)
    // NOTE: Here lies the difference with debug messages (see LogDebug & LogSilentDebug)
    // NOTE2: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Insane);

    // Cleanup
    va_end(argptr);
}

//-----------------------------------------------------------------------
void LogSilentTrace(const wxChar *szFormat, ...)
{
    // Make the variadic list point to the first argument after szFormat.
    va_list argptr;
    va_start(argptr, szFormat);

    // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
    // string.
    wxString msg;
    (void)msg.PrintfV(szFormat, argptr);

    // Insert previous string into log as a insane message (CELayoutEditor.log)
    // NOTE: Here lies the difference with debug messages (see LogDebug & LogSilentDebug)
    // NOTE2: If this function is called before CEGUI is properly initialized,
    // the CEGUI logger will be null (and so will the returned pointer). We need
    // to ignore logging in that case. A particular example is when the ini 
    // file is not found (first time CELayoutEditor is run) and a warning is 
    // issued. 
    // NOTE2: We are only logging to our own log file and ignoring the wxLog*
    // family of functions, since we want this to be silent
    CEGUI::Logger* const logger = CEGUI::Logger::getSingletonPtr();
    if(logger)
        logger->logEvent(StringHelper::ToCEGUIString(msg), CEGUI::Insane);

    // Cleanup
    va_end(argptr);
}
