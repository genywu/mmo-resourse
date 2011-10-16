


#ifndef _TeamMgr_H_
#define _TeamMgr_H_
#pragma once

class CTeamMgr
{
public:
	CTeamMgr(void);
	~CTeamMgr(void);

	void Release();

	void DecodeFromDataBlock(DBReadSet& dbRead);

	// 获得和设置招募状态
	bool IsRecruitingTeam(){return m_bIsRecruitingTeam;}
	void SetRecruitingTeam(bool b){m_bIsRecruitingTeam=b;}

	// 获得和设置队伍成员的数量
	long GetTeammateAmount(){return m_lTeammateAmount;}
	void SetTeammateAmount(long l){m_lTeammateAmount=l;}

	// 获得和设置队伍的名字
	const char * GetTeamname(){return m_strTeamname.c_str();}
	void SetTeamname(const char *strName){m_strTeamname=strName;}

	// 获得和设置队伍是否存在密码
	bool IsTeamHasPassword(){return m_bTeamHasPassword;}
	void SetTeamHasPassword(bool b){m_bTeamHasPassword=b;}
	
	//获得和设置队伍队长
	void SetIsTeamLeader(bool b)						{ m_bIsTeamLeader = b; }
	bool IsTeamLeader()									{ return m_bIsTeamLeader; }

	//获得和设置要求方GUID
	void SetTeamRequesterGuid(const CGUID Rguid)		{RequesterGuid = Rguid;}
	CGUID GetTeamRequesterGuid()						{return RequesterGuid;}
 
private:
	bool	m_bIsRecruitingTeam;
	long	m_lTeammateAmount;
	string	m_strTeamname;
	bool	m_bTeamHasPassword;
	bool    m_bIsTeamLeader;
	CGUID   RequesterGuid;
};
#endif//_TeamMgr_H_