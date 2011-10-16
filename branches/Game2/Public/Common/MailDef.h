/**************************************************
*		文件名  ：MailDef.h
*		简要描述：邮件基本定义

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/

#ifndef _MAILDEF_H_
#define _MAILDEF_H_
#pragma once

class CGoods;

/// 普通物品
struct listGoods 
{
	CGUID goodsguid;
	long  lNum;
	long  lContainerID;
	long  lPos;
};


/// 定时器
struct tagTimer
{
	long lTimer;
	long lIntev;
	long lCount;
};

/// 发信条件
struct tagMailCondition
{
	string strName;		//条件名称
	long   lValue;		//值
	string strOperator;	//操作符
};

/// 发信事件
struct tagMailEvent
{	
	string  strEventName;
	long	lBeginTime;
	long    lEndTime;
};

/// 邮件保存状态
enum eSaveState
{
	MAIL_SAVE_INITVALUE = 0,//初始值
	MAIL_SAVE_SAVEING,		//保存中
	MAIL_SAVE_SUCCESS_SAVE,	//保存成功
};

/// 发信失败
enum eMAILRESULT
{
	MAIL_SENDING_SUCCESS					=0,			//成功
	MAIL_SENDING_FAILED_INVALID_POSTAGE		=1,			//邮资不够
	//MAIL_SENDING_FAILED_INVALID_SUBJECT		=2,		//主题包含非法字符
	//MAIL_SENDING_FAILED_INVALID_TEXT,					//内容包含非法字符
	//MAIL_SENDING_FAILED_INVALID_EXTEXT,				//附加内容包含非法字符
	MAIL_SENDING_FAILED_INVALID_GOLD,					//金币超出限制
	MAIL_SENDING_FAILED_INVALID_GOODS,					//物品超出限制
	MAIL_SENDING_FAILED_INVALID_MAXNAME,				//名字过长
	MAIL_SENDING_FAILED_INVALID_MAXSUBJECT,				//主题过长
	MAIL_SENDING_FAILED_INVALID_MAXTEXT,				//文字内容过长
	//MAIL_SENDING_FAILED_INVALID_MANYRECEIVER,			//不能在群发邮件中包含有物品金钱
	MAIL_SENDING_FAILED_INVALID_RECEIVER,				//非法收信人
	//MAIL_SENDING_FALLED_INVALID_SUBJECTORRECEIVER,	//无主题或收信人
	MAIL_SENDING_FAILED_INVALID_STATE,					//非法状态
	MAIL_SENDING_FAILED_INVALID_TYPE,					//邮件类型不符合
	//MAIL_SENDING_FAILED_INVALID_SELF,					//非法收信人
};

struct tagMailParam
{
	tagMailParam();
    uchar					bRead;						//信件查阅标志符	
	uchar					bReject;					//退信标志符（1标志可退,0表示不能退）
	uchar					bSave;						//保存标志符(0表示初始值，1表示未保存，2表示保存成功)
    long					lCost;						//邮资
	long					lGold;						//金币
    long					lType;						//邮件类型
	time_t					tRemainTime;				//保存最后时间	
	time_t					tEndTime;					//系统群发的结束时间
	time_t					tWriteTime;					//写信时间
    CGUID					guid;						//guid
    CGUID					WriterGuid;					//写信人的GUID
	CGUID					ReceiverGuid;				//收信人的GUID
	string					strReceiver;				//收信人
	string					strWriter;					//发信人	
	string					strSubject;					//主题
	string					strText;					//邮件内容
	string					strExText;					//邮件附加内容
    list<string>			strReceiverList;			//收信人列表
	list<listGoods*>		Goodslist;					//物品列表
	list<tagTimer*>			ltgTimer;					//定时器
	list<tagMailCondition*> lMailCondition;				//发信条件
	list<tagMailEvent*>		ltgMailEvent;				//发信事件
	vector<CGoods*>			vMailGoods;					//物品列表
};
#endif//_MAILDEF_H_



