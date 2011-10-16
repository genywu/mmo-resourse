#include "stdafx.h"
#include "Mail.h"

#include "MailDef.h"
#include "../../Game.h"
#include "../../../UI/UIDef.h"
#include "../../../Public/Common/Public.h"
#include "../Other/GlobeSetup.h"
#include "../ClientWordsFilter.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../../Engine/Utility.h"

namespace mail{
void  DoAgreePaySel(MsgEvent& event,eMsgResult UserId);
void  DeleteMail(MsgEvent& event,eMsgResult UserId);
void  DoAgreeSend(MsgEvent& event,eMsgResult UserId);

CMail::CMail(void)
{
	m_guidCurMailBox = CGUID::GUID_INVALID;
	m_nGoodsIndex = -1;
	m_bIsPayMail = false;
	m_lGoldNum = 0;
	m_nGoodsIndex = -1;
}

CMail::~CMail(void)
{
	/*list<CPlayer::tagGoods>::iterator it = m_listGoods.begin();
	for (int i = 0; it != m_listGoods.end(); ++it,++i)
	{
		(*it).pGoods->UnLock();
		SAFEDELETE(it->pGoods);
	}*/
	ClearData();
}

// CMail *CMail::m_pInstance = NULL;
// CMail *CMail::Instance()
// {
// 	if ( !m_pInstance )
// 	{
// 		m_pInstance = new CMail;
// 	}
// 	return m_pInstance;
// }

// 添加附件物品(PackageEvent.cpp调用)
void CMail::AddGoods( CPlayer::tagGoods* pstGoods )
{
	// 物品存在且不是锁定的
	if( pstGoods->pGoods && !pstGoods->pGoods->IsLocked())
	{
		// 第一次添加物品
		if(m_listGoods.empty())
		{
			if(m_vecRecver.size() > 1)
			{
				//带有附件物品或金币的信件不能群发！
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_24"));
				return;
			}			
		}
		if(m_listGoods.size() == 4)
		{		
			GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_33"));
			return;
		}
		pstGoods->pGoods->Lock();
		CPlayer::tagGoods stGoods;
		stGoods.pGoods = pstGoods->pGoods;
		stGoods.lPos = pstGoods->lPos;
		stGoods.lType = pstGoods->lType;	
		m_listGoods.push_back(stGoods);
		
		FireUIEvent(GETGOODSCONTAINER,EVENT_ADDGOODS);
	}
}


void CMail::DoSendMail()
{
	eMAILTYPE eMailType = GetEMailType();	
	if(!CheckSendCondition( eMailType ))
	{	
		FireUIEvent(ADDGOODSCONTAINER,EVENT_UPDATA);
		ClearData();
		return;
	}

	if(m_vecRecver.size() > 1)
	{	
		char stText[256]="";
		DWORD dwPostage = GetPostage(eMailType);	
		sprintf(stText, AppFrame::GetText("Mail_29"),dwPostage);
		//您确定要花费%d银币群发此邮件？
		GetInst(MsgEventManager).PushEvent(Msg_YesNo,stText,DoAgreeSend);
		return;
	}

	SendMail();
	FireUIEvent(ADDGOODSCONTAINER,EVENT_UPDATA);
	ClearData();
	
	return;
}

void  DoAgreeSend(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		GetInst(CMail).SendMail();
	}
	FireUIEvent(ADDGOODSCONTAINER,EVENT_UPDATA);
	GetInst(CMail).ClearData();
}


void CMail::SendMail()
{
	eMAILTYPE eMailType = GetEMailType();
	long lGoodsNum = (long)m_listGoods.size();
	CMessage msg(MSG_C2S_MAIL_SEND);
	msg.Add(m_guidCurMailBox);
	size_t size = m_vecRecver.size();
	msg.Add((long)size);
	// 收信人名字
	for (size_t i = 0; i < size;i++)
	{
		msg.Add(m_vecRecver[i].c_str());
	}

	msg.Add(m_strTitle.c_str());
	msg.Add(m_strContent.c_str());
	msg.Add((long)m_lGoldNum);
	msg.Add((byte)eMailType);
	msg.Add(lGoodsNum);
	if(!m_listGoods.empty())
	{
		list<CPlayer::tagGoods>::iterator it = m_listGoods.begin();	
		for (; it != m_listGoods.end(); ++it)
		{
			msg.Add((*it).pGoods->GetExID());
			msg.Add((long)(*it).pGoods->GetNum());
			msg.Add((DWORD)(*it).lType);
			msg.Add((DWORD)(*it).lPos);
		}		
	}
	msg.Send();
	GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_37"));
}
void CMail::ClearData()
{
	m_vecRecver.clear();
	m_strTitle.clear();
	m_strContent.clear();
	
	for (list<CPlayer::tagGoods>::iterator it = m_listGoods.begin();it != m_listGoods.end();it++)
	{
		it->pGoods->UnLock();
	}
	m_listGoods.clear();
	m_lGoldNum = 0;
	m_nGoodsIndex = -1;
	m_bIsPayMail = false;
}
eMAILTYPE CMail::GetEMailType() const
{
	if ( m_bIsPayMail )
	{
		return PAYGOLD_MAIL;
	}
	else if(!m_listGoods.empty() || m_lGoldNum)
	{
		return AFFIX_MAIL;
	}
	else
	{
		return COMMON_MAIL;
	}
}
void CMail::DoRecvMail()
{
	CMessage msg(MSG_C2S_MAIL_RECEIVER);
	msg.Send();
}


// 邮件数据解码
void CMail::DecordDataFromByteArray(stMailData &stData,BYTE* pByte, long& pos)
{
	char str[102400]="";
	_GetBufferFromByteArray(pByte,pos,stData.gMailID);
	stData.dwTime = _GetDwordFromByteArray(pByte, pos);
	stData.dwGoldNum = _GetDwordFromByteArray(pByte, pos);
	stData.strMailName = _GetStringFromByteArray(pByte, pos, str);
	stData.strMailDesc = _GetStringFromByteArray(pByte, pos, str);
	stData.strMailDescEx = _GetStringFromByteArray(pByte, pos, str);
	stData.strSenderName = _GetStringFromByteArray(pByte, pos, str);
	stData.bIsReaded = _GetByteFromByteArray(pByte, pos)>0?true:false;
	stData.eMailType = (eMAILTYPE)_GetByteFromByteArray(pByte, pos);
	stData.bIsCanReBack = _GetByteFromByteArray(pByte, pos)>0?true:false;
	// 普通道具
// 	else
// 	{
		long lGoodssize = _GetLongFromByteArray(pByte, pos);
		for (int i=0; i<lGoodssize; ++i)
		{
			CGoods *pGoods=new CGoods;
			if(pGoods)
			{
				if( pGoods->DecordFromByteArray(pByte,pos) )
				{
					stData.GoodsList.push_back(pGoods);
				}
				else
				{
					SAFE_DELETE(pGoods);
				}
			}
		}
//	}
}
// 添加一条邮件信息
void CMail::AddMail(stMailData& tagData)
{
	vector<stMailData>::iterator it = m_vecMailData.begin();
	for (; it!=m_vecMailData.end(); ++it)
	{
		if(it->gMailID == tagData.gMailID)
			return;
	}
	FormatMailInfo(tagData);
	stMailData stData;
	stData.gMailID			= tagData.gMailID;
	stData.eMailType		= tagData.eMailType;
	stData.bIsReaded		= tagData.bIsReaded;
	stData.bIsCanReBack		= tagData.bIsCanReBack;
	stData.dwGoldNum		= tagData.dwGoldNum;
	stData.dwTime			= tagData.dwTime;
	
	stData.GoodsList		= tagData.GoodsList;
	
	stData.strMailDesc		= tagData.strMailDesc;
	stData.strMailDescEx	= tagData.strMailDescEx;
	stData.strMailName		= tagData.strMailName;
	stData.strSenderName	= tagData.strSenderName;
	stData.colorTitle		= tagData.colorTitle;
	stData.colorSender		= tagData.colorSender;
	stData.colorGoodsEx		= tagData.colorGoodsEx;
	stData.dwTimeStart		= timeGetTime()/1000;
	// 未阅读的放在最前面
	if(!stData.bIsReaded)
		m_vecMailData.insert(m_vecMailData.begin(),stData);
	else
		m_vecMailData.push_back(stData);

	
	// 如果正在阅读邮件，更新阅读界面的邮件信息显示
	//CReadMailPageEx * pReadMailPage = GetGame()->GetCGuiEx()->GetReadMailPageEx();
	//if(pReadMailPage && pReadMailPage->IsOpen())
	//	pReadMailPage->UpdataMailData(m_guidCurReadMailID);
}


// 确定邮件信息的显示规则（字体颜色和标题的附加文字信息）
void CMail::FormatMailInfo(stMailData &tagData)
{
	if(tagData.gMailID == CGUID::GUID_INVALID)
	{
		return;
	}
	// 默认白色
	tagData.colorSender = tagData.colorTitle = 0xffffffff;
	tagData.colorGoodsEx = 0x00ffffff;

	if(tagData.eMailType == SYSTEM_MAIL)
	{
		tagData.colorSender = 0xfffff600;
	}
	// 邮件被阅读过，主题为灰色
	if(tagData.bIsReaded)
	{
		tagData.colorTitle = 0xff949494;
		// 非系统邮件附件标示为灰色朦板
		if(tagData.eMailType != SYSTEM_MAIL)
		{
			tagData.colorGoodsEx = 0x64949494;
		}
	}
}

void CMail::DoReadMail(const unsigned int index)
{
	CMessage msg(MSG_C2S_MAIL_READ);
	msg.Add(m_guidCurMailBox);

	if ( m_vecMailData.size() >= index + 1 )
	{
		msg.Add(m_vecMailData[index].gMailID);
		msg.Send();
	}	
}
// 获得指定ID的邮件
stMailData* CMail::GetMailByGUID(const CGUID& gMailID)
{
	vector<stMailData>::iterator it = m_vecMailData.begin();
	for (; it!=m_vecMailData.end(); ++it)
	{
		if(it->gMailID == gMailID)
		{
			return &(*it);
		}
	}
	return NULL;
}

void CMail::DoDelMail( )
{
	WORD wSize = (WORD)m_vecDelMail.size();


	if ( wSize < 0 )
	{
		//to do: 请选择需要删除的邮件 
	}
	for ( int i = 0; i < wSize; i++ )
	{
		if (m_vecDelMail[i].eMailType == PAYGOLD_MAIL )
		{
			//付费类信件无法删除！
			GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_15")); 
			m_vecDelMail.clear();
			return;
		}
	}

	for ( int i = 0; i < wSize; i++ )
	{
		if (m_vecDelMail[i].GoodsList.size() > 0)
		{
			//您选中要删除的信件当中含有物品附件，请确认是否删除？
			GetInst(MsgEventManager).PushEvent(Msg_YesNo,AppFrame::GetText("Mail_16"),DeleteMail); 
			return;
		}
	}
	
	DelMail();
	
}

void DeleteMail(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		GetInst(CMail).DelMail();
	}
	GetInst(CMail).GetDelMail().clear();
}

void CMail::DelMail()
{
	CMessage msg(MSG_C2S_MAIL_DELETE);
	msg.Add(m_guidCurMailBox);
	WORD wSize = (WORD)m_vecDelMail.size();
	msg.Add(wSize);
	vector<stMailData>::iterator it = m_vecDelMail.begin();
	for (; it!=m_vecDelMail.end(); ++it)
	{
		msg.Add(it->gMailID);
	}
	msg.Send();
	m_vecDelMail.clear();
}
// 删除邮件的物品
void CMail::ReleaseMailGoods(stMailData &stData)
{
	// 清空以前的附件
	list<CGoods *>::iterator it = stData.GoodsList.begin();
	for (; it != stData.GoodsList.end(); ++it)
	{
		SAFE_DELETE((*it));
	}
	stData.GoodsList.clear();
}



// 删除指定ID的邮件
void CMail::DelMailByGuid(CGUID gMailID)
{
	vector<stMailData>::iterator it = m_vecMailData.begin();
	for (; it!=m_vecMailData.end(); ++it)
	{
		if(it->gMailID == gMailID)
		{
			// 删除邮件是当前正在阅读的邮件，关闭阅读界面
			//if(m_guidCurReadMailID == gMailID)
			//	GetGame()->GetCGuiEx()->GetReadMailPageEx()->Close();
			ReleaseMailGoods((*it));
			m_vecMailData.erase(it);
			return;
		}
	}
}

void CMail::AddMailToDel(const unsigned int id)
{
	if ( m_vecMailData.size() >= id+1 )
	{
		m_vecDelMail.push_back( m_vecMailData[id] );
	}
}

bool CMail::GetCurReadMailData(stMailData& mailData)
{
	for ( unsigned int i = 0; i < m_vecMailData.size(); i++ )
	{
		if ( m_vecMailData[i].gMailID == m_guidCurReadMail )
		{
			mailData =  m_vecMailData[i];
			return true;
		}
	}
	return false;
}

// 获得指定索引的附加物品（包括金币）
void CMail::DoGetGoods(uint nMailIndex,uint nGoodsIndex)
{
	if ( m_vecMailData.size() <= nMailIndex )
	{
		//to do:add log
		return;
	}
	if(nGoodsIndex < 0 || nGoodsIndex >= m_vecMailData[nMailIndex].GoodsList.size())
	{
		//to do:add log
		return;
	}
	// 付款收信型
	if(m_vecMailData[nMailIndex].eMailType == PAYGOLD_MAIL && m_vecMailData[nMailIndex].dwGoldNum > 0)//是不是点了同意付费就把 dwGoldNum 清0 了？
	{
		// 不能获取物品，请点击付款收信按钮！
		 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_5"));
		 return;
	}
	// 属于普通附件或系统邮件
	else if(m_vecMailData[nMailIndex].eMailType == AFFIX_MAIL || m_vecMailData[nMailIndex].eMailType == SYSTEM_MAIL)
	{
		CMessage msg(MSG_C2S_MAIL_GET_GOODS);
		msg.Add(m_guidCurMailBox);
		msg.Add(m_vecMailData[nMailIndex].gMailID);
		msg.Add((long)nGoodsIndex);
		msg.Send();
	}
}


 void CMail::SetRecver( string& strName )
 {
	unsigned int pos = 0;
	while (!strName.empty())
	{
		pos = (unsigned int)strName.find(";");
		if(string::npos != pos)
		{
			string strName = strName.substr(0,pos);
			if(!strName.empty())
			{
				m_vecRecver.push_back(strName);
			}
			strName = strName.substr(pos+1);
		}
		else
		{
			if(!strName.empty())
			{
				m_vecRecver.push_back(strName);
				break;
			}
		}
	}
 }

 // 更新指定的邮件信息(如果该邮件正在被查看，需要更新阅读界面的显示)
 void CMail::UpdataMail(stMailData& tagData)
 {
	 vector<stMailData>::iterator it = m_vecMailData.begin();
	 for (int i=0; it!=m_vecMailData.end(); ++it,++i)
	 {
		 if(it->gMailID == tagData.gMailID)
		 {
			 FormatMailInfo(tagData);
			 it->eMailType		= tagData.eMailType;
			 it->bIsReaded		= tagData.bIsReaded;
			 it->dwGoldNum		= tagData.dwGoldNum;
			 it->dwTime			= tagData.dwTime;
			 it->GoodsList		= tagData.GoodsList;
			 it->strMailDesc	= tagData.strMailDesc;
			 it->strMailDescEx	= tagData.strMailDescEx;
			 it->strMailName	= tagData.strMailName;
			 it->strSenderName	= tagData.strSenderName;
			 it->colorTitle		= tagData.colorTitle;
			 it->colorSender	= tagData.colorSender;
			 it->colorGoodsEx	= tagData.colorGoodsEx;
			 // 更新收件箱列表的界面显示
			// UpdataInBoxMailItem((*it),i);
			 return;
		 }
	 }
 }
 // 检查邮件发送条件是否达成，达成发信，否则不发信
 bool CMail::CheckSendCondition(eMAILTYPE eMailType)
 {
	 CPlayer *pPlayer = GetGame()->GetMainPlayer();
	 if( pPlayer )
	 {
		 DWORD dwPostage = GetPostage(eMailType);	
		 //if(pPlayer->GetSilverMoney() < dwPostage)
		 //{	
			// //邮资不够，发送失败
			// GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_51"));
			// return false;
		 //}
		 // 检查信件相关字符是否存在非法字符 
		 if(m_strTitle.empty() || m_vecRecver.size() == 0 )
		 {
			//没有收信人或主题，发送失败！			 
			GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_61"));
			return false;
		 }
		 if( ! CClientWordsFilter::GetInstance()->Check(m_strTitle, 1, 1) )
		 {
			 //主题包含非法字符，发送失败！
			 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_52"));
			 return false;
		 }
		 if( ! CClientWordsFilter::GetInstance()->Check(m_strContent, 1, 1) )
		 {
			 //内容包含非法字符，发送失败！ 
			 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_53"));
			 return false;
		 }
		 vector<string>::iterator it = m_vecRecver.begin();
		 for( ;it != m_vecRecver.end();it++ )
		 {
			 if ((*it) == pPlayer->GetName())
			 {
				 //对不起，不能给自己发送邮件！			 
				 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_27"));
				 return false;
			 }		
		 }

		 if(eMailType == PAYGOLD_MAIL)
		 {
			 if ( m_lGoldNum <= 0 )
			 {
				 //请正确输入付款收信的金额！	
				 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_32"));
				 return false;
			 }
			 return true;
		 }
		 // 普通附件
		 if(eMailType == AFFIX_MAIL)
		 {
			 if(pPlayer->GetMoney() < m_lGoldNum)
			 {
				 // 您的金币不足，发送失败！				 
				 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_13"));
				 return false;
			 }		
			 return true;
		 }
		 // 群发类邮件
		 size_t size = m_vecRecver.size();
		 //群发类邮件不能赠送物品或金钱
		 if (size >= 2)
		 {
			 if (m_lGoldNum > 0 || m_listGoods.size() > 0 ) 
			 {
				 //群发类邮件不能赠送物品或金钱！
				  GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_63"));
				  return false;
			 }
		 }
		 if (size > 100)
		 {
			 //收件人数量超过限制，发送失败		 
			 GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_62"));
			 return false;
		 }
	 }
	 return true;
 }


const int CMail::GetPostage(const eMAILTYPE eMailType)
 {
	 unsigned int dwPostage = 0;

	 CGlobeSetup::tagSetup *pSetup = CGlobeSetup::GetSetup();
	 switch(eMailType)
	 {
	 case COMMON_MAIL:	//普通邮件
		 dwPostage = pSetup->dwNormalMailPrice;
		 break;
	 case AFFIX_MAIL:	//普通附件邮件
		 dwPostage = pSetup->dwNormalGoodsMailPrice;
		 break;
	 case PAYGOLD_MAIL:	//付费邮件
		 dwPostage = pSetup->dwPayGoldMailPrice;
		 break;
	 }
	 // 根据玩家个数来计算邮资
	 dwPostage *= (unsigned int)m_vecRecver.size();

	 return dwPostage;
 }
// 获得金币附件
void CMail::DoGetGold()
{

	//CPlayer *pPlayer = GetGame()->GetMainPlayer();//del
	//char str2[256]="";//del
	//sprintf_s(str2,"领取金币前的金钱为%d",pPlayer->GetMoney());//del
	//GetInst(MsgEventManager).PushEvent(Msg_Ok,str2);//del

	stMailData MailData;
	GetCurReadMailData(MailData);
	if(MailData.dwGoldNum > 0)
	{
	// 非付款收信型
		if(MailData.eMailType != PAYGOLD_MAIL)
		{
			CMessage msg(MSG_C2S_MAIL_GET_GOLDS);
			msg.Add(m_guidCurMailBox);
			msg.Add(m_guidCurReadMail);
			msg.Send();
		}
	}
}

void CMail::DoAgreePay(const unsigned int index)
{
	if ( m_vecMailData.size() < index )
	{
		return;
	}
	CPlayer *pPlayer = GetGame()->GetMainPlayer();//del
	if ( m_vecMailData[index].eMailType == PAYGOLD_MAIL )
	{
		if(pPlayer->GetMoney() >=  m_vecMailData[index].dwGoldNum)
		{

			//char str2[256]="";//del
			//sprintf_s(str2,"付费前的金钱为%d",pPlayer->GetMoney());//del
			//GetInst(MsgEventManager).PushEvent(Msg_Ok,str2);//del

	
			char str[256]="";
			sprintf_s(str,AppFrame::GetText("Mail_18"),m_vecMailData[index].dwGoldNum);
			//"是否同意以%d金币价格收取信件中的附件？"
			GetInst(MsgEventManager).PushEvent(Msg_Agree,str,DoAgreePaySel);//请使用“&CMail::DoAgreePaySel”创建指向成员的指针
		}
		else
		{
			//"您的金币不足，不能付款！"
			GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_19"));
		}
	}
}

void  DoAgreePaySel(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		CMessage msg(MSG_C2S_MAIL_AGREED_PAY);
		msg.Add(GetInst(CMail).GetMailBoxGUID());
		msg.Add(GetInst(CMail).GetCurReadMailGUID());
		msg.Send();
	}	
}
// 清空所有邮件数据
void CMail::ClearAllMailData()
{
	m_vecDelMail.clear();
	for (unsigned int i = 0; i < m_vecMailData.size(); i++)
	{
		ReleaseMailGoods(m_vecMailData[i]);
	}
	m_vecMailData.clear();
}

// 删除指定索引的附加物品
void CMail::DoRemoveGoods(const unsigned int index)
{
	list<CPlayer::tagGoods>::iterator it = m_listGoods.begin();
	for (int i = 0; it != m_listGoods.end(); ++it,++i)
	{
		if(i == index)
		{
			(*it).pGoods->UnLock();
			//SAFEDELETE(it->pGoods);
			m_listGoods.erase(it);
			return;
		}
	}
}
// 退信操作
void CMail::DoRejectMail(const unsigned int index)
{
	if (m_vecMailData.size() < index+1)
	{
		return;
	}
	if (m_vecMailData[index].eMailType == SYSTEM_MAIL)
	{
		//系统邮件不能退信！
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_38"));
		return;
	}
	CMessage msg(MSG_C2S_MAIL_REJECT);
	msg.Add(m_guidCurMailBox);
	msg.Add(m_vecMailData[index].gMailID);
	msg.Send();
}



}