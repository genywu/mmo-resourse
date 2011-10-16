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

#ifndef _CONFIG_H_
#define _CONFIG_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Version.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * EXCEPTION HANDLING
 ************************************************************************/

/** Exit application when an unhandled exception is discovered? When set to 1, the first
 * unhandled exception will cause CELayoutEditor to exit immediately. This is only relevant
 * in debug builds, as the application ALWAYS terminates in release mode when an unhandled
 * exception is found. If set to 0, the application will continue running after each unhandled
 * exception.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (this prevents unexpected behavior if the unhandled exception
 * is caused continuously - see remarks)
 * @remarks Be careful if setting this to zero as log might grow quite big and you
 * might get an infinite supply of error message boxes (that require termination via
 * the task manager in Windows) if SHOW_UNHANDLED is also defined. Only remove
 * termination if testing for a specific problem.
 */ 
#define TERMINATE_ON_UNHANDLED 1

/** Display unhandled exceptions to user? When set to 1, all unhandled exceptions will be
 * logged and shown to the user via a message box. Otherwise, they will only be logged.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (this notifies of any unexpected problems in the application)
 * @remarks Be careful if this is set alongside with TERMINATE_ON_UNHANDLED. See remarks
 * on the latter for more info.
 */ 
#define SHOW_UNHANDLED 1

/** Prevent the user from saving the file when an unhandled exception is detected and 
 * the application is about to close? If set to 1, whenever an unhandled exception is
 * causing the program to exit and if the current document has been modified, the user
 * will be asked if he wants to save the file or not. This is only relevant if
 * TERMINATE_ON_UNHANDLED is set to 1 as well. If set to 0, the file is not saved.
 * 
 * Default is 0
 *
 * Recommended setting: It depends. Some unhandled exceptions might create an unstable
 * state in the system (rare occurrence) that can corrupt the file save (thus ruining 
 * the layout file). On the other hand, not saving the file will make the user lose 
 * all the work he has done on the file since the last save.
 */
#define PREVENT_SAVE_ON_UNHANDLED 0

/** Intercept all thrown exceptions (CPP & fatal Win32) and process stack trace info? 
 * If set to 1, whenever an exception is thrown, the VEH handler will obtain its
 * stack trace, which will then be shown to the user and logged. If set to 0, no
 * VEH handler is registered and the exception stack traces will be ignored.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (if running on Windows since stack traces can help the debug
 * process), irrelevant otherwise.
 * @remarks Only valid on Windows platforms (i.e. WIN32 define must be set) and under
 * debug builds (i.e. _DEBUG define).
 */
#define CHECK_STACK_TRACES 1

/** Display exception flags when logging or showing exception info to the user? 
 * If set to 1, whenever an exception is thrown, the exception flags will be displayed
 * alongside with the stack trace. Otherwise, the exception flags will not be shown.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (more exception info is always a good thing), irrelevant if not running
 * on Windows.
 * @remarks Only relevant if @see CHECK_STACK_TRACES is set to 1.
 */
#define SHOW_EXCEPTION_FLAGS 1

/** Display exception code when logging or showing exception info to the user? 
* If set to 1, whenever an exception is thrown, the exception code will be displayed
* alongside with the stack trace. Otherwise, the exception code will not be shown.
* 
* Default is 1
*
* Recommended setting: 1 (more exception info is always a good thing), irrelevant if not running
* on Windows.
* @remarks Only relevant if @see CHECK_STACK_TRACES is set to 1.
*/
#define SHOW_EXCEPTION_CODE 1

/** Display exception type when logging or showing exception info to the user? 
* If set to 1, whenever an exception is thrown, the exception type (if available) 
* will be displayed alongside with the stack trace. Otherwise, the 
* exception type will not be shown.
* 
* Default is 1
*
* Recommended setting: 1 (more exception info is always a good thing), irrelevant if not running
* on Windows.
* @remarks Only relevant if @see CHECK_STACK_TRACES is set to 1 and if
* the project and CEGUI are built with RTTI information (_CPPRTTI define) under
* Visual Studio (_MSC_VER define).
*/
#define SHOW_EXCEPTION_TYPE 1

/** Verify at program's exit if there are any "eaten" exceptions (caught in 
 * try/catch blocks but not processed)? "Eaten" exceptions might manifest
 * themselves much latter that when they are first thrown since they can only
 * be detected when a second caught exception is thrown or at program's exit. This option
 * takes care of the later scenario. If set to 1, the program will verify 
 * at shutdown if there are any unprocessed exceptions and log the fact
 * if so. If set to 0, no such verification is made.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (this might help detect unusual situations), irrelevant if not running
 * on Windows.
 * @remarks Only relevant if @see CHECK_STACK_TRACES is set to 1, since
 * we can only detect "eaten" exception via the VEH handler.
 */
#define CHECK_EATEN_EXCEPTIONS 1

/** Verify the presence of multiple stacks, which imply multiple calls to the
 * VEH handler before the exception is processed in a try/catch block? This situation 
 * is the result of one or more of the following three things:
 *      - "Eaten" exceptions (caught but not processed/logged exceptions);
 *      - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);
 *      - Nested exceptions (the exception is caught and a new one is thrown).
 * If set to 1, whenever an exception is thrown, the VEH handler will start by checking if a previously
 * unprocessed exception stack trace exists and if so marks the existence of multiple stacks. Depending
 * on the value of @see APPEND_MULTIPLE_STACKS, all of the stack traces will be logged or only the more
 * recent one. A warning will be issued in both cases. If set to 0, no verification is made and 
 * only the more recent stack trace is contemplated with no warning whatsoever.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (this provides more accurate exception handling), irrelevant if not running
 * on Windows.
 * @remarks Only relevant if @see CHECK_STACK_TRACES is set to 1.
 */
#define CHECK_MULTIPLE_STACKS 1

/** When multiple stack traces are detected, take in consideration the existing one(s)
 * and append the new one to the result? If set to 1, the stack traces will 
 * be joined together (more recent first). This might increase the size of the log
 * file considerably. If set to 0, only the more recent stack trace will be 
 * considered. A warning will be issued about the existence of multiple stack 
 * traces in both situations.
 *
 * Default is 1
 *
 * Recommended setting: 1 (in normal scenarios), 0 (if log physical disk space is a concern), 
 * irrelevant if not running on Windows.
 * @remarks Only relevant if @see CHECK_STACK_TRACES and @see CHECK_MULTIPLE_STACKS are set to 1.
 */
#define APPEND_MULTIPLE_STACKS 1

/** Holds the maximum number of stack traces stored by the VEH handler while a try/catch block
 * isn't reached. We need to limit the number of retained stack traces, because we 
 * never know what might happen in external code, where the number of re-thrown 
 * or "eaten" exceptions might reach astounding levels. By using a reasonable number here, we 
 * can be sure that the log file isn't overwhelmed no matter what.
 *
 * Default is 6
 *
 * Recommended setting: below 10 (in normal scenarios), any value you want (if you are
 * debugging a specific problem that generates many exceptions in one go).
 * @remarks Only relevant if @see CHECK_STACK_TRACES, @see CHECK_MULTIPLE_STACKS 
 * and @see APPEND_MULTIPLE_STACKS are set to 1.
 */
#define MAXIMUM_STACK_TRACES 6

// This was conditionalised due to DWORD not being a portable type.
#if defined (__WINDOWS__)
// Flag set whenever a CPP exception is thrown (present in the exception code)
const DWORD CPP_EXCEPTION    = 0xE06D7363UL;

// Indicates an exception that was explicitly raised (via RaiseException()) in 
// ::OutputDebugStringA() (used internally by wxWidgets functions wxLogDebug(), wxLogTrace(), 
// wxVLogDebug() and wxVLogTrace())
// NOTE: @see VectoredHandler for more details.
const DWORD OUTPUT_DEBUG_STRING_EXCEPTION = 0x40010006UL;

// This mask can be used to retrieve the thrown exception code severity (error, warning, informational or success)
const DWORD ERROR_SEVERITY_MASK = ( ERROR_SEVERITY_ERROR | ERROR_SEVERITY_WARNING | ERROR_SEVERITY_INFORMATIONAL | ERROR_SEVERITY_SUCCESS );
#endif

// VEH handler flags settings
// NOTE: Only EXCEPTION_NONCONTINUABLE in defined in WinNt.h
#define EXCEPTION_CONTINUABLE 0x00 // Execution can proceed
#define EXCEPTION_UNWINDING 0x02 // An exception unwind is in progress
#define EXCEPTION_EXIT_UNWIND 0x04 // An exit unwind is in progress
#define EXCEPTION_STACK_INVALID 0x08 // Stack out of limits or unaligned (not good)
#define EXCEPTION_NESTED_CALL 0x10 // Nested exception handler call 
#define EXCEPTION_TARGET_UNWIND 0x20 // Execute termination handler
#define EXCEPTION_COLLIDED_UNWIND 0x40 // Unwind through unwind dispatcher 

// An unwind of some type is in progress
#define EXCEPTION_UNWIND_CONTEXT (EXCEPTION_UNWINDING | \
                                  EXCEPTION_EXIT_UNWIND | \
                                  EXCEPTION_TARGET_UNWIND | \
                                  EXCEPTION_COLLIDED_UNWIND)

// Handy macros for determining unwind status
#define IS_UNWINDING(flag)	 (((flag) & EXCEPTION_UNWIND_CONTEXT) != 0)
#define IS_DISPATCHING(flag)	 (((flag) & EXCEPTION_UNWIND_CONTEXT) == 0)
#define IS_TARGET_UNWIND(flag) ((flag) & EXCEPTION_TARGET_UNWIND)


/************************************************************************
 * MESSAGE BOXES
 ************************************************************************/

/** Use modal message boxes for error, info and warning messages? If set to 1, ALL
 * message boxes will be modal, thus preventing any user input in other parts of
 * the application. If not, user will still be allowed to move other windows and interact
 * with application's menu.
 * 
 * Default is 1
 *
 * Recommended setting: 1 (setting to 0 will allow the user to interact with dangerous
 * parts of the application that might compromise the program state)
 */
#define USE_MODAL_MESSAGE_BOXES 1

/************************************************************************
 * SPLASH SCREEN
 ************************************************************************/

/** Display a splash screen when the application starts? If set to 1, a static png 
 * logo will be shown to the user before loading the editor. If set to 0, editor loading
 * will begin immediately.
 * 
 * Default is 1
 *
 * Recommended setting: 1 for releases (it is nicer); 0 for debug (it is quicker)
 */
#define DISPLAY_SPLASH_SCREEN 1

/** Time (in ms) that the splash screen will remain visible before loading the 
 * application.
 * 
 * Default is 2000 (ms)
 *
 * Recommended setting: Higher than 1000 ms (too quick) but not higher than 5000 ms (too slow)
 */
#define SPLASH_SCREEN_DURATION  2000 // (ms)

/** Small wait time (in ms) after the splash screen duration that allows the splash screen 
 * to completely disappear before loading the application.
 * 
 * Default is 300 (ms)
 *
 * Recommended setting: Lower than 500 ms
 */
#define SPLASH_SCREEN_FADE_OUT_DURATION 300 // (ms)

/************************************************************************
 * BUILD MESSAGES
 ************************************************************************/

/** Display build messages (i.e. messages shown at compile time)? Please 
 * note that the build messages include the build tasks (e.g. TODO, 
 * FIXME, MINOR, HACK) as well as warnings and notes. If set to 1,
 * the COMPILE_* family of macros will display messages during compile time. 
 * Otherwise, a placebo will be used instead and the macros will be ignored.
 * 
 * Default is 1
 * 
 * Recommended setting: 1 (build messages are an useful tool)
 */
#define SHOW_BUILD_MESSAGES 1

/** Display build tasks (e.g. TODO, FIXME, MINOR, HACK) as compiler warnings? If set
 * to 1 they will appear as warnings in the compiler list. If not, they will be output
 * as simple compiler messages.
 * 
 * Default is 1
 * 
 * Recommended setting: 1 (this will make it hard to forget them)
 * @remarks Only relevant if @see SHOW_BUILD_MESSAGES is set to 1.
 */
#define SHOW_BUILD_TASKS_AS_WARNINGS 1

//////////////////////////////////////////////////////////////////////////
// VC COMPILER VERSIONS
//////////////////////////////////////////////////////////////////////////

// Visual C++ 6.0  
// Released in 1998, and is still widely used for large and small projects. CELayoutEditor
// no longer supports it and developers are strongly encouraged to upgrade to at least 
// 7.1. Its version (stored in _MSC_VER) is 1200.
#define VISUAL_STUDIO_6_0 1200

// Visual C++ .NET 2002 (7.0)  
// First edition of Visual C++ to include managed extensions to the C++ language shared with 
// Visual Basic and Visual C#. This version was known to contain some major bugs at release, 
// and it's highly recommended developers upgrade to at least 7.1. Its version (stored 
// in _MSC_VER) is 1300.
#define VISUAL_STUDIO_7_0 1300  // VS 2002

// Visual C++ .NET 2003 (7.1)  
// Released as a major upgrade to 7.0 fixing a number of issues. The compiler and linker were 
// released for free in the Visual C++ Toolkit 2003 (which has now been taken down in favor 
// of using Visual C++ 2005 (8.0) Express Edition). Its version (stored in _MSC_VER) is 1310.
#define VISUAL_STUDIO_7_1 1310  // VS 2003

// Visual C++ 2005 (8.0)  
// The newest current release of Visual C++. This version contains a few improvements to 
// Intellisense and additional STL debugging features. The Express Edition is available for 
// free, even for commercial use. While this version is not labeled with ".NET" in the 
// name, the .NET Framework is still supported.  Its version (stored in _MSC_VER) is 1400.
#define VISUAL_STUDIO_8_0 1400  // VS 2005


//////////////////////////////////////////////////////////////////////////
// SECURE-CRT FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// Secure-CRT functions are only available starting with VC8
#if _MSC_VER < VISUAL_STUDIO_8_0
#define strcpy_s strcpy
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#define sprintf_s sprintf
#endif

//////////////////////////////////////////////////////////////////////////
// BUILD MESSAGES
//////////////////////////////////////////////////////////////////////////

#if SHOW_BUILD_MESSAGES
    // These three defines produce a line that will be shown in the compiler
    // output window (during builds). These are originally from 
    // Windows Developer Journal, 1997. 
    // IMPORTANT NOTE: There's a bug in Visual Studio 7.1 (see 
    // http://support.microsoft.com/kb/199057/en) and lower which leads to incorrect 
    // __LINE__ macro expansion if used inside a function and compiled with 
    // "Program Database for Edit & Continue" (/ZI) where instead of a constant 
    // expressing line number you'll get the following: (__LINE__Var+constant). The 
    // workaround consists in using compiler option "Program Database" (/Zi) 
    // instead --> Project Properties\C/C++\General\Debug Information Format\Program Database (/Zi).
    // Visual Studio 2005 corrects the problem.
    // NOTE: Note that you should NOT use the word "error" in your reminders, since 
    // it might make the IDE think it should abort execution. 
    // NOTE2: You can double click on these messages and jump to the line in question.
    // NOTE3: You should use the COMPILE_* macros below instead of these directly
    #define STRING_SIZE( L )        #L
    #define MAKE_STRING( M, L )     M(L)
    #define $LINE MAKE_STRING( STRING_SIZE, __LINE__ )

    // Outputs a simple compiler message during builds (e.g. a reminder)
    // NOTE: Note that you should NOT use the word "error" in your reminders, since 
    // it might make the IDE think it should abort execution. 
    // NOTE2: You can double click on these messages and jump to the line in question.
    // NOTE3: You should use the COMPILE_* macros below instead of this directly
    #define NOTE __FILE__ "(" $LINE ") : CELayoutEditor note: "

    // Issues a warning to the compiler during builds
    // NOTE: Using the text warning (and not Warning) produces a warning in Visual Studio. It might
    // work in other compilers as well. If not, try to use the non-standard preprocessor directive
    // #warning to achieve the same effect.
    // NOTE2: Note that you should NOT use the word "error" in your reminders, since 
    // it might make the IDE think it should abort execution. 
    // NOTE3: You can double click on these messages and jump to the line in question.
    // NOTE4: You should use the COMPILE_* macros below instead of this directly
    #define WARNING __FILE__ "(" $LINE ") : CELayoutEditor warning: "

    // The build tasks allow you to set reminders for your developer
    // work. Depending on the value of the SHOW_BUILD_TASKS_AS_WARNINGS define,
    // they will show up as normal compiler messages or as warnings.
    // NOTE: Note that you should NOT use the word "error" in your reminders, since 
    // it might make the IDE think it should abort execution. 
    // NOTE2: You can double click on these messages and jump to the line in question.
    // NOTE3: You should use the COMPILE_* macros below instead of these directly
    #if SHOW_BUILD_TASKS_AS_WARNINGS
        // NOTE: Using the text warning (and not Warning) produces a warning in Visual Studio. It might
        // work in other compilers as well. If not, try to use the non-standard preprocessor directive
        // #warning to achieve the same effect.
        #define TODO  __FILE__ "(" $LINE ") : CELayoutEditor warning (TODO): "
        #define FIXME __FILE__ "(" $LINE ") : CELayoutEditor warning (FIXME): "
        #define HACK  __FILE__ "(" $LINE ") : CELayoutEditor warning (HACK): "
        #define MINOR __FILE__ "(" $LINE ") : CELayoutEditor warning (MINOR): "
    #else
        #define TODO  __FILE__ "(" $LINE ") : CELayoutEditor TODO: "
        #define FIXME __FILE__ "(" $LINE ") : CELayoutEditor FIXME: "
        #define HACK  __FILE__ "(" $LINE ") : CELayoutEditor HACK: "
        #define MINOR __FILE__ "(" $LINE ") : CELayoutEditor MINOR: "
    #endif // SHOW_BUILD_TASKS_AS_WARNINGS

    // These are handy shortcuts to using the compiler build messages.
    // Instead of having to type something like:
    //      #pragma message(WARN "Something to remember!")
    // you can use instead
    //      COMPILE_WARN("Something to remember!")
    // In both cases, the output is similar to:
    //      C:\Dev\CELayoutEditor\CElayoutEditor.cpp(89): CELayoutEditor warning: Something to remember!
    // NOTE: We are using __pragma here. This is an undocumented directive of Visual Studio and used
    // mainly for internal testing at Microsoft. It works in a similar way to the #pragma 
    // preprocessor directive, but with the advantage of being able to be used inside macros. The
    // command __pragma(message(msg)) is equivalent to #pragma message(msg). It has been tested
    // in both VS2003 and VS2005.
    // NOTE2: If __pragma is not compatible with your compiler, start by checking if _Pragma
    // exists (if so, replace with it). If not, remove the command from all the 
    // COMPILE_* macros below and replace its use in the entire solution with #pragma COMPILE_*. 
    // For instance, replace:
    //      #define COMPILE_NOTE(s) __pragma(message(NOTE s))
    // by:
    //      #define COMPILE_NOTE(s) message(NOTE s)
    // Then, replace all occurrences of COMPILE_NOTE in the solution with #pragma COMPILE_NOTE. Do
    // the same for all the other COMPILE_*.
    #if defined (_MSC_VER)
        #define COMPILE_NOTE(s) __pragma(message(NOTE s))
        #define COMPILE_WARN(s) __pragma(message(WARNING s))
        #define COMPILE_TODO(s) __pragma(message(TODO s))
        #define COMPILE_FIXME(s) __pragma(message(FIXME s))
        #define COMPILE_HACK(s) __pragma(message(HACK s))
        #define COMPILE_MINOR(s) __pragma(message(MINOR s))
    #elif defined (__GNUC__)
        // TODO: Make these work!
        #define COMPILE_NOTE(s)
        #define COMPILE_WARN(s)
        #define COMPILE_TODO(s)
        #define COMPILE_FIXME(s)
        #define COMPILE_HACK(s)
        #define COMPILE_MINOR(s)
    #else
    #endif
#else
    // Placebos
    #define COMPILE_NOTE(s) 
    #define COMPILE_WARN(s) 
    #define COMPILE_TODO(s) 
    #define COMPILE_FIXME(s)
    #define COMPILE_HACK(s)
    #define COMPILE_MINOR(s)
#endif // SHOW_BUILD_MESSAGES

#endif // _CONFIG_H_
