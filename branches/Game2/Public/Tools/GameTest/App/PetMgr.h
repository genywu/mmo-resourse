


#ifndef _PetMgr_H_
#define _PetMgr_H_
#pragma once
#include "../Utility/gtDef.h"

class CPet;

class CPetMgr
{
public:
	CPetMgr(void);

	~CPetMgr(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData);

	//	添加一个宠物
	void AddPet(CPet *pPet,bool bExtra);
	//	激活一个宠物
	void ActivePet(CPet *pPet);
	void PetEnterRegion();
	CPet* GetActivePet(long lType);
	CPet* GetActivePet(const CGUID& guid);
	long	GetActivePetCount();

	list<tagPetInfo>*	GetPetList()	{return &m_listAllPet;}
	map<CGUID,CPet*>*	GetActivePetList()	{ return &m_mapActivePet; }

	long GetPetCount();
	void ProcessPetDied(CPet	*pDiedPet);		//宠物宠物死亡
	void RemovePet(const CGUID & guid);
	void DeletePet(const CGUID & guid,bool bExtra = false );
	void RemoveAllPet();

private:
	list<tagPetInfo>	m_listAllPet;		//所有宠物列表
	map<CGUID,CPet*>	m_mapActivePet;		//	激活的宠物列表

	class CPlayer*	m_pOwner;
};
#endif//_PetMgr_H_