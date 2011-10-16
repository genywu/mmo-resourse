


#ifndef _OrganiseSys_H_
#define _OrganiseSys_H_
#pragma once

class COrganiseSys
{
public:
	COrganiseSys(void);
	~COrganiseSys(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead,bool bSelf);

private:
	//	家族
	CGUID m_FamilyGuid;
	//	联盟
	CGUID m_UnionGuid;

	// 行会图标CRC
	DWORD m_dwFacIconCRC;

	//	官阶等级
	DWORD m_lFamilyJobLevel;
	//	家族名
	string	m_strFamilyName;
	//	家族称号
	string	m_strFamilyTitle;
};
#endif//_OrganiseSys_H_