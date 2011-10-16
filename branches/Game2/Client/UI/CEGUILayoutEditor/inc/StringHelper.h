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

#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_

//////////////////////////////////////////////////////////////////////////
// STRINGHELPER
//////////////////////////////////////////////////////////////////////////

/** StringHelper - Utility class for string manipulation, including conversion 
 * between CEGUI and wxWidgets strings.
 */
class StringHelper
{
    public:

    /************************************************************************
     * PUBLIC STATIC FUNCTIONS
     ************************************************************************/
    
    /** Converts a wxWidgets string into a CEGUI string, Unicode proof.
     * @access public static 
     * @qualifier none
     * @param str WxWidgets string to convert
     * @return CEGUI::String Converted CEGUI string
     */
    static CEGUI::String ToCEGUIString(const wxString& str)
    {
        const wxWCharBuffer buf = wxConvLibc.cWX2WC(str.c_str());
        return CEGUI::String( (const CEGUI::utf8*)(wxConvUTF8.cWC2MB(buf.data()).data() ) );
    }

    /** Converts a CEGUI string into a wxWidgets string, Unicode proof.
     * @access public static 
     * @qualifier none
     * @param str CEGUI string to convert
     * @return wxString Converted WxWidgets string 
     */
    static wxString ToWXString(const CEGUI::String& str)
    {
        const wxWCharBuffer buf = wxConvUTF8.cMB2WC(str.c_str());
        return wxString(wxConvLibc.cWC2WX(buf.data()));
    }

    /** Converts a wxWidgets string info a float.
     * @access public static 
     * @qualifier none
     * @param str String to convert
     * @return float float contained in the string
     */
    static float WXStringToFloat(const wxString& str)
    {
        double dVal = 0;
        str.ToDouble(&dVal);
        return wx_static_cast(float, dVal);
    }

    /** Adds a item to the end of the supplied wxWidgets array string, provided 
     * that it isn't a duplicated entry.
     * @access public static 
     * @qualifier none
     * @param arrayStr Array string to process
     * @param value Item to add to the array string
     * @return void
     */
    static void WXPushBackUnique(wxArrayString &arrayStr, wxString value)
    {
        // Verify if the supplied item already exists in the array string
        bool exists = false;
        for(wxArrayString::iterator it = arrayStr.begin(); it != arrayStr.end(); it++)
        {
            // Is this the same as the supplied item?
            if(*it == value)
            {	
                // Yes, so nothing more to do
                exists = true;
                break;
            }
        }

        // Add item to array string, if it wasn't found before
        if(!exists)
            arrayStr.push_back(value);
    }

    /** Converts a CEGUI string into a wxWidgets string, Unicode proof.
     * @access public static 
     * @qualifier none
     * @param str String to convert (already in correct WxWidgets format)
     * @return wxString WxWidgets string (that same that was supplied)
     * @remarks This is a placebo that returns the supplied string (already in 
     * the correct format). This is useful to ensure that this call always
     * works even when you don't know the type  of string that will be supplied
     * (for instance in macros like LOG_EXCEPTION).
     */
    static wxString ToWXString(const wxString& str)
    {
        return str;
    }

    /** Converts a wxWidgets string into a CEGUI string, Unicode proof.
     * @access public static 
     * @qualifier none
     * @param str String to convert (already in correct CEGUI format)
     * @return CEGUI::String CEGUI string (that same that was supplied)
     * @remarks This is a placebo that returns the supplied string (already in 
     * the correct format). This is useful to ensure that this call always
     * works even when you don't know the type  of string that will be supplied
     * (for instance in macros like LOG_EXCEPTION).
     */
    static CEGUI::String ToCEGUIString(const CEGUI::String& str)
    {
        return str;
    }

    /** Helper function which returns all array elements in a single comma-separated and
     * newline-terminated string. The entries can themselves be separated by newlines or not.
     * @access public static 
     * @qualifier none
     * @param inputArrayString Array string to process
     * @param addNewLine Adds a new line after each entry in the array string (true) or appends
     * them all sequentially in the same line (false)
     * @return wxString String with the array string entries
     * @remarks Based on wxWidgets (version 2.8.6) function AllAsString() found in aboutdlg.cpp 
     */
    static wxString ArrayStringToString(const wxArrayString& inputArrayString, bool addNewLine = false)
    {
        // Allocate space for the result
        wxString result;
        const size_t count = inputArrayString.size();
        result.reserve(20*count);

        // Scan all array string entries
        for(size_t n = 0; n < count; n++ )
        {
            // Add entry (with newline afterwards if necessary)
            if(addNewLine)
                result << inputArrayString[n] << wxT("\n");                
            else
                result << inputArrayString[n] << (n == count - 1 ? wxT("\n") : wxT(", "));
        }

        // We're done
        return result;
    }
};

#endif // _STRING_HELPER_H_
