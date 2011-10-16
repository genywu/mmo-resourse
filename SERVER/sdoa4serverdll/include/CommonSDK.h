#ifndef _EXTEN_COMMON_SDK_H_
#define _EXTEN_COMMON_SDK_H_

#define IN 
#define OUT

#if defined(_WIN32) || defined(_WIN64)
#define SNDACALL			__stdcall
#else
#define SNDACALL
#endif

#ifndef interface
#define interface	struct
#endif

#ifndef PURE
#define PURE		= 0
#endif

#define SNDACALLBACK		SNDACALL
#define SNDAMETHOD(Type)	virtual Type SNDACALL
#define SNDAAPI(Type)		extern "C" Type SNDACALL

interface ISDOCommonMsg
{
	SNDAMETHOD(int)	GetValue(IN const char* pKey, OUT void ** pValue, OUT int * pLen) PURE;
	SNDAMETHOD(int)	GetValue(IN const char* pKey, OUT char ** ppValue) PURE;
	SNDAMETHOD(int)	GetValue(IN const char* pKey, OUT int *pValue) PURE;
	
	SNDAMETHOD(void)	SetValue(IN const char* pKey, IN const void * pValue, IN int nLen) PURE;
	SNDAMETHOD(void)	SetValue(IN const char* pKey, IN const char* pValue) PURE;
	SNDAMETHOD(void)	SetValue(IN const char* pKey, IN int nValue) PURE;
	
	SNDAMETHOD(char *)	GetServiceName() PURE;
	
	SNDAMETHOD(void) Release() PURE;
};
SNDAAPI(ISDOCommonMsg*) sdoCreateRequestMsg(IN const char * szServiceName);
SNDAAPI(ISDOCommonMsg*) sdoCreateResponseMsg(IN const char * szServiceName);


typedef int (SNDACALLBACK *RequestCallBack)(unsigned int dwRpcId, int nResult,  const ISDOCommonMsg* pRequest);
typedef int (SNDACALLBACK *ResponseCallBack)(unsigned int dwRpcId, int nResult,  const ISDOCommonMsg* pResponse);
interface ISDOCommonHandler
{
	 SNDAMETHOD(int) Initialize(IN const char *pConfigFile, IN const RequestCallBack pReqCb, IN const ResponseCallBack pResCb) PURE;
	 SNDAMETHOD(void) Uninitialize() PURE;
	 
	 SNDAMETHOD(int) ASyncSendRequest(IN const ISDOCommonMsg* pRequest, OUT unsigned int *pRpcId) PURE;
	 SNDAMETHOD(int) SyncSendRequest(IN const ISDOCommonMsg* pRequest, OUT ISDOCommonMsg* pResponse) PURE;
	 
	 SNDAMETHOD(void) SendResponse(IN unsigned int dwRpcId,IN int nResult, IN const ISDOCommonMsg* pResponse) PURE;
	 
	 SNDAMETHOD(void) GetGUID(OUT char szGUID[33]) PURE;
	 SNDAMETHOD(void) Release() PURE;
};
SNDAAPI(ISDOCommonHandler*) sdoCreateCommonHandler();
#endif



