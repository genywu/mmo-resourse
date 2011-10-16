


#include "../../sdoa4serverdll/include/sdoa4server.h"
#include "../../sdoa4serverdll/include/ISDOAuthBusiness.h"
#include "../../sdoa4serverdll/include/ISDOAuthCallBack.h"

#pragma once

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
	//!                                 初始化补全资料接口
	VOID                                OnInitAuthBusiness();
	//!                                 获取信息资料
	VOID                                OnAsyncGetUserInfo(string strUserId,bool IsFirst=true,long lSocketID=0);
	//!                                 通过RpcId获得玩家cdkey
	string                              GetUserCDKey(int dwRpcId);
	//!                                 获取注册信息
	VOID                                OnAsyncGetUserInfo(tagSdoaValidateResult SdoaValidateResult);
	//!                                 通过RpcId查找验证结果并进行后续验证
	VOID                                OnValidateByRpcId(int dwRpcId);

	VOID                                MapTokenSocketID(string szToken,long SocketID);

	long                                GetSocketIDByCDKey(string szCDKey);
	string                              GetCDKeyBySocketID(long SocketID);

private:
	//!									获取一个临时验证ID
	LONG								AllotTmpID(VOID);

private:
	//!									响应异步的验证结果返回
	static		int		SNDACALLBACK	ValidateReturn(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg);
	//!									响应异步的验证结果返回(补全资料)
	static		int		SNDACALLBACK	ValidateReturnEx(int nResult, unsigned long nUniqueId, ISDOAMsg *pMsg);
	//!                                 响应玩家资料异步读取的返回
	static      void    SNDACALLBACK    GetUserInfoCB(unsigned int dwRpcId,int nResult, SKeyValue *pKeyValue, int nKeyValueNum);

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

	//!                                 盛大资料补全句柄
	ISDOAuthBusiness                    *m_pISDOAuthBusiness;

	//!                                 记录用户id
	map<unsigned int,string>             mapRpcUserId;

	//！                                 盛大认证结果的集合
	map<DWORD,tagSdoaValidateResult>        m_mapSdoaValidateResult;

	//!                                 用于存储数字帐号和SocketID的影射关系
	map<string,long>                    m_mapTokenSocketID;
	map<long,string>                    m_mapSocketTokenID;


};

