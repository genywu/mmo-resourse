///////////////////////////////////////////////////////////////////////////////
///  Mail.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\Mail
///
///  CREATED: 09/12/2008  by 陈先进
///
///  PURPOSE:   邮件类
///
//////////////////////////////////////////////////////////////////////
#ifndef _MAIL_H_
#define _MAIL_H_
#pragma once

#include "../public/MailDef.h"
#include "../BaseObject.h"
class CMailManager;

class CMail : public CBaseObject
{
public:
	struct tagMail 
	{
		CGUID MailGuid;
	};

public:
	CMail(tagMailParam *pMailParam);
	virtual ~CMail(void);

	CGUID					  GetExID()			{ return m_pBaseParam->guid;}			 //GUID
	long					  GetType()			{ return m_pBaseParam->lType;}			 //邮件类型
	string					  GetWriter()		{ return m_pBaseParam->strWriter;}	     //发信人	
	string					  GetSubject()		{ return m_pBaseParam->strSubject;}		 //主题
	string					  GetText()			{ return m_pBaseParam->strText;}		 //邮件内容
	string				      GetExText()		{ return m_pBaseParam->strExText;}	     //邮件附加内容
	time_t					  GetRemainTime()	{ return m_pBaseParam->tRemainTime;}	 //保存时间
	long					  GetCost()			{ return m_pBaseParam->lCost;}   		 //邮资
	long					  GetGold()			{ return m_pBaseParam->lGold;}			 //金币
	BYTE					  GetRead()			{ return m_pBaseParam->bRead;}			 //信件查阅标志符
	BYTE					  GetReject()		{ return m_pBaseParam->bReject;}		 //退信标志符	
	list<tagMailEvent*>		  GetMailEvent()	{return m_pBaseParam->ltgMailEvent;}	 //系统邮件事件
	CGUID					  &GetWriterGuid()  { return m_pBaseParam->WriterGuid;}		 //写信人的GUID
	list<string>			  &GetReceiverList()		{return m_pBaseParam->strReceiverList;} //收信人列表
	list<tagCGoods*>		  &GetCGood()				{return m_pBaseParam->lCGoods;}			//普通物品列表
	list<tagSGoods*>		  &GetSGood()				{return m_pBaseParam->lSGoods;}			//增值道具列表	
	vector<CGoods*>			  &GetMGoodsContainer();											//获取物品容器
	list<tagMailCondition*>   GetMailCondition()	{return m_pBaseParam->lMailCondition;}		//发信条件
	string					  GetReceiver()			{return m_pBaseParam->strReceiver;}			//收信人
	CGUID					  GetReceiverGuid()		{return m_pBaseParam->ReceiverGuid;}		//收信人GUID
											
	void				SetType(long lType)							{ m_pBaseParam->lType = lType;}
	void				SetReject(BYTE bReject)						{ m_pBaseParam->bReject = bReject;}
	
	void				SetGold(long lGold)					 		{m_pBaseParam->lGold = lGold;}
	void				SetRead(BYTE bRead)					 		{m_pBaseParam->bRead = bRead;}
	void				SetExID(const CGUID& guid)					{m_pBaseParam->guid = guid;}
	void				SetReceiverList(const char* szRecer);			
	void				SetWriter(const char* szWriter)	;			
	void				SetSubject(const char* szSubject);			
	void				SetText(const char* szText);					
	void				SetReceiver(string strName)					{m_pBaseParam->strReceiver = strName;}

	void				SetSGoods(list<tagSGoods*> &lSGoods);	
	void				AddSGoodsToContainer(vector<CGoods*> &vGoods);									//添加增值道具
	bool				AddGoodsToContainer(CPlayer *pPlayer);											//添加物品到邮件物品容器
	bool				DeleteGoodsInContainer(CPlayer *pPlayer);										//删除背包中的物品

	bool				CheckGoodsContainer(CPlayer *pPlayer);											//检查物品容器
	void				AddGoodsToContainerBySys();														//系统类邮件在物品容器中添加物品
	void				SendGetGoodsMsg(CPlayer *pPlayer, long lIndex);									//发送从邮件物品容器中取物品到背包消息
	void				SendGetGoodsToClient(CGUID &MailGuid,const CGUID &PlayerGuid,long lIndex,BYTE bResult);//向客户端发送取物品消息
	void				GetGoodsFromContainer(CPlayer *pPlayer, long lIndex);							//从邮件物品容器中取物品到背包
	void				DeleteMailGoods();																//删除邮件中的物品

	void				DecordFromByteArray(BYTE* pByte, long& pos);
	void				AddToByteArrayForClient(vector<BYTE>& vRet,CPlayer *pPlayer);				
	void				AddToByteArrayForWS(vector<BYTE>& vRet,CPlayer* pPlayer);	
	void				CodeToDataBlockForClient(DBWriteSet& setWriteDB,CPlayer *pPlayer);				
	void				CodeToDataBlockForWS(DBWriteSet& setWriteDB,CPlayer *pPlayer);																
	bool				JudgeSysMailEventTime();														//判断系统邮件事件时间区域	
	void				GetSysSendTime(long &lBeginTime,long &lEndTime);								//获取系统群发类的发送时间段
	

private:
	tagMailParam *m_pBaseParam;	

};
#endif//_MAIL_H_