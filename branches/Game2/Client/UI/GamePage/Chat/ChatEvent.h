#pragma once

#include "../../PageEvent.h"
#include "../../../GameClient/ClientApp/other/Chat.h"
#include "../../../../Public/Common/BaseDef.h"

class ChatEvent:public PageEvent
{
public:
    ChatEvent(){}
    ChatEvent(EVENTTYPE type);
    ~ChatEvent(){}
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);
};
//////////////////////////////////////////////////////////////////////////
//zhaohang 2010-03-18
//
/*!
\brief
处理聊天内容文本的类,与星尘的Chat.h 文件中得作用CChat类似
*/
class ChatInfo
{
public:
    ChatInfo();
    ~ChatInfo();

    /*!
    \brief
    修改文本内容
    */
    void ReSetChatContent(const char *content);
    /*!
    \brief
    根据文本前缀获取聊天类型（以前的频道概念）
    */
    eChatType GetChatType();

    /*!
    \brief
    根据聊天类型移除成员变量对应得文本前缀并返回移除后得字符串指针
    */
    const char* RemoveFlag(eChatType e);

private:
    string m_Content;
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//zhaohang 2010-03-18
//
/*!
\brief
处理聊天的逻辑管理类,目前只加入了脚本调用部分
*/
class ChatLogicEventManager
{
public:
    ChatLogicEventManager();
    ~ChatLogicEventManager();

    /*!
    \brief
    向服务器发送脚本调用请求
    \param ScriptContent
    脚本内容
    */
    bool SendScript(const CEGUI::EventArgs &args);

    //回滚曾经发送的信息
    bool RollBackSendedText(const CEGUI::EventArgs &args);

    void SetRollPos(long pos)   {RollPos = pos;}

    long GetMsgQueSize()        {return (long)SendedMsgQue.size();}
protected:

private:
    ChatInfo m_chat;

    vector<string>      SendedMsgQue;

    long     RollPos;
    
};

