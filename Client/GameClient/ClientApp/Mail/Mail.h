/**************************************************
*		文件名  ：Mail.h
*		简要描述：邮件规则逻辑

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/
#ifndef _MAIL_H_
#define _MAIL_H_

#pragma once

#include <list>
using namespace std;
#include "../Goods/Goods.h"
#include "../../../../Public/Common/BaseDef.h"
#include "../Player.h"



namespace mail{
	// 客户端定义的邮件信息结构
	struct	stMailData
	{
		CGUID			gMailID; 			// 邮件的ID
		DWORD			dwTimeStart;		// 接收邮件的开始时间(秒为单位)
		DWORD			dwTime;				// 邮件的有效时间(秒为单位)
		DWORD			dwGoldNum;			// 操作的金钱数
		bool			bIsReaded;			// 是否阅读过
		bool			bIsInDelList;		// 是否已被添加到了被删除列表中去
		bool			bIsCanReBack;		// 是否可以退信
		string			strMailName;		// 邮件名称
		string			strMailDesc;		// 邮件内容
		string			strMailDescEx;		// 邮件附加内容
		string			strSenderName;		// 邮件发送者名称
		long			colorTitle;			// 信件标题字体颜色
		long			colorSender;		// 信件发送者字体颜色
		long			colorGoodsEx;		// 附件物品遮罩的颜色
		list<CGoods*>	GoodsList;			// 附件物品列表
		eMAILTYPE		eMailType;			// 邮件的类型（3种：普通，普通物品，付款收信）
		stMailData()
		{
			gMailID = CGUID::GUID_INVALID;
			dwTimeStart = 0;
			dwTime	= 0;
			dwGoldNum = 0;
			bIsReaded = false;
			bIsInDelList = false;
			bIsCanReBack = true;
			strMailName = "";
			strMailDesc = "";
			strMailDescEx = "";
			strSenderName = "";
			colorTitle = 0xffffffff;
			colorSender = 0xffffffff;
			colorGoodsEx = 0xffffffff;
			eMailType = COMMON_MAIL;
		}
	}; 

	class CMail
	{
	public:
		CMail(void);
		~CMail(void);

	public:
		//static CMail *Instance();
		void	AddGoods( CPlayer::tagGoods* pstGoods );
		void	DecordDataFromByteArray( stMailData &stData,BYTE* pByte, long& pos );			// 邮件数据解码	
		void 	AddMail(stMailData &tagData);													// 添加一条邮件信息	
		void	FormatMailInfo(stMailData &tagData);											// 确定邮件信息的显示规则（字体颜色和标题的附加文字信息）		
		void	ReleaseMailGoods(stMailData &stData);											// 删除邮件的物品	
		void	DelMailByGuid(CGUID gMailID);													// 删除指定ID的邮件
		void	AddMailToDel(const unsigned int id);
		void	UpdataMail(stMailData& tagData);												// 更新指定的邮件信息(如果该邮件正在被查看，需要更新阅读界面的显示
		bool	CheckSendCondition( eMAILTYPE eMailType );										// 检查邮件发送条件是否达成，达成发信，否则不发信
		void	ClearData();																	// 消除已发送的邮件数据
		void    ClearAllMailData();
		void	DelMail();
		void	SendMail();
		
		
		//do...供PersonalMail.cpp调用
		void	DoSendMail();
		void	DoRecvMail();
		void	DoReadMail( const unsigned int index );
		void	DoDelMail();
		void	DoAgreePay(const unsigned int index);	
		void	DoGetGold();
		void	DoGetGoods(uint nMailIndex,uint nGoodsIndex);	
		void	DoRemoveGoods(const unsigned int index);														// 获取邮件物品时移除物品
		void	DoRejectMail(const unsigned int index);											//退信
		
		//set
		void	SetRecver( string& strName );
		void	SetTitle( string& strTitle ) 			{ m_strTitle = strTitle; }
		void	SetContent( string& strContent ) 		{ m_strContent = strContent; }
		void	SetGoldNum( const long lGoldNum ) 		{ m_lGoldNum = lGoldNum; }						// 寄送金额或者付款取信
		void	SetGoodsIndex( int nIndex )				{ m_nGoodsIndex = nIndex; }						// index通过服务器返回过来的消息获得
		void	SetMailType( bool bMailType )			{ m_bIsPayMail = bMailType; }
		void	SetCurReadMailGUID(CGUID& guid)			{ m_guidCurReadMail = guid; }
		void	SetMailBoxGUID(CGUID& guid)				{m_guidCurMailBox = guid;}							
			
		//get
		const int					GetGoodsIndex()	const		{ return m_nGoodsIndex; }	
		const CGUID&				GetCurReadMailGUID() const	{ return m_guidCurReadMail; }			// 设置当前操作的邮箱guid		
		const CGUID&				GetMailBoxGUID() const 		{ return m_guidCurMailBox; }
		vector<stMailData>&			GetMailData() 				{ return m_vecMailData; }	
		bool						GetCurReadMailData ( stMailData& mailData);					// 得到当前正在读的邮件数据	
		const int					GetPostage(const eMAILTYPE eMailType);
		eMAILTYPE					GetEMailType() const;
		stMailData*					GetMailByGUID(const CGUID& gMailID);	
		list<CPlayer::tagGoods>&	GetGoodsList()				{ return m_listGoods; }	
		vector<stMailData>&			GetDelMail() 				{ return m_vecDelMail; }

	private:	
		//static CMail*			m_pInstance;
		list<CPlayer::tagGoods>	m_listGoods;			// 附件列表	
		vector<stMailData>		m_vecMailData;			// 邮件列表	
		vector<stMailData>		m_vecDelMail;			// 待删除的邮件列表	
		CGUID					m_guidCurReadMail;		// 当前正在阅读的邮件ID	
		CGUID					m_guidCurMailBox;		// 当前正在操作的邮箱GUID
		vector<string>			m_vecRecver;
		string					m_strTitle;				// 邮件标题
		string					m_strContent;			// 邮件内容
		unsigned long			m_lGoldNum;				// 寄送金额或者付款取信
		int						m_nGoodsIndex;			// 物品索引(第几件物品)
		bool					m_bIsPayMail;
	};
	}

#endif
