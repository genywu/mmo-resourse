#pragma once
#include <list>
#include <vector>

using namespace std;

#include "../serverClient.h"


//const int MAX_MESSAGE_LENGTH = 10240;


class CMyServerClient:
	public CServerClient
{
private:


	list<long>  m_listRegion	;		// 对应的场景ID

public:

	void PushRegionID(long l)	    	{m_listRegion.push_back(l);}
	void PopRegionID(long l)			{ m_listRegion.remove(l);}
	bool FindRegion(long l)        {return find(m_listRegion.begin(),m_listRegion.end(),l) != m_listRegion.end();}
	list<long>* GetListRegion()         {return &m_listRegion;}

	void AddRegionID(long lregionID){
		if(!FindRegion(lregionID))  m_listRegion.push_back(lregionID);}
	void DelRegionID(long lregionID)    {m_listRegion.remove(lregionID);}

public:
	CMyServerClient(void);
	~CMyServerClient(void);

	virtual void OnAccept(int errorCode=0);		// 连接成功时激活
	virtual void OnReceive(int nErrorCode=0);	// 有消息到的时候激活
	virtual void OnClose(int errorCode=0);		// 与对方断开的时候激活
	virtual void OnSend(int nErrorCode=0)	{};	// 有消息发送的时候激活
};
