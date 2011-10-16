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

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include "iniFile.h"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

// Grid snap modes
#define SNAP_X	0
#define SNAP_Y	1
#define SNAP_XY 2

//////////////////////////////////////////////////////////////////////////
// OPTIONS
//////////////////////////////////////////////////////////////////////////

/** This class provides a central repository of application options & settings. The members
* are read from and written to an INI file. (Not each value can be modified at the moment).
*/
class Options
{
public:

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Destructor.*/
    ~Options();

    /************************************************************************
     * GETTERS/SETTERS
     ************************************************************************/
    
    /** Used when moving a selection by keys.*/
    int GetBigKeyStep()
    {
        return m_bigKeyStep;
    }

    /** Used when moving a selection by keys.*/
    int GetSmallKeyStep()
    {
        return m_smallKeyStep;
    }

    /** Sets the default font to use for new windows.
     * @param font the (new) default font.
     */
    void SetDefaultFont(const wxString& font)
    {
        m_defaultFont = font;
    }

    /** @returns the current font path.
     */
    wxString GetDefaultFont()
    {
        return m_defaultFont;
    }

    /** Sets the current background path.
     * @param path path to currently set background.
     */
    void SetCurrentBackground(const wxString& path)
    {
        m_currentBackground = path;
    }

    /** @returns the current background path.
     */
    wxString GetCurrentBackground()
    {
        return m_currentBackground;
    }

    /** Sets the visibility state of the background.
     * @param visible visible or not.
     */
    void SetBackgroundVisible(bool visible)
    {
        m_backgroundVisible = visible;
    }

    /** Sets the current layout path.
     * @param path path to currently set layout.
     */
    void SetCurrentLayout(const wxString& path)
    {
        m_currentLayout = path;
    }

    /** @returns the current layout path.
     */
    wxString GetCurrentLayout()
    {
        return m_currentLayout;
    }

    /** Sets the current resolution of the view.
     * @param width e.g. 640
     * @param height e.g. 480
     */
    void SetCurrentResolution(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

    /** @returns the current resolution of the view.
    */
    void GetCurrentResolution(int* width, int* height)
    {
        *width = m_width;
        *height = m_height;
    }

    /** Sets the current snap mode.
     * @param mode e.g. 0 for "snap X".
     */
    void SetSnapMode(int mode)
    {
        m_snapMode = mode;
    }

    /** @returns the current snap mode for the grid.
     */
    int GetSnapMode()
    {
        return m_snapMode;
    }

    /** Sets the current grid size in pixels.
     * @param size e.g. 10
     */
    void SetGridSize(int size)
    {
        m_gridSize = size;
    }

    /** @returns the current grid size in pixels.
     */
    int GetGridSize()
    {
        return m_gridSize;
    }

    /** Sets the visibility state of the grid.
     * @param visible visible or not.
     */
    void SetGridVisible(bool visible)
    {
        m_gridVisible = visible;
    }

    /** 
     * @return the path to the configurations files 
     */
    wxString GetPathToConfigs()
    {
        return m_pathConfigs;
    }

    /** 
     * Set the path to configuration files 
     * @param path the new path for configuration files 
     */ 
    void SetPathToConfigs(const wxString& path)
    {
        m_pathConfigs = path;
    }

    /** 
     * @return the path to the fonts files 
     */
    wxString GetPathToFonts()
    {
        return m_pathFonts; 
    }

    /** 
     * Set the path to fonts files 
     * @param path the new path for fonts files 
     */
    void SetPathToFonts(const wxString& path)
    {
        m_pathFonts = path;
    }

    /** 
     * @return the path to the imagesets 
     */ 
    wxString GetPathToImagesets()
    {
        return m_pathImagesets;
    }

    /** 
     * Set the path to imageset files 
     * @param path  the new path for imagesets files 
     */
    void SetPathToImagesets(const wxString& path)
    {
        m_pathImagesets = path;
    }

    /** 
     * @return the path to look&feel 
     */
    wxString GetPathToLookNFeels()
    {
        return m_pathLookNFeels;
    }

    /** 
     * Set the path to look&feels files 
     * @param path the new path for look&feels 
     */
    void SetPathToLookNFeels(const wxString& path)
    {
        m_pathLookNFeels = path;
    }

    /** 
     * @return the path to scripts files 
     */
    wxString GetPathToScripts()
    {
        return m_pathScripts;
    }

    /** 
     * Set the path to scripts files 
     * @param path the new path to scripts 
     */
    void SetPathToScripts(const wxString& path)
    {
        m_pathScripts = path;
    }

    /** 
     * @return the path to schemes files 
     */
    wxString GetPathToSchemes()
    {
        return m_pathSchemes;
    }

    /**
     * Set the path to schemes files 
     * @param path the new path to schemes 
     */
    void SetPathToSchemes(const wxString& path)
    {
        m_pathSchemes = path;
    }

	/** 
     * @return the path to layout files 
     */
    wxString GetPathToLayouts()
    {
        return m_pathLayouts;
    }

    /**
     * Set the path to schemes files 
     * @param path the new path to schemes 
     */
    void SetPathToLayouts(const wxString& path)
    {
        m_pathLayouts = path;
    }

    /**
     * List of supported properties
     *
     * e.g. Alpha,float;ClickStepSize,float;Foo,bar
     *
     * @return a semi-colon seperated list of couple. Each couple is seperated by a comma. The first element is the name and the second element is the type.
     */
    wxString GetSupportedProperties()
    {
        return m_supportedProperties;
    }

    /************************************************************************
     * PUBLIC FUNCTIONS
     ************************************************************************/
    
    /** Returns pointer to singleton (only instance).*/
    static Options* GetInstancePtr();

    /** @returns the visibility state of the grid.
     */
    bool IsBackgroundVisible()
    {
        return m_backgroundVisible;
    }

    /** @return the visibility state of the grid.
     */
    bool IsGridVisible()
    {
        return m_gridVisible;
    }

private:

    /************************************************************************
     * PRIVATE VARIABLES
     ************************************************************************/
    
    /** The one-and-only instance of the class.*/
    static Options *m_instance;

    /** The handy ini file helper class.*/
    CIniFile  m_iniFile;

    /** Step sizes when moving the selection by key-presses.*/
    int   m_bigKeyStep;
    int   m_smallKeyStep;

    /** The default font.*/
    wxString m_defaultFont;

    /** The currently set background.*/
    wxString m_currentBackground;

    /** Current visibility state of the background.*/
    bool	m_backgroundVisible;

    /** The currently set layout.*/
    wxString m_currentLayout;

    /** Current width of the view.*/
    int   m_width;

    /** Current width of the view.*/
    int   m_height;

    /** Current snap mode.*/
    int		m_snapMode;

    /** Current grid size.*/
    int		m_gridSize;

    /** Current visibility state of the grid.*/
    bool	m_gridVisible;

    /** Current path to the CEGUI config files */
    wxString m_pathConfigs;

    /** Current path to the CEGUI fonts files */
    wxString m_pathFonts;

    /** Current path to the CEGUI imagesets files */
    wxString m_pathImagesets;

    /** Current path to the CEGUI layouts files */ 
    wxString m_pathLayouts;

    /** Current path to the CEGUI looknfeel files */
    wxString m_pathLookNFeels;

    /** Current path to the CEGUI lua scripts files */ 
    wxString m_pathScripts;

    /** Current path to the CEGUI schemes files */ 
    wxString m_pathSchemes; 

    /** List of supported properties for the main dialog */ 
    wxString m_supportedProperties;

    /************************************************************************
     * CONSTRUCTORS/DESTRUCTORS
     ************************************************************************/
    
    /** Constructor. Singleton, that's why its private.*/
    Options();
   
    /************************************************************************
     * PRIVATE FUNCTIONS
     ************************************************************************/
    
    /** When the ini file is found, the options are loaded by this method.
     * @access private 
     * @qualifier
     * @return void
     * @remarks When no ini file is found, the class uses default values set
     * in the constructor initialization list.
     */
    void UseSettings();    
};

#endif // _OPTIONS_H_
