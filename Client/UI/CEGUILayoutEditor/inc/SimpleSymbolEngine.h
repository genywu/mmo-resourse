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

#ifndef _SIMPLESYMBOLENGINE_H_
#define _SIMPLESYMBOLENGINE_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Config.h"

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

#include <iosfwd>
#include <string>
#include <windows.h>
#include <dbghelp.h>

#include <wx/thread.h>

//////////////////////////////////////////////////////////////////////////
// SIMPLE SYMBOL ENGINE
//////////////////////////////////////////////////////////////////////////

/** Symbol Engine wrapper to assist with processing PDB information. 
 * Only works under windows platforms and it is thread safe.
 *
 * The code is based on 'SimpleSymbolEngine' V1.4 class created by 
 * Roger Orr <rogero@howzatt.demon.co.uk> (2004). Some code is also
 * inspired on http://www.codeproject.com/KB/threads/StackWalker.aspx
 * by Jochen Kalmbach.
 */
class SimpleSymbolEngine
{
public:

    /************************************************************************
     * SINGLETON ACCESS
     ************************************************************************/
    
    /** Get the symbol engine for this process (singleton).
     * @access public 
     * @qualifier none
     * @return SimpleSymbolEngine& Reference to the symbol engine singleton
     * @remarks SymInitialize doesn't support multiple calls.
     */   
    static SimpleSymbolEngine &instance();

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Provide a stack trace for the specified stack frame.
     * @access public 
     * @qualifier const
     * @param context Context record
     * @param exceptionCode Exception code
     * @param exceptionFlags Exception flags
     * @param exceptionType Exception class
     * @return wxString String with the stack trace
     */
    wxString StackTrace(PCONTEXT context, const DWORD exceptionCode, const DWORD exceptionFlags, const wxString &exceptionType=wxT("")) const;

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Critical section used to lock the @see StackTrace function **/
    static wxCriticalSection m_locker;

    /************************************************************************
     * PRIVATE CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Prevent copies (does nothing).
     * @access private 
     * @qualifier none
     * @param copy
     * @return nothing 
     */
    SimpleSymbolEngine(SimpleSymbolEngine const &);

    /** Prevent assignments (does nothing).
     * @access private 
     * @qualifier none
     * @param 
     * @return SimpleSymbolEngine& Not relevant
     */
    SimpleSymbolEngine& operator=(SimpleSymbolEngine const &);

    /** Construct wrapper for this process (does nothing).
     * @access private 
     * @qualifier none
     * @return nothing
     */
    SimpleSymbolEngine();

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Convert an address to a string.
    * @access public 
    * @qualifier const
    * @param address Address to convert
    * @return wxString Address as string
    */
    wxString AddressToString(PVOID address) const;

    /** Obtains the description of the supplied exception code (text).
    * @access public 
    * @qualifier const
    * @param exceptionCode Exception code to provide the description 
    * @return wxString Last exception's code description
    * @remarks Only relevant on Windows platforms
    */
    wxString GetExceptionCodeDescription(const DWORD exceptionCode) const;

    /** Obtains the description of the supplied exception flags (text).
    * @access public 
    * @qualifier const
    * @param exceptionFlags Exception flags to provide the description 
    * @return wxString Last exception's flags description
    * @remarks Only relevant on Windows platforms
    */
    wxString GetExceptionFlagsDescription(const DWORD exceptionFlags) const;

public: // Work around for MSVC 6 bug

    /************************************************************************
     * PUBLIC CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Destroy information for this process.
     * @access public 
     * @qualifier none
     * @return nothing
     */
    ~SimpleSymbolEngine();

private:
};

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES 

#endif // _SIMPLESYMBOLENGINE_H_

