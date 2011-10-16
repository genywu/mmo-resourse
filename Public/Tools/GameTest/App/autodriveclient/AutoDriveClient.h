/*****
*	File       : AutoDriveClient.h
*	Directions : Test驱动Client类头文件
*	Type	   : 管理类(管理DriveGameClient类、CMemclient类)
*	by		   : tanhaowen 
*	day		   : 2009.1.16 
*/

#pragma once 

#include "DriveGameClient.h"
#include "DrivePlayer.h"
#include "DriveGameClient.h"

#include "..\Player.h"
#include "..\ClientRegion.h"


class CDriveGameClient;
class CDrivePlayer;
class CDriveRegion;
class CMemServer;

class CDriveClient
{
private:
	map<string, CGameClient *>	     m_mapGameClientManager;												//客户端管理(多个),根据帐号进行管理 

public:	

	map<string, CGameClient *>      &_GetGameClientManager()			{ return m_mapGameClientManager; }  //客户端管理功能
	CGameClient						*_FindGameClient(string cdkey);											//查找
	void							 _DeleteGameClient(string cdkey);										//删除单个
	void							 _AddGameClient(string cdkey, CGameClient *gameclient);					//添加单个	
	CGameClient						*_CreateGameClient(string cdkey, unsigned uID, Timer *timer = NULL);	//创建客户端,创建GameClient 使用多态	
	void							 _ReleaseGameClientManager();											//释放管理器,清空						

	//***
	//	Mapping File
	//*
private:

	static CMemServer				*m_pMemServerManager;													//MappingFile管理指针对象,不要释放!	
	unsigned						 m_uID;																	//保存创建的sever映射文件的线程ID
public:

	CMemServer						*GetMemServerManager()		{ return m_pMemServerManager; }
	static CMemServer				*GetMemServerInstance();
public:

	//Create fileMapping
	unsigned						_CreateFileMapping(const char *pFileobj, 
		const char *pFileName,
		const char *pFileDir);
	//init fileMapping
	void							 _InitialFileMapping(CDataBlockAllocator *pDBAllocator);
	//Write fileMapping
	void							 _WriteFileMapping(unsigned uID, CBaseMessage *pMsg);
	//Get fileMappingMsgQueue
	CMsgQueue						*_GetRecvMsgs()				{ return m_pMemServerManager->GetRecvMsgs(); }
	//Release fileMaping resource
	void							 _ReleaseFileMapping();
	//get fileMapping uID
	unsigned						 _GetUId()					{ return m_uID; }

public:

	CDriveClient();
	~CDriveClient();
};