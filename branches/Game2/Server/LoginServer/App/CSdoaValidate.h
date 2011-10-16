#pragma once

#include "../../../Public/ServerPublic/SDAPI/sdoa4serverdll/include/sdoa4server.h"



//!
class CSdoaValidate
{
public:
	CSdoaValidate(VOID);
	~CSdoaValidate(VOID);

public:
	//!					
	bool								Init(VOID);
	//!
	VOID								Release(VOID);

	//!									请求验证
	bool								RequestValidate(LPCSTR pToken, LONG lIP, LONG lClientSocket, LPCSTR pWSName);
	//!									处理验证结果
	VOID								OnValidateResult(tagSdoaValidateResult &SdoaValidateResult);
	//!									客户端断开连接
	VOID								OnClientDisConn(LONG lClientSocket);

private:
	//!									获取一个临时验证ID
	LONG								AllotTmpID(VOID);

private:
	//!									响应异步的验证结果返回
	static		int		SNDACALLBACK	ValidateReturn(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg);


private:
	//! 保存临时验证信息的结构
	struct tagTmpClientInfo
	{
		LONG	lIP;
		LONG	lSocket;
		char	szWSName[WS_NAME_SIZE];
		
		tagTmpClientInfo(VOID){memset(this, 0, sizeof(tagTmpClientInfo));}
		tagTmpClientInfo(LONG lInIP, LONG lInSocket, LPCSTR pWSName)
			:lIP(lInIP),lSocket(lInSocket)
		{strncpy(szWSName, pWSName, WS_NAME_SIZE);}
	};

private:
	//!									临时验证ID种子
	DWORD								m_dwIDSeed;
	//!									请求验证的客户端<临时ID，临时信息>
	map<DWORD, tagTmpClientInfo>		m_mapTmpClientInfo;
	//!									记录m_mapTmpClientInfo满的次数
	DWORD								m_TmpClientInfoFullCount;
	//!									Socket关联的临时ID<Socket, 临时ID>
	map<LONG, DWORD>					m_mapSocket2ID;

	//!									盛大SDK中的通行证验证对象
	ISDOAUserInfoAuthen					*m_pISDOAUserInfoAuthen;


};