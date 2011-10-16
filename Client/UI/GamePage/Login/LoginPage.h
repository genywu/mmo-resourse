#pragma once
#include "../../GamePage.h"
#include "LoginEvent.h"
class GameScene;

class LoginPage:public GamePage
{
public:    
    LoginPage();
    
    virtual ~LoginPage();

    void Display();

    bool DisplaySideBar();

    virtual void LoadPage();

    virtual void ReleasePage();

    virtual bool Open();

    virtual bool Close();

    DWORD GetLastLoginTime()    {return m_LastLoginTime;}

    void UpdateLastLoginTime()     {m_LastLoginTime = timeGetTime();}

    const char *   GetCDkey()   {return m_strCDKey.c_str();}

    void SetCDkey(const char *cdkey) {m_strCDKey = cdkey;}
private:
    /*µÇÂ½3D±³¾°*/
    GameScene *      m_LoginSence;

    DWORD            m_LastLoginTime;

    string           m_strCDKey;

	LoginEvent*		m_event;
};