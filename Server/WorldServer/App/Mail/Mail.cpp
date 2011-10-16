
#include "StdAfx.h"
#include "Mail.h"
#include "../Goods/Goods.h"
#include "../Goods/GoodsFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern long g_lMailCount;

CMail::CMail(void)
{
	m_pBaseParam = new tagMailParam;
	m_pBaseParam->strExText.clear();
	m_pBaseParam->WriterGuid = NULL_GUID;
	m_pBaseParam->strReceiverList.clear();
	m_pBaseParam->strText.clear();
	m_pBaseParam->strSubject.clear();
	m_pBaseParam->bRead = 0;
	m_pBaseParam->lType = 0;
	m_pBaseParam->tRemainTime = 0;
	m_pBaseParam->lGold = 0;
	m_pBaseParam->tWriteTime = 0;
	m_pBaseParam->bSave = 0;
	m_pBaseParam->ReceiverGuid = NULL_GUID;
	m_pBaseParam->strWriter.clear();
	m_pBaseParam->Goodslist.clear();
	m_pBaseParam->ltgTimer.clear();
	m_pBaseParam->ltgMailEvent.clear();
	m_pBaseParam->vMailGoods.clear();
	m_pBaseParam->lMailCondition.clear();
	++g_lMailCount;
}
CMail::CMail(tagMailParam *pMailParam)
{	
	m_pBaseParam = pMailParam;
	++g_lMailCount;
}

CMail::~CMail(void)
{
	if (m_pBaseParam)
	{
		for (list<listGoods*>::iterator it=m_pBaseParam->Goodslist.begin(); it!=m_pBaseParam->Goodslist.end(); ++it)
		{
			SAFE_DELETE(*it);
		}
		for (list<tagTimer*>::iterator it=m_pBaseParam->ltgTimer.begin(); it!=m_pBaseParam->ltgTimer.end(); ++it)
		{
			SAFE_DELETE(*it);
		}
		for (list<tagMailEvent*>::iterator it=m_pBaseParam->ltgMailEvent.begin(); it!=m_pBaseParam->ltgMailEvent.end(); ++it)
		{
			SAFE_DELETE(*it);
		}

		for (vector<CGoods*>::iterator it=m_pBaseParam->vMailGoods.begin(); it!=m_pBaseParam->vMailGoods.end(); ++it)
		{
#ifdef _GOODSLOG_
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			(*it)->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:邮件析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				(*it)->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName((*it)->GetBasePropertiesIndex()),srcGoodsGUID,(*it)->GetAmount());
			CMessage::AsyWriteFile(GetGame().GetGoodsLogFile(),pszGoodsLog);							
#endif	
			CGoodsFactory::GarbageCollect( &(*it));			
		}
		m_pBaseParam->vMailGoods.clear();
		for (list<tagMailCondition*>::iterator it=m_pBaseParam->lMailCondition.begin(); it!=m_pBaseParam->lMailCondition.end(); ++it)
		{
			SAFE_DELETE(*it);
		}	

		SAFE_DELETE(m_pBaseParam);
	}
	--g_lMailCount;
}


void	CMail::AddMailMsgByteArrayToGS(vector<BYTE>& vRet)
{
	_AddToByteArray(&vRet,m_pBaseParam->guid);
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strSubject.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strWriter.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strText.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strExText.c_str());
	_AddToByteArray(&vRet,m_pBaseParam->lGold);
	_AddToByteArray(&vRet,(DWORD)m_pBaseParam->tRemainTime);		
	_AddToByteArray(&vRet,m_pBaseParam->bRead);
	_AddToByteArray(&vRet,m_pBaseParam->lType);
	_AddToByteArray(&vRet,m_pBaseParam->bReject);
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strReceiver.c_str());
	_AddToByteArray(&vRet,m_pBaseParam->ReceiverGuid);

	_AddToByteArray(&vRet,(long)m_pBaseParam->Goodslist.size());
	for (list<listGoods*>::iterator it=m_pBaseParam->Goodslist.begin();it!=m_pBaseParam->Goodslist.end();++it)
	{
		_AddToByteArray(&vRet,(*it)->goodsguid);
		_AddToByteArray(&vRet,(*it)->lNum);
		_AddToByteArray(&vRet,(*it)->lContainerID);
		_AddToByteArray(&vRet,(*it)->lPos);
	}
}



void CMail::AddToByteArrayToGS(vector<BYTE>& vRet)
{
	_AddToByteArray(&vRet,m_pBaseParam->guid);
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strSubject.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strWriter.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strText.c_str());
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strExText.c_str());
	_AddToByteArray(&vRet,m_pBaseParam->lGold);
	_AddToByteArray(&vRet,(DWORD)(m_pBaseParam->tRemainTime));		
	_AddToByteArray(&vRet,m_pBaseParam->bRead);
	_AddToByteArray(&vRet,m_pBaseParam->lType);
	_AddToByteArray(&vRet,m_pBaseParam->bReject);
	_AddToByteArray(&vRet,(char*)m_pBaseParam->strReceiver.c_str());
	_AddToByteArray(&vRet,m_pBaseParam->ReceiverGuid);

	_AddToByteArray(&vRet,(long)m_pBaseParam->Goodslist.size());
	for (list<listGoods*>::iterator it=m_pBaseParam->Goodslist.begin();it!=m_pBaseParam->Goodslist.end();++it)
	{
		_AddToByteArray(&vRet,(*it)->goodsguid);
		_AddToByteArray(&vRet,(*it)->lNum);
		_AddToByteArray(&vRet,(*it)->lContainerID);
		_AddToByteArray(&vRet,(*it)->lPos);
	}

	_AddToByteArray(&vRet,(long)m_pBaseParam->vMailGoods.size());
	for (vector<CGoods*>::iterator it=m_pBaseParam->vMailGoods.begin(); it!=m_pBaseParam->vMailGoods.end(); ++it)
	{
		(*it)->Serialize(&vRet);
	}

}
//清除物品列表
void	CMail::ClearMailGoodsContainer()
{
	for (vector<CGoods*>::iterator it=m_pBaseParam->vMailGoods.begin(); it!=m_pBaseParam->vMailGoods.end(); ++it)
	{
		CGoodsFactory::GarbageCollect(&(*it));
	}
	m_pBaseParam->vMailGoods.clear();
}


//根据索引清除物品列表
bool	CMail::ClearMailGoodsByIndex(unsigned long lIndex)
{
	if (lIndex<m_pBaseParam->vMailGoods.size())
	{
		CGoods *pGoods = m_pBaseParam->vMailGoods[lIndex];
		m_pBaseParam->vMailGoods.erase(m_pBaseParam->vMailGoods.begin()+lIndex);
		m_pBaseParam->bReject = 0;
#ifdef _GOODSLOG_
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		(*it)->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:邮件析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
			(*it)->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName((*it)->GetBasePropertiesIndex()),srcGoodsGUID,(*it)->GetAmount());
		CMessage::AsyWriteFile(GetGame().GetGoodsLogFile(),pszGoodsLog);							
#endif	
		CGoodsFactory::GarbageCollect( &(pGoods));		

		if (m_pBaseParam->vMailGoods.size()==0&&m_pBaseParam->lGold==0&&m_pBaseParam->lType!=SYSTEM_MAIL&&m_pBaseParam->lType<MAX_MAIL)
		{
			m_pBaseParam->lType = COMMON_MAIL;
		}
		return true;
	}
	return false;
}
