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
	bool ApplyForJoin(CGUID& PlayerID,long lLvl,long lOrgMemID = 0);		//无行会成员申请加入
	bool Invite(const CGUID& InvitedID, const CGUID& BeInvitedID);					//邀请人员加入
	bool Exit(const CGUID& PlayerID);											//退出
	bool FireOut(const CGUID& FireID,const CGUID& BeFiredID);							//开除
	bool DubAndSetJobLvl(const CGUID& DubID,const CGUID& BeDubedID,string& strTitle,long lLvl);//授予称号
	//bool EndueRightOfConMem(long lEndueID,long lBeEnduedID);			//赋予征召新人权利
	//bool EndueRightOfFireOut(long lEndueID,long lBeEnduedID);			//赋予开除权利
	bool Pronounce(const CGUID& PlayerID,string& strPro,tagTime& Time);		//宣告
	bool LeaveWord(const CGUID& PlayerID,string& strWords,tagTime& Time);		//留言
};
