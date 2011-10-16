



#ifndef _Packet_Mgr_H_
#define _Packet_Mgr_H_
#pragma once
#include "../Utility/gtDef.h"


class CPacketMgr
{
public:
	CPacketMgr(class CGoodsMgr* pFather);

	~CPacketMgr(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead,bool bExData);

	void DecodeBusinessFromDataBlock(DBReadSet& dbRead,bool bExData);

	list<tagGoods>& GetMainGoodsList()			{ return m_listGoods; }

	CGoods* GetGoodsByGUID(const CGUID& guID,int *pPos,long* lExtendID);

	tagCote* GetCoteGoods(long i){						//得到快捷栏上的物品
		if( i>=0 && i <COTE_NUM)
			return m_Cote[i];
		return NULL;
	}

	int FindCote(){
		for(int i=0; i!=COTE_NUM; i++)
		{
			if( m_Cote[i]&&m_Cote[i]->pGoods)
				return i;
		}
		return -1;
	}
	int FindEmptyCote()
	{
		for(int i=0; i!=COTE_NUM; i++)
		{
			if( m_Cote[i] && m_Cote[i]->pGoods == NULL )
				return i;
		}
		return -1;
	}

	long GetCoteGoodsAmount()	{
		long l=0;
		for(int i=0; i!=COTE_NUM; i++)
		{
			if( GetCoteGoods(i)->pGoods )
				l++;
		}
		return l;
	}

	void ClearAllGoods(long lSocketID);
	list<tagGoods>* GetMainPacket()		{ return &m_listGoods;}
	list<tagGoods>* GetChildPackGoods(long lPackIndex)  {return &m_Cote[lPackIndex]->m_listCoteGoods;}

	CGoods* FindGoodsByOrgName(const char* name);
	//容器接口
	list<tagGoods>* GetContainerGoodsList( int iContainerID);
	DWORD	GetContainerEmptyPos(int iContainerID);
	//背包接口
	list<tagGoods>* GetGoodsList(WORD index=0);															//获取指定背包的物品链表（包括原始背包）
	list<tagGoods>* GetSubGoodslist(WORD index);														//获取指定序号的子背包的物品链表	

	tagGoods* GetGoods(WORD wPos);																		//获取原始背包的指定位置的物品
	tagGoods* GetSubGoods(WORD ExtendID,WORD lPos);														//获取指定序号的子背包的某个位置的物品

	CGoods* GetSubPGoods(WORD index);																	//获取指定序号的子背包自身作为物品的属性

	void SetSubPGoods(WORD index,CGoods* pGoods);														//设置指定序号的子背包的pGoods
	//	void ClearCoteGoods(WORD index);																	//清空指定序号的子背包

	WORD GetPackageXRowNum(WORD index) const		  {return (WORD)PACK_WIDE;}							//获取指定序号的子背包横排列数
	WORD GetPackageDimen(WORD index);																	//获取指定序号的子背包的容积
	WORD GetHostPackgeDimen() const 				  {return (WORD)HOST_GRIDITEM;}						//获取原始背包的容积

	bool IsAllPackageFull();																			//是否所有的背包都是满的
	WORD GetNotFullPackage();																			//获取指定序号的背包的第一个空格子的位置
	tagCote* GetCotePackage(WORD index);																//获取人物指定序号的子背包数据结构

	////////////////////////#新增加的物品管理的函数/////////////////////////////////////
	tagGoods* GetGoodsByOrgName(const char* strOrgName);
	//
	long	GetPackGoodsPos(WORD wContainerID, const CGoods* pGoods);			//得到容器物品的背包位置

	bool	AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID);
	CGoods	*RemoveGoods(const CGUID& SourceGUID, const CGUID& DesGUID,DWORD dwPos,LONG lExtendID,DWORD dwAmount);

	WORD	GetGoodsContainerID(CGoods* pGoods);						//获取人物身上的物品所在的容器ID
	WORD	FindContainerIDOfGoods(CGoods* pGoods);						//查找人物身上是否有某种类型的物品，如果有返回所在的容器ID
	//CGoods* GetGoodsByPosition(long lExtendID,WORD dwPos);				//通过位置查找物品
	CGoods* FindFirstGoodsByIndex(DWORD dwIndex,int& lExtendID,int& pPos);						//查找第一个物编ID等于传入的ID的物品

	//根据物品类型在人物身上来查找是否有这个物品，并且这个物品所在的位置，及它的帮定位置
	bool	FindAppointGoods(long lGoodType,long lGoodId, long lGoodNum, long lContainerID);									
	//根据物编号查找人物身上所有的此类物品的总量
	long	GetGoodsAmount(long nIdx);
	//根据物品名称查找人物身上所有的此类物品的总量
	long	GetGoodsAmount(const char* cGoodsName);
	// 根据传入的物品ID和要增加的数量查找一个堆叠数不会超过堆叠上限并且未被锁定的物品
	CGoods* FindOverLapGoodsByIndex(DWORD dwIndex,DWORD dwGoodsNum,int& lExtendID,int& pPos);

	///获取跑墒背包指定位置的物品
	tagCote*	GetBusinessCote(void)		{ return m_CoteBussinesPack; }
	tagGoods* GetBusinessGoods(WORD wPos);
	void SetBussinesPack(CGoods*pPackGoods);
	bool ClearBussinesPackGoods();

	void AddGoodsPos(DWORD dwPos)	{ m_setPacketEmptyPos.insert(dwPos); }
	void DelGoodsPos(DWORD dwPos)	
	{
		set<DWORD>::iterator itPos = m_setPacketEmptyPos.find( dwPos );
		if( itPos != m_setPacketEmptyPos.end() )
			m_setPacketEmptyPos.erase(itPos);
	}

private:
	CGoodsMgr*		m_pFather;
	 //	管理人物的跑墒背包中的物品
	tagCote*		m_CoteBussinesPack;
	//	管理人物原始背包物品的链表
	list<tagGoods>		m_listGoods;		
	//	背包快捷栏
	tagCote*			m_Cote[COTE_NUM];	

	set<DWORD>			m_setPacketEmptyPos;
};
#endif//_Packet_Mgr_H_