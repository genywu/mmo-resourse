#pragma once

class CWeaponEffectXml
{
public:
	CWeaponEffectXml(void);
	virtual ~CWeaponEffectXml(void);
	static BOOL LoadWeaponEffect(const char* filename);
	static DWORD GetWeaponEffect(DWORD dwWeaponID);
	static void Release();
private:
	static map<DWORD, DWORD> m_mapWeaponEffectXml;
};