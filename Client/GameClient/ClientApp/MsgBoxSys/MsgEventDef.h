/**
 *  brief  弹出窗口一些事件定义
 *  author expter
 */

#pragma  once 

#include "../../../../Public/Common/BoxType.h"


struct MsgEvent;
class  CGUID;
typedef  void (* Msg_EventHandle)(MsgEvent& event,eMsgResult UserId);

struct  MsgEvent_Def
{
    eMsgBoxType         BoxType;
    Msg_EventHandle     FunTable;
};

struct  MsgEvent
{
   MsgEvent_Def         pEventDef;
   const char *         pContext;
   void*                pData;
   bool                 bMode;
   bool                 bCache;
   MsgEvent()
   {
       pContext="";
       pData = "";
       bMode = bCache = false;
   }
   const MsgEvent& operator = (const MsgEvent& other)
   {
        if ( &other == this )
            return *this;
        
        this->pEventDef = other.pEventDef;
        this->pContext = other.pContext;
        this->pData = other.pData;
        this->bMode = other.bMode;
        this->bCache= other.bCache;

        return *this;
   }
};