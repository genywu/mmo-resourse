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
		WEAPONSHOW_SINGLESWORD = 1,
		WEAPONSHOW_SINGLEHAMMER,
		WEAPONSHOW_SINGLEKNIFE,
		WEAPONSHOW_BIGSWORD,
		WEAPONSHOW_BIGWANG,
		WEAPONSHOW_BIGAX,
		WEAPONSHOW_DOUBLEWEAPON,
		WEAPONSHOW_BOXINGLEFT,
		WEAPONSHOW_BOXINGRIGHT,
		WEAPONSHOW_SHIELD,
		WEAPONSHOW_BOW,
	};
	CWeapon();
	~CWeapon();
	void SetWeaponType(long dwtype){m_dwWeaponType = dwtype;}
	long GetWeaponType(){return m_dwWeaponType;}
	void ChangeStatus(DWORD eStaus);
	CDisplayModel* GetSimpleModel(){return m_pSimpleModel;}
	virtual void Copy(CWeapon* psequip);
	virtual void SetDisplayModel();
	virtual void Display(CShape * pParentObj);
	virtual void DisplayForFlashColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwFlashColor);		//显示装备带闪光颜色的特殊效果(强制接受灯光处理)
	virtual void DisplayForExStateColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwExColor);			//显示装备带异常状态颜色的特殊效果(强制接受灯光处理)
	virtual void Destroy();
protected:
	long m_dwWeaponType;
	CDisplayModel* m_pSimpleModel;

};