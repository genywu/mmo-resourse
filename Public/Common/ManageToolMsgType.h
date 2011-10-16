#pragma once
#include "MsgType.h"

enum eManageToolMsgType
{
	//SM->GAPP-------------------------
	MSG_SM2GAPP = MAX_TYPE * 36,
	MSG_SM2GAPP_OPERATOR = MSG_SM2GAPP + MAX_SUB * 0,
	MSG_SM2GAPP_TESTCONN,       //检测延迟
	MSG_SM2GAPP_SHUTDOWN,

	MSG_SM2GAPP_GET_GSPLAYERS,  //获取GS在线玩家														
	
	MSG_SM2GAPP_ACTION,
	MSG_SM2GAPP_GET_MAINAITICK,

    MSG_SM2GAPP_WATCH,
    MSG_SM2GAPP_REPORT_Result,
    MSG_SM2GAPP_WS_SENDTOPNEWS,
    MSG_SM2GAPP_MANAGE_Request_Gm_Command,

	//SM->GAPP-------------------------
    //GAPP->SM-------------------------
	MSG_GAPP2SM = MAX_TYPE * 37,
	MSG_GAPP2SM_OPERATOR = MSG_GAPP2SM + MAX_SUB * 0,
	MSG_GAPP2SM_TESTCONN,
	MSG_GAPP2SM_WRITEFILE,      //写文件
	MSG_GAPP2SM_CLEARFILE,      //清空文件
	MSG_GAPP2SM_OPENFILE,       //打开文件
	MSG_GAPP2SM_CLOSEFILE,      //关闭文件


	MSG_GAPP2SM_GAPP_SENDINFO,  //通过网络连接SM
	MSG_GAPP2SM_GAPP_INITOK,    //GAPP初始化成功
	MSG_GAPP2SM_GAPP_STARTOK,   //GAPP启动成功
	MSG_GAPP2SM_SHUTDOWN,       //GAPP关闭
	MSG_GAPP2SM_ACTION,
	MSG_GAPP2SM_GET_GSPLAYERS,
	MSG_GAPP2SM_SEND_MAINAITICK,
	MSG_GAPP2SM_SENDINFO,
    MSG_GAPP2SM_REPORTINFO,
    MSG_GAPP2SM_WATCH_INFO_Reply,
    MSG_GAPP2SM_WATCH_INFO,
    MSG_GAPP2SM_REPORT_Result_Reply,
    MSG_GAPP2SM_MANAGE_Request_Gm_Command

};