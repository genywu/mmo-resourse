


#ifndef _DepotMgr_H_
#define _DepotMgr_H_
#pragma once
#include "../Utility/gtDef.h"

class CDepotMgr
{
public:
	CDepotMgr(void);
	~CDepotMgr(void);

	void DecordDepotHeadInfo(DBReadSet & db_read);
	void CDepotMgr::DecordDepotInfo(DBReadSet& db_read);
	void DecordDepotChildGoodsInfo(DBReadSet& db_read,long lDepotPackID,long lPackID);

	class CGoods* GetGoodsByGuid(const CGUID& guID,int *pPos,long* lExtendID);

	DWORD	GetEmptyPos(DWORD dwContainerID);

	//金币
	const CGUID& GetDepotMoneyGuid()	{return m_tagDepotData->guidMoney; }
	void SetDepotMoneyGuid(const CGUID& guid)	{ m_tagDepotData->guidMoney = guid; }

	long GetDepotMoneyAmount()	{return m_tagDepotData->lMoneyAmount;}
	void SetDepotMoneyAmount(long l)	{ m_tagDepotData->lMoneyAmount = l;}
	//银币
	const CGUID& GetDepotSilverMoneyGuid()	{ return m_tagDepotData->guidSilver; }
	void SetDepotSilverMoneyGuid(const CGUID& guid)	{m_tagDepotData->guidSilver = guid; }

	void SetDepotSilverMoneyAmount(long l)	{m_tagDepotData->lSilverAmount = l;}
	long GetDepotSilverMoneyAmount()	{ return m_tagDepotData->lSilverAmount; }
	//密码
	bool IsDepotHasPwd()		{ return m_tagDepotData->bHasPassWord;}
	void SetDepotHasPwd( bool b) { m_tagDepotData->bHasPassWord = b;}
	//旧密码
	char* GetDepotOldPwd()	{ return m_tagDepotData->strOldPwd;}
	//新密码
	char* GetDpeotNewPwd()	{ return m_tagDepotData->strNewPwd;}
	//是否打开
	BOOL IsDepotOpen()		{ return m_tagDepotData->bOpen; }
	void SetDepotOpen(BOOL b)	{ m_tagDepotData->bOpen = b; }
	//
	bool AddDepotGoods(long lContainerID, long pos, CGoods * &pGoods);

	CGoods* DelDepotGoods(long lContainerID, long pos,long lNum,const CGUID& SourGuid,const CGUID& AimGuid);
	tagDepotPackData * GetDepotInfo(eDEPOT_PACK_ID id)	{ return &m_tagDepotData->m_ArrPackData[id];}
	hash_map<CGUID,tagGoods,hash_guid_compare>* GetDepotGoodsList(eDEPOT_PACK_ID id)	{ return &m_tagDepotData->m_ArrPackData[id].mapGoodsList; }

	tagGoods* GetDepotGoodsByPos(long lExtendID,WORD dwPos);


private:
	// ! 仓库信息结构
	tagPlayerDepotDataHead			m_tagDepotInfo;
	//	!@ 仓库数据
	tagDepotData*						m_tagDepotData;	
};
#endif//_DepotMgr_H_