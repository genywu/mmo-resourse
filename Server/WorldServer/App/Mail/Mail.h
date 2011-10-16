/**************************************************
*		文件名  ：Mail.h
*		简要描述：邮件规则逻辑

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/
#pragma once

#include "../../../Public/Common/MailDef.h"

class CMail
{
public:
	struct stTimerParam
	{
		//定时器ID
		long lTimerID;
		//预定触发时间
		long lFuture;
		//调用次数
		long lCount;
		//已调用次数
		long lCalledCount;
		//开始时间
		DWORD nStartTime;			
	};

	struct tagMail 
	{
		CGUID PlayerGuid;
		CGUID MailGuid;		
		long  lMailID;
	};
public:
	CMail(void);
	CMail(tagMailParam *pMailParam);
	virtual ~CMail(void);

	long	 				GetType()				{ return m_pBaseParam->lType;}	//邮件类型
	string&	  				GetWriter()				{ return m_pBaseParam->strWriter;}		    //发信人	
	string&	  				GetSubject()			{ return m_pBaseParam->strSubject;}			//主题
	string&	  				GetText()				{ return m_pBaseParam->strText;}			//邮件内容
	string&    				GetExText()				{ return m_pBaseParam->strExText;}		    //邮件附加内容
	time_t	  				GetRemainTime()			{ return m_pBaseParam->tRemainTime;}		//保存时间
	long	  				GetGold()				{ return m_pBaseParam->lGold;}				//金币
	BYTE	  				GetRead()				{ return m_pBaseParam->bRead;}				//信件查阅标志符
	BYTE					GetReject()				{ return m_pBaseParam->bReject;}			//退信标志符
	BYTE					GetSave()				{ return m_pBaseParam->bSave;}				//保存标志符
	CGUID&					GetWriterExID()		    { return m_pBaseParam->WriterGuid;}			//发信人GUID
	CGUID&					GetReceiverExID()		{ return m_pBaseParam->ReceiverGuid;}		//收信人GUID
	list<string>&			GetReceiverList()		{ return m_pBaseParam->strReceiverList;} 	//收信人列表
	list<listGoods*>&		GetCGood()				{ return m_pBaseParam->Goodslist;}			//普通物品列表	
	vector<CGoods*>&		GetMGoodsContainer()	{ return m_pBaseParam->vMailGoods;}			//获取物品列表
	list<tagMailCondition*> GetMailCondition()		{ return m_pBaseParam->lMailCondition;}		//邮件条件
	string&					GetReceiver()			{ return m_pBaseParam->strReceiver;}		//收信人	
	tagMailParam*			GetBaseParm()			{ return m_pBaseParam;}

	void					SetType(long lType)							{ m_pBaseParam->lType = lType;}
	void					SetWriter(const char* szWriter)				{ if(szWriter) m_pBaseParam->strWriter = szWriter;		}
	void					SetWriterID(const CGUID &guid)				{ m_pBaseParam->WriterGuid = guid;}
	void					SetSubject(const char* szSubject)			{ if(szSubject) m_pBaseParam->strSubject = szSubject;	}
	void					SetText(const char* szText)					{ if(szText) m_pBaseParam->strText = szText;			}
	void					SetExText(const char* szExText)				{ if(szExText) m_pBaseParam->strExText = szExText;}
	void					SetRemainTime(time_t	lRemainTime)	 	{m_pBaseParam->tRemainTime = lRemainTime;}	
	void					SetGold(long lGold)					 		{m_pBaseParam->lGold = lGold;}
	void					SetRead(BYTE bRead)					 		{m_pBaseParam->bRead = bRead;}
	void					SetReject(BYTE bReject)						{m_pBaseParam->bReject = bReject;}
	void					SetSave(BYTE bSave)							{m_pBaseParam->bSave = bSave;}
	void					SetReceiverList(const char* szRecer)		{ if(szRecer) m_pBaseParam->strReceiverList.push_back(string(szRecer));}
	void					SetReceiver(string strReceiver)				{m_pBaseParam->strReceiver = strReceiver;}
	void					SetReceiverExID(const CGUID &guid)			{m_pBaseParam->ReceiverGuid = guid;}
	void					SetMGoodsContainer(vector<CGoods*> &vGoods) {m_pBaseParam->vMailGoods = vGoods;}	
	void					SetWriteTime(time_t lTime)					{m_pBaseParam->tWriteTime = lTime;}			
	void					ClearMailGoodsContainer();
	bool					ClearMailGoodsByIndex(unsigned long lIndex);
	void					AddToByteArrayToGS(vector<BYTE>& vRet);
	void					AddMailMsgByteArrayToGS(vector<BYTE>& vRet);
	const CGUID&			GetExID(void)	{ return m_pBaseParam->guid; }
	void					SetExID(const CGUID& guid) { m_pBaseParam->guid = guid; }

private:
	tagMailParam *m_pBaseParam;			
};
