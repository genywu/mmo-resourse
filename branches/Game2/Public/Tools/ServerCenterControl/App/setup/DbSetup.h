







#pragma once



struct tagThreadSetup
{
	DWORD	dwMinDbOptThreadNum;
	DWORD	dwMaxDbOptThreadNum;
	DWORD	dwThreadNumAdjustSpace;

	tagThreadSetup(VOID){memset(this, 0, sizeof(tagThreadSetup));}
};

////////////////////////////////////
class DbSetup
{
public:
	DbSetup(VOID);
	~DbSetup(VOID);

public:
	static			DbSetup&			GetInstance(VOID);

public:
	//!									ªÒ»°≈‰÷√
	BOOL								LoadSetup(const char *pXmlFilename);

	//!
	//!
	const	tagThreadSetup*				GetThreadSetup(VOID);
	//!
	const	string&						GetConnectString(DWORD dwDBID);

private:
	map<DWORD, string>					m_mapConnectString;
	tagThreadSetup						m_ThreadSetup;

private:
	static	DbSetup	c_DbSetup;
};