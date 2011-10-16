#pragma once
/*
界面逻辑事件类 界面的事件类可继承此类，来编写具体界面对应的逻辑功能
*/
enum EVENTTYPE
{
    ON_LOAD   = 0,      //界面加载
    ON_OPEN   = 1,      //界面打开
    ON_CLOSE  = 2,      //界面关闭
    ON_UPDATE = 3,      //界面更新
    ON_RELEASE = 4
};
class GamePage;
class PageEvent
{
public:
    PageEvent(){}
    PageEvent(EVENTTYPE type){m_emEventType = type;}
    virtual ~PageEvent(){}

    virtual void OnPageLoad(GamePage *pPage) = 0;
    virtual void OnPageOpen(GamePage *pPage) = 0;
    virtual void OnPageClose(GamePage *pPage) = 0;
    virtual void OnPageUpdate(GamePage *pPage) = 0;
    virtual void OnPageRelease(GamePage *pPage) = 0;
    EVENTTYPE GetEventType() {return m_emEventType;}
    void      SetEventType(EVENTTYPE type) {m_emEventType = type;}
private:
    EVENTTYPE  m_emEventType;
};