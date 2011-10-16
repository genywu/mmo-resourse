//-------------------------------------------------------------------------------
// PT帐号验证. by Fox.		2008-01-17
//-------------------------------------------------------------------------------

#include "StdAfx.h"
#include "PtCheck.h"
#include "LoginQueue.h"
#include "Game.h"
#include "..\nets\netlogin\message.h"

CPtCheck::CPtCheck(void)
{
	m_InfoHandleThread.Set(InfoProcess, (void*)this, CThreadMaster::AlwaysWait);
}

CPtCheck::~CPtCheck(void)
{
	m_InfoQueue.Clear();
	m_InfoHandleThread.Stop();
}

void CPtCheck::Initial()
{
	char file[MAX_PATH];
	RegistSource(m_pAS.GetInfoNode());
	GetCurrentDirectory(MAX_PATH, file);
	strcat(file,"\\asconfig.ini");
	m_pAS->Init(file);
	m_pAS->SetClientInterface(this);
}

void CPtCheck::GotInfo(int intInfo, const char * szInfo)
{
	string str = szInfo;
	m_InfoQueue.Push(str);
	m_InfoHandleThread.WorkNow();
}

void CPtCheck::InfoProcess(void *param)
{
	CPtCheck * pThis = (CPtCheck*)param;
	string strLogInfo;
	for (;!pThis->m_InfoQueue.IsEmpty();)
	{
		pThis->m_InfoQueue.Front(strLogInfo);
		pThis->m_InfoQueue.Pop();
		PutLogInfo(strLogInfo.c_str());
	}
}

void CPtCheck::OperateResult( CSessionPtr &pSession, int nOperation, int nRes )
{
	long lSocketID		= pSession->GetValueInt("SockID_");
	long lClientIP		= pSession->GetValueInt("ClientIP_");
	string strCdkey		= pSession->GetValue("PopTangId");
	string strWSName	= pSession->GetValue("WSName_");
	long lChallengeCode	= pSession->GetValueInt("ChallengeCode_");
	long lEkey			= pSession->GetValueInt("EKey_");
	string strPWD		= pSession->GetValue("PopTangPwd");
	//long matrix = p_session->GetValueInt("Matrix_");
	//bool is_protected = p_session->GetValueInt("IsProtected");
	//bool bLimit			= pSession->GetValueInt("IsLimited");
    //int is_mobile_user = p_session->GetValueInt("IsMobileUser");

#ifdef _DEBUG
	char szLogInfo[512];
	sprintf(szLogInfo, "PT:%s, OPR:%d, RES:%d.", strCdkey, nOperation, nRes);
	PutLogInfo(szLogInfo);
#endif

	switch ( nRes )
	{
		// 验证成功
	case success:
		// 验证成功,还需要密宝
	case need_ekey: 
		{
			vector<BYTE> strPwd(0);			
            // 把帐号压入“已经通过密码验证的PT帐号队列”
			CLoginQueue::tagQuestCdkey* pAcc = new CLoginQueue::tagQuestCdkey(lSocketID, lClientIP, 0, strCdkey, 
				strPwd, strWSName, lChallengeCode, lEkey, 0, false, nRes);
			pAcc->bISCPTAuthen = true;
            GetGame()->GetLoginQueue()->PushBackPtChecked( pAcc );
		}
		break;

	case failed:
		{
			if( nOperation == authen )
			{
                // 把帐号压入“已经通过密码验证的PT帐号队列”
				vector<BYTE> strPwd(0);
                CLoginQueue::tagQuestCdkey* pAcc = new CLoginQueue::tagQuestCdkey(lSocketID, lClientIP, 0, strCdkey, 
				strPwd, strWSName, lChallengeCode, lEkey, 0, false, nRes);
                GetGame()->GetLoginQueue()->PushBackPtChecked( pAcc );
			}
			else if( nOperation == ekeyauth ) {
				string errcode = pSession->GetValue("FailCode");
				CMessage msgx(MSG_L2C_LOG_ANSWERCDKEY);
				msgx.Add((char)nRes);
				msgx.Add((long)atol(errcode.c_str()));
				msgx.SendToClient(lSocketID);
			}
		}
		break;

	case need_reserve_pwd:	// 本地验证：密码正确并有密宝
		{
			//CInputDlg dlg;
			//dlg.AddName("输入你的保留密码");
			//if (dlg.DoModal()!=IDOK)
			//	return;

			//m_pAS->GiveReservePwd(p_session,  dlg.Value(0) );

			// 如果AS出现问题，就用保留码尝试登录。
			//string ekey = p_session->GetValue("ekey");

			//m_pAS->GiveReservePwd(p_session,  ekey.c_str() );
		}
		break;

	case user_locked:
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)nRes);
			msg.SendToClient(lSocketID);
		}
		break;

	case no_user:
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)LOGIN_NOTCDKEY);
			msg.SendToClient(lSocketID);
		}
		break;

	case miss_pwd:
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)LOGIN_PASSWORDERROR);
			msg.SendToClient(lSocketID);
		}
		break;

	case time_out:
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)nRes);
			msg.SendToClient(lSocketID);
		}
		break;

	case refuse:
		{
			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)LOGIN_BEBANED);
			msg.Add((char)1);// 0表示锦天封的，1表示盛大封的
			msg.SendToClient(lSocketID);
		}
		break;
	default:
		{

			CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
			msg.Add((char)LOGIN_PASSWORDERROR);
			msg.SendToClient(lSocketID);
		}
	}
}

void CPtCheck::QuestEKey( CSessionPtr &pSession, const char *szChallengeCode )
{
	string strChallengeCode = pSession->GetValue("ChallengeCode_");
	string strEkey = pSession->GetValue("Ekey_");
	string strEkeyType = pSession->GetValue("EkeyType_");

	if( strEkeyType == "1" )
	{
		pSession->PutValue("EkeyType_","2");
		strEkeyType = pSession->GetValue("EkeyType_");
	}

	pSession->PutValue("ChallengeCode_",strChallengeCode.c_str());
	m_pAS->GiveEkey(pSession,strEkey.c_str());
}

void CPtCheck::RecvDownMessage( CSDMessage& vMsg )
{
	//AfxMessageBox("RecvDownMessage");
}

