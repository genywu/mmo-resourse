



#ifndef _MailMgr_H_
#define _MailMgr_H_
#pragma once
#include "../Utility/PlayerDef.h"
#include "../Utility/gtDef.h"

class CMessage;

class CMailMgr
{
public:
	CMailMgr(void);
	~CMailMgr(void);

	void	Release();
	// ! 清空收件箱保存数据	
	void    ClearMailData();																	 		   

	//	@ 是否得到数据
	bool	IsGetData(void)		{ return m_bIsGetData; }
	void	SetGetData(bool b)	{ m_bIsGetData = b; }

	//	@	删除邮件物品
	void	ReleaseMailGoods(stMailData& stMail);

	//	@	邮件解码
	void DecodeFromByteArray(stMailData& stMail,BYTE* pBytes,long& lpos);

	//	@ 更新邮件数据
	void RefreshData();
public:
	//**********************************收件箱相关方法***************************************//
	// By MartySa
	// 添加一条邮件信息
	void 	AddMail(stMailData &tagData);

	// 删除指定ID的邮件
	void	DelMailByGuid(const CGUID &gMailID);

	// 删除选中的所有邮件(清空待删除的邮件列表)
	//void	DelAllSelectedMail();

	// 确定邮件信息的显示规则（字体颜色和标题的附加文字信息）
	void	FormatMailInfo(stMailData &tagData);

	// 确定邮件的时间(返回描述以及颜色)
	void	FormatMailTime(stMailData &tagData, string &str, long &color);

	// 获得指定ID的邮件
	stMailData*	GetMailByGUID(const CGUID &gMailID);

	//**********************************发件箱相关方法***************************************//
	// By MartySa
	// 删除指定索引的附加物品
	void	DelGoodsEx(const int nIndex);
	//********************************** For Lua 接口*******************************************
	
	void	AddSendToPlayerName(const string &name)   { m_vecSendToPlayerName.push_back(name); m_lSendMoney+=10; } //添加名字
	
	void   AddSendToNorGoods(const tagGoods *taggoods);												//添加附件物品

	bool	SendPlayerName(CMessage *&msg);																//发送名字
	
	bool   SendNorGoods(CMessage *&msg, BYTE mailtype);												//发送物品

	void	ClearAllSendData();																			//清空数据，下次用

	long	GetSendMoney();																				//计算发送的钱数
	
	long	GetSendToPlayerSize()              { return (long)m_vecSendToPlayerName.size(); } //求当前联系人人数

	long   GetSendGoodsSize()			       { return (long)m_listSendToGoods.size(); }	 //求当前发送物品数量

	CGUID   &GetMailDataGuid(long index)       { return m_vecMailData[index].gMailID; }		 //获取收件箱邮件列表
	long    GetMailDataSize()				   { return (long)m_vecMailData.size(); }		 //获取收件箱邮件数目

	//2009.1.6 新加，用于邮件数据保存检查使用 MartySa
	vector<stMailData>	&GetMailData()			   { return m_vecMailData; }



private:

	// 是否已经获得了邮箱数据
	bool			   m_bIsGetData;

	// 邮件提示信息类型（目前只有发送和删除会产生提示）
	BYTE			   m_bTipType;

	// 邮件列表
	vector<stMailData> m_vecMailData;

	// 待删除的邮件列表
	vector<stMailData> m_vecDelMailData;

	// 收件人姓名列表
	vector<string>		m_vecSendToPlayerName;

	// 即将发送的附件物品列表
	list<stGoodsInfo>	m_listSendToGoods;

	// 邮件类型
	eMAILTYPE			m_eMailType;

	// 物品类型
	eMailGoodsType		m_eGoodsType;

	//	钱
	long				m_lSendMoney;
};
#endif//_MailMgr_H_