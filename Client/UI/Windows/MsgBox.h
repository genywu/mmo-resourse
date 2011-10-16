#pragma  once 

#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventDef.h"
#include "../../../Public/Common/BoxType.h"

class  CMsgBox
{      

 public:
     CMsgBox()
     {  
     }

     void  Reset()              
     { 
         memset(&event,0,sizeof(MsgEvent)); 
     }

     void  Set(const MsgEvent& _event)
     { 
         event = _event; 
     }


 public:
     bool  OnOpen(const CEGUI::EventArgs& e);
    
     bool  OnClose(const CEGUI::EventArgs& e);   
     
     bool  OnClickOk(const CEGUI::EventArgs& e);

     bool  OnClickMk(const CEGUI::EventArgs& e);

     bool  OnClickCancel(const CEGUI::EventArgs& e);

 private:
     MsgEvent  event;
 };