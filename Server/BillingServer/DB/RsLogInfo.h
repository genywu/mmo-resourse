#ifndef  _INC_RSLOGINFO
#define  _INC_RSLOGINFO

#pragma once
#include "../../../Public/ServerPublic/db/CnPool.h"
class CRsLogInfo :
	public CMyAdoBase
{
public:
	CRsLogInfo(void);
	virtual ~CRsLogInfo(void);

	// 用户在登录时，创建一条日志记录，含有基本用户信息、游戏角色信息和上线时间
	bool AccountEnterLog(LPCSTR szAccount, DWORD dwIP);

	// 用户在进入角色时，更新该记录进入角色时间
	bool RoleEnterLog(LPCSTR szAccount, LPCSTR szRoleName, BYTE btRoleLevel, BYTE btWorldNumber);

	// 用户下线时，更新该记录的角色下线时间，和帐户离开时间（2个时间相同）
	bool LeaveLog(LPCSTR szAccount);

	bool AccountLeaveLog(LPCSTR szAccount);

protected:
	// 获得当前时间的串
	void GetSystemTimeAsString(OUT LPSTR szCurrentTime);

	// 通过用Account和Role等关系确定LogInfoID，用于更新(update)
	DWORD GetLogInfoIDForUpdate(LPCSTR szAccount);
};

#endif //_INC_RSLOGINFO