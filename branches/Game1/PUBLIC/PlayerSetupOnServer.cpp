
#include "StdAfx.h"
#include "public.h"
#include "PlayerSetupOnServer.h"


PlayerSetupOnServer::PlayerSetupOnServer(void)
{
	Reset();
}
PlayerSetupOnServer::~PlayerSetupOnServer(void)
{

}



//! 编码
void PlayerSetupOnServer::AddToByteArray(vector<BYTE> &ByteArray)
{
	_AddToByteArray(&ByteArray, &m_SetupOnServer, sizeof(tagSetupOnServer));
}

//! 解码
void PlayerSetupOnServer::GetFormByteArray(BYTE *pByteArray, long &lPos)
{
	_GetBufferFromByteArray(pByteArray, lPos, &m_SetupOnServer, sizeof(tagSetupOnServer));
}

//!								编码
void PlayerSetupOnServer::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(&m_SetupOnServer, sizeof(tagSetupOnServer));
}
//!								解码
void PlayerSetupOnServer::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	setReadDB.GetBufferFromByteArray(&m_SetupOnServer, sizeof(tagSetupOnServer));
}

//! 重置对象
void PlayerSetupOnServer::Reset(void)
{
	memset(&m_SetupOnServer, 0, sizeof(tagSetupOnServer));
}

//! 获得一个设置（uType）
const tagSetupOnServer* PlayerSetupOnServer::GetSetup(void)
{
	return &m_SetupOnServer;
}

//! 修改一个设置
BOOL PlayerSetupOnServer::SetSetup(const tagSetupOnServer &SetupOnServer)
{
	//! 相同的对象值不重新设置
	if(0 == memcmp(&m_SetupOnServer, &SetupOnServer, sizeof(tagSetupOnServer)))
		return FALSE;

	memmove(&m_SetupOnServer, &SetupOnServer, sizeof(tagSetupOnServer));
	return TRUE;
}