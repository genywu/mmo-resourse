#pragma once

#include "specialequip.h"



class CWeapon:public CSpecialEquip
{
public:
	enum WEAPON_STATUS 
	{
		WEAPON_ACTION0 = MAKEFOURCC('A','C','T','0'),
		WEAPON_ACTION1 = MAKEFOURCC('A','C','T','1'),
		WEAPON_ACTION2 = MAKEFOURCC('A','C','T','2'),
		WEAPON_ACTION3 = MAKEFOURCC('A','C','T','3'),
		WEAPON_ACTION4 = MAKEFOURCC('A','C','T','4'),
		WEAPON_ACTION5 = MAKEFOURCC('A','C','T','5'),
		WEAPON_ACTION6 = MAKEFOURCC('A','C','T','6'),
		WEAPON_ACTION7 = MAKEFOURCC('A','C','T','7'),
	};
	enum WEAPONSHOW_STATUS 
	{
		WEAPONSHOW_SINGLEWEAPON = 1,
		WEAPONSHOW_BIGSWORD,
		WEAPONSHOW_BIGSTAFF,
		WEAPONSHOW_DOUBLEWEAPON,
		WEAPONSHOW_BOXING,
		WEAPONSHOW_SHIELD,
		WEAPONSHOW_BOW,
	};
	CWeapon();
	~CWeapon();
	void SetWeaponType(long dwtype){m_dwWeaponType = dwtype;}
	long GetWeaponType(){return m_dwWeaponType;}
	void ChangeStatus(DWORD eStaus);
	virtual void Copy(CWeapon* psequip);

protected:
	long m_dwWeaponType;

};