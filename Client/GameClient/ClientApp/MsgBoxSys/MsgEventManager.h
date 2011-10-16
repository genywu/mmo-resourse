/** 
 *   brief   关于弹出窗口的所有事件处理,基于列队管理所有的弹出窗口.
             只有等上一次对话框关闭后才进行打开下一个.
             
             使用方式: 1. 注册一个全局的事件与事件处理的绑定函数
                        2. 调用可以通过PushEvent来投递事件，支持2种方式.
 *   author  Expter
 *   date    2010/06/12
     Modify  2010/06/25
 */

#pragma  once 

#include "../../../../Public/Common/BoxType.h"
#include "MsgEventDef.h"

#include <Queue>
#include <map>
#include <list>

class   MsgEventManager
{
public:
    MsgEventManager();
    ~MsgEventManager();
    
    void  Init();

public:
    void  PushEvent(eMsgBoxType Type,const char* str=NULL,
                    Msg_EventHandle Handle=NULL,void* pData=NULL,
                    bool Mode=false,bool Cache=false);

    void  PushEvent(MsgEvent& Event);

    void  GoProcessEvent();

private:
    void _PushEvent(MsgEvent& event);

    void ProcesEvent();
 
private:
    /// 所有事件映射表
    /// 弹出对话框处理缓存
    std::list<MsgEvent>                    m_MsgBoxList;

private:
    bool                                   m_IsRuning;
};

#define GetMsgBox() GetInst(MsgEventManager)