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
#include "ExceptionManager.h"
#include "CELayoutEditor.h"
#include "SimpleSymbolEngine.h"

//////////////////////////////////////////////////////////////////////////
// EXCEPTION MANAGER GLOBALS
//////////////////////////////////////////////////////////////////////////

/** Declare the instance.*/
ExceptionManager* ExceptionManager::m_instance = NULL;

//////////////////////////////////////////////////////////////////////////
// VEH HANDLER
//////////////////////////////////////////////////////////////////////////

// Vectored Exception Handling (VEH) is only supported on Windows platforms. 
// NOTE: Therefore, obtaining the stack trace of any given exception is only possible
// if running under Windows (at the moment). Plans are being made for linux
// support.
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
//-----------------------------------------------------------------------
static LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS exceptionInfo) 
{
    // Validations
    wxASSERT_MSG(exceptionInfo != NULL, wxT("ExceptionInfo parameter is NULL"));

    // Obtain exception code
    const DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;

    // If the exception was raised explicitly by ::OutputDebugStringA, transfer control 
    // to the appropriate handler (thus ignoring the exception).
    // IMPORTANT NOTE: An example of one such exception is whenever you call ::OutputDebugString* 
    // family of functions (which is invoked internally by wxWidgets when you use wxLogDebug(), 
    // wxLogTrace(), wxVLogDebug() and wxVLogTrace(), the latter two which happen to be used by 
    // LogDebug() and LogTrace()). In Unicode-versions (which is the one that interest us since 
    // CELayoutEditor is built in Unicode), ::OutputDebugStringW() converts Unicode characters 
    // to ANSI and calls ::OutputDebugStringA(). ::OutputDebugStringA() registers its own 
    // exception handler and raises an exception with code 0x40010006UL via RaiseException(). 
    // This is done to communicate with the currently active debugger and to output the supplied 
    // text to the debug console. If there is a registered system debugger (i.e. one running at 
    // the moment), it handles the exception (and "eats" it, thus never triggering this VEH 
    // handler). Otherwise, the OutputDebugString()'s own handler is invoked (and this VEH 
    // handler is called and we must ignore the call). 
    //
    // For those wishing to know the internals, it does the following: attempts to open 
    // 'DBWIN_DATA_READY' and 'DBWIN_BUFFER_READY' named events, then named file mapping 
    // 'DBWIN_BUFFER'. If any of these operations fails, the message is dropped. In other 
    // words ::OutputDebugString() works by raising a structured exception 0x40010006UL, passing 
    // the length of the string and the string as exception parameters. Windows catches this 
    // exception and makes this data available through the shared memory-mapped file 
    // DBWIN_BUFFER. Because this buffer is shared, it must be protected from multi-threaded 
    // access (i.e. access to it must be synchronized, which basically means that only one 
    // thread  must be able to write to the memory-mapped file at any one time). This is done 
    // using two named Win32 events: One is used to indicate that a monitor is reading the 
    // memory and hence prevents writing to it (DBWIN_BUFFER_READY), and the other is used by 
    // the system to indicate that a new message has been written to the shared memory 
    // (DBWIN_DATA_READY). A thread that calls ::OutputDebugString() is blocked until the 
    // DBWIN_BUFFER_READY event is signaled, this means that the monitor process is coupled to 
    // the process generating the messages, and a poorly written monitor process can perform 
    // unwanted synchronization on the process generating messages. In other words, 
    // ::OutputDebugString couples your process to another process that will read the 
    // debug string. This is bad news, so you should not generate ::OutputDebugString messages 
    // in release code. Again, this means that you should only generate trace messages through 
    // the Debug class in debug builds.  
    // More info at 'Adding Diagnostics to .NET Code' (http://www.ddj.com/windows/184406003) and 
    // 'Tracing Code: Part 2' (http://www.ddj.com/windows/184416750) by Richard Grimes.
    // NOTE2: This is an informational exception (ERROR_SEVERITY_INFORMATIONAL), but
    // one that is expected and can be handled safely. Therefore, this is a special case, and
    // the remainder informational exceptions will be handled differently just below.
    // NOTE3: The reason why you *don't* see exception 0x40010006UL when running 
    // under a debugger is that the O/S maps the 'first-chance' exception event into 
    // an OUTPUT_DEBUG_STRING_EVENT - the RaiseException never makes it back into 
    // user-code in the process being debugged.
    if(exceptionCode == OUTPUT_DEBUG_STRING_EXCEPTION)
        return EXCEPTION_EXECUTE_HANDLER;

    // Obtain exception severity (error, warning, informational or success)
    // NOTE: Success should never occur.
    const DWORD severity = exceptionCode & ERROR_SEVERITY_MASK; 

    // We're only interested in errors and warnings. Informational 
    // messages should be ignored since that are usually raised
    // explicitly by the program (via RaiseException) and aren't harmful
    // to the program's execution.
    // NOTE: When EXCEPTION_CONTINUE_EXECUTION is returned, the system 
    // attempts to restart execution of the process. Vectored exception 
    // handlers that appear later in the list won't be called, nor will 
    // any of the structured exception handlers. When the function 
    // returns EXCEPTION_CONTINUE_SEARCH (which is our case), the system moves on 
    // to the next vectored exception handler. After all vectored 
    // exception handlers have been called, the system starts 
    // with the structured exception handling list.
    if((severity != ERROR_SEVERITY_ERROR) && (severity != ERROR_SEVERITY_WARNING))
        return EXCEPTION_CONTINUE_SEARCH;

    // DEBUG info
    LogSilentTrace(wxT("VectoredHandler - Entered VEH handler"));  

    // Obtain exception flags
    const DWORD exceptionFlags = exceptionInfo->ExceptionRecord->ExceptionFlags;

    // Check if we're in the middle of an unwinding operation. If so, we ignore the 
    // exception (since it was already processed).
    // NOTE: Basically the exception system follows the chain of handlers once to find the 
    // handler that will deal with the exception.  Then this handler runs the chain again, 
    // with the 'unwind' bit set to give handlers a chance to do their own unwinding (in 
    // the C++ case this involves calling destructors). 
    if (IS_DISPATCHING(exceptionFlags))
    {
        // Get exception manager, the singleton which stores and manages all exception related info
        ExceptionManager* const exceptionManager = ExceptionManager::GetInstancePtr();

        // Store exception flags for later use (inside the try/catch block)
        exceptionManager->SetExceptionFlags(exceptionInfo->ExceptionRecord->ExceptionFlags);

        // Check if we have a Win32 exception (fatal) or a normal CPP exception
        const bool fatalException = ((exceptionCode & 0xf0000000) == 0xc0000000);

        // Store exception code
        exceptionManager->SetExceptionCode(exceptionCode);

        // Obtain and store exception type using RTTI (if available)
        wxString exceptionType;
        #if defined(_CPPRTTI) && defined(_MSC_VER)
        exceptionManager->SetExceptionType(GetRTTIExceptionType(exceptionInfo));
        exceptionType = exceptionManager->GetExceptionType();
        #endif // defined(_CPPRTTI) && defined(_MSC_VER)

        // Store exception timestamp
        // FIXME: Create util func : GetTimeStamp()
        const wxDateTime dt = wxDateTime::Now();
        exceptionManager->SetExceptionTime(dt.FormatISODate() + wxT(' ') + dt.FormatISOTime()); 

        // Obtain exception stack trace
        wxString stackTrace = SimpleSymbolEngine::instance().StackTrace(exceptionInfo->ContextRecord, exceptionCode, exceptionFlags, exceptionType);

        // Did we manage to retrieve the stack trace?
        if(stackTrace.IsEmpty())
            // Failed to get stack trace
            stackTrace = wxT("Failed to obtain stack trace -- Check log for details");
        else
        {
            // Stack trace obtained correctly, so let's check if there are already any 
            // unprocessed stack traces, which implies multiple stacks 
            // NOTE: Multiple stack traces are the result of one or more of the following 
            // three things:
            //       - "Eaten" exceptions (caught but not processed/logged exceptions);
            //       - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);
            //       - Nested exceptions (the exception is caught and a new one is thrown).
            // NOTE2: Please note that "eaten" exceptions might manifest themselves much 
            // later than when they first appear since they can only be detected when a second 
            // exception is thrown (or at program shutdown). That is why they receive special 
            // treatment at program's exit (see PrepareForExit).
            #if CHECK_MULTIPLE_STACKS
            if(!exceptionManager->IsExceptionProcessed())
                // Multiple stacks have been detected
                exceptionManager->SetMultipleStacks(true);
            #endif // CHECK_MULTIPLE_STACKS
        }

        // Store exception stack trace and mark it as needing processing (in the try/catch block)
        exceptionManager->SetExceptionStackTrace(stackTrace);
        exceptionManager->ExceptionProcessed(false);

        // Do we have a fatal exception?
        if (fatalException)
        {
            // Add Win32 fatal exception to log
            // NOTE: Fatal exception always terminate the application and are not caught in
            // try/catch blocks, so we need to add the exception to log right here
            // NOTE2: A fatal exception was detected and while you could theoretically display a message
            // to the user, it is best to avoid such practice and only log silently
            LogSilentError(wxT("VectoredHandler - Fatal Win32 exception detected:%s"), exceptionManager->GetExceptionVEHInfo());
        }
    }

    // Pass control to next exception handler
    // NOTE: When EXCEPTION_CONTINUE_EXECUTION is returned, the system 
    // attempts to restart execution of the process. Vectored exception 
    // handlers that appear later in the list won't be called, nor will 
    // any of the structured exception handlers. When the function 
    // returns EXCEPTION_CONTINUE_SEARCH (which is our case), the system moves on 
    // to the next vectored exception handler. After all vectored 
    // exception handlers have been called, the system starts 
    // with the structured exception handling list.
    return EXCEPTION_CONTINUE_SEARCH;
}

//-----------------------------------------------------------------------
#if defined(_CPPRTTI) && defined(_MSC_VER)
wxString GetRTTIExceptionType(const EXCEPTION_POINTERS* const exceptionInfo)
{
    // Validations
    wxASSERT_MSG(exceptionInfo != NULL, wxT("ExceptionInfo parameter is NULL"));

    // Try and get proper class name if the object has got RTTI information. However, we can 
    // only obtain the exception type if we have a CPP exception and a not a SEH one
    if (exceptionInfo->ExceptionRecord->ExceptionCode == CPP_EXCEPTION) 
    {
        // Get exception object address
        // NOTE: This is an horrid hack to try and get thrown type. It basically employs the following trick - 
        // pretend the thrown object is an instance of a polymorphic class to make typeid() delay until runtime
        const DummyPolymorphic* const exceptionAddress = (DummyPolymorphic *)exceptionInfo->ExceptionRecord->ExceptionInformation[1]; 

        // Did we manage to obtain the exception address?           
        if(exceptionAddress != 0)
        {
            // Yes, so obtain exception type using RTTI (using our dirty trick)
            const type_info& exceptionType = typeid(*exceptionAddress);

            // Obtain exception name
            const char* const exceptionName = exceptionType.name();

            // Convert result to wxString and return it, supporting unicode
            // NOTE: The converter is ignored in non-unicode builds
            return wxString(exceptionName, wxConvUTF8);
        }
        // Check if the exception was re-thrown
        // NOTE: Microsoft implemented "throw;" by raising an exception with
        // 0 for the address of the object and for the address of the type information.
        // The C runtime code then looks in the internal per-thread data structure to
        // find the 'current exception' (_curexception).
        else if(exceptionInfo->ExceptionRecord->ExceptionInformation[2] == 0)
        {
            // Exception was re-thrown
            // NOTE: RTTI info doesn't work when you re-throw an exception (i.e. throw;) as
            // the exception address will be empty (NULL)
            LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (exception was re-thrown)"));
            return wxT("Unknown (exception was re-thrown - check prior stack traces for type)");
        }
        else
        {
            // Invalid or empty exception address
            LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (exceptionAddress is empty or invalid)"));
            return wxT("Unknown (exceptionAddress is empty or invalid)");
        }
    }
    else
    {
        // Non CPP Exception - SEH Win32 exception
        LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (SEH Win32 Exception)"));
        return wxT("Unknown (SEH Win32 Exception)");
    }
}
#endif // defined(_CPPRTTI) && defined(_MSC_VER)

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

//////////////////////////////////////////////////////////////////////////
// EXCEPTION MANAGER FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
ExceptionManager::ExceptionManager() :
m_exceptionProcessed(true) // This starts as true
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
,m_multipleStacks(false),
m_handler(NULL),
m_exceptionStackTrace(),
#if APPEND_MULTIPLE_STACKS
m_exceededStackTraces(false),
#endif // APPEND_MULTIPLE_STACKS
m_exceptionTime(),
m_exceptionCode(0UL),
m_exceptionFlags(0UL)
#if defined(_CPPRTTI) && defined(_MSC_VER)
,m_exceptionType()
#endif // defined(_CPPRTTI) && defined(_MSC_VER)
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Empty at the moment 
}

//------------------------------------------------------------------------
ExceptionManager::~ExceptionManager()
{
    // Swallow any exceptions found to prevent the destructor from emitting them.
    // NOTE: An exception should never leave a destructor as it may yield
    // undefined behavior or even lead to program abnormal termination 
    // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
    try
    {
        #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        // Remove exception VEH handler
        UnRegisterExceptionHandler();

        #if APPEND_MULTIPLE_STACKS
        // Free memory allocated by the elements in the array
        m_exceptionStackTrace.Clear();
        #endif // APPEND_MULTIPLE_STACKS
        #endif //defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

    }
    // Add exception to log
    LOG_EXCEPTION_SAFE("ExceptionManager::~ExceptionManager", "Failed to destroy exception manager", "Failed to destroy exception manager", "Failed to destroy exception manager");    

}

//-----------------------------------------------------------------------
wxString ExceptionManager::GetExceptionInfo(const wxString& location, const wxString& text, const wxString& exceptionMessage, const wxString& exceptionFileName, int exceptionLine)
{
    // Build exception basic info (location/Descriptive text)
    wxString exceptionInfo = wxString::Format(wxT("CELayoutEditor has detected and caught an exception. The following detailed info is available:\n\nDescription: %s\nException Location: %s"), text.c_str(), location.c_str());

    // Check if we are to add the exception message, exception filename and exception line
    // NOTE: The last two are only relevant for CEGUI exceptions
    if(!exceptionMessage.IsEmpty())
        (void)exceptionInfo.Append(wxString::Format(wxT("\nException Message: %s"), exceptionMessage.c_str()));
    if(!exceptionFileName.IsEmpty())
        (void)exceptionInfo.Append(wxString::Format(wxT("\nException Filename: %s"), exceptionFileName.c_str()));
    if(exceptionLine != -1)
        (void)exceptionInfo.Append(wxString::Format(wxT("\nException Line: %d"), exceptionLine));

    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    // Add VEH info (stack/code/type/flags) if running on windows platform under debug builds
    (void)exceptionInfo.Append(GetExceptionVEHInfo());

    // Reset multiple stack traces info
    m_multipleStacks = false;
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

    // Mark the exception as processed
    ExceptionProcessed(true);
    
    // Return exception info
    return exceptionInfo;
}

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
//-----------------------------------------------------------------------
wxString ExceptionManager::GetExceptionVEHInfo()
{
    wxString exceptionInfo;

    #if CHECK_MULTIPLE_STACKS
    // Do we have multiple stack traces?
    if(GetMultipleStacks())
    {
        // Yes, so add warning to inform user 
        // NOTE: Depending on the value of the APPEND_MULTIPLE_STACKS define, all the stack
        // traces will be displayed (more recent first) (1), or only the more recent one (0)
        #if APPEND_MULTIPLE_STACKS
        (void)exceptionInfo.Append(wxT("\n\nWARNING: Multiple stacks detected. This is the result of one or more of the following three things:\n   - \"Eaten\" exceptions (caught but not processed/logged exceptions);\n   - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);\n   - Nested exceptions (the exception is caught and a new one is thrown).\nAll stack traces will be displayed (more recent first)."));

        // Have we reached the stack trace limit?
        if(m_exceededStackTraces)
        {
            // Inform the user as well and reset the limit
            (void)exceptionInfo.Append(wxString::Format(wxT("\n\nWARNING: Maximum stack trace (%d) exceeded! Oldest stack traces will be discarted!"), MAXIMUM_STACK_TRACES));        
            m_exceededStackTraces = false;
        }
        #else
        (void)exceptionInfo.Append(wxT("\n\nMultiple stacks detected. This is the result of one or more of the following three things:\n   - \"Eaten\" exceptions (caught but not processed/logged exceptions);\n   - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);\n   - Nested exceptions (the exception is caught and a new one is thrown).\nOnly the most recent stack trace is displayed (the others are discarted)."));
        #endif //APPEND_MULTIPLE_STACKS
    }
    #endif //CHECK_MULTIPLE_STACKS

    // Add stack trace information
    (void)exceptionInfo.Append(wxString::Format(wxT("\n\nException Stack Trace (%s):\n\n%s"), m_exceptionTime, GetExceptionStackTrace()));

    return exceptionInfo;
}

//-----------------------------------------------------------------------
void ExceptionManager::RegisterExceptionHandler()
{
    // Register VEH exception handler that will be responsible for obtaining
    // the thrown exceptions (both CPP & win32 fatal ones) stack traces, codes and flags
    // NOTE: Only applicable on windows platforms
    m_handler = ::AddVectoredExceptionHandler(0, &VectoredHandler);
}

//-----------------------------------------------------------------------
void ExceptionManager::UnRegisterExceptionHandler()
{
    // Windows exception handling specific cleanup
    if (m_handler != NULL)
    {
        // Unregister VEH exception handler
        // NOTE: We won't delete the handler ourselves since ::RemoveVectoredExceptionHandler()
        // will take care of this for us
        if(0 == ::RemoveVectoredExceptionHandler(m_handler))
            LogSilentError(wxT("CELayoutEditor::PrepareForExit - Failed to remove vectored exception handler!"));        
        m_handler = NULL;
    }
}

//-----------------------------------------------------------------------
void ExceptionManager::SetExceptionStackTrace(const wxString& val) 
{ 
    // If we support multiple stacks, we need to add the old ones to the more
    // recent one. Otherwise, the application only retains the newest.
    #if APPEND_MULTIPLE_STACKS
    // Have we reached the stack trace limit?
    if(m_exceptionStackTrace.GetCount() == MAXIMUM_STACK_TRACES)
    {
        // Yes, so we need to remove the oldest stack trace and mark the limit as reached
        // NOTE: The limit prevents "exception number overload" in external code
        m_exceptionStackTrace.RemoveAt(m_exceptionStackTrace.GetCount()-1);
        m_exceededStackTraces = true;
        LogSilentWarning(wxString::Format(wxT("SetExceptionStackTrace - Maximum stack trace (%d) exceeded! Oldest stack traces will be discarted!"), MAXIMUM_STACK_TRACES));
    }

    // Add new stack trace at the front of the array
    // NOTE: Newest stack traces always appear first
    m_exceptionStackTrace.Insert(val, 0);
    #else
    // Retain the newest stack trace
    m_exceptionStackTrace = val; 
    #endif // APPEND_MULTIPLE_STACKS
}

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES