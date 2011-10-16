//////////////////////////////////////////////////////////////////////////
//FactionWarSys.h
//Function:帮战系统相关功能
//Author:Joe
//Create Time:2005.6.27
//////////////////////////////////////////////////////////////////////////

#pragma once


class CFactionWarSys
{
public:
	CFactionWarSys(void);
	~CFactionWarSys(void);
private:
	static	CFactionWarSys* pInstance;
public:
	static	CFactionWarSys* GetInstance();
	void	Release();

	bool DigUpTheHatchet(LONG64 lNetSessionID,long lPassword,const CGUID& FactionID,long lLevl);	//行会宣战
	void CloseDecWarSession(LONG64 lNetSessionID,long lPassword);							//关闭宣战会话
};

CFactionWarSys* GetFactionWarSys();
