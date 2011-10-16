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

#ifndef _WINDOW_CONTEXT_H_
#define _WINDOW_CONTEXT_H_

//////////////////////////////////////////////////////////////////////////
// WINDOWCONTEXT
//////////////////////////////////////////////////////////////////////////

/** This class is given a Window, and then you can do requests on it to determine in which
 * context you should work. At the moment, the class is used to determine whether Windows
 * can be added the set Window.
 */
class WindowContext 
{

public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor.
     *
     * @param pWindow the initial window to do queries on.
     */
    WindowContext (CEGUI::Window* pWindow);

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Sets the (new) window. From now on, "queries" are done on this window.
     *
     * @param pWindow the (new) window to do queries on.*/
    void SetWindow(CEGUI::Window* pWindow);

    /** "Course grained" way of checking whether the currently set window can be given a child
     * window by just creating a new window and setting the parent / child relation. For example,
     * when the current window is a Listbox, it will fail, because this widget requires you to call 
     * "addListItem" from your code, so that's a special case.
     */
    bool AcceptsWindowAsChild() const;
    /** Used to do a context sensitive delete.*/
    //bool isListItem();
    //bool acceptsListItemAsChild();
    //bool acceptsColumn();
    //bool acceptsRow();
    //bool requiresTabs();
    //void addWindow(CEGUI::Window* pChildWindow);
    //void addColumn(CEGUI::String columnLabel);
    //void addRow(CEGUI::String rowLabel);
private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    CEGUI::Window* m_pWindow;
};

/*WindowContext::WindowContext (CEGUI::Window* pWindow){
setWindow(pWindow);
}*/

/** Fills a context for the last selected window. This window will be the parent of new windows.
* If no windows are available, the layout root will be returned, which is also a valid window to
* add windows too.
*/
/*CELayoutEditorDoc::getSelectedWindowContext(WindowContext &context){
}*/

#endif // _WINDOWCONTEXT_H_
