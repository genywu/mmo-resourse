#pragma once
#include "stdafx.h"
#include "bsclient.h"
#include "pcallbackfunc.h"
#include "bsipfun.h"
#include <iostream>
using namespace std;

BSClient::FunGSInitialize					BSClient::pGSInitialize;
BSClient::FunUninitialize					BSClient::pUninitialize;
BSClient::FunGetNewId				        BSClient::pGetNewId;
BSClient::FunGetNewIdByParam		        BSClient::pGetNewIdByParam;
BSClient::FunSendAccountAuthenRequest		BSClient::pSendAccountAuthen;
BSClient::FunSendAccountLockExRequest		BSClient::pSendAccountLockEx;
BSClient::FunSendAccountUnlockExRequest		BSClient::pSendAccountUnlockEx;
BSClient::FunSendAwardAuthenRequest			BSClient::pSendAwardAuthen;
BSClient::FunSendAwardAck					BSClient::pSendAwardAck;
BSClient::FunSendTokenDepositRequest		BSClient::pSendTokenDepositRequest;
GSCallbackFuncDef BSClient::CallbackFun;

BSClient::BSClient()
{
	// TODO: Add your specialized code here.
}

BSClient::~BSClient()
{
	// TODO: Add your specialized code here.
}

int BSClient::GetFunction(HINSTANCE hDll)
{
	if (hDll == NULL )
	{
		return -1;
	}
	pGSInitialize               =  (FunGSInitialize)GetProcAddress(hDll, "GSInitialize");	
	pUninitialize               =  (FunUninitialize)GetProcAddress(hDll, "Uninitialize");
	pSendAccountAuthen          =  (FunSendAccountAuthenRequest)GetProcAddress(hDll, "SendAccountAuthenRequest");
	pSendAccountLockEx          =  (FunSendAccountLockExRequest)GetProcAddress(hDll, "SendAccountLockExRequest");
	pSendAccountUnlockEx        =  (FunSendAccountUnlockExRequest)GetProcAddress(hDll, "SendAccountUnlockExRequest");
	pSendAwardAuthen            =  (FunSendAwardAuthenRequest)GetProcAddress(hDll, "SendAwardAuthenRequest");
	pSendAwardAck               =  (FunSendAwardAck)GetProcAddress(hDll, "SendAwardAck");
	pSendTokenDepositRequest    =  (FunSendTokenDepositRequest)GetProcAddress(hDll, "SendTokenDepositRequest");
    pGetNewId                   =  (FunGetNewId)GetProcAddress(hDll, "GetNewId");
	pGetNewIdByParam            =  (FunGetNewIdByParam)GetProcAddress(hDll, "GetNewIdByParam");
    pGetNewId                   =  (FunGetNewId)GetProcAddress(hDll, "GetNewId");
	pGetNewIdByParam            =  (FunGetNewIdByParam)GetProcAddress(hDll, "GetNewIdByParam");
	if( 
		(pGSInitialize              ==NULL)||
		(pUninitialize              ==NULL)||
		(pSendAccountAuthen         ==NULL)||
		(pSendAccountLockEx         ==NULL)||
		(pSendAccountUnlockEx       ==NULL)||
		(pSendAwardAuthen           ==NULL)||
		(pSendAwardAck              ==NULL)||
		(pGetNewId                  ==NULL)||
		(pGetNewIdByParam           ==NULL)

	)
	{
		printf("获取函数指针失败!\n");
		FreeLibrary(hDll);
		return -1;
	}
	return 0;
}

int BSClient::SetCallBackFunction()
{		
	memset(&(BSClient::CallbackFun), 0 ,sizeof(GSCallbackFuncDef));	
    BSClient::CallbackFun.CallbackAccountAuthenRes	=	PCallbackAccountAuthenRes;
	BSClient::CallbackFun.CallbackAccountLockExRes	=	PCallbackAccountLockExRes;
	BSClient::CallbackFun.CallbackAccountUnlockExRes  =	PCallbackAccountUnlockExRes;
	BSClient::CallbackFun.CallbackAwardAuthenRes	    =	PCallbackAwardAuthenRes;
	BSClient::CallbackFun.CallbackAwardAckRes	        =	PCallbackAwardAckRes;
    BSClient::CallbackFun.CallbackTokenDepositRes     =   PCallbackTokenDepositRes;
	return 0;	
}

int BSClient::SendAuth()
{
	GSBsipAccountAuthenReqDef accountAuthen = {0};
	strcpy(accountAuthen.endpoint_ip, "127.0.0.1");
	pGetNewId(accountAuthen.order_id);
	pGetNewId(accountAuthen.sess_id);
	accountAuthen.uid_type = 1;
	strcpy(accountAuthen.user_id, "pt1234");
	int nVal= pSendAccountAuthen(&accountAuthen);
	if (nVal== 0)
	{
		printf("发送AccountAuthen消息成功!\n");
	}
	return 1;
}