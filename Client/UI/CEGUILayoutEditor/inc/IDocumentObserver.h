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

#ifndef _IDOCUMENT_OBSERVER_H_
#define _IDOCUMENT_OBSERVER_H_

//////////////////////////////////////////////////////////////////////////
// IDOCUMENTOBSERVER
//////////////////////////////////////////////////////////////////////////

/** Classes can register to the OverlayEditorDocument through this interface, when they are interested 
* to be notified in one ore more of the available methods. Note that the methods are not 'pure' since
* not each class might be interested in every notification. */

class IDocumentObserver 
{
public:

    /** Called when an overlay has been saved.
     * @param aRoot the new root window.*/
    virtual void LayoutOpened (CEGUI::Window* aRoot) {
    }

    /** Called when the current overlay has been closed.*/
    virtual void LayoutClosed () {
    }

    /** Called when a new overlay has been started.
     * @param aRoot the new root window.*/
    virtual void LayoutStarted (CEGUI::Window* aRoot) {
    }

    /** Called when a window is selected or unselected.
     * @param aWindow selected window.*/
    virtual void WindowSelected (CEGUI::Window* aWindow) {
    }

    /** Called when a change takes place. Can be a drag of the selection or the
     * setting of a parameter.*/
    virtual void LayoutModified (CEGUI::Window* aRoot) {
    }

    /** Called when an new window has been added or when windows have been pasted.
     * @param pWindow the new window.*/
    virtual void WindowAdded (CEGUI::Window* aWindow) {
    }

    /** Called when one or more windows have been deleted.*/
    virtual void WindowsRemoved () {
    }

    /** Called when the current selection is being dragged or resized.*/
    virtual void SelectionDraggedOrResized () {
    }
} ;

#endif // _IDOCUMENT_OBSERVER_H_
