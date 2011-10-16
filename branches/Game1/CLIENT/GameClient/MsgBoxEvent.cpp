#include "StdAfx.h"
#include "Game.h"
#include ".\msgboxevent.h"

#include "..\nets\netclient\message.h"
//#include "..\appclient\interface\interface.h"
#include "..\appclient\other\GlobeSetup.h"

//////////////////////////////////////////////////////////////////////////
//MsgBoxEvent.h
//Func:MsgBox运行的事件类
//Author:joe
//Create Time:2005.2.2
//////////////////////////////////////////////////////////////////////////


CMsgBoxEvent::CMsgBoxEvent(void)
{
	ClearAllEvent();
}

CMsgBoxEvent::~CMsgBoxEvent(void)
{
}

void	CMsgBoxEvent::ClearAllEvent()
{
	//如果存在，则改变
	list<tagMsgBoxEvent*>::iterator it = m_MsgBoxEvents.begin();
	for(;it != m_MsgBoxEvents.end();it++)
	{
		tagMsgBoxEvent* pEvent = (*it);

		switch(pEvent->eEvent)
		{
		case 0:
			{
			}
			break;
		default:
			{
			}
		}
		delete pEvent;
	}
	m_MsgBoxEvents.clear();
}


void	CMsgBoxEvent::AddEvent(eMsgBoxEvent eEvent,...)
{
	va_list va;
	va_start(va,eEvent);

	//如果存在，则改变
	tagMsgBoxEvent *ptagEvent=NULL;

	list<tagMsgBoxEvent*>::iterator it = m_MsgBoxEvents.begin();
	for(;it != m_MsgBoxEvents.end();it++)
	{
		if((*it)->eEvent == eEvent)
		{
			ptagEvent = (*it);
			break;
		}
	}

	if(ptagEvent == NULL)
	{
		ptagEvent = new tagMsgBoxEvent;
		ptagEvent->eEvent=eEvent;
		m_MsgBoxEvents.push_back(ptagEvent);
	}

	switch(eEvent)
	{
	case MBE_DIEDSELECT:
		{
		}
		break;
	default:
		{
			return;
		}
		break;
	}		
}

//运行 MsgBox 并处理结果
void	CMsgBoxEvent::Run()
{
	if(m_MsgBoxEvents.size()<=0)
		return;
	char	strMsg[256];
	tagMsgBoxEvent* pMsgBoxEvent = m_MsgBoxEvents.front();
	switch(pMsgBoxEvent->eEvent)
	{
	case MBE_DIEDSELECT:		//死亡选择事件
		{
			sprintf(strMsg,"是否在复活点复活?");
			//long lRet = GetGame()->GetInterface()->GetMsgBox()->RunByNoModal(strMsg,
			//												CGameMsgBox::MWS_NO | CGameMsgBox::MWS_YES);
			//if(lRet ==  CGameMsgBox::MWS_YES)
			//{
			//	delete pMsgBoxEvent;
			//	m_MsgBoxEvents.pop_front();

			//	//CMessage msg(MSG_C2S_PLAYER_QUEST_RELIVE);
			//	//msg.Send();
			//}
			//else if(lRet ==  CGameMsgBox::MWS_NO)
			//{
			//	delete pMsgBoxEvent;
			//	m_MsgBoxEvents.pop_front();

			//	GetGame()->GetInterface()->AddText("人物死亡，按Esc选择复活，可以在最近的村庄或城镇复活。", CGlobeSetup::GetSetup()->colorWarningText, D3DCOLOR_ARGB(255,255,255,255));
			//	GetGame()->GetInterface()->AddOtherInfo("人物死亡，按Esc选择复活，可以在最近的村庄或城镇复活。", CGlobeSetup::GetSetup()->colorWarningText);
			//}
		}
		break;
	}
}

//显示MsgBox
void	CMsgBoxEvent::Display()
{
	if(m_MsgBoxEvents.size()<=0)
		return;
	//GetGame()->GetInterface()->GetMsgBox()->Display();
}
