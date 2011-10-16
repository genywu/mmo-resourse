/*****
*	File       : DriveGameClient.h
*	Directions : Test产生的GameClient临时对象进行数据检测计算
*	Type	   : 主类(被class CDriveClient管理), 并管理DrivePlayer、DriveClientRegion
*	by		   : tanhaowen 
*	day		   : 2009.1.16 
*/
#ifndef _DriveGameClient_H_
#define _DriveGameClient_H_
#pragma once 
#include "../GameClient.h"
#include "DrivePlayer.h"
// 
// #include "..\GameClient.h"
// #include "..\Player.h"
// #include "..\ClientRegion.h"
// #include "..\..\Utility\TestMsgType.h"
// //#include "..\..\Net\MemServer.h"
// //#include "../../../nets/MemServer.h"
// //#include "AutoDriveClient.h"
// 
// class CDriveClientDlg;
// class CDriveGameClient;
// class CDrivePlayer;

//extern CRITICAL_SECTION g_csMapFile;

//由于都是继承Test已有数据，所以直接设计为使用多态，沿用已有数据
class CDriveGameClient : public CGameClient
{
private:
	//主驱动玩家
	CPlayer			 *m_pDrivePlayer;
	//主游戏场景			
	CClientRegion	 *m_pDriveRegion;

/*先设置为私有，后确实需要用到在公有*/	
private:										
	//创建单个主驱动玩家
	CPlayer			 *CreateMainPlayer()		{ return !m_pDrivePlayer ? new CDrivePlayer : m_pDrivePlayer; }
	//创建单个场景
	CClientRegion	 *CreateMainRegion()		{ return !m_pDriveRegion ? new CClientRegion : m_pDriveRegion; }							
	
	//设置主驱动玩家
	void			  SetDriveMainPlayer(CPlayer *pPlayer);  
	//设置主游戏场景
	void			  SetDriveMainRegion(CClientRegion *pRegion);

public:
	//获取主驱动玩家
	CPlayer			 *GetDriveMainPlayer()		{ return !m_pDrivePlayer ? m_pDrivePlayer : NULL; }
	//获取主游戏场景
	CClientRegion    *GetDriveMainRegion()		{ return !m_pDriveRegion ? m_pDriveRegion : NULL; }
	
//private:
	//保存MSG_FILE msg
	//msgQueue		  MsgQueueMSG_FILE;

//public:
//	void		AddMsgQueueMSG_FILE(CBaseMessage *pMsg)		{ MsgQueueMSG_FILE.push_back(pMsg); }
//	msgQueue   *GetMsgQueueMSG_FILE()						{ return &MsgQueueMSG_FILE; }
//	void		ReleaseMapFileMsg();						
//临时保存Camera数据并创建Camera Matrix
//public:
//	float		m_fCameraPosX;
//	float		m_fCameraPosY;
//	float		m_fCameraPosZ;
//
//	D3DXVECTOR3 m_vecPosition;
//	D3DXVECTOR3 m_vecView;
//
//	D3DXMATRIX  m_ViewMatrix;
//	D3DXMATRIX  m_ProjectionMatrix;
	
//---------------------------------------------------------------------------
//初始化数据
//----------------------------------------------------------------------------
public:
	//主玩家
	void			 InitPlayerData(CMessage *pMsg);
	//场景
	void			 InitGameRegionData(CMessage *pMsg);
	//场景格子的所有高度 ClientRegion::
	void			 InitGameRgionTileHeight(CMessage *pMsg);

//private:
	// MappingFile File Operation , Client to Test
	//void			 MsgFile(CMessage *pMsg);
	//void			 MsgFileWriteMappingFile(CMessage *pMsg);
	//void			 MsgFileClearMappingFile(CMessage *pMsg);
	//void			 MsgFileOpenMappingFile(CMessage *pMsg);
	//void			 MsgFileloseMappingFile(CMessage *pMsg);
	//
//------------------------------------------------------------------------------
//消息处理
//------------------------------------------------------------------------------
public:
	//实现原理 多态父类GameClient::OnMessage();
	void			 OnMessage(CMessage *pMsg);

	// MappingFile Msg, to do you function here
	//------------------------------------------
	
	// MSG_C2T_LOG
	void			 C2TLog(CMessage *pMsg);
	
	void			 C2TLogAnswerPlayerData(CMessage *pMsg);

	//MSG_C2T_RGN
	void			 C2TRgn(CMessage *pMsg);
				
	void			 C2TRgnAddShape(CMessage *pMsg);			// 有对象加入场景(对应MSG_S2C_RGN_ADDSHAPE = 783618)
	void			 C2TRgnDelShape(CMessage *pMsg);			// 有对象离开场景(对应MSG_S2C_RGN_DELSHAPE = 783620)
	void			 C2TRgnChange(CMessage *pMsg);				// 服务器命令玩家切换场景(对应MSG_S2C_RGN_CHANGE)
	void			 C2TRgnChangeServer(CMessage *pMsg);		// 服务器命令玩家切换场景服务器(对应MSG_S2C_RGN_CHANGESERVER)
	void			 C2TRgnChangeWeather(CMessage *pMsg);		// 改变天气
	void			 C2TRgnPlayAction(CMessage *pMsg);			// 播放动作
	void			 C2TRgnChangeName(CMessage *pMsg);			// 改变场景名字(对应MSG_S2C_RGN_CHANGENAME)
	void			 C2TRgnClearDupRgnPlayer(CMessage *pMsg);	// 副本清人消息(对应MSG_S2C_RGN_CLEARDUPRGNPLAYER)
	//MSG_L2T_Log
	void			L2TLog(CMessage *pMsg);
	//
	//MSG_C2T_LOGIC
	void			C2TLogic(CMessage *pMsg);
	
	//MSG_C2T_LOGIC_UIISOPENED
	void			C2TLogicUiIsOpend(CMessage *pMsg);
	
	// MSG_C2T_SHAPEE
	void			 C2TShape(CMessage *pMsg);
	
	void			 C2TShapeChangeDir(CMessage *pMsg);
	void			 C2TShapeSetPos(CMessage *pMsg);
	void			 C2TShapeForceMove(CMessage *pMsg);
	void			 C2TShapeMove(CMessage *pMsg);
	void			 C2TShapeStopMove(CMessage *pMsg);
	void			 C2TShapeMoveMode(CMessage *pMsg);
	void			 C2TShapeChangeState(CMessage *pMsg);
	void			 C2TShapeAtkDied(CMessage *pMsg);
	void			 C2TShapeSinState(CMessage *pMsg);
	void			 C2TShapeRefresh(CMessage *pMsg);
	void			 C2TShapeEmotion(CMessage *pMsg);
	
	void			 C2TShapeAtkBreak(CMessage *pMsg);
	void			 C2TShapeAtkDamage(CMessage *pMsg);
	void			 C2TShapeChangeProperties(CMessage *pMsg);
	
	
	// MSG_C2T_PLAYER
	void			 C2TPlayer(CMessage *pMsg);

	// MSG_C2T_GOODS
	void			 C2TGoods(CMessage *pMsg);
	
	//消息处理 end
public:
	//由于Client要对内存映射文件进行操作，所以传递MappingFile线程ID
	CDriveGameClient(unsigned uID);
	CDriveGameClient(Timer *pTimer, unsigned uID);
	~CDriveGameClient();
	
};
#endif//_DriveGameClient_H_