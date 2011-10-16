#include "stdafx.h"
#include "MsgEventManager.h"
#include "MsgFunTable.h"
#include "../../../ui/Windows/MsgBox.h"

MsgEventManager::MsgEventManager()
{
    m_IsRuning = false ;
    m_MsgBoxList.clear();
}

MsgEventManager::~MsgEventManager()
{ 
    m_MsgBoxList.clear();
}

void  MsgEventManager::Init()
{

}


void  MsgEventManager::PushEvent(eMsgBoxType Type,const char* str,
                                 Msg_EventHandle Handle,void* pData,
                                 bool Mode,bool Cache)
{
    MsgEvent  event;
    event.pEventDef.BoxType = Type;
    event.pEventDef.FunTable= Handle;
    event.pData = pData;
    event.pContext= str;
    event.bMode = Mode;
    event.bCache= Cache;                   

    _PushEvent( event);
}

void  MsgEventManager::PushEvent(MsgEvent& Event)
{
    _PushEvent( Event );
}

void  MsgEventManager::_PushEvent(MsgEvent& event)
{
    m_MsgBoxList.push_back( event );

    ProcesEvent();
}

void  MsgEventManager::ProcesEvent()
{
    if( m_IsRuning )
        return ;
    std::list<MsgEvent>::iterator  it = m_MsgBoxList.begin();
    if( it != m_MsgBoxList.end() )
    {
        m_IsRuning = true;
       
        GetInst(CMsgBox).Set(*it);
        FireUIEvent("MessageBox",EVENT_OPEN);
        m_MsgBoxList.erase( it );
    }
}

void  MsgEventManager::GoProcessEvent()
{
    std::list<MsgEvent>::iterator  it = m_MsgBoxList.begin();
    if( it != m_MsgBoxList.end() )
    {
        m_IsRuning = true;
       
        GetInst(CMsgBox).Set(*it);
        FireUIEvent("MessageBox",EVENT_OPEN);
        m_MsgBoxList.erase( it );
    }
    else
    {
       m_IsRuning = false ;
    }
}