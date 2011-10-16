



#include "StdAfx.h"
#include "CSdoaValidate.h"

#define MAX_BE_VALIDATING_COUNT (256 * 1024)
#define MAX_WAIT_ID_COUNT		(128)


CSdoaValidate::CSdoaValidate(VOID)
:m_dwIDSeed(0)
,m_pISDOAUserInfoAuthen(NULL)
,m_TmpClientInfoFullCount(0)
{

}

CSdoaValidate::~CSdoaValidate(VOID)
{

}

//!					
bool CSdoaValidate::Init(VOID)
{
	//创建登陆认证对象实例
	if(NULL != m_pISDOAUserInfoAuthen)
		return false;

	m_pISDOAUserInfoAuthen = sdoaCreateUserInfoAuthen();

	//认证接口初始化
	int iRe=-1;
	if(GetGame()->GetLoginSetup()->bAuthenEnable)
	{
	   iRe = m_pISDOAUserInfoAuthen->Initialize("sdoa4server.ini", CSdoaValidate::ValidateReturnEx);
	}else{
	   iRe = m_pISDOAUserInfoAuthen->Initialize("sdoa4server.ini", CSdoaValidate::ValidateReturn);
	}
	if(0 != iRe)
	{
		//! 初始化失败
		AddLogText("初始化ISDOAUserInfoAuthen失败,error:%d", iRe);
		return false;
	}

	OnInitAuthBusiness();

	return true;
}

//!
VOID CSdoaValidate::Release(VOID)
{
	m_mapTmpClientInfo.clear();
	m_mapSocket2ID.clear();
	m_mapTokenSocketID.clear();
	m_mapSocketTokenID.clear();

}

//! 请求验证
bool CSdoaValidate::RequestValidate(LPCSTR pToken, LONG lIP, LONG lClientSocket, LPCSTR pWSName)
{
	map<LONG, DWORD>::iterator iteID = m_mapSocket2ID.find(lClientSocket);
	if(m_mapSocket2ID.end() != iteID)
	{
		map<DWORD, tagTmpClientInfo>::iterator iteInfo = m_mapTmpClientInfo.find(iteID->second);
		if(m_mapTmpClientInfo.end() != iteInfo)
		{
			if(lIP == iteInfo->second.lIP)
				return true;

			m_mapTmpClientInfo.erase(iteInfo);
		}
		m_mapSocket2ID.erase(iteID);
	}

	//! 防止下面循环因为ID全部用完而进入死循环
	if(MAX_BE_VALIDATING_COUNT == (DWORD)m_mapTmpClientInfo.size())
	{
		++m_TmpClientInfoFullCount;
		if(MAX_WAIT_ID_COUNT < m_TmpClientInfoFullCount)
			m_mapTmpClientInfo.clear();
		return false;
	}

	//! 获取一个没有使用的ID
	DWORD dwTmpID = AllotTmpID();
	while (m_mapTmpClientInfo.end() != m_mapTmpClientInfo.find(dwTmpID))
	{
		dwTmpID = AllotTmpID();
	}

	m_mapTmpClientInfo.insert(make_pair(dwTmpID, tagTmpClientInfo(lIP, lClientSocket, pWSName)));
	m_mapSocket2ID.insert(make_pair(lIP, dwTmpID));
	
	UCHAR arrIp[4] = {0};
	memmove(arrIp, &lIP, sizeof(LONG));
	char szClientIP[64] = {0};
	sprintf(szClientIP, "%u.%u.%u.%u", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);
	int iRe = m_pISDOAUserInfoAuthen->AsyncGetUserInfo(pToken, szClientIP, dwTmpID);

#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "客户端【%u.%u.%u.%u】请求通过PT验证，验证码【%s】，验证ID【%u】", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3], pToken, dwTmpID);
#endif

	return (0 == iRe);
}

//! 处理验证结果
VOID CSdoaValidate::OnValidateResult(tagSdoaValidateResult &SdoaValidateResult)
{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "客户端PT验证结果返回，验证ID【%u】，验证结果【%d】,PT数字账号【%s】", SdoaValidateResult.nUniqueId, SdoaValidateResult.nResult, SdoaValidateResult.szSndaID);
#endif
	//! 找到客户端信息
	map<DWORD, tagTmpClientInfo>::iterator iteInfo = m_mapTmpClientInfo.find(SdoaValidateResult.nUniqueId);
	if(m_mapTmpClientInfo.end() == iteInfo)
	{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
		PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "CSdoaValidate::OnValidateResult()错误返回1");
#endif
		return;
	}

	//! 处理结果
	if(0 != SdoaValidateResult.nResult)
	{
		switch(SdoaValidateResult.nResult)
		{
		case -10332006://! 系统错误
		case -10332007://! socket error
			{
				m_mapTmpClientInfo.clear();
				m_mapSocket2ID.clear();
				m_mapTokenSocketID.clear();
	            m_mapSocketTokenID.clear();
			}
			break;
		default:
			{
				//! -10332001(内容或签名为空)
				//! -10332002(该签名已经验证过)
				//! -10332003(签名验证失败)
				//! -10332004(内容格式有误)
				//! -10332005(超时)
			}
		    break;
		}

		m_mapSocket2ID.erase(iteInfo->second.lIP);
		m_mapTmpClientInfo.erase(iteInfo);

#ifdef OUT_VALIDATE_FCM_INFO_20090720
		PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "CSdoaValidate::OnValidateResult()错误返回2");
#endif
		return;
	}

	tagAccountInfo_FromAS AccountInfo_FromAS;
	strncpy(AccountInfo_FromAS._szCdkey, SdoaValidateResult.szSndaID, CDKEY_SIZE);
	strncpy(AccountInfo_FromAS._szWSName, iteInfo->second.szWSName, WS_NAME_SIZE);
	AccountInfo_FromAS._dwIp						= iteInfo->second.lIP;
	AccountInfo_FromAS._lSocket						= iteInfo->second.lSocket;
	AccountInfo_FromAS._LoginRerult._InfoCode		= LOGIN_OK;
	AccountInfo_FromAS._LoginRerult._dwLoginResult	= ePLR_Normal;
	
	m_mapTmpClientInfo.erase(iteInfo);

	map<string,long>::iterator  it = m_mapTokenSocketID.find(AccountInfo_FromAS._szCdkey);
	if(it != m_mapTokenSocketID.end() )
		m_mapTokenSocketID.erase(it);
	map<long,string>::iterator  itor = m_mapSocketTokenID.find(AccountInfo_FromAS._lSocket);
	if(itor != m_mapSocketTokenID.end() )
		m_mapSocketTokenID.erase(itor);

	//! 如果要进行自己的安全验证
	if(GetGame()->GetLoginSetup()->bXmCardEnable)
	{
		AccountInfo_FromAS._cValidateType = eVT_Not_Pwd | eVT_Not_Fcm;
	}
	else
	{
		AccountInfo_FromAS._cValidateType = eVT_Not_Pwd | eVT_Not_Safe | eVT_Not_Fcm;
	}

	GetGame()->GetLoginManager().AddValidating(AccountInfo_FromAS);
	//! AS验证
	CMessage msg(MSG_O2A_ACCOUNT_Validate_Request);
	msg.AddEx(&AccountInfo_FromAS, sizeof(tagAccountInfo_ToAS));
	msg.SendToAS();
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "CSdoaValidate::OnValidateResult()成功返回");
#endif
}


//! 客户端断开连接
VOID CSdoaValidate::OnClientDisConn(LONG lClientSocket)
{
	map<LONG, DWORD>::iterator iteID = m_mapSocket2ID.find(lClientSocket);
	if(m_mapSocket2ID.end() != iteID)
	{
		m_mapTmpClientInfo.erase(iteID->second);
		m_mapSocket2ID.erase(iteID);
	}
	map<long,string>::iterator itor = m_mapSocketTokenID.find(lClientSocket);
	if(m_mapSocketTokenID.end() != itor)
	{
		m_mapTokenSocketID.erase(itor->second);
		m_mapSocketTokenID.erase(itor);
	}
}

//! 获取一个临时验证ID
LONG CSdoaValidate::AllotTmpID(VOID)
{
	if(MAX_BE_VALIDATING_COUNT <= m_dwIDSeed)
		m_dwIDSeed = 0;
	return m_dwIDSeed++;
}

//! 辅助函数
inline bool GetYesOrNo(ISDOAMsg *pMsg, LPCSTR pItemName)
{
	LPCSTR pYesOrNo = pMsg->GetValue(pItemName);
	if(NULL != pYesOrNo && 0 == stricmp("Y", pYesOrNo))
		return true;
	return false;
}
//! 响应异步的验证结果返回
int	SNDACALLBACK CSdoaValidate::ValidateReturn(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg)
{
	AinMessage *pAinMessage = AinMessage::Create(AIN_MSG_SDOA_VALIDARE_RETURN);
	if (NULL != pAinMessage)
	{
		tagSdoaValidateResult SdoaValidateResult;
		SdoaValidateResult.nResult = nResult;
		SdoaValidateResult.nUniqueId = nUniqueId;

		if(0 == nResult)
		{
			//! 得到数字账号
			LPCSTR pPTID = pMsg->GetValue("sndaID");
			if(NULL != pPTID)
				strncpy(SdoaValidateResult.szSndaID, pPTID, CDKEY_SIZE);
			//! 得到是否防沉迷
			if(GetYesOrNo(pMsg, "applingAdult"))
				SdoaValidateResult.bNotFcm = GetYesOrNo(pMsg, "adult");
			//! 是否为禁止用户
			SdoaValidateResult.bForbidden = GetYesOrNo(pMsg, "forbiddenAccount");
		}
		
		pAinMessage->AddEx(&SdoaValidateResult, sizeof(tagSdoaValidateResult));
		GetGame()->SendToSelf(pAinMessage);
	}
	return 0;
}

//! 响应异步的验证结果返回(补全资料)
int	SNDACALLBACK CSdoaValidate::ValidateReturnEx(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg)
{
	AinMessage *pAinMessage = AinMessage::Create(AIN_MSG_SDOA_Business_RETURN);
	if (NULL != pAinMessage)
	{
		tagSdoaValidateResult SdoaValidateResult;
		SdoaValidateResult.nResult = nResult;
		SdoaValidateResult.nUniqueId = nUniqueId;

		if(0 == nResult)
		{
			//! 得到数字账号
			LPCSTR pPTID = pMsg->GetValue("sndaID");
			if(NULL != pPTID)
				strncpy(SdoaValidateResult.szSndaID, pPTID, CDKEY_SIZE);

			//! 得到是否防沉迷
			if(GetYesOrNo(pMsg, "applingAdult"))
				SdoaValidateResult.bNotFcm = GetYesOrNo(pMsg, "adult");
			//! 是否为禁止用户
			SdoaValidateResult.bForbidden = GetYesOrNo(pMsg, "forbiddenAccount");
		}
		
		pAinMessage->AddEx(&SdoaValidateResult, sizeof(tagSdoaValidateResult));
		GetGame()->SendToSelf(pAinMessage);
	}
	return 0;
}

//！初始化补全资料接口
VOID  CSdoaValidate::OnInitAuthBusiness()
{
	m_pISDOAuthBusiness=NULL;
	m_pISDOAuthBusiness=CreateSDOAuthBusinessInstance();
	int re = m_pISDOAuthBusiness->Init("client.conf");
	if(re!=0)
	{
		char Error[50];
		sprintf(Error,"初始化client.conf失败：%d",re);
			PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, Error);
	}
	AddLogText("初始化AuthBusiness,返回码:%d", re);
	SSDOAuthCallBack  *pbjCb = new SSDOAuthCallBack;
	pbjCb->GetUserInfoCallBack = GetUserInfoCB;
	m_pISDOAuthBusiness->SetCallBack(pbjCb);

}

//!获得玩家防沉迷资料（参数2为是否进行第一次验证） 
VOID  CSdoaValidate::OnAsyncGetUserInfo(string strUserId,bool IsFirst,long lSocketID)
{
	unsigned int dwRpcId=0;
	//vector<int> vecKey;
	int arrKey[2];
	if(IsFirst)
	{
		arrKey[0] = EM_SDO_UserInfo_FirstNeedFullInfo;
		arrKey[1] = EM_SDO_UserInfo_FullInfoUrl;
	}else{
		arrKey[0] = EM_SDO_UserInfo_SecondNeedFullInfo;
		arrKey[1] = EM_SDO_UserInfo_FullInfoUrl;
	}
	SKeyValue oKeyValue;
	oKeyValue.nKey = 1;
	oKeyValue.pValue = (char *)malloc(128);
	strcpy(oKeyValue.pValue, "");
	int iRe = m_pISDOAuthBusiness->AsyncGetUserInfo(0,strUserId.c_str(),arrKey, 2, &oKeyValue, 1, &dwRpcId);
	if(iRe==0)
	{
		mapRpcUserId[dwRpcId]=strUserId;
		//todo other
	}else{   //失败时添加输出信息
		//long lSocketID =  GetGame()->GetCSdoaValidate().GetSocketIDByCDKey(strUserId);
		//AddLogText("验证个人资料失败:%d！！",iRe);
		char chText[250];
		sprintf(chText,"验证个人资料失败！%d，%s",iRe,strUserId.c_str());
		PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, chText);
		CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		msg.Add((long)LOGIN_AuthBusiness_ERROR);
		msg.Add((long)iRe);
		msg.SendToClient(lSocketID);
	}
	//printf("request error, continue login\n");

}

//！获得玩家防沉迷资料，该接口在第一次验证时调用
VOID  CSdoaValidate::OnAsyncGetUserInfo(tagSdoaValidateResult SdoaValidateResult)
{
	if(SdoaValidateResult.bNotFcm==true)
	{
		OnValidateResult(SdoaValidateResult);
	}else{
		DWORD dwSdoID = atoi(SdoaValidateResult.szSndaID);
		map<DWORD, tagTmpClientInfo>::iterator iteInfo = m_mapTmpClientInfo.find(SdoaValidateResult.nUniqueId);
		if(iteInfo!=m_mapTmpClientInfo.end())
		{
			MapTokenSocketID(SdoaValidateResult.szSndaID,iteInfo->second.lSocket); 		
			m_mapSdoaValidateResult[dwSdoID] = SdoaValidateResult;
		    OnAsyncGetUserInfo(SdoaValidateResult.szSndaID,true,iteInfo->second.lSocket);
		}

	}
}

//!通过RpcId获得玩家的数字帐号
string CSdoaValidate::GetUserCDKey(int dwRpcId)
{
	map<unsigned int,string>::iterator itor = mapRpcUserId.find(dwRpcId);
	if(itor!=mapRpcUserId.end())
	{
		return itor->second;
	}else{
		return "";
	}
}

//通过RpcId进行后续验证
VOID CSdoaValidate::OnValidateByRpcId(int dwRpcId)
{
	string strCDKey = GetUserCDKey(dwRpcId);
	DWORD dwSdoID = atoi(strCDKey.c_str());

	map<DWORD,tagSdoaValidateResult>::iterator itor = m_mapSdoaValidateResult.find(dwSdoID);
	if(itor!=m_mapSdoaValidateResult.end())
	{	
		OnValidateResult(itor->second);
		m_mapSdoaValidateResult.erase(itor);
	}
}
//玩家资料查询的返回
void SNDACALLBACK CSdoaValidate::GetUserInfoCB(unsigned int dwRpcId,int nResult, SKeyValue *pKeyValue, int nKeyValueNum)
{
	//please send the return information to other thread which call GetUserInfo. 
	//The message should include (dwRpcId,nResult,pKeyValue);
	//获得该cdkey
	//string strCDkey = GetUserCDKey(dwRpcId);
	if(nResult==0)
	{
		AinMessage *pAinMessage = AinMessage::Create(AIN_MSG_ACCOUNT_Business_Re);
		if(pAinMessage!=NULL)
		{
			pAinMessage->AddDword((int)dwRpcId);
			for(int i = 0; i< nKeyValueNum; i++)
			{
				SKeyValue *pTemp = &(pKeyValue[i]);
				if(strcmp(pTemp->pValue,"")==0)
				{
					pTemp->pValue = NULL;
				}
				//printf("%d %s\n", pTemp->nKey, pTemp->pValue);
				if(pTemp->nKey==EM_SDO_UserInfo_FirstNeedFullInfo)
				{
				   if(pTemp->pValue!=NULL)
				   {
					   pAinMessage->AddByte(atoi(pTemp->pValue));
				   }else{
					   pAinMessage->AddByte(0);
				   }
				}else if(pTemp->nKey==EM_SDO_UserInfo_SecondNeedFullInfo){
				   //if(pTemp->pValue!=NULL)
				   //{
					  // pAinMessage->AddByte(atoi(pTemp->pValue));
					  // sprintf(chText,"验证结果（AIN_MSG_ACCOUNT_Business_Re）%d %d！",pTemp->nKey,atoi(pTemp->pValue));
					  // PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, chText);
				   //}else{
					   pAinMessage->AddByte(0);
				   //}
				}else if(pTemp->nKey==EM_SDO_UserInfo_FullInfoUrl)
				{
				   if(pTemp->pValue!=NULL)
				   {
					   pAinMessage->AddStr(pTemp->pValue);
				   }else{
					   pAinMessage->AddStr("Url=NULL");
				   }
				}
			}		
			GetGame()->SendToSelf(pAinMessage);
		}

	}else{
		char chText[250]="";
		sprintf(chText,"异步获得RPCID（%d）信息返回失败！%d",dwRpcId,nResult);
		PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, chText);
		//AinMessage *pAinMessage = AinMessage::Create(AIN_MSG_SDOA_Business_Validate);
		//pAinMessage->AddDword((int)dwRpcId);
		//GetGame()->SendToSelf(pAinMessage);
		AinMessage *pAinMessage = AinMessage::Create(AIN_MSG_SDOA_Business_Re_Error);
		pAinMessage->AddDword((int)dwRpcId);
		pAinMessage->AddDword((int)nResult);
		GetGame()->SendToSelf(pAinMessage);
	}
}

VOID CSdoaValidate::MapTokenSocketID(string szToken,long SocketID)
{
	if(strcmp(szToken.c_str(),"")==0|| SocketID <= 0)	return;
	//清除之前的szToken记录
	map<string,long>::iterator  it = m_mapTokenSocketID.find(szToken);
	if(it != m_mapTokenSocketID.end() )
	{
		m_mapTokenSocketID.erase(it);
		map<long,string>::iterator  itor = m_mapSocketTokenID.find(SocketID);
		if(itor != m_mapSocketTokenID.end() )
			m_mapSocketTokenID.erase(itor);
	}

	m_mapTokenSocketID[szToken] = SocketID;
	m_mapSocketTokenID[SocketID] = szToken;
}

long CSdoaValidate::GetSocketIDByCDKey(string szCDKey)
{
	if(strcmp(szCDKey.c_str(),"")==0)
		return 0;

	map<string,long>::iterator  it = m_mapTokenSocketID.find(szCDKey);
	if(it != m_mapTokenSocketID.end() )
	{
		return (*it).second;
	}
	char chText[250]="";
	sprintf(chText,"获得SocketID:%d。查询cdKey:%s==失败 size：%d",0,szCDKey.c_str(),m_mapTokenSocketID.size());	
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, chText);
	map<string,long>::iterator  itor = m_mapTokenSocketID.begin();
	int i=0;
	for(itor;itor!=m_mapTokenSocketID.end();itor++)
	{
	   char chText[250]="";
	   sprintf(chText,"mapTokenSocketID--行数：%d   %s   %d",i,itor->first.c_str(),itor->second);
	   PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, chText);
	   i++;
	}

	return 0;
}
string CSdoaValidate::GetCDKeyBySocketID(long SocketID)
{
	if(SocketID<=0)
		return "";
	map<long,string>::iterator it = m_mapSocketTokenID.find(SocketID);
	if(it!=m_mapSocketTokenID.end())
	{
		return (*it).second;
	}
	return "";
}
