///////////////////////////////////////////////////////////////////////////////
///  Mail.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\GameServer\AppServer\Mail
///
///  CREATED: 09/12/2008  by 陈先进
///
///  PURPOSE:   邮件类
///
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Mail.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../container/CTestContainer.h"
#include "../Player.h"
#include "..\..\GameServer\GameManager.h"
#include "../GoodsExManage.h"
#include "..\message packaging\CS2CContainerObjectAmountChange.h"
#include "..\message packaging\CS2CContainerObjectMove.h"

CMail::CMail(tagMailParam *pMailParam)
{
	m_pBaseParam = pMailParam;
}
CMail::~CMail(void)
{

	for (list<tagSGoods*>::iterator it=m_pBaseParam->lSGoods.begin(); it!=m_pBaseParam->lSGoods.end(); ++it)
	{
		OBJ_RELEASE(tagSGoods, *it);
	}
	m_pBaseParam->lSGoods.clear();

	for (list<tagCGoods*>::iterator it=m_pBaseParam->lCGoods.begin(); it!=m_pBaseParam->lCGoods.end(); ++it)
	{
		OBJ_RELEASE(tagCGoods, *it);
	}
	m_pBaseParam->lCGoods.clear();

	

	for (list<tagMailCondition*>::iterator it=m_pBaseParam->lMailCondition.begin(); it!=m_pBaseParam->lMailCondition.end(); ++it)
	{
		OBJ_RELEASE(tagMailCondition, *it);
	}
	m_pBaseParam->lMailCondition.clear();

	for (list<tagMailEvent*>::iterator it=m_pBaseParam->ltgMailEvent.begin(); it!=m_pBaseParam->ltgMailEvent.end(); ++it)
	{
		OBJ_RELEASE(tagMailEvent, *it);
	}
	m_pBaseParam->ltgMailEvent.clear();

	OBJ_RELEASE(tagMailParam, m_pBaseParam);
}

//删除邮件中的物品
void	CMail::DeleteMailGoods()																
{
	for (vector<CGoods*>::iterator it=m_pBaseParam->vMailGoods.begin(); it!=m_pBaseParam->vMailGoods.end(); ++it)
	{		
#ifdef _GOODSLOG1_
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		(*it)->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:邮件析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
			(*it)->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName((*it)->GetBasePropertiesIndex()),srcGoodsGUID,(*it)->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
		CGoodsFactory::GarbageCollect( &(*it) ,52);
	}
	m_pBaseParam->vMailGoods.clear();
}
//物品列表
vector<CGoods*>&	CMail::GetMGoodsContainer()
{
	return m_pBaseParam->vMailGoods;
}

//信件解码
void	CMail::DecordFromByteArray(BYTE* pByte, long& pos)
{

}
void	CMail::SetSGoods(list<tagSGoods*> &lSGoods)
{
	list<tagSGoods*>::iterator it=lSGoods.begin();
	for (; it!=lSGoods.end(); ++it)
	{
		tagSGoods *ptgSGoods = OBJ_CREATE(tagSGoods);
		ptgSGoods->lIndex = (*it)->lIndex;
		ptgSGoods->lNum = (*it)->lNum;
		m_pBaseParam->lSGoods.push_back(ptgSGoods);
	}	
}
//从邮件物品容器中取物品到背包
void	CMail::GetGoodsFromContainer(CPlayer *pPlayer, long lIndex)
{
	CGoods *pGoods = NULL;
	if (lIndex<m_pBaseParam->vMailGoods.size())
	{
		pGoods = m_pBaseParam->vMailGoods.at(lIndex);
	}
	else
	{
		return;
	}
	
	if (pGoods && pPlayer)
	{
		bool bGetGoods = false;
		CTestContainer testContainer;
		testContainer.Init(pPlayer);
		vector<tagTestResult> vecResult;	
		BOOL bSucced=testContainer.AddTest(pGoods,&vecResult);	
		if (bSucced)
		{				
			long lGoodsIndex = pGoods->GetBasePropertiesIndex();
			CGUID GoodsGuid = pGoods->GetExID();
			string strName = pGoods->GetName();
			long   lGoodsAmount = pGoods->GetAmount();

			CVolumeLimitGoodsContainer* pVolumeContainer=dynamic_cast<CVolumeLimitGoodsContainer*>(pPlayer->FindContainer(vecResult[0].dwContainerId));
			BOOL bAddSucceed=FALSE;
			if(pVolumeContainer)
			{
				if(vecResult[0].eOperType == CS2CContainerObjectMove::OT_NEW_OBJECT)
				{
					//添加新物品							
					CS2CContainerObjectMove comMsg;
					if(pVolumeContainer->Add(vecResult[0].dwPos, pGoods, NULL, &comMsg) )
					{
						bAddSucceed=TRUE;
						comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
						comMsg.SetDestinationContainerExtendID( vecResult[0].dwContainerId );
						pGoods->SerializeForOldClient( &comMsg.GetObjectStream() );
						comMsg.Send( pPlayer->GetExID() );
					}
				}
				else if(vecResult[0].eOperType == CS2CContainerObjectMove::OT_CHANGE_AMOUNT)						
				{
					//改变物品数量
					if(pVolumeContainer->Add(vecResult[0].dwPos, pGoods, NULL) )
					{
						bAddSucceed=TRUE;
						CGoods* pExistedGoods=pVolumeContainer->GetGoods(vecResult[0].dwPos);
						if(pExistedGoods)
						{
							CS2CContainerObjectAmountChange coacMsg;
							coacMsg.SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), 
								vecResult[0].dwPos );
							coacMsg.SetSourceContainerExtendID( vecResult[0].dwContainerId );
							coacMsg.SetObject( pExistedGoods->GetType(), pExistedGoods->GetExID() );
							coacMsg.SetObjectAmount( pExistedGoods->GetAmount() );
							coacMsg.Send( pPlayer->GetExID() );
						}								
					}
				}

				if(bAddSucceed)
				{
					GetGameLogInterface()->logT602_GetGoods_mail_log(GetExID(),m_pBaseParam->strReceiver.c_str(),m_pBaseParam->strWriter.c_str(),lGoodsIndex,GoodsGuid,strName.c_str(),m_pBaseParam->lType,m_pBaseParam->lGold,lGoodsAmount,m_pBaseParam->lSGoods.size());
					m_pBaseParam->vMailGoods.erase(m_pBaseParam->vMailGoods.begin()+lIndex);
					m_pBaseParam->bReject = 0; 
					if (m_pBaseParam->lGold==0 && m_pBaseParam->vMailGoods.size()==0 && m_pBaseParam->lType!=SYSTEM_MAIL && m_pBaseParam->lType<MAX_MAIL)
					{
						m_pBaseParam->lType = COMMON_MAIL;
					}
					SendGetGoodsToClient(GetExID(),pPlayer->GetExID(),lIndex,1);		
				}
			}		
		}
	}
}
//向客户端发送取物品消息
void	CMail::SendGetGoodsToClient(CGUID &MailGuid,const CGUID &PlayerGuid,long lIndex,BYTE bResult)
{
	CMessage msg(MSG_S2C_MAIL_MOVE_GOODS_RESULT);
	msg.Add(MailGuid);
	msg.Add(lIndex);
	msg.Add(bResult);
	msg.SendToPlayer(PlayerGuid);
}
//发送从邮件物品容器中取物品到背包消息
void	CMail::SendGetGoodsMsg(CPlayer *pPlayer, long lIndex)
{
	CGoods *pGoods = NULL;
	if (lIndex>=m_pBaseParam->vMailGoods.size())
		return;	

	pGoods = m_pBaseParam->vMailGoods.at(lIndex);
	
	if (pGoods)
	{
		//唯一检测	
		if(pGoods->HasAddonProperty(GAP_UNIQUE))
		{
			DWORD dwGoodsId=pGoods->GetBasePropertiesIndex();
			if(!pPlayer->IsGoodsUnique(CGoodsFactory::QueryGoodsOriginalName(dwGoodsId)))
			{
				char* strMsg=CStringReading::LoadString(20,49);
				if(strMsg)
				{
					CMessage msg(MSG_S2C_OTHER_GAMEMESSAGE);
					msg.Add(strMsg);
					msg.Add((long)0);
					msg.Add(CGUID::GUID_INVALID);
					msg.SendToPlayer(pPlayer->GetExID());
				}
				return;
			}
		}

		bool bGetGoods = false;
		CTestContainer testContainer;
		testContainer.Init(pPlayer);
		vector<tagTestResult> vecResult;	
		BOOL bSucced=testContainer.AddTest(pGoods,&vecResult);	

		if (!bSucced)
		{
			SendGetGoodsToClient(GetExID(),pPlayer->GetExID(),lIndex,0);
			return;
		}

		CMessage msg(MSG_S2W_MAIL_OPERA_GOODS);
		msg.Add(GetExID());
		msg.Add(lIndex);
		msg.Send();	
	}
}
//设置收信人列表
void CMail::SetReceiverList(const char* szRecer)		
{ 
	if(szRecer) 
		m_pBaseParam->strReceiverList.push_back(string(szRecer));
}	
//设置写信人
void CMail::SetWriter(const char* szWriter)				
{ 
	if(szWriter) 
		m_pBaseParam->strWriter = szWriter;		
}
//设置主题
void CMail::SetSubject(const char* szSubject)			
{ 
	if(szSubject) 
		m_pBaseParam->strSubject = szSubject;	
}
//设置邮件文本
void CMail::SetText(const char* szText)					
{
	if(szText) 
		m_pBaseParam->strText = szText;			
}


//系统类邮件在物品容器中添加物品
void	CMail::AddGoodsToContainerBySys()														
{
	if (GetType()==SYSTEM_MAIL || GetType()>=MAX_MAIL)
	{
		vector<CGoods*> vGoods;

		list<tagSGoods*>::iterator it=m_pBaseParam->lSGoods.begin();
		for (;it!=m_pBaseParam->lSGoods.end();++it)
		{
			CGoodsFactory::CreateGoods((*it)->lIndex,(*it)->lNum,vGoods,24);
			OBJ_RELEASE(tagSGoods, *it);
		}
		m_pBaseParam->lSGoods.clear();

		for (vector<CGoods*>::iterator it=vGoods.begin(); it!=vGoods.end();++it)
		{
			m_pBaseParam->vMailGoods.push_back((*it));
			if (m_pBaseParam->vMailGoods.size()>=5)			
				break;
		}

	}
}
//检查物品容器
bool	CMail::CheckGoodsContainer(CPlayer *pPlayer)
{
	if ((GetType()==AFFIX_MAIL && m_pBaseParam->lGold==0 && m_pBaseParam->lCGoods.size()==0) || m_pBaseParam->lCGoods.size()>5   )
	{
		return false;
	}
	list<tagCGoods*>::iterator it=m_pBaseParam->lCGoods.begin();
	for (; it!=m_pBaseParam->lCGoods.end(); ++it)
	{
		
		list<tagCGoods*>::iterator sit=m_pBaseParam->lCGoods.begin();
		for (; sit!=m_pBaseParam->lCGoods.end(); ++sit)
		{
			if ((*it)!=(*sit))
			{
				if ((*it)->goodsguid==(*sit)->goodsguid || ((*it)->lContainerID==(*sit)->lContainerID && (*it)->lPos==(*sit)->lPos))
				{
					return false;
				}
			}
		}
		CGoods *pGoods = pPlayer->FindGoods((*it)->lContainerID,(*it)->lPos);
		if (!pGoods)
		{
			return false;
		}
		if (pGoods)
		{
			if (GoodsExManage::GetInstance().IsFrost(pGoods) || GoodsExManage::GetInstance().IsBind(pGoods) 
															 || pGoods->GetExID()!=(*it)->goodsguid															 
															 || pGoods->GetAmount()<(*it)->lNum) 
			{
				return false;
			}		
			if (m_pBaseParam->lType!=SYSTEM_MAIL && m_pBaseParam->lType<MAX_MAIL 
												 && (pGoods -> GetAddonPropertyValues(GAP_PARTICULAR_ATTRIBUTE, 1)&CGoodsBaseProperties::PA_CANNOT_TRADE_AND_STORE )==CGoodsBaseProperties::PA_CANNOT_TRADE_AND_STORE)
			{
				//系统邮件可以发送不能交易的物品
				return false;
			}
		}
	}
	return true;
}

//删除背包中的物品
bool	CMail::DeleteGoodsInContainer(CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return false;
	for (list<tagCGoods*>::iterator it=m_pBaseParam->lCGoods.begin(); it!=m_pBaseParam->lCGoods.end(); ++it)
	{
		if(!pPlayer->DelGoods((*it)->goodsguid,(*it)->lNum))
			return false; 		
	}
	return true;
}


//添加物品到邮件物品容器
bool	CMail::AddGoodsToContainer(CPlayer *pPlayer)
{
	if (pPlayer==NULL)
		return false;
	for (list<tagCGoods*>::iterator it=m_pBaseParam->lCGoods.begin(); it!=m_pBaseParam->lCGoods.end(); ++it)
	{
		CGoods *pGoods = pPlayer->FindGoods((*it)->lContainerID,(*it)->lPos);

			if (pGoods && pGoods->GetAmount()<=(*it)->lNum )
			{				
				m_pBaseParam->vMailGoods.push_back(pGoods);				
			}		
			else
			{
				return false;
			}
	}
	return true;
}
//为客户端编码
void	CMail::AddToByteArrayForClient(vector<BYTE>& vRet,CPlayer *pPlayer)				
{

	time_t t_CurrentTime = time(NULL);
	DWORD dwRemainTime = 0;
	if (GetRemainTime() - t_CurrentTime>0)
	{
		dwRemainTime = GetRemainTime() - t_CurrentTime;
	} 	

	_AddToByteArray(&vRet,GetExID());		//邮件的ID
	_AddToByteArray(&vRet,dwRemainTime);	//邮件的有效时间
	_AddToByteArray(&vRet,(DWORD)GetGold());//操作的金钱数
	_AddToByteArray(&vRet,GetSubject().c_str());//邮件名称
	_AddToByteArray(&vRet,GetText().c_str());	//邮件内容
	_AddToByteArray(&vRet,GetExText().c_str());	//邮件附加内容
	_AddToByteArray(&vRet,GetWriter().c_str());	//邮件发送者名称
	_AddToByteArray(&vRet,(BYTE)GetRead());	    //是否阅读过
	_AddToByteArray(&vRet,(BYTE)GetType());     //类型
	_AddToByteArray(&vRet,(BYTE)GetReject());   //退信标志符

	_AddToByteArray(&vRet,(long)GetSGood().size());

	if (GetSGood().size()>0 && GetType()==REQUEST_MAIL)
	{
		for (list<tagSGoods*>::iterator it=GetSGood().begin(); it!=GetSGood().end(); ++it)
		{
			_AddToByteArray(&vRet,(long)(*it)->lIndex);
			_AddToByteArray(&vRet,(long)(*it)->lNum);		
		}
	}
	else
	{
		_AddToByteArray(&vRet,(long)GetMGoodsContainer().size());
		for (vector<CGoods*>::iterator it=GetMGoodsContainer().begin(); it!=GetMGoodsContainer().end(); ++it)
		{
			(*it)->SerializeForOldClient(&vRet);		
		} 	
	}


}
//获取系统群发类的发送时间段
void CMail::GetSysSendTime(long &lBeginTime,long &lEndTime)
{
	list<tagMailEvent*> lEventTime = GetMailEvent();
	list<tagMailEvent*>::iterator it=lEventTime.begin();
	for (;it!=lEventTime.end();++it)
	{
		if ((*it)->strEventName=="ET_Send")
		{
			lBeginTime = (*it)->lBeginTime;
			lEndTime   = (*it)->lEndTime;
		}
	}
}
void CMail::CodeToDataBlockForClient(DBWriteSet& setWriteDB,CPlayer *pPlayer)
{
	time_t t_CurrentTime = time(NULL);
	DWORD dwRemainTime = 0;
	if (GetRemainTime() - t_CurrentTime>0)
	{
		dwRemainTime = GetRemainTime() - t_CurrentTime;
	} 	
	setWriteDB.AddToByteArray(GetExID());									//邮件的ID

	setWriteDB.AddToByteArray(dwRemainTime);//邮件的有效时间

	setWriteDB.AddToByteArray((DWORD)GetGold());//操作的金钱数

	setWriteDB.AddToByteArray(GetSubject().c_str());//邮件名称
	setWriteDB.AddToByteArray(GetText().c_str());//邮件内容
	setWriteDB.AddToByteArray(GetExText().c_str());//邮件附加内容
	setWriteDB.AddToByteArray(GetWriter().c_str());//邮件发送者名称
	setWriteDB.AddToByteArray((BYTE)GetRead());		   //是否阅读过
	setWriteDB.AddToByteArray((BYTE)GetType());        //类型
	setWriteDB.AddToByteArray((BYTE)GetReject());      //退信标志符


	setWriteDB.AddToByteArray((long)GetSGood().size());

	if (GetSGood().size()>0 && GetType()==REQUEST_MAIL)
	{
		for (list<tagSGoods*>::iterator it=GetSGood().begin(); it!=GetSGood().end(); ++it)
		{
			setWriteDB.AddToByteArray((long)(*it)->lIndex);
			setWriteDB.AddToByteArray((long)(*it)->lNum);		
		}
	}
	else
	{
		setWriteDB.AddToByteArray((long)GetMGoodsContainer().size());
		for (vector<CGoods*>::iterator it=GetMGoodsContainer().begin(); it!=GetMGoodsContainer().end(); ++it)
		{
			(*it)->SerializeForOldClient(setWriteDB);		
		} 	
	}
	
}
//添加增值道具
void	CMail::AddSGoodsToContainer(vector<CGoods*> &vGoods)
{
	vector<CGoods*>::iterator it=vGoods.begin();
	for (;it!=vGoods.end();)
	{
		m_pBaseParam->vMailGoods.push_back((*it));
		it=vGoods.erase(it);
	}
	vGoods.clear();

	for (list<tagSGoods*>::iterator it=m_pBaseParam->lSGoods.begin(); it!=m_pBaseParam->lSGoods.end(); ++it)
	{
		OBJ_RELEASE(tagSGoods, *it);
	}
	m_pBaseParam->lSGoods.clear();
}





void	CMail::AddToByteArrayForWS(vector<BYTE>& vRet,CPlayer* pPlayer)
{	
	_AddToByteArray(&vRet,GetExID());					//邮件Guid
	_AddToByteArray(&vRet,(char*)GetWriter().c_str());	//邮件写信人
	_AddToByteArray(&vRet,(char*)GetSubject().c_str());	//主题
	_AddToByteArray(&vRet,(char*)GetText().c_str());	//邮件内容
	_AddToByteArray(&vRet,(char*)GetExText().c_str());	//邮件附加内容
	_AddToByteArray(&vRet,(BYTE)GetRead());				//读取标志符
	_AddToByteArray(&vRet,(BYTE)GetType());				//邮件类型
	_AddToByteArray(&vRet,(long)GetGold());				//金钱
	_AddToByteArray(&vRet,(DWORD) GetRemainTime());		//保存时间
	_AddToByteArray(&vRet,(BYTE)GetReject());			//退信标志符

	//收信人
	_AddToByteArray(&vRet,(char*)pPlayer->GetName());

	//增值物品列表
	long lSGoodNum =  GetSGood().size();
	_AddToByteArray(&vRet,lSGoodNum);							
	for (list<tagSGoods*>::iterator it=GetSGood().begin(); it!=GetSGood().end(); ++it)
	{
		_AddToByteArray(&vRet,(*it)->lIndex);
		_AddToByteArray(&vRet,(*it)->lNum);
	}
	//普通物品列表
	long lCGoodNum = GetMGoodsContainer().size();
	_AddToByteArray(&vRet,lCGoodNum);
	int nPos = 0;
	vector<BYTE> pBA;
	for (vector<CGoods*>::iterator it=GetMGoodsContainer().begin(); it!=GetMGoodsContainer().end(); ++it)
	{				
		(*it)->Serialize(&pBA);
		_AddToByteArray(&vRet,(LONG)pBA.size());
		if (pBA.size())		
			_AddToByteArray(&vRet,&pBA[0], pBA.size());			
	}
}


//判断系统邮件事件时间区域
bool	CMail::JudgeSysMailEventTime()
{
	for (list<tagMailEvent*>::iterator it=m_pBaseParam->ltgMailEvent.begin(); it!=m_pBaseParam->ltgMailEvent.end(); ++it)
	{
		time_t tCurrentTime = time(NULL);
		if (tCurrentTime<(*it)->lEndTime)
		{
			return true;
		}		
	}	
	return false;
}