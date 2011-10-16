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

#ifndef _COPIED_SELECTION_H_
#define _COPIED_SELECTION_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

// MINOR: Maybe someday replace by wxWidgets container(?)
#include <map>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// COPIEDSELECTION
//////////////////////////////////////////////////////////////////////////

/** This class contains a deep copy of a selection of Windows in the gui area. 
 * @see CELayoutEditorDoc::copySelection. Note that we only store the windows; not the entire "boxes".
 *
 * Deep copies have advantages over "shallow" copies, because copied or cut Windows can 
 * be deleted from the layout and still be pasted. There is no need for references.
 *
 * The class uses a counter variable to create unique names, in case a same object is 
 * pasted multiple times.
 */
class CopiedSelection 
{

public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor. Empties members.*/
    CopiedSelection ();

    /** Destructor. Required to release New'd memory.*/
    ~CopiedSelection ();

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Stores a deep copy of the given Window and all of its children. It does not 
     * check for duplicates or anything.
     *
     * @param pWindow the Window to create a deep copy of.
     */
    void Copy (const CEGUI::Window* const pSource);

    /** Pastes the deep copy recursively onto the given Window, which may not be NULL.
     *
     * @param aTarget the Window to paste onto.
     */
    void Paste(CEGUI::Window* aTarget);

    /** Clears current copies.*/
    void Reset () ;

    /** Returns whether this object contains a copy at the moment.*/
    size_t Size () const;

private:

    /************************************************************************
     * PRIVATE STRUCTS
     ************************************************************************/
    
    // Recursive storage
    struct DeepCopy {
        CEGUI::String strName;
        CEGUI::String strType;
        std::map<CEGUI::String, CEGUI::String> mapProperties;
        std::vector<struct DeepCopy*> children;
        DeepCopy() {}
        ~DeepCopy()
        {
            for(unsigned int i = 0; i < children.size(); ++i)
                delete children[i];
        }
    };

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Used to create unique names during pasting. This value is added to the currently
     * pasted name. E.g. "Panel/Button" becomes in additional pastes "Panel/Button1",
     * "Panel/Button2" and so on.*/
    int m_pasteCounter;

    /** To avoid many argument to the paste methods, we toggle this member.*/
    bool    m_applyPasteOffset; // True for the first window, to avoid an exact overlap of positions.

    /** Contains deep copies of GUI elements.*/
    std::vector<DeepCopy*>     m_deepCopies;

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    void CopyWindow( DeepCopy& data, const CEGUI::Window* pSource );
    void CopyWindowRecursive( DeepCopy& data, const CEGUI::Window* pSource );
    CEGUI::Window* PasteWindow( const DeepCopy& data, CEGUI::Window* pTarget );
    void PasteWindowRecursive( const DeepCopy& data, CEGUI::Window* pTarget );
    CEGUI::String CreateUniqueName( const CEGUI::String& oldName ) const;
};

#endif // _COPIED_SELECTION_H_
