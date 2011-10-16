#pragma once
class PageEvent;
namespace CEGUI
{
    class Window;
};

class GamePage
{
public:
    GamePage();
    
    virtual ~GamePage();
    
    virtual void LoadPage() = 0;

    virtual bool Open() = 0;
    
    virtual bool Close() = 0;
    
    virtual void ReleasePage() = 0;

    void SetLayOutFileName(const char *filename);

    void SetLuaFileName(const char *filename);

    string &GetLayoutFileName()   {return m_layout;}

    string &GetLuaFileName()      {return m_luaFileName;}

    void ProcessPageEvent(PageEvent *pEvent);

    CEGUI::Window*   GetPageWindow();
    
    void             SetPageWindow(CEGUI::Window *pWindow) {m_PageWindow = pWindow;}

    void LoadPageWindow();

    bool isOpen();

    void SwitchPage();
private:
    string m_layout;
    
    string m_luaFileName;

    CEGUI::Window* m_PageWindow;
};