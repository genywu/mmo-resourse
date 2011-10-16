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

#include "Config.h"

// The symbol engine only makes sense on Windows platforms, under debug builds
// and if CHECK_STACK_TRACES is set
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

#include "SimpleSymbolEngine.h"
#include "CELayoutEditor.h"

#include "wx/filename.h"
#include "wx/ffile.h"

#include <windows.h>
#include <excpt.h>
#include <stdexcept>

// CELayoutEditor only supports VS 7.0 or higher, so no need to worry about older dbghelp versions
#include <dbghelp.h>

//////////////////////////////////////////////////////////////////////////
// LINKER OPTIONS
//////////////////////////////////////////////////////////////////////////

// Link the debug help library
#pragma comment(lib, "dbghelp" )

//////////////////////////////////////////////////////////////////////////
// STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////

// Initialize the critical section static variable
wxCriticalSection SimpleSymbolEngine::m_locker;

//////////////////////////////////////////////////////////////////////////
// SIMPLE SYMBOL ENGINE FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
SimpleSymbolEngine& SimpleSymbolEngine::instance()
{
    // Return the symbol engine singleton instance
    static SimpleSymbolEngine theEngine;
    return theEngine;
}

//-----------------------------------------------------------------------
SimpleSymbolEngine::SimpleSymbolEngine()
{
    // Set the symbol options.
    // NOTE: The SYMOPT_DEFERRED_LOADS option can create significant slowdowns when symbols for some modules are loaded  
    // as deferred , since an attempt to search for a nonexistent symbol will cause dbghelp to actually 
    // load symbols for  all such modules (spending time on symbol search, potentially with symbol server access).  
    // There are two workarounds:  
    // 1) Do not use SYMOPT_DEFERRED_LOADS option  
    // 2) Limit the search only to modules whose symbols are already loaded (it is done by  
    // setting SYMOPT_NO_UNQUALIFIED_LOADS option). This is the adopted solution.  
    DWORD dwOpts = SymGetOptions();
    dwOpts |= (SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_NO_UNQUALIFIED_LOADS);
    (void)::SymSetOptions(dwOpts);

    // We'll be using SymFunctionTableAccess64 and SymGetModuleBase64,
    // so we need to call SymInitialize first.
    // NOTE: We don't supply any user search paths, so the library 
    // attempts to form a symbol path from the following sources: 
    //  1) The current working directory of the application 
    //  2) The _NT_SYMBOL_PATH environment variable 
    //  3) The _NT_ALTERNATE_SYMBOL_PATH environment variable    
    (void)::SymInitialize(GetCurrentProcess(), 0, true);
}

//-----------------------------------------------------------------------
SimpleSymbolEngine::~SimpleSymbolEngine()
{
    // Having called SymInitialize, we also need to call SymCleanup.
    (void)::SymCleanup(GetCurrentProcess());
}

//-----------------------------------------------------------------------
wxString SimpleSymbolEngine::AddressToString(PVOID address) const
{
    std::ostringstream result;

    // First the raw address
    result << "0x" << address;

    // Then any name for the symbol
    // NOTE: Despite the fact we don't use the 'nameBuffer' field directly,
    // it is necessary to allocate the necessary memory and prevent stack corruption
    struct tagSymInfo
    {
        IMAGEHLP_SYMBOL64 symInfo;
        char nameBuffer[ 4 * 256 ]; 
    } SymInfo = { { sizeof( IMAGEHLP_SYMBOL64 ) } };

    IMAGEHLP_SYMBOL64* const symbols = &SymInfo.symInfo;
    symbols->MaxNameLength = sizeof( SymInfo ) - offsetof( tagSymInfo, symInfo.Name );

    // Show procedure info (SymGetSymFromAddr64())
    // NOTE: We'll ignore any errors (obtained via ::GetLastError()) thrown by SymGetSymFromAddr64()
    DWORD64 procedureDisplacement;
    if(::SymGetSymFromAddr64(::GetCurrentProcess(), (DWORD64)address, &procedureDisplacement, symbols))
    {
        result << " " << symbols->Name;
        if ( procedureDisplacement != 0 )
            result << "+0x" << std::hex << procedureDisplacement << std::dec;
    }
  
    // Show file/line number info, NT5.0-method (SymGetLineFromAddr64())
    // NOTE: We'll ignore any errors (obtained via ::GetLastError()) thrown by SymGetLineFromAddr64()
    IMAGEHLP_LINE64 lineInfo = { sizeof( IMAGEHLP_LINE64 ) };
    DWORD lineDisplacement;
    if (::SymGetLineFromAddr64(::GetCurrentProcess(), (DWORD64)address, &lineDisplacement, &lineInfo ))
    {
        const char * const delimiter = strrchr( lineInfo.FileName, '\\' );
        result << " at " << ( delimiter ? delimiter + 1 : lineInfo.FileName ) << "(" << lineInfo.LineNumber << ")";
    }
    
    // Convert result to wxString and return it, supporting unicode
    // NOTE: The converter is ignored in non-unicode builds
    return wxString(result.str().c_str(), wxConvUTF8);
}

//-----------------------------------------------------------------------
wxString SimpleSymbolEngine::GetExceptionCodeDescription(const DWORD exceptionCode) const
{
    wxString exceptionCodeDesc;

    // Do we have a CPP exception?
    if (exceptionCode == CPP_EXCEPTION)
        return wxT("CPP_EXCEPTION");

    // It's not a CPP exception, so check what type of SEH Win32 exception we have on our hands
    switch (exceptionCode) 
    {
    case EXCEPTION_ACCESS_VIOLATION: return wxT("EXCEPTION_ACCESS_VIOLATION");
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return wxT("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
    case EXCEPTION_BREAKPOINT: return wxT("EXCEPTION_BREAKPOINT");
    case EXCEPTION_DATATYPE_MISALIGNMENT: return wxT("EXCEPTION_DATATYPE_MISALIGNMENT");
    case EXCEPTION_FLT_DENORMAL_OPERAND: return wxT("EXCEPTION_FLT_DENORMAL_OPERAND");
    case EXCEPTION_FLT_DIVIDE_BY_ZERO: return wxT("EXCEPTION_FLT_DIVIDE_BY_ZERO");
    case EXCEPTION_FLT_INEXACT_RESULT: return wxT("EXCEPTION_FLT_INEXACT_RESULT");
    case EXCEPTION_FLT_INVALID_OPERATION: return wxT("EXCEPTION_FLT_INVALID_OPERATION");
    case EXCEPTION_FLT_OVERFLOW: return wxT("EXCEPTION_FLT_OVERFLOW");
    case EXCEPTION_FLT_STACK_CHECK: return wxT("EXCEPTION_FLT_STACK_CHECK");
    case EXCEPTION_FLT_UNDERFLOW: return wxT("EXCEPTION_FLT_UNDERFLOW");
    case EXCEPTION_GUARD_PAGE: return wxT("EXCEPTION_GUARD_PAGE");
    case EXCEPTION_ILLEGAL_INSTRUCTION: return wxT("EXCEPTION_ILLEGAL_INSTRUCTION");
    case EXCEPTION_IN_PAGE_ERROR: return wxT("EXCEPTION_IN_PAGE_ERROR");
    case EXCEPTION_INT_DIVIDE_BY_ZERO: return wxT("EXCEPTION_INT_DIVIDE_BY_ZERO");
    case EXCEPTION_INT_OVERFLOW: return wxT("EXCEPTION_INT_OVERFLOW");
    case EXCEPTION_INVALID_DISPOSITION: return wxT("EXCEPTION_INVALID_DISPOSITION");
    case EXCEPTION_INVALID_HANDLE: return wxT("EXCEPTION_INVALID_HANDLE");
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return wxT("EXCEPTION_NONCONTINUABLE_EXCEPTION");
    case EXCEPTION_PRIV_INSTRUCTION: return wxT("EXCEPTION_PRIV_INSTRUCTION");
    case EXCEPTION_SINGLE_STEP: return wxT("EXCEPTION_SINGLE_STEP");
    case EXCEPTION_STACK_OVERFLOW: return wxT("EXCEPTION_STACK_OVERFLOW");
    default: return wxT("UNKNOWN");
    }
}

//-----------------------------------------------------------------------
wxString SimpleSymbolEngine::GetExceptionFlagsDescription(const DWORD exceptionFlags) const
{
    wxString exceptionFlagsDesc;

    // Verify which exception flags are active
    if (exceptionFlags == EXCEPTION_CONTINUABLE)  // Special case, since the flag is 0
        (void)exceptionFlagsDesc.Append(wxT("CONTINUABLE "));
    else
    {
        if (exceptionFlags & EXCEPTION_NONCONTINUABLE)
            (void)exceptionFlagsDesc.Append(wxT("NONCONTINUABLE "));
        if (exceptionFlags & EXCEPTION_UNWINDING)
            (void)exceptionFlagsDesc.Append(wxT("UNWINDING "));
        if (exceptionFlags & EXCEPTION_EXIT_UNWIND)
            (void)exceptionFlagsDesc.Append(wxT("EXIT_UNWIND "));
        if (exceptionFlags & EXCEPTION_STACK_INVALID)
            (void)exceptionFlagsDesc.Append(wxT("STACK_INVALID "));
        if (exceptionFlags & EXCEPTION_NESTED_CALL)
            (void)exceptionFlagsDesc.Append(wxT("NESTED_CALL "));
        if (exceptionFlags & EXCEPTION_TARGET_UNWIND)
            (void)exceptionFlagsDesc.Append(wxT("TARGET_UNWIND "));
        if (exceptionFlags & EXCEPTION_COLLIDED_UNWIND)
            (void)exceptionFlagsDesc.Append(wxT("COLLIDED_UNWIND "));
    }

    // Return exception flags description
    return exceptionFlagsDesc;
}

//-----------------------------------------------------------------------
// Turn off optimizations to make sure that frame pointer is present 
#pragma optimize ( "", off )
wxString SimpleSymbolEngine::StackTrace(PCONTEXT context, const DWORD exceptionCode, const DWORD exceptionFlags, const wxString &exceptionType) const
{
    // Lock the function, thus preventing simultaneous accesses (thread-safe)
    // NOTE: A wxCriticalSectionLocker enters the critical section in the 
    // constructor and leaves it in the destructor (automatically). Therefore,
    // there is no need to manually free it since it will be destroyed
    // at the end of the function call.
    wxCriticalSectionLocker locker(m_locker);

    wxString result;

    // Make sure we have a valid context
    // NOTE: It is possible to capture the context if none is supplied, but this
    // is not supported at the moment
    if(!context)
    {
        LogSilentError(wxT("SimpleSymbolEngine::StackTrace - Stack Trace not obtained : invalid context record supplied!"));
        return wxT("");
    }

    // Create a zero-initialized STACKFRAME64 (our in/out stackframe) object.
    // NOTE: Functionally equivalent to:
    // STACKFRAME64 stackFrame;
    // memset(&stackFrame, 0, sizeof(stackFrame));
    STACKFRAME64 stackFrame = {0};

    // Will store the machine type
    DWORD machineType;

    // STACKFRAME64 structure initialization
    // NOTE: According to the Microsoft dbghelp-team, your code should:
    // 1) Always use StackWalk64 
    // 2) Always set AddrPC to the current instruction pointer (Eip on x86, Rip on x64 and StIIP on IA64) 
    // 3) Always set AddrStack to the current stack pointer (Esp on x86, Rsp on x64 and IntSp on IA64) 
    // 4) Set AddrFrame to the current frame pointer when meaningful. On x86 this is Ebp, on 
    //    x64 you can use Rbp (but is not used by VC2005B2; instead it uses Rdi!) and on IA64 you can use RsBSP. 
    //    StackWalk64 will ignore the value when it isn't needed for unwinding. 
    // 5) Set AddrBStore to RsBSP for IA64. 
    // NOTE2: In the past, you could get away with only initializing AddrPC and AddrFrame, but starting as of
    // dbhhelp.dll (v5.6.3.7) you MUST also initialize the AddrStack.
    #ifdef _M_IX86 
    machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = (DWORD64)context->Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = (DWORD64)context->Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = (DWORD64)context->Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    #elif _M_X64
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = (DWORD64)context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = (DWORD64)context->Rsp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = (DWORD64)context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    #elif _M_IA64
    machineType = IMAGE_FILE_MACHINE_IA64;
    stackFrame.AddrPC.Offset = (DWORD64)context->StIIP;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = (DWORD64)context->IntSp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrBStore.Offset = (DWORD64)context->RsBSP;
    stackFrame.AddrBStore.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = (DWORD64)context->IntSp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    #else
        #error "This architecture is not supported."
    #endif //_M_IX86

    // Add code/flags/type info (if relevant)
    #if SHOW_EXCEPTION_CODE
        (void)result.Append(wxString::Format(wxT("  Exception Code: %s\n"), GetExceptionCodeDescription(exceptionCode)));
    #endif // SHOW_EXCEPTION_CODE
    #if SHOW_EXCEPTION_FLAGS
        (void)result.Append(wxString::Format(wxT("  Exception Flags: %s\n"), GetExceptionFlagsDescription(exceptionFlags)));
    #endif // SHOW_EXCEPTION_FLAGS
    #if defined (_CPPRTTI) && SHOW_EXCEPTION_TYPE 
        (void)result.Append(wxString::Format(wxT("  Exception Type: %s\n"), exceptionType));
    #endif // defined (_CPPRTTI) && SHOW_EXCEPTION_TYPE

    // Obtain stack trace timestamp
    // FIXME: Create util func : GetTimeStamp()
    const wxDateTime dt = wxDateTime::Now();
    (void)result.Append(wxT("  Captured at: ") + dt.FormatISODate() + wxT(' ') + dt.FormatISOTime() + wxT("\n"));

    // Add header
    (void)result.Append(wxT("  Frame          Code address\n"));

    // Walk the stack
    // NOTE: Starting with Win9x/W2K, this functionality is present in dbghelp.dll library (on NT, it is in 
    // imagehlp.dll) which is linked above. But the function name (StackWalk64) has changed 
    // starting with W2K (before it was called StackWalk -- without the 64). We're using the newer version 
    // 'StackWalk64' instead of 'StackWalk', which despite the name works with both 64 and 32-bit applications.
    // NOTE2: If you're running an older version of Windows, you can still use the function if you get your 
    // hands on the dbghelp.dll redistributable. The latest dbghelp.dll can always be downloaded 
    // with the 'Debugging Tools for Windows'.
    while ( ::StackWalk64(
        machineType,                // Machine architecture type 
        ::GetCurrentProcess(),      // Process handle 
        ::GetCurrentThread(),       // Thread handle -- this value doesn't matter much if previous one is a real handle
        &stackFrame,                // Stack frame 
        context,                    // Thread context (not required for x86)
        NULL,                       // Read memory function - not used 
        ::SymFunctionTableAccess64, // Function table access function (FPO access on x86)
        ::SymGetModuleBase64,       // Function that can determine module base for the given address 
        NULL ) )                    // Address translation function - not used 
    {
        // We'll start by avoiding endless call stack loops -- This should be a *very* rare occurrence and a result of a corrupt
        // stack frame.
        if (stackFrame.AddrPC.Offset == stackFrame.AddrReturn.Offset)
        {
            LogSilentError(wxT("SimpleSymbolEngine::StackTrace - Endless call stack loop detected!"));
            return wxT("");
        }
        // Then, let's make sure the Program Counter (PC) is valid
        // NOTE: The program counter, PC, is a special-purpose register that is used by the processor to hold the address of the 
        // next instruction to be executed.
        if (stackFrame.AddrPC.Offset != 0)
        {
            // We seem to have a valid PC, so construct stack information
            const UINT_PTR pointerPC = static_cast<UINT_PTR>(stackFrame.AddrPC.Offset);
            const UINT_PTR pointerFrame = static_cast<UINT_PTR>(stackFrame.AddrFrame.Offset);
            const wxString addrFrame = wxString::Format(wxT("  0x%.8X"), reinterpret_cast<PVOID>(pointerFrame));
            (void)result.Append(addrFrame).Append(wxT("  ")).Append(AddressToString(reinterpret_cast<PVOID>(pointerPC))).Append(wxT("\n"));
        }
        else
        {
            // Not a good scenario
            LogSilentError(wxT("SimpleSymbolEngine::StackTrace - Invalid Program Counter (PC) register detected!"));
            return wxT("");
        }
    }

    // We've successfully retrieved the stack trace
    return result;
}
// Turn the optimizations on again
#pragma optimize ( "", on )


#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

