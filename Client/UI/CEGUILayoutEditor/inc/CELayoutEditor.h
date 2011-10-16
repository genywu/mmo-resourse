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

#ifndef _CELAYOUT_EDITOR_H_
#define _CELAYOUT_EDITOR_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "EditorFrame.h"
#include "DialogAbout.h"
#include "ExceptionManager.h"

#include "wx/debugrpt.h"

//////////////////////////////////////////////////////////////////////////
// FORWARD REFERENCES
//////////////////////////////////////////////////////////////////////////

class wxDocManager;

//////////////////////////////////////////////////////////////////////////
// CELAYOUTEDITOR                                                       
//////////////////////////////////////////////////////////////////////////

/** CelayoutEditor - The application class. The program starts here.
 */
class CELayoutEditor : public wxApp
{

public:

    /************************************************************************
     * CONSTRUCTOR/DESTRUCTOR
     ************************************************************************/
    
    /** Constructor - initializes / prepares members.
     * @access public 
     * @qualifier none
     * @return void
     */
    CELayoutEditor();

    /************************************************************************
     * GETTERS/SETTERS
     ************************************************************************/
    
    /** Returns the main frame, which includes the menu and such.
     * @access public 
     * @qualifier const
     * @return EditorFrame* Main frame
     */
    EditorFrame* GetMainFrame() const
    {
        return m_mainFrame;
    }

	/** Returns the doc manager.
     * @access public 
     * @qualifier const
     * @return wxDocManager* doc manager
     */
    wxDocManager* GetDocManager() const
    {
        return m_docManager;
    }
    
    /************************************************************************
     * PUBLIC OVERRIDDEN
     ************************************************************************/
    
    /** Called by the wxWidgets framework. See this as the Main function
     * of your application.
     * @access public 
     * @qualifier none
     * @return bool success (true) or failure (false).
     */
    bool OnInit();

    /** Called by the wxWidgets framework when the application exits. Request
     * for saving the current document and such have already been done by now.
     * @access public 
     * @qualifier none
     * @return int
     */
    int OnExit();

    /** We handle the About box ourself (overridden).
     * @access public 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnAbout(wxCommandEvent& event);

    /** Overridden function which encapsulates with a try/catch clause the call 
     * to the supplied function of the specified event handler with the event as parameter, 
     * thus allowing us to catch all C++ exceptions which are thrown by any event handler 
     * in the application. This effectively represents the 2nd-level exception 
     * handling (the first being handled exceptions). The received exception is processed by 
     * @see ProcessUnhandledException(). Depending on @see TERMINATE_ON_UNHANDLED define 
     * execution resumes normally (0) or the application is terminated (1).
     * @access public 
     * @qualifier const
     * @param handler Event handler to receive the function call
     * @param func Function be called
     * @param event Event supplied as parameter to the function call
     * @return void
     * @remarks Since we are overriding this function, the 
     * @see CELayoutEditor::OnUnhandledException() will probably never be called (except during
     * initialization and termination), since any C++ exceptions will be intercepted here first.
     * @remarks This function will not catch exceptions generated during the program startup 
     * and shutdown (OnInit, OnExit).
     * @remarks We are not overriding wxApp::OnExceptionInMainLoop, since this function
     * catches all exceptions during wxApp::Run().
     */
    virtual void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const;

    /** Overridden function representing the 3th, and final, level exception 
     * handling (the first being handled exceptions): This function is called whenever 
     * an unhandled C++ exception occurs inside OnRun(). Program terminates after 
     * this call.
     * @access public 
     * @qualifier none
     * @return void
     * @remarks This function will probably never be called, because we are also
     * overriding the @see CELayoutEditor::HandleEvent which will catch every exception
     * thrown by any event handler in the application. However, unlike 
     * wxApp::OnExceptionInMainLoop exceptions which occur during the program startup 
     * and shutdown (OnInit, OnExit) might be caught here, but there is no guarantee.
     * @remarks Note that the exception type is lost by now.
     */
    virtual void OnUnhandledException();

    /** Overridden function that may be called when a fatal error occurs (such as an 
     * unhandled win32 exception or a fatal signal under Unix). Examples of such
     * errors are dereferencing null pointer, division by 0, access violation, etc. 
     * However, this will not happen by default: you have to explicitly call 
     * wxHandleFatalExceptions() to enable this. A message box will pop up to inform 
     * the user of the problem and CELayoutEditor will close. If wxUSE_DEBUGREPORT 
     * is set to 1 (true) this function will generate a debug 
     * report (@see CELayoutEditor::GenerateDebugReport) on the error before 
     * closing the application.
     * @access public 
     * @qualifier none
     * @return void
     * @remarks If you really want to test this func, add these lines somewhere in your code:
     * int* p = 0;
     * *p = 0; // Cause access violation
     * If you add them in for instance EditorFrame::OnLayoutSkin, whenever you try
     * to apply a new skin, the application will generate a fatal exception.
     * @remarks Fatal errors are normally uncatchable in the application code (at 
     * least in VS2005 synchronous mode - see http://members.cox.net/doug_web/eh.htm). 
     * However, if you're building using VS2003, a bug in the compiler (see previous link)
     * will make the catch(...) statements catch these exceptions (when they shouldn't).
     */
    virtual void OnFatalException();

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** This changes the modification state (i.e. indication of whether the document
     * was modified since the last save) of ALL the open documents.
     * @access public 
     * @qualifier const
     * @param modificationState New modification state (true=modified, false=not modified)
     * @return void
     */
    void SetModificationStateAllDocuments(bool modificationState) const;

protected:

    /************************************************************************
     * PROTECTED VARIABLES
     ************************************************************************/
    
    /** Manages the document/view framework supported by wxWidgets.
     */
    wxDocManager* m_docManager;

    /************************************************************************
     * PROTECTED FUNCTIONS
     ************************************************************************/
    
    /** This document manages an application unhandled exception caught in @see HandleEvent(). 
     * Depending on @see TERMINATE_ON_UNHANDLED, the application can continue after
     * the unhandled exception (0) or exit gracefully (1). In this last case, 
     * @see PREVENT_SAVE_ON_UNHANDLED will determine if the layout file is to be saved (1)
     * or not (0). Finally, @see SHOW_UNHANDLED is used to verify if the exception is 
     * to be displayed to the user (1) or not (0).       
     * @access protected 
     * @qualifier const
     * @param exceptionType Exception type (e.g. CEGUI, standard or non-standard)
     * @param exceptionText Exception message
     * @param exceptionFileName Exception file name (only for CEGUI exceptions)
     * @param exceptionLine Exception line (only for CEGUI exceptions)
     * @return void
     */
    void ProcessUnhandledException(const wxString& exceptionType, const wxString& exceptionText, const wxString& exceptionFileName=wxT(""), const int exceptionLine=-1) const;

    /** Cleans all application events (current and pending). This method contemplates
     * threads (if active) and validators (if active).
     * @access protected 
     * @qualifier const
     * @return void
     * @remarks This effectively removes all application activity and should only
     * be called when application is about to exit.
     */
    void ClearAllEvents() const;

#if wxUSE_DEBUGREPORT
    /** This function creates a standard debug report based on the current debug context. 
     * It can also display a preview window (via wxDebugReportPreviewStd) allowing the 
     * user to examine the contents of the debug report, remove files from and add 
     * notes to it. The user can also cancel the report by pressing the cancel button. If no 
     * preview window is supplied, the debug report is automatically created with default 
     * settings. By default, it  attaches the current log file (CELayoutEditor.log) and the CELayoutEditor 
     * configuration file (CELayoutEditor.ini).
     * @access protected 
     * @qualifier const
     * @param ctx Current debug context used to generate the report
     * @param showPreview Indicates if a preview window should be displayed (true) or 
     * not (false)
     * @return void
     * @remarks This function should only be called when a fatal exception occurs
     * or unpredictable results might occur (including wxWidgets assert violations)
     */
    void GenerateDebugReport(wxDebugReport::Context ctx, const bool showPreview = true) const;
#endif

    /** Performs various cleanups in order to allow a safe exit.
     * @access protected 
     * @qualifier const
     * @return void
     */
    void PrepareForExit() const;

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Application's main frame, which includes the menu and such.
     */
    EditorFrame*	m_mainFrame;

    /** Application's about box
     */
    DialogAbout* m_aboutBox;
   
    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Creates and setups the CELayoutEditor logger.
     * @access private 
     * @qualifier const
     * @return void
     */
    void InitLogger() const;

    /** Load all required wxWidgets image handlers, responsible for displaying
     * images in various formats (e.g. PNG).
     * @access private 
     * @qualifier const
     * @return void
     * @remarks wxWidgets offers an function wxInitAllImageHandlers() that loads
     * all available image handlers. However, we avoid using that because
     * the size the size of the executable will grow due to all the installed handlers 
     * (most of which are not needed in CELayoutEditor anyway).
     */
    void InitImageHandlers() const;

    #if DISPLAY_SPLASH_SCREEN
        /** Shows a splash screen to the user before loading the application.
         * @access private 
         * @qualifier none
         * @return void
         */
        void DisplaySplashScreen();
    #endif // DISPLAY_SPLASH_SCREEN

    /** Initializes the application's main frame (with OpenGL support).
     * @access private 
     * @qualifier none
     * @return bool
     */
    bool InitMainFrame();
    
    /** Creates and populates the application's "About" box.
     * @access private 
     * @qualifier none
     * @return void
     * @remarks The "About" box will appear hidden initially
     */
    void CreateAboutBox();

    /************************************************************************
     * EVENTS
     ************************************************************************/
    
    DECLARE_EVENT_TABLE();
};

// Provides wxGetApp();
DECLARE_APP(CELayoutEditor)

//////////////////////////////////////////////////////////////////////////
// MESSAGE BOXES
//////////////////////////////////////////////////////////////////////////

/** This function shows a message to the user in a safe way and should be safe to call even 
 * before the application has been initialized or if it is currently in some other strange 
 * state (for example, about to crash). Under Windows this function shows a message box 
 * using a native dialog instead of wxMessageBox (which might be unsafe to call), elsewhere 
 * it simply prints the message to the standard output using the title as prefix.
 * @access public 
 * @qualifier none
 * @param text Message text to be displayed
 * @param title Message box window title
 * @param style MFC flags that specify the contents and behavior of the message box (e.g. MB_OK)
 * @return int Windows identifier of the message box's button the user pressed (e.g. IDOK)
 */
int SafeMessageBox(const wxString& text, const wxString& title, long style);

/** Displays a message to the user using a true modal window (i.e. all external application 
 * input is turned off while while the user doesn't close the dialog) by disabling all other
 * windows. The message box can be displayed in normal or safe mode. In the first case, the 
 * wxWidgets wxMessageBox() is used, whereas @see SafeMessageBox() is employed in 
 * the second one. 
 * @access public 
 * @qualifier none
 * @param text Message text to be displayed
 * @param title Message box window title
 * @param style MFC flags that specify the contents and behavior of the message box (e.g. MB_OK) 
 * when in safe mode, or wxWidgets flags instead (e.g. wxOK).
 * @param safe Display message box in safe mode (true) or in normal one (false)
 * @return int Windows identifier of the message box's button the user pressed (e.g. IDOK) when
 * in normal mode or wxWidgets equivalent identifier in safe mode (e.g. wxID_OK) 
 */
int ModalMessageBox(const wxString& text, const wxString& title, long style, const bool safe=false);

/** Shows a message to the user.  The exact behaviour of this function depends
 * upon the configuration options set; basically it will call either the
 * ModalMessageBox or SafeMessageBox function.
 * @access public 
 * @qualifier none
 * @param text Message text to be displayed
 * @param title Message box window title
 * @param style Flags that specify the contents and behavior of the message box.
 * @return int Identifier of the message box's button the user pressed (e.g. CELE_MBOK)
 */
int CELEMessageBox(const wxString& text, const wxString& title, long style);

//////////////////////////////////////////////////////////////////////////
// WXLOG WRAPPERS
//////////////////////////////////////////////////////////////////////////

/** Wrapper function around wxLogError which pops up a message box (with an error icon) 
 * to inform the user about it (default behavior). It also saves the error message
 * to log (CELayoutEditor.log) with logging level 'Errors'.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style) 
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogError(const wxChar *szFormat, ...);

/** Log function which saves the error message to log (CELayoutEditor.log) with 
 * logging level 'Errors'. It provides no wrapper around the wxLog* family of functions
 * since we want the output to be silent.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style) 
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogSilentError(const wxChar *szFormat, ...);

/** Wrapper function around wxLogDebug which in its default behavior only does anything 
 * at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
 * to nothing in release mode (otherwise). It also saves the error message to log (CELayoutEditor.log)
 * with logging level 'Standard'.
 * @access public
 * @qualifier none
 * @param szFormat Format string to log (printf style) 
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogDebug(const wxChar *szFormat, ...);

/** Log function which saves the error message to log (CELayoutEditor.log) with 
* logging level 'Standard'. It provides no wrapper around the wxLog* family of functions
* since we want the output to be silent.
* @access public 
* @qualifier none
* @param szFormat Format string to log (printf style)
* @param ... Variable number of arguments to the format string
* @return void
* @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
* other string types (like CEGUI::String or char*) because it could lead to unpredictable
* results and Win32 'Access Violation' exceptions that will crash the application (activating
* CELayoutEditor::OnFatalException() method).
*/
void LogSilentDebug(const wxChar *szFormat, ...);

/** Wrapper function around wxLogWarning which pops up a message box (with OK button) 
 * to inform the user about it (default behavior). It also saves the error 
 * message to log (CELayoutEditor.log) with logging level 'Standard'.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style) 
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogWarning(const wxChar *szFormat, ...);

/** Log function which saves the error message to log (CELayoutEditor.log) with 
* logging level 'Warnings'. It provides no wrapper around the wxLog* family of functions
* since we want the output to be silent.
* @access public 
* @qualifier none
* @param szFormat Format string to log (printf style)
* @param ... Variable number of arguments to the format string
* @return void
* @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
* other string types (like CEGUI::String or char*) because it could lead to unpredictable
* results and Win32 'Access Violation' exceptions that will crash the application (activating
* CELayoutEditor::OnFatalException() method).
*/
void LogSilentWarning(const wxChar *szFormat, ...);

/** Wrapper function around wxLogMessage which pops up a message box (with OK button) 
 * to inform the user about it (default behavior). It also saves the error 
 * message to log (CELayoutEditor.log) with logging level 'Informative'.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style)
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogMessage(const wxChar *szFormat, ...);

/** Wrapper function around wxLogDebug which in its default behavior only does anything 
 * at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
 * to nothing in release mode (otherwise). It also saves the error message to log (CELayoutEditor.log)
 * with logging level 'Insane'.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style)
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogTrace(const wxChar *szFormat, ...);

/** Log function which saves the error message to log (CELayoutEditor.log) with 
 * logging level 'Insane'. It provides no wrapper around the wxLog* family of functions
 * since we want the output to be silent.
 * @access public 
 * @qualifier none
 * @param szFormat Format string to log (printf style)
 * @param ... Variable number of arguments to the format string
 * @return void
 * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
 * other string types (like CEGUI::String or char*) because it could lead to unpredictable
 * results and Win32 'Access Violation' exceptions that will crash the application (activating
 * CELayoutEditor::OnFatalException() method).
 */
void LogSilentTrace(const wxChar *szFormat, ...);

//////////////////////////////////////////////////////////////////////////
// MACROS
//////////////////////////////////////////////////////////////////////////
// defines used by message boxes and such
// main purpose of these was to to be able to remove
// some Win32 platform defines from the main code.
// I guess they could have just been defined for other
// platforms but such things are not exactly collision safe.
#if defined(__WINDOWS__)
    #define CELE_MB_RESULT_OK       IDOK
    #define CELE_MB_RESULT_CANCEL   IDCANCEL
    #define CELE_MB_TYPE_OK         MB_OK
    #define CELE_MB_ICON_STOP       MB_ICONSTOP
    #define CELE_MB_SYSTEMMODAL     MB_SYSTEMMODAL
#else
    #define CELE_MB_RESULT_OK       1
    #define CELE_MB_RESULT_CANCEL   2
    #define CELE_MB_TYPE_OK         0
    #define CELE_MB_ICON_STOP       0
    #define CELE_MB_SYSTEMMODAL     0
#endif

/** Macro that is defined to be the sleep function on the target system
 * @access public 
 * @qualifier none
 * @param s Number of microseconds to sleep for.
 * @remarks Currently this breaks if not on Windows, Linux, or Apple.
 */
#if defined (__WINDOWS__)
    #define CELE_Sleep(s)   Sleep(s)
#elif defined (__linux__) || defined(__APPLE_CC__)
    #define CELE_Sleep(s)   usleep((s) * 1000)
#else
    // What to put here?
    #define CELE_Sleep(s)
#endif

/** Macro that catches, logs and shows the the thrown exception (either CEGUI based, standard 
 * or non-standard) to the user.
 * @access public 
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. CELayoutEditor::HandleEvent)
 * @param CEGUITxt Text to display when a CEGUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @remarks All parameters can be supplied as CEGUI strings, wxStrings or char*.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this 
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See 
 * http://members.cox.net/doug_web/eh.htm for more info.
 */
#define SHOW_EXCEPTION(location, CEGUITxt, standardTxt, nonStandardTxt)\
    catch(const CEGUI::Exception& guiException)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(CEGUITxt), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));\
    }\
    catch(const std::exception& ex)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(standardTxt), StringHelper::ToWXString(ex.what())));\
    }\
    catch ( ... )\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(nonStandardTxt)));\
    }

/** Macro that catches, logs and shows the the thrown exception (either CEGUI based, standard 
 * or non-standard) to the user. It also returns the supplied value when an exception occurs.
 * @access public 
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. CELayoutEditor::HandleEvent)
 * @param CEGUITxt Text to display when a CEGUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @param returnValue Value to return when an exception is caught.
 * @remarks All text parameters can be supplied as CEGUI strings, wxStrings or char*. The 
 * return value can be anything.
 * @remarks When you want to return void, just supply an empty last macro parameter 
 * (e.g. SHOW_EXCEPTION_RETURN("1", "2", "3", "4",) ). @see DialogMain::SetWidgetLook for an example.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this 
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See 
 * http://members.cox.net/doug_web/eh.htm for more info.
 */
#define SHOW_EXCEPTION_RETURN(location, CEGUITxt, standardTxt, nonStandardTxt, returnValue)\
    catch(const CEGUI::Exception& guiException)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(CEGUITxt), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));\
        return returnValue;\
    }\
    catch(const std::exception& ex)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(standardTxt), StringHelper::ToWXString(ex.what())));\
        return returnValue;\
    }\
    catch ( ... )\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(nonStandardTxt)));\
        return returnValue;\
    }

/** Macro that catches and logs the thrown exception (either CEGUI based, standard 
 * or non-standard). If you want to display it to the user as well @see SHOW_EXCEPTION or 
 * @see SHOW_EXCEPTION_RETURN.
 * @access public 
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. CELayoutEditor::HandleEvent)
 * @param CEGUITxt Text to display when a CEGUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @remarks All parameters can be supplied as CEGUI strings, wxStrings or char*.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this 
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See 
 * http://members.cox.net/doug_web/eh.htm for more info.
 */
#define LOG_EXCEPTION(location, CEGUITxt, standardTxt, nonStandardTxt)\
    catch(const CEGUI::Exception& guiException)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(CEGUITxt), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));\
    }\
    catch(const std::exception& ex)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(standardTxt), StringHelper::ToWXString(ex.what())));\
    }\
    catch ( ... )\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(nonStandardTxt)));\
    }

/** Macro that catches and logs the thrown exception (either CEGUI based, standard 
 * or non-standard) in a complete safe way (i.e. no more exceptions can be generated
 * while processing the current one). If you want to display it to the user as well @see SHOW_EXCEPTION or 
 * @see SHOW_EXCEPTION_RETURN.
 * @access public 
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. CELayoutEditor::HandleEvent)
 * @param CEGUITxt Text to display when a CEGUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @remarks All parameters can be supplied as CEGUI strings, wxStrings or char*.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this 
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See 
 * http://members.cox.net/doug_web/eh.htm for more info.
 * @remarks This macro, since it may never issue an exception itself, is
 * very useful for catching exceptions inside a destructor (which should never
 * throw exceptions).
 * @remarks If a problem occurs during logging (highly unlikely) the new
 * freshly produced exception will be eaten and ignored and no indication
 * will exist of what happened.
 * 
 */
#define LOG_EXCEPTION_SAFE(location, CEGUITxt, standardTxt, nonStandardTxt)\
    catch(const CEGUI::Exception& guiException)\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(CEGUITxt), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));\
        }\
        catch(...) {} \
    }\
    catch(const std::exception& ex)\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(standardTxt), StringHelper::ToWXString(ex.what())));\
        }\
        catch(...) {} \
    }\
    catch ( ... )\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(nonStandardTxt)));\
        }\
        catch(...) {} \
    }

/** Macro that catches and logs the thrown exception (either CEGUI based, standard or 
 * non-standard). If you want to display it to the user as well @see SHOW_EXCEPTION or 
 * @see SHOW_EXCEPTION_RETURN. It also returns the supplied value when an exception occurs.
 * @access public 
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. CELayoutEditor::HandleEvent)
 * @param CEGUITxt Text to display when a CEGUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @param returnValue Value to return when an exception is caught.
 * @remarks All text parameters can be supplied as CEGUI strings, wxStrings or char*. The 
 * return value can be anything.
 * @remarks When you want to return void, just supply an empty last macro parameter 
 * (e.g. LOG_EXCEPTION_RETURN("1", "2", "3", "4",) ). @see DialogMain::SetWidgetLook for a similar example 
 * using @see SHOW_EXCEPTION_RETURN.
 * @remarks When you want to return void, just supply ; as the returnValue.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this 
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See 
 * http://members.cox.net/doug_web/eh.htm for more info.
 */
#define LOG_EXCEPTION_RETURN(location, CEGUITxt, standardTxt, nonStandardTxt, returnValue)\
    catch(const CEGUI::Exception& guiException)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(CEGUITxt), StringHelper::ToWXString(guiException.getMessage()), StringHelper::ToWXString(guiException.getFileName()), guiException.getLine()));\
        return returnValue;\
    }\
    catch(const std::exception& ex)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(standardTxt), StringHelper::ToWXString(ex.what())));\
        return returnValue;\
    }\
    catch ( ... )\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StringHelper::ToWXString(location), StringHelper::ToWXString(nonStandardTxt)));\
        return returnValue;\
    }

#endif // _CELAYOUT_EDITOR_H_
