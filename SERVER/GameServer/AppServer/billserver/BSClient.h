#pragma once

#include "bsipfun.h"
#include <windows.h>

class BSClient
{
public:		
	BSClient();
	~BSClient();
public:
	
	typedef int (*FunGSInitialize)(GSCallbackFuncDef *callbackFun, const char *configfilename,int type);
	typedef int (*FunUninitialize)(int type);	
	typedef int (*FunSendAccountAuthenRequest)(const GSBsipAccountAuthenReqDef *req);
	typedef int (*FunSendAccountLockExRequest)(const GSBsipAccountLockExReqDef *req);
	typedef int (*FunSendAccountUnlockExRequest)(const GSBsipAccountUnlockExReqDef *req);
	typedef int (*FunSendAwardAuthenRequest)(const GSBsipAwardAuthenReqDef *req);
	typedef int (*FunSendAwardAck)(const GSBsipAwardAckDef *req);
	typedef int (*FunSendTokenDepositRequest)(const GSBsipTokenDepositReqDef *req);	
	typedef int (*FunGetNewId)(char *id);
	typedef int (*FunGetNewIdByParam)(int intareaId,int intGroupId, char *id);
	typedef int (*FunGetId)(int spId, int servId, int areaId, int groupId, char *id);


	//定义函数指针
	static FunGSInitialize			        pGSInitialize;	
	static FunUninitialize                 pUninitialize;
	static FunGetNewId				        pGetNewId;
	static FunGetNewIdByParam		        pGetNewIdByParam;
	static FunSendAccountAuthenRequest     pSendAccountAuthen;
	static FunSendAccountLockExRequest     pSendAccountLockEx;
	static FunSendAccountUnlockExRequest   pSendAccountUnlockEx;
	static FunSendAwardAuthenRequest       pSendAwardAuthen;
	static FunSendAwardAck                 pSendAwardAck;
	static FunSendTokenDepositRequest      pSendTokenDepositRequest;


	//定义回调函数结构体
	static GSCallbackFuncDef CallbackFun;

	static int GetFunction(HINSTANCE hDll);
	//设置回调函数
	static int SetCallBackFunction();		

	static int SendAuth();
};

