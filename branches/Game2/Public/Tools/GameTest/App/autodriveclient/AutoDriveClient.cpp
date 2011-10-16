#include "stdafx.h"
#include "AutoDriveClient.h"
//#include "../../../nets/netGT/message.h"
#include "..\..\Net\Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//静态单实列
CMemServer *CDriveClient::m_pMemServerManager = NULL;

CDriveClient::CDriveClient() : m_uID(0)
{	
	//m_pMemServerManager = new CMemServer; 
	GetMemServerInstance();
	//设置CMessage的MappingFile管理器
	CMessage::SetMemServerManager(GetMemServerInstance());
}

CDriveClient::~CDriveClient()
{
	_ReleaseGameClientManager();

	//释放MapFile资源
	//_ReleaseFileMapping();			//转移到Dlg释放内存块之前释放

	//释放MapFile管理指针对象
	SAFE_DELETE(m_pMemServerManager);
}

//创建一个GameClient
CGameClient *CDriveClient::_CreateGameClient(string cdkey, unsigned uID, Timer *timer)
{	
	//检测是GameClient否已经存在
	map<string, CGameClient*>::iterator it = m_mapGameClientManager.begin();
	for ( ; it != m_mapGameClientManager.end(); ++it)
	{	
		if ( cdkey.compare(it->first) == 0)
		{
			return NULL;
		}
	}		
	//根据传入的Timer来判断调用相应的构造，并放入游戏客户端管理器中
	m_uID = uID;
	CGameClient *gameclient = NULL;
	if ( timer == NULL )
	{	
		gameclient = new CDriveGameClient(uID);	
	}
	else
	{	
		gameclient = new CDriveGameClient(timer, uID);	
	}			
	m_mapGameClientManager.insert(make_pair(cdkey, gameclient));
	return gameclient;


}

//删除指定客户端
void CDriveClient::_DeleteGameClient(string cdkey)
{
	map<string, CGameClient*>::iterator it = m_mapGameClientManager.begin();
	for ( ; it!= m_mapGameClientManager.end(); ++it)
	{
		if ( cdkey == it->first)
		{	
			if (it->second != NULL)
			{
				SAFE_DELETE((*it).second);
			}

			m_mapGameClientManager.erase(it);
		}
	}
}

//查找指定客户端
CGameClient *CDriveClient::_FindGameClient(string cdkey)
{
	map<string, CGameClient*>::iterator it = m_mapGameClientManager.find(cdkey);
	if ( it!= m_mapGameClientManager.end() )
	{
		return ((*it).second);
	}

	return NULL;
}

//添加一个客户端
void CDriveClient::_AddGameClient(string cdkey, CGameClient *gameclient)
{	
	if ( gameclient == NULL )
	{
		return ;
	}

	map<string, CGameClient*>::iterator it = m_mapGameClientManager.find(cdkey);	
	if ( it == m_mapGameClientManager.end() )
	{
		m_mapGameClientManager.insert(make_pair(cdkey, gameclient));
	}
}

//清空客户端管理器
void CDriveClient::_ReleaseGameClientManager()
{
	if ( !m_mapGameClientManager.empty() )
	{
		map<string, CGameClient*>::iterator it = m_mapGameClientManager.begin();
		for ( ; it!= m_mapGameClientManager.end(); ++it)
		{
			if ((*it).second != NULL)
			{
				SAFE_DELETE((*it).second);
			}
		}
		m_mapGameClientManager.clear();
	}
}

//Create or Get CMemServerInstance
CMemServer	*CDriveClient::GetMemServerInstance()
{
	if ( m_pMemServerManager == NULL )
	{
		m_pMemServerManager = new CMemServer; 
	}
	return m_pMemServerManager;
}

//Create FileMapping
unsigned CDriveClient::_CreateFileMapping(const char *pFileobj, const char *pFileName, const char *pFileDir)
{	
	if ( pFileobj && pFileName && pFileDir )
	{	
		/*return m_uID = GetMemServerInstance()->AddServer(pFileobj, pFileName, pFileDir);*/
		//根据最新MemSrv进行修改
		m_uID = 111111;
		//GetMemServerInstance()->AddServer(m_uID, pFileobj, pFileName, pFileDir);
		return m_uID;
	}

	return 0;	
}

//init fileMapping
void CDriveClient::_InitialFileMapping(CDataBlockAllocator *pDBAllocator)
{	
	if (pDBAllocator != NULL )
	{	
		if ( !GetMemServerInstance()->Initial(pDBAllocator) )
		{
			::MessageBox(NULL,"初始化CMemServer内存块失败!", "DriveClient::MartySa", MB_YESNO);
		}
	}
}

//Write FileMapping
void CDriveClient::_WriteFileMapping(unsigned uID, CBaseMessage *pMsg)
{	
	if ( uID>0 && pMsg )
	{
		//GetMemServerInstance()->Send(uID, pMsg);
	}
}

//Release fileMaping resource
void CDriveClient::_ReleaseFileMapping()
{	
	if ( m_pMemServerManager )
	{	
		m_pMemServerManager->Release();
		SAFE_DELETE(m_pMemServerManager);
	}
}
