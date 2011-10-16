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

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CELayoutEditor.h"
#ifdef __linux__
# include <unistd.h>

# define DATAPATH_VAR_NAME "CELAYOUTEDITOR_DATAPATH"
#endif

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

/** Define strings.*/
#define BIG_KEY_STEP			"BigKeyStep"
#define SMALL_KEY_STEP			"SmallKeyStep"
#define DEFAULT_FONT			"DefaultFont"
#define CURRENT_BACKGROUND      "CurrentBackground"
#define BACKGROUND_VISIBLE      "BackgroundVisible"
#define CURRENT_LAYOUT          "CurrentLayout"
#define VIEW_WIDTH              "ViewWidth"
#define VIEW_HEIGHT             "ViewHeight"
#define SNAP_MODE				"SnapMode"
#define GRID_SIZE				"GridSize"
#define GRID_VISIBLE			"GridVisible"
#define PATH_CONFIGS            "ConfigsPath"
#define PATH_FONTS              "FontsPath"
#define PATH_IMAGESETS          "ImagesetsPath"
#define PATH_LOOKNFEELS         "LookNFeelsPath"
#define PATH_SCRIPTS            "ScriptsPath"
#define PATH_SCHEMES            "SchemesPath" 
#define PATH_LAYOUTS            "LayoutsPath" 
#define SUPPORTED_PROPERTIES    "SupportedProperties"

#define INI_FILE    "CELayoutEditor.ini"
#define INI_SECTION "SETTINGS"

//////////////////////////////////////////////////////////////////////////
// OPTIONS GLOBALS
//////////////////////////////////////////////////////////////////////////

/** Declare the instance.*/
Options* Options::m_instance = NULL;

//////////////////////////////////////////////////////////////////////////
// OPTIONS FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
Options* Options::GetInstancePtr()
{
    // Create instance, if not done yet
    if (!m_instance)
    {
        m_instance = new Options();
    }
    return m_instance;
}

//------------------------------------------------------------------------
Options::Options() :

// Set configuration file path
m_iniFile(INI_FILE),

// Step defaults
m_bigKeyStep(10),        // 10 pixels per big step
m_smallKeyStep(1),       // 1 pixel per small step

// No font initially
m_defaultFont(),

// No background nor layout initially
m_currentBackground(),
m_backgroundVisible(false),
m_currentLayout(),

// Default to the medium resolution
m_width(1024),
m_height(768),

// Snap both X and Y
m_snapMode(SNAP_XY),

// Invisible grid, 10x10
m_gridSize(10),
m_gridVisible(false),

// No default data directories are set. This forces to prompt a directory browser on first use.
m_pathConfigs(),
m_pathFonts(),
m_pathImagesets(),
m_pathLookNFeels(),
m_pathScripts(),
m_pathSchemes(), 
m_pathLayouts(),

// Supported properties
m_supportedProperties(wxT("Alpha,float; \
ClickStepSize,float; \
MaximumValue,float;\
Visible,bool;\
AlwaysOnTop,bool;\
ClippedByParent,bool;\
InheritsAlpha,bool;\
Selected,bool;\
ReadOnly,bool;\
CloseButtonEnabled,bool;\
DragMovingEnabled,bool;\
FrameEnabled,bool;\
SizingEnabled,bool;\
TitlebarEnabled,bool;\
MultiSelect,bool;\
Sort,bool;\
DraggingEnabled,bool;\
BackgroundEnabled,bool;\
InheritsTooltipText,bool;\
HoverImage,text;\
PushedImage,text;\
DisabledImage,text;\
NormalImage,text;\
Font,font;\
TitlebarFont,font;\
VerticalAlignment,vert_align;\
HorizontalAlignment,horz_align;\
VertFormatting,vert_text_format;\
HorzFormatting,horz_text_format;\
Tooltip,text;\
Image,text;\
TextColours,text;\
VerticalProgress,bool;\
ReversedProgress,bool;\
CurrentProgress,float;\
BackImage,text;\
DocumentSize,float;\
PageSize,float;\
StepSize,float;\
LeftOrUpImage,text;\
RightOrBottomImage,text;\
ProgressImage,text;\
IncreseBtnNormal,text;\
IncreseBtnHover,text;\
IncreseBtnPushed,text;\
DecreseBtnNormal,text;\
DecreseBtnHover,text;\
DecreseBtnPushed,text;\
ThumbBtnNormal,text;\
ThumbBtnHover,text;\
ThumbBtnPushed,text;\
TabPanePosition,tab_positon;"))
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

    // Attempt to read configuration file from disk
    if (!m_iniFile.ReadFile())
        // File not found: Warn user and keep default settings
        LogWarning (wxT("The file '%s' could not be found; default settings will apply now."), wxT(INI_FILE));
    else
        // File found: Read and assign read values
        UseSettings();
}

//------------------------------------------------------------------------
Options::~Options()
{
    // Swallow any exceptions found to prevent the destructor from emitting them.
    // NOTE: An exception should never leave a destructor as it may yield
    // undefined behavior or even lead to program abnormal termination 
    // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
    try
    {
        // Save all current settings.
        (void)m_iniFile.SetValueI(INI_SECTION, BIG_KEY_STEP, m_bigKeyStep);
        (void)m_iniFile.SetValueI(INI_SECTION, SMALL_KEY_STEP, m_smallKeyStep);
        (void)m_iniFile.SetValue(INI_SECTION, DEFAULT_FONT, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_defaultFont)));
        (void)m_iniFile.SetValue(INI_SECTION, CURRENT_BACKGROUND, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_currentBackground)));
        (void)m_iniFile.SetValueI(INI_SECTION, BACKGROUND_VISIBLE, m_backgroundVisible);
        (void)m_iniFile.SetValue(INI_SECTION, CURRENT_LAYOUT, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_currentLayout)));
        (void)m_iniFile.SetValueI(INI_SECTION, VIEW_WIDTH, m_width);
        (void)m_iniFile.SetValueI(INI_SECTION, VIEW_HEIGHT, m_height);
        (void)m_iniFile.SetValueI(INI_SECTION, SNAP_MODE, m_snapMode);
        (void)m_iniFile.SetValueI(INI_SECTION, GRID_SIZE, m_gridSize);
        (void)m_iniFile.SetValueI(INI_SECTION, GRID_VISIBLE, m_gridVisible);
        (void)m_iniFile.SetValue(INI_SECTION, PATH_CONFIGS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathConfigs)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_FONTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathFonts)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_IMAGESETS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathImagesets)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_LOOKNFEELS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathLookNFeels)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_SCRIPTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathScripts)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_SCHEMES, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathSchemes)));
		(void)m_iniFile.SetValue(INI_SECTION, PATH_LAYOUTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathLayouts)));
        (void)m_iniFile.SetValue(INI_SECTION, SUPPORTED_PROPERTIES, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_supportedProperties)));

        // Save into the same path as the Load
        (void)m_iniFile.WriteFile();
    }
    // Add exception to log
    LOG_EXCEPTION_SAFE("Options::~Options", "Failed to destroy options", "Failed to destroy options", "Failed to destroy options");
}

//------------------------------------------------------------------------
void Options::UseSettings()
{
    std::string  value;

    // Don't blindly read the values; Only overwrite defaults when found
    value = m_iniFile.GetValue(INI_SECTION, BIG_KEY_STEP);
    if (value != "")
    {
        m_bigKeyStep = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SMALL_KEY_STEP);
    if (value != "")
    {
        m_smallKeyStep = atoi(value.c_str());
    }
    m_currentBackground = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, CURRENT_BACKGROUND).c_str());
    value = m_iniFile.GetValue(INI_SECTION, BACKGROUND_VISIBLE);
    if (value != "")
    {
        const int number = atoi(value.c_str());
        m_backgroundVisible = (number == 1);
    }
    m_defaultFont = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, DEFAULT_FONT).c_str());
    m_currentLayout = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, CURRENT_LAYOUT).c_str());
    value = m_iniFile.GetValue(INI_SECTION, VIEW_WIDTH);
    if (value != "")
    {
        m_width = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, VIEW_HEIGHT);
    if (value != "")
    {
        m_height = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SNAP_MODE);
    if (value != "")
    {
        m_snapMode = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, GRID_SIZE);
    if (value != "")
    {
        m_gridSize = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, GRID_VISIBLE);
    if (value != "")
    {
        const int number = atoi(value.c_str());
        m_gridVisible = (number == 1);
    }
    // Load path information here 
    value = m_iniFile.GetValue(INI_SECTION, PATH_CONFIGS);
    if (value != "")
    {
        m_pathConfigs = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_FONTS);
    if (value != "")
    {
        m_pathFonts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_IMAGESETS);
    if (value != "")
    {
        m_pathImagesets = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_LOOKNFEELS);
    if (value != "")
    {
        m_pathLookNFeels = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_SCRIPTS);
    if (value != "")
    {
        m_pathScripts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_SCHEMES);
    if (value != "")
    {
        m_pathSchemes = wxConvLibc.cMB2WX(value.c_str());
    }
	value = m_iniFile.GetValue(INI_SECTION, PATH_LAYOUTS);
    if (value != "")
    {
        m_pathLayouts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SUPPORTED_PROPERTIES);
    if (value != "")
    {
        m_supportedProperties = wxConvLibc.cMB2WX(value.c_str());
    }
}
