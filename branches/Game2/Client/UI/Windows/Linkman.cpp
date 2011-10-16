#include "StdAfx.h"
#include "../UIDef.h"

#include "../cegui/cegui/include/elements/ceguilistbox.h"

using namespace CEGUI;

static String s_Friends(ToCEGUIString("我的好友"));
static String s_Colleagues(ToCEGUIString("企业好友"));
static String s_Blacklist(ToCEGUIString("黑名单"));
static set<String> s_Groups;

static CEGUI::uint s_GroupId=0, s_NameId=1;//用于设置TreeItem的id

static bool OnMouseButtonDown_tLinkman(const EventArgs& args);
static bool OnKeyDown_tLinkman(const EventArgs& args);

static bool OnKeyDown_eGroupName(const EventArgs& args);
static bool OnHide_eGroupName(const EventArgs& args);
static bool OnShow_eGroupName(const EventArgs& args);
static bool OnDeactive_eGroupName(const EventArgs& args);

static bool OnHide_eRoleName(const EventArgs& args);

static bool OnClicked_miAddGroup(const EventArgs& args)
{
    Window* edit = GetWndMgr().getWindow("fwLinkman/tcClass/dwLinkman/eGroupName");
    edit->show();
    return false;
}
static bool OnClicked_miAddRole(const EventArgs& args)
{
    Window* edit = GetWndMgr().getWindow("fwLinkman/tcClass/dwLinkman/eRoleName");
    edit->show();
    return false;
}

Window* InitLinkman()
{
    s_Groups.insert(s_Friends);
    s_Groups.insert(s_Colleagues);
    s_Groups.insert(s_Blacklist);

    Window* frame = LoadUI("fwLinkman");
    TabControl* tabCtl = (TabControl*)frame->getChild("fwLinkman/tcClass");
    Window* def = tabCtl->getChildRecursive("fwLinkman/tcClass/dwLinkman");//DefaultWindow比较特殊。

    Tree* tree = (Tree*)def->getChild("fwLinkman/tcClass/dwLinkman/tLinkman");
    tree->subscribeEvent(Window::EventMouseButtonDown, OnMouseButtonDown_tLinkman);
    tree->subscribeEvent(Window::EventKeyDown, OnKeyDown_tLinkman);
    tree->setSortingEnabled(true);
    for (set<String>::iterator it = s_Groups.begin();
        it != s_Groups.end(); ++it)
    {
        TreeItem* treeItem = new TreeItem(*it);
        treeItem->setID(s_GroupId);
        tree->addItem(treeItem);
        treeItem->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
    }

    PopupMenu* popup = CreatePopupMenu("fwLinkman/tcClass/dwLinkman/tLinkman/pm");
    CreateMenuItem(popup, "fwLinkman/tcClass/dwLinkman/tLinkman/pm/miAddGroup", ToCEGUIString("添加分组"), OnClicked_miAddGroup);
    CreateMenuItem(popup, "fwLinkman/tcClass/dwLinkman/tLinkman/pm/miAddRole", ToCEGUIString("添加好友"), OnClicked_miAddRole);

    Window* edit = def->getChild("fwLinkman/tcClass/dwLinkman/eGroupName");
    edit->subscribeEvent(Window::EventKeyDown, OnKeyDown_eGroupName);
    edit->subscribeEvent(Window::EventHidden, OnHide_eGroupName);
    edit->subscribeEvent(Window::EventShown, OnShow_eGroupName);
    edit->subscribeEvent(Window::EventDeactivated, OnDeactive_eGroupName);
    edit->setVisible(false);

    edit = def->getChild("fwLinkman/tcClass/dwLinkman/eRoleName");
    edit->subscribeEvent(Window::EventKeyDown, OnKeyDown_eGroupName);      //group name!
    edit->subscribeEvent(Window::EventHidden, OnHide_eRoleName);           //role name
    edit->subscribeEvent(Window::EventShown, OnShow_eGroupName);           //group name!
    edit->subscribeEvent(Window::EventDeactivated, OnDeactive_eGroupName); //group name!
    edit->setVisible(false);

    tabCtl->setSelectedTab("fwLinkman/tcClass/dwLinkman");
    frame->setVisible(false);
    return frame;
}
static bool OnMouseButtonDown_tLinkman(const EventArgs& args)
{
    const MouseEventArgs& mouse = static_cast<const MouseEventArgs&>(args);
    switch (mouse.button)
    {
    case RightButton:
        {
            MouseEventArgs newMouseArgs = mouse;
            newMouseArgs.button = LeftButton;
            mouse.window->fireEvent(Window::EventMouseButtonDown, newMouseArgs);

            WindowManager& wndMgr = WindowManager::getSingleton();
            Window* popup = wndMgr.getWindow("fwLinkman/tcClass/dwLinkman/tLinkman/pm");
            UDim x(0.0f, mouse.position.d_x);
            UDim y(0.0f, mouse.position.d_y);
            UVector2 pos(x, y);
            popup->setPosition(pos);
            popup->show();
        }
        return true;
    }
    return false;
}
static bool OnKeyDown_tLinkman(const EventArgs& args)
{
    const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(args);
    switch (keyArgs.scancode)
    {
    case Key::Delete:
        {
            Tree* tree = (Tree*)keyArgs.window;
            TreeItem* treeItem = tree->getFirstSelectedItem();
            if (treeItem == NULL)
                return true;
            //由于tree的remove方法不支持递归，所以若删除的是玩家名字，只能遍历每个组
            if (treeItem->getID() == s_GroupId)
            {
                if (treeItem->getText() != s_Friends
                    && treeItem->getText() != s_Blacklist)
                {
                    tree->removeItem(treeItem);
                }
            }
            else if (treeItem->getID() == s_NameId)
            {
                TreeItem* tiGroup = tree->findFirstItemWithID(s_GroupId);
                while (tiGroup != NULL)
                {
                    tiGroup->removeItem(treeItem);
                    tiGroup = tree->findNextItemWithID(s_GroupId, tiGroup);
                }
            }
            return true;//must true
        }
        break;
    }
    return false;
}
static bool OnKeyDown_eGroupName(const EventArgs& args)
{
    const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(args);
    switch (keyArgs.scancode)
    {
    case Key::NumpadEnter:
    case Key::Return:
        {
            Window* edit = keyArgs.window;
            edit->hide();
            return true;//must true
        }
        break;
    }
    return false;
}
static bool OnHide_eGroupName(const EventArgs& args)
{
    Tree* tree = (Tree*)GetWndMgr().getWindow("fwLinkman/tcClass/dwLinkman/tLinkman");
    TreeItem* blankTreeItem = tree->findFirstItemWithText(" ");
    tree->removeItem(blankTreeItem);

    Window* edit = WEArgs(args).window;
    String groupName = edit->getText();
    if (groupName.find_first_not_of(" ") != String::npos)//组名不全是空格
    {
        //如果tree中没有该组名，则添加之
        TreeItem* treeItem = tree->findFirstItemWithText(groupName);
        while (treeItem != NULL && treeItem->getID() != s_GroupId)
        {
            treeItem = tree->findNextItemWithText(groupName, treeItem);
        }
        if (treeItem == NULL)
        {
            treeItem = new TreeItem(groupName);
            tree->addItem(treeItem);
            treeItem->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
        }
    }

    edit->setText("");
    edit->setAlwaysOnTop(false);
    return false;
}
static bool OnShow_eGroupName(const EventArgs& args)
{
    //在Editbox显示的位置添加一个空的TreeItem，使该Editbox正好覆盖它。
    //这个是个投机取巧的办法，不过目前看来效果还不错。
    Tree* tree = (Tree*)GetWndMgr().getWindow("fwLinkman/tcClass/dwLinkman/tLinkman");
    TreeItem* blankTreeItem = new TreeItem(" ");
    tree->addItem(blankTreeItem);
    blankTreeItem->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);

    Window* edit = WEArgs(args).window;
    edit->activate();
    edit->setAlwaysOnTop(true);
    return false;
}
static bool OnDeactive_eGroupName(const EventArgs& args)
{
    Window* edit = WEArgs(args).window;
    edit->hide();
    return false;
}
static bool OnHide_eRoleName(const EventArgs& args)
{
    Tree* tree = (Tree*)GetWndMgr().getWindow("fwLinkman/tcClass/dwLinkman/tLinkman");
    TreeItem* blankTreeItem = tree->findFirstItemWithText(" ");
    tree->removeItem(blankTreeItem);

    Window* edit = WEArgs(args).window;
    String roleName = edit->getText();
    if (roleName.find_first_not_of(" ") != String::npos)//人名不全是空格
    {
        //如果tree中没有该人名，则添加之
        TreeItem* treeItem = tree->findFirstItemWithText(roleName);
        while (treeItem != NULL && treeItem->getID() != s_NameId)
        {
            treeItem = tree->findNextItemWithText(roleName, treeItem);
        }
        if (treeItem == NULL)
        {
            //找到组
            TreeItem* tiGroup = tree->findFirstItemWithText(s_Friends);
            while (tiGroup != NULL && tiGroup->getID() != s_GroupId)
            {
                tiGroup = tree->findNextItemWithText(s_Friends, tiGroup);
            }
            if (tiGroup != NULL)
            {
                //把玩家名字添加到组中
                TreeItem* tiRole = new TreeItem(roleName);
                tiRole->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
                tiGroup->addItem(tiRole);
                tiRole->setID(s_NameId);
            }
        }
    }

    edit->setText("");
    edit->setAlwaysOnTop(false);
    return false;
}