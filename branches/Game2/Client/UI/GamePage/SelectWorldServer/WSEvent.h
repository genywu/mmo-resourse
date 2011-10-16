#pragma once
#include "../../PageEvent.h"

class WSEvent:public PageEvent
{
public:
    WSEvent(){}

    WSEvent(EVENTTYPE type);

    ~WSEvent(){}

    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);
    /*
    brief: 清除worldserver表
    */
    void ClearWS(){m_WorldInfos.clear();}
    /*
    brief: 向worldserver表中增加服务器信息
    */
    void AddWorldInfo(long lID,char* strWorldName,long lState);
    /*
    brief: 回到登陆界面按钮的处理
    */
    bool OnBackToLoginBtn(const CEGUI::EventArgs &e);
    /*
    brief: 进入游戏按钮的处理
    */
    bool OnEnterBtnDown(const CEGUI::EventArgs &e);
private:
    struct tagWorldInfo
    {
        string       strWSName;    //世界服务器名
        long         lState;       //服务器状态
    };
    static map<long,tagWorldInfo>		m_WorldInfos;				//游戏服务器信息
    typedef map<long,tagWorldInfo>::iterator    wsIter;

    /*
    brief: 世界服务器各状态的图像集名字
    */
    static map<long,string>    WS_stateToImageSetName;

    void LoadServerImageSet();
};