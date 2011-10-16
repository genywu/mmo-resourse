#pragma once
#include "organizing.h"

class CNoOrganizing :
	public COrganizing
{
public:
	CNoOrganizing(void);
	~CNoOrganizing(void);

	//无组织相关的功能操作
public:
	bool ApplyForJoin(long lPlayerID,long lLvl,long lOrgMemID = 0);		//无帮派成员申请加入
	bool Invite(long lInvitedID, long lBeInvitedID);					//邀请人员加入
	bool Exit(long lPlayerID);											//退出
	bool FireOut(long lFireID,long lBeFiredID);							//开除
	bool DubAndSetJobLvl(long lDubID,long lBeDubedID,string& strTitle,long lLvl);//授予称号
	//bool EndueRightOfConMem(long lEndueID,long lBeEnduedID);			//赋予征召新人权利
	//bool EndueRightOfFireOut(long lEndueID,long lBeEnduedID);			//赋予开除权利
	bool Pronounce(long lPlayerID,string& strPro,tagTime& Time);		//宣告
	bool LeaveWord(long lPlayerID,string& strWords,tagTime& Time);		//留言
};
