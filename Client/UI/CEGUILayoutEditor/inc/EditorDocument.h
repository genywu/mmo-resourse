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

#ifndef _EDITOR_DOCUMENT_H_
#define _EDITOR_DOCUMENT_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "IDocumentObserver.h"
#include "Selection.h"
#include "CopiedSelection.h"

// MINOR: Maybe someday replace by wxWidgets containers(?)
#include <map>
#include <vector>
#include <set>

#define BACKGROUND_WIDGET_NAME CEGUI::String("BackgroundImage")

//////////////////////////////////////////////////////////////////////////
// EDITORDOCUMENT
//////////////////////////////////////////////////////////////////////////

/** EditorDocument - The application's Document implementation.
*
* This class is used as interface between the application and the CEGUI system. It is important to know that this
* class is like a "thin client". It does not catch anything (except for a few pointers). Each call is made directly
* on CEGUI, so no separate administrations have to be maintained, which makes this class remarkably easy to understand
* I think.
*
*/
class EditorDocument : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(EditorDocument)
   
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor.
    */
    EditorDocument();

    /************************************************************************
     * GETTERS/SETTERS
     ************************************************************************/
    
    /** Returns the current width of the render window.*/
    int GetWidth() 
    {
        return m_width ;
    }

    /** Returns the current height of the render window.*/
    int GetHeight() 
    {
        return m_height ;
    }

    /** @Returns container with selected GUI windows.*/
    Selection* GetSelection()
    {
        return &m_selection;
    }

    /** @Returns container with deep copied GUI windows.*/
    CopiedSelection* GetCopiedSelection()
    {
        return &m_copiedSelection;
    }

    /** @Returns the currently active (loaded) layout.*/
    CEGUI::Window* GetActiveLayout() const
    {
        return m_activeLayout;
    }

    /** Sets/Removes the locked window selection status.
     * @access public
     * @qualifier none
     * @param lockedSelection New selection lock status
     * @return void
     * @remarks In locked mode, the user can't choose another selection without removing the lock
     */
   // void SetLockedSelection(const bool lockedSelection) { m_lockedSelection = lockedSelection; }

    /** Obtains the current window selection lock status.
     * @access public 
     * @qualifier const
     * @return bool Selection lock status
     * @remarks In locked mode, the user can't choose another selection without removing the lock
     */
  //  bool GetLockedSelection() const { return m_lockedSelection; }

    /** Swaps the current value of the window selection lock status.
     * @access public 
     * @qualifier none
     * @return void
     */
    void SwitchLockedSelection(); //{ m_lockedSelection = !m_lockedSelection; }

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Opens the given layout file as editable.
     */
    bool OnOpenDocument(const wxString& fullpathName);

    /** Starts a new layout.
     */
    bool OnNewDocument() ;

    /** Saves the current layout to its filepath.
     */
    bool OnSaveDocument(const wxString& filePath);

    /** The current layout has been closed. The system has prompted the user to save changes, if any.
     */
    bool OnCloseDocument();

    /** Sets the document to being 'dirty' and updates the attached views.
     */
    void Modify (bool modify);

    /** Sets the dimensions of the current view settings. This depends on the resolution. This values
     * are used to normalize the X and Y coordinates.
     * @param iWidth the width of the render window
     * @param iHeight the height of the render window*/
    void SetDimensions (int aWidth, int aHeight) 
    {
        m_width = aWidth;
        m_height = aHeight ;
    }   

    /** Resets the document for editing eg. after a resolution-change which re-starts Ogre.*/
    void Reset();

    /** Adds the given observer.
     * @param observer.*/
    void AddObserver (IDocumentObserver* pObserver) ;

    /** Adds a window of the given type, with the given name, to the given parent.
     *
     * @param pParentWindow the parent of the new window.
     * @param aType the window type e.g. "Checkbox"
     * @param aName the window name e.g. "Female".
     * @return the newly created window, or NULL if failed.
     */
    CEGUI::Window* AddWindow (CEGUI::Window* pParentWindow, const CEGUI::String& aType, CEGUI::String aName);

    /** Removes the given window from the document.
     *
     * @param aWindow the window to remove.
     * @param notifyListeners can be used as a performance gain to only notify on the last in a loop.
     * @return whether the window could be removed. For example the 'root' cannot be removed.
     */
    bool RemoveWindow (CEGUI::Window* aWindow, const bool notifyListeners = true) ;

    /** Sets the given 'root' as the active layout.
     *
     * @param  pRoot
     */
    void SetActiveLayout (CEGUI::Window* aRoot) ;

    /** Delegate for mouse-moving events.
     * @param mouseX current mouse position.
     * @param mouseY current mouse position.*/
    void HandleMouseMoved (int aMouseX, int aMouseY) ;

    /** Delegate for mouse-pressing events. Returns wheter changes have been made.
     * @param mouseX current mouse position.
     * @param mouseY current mouse position.
     * @param whether CTRL is pressed.*/
    void HandleMousePressed (const int aMouseX, const int aMouseY, const bool aMustReset) ;

    /** Invoked when the mouse has been released.*/
    void HandleMouseReleased () ;

    /** Selects the given window, and optionally resets the current selection first.
     * @todo needs to be moved to the VIEW*/
    void SelectWindow (CEGUI::Window* aWindow, const bool aMustReset = true) ;

    /* Selects all windows in the layout */
    void SelectAll();

    // Relevant Parameters
    typedef std::map<CEGUI::String, CEGUI::String> RelevantProperties;
    typedef RelevantProperties::iterator RelevantPropertiesIterator;

    /** Builds a collection (in the formal sense of the meaning) of relevant 
     * properties of the current window selection. Depending on the value of the onlyCommon
     * parameter, the returned collection will only include the properties that are
     * common to ALL elements in the current user selection (true) or every property
     * that is present in at least one element in the user selection, including the common
     * ones (false).
     * @access public 
     * @qualifier none
     * @param commonProperties Collection of properties common to ALL widgets in the current 
     * user selection
     * @param nonCommonProperties Collection of properties that are present in at least one element
     * in the current user selection but not in ALL of them (only filled if onlyCommon is set to true)
     * @param onlyCommon Indicates if only common properties are to be retrieved (true) or not (false)
     * @return void
     */
    void GetRelevantProperties(RelevantProperties* commonProperties, RelevantProperties* nonCommonProperties, const bool onlyCommon=true);

    /** Sets the given property to the given value.
     * @param aName, eg. \"Alpha\".
     * @param newValue, eg. \"0.5\".
     * @param allowEmpty denotes whether an empty values is allowed.
     */
    bool SetProperty (const CEGUI::String& aName, const CEGUI::String& newValue, const bool allowEmpty=false) ;

    /** Cuts the currently selected windows from the GUI system.*/
    void CutSelection();

    /** Copies the currently selected windows from the GUI system.*/
    void CopySelection();

    /** Pastes the currently selected windows from the GUI system onto
     * the first window of the current selection. TODO: After that, the pasted
     * elements become the new selection. (Powerpoint style).
     */
    void PasteCopiedSelection();

    /** Deletes the currently selected windows from the GUI system.*/
    void DeleteSelection();

    /** Returns whether we have a valid background widget loaded.*/
    bool IsBackgroundAvailable() const;

    /** Applies the given filename as a background image.
     * @todo needs to be moved to the VIEW*/
    void SetBackgroundImage(const wxString& filepath);

    /** Toggles background visibility.
     * @todo needs to be moved to the VIEW*/
    void SetBackgroundVisibility(bool visible);

    /** Saves the window tree, starting at "aWindow" to the given file.*/
    void SaveFromWindow(const CEGUI::Window* const aWindow, const wxString& fileName) const;

    //获取当前激活窗口锁定状态
    const bool GetSelectWndisLocked() const;
    
    //获取窗口是否处于锁定状态
    bool isLocked(CEGUI::Window *wnd);

    //切换当前window锁定状态
    void SwitchWindowLock(CEGUI::Window *wnd);

    void UpdateWndMap(CEGUI::Window *wnd);

    //增加窗口
    void AppendWndMap(CEGUI::Window *wnd);
private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Currently active layout.*/
    CEGUI::Window*  m_activeLayout;

    /** Background, if set.*/
    CEGUI::Window*  m_background;

    /** Stores observers.*/
    std::vector<IDocumentObserver*>   m_observers;

    /** Resize point above which the mouse cursor is. */
    int m_hoveredResizePoint;
    int m_previousHoveredResizePoint;

    /** Is the mouse currently pressed? */
    bool  m_mousePressed ;

    /** Current render window width.*/
    int   m_width ;

    /** Current render window height.*/
    int   m_height ;

    /** Current mouseX position. Integer because it's as accurate as it gets.*/
    int   m_mouseX ;

    /** Current mouseX position. Integer because it's as accurate as it gets.*/
    int   m_mouseY ;

    /** Previous mouseX position, to see differences. Integer because it's as accurate as it gets.*/
    int   m_previousMouseX ;

    /** Previous mouseY position, to see differences. Integer because it's as accurate as it gets.*/
    int   m_previousMouseY ;

    /** Window above which the mouse cursor is. */
    CEGUI::Window* m_hoveredWindow;

    /** This object manages the selections. Selects elements and also allows copy / paste / delete operations.
     * It is public to avoid forwarding of its methods.*/
    Selection m_selection ;

    /** This object contains the result of a copy or cut.*/
    CopiedSelection m_copiedSelection ;

    /** Determines if the user can change the current window selection by clicking on other windows
    * outside the selection (false) or if the selection remains constant independently of the user's
    * actions (true).*/
    //bool m_lockedSelection;

    //新增对每一个组件加锁的功能 nikai 2010-8-16
    typedef std::map<CEGUI::Window* , bool> windowLock;
    windowLock    mapWndLock;
    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** Initialises a newly created window, so we can do some initialization
     * based on the type. For example a Menubar root item required a Popupmenu immediately.*/
    void InitNewWindow (CEGUI::Window* aNewWindow, CEGUI::Window* aParentWindow);

    /** Convenience method: pre-populated the given tabcontrol.*/
    void InitTabControl (CEGUI::Window* aTabControl);

    /** Convenience method: pre-populated the given menu bar.*/
    void InitMenubar (CEGUI::Window* aMenuBar);

    /** Very minimal locking behaviour; when the user selects a window, we check whether it may be resized.
     * We use it to block resizing of auto-calculated children, such as menu items and tab buttons.*/
    bool RequiresLock (const CEGUI::Window* const aWindow) const;

    /** Finds the first occurence within any scheme for the given widget type. This avoids dependencies
     * on certain schemes.
     * @returns the full type including scheme name.*/
    CEGUI::String FindFirstMapping(const CEGUI::String& aType) const;

    /** Creates a background (StaticImage) as the root of our upcoming layouts. If not possible,
     * it warns the user and creates a DefaultWindow as a stub.
     * @returns whether it succeeded with a real StaticImage.
     * @todo needs to be moved to the VIEW*/
    bool InitBackground();

    /** Unactivates the current layout. Undoes selection and such.*/
    void DeactivateCurrentLayout();

    /** Updates the status bar.
    * @todo needs to be moved to the VIEW*/
    void UpdateStatusBar(const wxChar *format, ...) const;

    /** Sets the cursor according to the hovered resize point.
     * @todo needs to be moved to the VIEW*/
    void UpdateCursor();

    /** Checks if m_hoveredWindow is a TabButton, which is a special case. If so, the m_hoveredWindow,
     * will be updated to point to the actual ContentPane. */
    bool PressedTabButton();

    /** Checks if m_hoveredWindow is a MenuItem, which is a special case. If so, the menu item's
     * PopupMenu will get toggled. */
    bool PressedMenuItem();

    /** Checks if the user want to change the colours of selected widget. 
     * @todo testing!!*/
    void HandleColoursUpdate(const bool shiftPressed) const;

    /** Called before saving, so we can make some just-in-time modifications if we want.
     */
    void OnPreSave() const;    
};

#endif // _EDITOR_DOCUMENT_H_
