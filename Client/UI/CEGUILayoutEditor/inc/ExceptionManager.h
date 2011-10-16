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

#ifndef _EXCEPTION_MANAGER_H_
#define _EXCEPTION_MANAGER_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Config.h"

//////////////////////////////////////////////////////////////////////////
// DUMMY POLYMORPHIC
//////////////////////////////////////////////////////////////////////////

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

#if defined(_CPPRTTI) && defined(_MSC_VER)
/** This dummy polymorphic class is used to obtain the class name of
* the thrown exception in the VEH handler using RTTI. @see GetRTTIExceptionType
* for more info.
*/
class DummyPolymorphic { virtual ~DummyPolymorphic() {} };
#endif // defined(_CPPRTTI) && defined(_MSC_VER)

//////////////////////////////////////////////////////////////////////////
// VEH HANDLER
//////////////////////////////////////////////////////////////////////////

/** VEH (Vectored Exception Handling) handler function called 
 * whenever an exception is thrown (both CPP & Win32 exceptions). The handler
 * is called before the try/catch block and is responsible for obtaining
 * the exception stack trace, code, type and flags (each item is optional as retrieval
 * is dependent on the value of the defines present in Config.h). It is also capable of handling
 * multiple stack traces if CHECK_MULTIPLE_STACKS is 1.
 * 
 * Thanks go to Roger Orr (rogero@howzatt.demon.co.uk) for all the help he provided in 
 * the process.
 *
 * @access public static 
 * @qualifier none
 * @param exceptionInfo Contains caught exception info (record & context)
 * @return LONG WINAPI EXCEPTION_CONTINUE_SEARCH under normal scenarios and 
 * EXCEPTION_EXECUTE_HANDLER in the case of ignorable user raised exceptions (see remarks)
 * @remarks Only works on Windows platforms, under debug builds and if CHECK_STACK_TRACES is set.
 * @remarks This might work in non-MS compilers but it has only been tested in VS2003 and VS2005.
 * @remarks The handler returns EXCEPTION_CONTINUE_SEARCH and occasionally EXCEPTION_EXECUTE_HANDLER 
 * but never EXCEPTION_CONTINUE_EXECUTION, because we don't want to override 
 * the windows exception mechanism (merely intercept it). In fact, the main pitfall of VEH 
 * is that under .Net debugging you get a lot of exceptions which the .Net runtime uses 
 * to talk to the debugger. These must get through the VEH code.
 */
static LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS exceptionInfo);

#if defined(_CPPRTTI) && defined(_MSC_VER)
/** Attempts to retrieve the exception type (class name) using RTTI. The following
 * things must be considered:
 * a) It is MSVC only 
 * b) It only works with RTTI enabled (/GR+)
 * c) It uses a trick - pretend the thrown exception object is an instance of a polymorphic
 *    class to make typeid() delay until runtime. 
 * @access public 
 * @qualifier
 * @param exceptionInfo
 * @return wxString
 * @remarks Called internally by the @see VectoredHandler.
 */
wxString GetRTTIExceptionType(const EXCEPTION_POINTERS* const exceptionInfo);
#endif //defined(_CPPRTTI) && defined(_MSC_VER)

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

//////////////////////////////////////////////////////////////////////////
// EXCEPTION MANAGER CLASS
//////////////////////////////////////////////////////////////////////////

/** This singleton class holds and manages information about the exceptions thrown
 * by the application. When running under Windows platforms, it also stores all
 * relevant VEH handler related information, including but not limited to stack trace.
 */
class ExceptionManager
{
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Destructor.*/
    ~ExceptionManager();

    /************************************************************************
     * SINGLETON ACCESS
     ************************************************************************/
    
    /** Returns pointer to singleton (only instance).
     * @access public static 
     * @qualifier none
     * @return ExceptionManager*
     */
    static ExceptionManager* GetInstancePtr() 
    { 
        // Create instance, if not done yet
        if (!m_instance)
        {
            m_instance = new ExceptionManager();
        }
        return m_instance;
    }

    /************************************************************************
     * GETTERS/SETTERS
     ************************************************************************/
    
    /** Indicates if the last thrown exception has been caught and handled 
     * (logged and/or shown to the user).
     * @access public 
     * @qualifier const
     * @return bool true if exception has been processed, false otherwise
     */
    bool IsExceptionProcessed() const { return m_exceptionProcessed; }

    /** Marks the last thrown exception as caught and handled (logged 
     * and/or shown to the user) or not.
     * @access public 
     * @qualifier none 
     * @param val Boolean indicating if the last exception has been processed
     * @return void
     */
    void ExceptionProcessed(bool val) 
    {
        m_exceptionProcessed = val;
        #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES && APPEND_MULTIPLE_STACKS
        if(val)
            // We can now clear the stack trace chain
            // NOTE: We're using Empty() and not Clear() since we'll probably be reusing
            // the array (in the event of other exceptions)
            m_exceptionStackTrace.Empty();
        #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES && APPEND_MULTIPLE_STACKS
    }
   
    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    /** Returns last exception's stack trace. If @see APPEND_MULTIPLE_STACKS is
     * set to 1, the old stack traces (if they exist) are also returned.
     * @access public 
     * @qualifier const
     * @return wxString Last exception's stack trace
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    wxString GetExceptionStackTrace() const 
    {
        #if APPEND_MULTIPLE_STACKS
        // Convert array string holding the various stack traces to a single
        // string
        return StringHelper::ArrayStringToString(m_exceptionStackTrace, true);
        #else
        return m_exceptionStackTrace; 
        #endif // APPEND_MULTIPLE_STACKS
    
    }

    /** Stores the last exception's stack trace. If @see APPEND_MULTIPLE_STACKS is
     * set to 1, the old stack traces are added to the current one.
     * @access public 
     * @qualifier none
     * @param val Last exception's stack trace
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void SetExceptionStackTrace(const wxString& val);
    
    /** Obtains the last exception's time.
     * @access public 
     * @qualifier const
     * @return wxString Last exception's time
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    wxString GetExceptionTime() const { return m_exceptionTime; }

    /** Sets the last exception's time.
     * @access public 
     * @qualifier none
     * @param val Last exception's time
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void SetExceptionTime(wxString val) { m_exceptionTime = val; }

    /** Obtains last exception's code (DWORD).
     * @access public 
     * @qualifier const
     * @return DWORD Last exception's code
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    DWORD GetExceptionCode() const { return m_exceptionCode; }

    /** Stores the last exception's code (DWORD).
     * @access public 
     * @qualifier none
     * @param val Last exception's code
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void SetExceptionCode(DWORD val) { m_exceptionCode = val; }

    /** Obtains last exception's flags (DWORD).
     * @access public 
     * @qualifier const
     * @return DWORD Last exception's flags
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    DWORD GetExceptionFlags() const { return m_exceptionFlags; }

    /** Stores the last exception's flags (DWORD).
     * @access public 
     * @qualifier none
     * @param val Last exception's flags
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void SetExceptionFlags(DWORD val) { m_exceptionFlags = val; }

    /** Indicates if multiple stack traces were detected in 
     * the VEH handler.
     * @access public 
     * @qualifier const
     * @return bool true if multiple stack traces exist, false otherwise
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    bool GetMultipleStacks() const { return m_multipleStacks; }

    /** Mark the existence of multiple stack traces or not.
     * @access public 
     * @qualifier none
     * @param val Boolean indicating if multiple stack traces exist or not
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void SetMultipleStacks(bool val) { m_multipleStacks = val; }

    #if defined(_CPPRTTI) && defined(_MSC_VER)
    /** Returns the exception type (class name), obtained via RTTI.
     * @access public 
     * @qualifier const
     * @return wxString Exception type (class name)
     * @remarks Only relevant on Windows platforms under debug builds if 
     * running Visual studio with RTTI support and if
     * CHECK_STACK_TRACES is set.
     */
    wxString GetExceptionType() const { return m_exceptionType; }

    /** Stores the exception type (class name), obtained via RTTI.
     * @access public 
     * @qualifier none
     * @param val Exception type (class name)
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds if
     * running Visual studio with RTTI support and if
     * CHECK_STACK_TRACES is set.
     */
    void SetExceptionType(wxString val) { m_exceptionType = val; }
    #endif // defined(_CPPRTTI) && defined(_MSC_VER)

    #if APPEND_MULTIPLE_STACKS
    /** Verifies if the maximum number of allowed stack traces has been
     * reached.
     * @access public 
     * @qualifier const
     * @return bool true if stack trace limit has been reaches, false otherwise
     */
    bool HasExceededStackTraces() const { return m_exceededStackTraces; }

    /** Marks the stack trace limit as reached or not.
     * @access public 
     * @qualifier none
     * @param val Boolean which indicates if the stack trace limit has been reached
     * @return void
     */
    void ExceededStackTraces(bool val) { m_exceededStackTraces = val; }
    #endif // APPEND_MULTIPLE_STACKS

    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Retrieves the last caught exception basic info (location, description and exception message),
     * CEGUI exceptions specific info (line & file) and optionally if 
     * running under Windows the VEH info (stack trace, code, type and flags) as
     * a single string. Depending on the defines present in Config.h, some of this info might 
     * or not be processed. It also marks the exception as processed.
     * @access public 
     * @qualifier const
     * @param location Location where the exception was caught
     * @param text Description of the caught exception
     * @param exceptionMessage Message contained in the exception itself (if available)
     * @param exceptionFileName Name of the file where the exception occurred
     * @param exceptionLine Line where the exception occurred
     * @return wxString String containing the built exception info
     */
    wxString GetExceptionInfo(const wxString& location, const wxString& text, const wxString& exceptionMessage=wxT(""), const wxString& exceptionFileName=wxT(""), int exceptionLine=-1);

    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    /** Retrieves the last exception VEH info (stack trace, code, type and flags). Depending
     * on the defines present in Config.h, some of this info might or not be processed.
     * @access public 
     * @qualifier none
     * @return wxString Last exception VEH info
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     * @remarks The @see SHOW_EXCEPTION_CODE, @see SHOW_EXCEPTION_FLAGS and 
     * @see SHOW_EXCEPTION_TYPE defines control which information is retrieved.
     */
    wxString GetExceptionVEHInfo();

    /** Registers the VEH handler in the system.
     * @access public 
     * @qualifier none
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void RegisterExceptionHandler();

    /** Unregisters the VEH handler from the system.
     * @access public 
     * @qualifier none
     * @return void
     * @remarks Only relevant on Windows platforms under debug builds and if
     * CHECK_STACK_TRACES is set.
     */
    void UnRegisterExceptionHandler();
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** The one-and-only instance of the class.*/
    static ExceptionManager *m_instance;

    /** Indicates if the last thrown exception has been caught and processed (logged
     * and/or shown to user).
     */
    bool m_exceptionProcessed;
    
    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    /** Indicates the existence of multiple stack traces 
     */
    bool m_multipleStacks;

    /** Pointer to the VEH handler that will manage all thrown exceptions (CPP & 
     * Win32) under Windows. 
     */
    void *m_handler;

    /** Holds the last exception's stack trace
     * @remarks This is either a normal string if @see APPEND_MULTIPLE_STACKS is set
     * to 0 (single stack trace) or an array string that can hold multiple stack 
     * traces otherwise.
     */
    #if APPEND_MULTIPLE_STACKS
    wxArrayString m_exceptionStackTrace;
    #else
    wxString m_exceptionStackTrace;
    #endif // APPEND_MULTIPLE_STACKS

    #if APPEND_MULTIPLE_STACKS
    /** Indicates if the maximum number of stack traces (@see MAXIMUM_STACK_TRACES)
     * has been reached
     */
    bool m_exceededStackTraces;
    #endif // APPEND_MULTIPLE_STACKS

    /** Holds the last exception's time
     */
    wxString m_exceptionTime;

    /** Holds the last exception's code
     */
    DWORD m_exceptionCode;

    /** Holds the last exception's flags
     */
    DWORD m_exceptionFlags;

    #if defined(_CPPRTTI) && defined(_MSC_VER)
    /** Holds the last exception's type (if available)
     */
    wxString m_exceptionType;
    #endif // defined(_CPPRTTI) && defined(_MSC_VER)
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES   

    /************************************************************************
     * PRIVATE CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor. Singleton, that's why its private.
     * @access private 
     * @qualifier none
     * @return void
     */
    ExceptionManager();

    /** Prevent copies (does nothing).
    * @access private 
    * @qualifier none
    * @param copy
    * @return nothing 
    */
    ExceptionManager(ExceptionManager const &);

    /** Prevent assignments (does nothing).
    * @access private 
    * @qualifier none
    * @param 
    * @return ExceptionManager& Not relevant
    */
    ExceptionManager& operator=(ExceptionManager const &);
};


#endif // _EXCEPTION_MANAGER_H_

