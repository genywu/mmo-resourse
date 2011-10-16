#pragma once

#include "cegui/cegui/include/ceguistring.h"//default argument for CEGUI::String

//所有界面控件使用的皮肤（外观）宏定义
#define IMAGES_FILE_NAME		"TaharezLook"
#define BRUSH_NAME				"TextSelectionBrush"
#define MULTILISTSEL_BRUSH_NAME "MultiListSelectionBrush"
#define CHECKBOXMARK			"CheckboxMark"
//通用事件名宏定义
#define EVENT_OPEN				"Opened"
#define EVENT_CLOSE				"Closed"
#define EVENT_SWITCH			"Switch"
#define EVENT_UPDATA			"Updata"
#define GetWndMgr() CEGUI::WindowManager::getSingleton()
#define GetCEGUISystem() CEGUI::System::getSingleton()
#define GetImageSetMgr() CEGUI::ImagesetManager::getSingleton()
// 下面的宏定义须依字母序排列
#define WComboBox(w)        static_cast<CEGUI::Combobox*>(w)
#define WDefaultWindow(w)   static_cast<CEGUI::DefaultWindow*>(w)
#define WDragContainer(w)   static_cast<CEGUI::DragContainer*>(w)
#define WEArgs(e)           static_cast<const CEGUI::WindowEventArgs&>(e)
#define WEditBox(w)         static_cast<CEGUI::Editbox*>(w)
#define WMLEditBox(w)       static_cast<CEGUI::MultiLineEditbox*>(w)
#define WGUISheet(w)        static_cast<CEGUI::GUISheet*>(w)
#define WListBox(w)         static_cast<CEGUI::Listbox*>(w)
#define WListBoxItem(w)     static_cast<CEGUI::ListboxItem*>(w)
#define WMCL(w)	            static_cast<CEGUI::MultiColumnList*>(w)
#define WProgress(w)        static_cast<CEGUI::ProgressBar*>(w)
#define WPushButton(w)      static_cast<CEGUI::PushButton*>(w)
#define WTree(w)            static_cast<CEGUI::Tree*>(w)
#define WTabControl(w)      static_cast<CEGUI::TabControl*>(w)
#define WMenuItem(w)        static_cast<CEGUI::MenuItem*>(w)
#define UIRes               "GameUIResource"

namespace CEGUI
{
    class Window;
    class String;
    class EventArgs;
    class Image;
    class GUISheet;
    class MenuItem;
    class PopupMenu;
    class SubscriberSlot;
};

/// @brief
/// UI事件发送
/// @param id
/// 窗口名称
/// @param e
/// 事件名称
/// @return
/// 

void FireUIEvent(const char* id, const char* e);
CEGUI::Window* LoadUI(const CEGUI::String& name);
CEGUI::Window* GetFocusWindow();
const char * ReloadUILayout(char * WndName);
void SaveLayout();
bool OnShown(const CEGUI::EventArgs &args);
bool OnHide(const CEGUI::EventArgs &args);
bool OnSwitch(const CEGUI::EventArgs &args);

void CloseAllWindow();
void DestroyAllWindow();


//设置背景
const CEGUI::Image& SetBackGroundImage(CEGUI::GUISheet *pBackWin,const char *ImageSetName,
                                       const char * filePath,const char *ImageFileName,
                                       bool autoadjustWndsize = true);


const char *CEGUIStringToAnsiChar(const CEGUI::String &inStr, char *outStr);
const char *CEGUIStringToAnsiChar(const CEGUI::String &inStr);
//转换为CEGUI::String
CEGUI::String & ToCEGUIString(const char *str,CEGUI::String &outString);
CEGUI::String   ToCEGUIString(const char *str);

//获得子组件的索引
long GetIndexByChild(const CEGUI::Window *pParent, const CEGUI::Window *pChild);

//对CEGUI组件设置文本（自动转换UTF-8）
void SetText(CEGUI::Window * pWnd, const char * Text);

//返回组件上的文字（自动转换为ANSI编码）
const char *GetText(CEGUI::Window *pWnd);

//由于编辑器对PopupMenu支持的不好，这些函数用于方便地创建PopupMenu和MenuItem
CEGUI::PopupMenu* CreatePopupMenu(const CEGUI::String& name, 
                                  const CEGUI::String& parent="Root");
CEGUI::MenuItem* CreateMenuItem(CEGUI::PopupMenu* popup, 
                                const CEGUI::String& name, 
                                const CEGUI::String& text, 
                                const CEGUI::SubscriberSlot& onClicked);
