#ifndef __BSIP_FUN_RE_H__
#define __BSIP_FUN_RE_H__
#include "bsipmsg.h"


#ifdef WIN32
	#define BSIP_API_TYPE _declspec(dllexport)
#else
	#define BSIP_API_TYPE
#endif



typedef struct
{
	void (*CallbackAuthorRes)(GSBsipAuthorResDef *pMsg );  
	void (*CallbackAccountRes)(GSBsipAccountResDef *pMsg );
	int  (*BELoginReq)(const GSBsipLoginMsg * pMsg);
	int  (*BELogoutReq)( const GSBsipUser *pUSER,GSBsipSessionMsg *pSession,const int nStatus );
	int  (*BEChangeMapReq)( const GSBsipUser *pUser,const GSBsipSessionMsg *pSession, const int nMapId );
	int  (*BESendAuthorRequest)( const GSBsipAuthorReqDef *pMsg );
	int  (*BESendAccountRequest)( const GSBsipAccountReqDef *pMsg );
	void (*CallbackReAlert)(GSBsipAlertMsg *pMsg);
	int  (*BEGetNewId)( char *szId );
	int  nBEAccountInterval;/*账户扣费时间间隔*/
	char szLocalIP[IP_ADDRESS_LEN+1];
	char szLogPath[256];
}BECONTEXT;


#ifdef __cplusplus
extern "C" 
{
#endif
	BSIP_API_TYPE int BEStart( BECONTEXT *BEContext);
	BSIP_API_TYPE int BEStop();
#ifdef __cplusplus
}
#endif

#endif

