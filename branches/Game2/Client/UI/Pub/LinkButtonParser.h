//[LINK]±ê¼Ç½âÎö
#pragma once
namespace
{
    CEGUI::Window;
}
class LinkButtonParser
{
public:
    LinkButtonParser();
    ~LinkButtonParser();
    CEGUI::String Parse(const CEGUI::String &str);
    static void RemoveLinkBtnFromWindow(CEGUI::Window *Wnd);
    bool OnLinkBtnClicked(const CEGUI::EventArgs &e);
    
private:
    typedef map<CEGUI::Window *, string >    mapLinkInfo;
    static mapLinkInfo     LinkMap;
};
