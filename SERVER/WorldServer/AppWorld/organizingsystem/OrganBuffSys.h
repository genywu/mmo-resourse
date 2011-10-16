


/*
*	file:		OrganBuffSys.h
*	Brief:		组织系统的buff系统
*	detail:		管理组织系统的相关buff操作
*				
*				
*	Authtor:	张科智
*	Datae:		2008-11-03
*
*/



#pragma once




class OrganBuffSys
{
public:
	//! 组织类型
	enum eOrganType
	{
		eOT_Faction,
		eOT_Union,
	};

public:
	OrganBuffSys(VOID);
	OrganBuffSys(DWORD dwOrganType);
	~OrganBuffSys(VOID);

public:
	//!					读取配置
	static	BOOL		LoadSetup(const char *pFilename);

	//!					编码
	BOOL				AddToByteArray(vector<BYTE>& ByteArray);
	//!					解码
	BOOL				Decode(BYTE* pData, LONG &lPos);


public:
	//!					添加一个可用的buff
	BOOL				AddBuff(DWORD dwBuff);
	//!					删除一个可用的buff
	BOOL				DelBuff(DWORD dwBuff);

	//!					开启一个buff
	BOOL				StartBuff(DWORD dwBuff);
	//!					关闭一个buff
	BOOL				CloseBuff(DWORD dwBuff);


private:
	//!		buff配置
	struct	tagOrgBuffSetup
	{
		//!		buff编号
		DWORD	_dwBuffID;
		//!		默认的开启状态
		BOOL	_bOpenState;
		//!		开启等级
		DWORD	_dwOpenLvl;
	};

	//!										工会配置
	static	map<DWORD, tagOrgBuffSetup>		m_mapFactionSetup;
	//!										联盟配置
	static	map<DWORD, tagOrgBuffSetup>		m_mapUnionSetup;
};