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

//无帮派成员加入
bool CNoOrganizing::ApplyForJoin(long lPlayerID,long lLvl,long lOrgMemID /*= 0*/)
{
	//是自由人员
	return true;
}
//邀请人员加入
bool CNoOrganizing::Invite(long lInvitedID, long lBeInvitedID)
{
	return true;
}
//退出
bool CNoOrganizing::Exit(long lPlayerID)
{
	return true;
}
//开除
bool CNoOrganizing::FireOut(long lFireID,long lBeFiredID)
{	
	return true;
}
//授予称号
bool CNoOrganizing::DubAndSetJobLvl(long lDubID,long lBeDubedID,string& strTitle,long lLvl)
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
bool CNoOrganizing::Pronounce(long lPlayerID,string& strPro,tagTime& Time)
{	
	return true;
}
//留言
bool CNoOrganizing::LeaveWord(long lPlayerID,string& strWords,tagTime& Time)
{
	return true;
}