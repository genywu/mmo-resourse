
#include "StdAfx.h"
#include "GameClient.h"
#include "RgnObjMgr.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





void CGameClient::OnOtherMessage(CMessage* msg)
{
	long lType = msg->GetType();
	switch( lType )
	{
	case MSG_S2C_OTHER_ANSWERNPCLIST:
		this->AnswerNpcList(msg);
		break;
	case MSG_S2C_OTHER_WARNING:
		{
			long lAtkedCountry = (long)msg->GetByte();
			long lAtkerCountry = (long)msg->GetByte();
			long lRgnID = msg->GetLong();

			long lPosX = msg->GetLong();
			long lPosY = msg->GetLong();

			if( lPosX < 0 || lPosY < 0 ||
				lPosX > m_pRegion->GetWidth() ||
				lPosY > m_pRegion->GetHeight() )
			{
				char szInfo[1024] = {};
				sprintf(szInfo,"PK坐标Error:在<%s>(%d,%d)有PK发发生",m_pRegion->GetName(),lPosX,lPosY);
				PutStringToFile("Pk Error",szInfo);
			}

			bool bKilled = msg->GetByte() == 0? false:true;

			char szAtkedName[256];
			msg->GetStr(szAtkedName,256);
			char szAtkerName[256];
			msg->GetStr(szAtkerName,256);

			if( strcmp(m_pMainPlayer->GetName(),szAtkerName) == 0 ||
				strcmp(m_pMainPlayer->GetName(),szAtkedName) == 0 )
				break;
		}
		break;
	case MSG_S2C_OTHER_OPENTALKBOX:
		{
			long lFalg = msg->GetLong();
			char str[2048];
			string strText = "";
			CGUID ScriptGuid;
			msg->GetGUID(ScriptGuid);
			if( lFalg == 0)
				strText = msg->GetStr(str,2048);
			else
			{
				DWORD dwType = msg->GetDWord();
				DWORD dwIndex = msg->GetDWord();
			}
			string strBtn1 = "";
			string strBtn2 = "";
			strBtn1 = msg->GetStr(str,2048);
			bool bEnable1 = msg->GetByte()>0?true:false;
			strBtn2 = msg->GetStr(str,2048);
			bool bEnable2 = msg->GetByte()>0?true:false;

			long lStype = msg->GetLong();
			BYTE bIsCloseWindow = 0;
			long lPicID = 0;
			long lPicX = 0;
			long lPicY = 0;
			long lSoundID = 0;
			long lBkMouseID = 0;

			if( lStype)
			{
				bIsCloseWindow = msg->GetByte();
				lPicID = msg->GetLong();
				lPicX = msg->GetLong();
				lPicY = msg->GetLong();
				lBkMouseID = msg->GetLong();
				lSoundID = msg->GetLong();
			}

//			m_pMainPlayer->SetTalkScriptGuid(ScriptGuid);
//			m_pMainPlayer->SetNpcTalkBoxText(strText.c_str());

			//sprintf(str,"TalkBoxOpen(%d)",msg->GetSocketID());
			//CScriptSys::getInstance()->PerformString(str);
		}
	default:
		break;
	}
}