
/*
*	file:		PlayerSetupOnServer.h
*	Brief:		玩家的游戏设置，需要在服务器保存的部分
*	detail:		提供对象的编解码接口
*	Authtor:	张科智
*	Datae:		2008-06-30
*/

#pragma once

//! 数据对象（客户端可按需修改）
//###############################################################
struct tagSetupOnServer
{

};





//! 功能对象
//###############################################################
class PlayerSetupOnServer
{
public:
	PlayerSetupOnServer(void);
	~PlayerSetupOnServer(void);


public:
	//!								编码
	void							AddToByteArray		(vector<BYTE> &ByteArray);
	//!								解码
	void							GetFormByteArray	(BYTE *pByteArray, long &lPos);

	//!								编码
	void							CodeToDataBlock		(DBWriteSet& setWriteDB);
	//!								解码
	void							DecodeFromDataBlock	(DBReadSet& setReadDB);


	//!								重置对象（恢复默认设置）
	void							Reset				(void);

	//!								获得设置
	const	tagSetupOnServer*		GetSetup			(void);
	//!								修改设置
	BOOL							SetSetup			(const tagSetupOnServer &SetupOnServer);

private:
	//!								自动喝药的设置数据
	tagSetupOnServer				m_SetupOnServer;

};