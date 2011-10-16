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

#ifndef _DIALOG_MAIN_H_
#define _DIALOG_MAIN_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "EditorDocument.h"
#include "IDocumentObserver.h"
#include "DialogAddWindow.h"
#include "WindowContext.h"

#include <wx/treectrl.h>
#include <wx/grid.h>

// Maybe someday replace by wxWidgets container(?)
#include <map>

//////////////////////////////////////////////////////////////////////////
// DIALOGMAIN
//////////////////////////////////////////////////////////////////////////

/** The AddGuiElement dialog. This dialog lets the user browse through the availabe
* Overlays and their elements. Besides that, it provides a way of changing parameters
* of the selected element(s) in the Edit area.
*/
class DialogMain : public wxDialog, IDocumentObserver
{
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/

    /** Constructor. Clears out all control-mapped member variables.
     * @access public 
     * @qualifier none
     * @param parent the dialog's parent window.
     * @return nothing
     */
    DialogMain(wxWindow* parent);

    /** Destructor.
     * @access public 
     * @qualifier none
     * @return nothing
     */
    ~DialogMain();

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Sets the Document to communicate with. This dialog needs the Document to 
     * performs task like getting / setting Window parameters and such. CEGUI 
     * could have been invoked directly from here, but using the Document makes 
     * the system more centralized.
     * @access public 
     * @qualifier none
     * @param document pointer to the wxDocument, which communicates with the CEGUI system.
     * @return void
     */
    void SetDocument (EditorDocument* document);

    /** Implements the IDocumentObserver.*/
    void LayoutOpened (CEGUI::Window* aRoot);

    /** Implements the IDocumentObserver.*/
    void LayoutClosed () ;

    /** Implements the IDocumentObserver.*/
    void LayoutStarted (CEGUI::Window* aRoot);

    /** Implements the IDocumentObserver.*/
    void WindowSelected (CEGUI::Window* aWindow) ;

    /** Implements the IDocumentObserver.*/
    void LayoutModified (CEGUI::Window* aRoot) ;

    /** Implements the IDocumentObserver.*/
    void WindowAdded (CEGUI::Window* aWindow = NULL) ;

    /** Implements the IDocumentObserver.*/
    void WindowsRemoved ();

    /** Implements the IDocumentObserver.*/
    virtual void SelectionDraggedOrResized ();

    /** Its need for the color editor in the EditorDocument.cpp */
    CEGUI::Window *GetCurrentWindow(){return m_currentWindow;};

    /** Holds the widget name and look, as well as the widget itself. */
    typedef struct LookAndWidgetStruct
    {
        CEGUI::Window *window;
        CEGUI::String look;
        CEGUI::String widget;
    }LookAndWidget;

    /** Returns the widget looks registered in the system.
     * @access public 
     * @qualifier none
     * @return wxArrayString Looks registered in the system
     */
    wxArrayString GetAvailableLooks() { return m_availableLooks; }

    /** Modifies the Look'N'Feel of ALL the windows in the layout.
     * @access public 
     * @qualifier none
     * @param newLook New Look'N'Feel to apply
     * @return void
     * @remarks Widgets that do not support the new Look'N'Feel are ignored.
     */
    void SetWidgetsLook(const CEGUI::String& newLook);

    /** Modifies the Look'N'Feel of the windows in the current selection.
     * @access public 
     * @qualifier none
     * @param currentSelection Currently selected widgets
     * @param newLook New Look'N'Feel to apply
     * @return void
     * @remarks Widgets that do not support the new Look'N'Feel are ignored.
     */
    void SetWidgetsLook(Selection* currentSelection, const CEGUI::String& newLook);

    /** Modifies the Look'N'Feel of the supplied widget.
     * @access public 
     * @qualifier none
     * @param currentWindow Widget whose Look'N'Feel we're going to change
     * @param newLook New Look'N'Feel to apply
     * @param refresh Indicates if we need to refresh the application's main frame or not
     * @return void
     */
    void SetWidgetLook(CEGUI::Window *currentWindow, const CEGUI::String& newLook, const bool refresh);

    /** Scans all available Falagard mappings and for each widget saves the looks it can mapped with.
     * @access public 
     * @qualifier none
     * @param clearBeforeHand Indicates if the current widget/look hash mapping is to be cleared before the operation or not
     * @return void
     * @remarks Duplicate pairs (widget/look) are not allowed and will be ignored
     */
    void StoreWidgetLooks(const bool clearBeforeHand = false);

    //¸üÐÂTreeËøÍ¼±ê
    void UpdateTreeImage(CEGUI::Window * pCurWnd);
private:

    /************************************************************************
     * PRIVATE ENUMS
     ************************************************************************/
    
    /** Control ID's who generate interesting events.*/
    enum 
    {
        ID_LAYOUT_TREE = wxID_HIGHEST,
        ID_TEXT,
        ID_GRID,
        ID_ADD_WINDOW,
        ID_LOAD_LAYOUT,
        ID_SAVE_SELECTION,
        ID_SKINS,
        ID_SKINS_DISPLAY,
        ID_TREE_KEYEVENT
    };

    /** Grid column identifiers.*/
    enum Columns
    {
        COLUMN_NAME   = 0,
        COLUMN_VALUE  = 1
    };

    /** Property types.*/
    enum PropertyType
    {
        FLOAT_TYPE,
        BOOL_TYPE,
        FONT_TYPE,
        TEXT_TYPE,
        VERT_ALIGN_TYPE,
        HORZ_ALIGN_TYPE,
        VERT_TEXT_FORMAT_TYPE,
        HORZ_TEXT_FORMAT_TYPE,
        TEXT_COLOUR,
        TABPANE_POS,
        UNSUPPORTED
    };

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** Reference to the application's Document.*/
    EditorDocument*	m_document ;

    /** Some handy references.*/
    CEGUI::Window*	m_currentRoot;
    CEGUI::Window*	m_currentWindow;

    /** Represents the current layout, and allows for re-arranging and re-naming.*/
    wxTreeCtrl*			m_tree;
    wxIcon              m_lockIcon;
    wxImageList*        m_TreeImageList;
    /** The currently BOLD item.*/
    //wxTreeItemId		m_boldItem;
    CEGUI::Window*      m_boldItemWindow;

    /** Button which shows the AddWindow dialog.*/
    wxButton*				m_addWindow;
    wxButton*				m_saveSelection;

    /** Provides quick access to the common "Text" property.*/
    wxTextCtrl*			m_editText;

    /** This maps each existing registered widget (hash key) to its available looks (e.g. Vanilla, TaharezLook, etc.) */
    typedef std::multimap<CEGUI::String, CEGUI::String> WidgetLookHash;    
    WidgetLookHash	m_widgetLooks;

    /** Holds all the looks registered in the system */
    wxArrayString m_availableLooks;

    /** Displays the Type of the currently selected control */
    wxStaticText*			m_typeText;

    /** Displays the current skin of the selected widget and the available ones for the user to switch */
    wxComboBox*			m_skinList;

    /** Radio box that allows the user to switch between the available display modes (common or all)*/
    wxRadioBox*		m_displayModeRadio;

    /** Indicates when in multi-selection mode if we are to only show skins and properties that 
     * ALL elements in the selection share (true) OR if we are to show skins and properties 
     * that at least one element in the selection supports (false). In the second case, if the 
     * user chooses a skin or modifies a property that is not applicable for all elements, 
     * only apply to those that can and ignore others. */
    bool m_displayOnlyCommon;

    /** Represents the current layout, and allows for re-arranging and re-naming.*/
    wxGrid*         m_grid;

    /** We need the sizer to calculate the new column sizes when the user resizes the window.
     * Attaching the sizer with m_grid->setSizer() just didn't draw anything anymore...*/
    wxStaticBoxSizer* m_propertySizer;

    /** Stores all available CE's fonts. We use this array-type, so we can pass it directly
     * to a "choices" cell editor.*/
    wxString*				m_fonts;
    size_t					m_fontCount;

	/** Block 'changed' event on certain occasions.*/
	bool					m_updating;

    typedef std::map<CEGUI::String, PropertyType>	SupportedPropertiesType;

    /** Maps supported properties and their type.*/
    SupportedPropertiesType m_supportedProperties;

    typedef std::map<CEGUI::Window*, wxTreeItemId> TreeItemMappingType;

    /** Stores windows and their tree item id for easy lookups.*/
    TreeItemMappingType			m_treeItemMapping;

    /** List that stores widget looks */
    typedef std::list<CEGUI::String> WidgetLooks;

	/** We save the last-selected row, so we can save the property when the user selects a different widget.*/
	CEGUI::Window*	m_lastSelectedWindow;
	int				m_lastSelectedRow;
    
    /************************************************************************
     * PROPERTY GRID FUNCTIONS
     ************************************************************************/
    
    /** Builds up a map with supported CEGUI properties and their type. The type is used,
     * to determine which wxGridCellEditor subclass we must use for each property's edit cell.
     * @access private 
     * @qualifier none
     * @return void
     */
    void CreateSupportedPropertyMap();

    /** Empties the grid, except for the headers columns.
     * @access private 
     * @qualifier none
     * @return void
     */
    void ClearGrid();

    /** Create our grid object internals.
     * @access private 
     * @qualifier none
     * @return void
     */
    void CreateGrid();

    /** Repopulates the grid, thus updating all property values (both "fake" and "real").
     * @access private 
     * @qualifier none
     * @param mapSkins Maps the possible skins for the current user selection (true) or not (false)
     * @return void
     */
    void UpdateProperties(bool mapSkins = true);

    /** Adds the "fake" properties, like the name and UDIM values for the Left, Top, Right
     * and Bottom window area.
     * @access private 
     * @qualifier none
     * @return void
     */
    void AddFakeProperties();

    /** Adds the remaining "real" and supported CEGUI properties for the current selection.
     * @access private 
     * @qualifier none
     * @param mapSkins Maps the possible skins for the current user selection (true) or not (false)
     * @return void
     */
    void AddRealProperties(const bool mapSkins = true);

    /** Returns the value of the given "fake" property in the supplied window.
     * @access private 
     * @qualifier none
     * @param propertyName Name of the "fake" property to check
     * @param currentWindow Window whose "fake" property value we're interested in
     * @return wxString Value of the supplied "fake" property
     */
    wxString GetFakePropertyValue(const wxString& propertyName, const CEGUI::Window* const currentWindow) const;

    /** Given the supplied "fake" property, verifies in ALL currently selected widgets if its
     * value is the same (common). If it is, the shared value is returned. Otherwise, the function
     * returns an empty value - wxT("") - to inform the user the property has different
     * values in the selection.
     * @access private 
     * @qualifier none
     * @param propertyName Property to process (e.g. "Alpha")
     * @return wxString Fake property value or empty string
     * @remarks "Fake" properties, unlike real ones, are mandatory elements and every widget
     * must have them!
     */
    wxString GetFakePropertyCommonValue(const wxString& propertyName) const;

    /** Verifies if the supplied property contains its default value in ALL elements
     * in the current selection that contain the property. Elements that do not possess
     * the property are ignored.
     * @access private 
     * @qualifier none
     * @param propertyName Property to check (e.g. "Alpha")
     * @return bool True if property is at default value in all elements that contain it, false otherwise
     */
    bool IsPropertyDefaultInSelection(const wxString& propertyName);

    /** The property value font is set in bold if the property is not at its default value
     * in ALL elements in the current selection (@see IsPropertyDefaultInSelection) or as normal
     * text otherwise.
     * @access private 
     * @qualifier none
     * @param propertyRow Grid row to affect
     * @param propertyName Property to check (e.g. "Alpha")
     * @param forceRefresh Forces the visual update of the grid (true) or not (false)
     * @return void
     */
    void SetPropertyValueFont(int propertyRow, const wxString& propertyName, const bool forceRefresh=false);

    /** Updates a "fake" property, when a corresponding grid cell has changed.
     * It uses the first 8 cells to build up a new window area rectangle.
     * @access private 
     * @qualifier none
     * @return void
     */
    void ApplyFakePropertyChange();

    /** Add a row to the grid for the given property, when it is supported. It will search
     * for the corresponding so-called "cellEditor". Color management is supported.
     * @access private 
     * @qualifier none
     * @param propertyName Property name (e.g. "Alpha")
     * @param propertyValue Property value (e.g. "0.7")
     * @param textColor Text color (only for value column)
     * @param backgroundColor Background color (for both name and value columns)
     * @param lockedBackgroundColor Background color for readonly properties (only for value column)
     * @return void
     */
    void AddPropertyRow(const CEGUI::String& propertyName, const CEGUI::String& propertyValue, const wxColour& textColor=*wxBLACK, const wxColour& backgroundColor=*wxWHITE, const wxColour& lockedBackgroundColor=*wxLIGHT_GREY);

    /** Adds a row with the given editor, name and value. Color management is supported.
     * @access private 
     * @qualifier none
     * @param editor One of the specific wxWidgets cell editors, such as "float", "choice" and such.
     * @param propertyName Property name (e.g. "Alpha")
     * @param propertyValue Property value (e.g. "0.7")
     * @param valueReadOnly Whether the value cell is editable (false) or not (true).
     * @param textColor Text color (only for value column)
     * @param backgroundColor Background color (for both name and value columns)
     * @param lockedBackgroundColor Background color for readonly properties (only for value column)
     * @return void
     */
    void AddPropertyRow(wxGridCellEditor* editor, const wxString& propertyName, const wxString& propertyValue, bool valueReadOnly = true, const wxColour& textColor=*wxBLACK, const wxColour& backgroundColor=*wxWHITE, const wxColour& lockedBackgroundColor=*wxLIGHT_GREY);

    /** Returns the property type for the given CEGUI property, or UNSUPPORTED if it can't find the
     * property.
     * @access private 
     * @qualifier none
     * @param propertyName Property name (e.g. "Alpha")
     * @return DialogMain::PropertyType Property type or UNSUPPORTED (if not found)
     */
    PropertyType GetPropertyType(const CEGUI::String& propertyName);

    /** Returns an appropriate wxWidgets cell editor for the given property. Or NULL when the 
     * property is unsupported.
     * @access private 
     * @qualifier none
     * @param propertyName Property name (e.g. "Alpha")
     * @return wxGridCellEditor* Specific wxWidgets cell editor for the property, such as "float", 
       "choice" and such. 
     */
    wxGridCellEditor* GetPropertyEditor(const CEGUI::String& propertyName);

    /** Update the widths of our grid columns
     * @access private 
     * @qualifier none
     * @return void
     */
    void UpdateColumnWidths();

    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/

    /** Cleans up, ready for a new Document.
     * @access private 
     * @qualifier none
     * @return void
     */
    void Reset() ;

    /** Empties the tree and the mapping to CEGUI windows.
     * @access private 
     * @qualifier none
     * @return void
     */
    void ClearTree();

    /** Initializes the Tree Control for the given overlay.*/
    void InitLayoutTree (CEGUI::Window* window, const wxTreeItemId& parent, const bool addThis = true) ;

    /** Caches window/tree item pairs for easy cross-lookup.*/
    void AddTreeItemMapping (CEGUI::Window* window, const wxTreeItemId& treeItem);

    /** Rebuilds the browse-tree. Way easier than keep valid after additions/changes/deletions.
     * @access private 
     * @qualifier none
     * @return void
     */
    void SyncTree();

    /** When an item is programmaticaly selected, it doesn't get blue. This little trick
    * highlights it bold.
    * @access private 
    * @qualifier none
    * @return void
    */
    void HighlightCurrentWindow();

    /** Collects available Fonts.
     * @access private 
     * @qualifier none
     * @return void
     */
    void FindAvailableFonts();

    /** Finds the current GUI Window for the given browse-tree item. Returns the found one.*/
    CEGUI::Window* FindWindow (const wxTreeItemId& treeItem);

    /** Verifies if the given look can be applied to the supplied widget.
     * @access private 
     * @qualifier none
     * @param widget Widget kind (e.g. Button) 
     * @param look Look name (e.g. TaharezLook)
     * @return bool TRUE if the look can be applied to the widget, FALSE otherwise
     */
    bool WidgetLookExists(const CEGUI::String& widget, const CEGUI::String& look);

    /** Returns all the available looks for a given widget.
     * @access private 
     * @qualifier none
     * @param widget Widget kind (e.g. Button)
     * @return DialogMain::WidgetLooks Possible looks for the given widget
     */    
    WidgetLooks GetWidgetLooks(const CEGUI::String& widget);

    /** Retrieves the look (e.g. TaharezLook) and the widget kind (e.g. Button).
     * @access private 
     * @qualifier none
     * @param window Widget to scan
     * @return DialogMain::LookAndWidget Info containing the widget's look and kind
     */
    LookAndWidget GetLookAndWidgetFromType(CEGUI::Window *window) const;

    /** Given the current user widget selection, populates the skin dropdown with 
     * the possible skins that can be mapped on the elements of the selection.
     * @access private 
     * @qualifier none
     * @return void
     */
    void MapWidgetSelectionPossibleSkins();

    /** Given the current user widget selection, update the text, type and skin fields
     * to reflect the selection data.
     * @access private 
     * @qualifier none
     * @return void
     */
    void UpdateSelectionInfo();

    /** General initialization method. Should be called after the constructor.
     * @access private 
     * @qualifier none
     * @return void
     */
    void Setup();

    /************************************************************************
     * PRIVATE SUBCLASSES
     ************************************************************************/
    
    /** Class type function object (functor) that verifies if the supplied widget look is already
    * stored in the widget looks list.
    * @remarks This class could be made more generic through the use
    * of templates, but since it serves a very specific goal it wasn't
    * deemed necessary.
    */
    class NotIn : public std::unary_function<CEGUI::String, bool> 
    {
    public: 
        NotIn(const WidgetLooks looks) : m_looks(looks) {} 
        // Function call operator overload which verifies if the supplied widget look
        // is present in the stored widget list
        bool operator() (CEGUI::String widgetLook)
        {
            std::list<CEGUI::String>::iterator it = m_looks.begin();
            while (it != m_looks.end() && *it != widgetLook) ++it;
            return (it != m_looks.end() ? false : true);
        }
    private:
        WidgetLooks m_looks;
    };

    /************************************************************************
     * EVENTS
     ************************************************************************/
    
    /** Lots of interesting event here :-) 
     * @remarks This set the class access level as protected
     */
    DECLARE_EVENT_TABLE();

    /** The dialog gets resized. We manually update the grid size then.*/
    void OnResize(wxSizeEvent& event);
    //void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);

    void OnAddWindow(wxCommandEvent& event);
    void OnSaveSelection(wxCommandEvent& event);
    void OnChangeText(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnSelchangedLayoutTree(wxTreeEvent& event);
    
	/** Tells us that a cell got selected.*/
    void OnGridCellSelected(wxGridEvent& event);

	/** Tells us that the contents of a cell have changed.*/
    void OnGridCellChange(wxGridEvent& event);

	/** Saves the currently / last selected property/*/
	void SaveCurrentProperty(long lRow);

    /** Called when the user changes the size of the columns by dragging the middle line.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnGridColSize(wxGridSizeEvent& event);

    /** Processes the event raised when the user chooses a new skin.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnChangeSkin(wxCommandEvent& event);	

    /** Processes the event raised when the user chooses a multi-selection skin display type.
     * @access protected 
     * @qualifier none
     * @param event Object that holds the event data.
     * @return void
     */
    void OnDisplayModeSelection(wxCommandEvent& event);

};

#endif // _DIALOG_MAIN_H_
