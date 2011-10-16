/// 
/// @file ScriptMailFunc.cpp
/// @brief Script内的邮件操作函数
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../Mail/MailManager.h"
#include "../../../../../Public/Setup/MailList.h"


bool Script::ReceiveMail( const CGUID &playerID )
{
    CPlayer *player = FindPlayer( playerID );
    if( player == NULL )
    {
        return false;
    }
    GameManager::GetInstance()->GetMailManager()->SendReceiveMail( player );
    return true;
}

bool Script::SendSysMail( ulong mailID, const char *writer )
{
    CPlayer *player = FindPlayer( NULL );
    if( player == NULL ) return false;
    tagMailParam *mailParam = MailList::GetSysMail( mailID );
   //if( player->CheckSysMail( mailID ) )//--add
   // {
   //     return false;
   // }
    player->InsertSysMail( mailID, 1 );
    if( mailParam )
    {
        bool receive = false;
        if( mailParam->strReceiverList.size() == 0 )
        {
            receive = true;
            mailParam->strReceiverList.push_back( player->GetName() );		
        }

        mailParam->WriterGuid = NULL_GUID;
        mailParam->strWriter = writer;

        tagMailParam *pMailParam = OBJ_CREATE( tagMailParam );
        GameManager::GetInstance()->GetMailManager()->InitSysMailParam( pMailParam, mailParam );//为g_MailManager对象做初始化	

        if( receive )
            mailParam->strReceiverList.clear();

        //pMailParam->lType = mailID;//--modify by shixi

        CMail *pMail = OBJ_CREATE_PVOID( CMail, (void*)pMailParam );
        GameManager::GetInstance()->GetMailManager()->SendSysMailByScript( pMail );
    }
    return true;
}

void Script::ReceiveSysMail()
{
    CPlayer *player = FindPlayer( NULL );
    if( player != NULL )
    {
        CMessage msg( MSG_S2W_MAIL_RECEIVE );
        msg.Add( player->GetExID() );
        msg.Send();
    }
}