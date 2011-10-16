#pragma once

//Functin:完成会话功能，处理服务器与客户端的一次异步回调
//Athor:Joe
//Create Time:2004.6.17

class CNetSessionManager;

//异步调用结果
enum eAsyncResult
{
	AR_DO,				//调用
	AR_OK,				//调用成功
	AR_TimeOut,			//超时
	AR_Fail,			//失败
};

//异步调用结果数据结构
struct tagAsyncResult
{
	eAsyncResult	Result;		//结果
	va_list			Arguments;	//参数
};

//回调接口 
class IAsyncCallback
{
public:
	virtual	void OnAsyncCallback(tagAsyncResult&)	= 0;			    //异步调用中的回调函数,表现为在会话中返回消息时出发的函数，最后一次调用
	virtual	void Release() = 0;											//释放资源
};

//调用接口
class IAsyncCaller
{
public:
	virtual void DoAsyncCall(const CGUID& guid,long lPassWord,va_list) = 0;		//异步调用中的调用接口,该接口同步执行
	virtual void Release() = 0;											        //释放资源
};


class CNetSession
{
	//构造和析构只能由CNetSessionManager类来完成
	//friend class CNetSessionManager;

public:
	CNetSession(const CGUID& guid, pair<CGUID,long> CookiePair);
	~CNetSession(void);

public:
	const CGUID&	GetGUID()	{return m_guid;}							//得到唯一标识

	bool	CheckCookie(const CGUID& PlayerGuid,long lPassWord);		    //得到全局唯一的事列句柄 
	void	SetCallbackHandle(IAsyncCallback*);				                //设置回调函数句柄

	void	Beging(ulong dwTimeOut,IAsyncCaller*,...);		                //开始异步调用

	void	OnDo(va_list);									                //回调时处理
	void	OnResult(va_list);								                //回调结束发生时处理
	void	OnTimeOut();									                //超时处理

	ulong	GetLeftTime()	{return m_dwTimeOut;}
	void	SetLeftTime(ulong dwTime)	{m_dwTimeOut=dwTime;}

private:
	CGUID	            m_guid;											    //唯一标识
	pair<CGUID,long>	m_Cookie;							                //验证标识<PlayerID,randomID>
	ulong	            m_dwTimeOut;									    //超时时间
	IAsyncCallback*	    m_pCallback;							            //回调接口
};
