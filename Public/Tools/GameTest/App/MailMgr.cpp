


#include "stdafx.h"
#include "MailMgr.h"
#include "Goods.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMailMgr::CMailMgr()
{

}

CMailMgr::~CMailMgr()
{
	Release();
}

void CMailMgr::Release()
{
	ClearMailData();
}


//	删除邮件物品
void CMailMgr::ReleaseMailGoods(stMailData& stMail)
{
	if ( !(stMail.GoodsList).empty() )
	{	
		list<CGoods *>::iterator it = stMail.GoodsList.begin();
		for (; it != stMail.GoodsList.end(); ++it)
		{
			SAFE_DELETE((*it));
		}
		stMail.GoodsList.clear();
	}
}


void CMailMgr::DecodeFromByteArray(stMailData& stData,BYTE* pByte,long& pos)
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
}


void CMailMgr::AddMail(stMailData &tagData)
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

}


void CMailMgr::DelMailByGuid(const CGUID &gMailID)
{
	if (!m_vecMailData.empty())
	{
		vector<stMailData>::iterator it = m_vecMailData.begin();
		for (; it!=m_vecMailData.end(); ++it)
		{
			if((*it).gMailID == gMailID)
			{
				ReleaseMailGoods((*it));
				m_vecMailData.erase(it);
				return;
			}
		}
	}
}


void CMailMgr::FormatMailInfo(stMailData &tagData)
{
	if(tagData.gMailID == CGUID::GUID_INVALID)
		return;
	// 默认白色
	tagData.colorSender = tagData.colorTitle = 0xffffffff;
	tagData.colorGoodsEx = 0x00ffffff;
	//string str = "";
	// 赠送类型（标题为绿色加赠送字样）
// 	if(tagData.eMailType == PRESENT_MAIL)
// 	{
// 		tagData.colorTitle = 0xff00ff00;
// 		//str = CStringReading::LoadString(eIDS_MAIL,eIDS_MAIL_PRESENT);
// 	}
	// 索取（所有信息均为蓝色）
// 	else if(tagData.eMailType == REQUEST_MAIL)
// 	{
// 		tagData.colorSender = tagData.colorTitle = 0xff00afff;
// 		tagData.colorGoodsEx = 0x6400afff;
// 		//str = CStringReading::LoadString(eIDS_MAIL,eIDS_MAIL_REQUEST);
// 	}
	// 付款（标题和物品遮罩为红色加付款字样）
// 	else if(tagData.eMailType == REQUEST_MAIL)
// 	{
// 		tagData.colorTitle = 0xffff0000;
// 		tagData.colorGoodsEx = 0x64ff0000;
// 		//str = CStringReading::LoadString(eIDS_MAIL,eIDS_MAIL_PAYMENT);
// 	}
	// 系统（发件人为黄色，如果有附件，肯定为系统赠送，标题为红色加赠送字样）
	if(tagData.eMailType == SYSTEM_MAIL)
	{
		tagData.colorSender = 0xfffff600;
		if(!tagData.GoodsList.empty())
		{
			tagData.colorTitle = 0xffff0000;
			//str = CStringReading::LoadString(eIDS_MAIL,eIDS_MAIL_PRESENT);
		}
	}
	// 邮件被阅读过，主题为灰色
	if(tagData.bIsReaded)
	{
		tagData.colorTitle = 0xff949494;
		// 非系统邮件附件标示为灰色朦板
		if(tagData.eMailType != SYSTEM_MAIL)
			tagData.colorGoodsEx = 0x64949494;
	}
	//tagData.strMailName = str + tagData.strMailName;
}


void CMailMgr::FormatMailTime(stMailData &tagData, string &str, long &color)
{
	str = "";
	if(tagData.dwTimeStart > 0 && tagData.dwTime > 0)
	{
		// 颜色默认白色
		color = 0xffffffff;
		// 显示的时间
		DWORD dwTime = 0;
		char strText[256]="";
		// 流逝时间
		DWORD dwTimeElapse = (timeGetTime() / 1000) - tagData.dwTimeStart;
		if(tagData.dwTime > dwTimeElapse)
		{
			// 剩余时间
			dwTimeElapse = tagData.dwTime - dwTimeElapse;
			// 天数(绿色)
			dwTime = dwTimeElapse / (3600*24);
			if (dwTime > 0)
			{
				sprintf_s(strText,"%d 天",dwTime);
				color = 0xff00ff00;
			}
			else
			{
				// 小时(红色)
				dwTime = dwTimeElapse / 3600;
				if(dwTime > 0)
				{
					sprintf_s(strText,"%d 小时",dwTime);
					color = 0xffff0000;
				}
				else
				{
					// 分钟(白色)
					dwTime = dwTimeElapse / 60;
					if(dwTime > 0)
					{
						sprintf_s(strText,"%d 分钟",dwTime);
						color = 0xffffffff;
					}
				}
			}
			str = strText;
		}
	}
}


stMailData* CMailMgr::GetMailByGUID(const CGUID &gMailID)
{
	if (!m_vecMailData.empty())
	{	
		vector<stMailData>::iterator it = m_vecMailData.begin();
		for (; it!=m_vecMailData.end(); ++it)
		{
			if((*it).gMailID == gMailID)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

void CMailMgr::DelGoodsEx(const int nIndex)
{
	if ( !m_listSendToGoods.empty() )
	{	
		list<stGoodsInfo>::iterator it = m_listSendToGoods.begin();
		for (int i = 0; it != m_listSendToGoods.end(); ++it,++i)
		{
			if(i == nIndex)
			{
				(*it).pGoods->UnLock();

				m_listSendToGoods.erase(it);
				// 附件物品被删除完了,群发按钮，付费收信按钮可用
				if(/*m_listSendToGoods.size() <= 0*/m_listSendToGoods.empty() )
				{
					m_eGoodsType = GOODS_UNKNOW_TYPE;
					if(m_eMailType != PAYGOLD_MAIL)
					{
						m_eMailType = COMMON_MAIL;
					}
				}
				return;
			}
		}
	}
}

void CMailMgr::AddSendToNorGoods(const tagGoods *taggoods)
{
	if (NULL == taggoods)
	{	
		return ;
	}
	stGoodsInfo goodsinf;
	goodsinf.dwPos	   = taggoods->lPos;
	goodsinf.dwType	   = taggoods->lType;
	//goodsinf.dwYuanBao = ; 
	goodsinf.pGoods    = taggoods->pGoods;
	m_listSendToGoods.push_back(goodsinf);
}

bool CMailMgr::SendPlayerName(CMessage *&msg)
{
	//先检名字并发送
	if ( !m_vecSendToPlayerName.empty() && msg != NULL )
	{	
		for (int i=0; i!=m_vecSendToPlayerName.size(); ++i)
		{
			msg->Add(m_vecSendToPlayerName[i].c_str());
		}

		return true;
	}

	return false;
}



bool CMailMgr::SendNorGoods(CMessage *&msg, BYTE mailtype)
{
	//检查是否有附件或者增值物品
	if ( !m_listSendToGoods.empty() && msg != NULL )
	{
		list<stGoodsInfo>::iterator it = m_listSendToGoods.begin();
		// 普通物品
		if(mailtype == COMMON_MAIL || mailtype == AFFIX_MAIL || mailtype == PAYGOLD_MAIL)
		{
			for (; it != m_listSendToGoods.end(); ++it)
			{
				msg->Add((*it).pGoods->GetExID());
				msg->Add((long)(*it).pGoods->GetNum());
				msg->Add((DWORD)(*it).dwType);
				msg->Add((DWORD)(*it).dwPos);
			}

			ClearAllSendData();
//			MailClearAllSendData();
			m_lSendMoney = 0;
			return true;
		}
		// 增值道具
		else
		{
			for (; it != m_listSendToGoods.end(); ++it)
			{
				msg->Add((long)(*it).pGoods->GetIndex());
				msg->Add((long)(*it).pGoods->GetNum());
			}
			ClearAllSendData();
			m_lSendMoney = 0;
			return true;
		}
	}
	ClearAllSendData();
	m_lSendMoney = 0;
	return false;
}

void CMailMgr::ClearAllSendData()
{
	//清空名字
	if (!m_vecSendToPlayerName.empty())
	{
		m_vecSendToPlayerName.clear();
	}
	//清空物品
	if (!m_listSendToGoods.empty())
	{
		//list<stGoodsInfo>::iterator it = m_listSendToGoods.begin();
		m_listSendToGoods.clear();
	}
}

//计算发送的钱
long CMailMgr::GetSendMoney()
{
	if (!m_listSendToGoods.empty())
	{
		return m_lSendMoney+=10;
	}

	return m_lSendMoney;
}

void CMailMgr::ClearMailData()
{
	if ( !m_vecMailData.empty() )
	{	
		vector<stMailData>::iterator itMail = m_vecMailData.begin();
		for(;itMail != m_vecMailData.end();itMail++)
		{
			list<CGoods*>::iterator it = itMail->GoodsList.begin();
			for ( ; it!= itMail->GoodsList.end(); it++)
			{
				SAFE_DELETE(*it);
			}
			itMail->GoodsList.clear();
		}
		m_vecMailData.clear();
	}
}