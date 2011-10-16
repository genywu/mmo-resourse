  
#include "StdAfx.h"
#include "MedalContainer.h"
#include "Goods\CGoodsFactory.h"
#include "goods/CGoods.h"



MedalContainer::MedalContainer(CPlayer *pOwner)
:m_pOwner(pOwner)
{
}
MedalContainer::~MedalContainer(void)
{
	m_pOwner = NULL;
	m_setHold.clear();
	m_setUsing.clear();
}



//! 按索引(勋章ID)添加勋章
eMCOptReValue MedalContainer::AddMedal(LONG lGoodsBaseIndex)
{
	if(NULL == FindMedia(lGoodsBaseIndex))
		return eMCRV_MedalNotBeing;
	if(HasMedal(lGoodsBaseIndex))
		return eMCRV_RepeatAdd;
	m_setHold.insert(lGoodsBaseIndex);
	return eMCRV_Succeed;
}

//! 按索引(勋章ID)删除勋章
eMCOptReValue MedalContainer::DelMedal(LONG lGoodsBaseIndex)
{
	if(!HasMedal(lGoodsBaseIndex))
		return eMCRV_AimCantFind;
	m_setHold.erase(lGoodsBaseIndex);
	return eMCRV_Succeed;
}

//! 判断是否有指定索引(勋章ID)的的勋章
BOOL MedalContainer::HasMedal(LONG lGoodsBaseIndex)
{
	return (m_setHold.end() != m_setHold.find(lGoodsBaseIndex));
}

//! 得到可使用的勋章数量
LONG MedalContainer::GetMedalNum(void)
{
	return m_setHold.size();
}

//! 使用一个勋章（同时只能使用最多MEDAL_MAX_USE_NUM个勋章）
CGoods* MedalContainer::UseMedal(LONG lGoodsBaseIndex)
{
	if((!HasMedal(lGoodsBaseIndex)) || MEDAL_MAX_USE_NUM <= m_setUsing.size() || m_setUsing.end() != m_setUsing.find(lGoodsBaseIndex))
		return NULL;
	m_setUsing.insert(lGoodsBaseIndex);
	assert(NULL != FindMedia(lGoodsBaseIndex));
	return FindMedia(lGoodsBaseIndex);
}

//! 归还一个使用的勋章
eMCOptReValue MedalContainer::ReturnMedal(LONG lGoodsBaseIndex)
{
	if(m_setUsing.end() == m_setUsing.find(lGoodsBaseIndex))
		return eMCRV_AimCantFind;
	assert(HasMedal(lGoodsBaseIndex));
	m_setUsing.erase(lGoodsBaseIndex);
	return eMCRV_Succeed;
}

//! 用一个正在使用的勋章交换出一个未被使用的勋章
CGoods* MedalContainer::SwapMedal(LONG lUsingIndex, LONG lNewIndex)
{
	if((!HasMedal(lNewIndex)) || m_setUsing.end() == m_setUsing.find(lUsingIndex) || m_setUsing.end() != m_setUsing.find(lNewIndex))
		return NULL;
	assert(HasMedal(lUsingIndex));
	m_setUsing.erase(lUsingIndex);
	m_setUsing.insert(lNewIndex);

	assert(NULL != FindMedia(lNewIndex));
	return FindMedia(lNewIndex);
}


//! 编码
void MedalContainer::AddToByteArray(vector<BYTE> &vData)
{
	_AddToByteArray(&vData, (LONG)m_setHold.size());
	for (set<LONG>::iterator ite = m_setHold.begin(); ite != m_setHold.end(); ++ite)
	{
		_AddToByteArray(&vData, *ite);
	}
}

//! 解码
void MedalContainer::Decode(BYTE *pData, LONG &lPos)
{
	m_setHold.clear();
	LONG lNum = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lNum; ++i)
	{
		m_setHold.insert(_GetLongFromByteArray(pData, lPos));
	}
}

//!						编码
void MedalContainer::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray( (LONG)m_setHold.size());
	for (set<LONG>::iterator ite = m_setHold.begin(); ite != m_setHold.end(); ++ite)
	{
		setWriteDB.AddToByteArray(*ite);
	}
}
	//!						编码
void MedalContainer::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_setHold.clear();
	LONG lNum = setReadDB.GetLongFromByteArray( );
	for (LONG i = 0; i < lNum; ++i)
	{
		m_setHold.insert(setReadDB.GetLongFromByteArray( ));
	}
}



#pragma region 静态全局成员


//! 初始化勋章系统
BOOL MedalContainer::InitializeMedalSystem(void)
{
	CGoodsFactory::CreateGoodsListByType(GT_MEDAL, s_mapMedalByIndex);
	return TRUE;
}

//! 释放资源
void MedalContainer::Release(void)
{
	map<LONG, CGoods*>::iterator ite = s_mapMedalByIndex.begin();
	for (; ite != s_mapMedalByIndex.end(); ++ite)
	{
#ifdef _GOODSLOG1_
		if(ite->second)
		{
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			ite->second->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:MedalContainer::Release][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				ite->second->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(ite->second->GetBasePropertiesIndex()),srcGoodsGUID,ite->second->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
		}		
#endif
		CGoodsFactory::GarbageCollect(&(ite->second),10000,TRUE);
	}
	s_mapMedalByIndex.clear();
}

//! 找到一个勋章
CGoods* MedalContainer::FindMedia(LONG lGoodsBaseIndex)
{
	map<LONG, CGoods*>::iterator ite = s_mapMedalByIndex.find(lGoodsBaseIndex);
	if(s_mapMedalByIndex.end() != ite)
		return ite->second;
	return NULL;
}

//! 以索引为key的勋章对象列表
map<LONG, CGoods*> MedalContainer::s_mapMedalByIndex;

#pragma endregion