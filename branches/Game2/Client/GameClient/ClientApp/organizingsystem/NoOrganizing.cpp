#include "StdAfx.h"
#include ".\noorganizing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNoOrganizing::CNoOrganizing(void)
{
}

CNoOrganizing::~CNoOrganizing(void)
{
}

//无行会成员加入
bool CNoOrganizing::ApplyForJoin(CGUID& PlayerID,long lLvl,long lOrgMemID /*= 0*/)
{
	//是自由人员
	return true;
}
//邀请人员加入
bool CNoOrganizing::Invite(const CGUID& InvitedID, const CGUID& BeInvitedID)
{
	return true;
}
//退出
bool CNoOrganizing::Exit(const CGUID& PlayerID)
{
	return true;
}
//开除
bool CNoOrganizing::FireOut(const CGUID& FireID,const CGUID& BeFiredID)
{	
	return true;
}
//授予称号
bool CNoOrganizing::DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle,long lLvl)
{	
	return true;
}
////赋予征召新人权利
//bool CNoOrganizing::EndueRightOfConMem(long lEndueID,long lBeEnduedID)
//{	
//	return true;
//}
////赋予开除权利
//bool CNoOrganizing::EndueRightOfFireOut(long lEndueID,long lBeEnduedID)
//{	
//	return true;
//}
//宣告
bool CNoOrganizing::Pronounce(const CGUID& PlayerID,string& strPro,tagTime& Time)
{	
	return true;
}
//留言
bool CNoOrganizing::LeaveWord(const CGUID& PlayerID,string& strWords,tagTime& Time)
{
	return true;
}