#pragma once
#include "..\myadobase.h"


class CRsCDKey :
	public CMyAdoBase
{
public:
	CRsCDKey(void);
	virtual ~CRsCDKey(void);
    
	int CDKeyIsValidate(DWORD dwIP, const char *cdkey,vector<BYTE>& password, char *szOutBanTime);//1,CDKey可用 2,CDKey or password error 3,cdkey in ban(pOutBantime not null)
	bool CDKeyBan(const char* cdkey,long lminute);
	bool GetPassWord(_RecordsetPtr& rs,BYTE* pByte);
	bool GetPassWord(_RecordsetPtr& rs,string& strMD5);
	
	// 针对面向网吧IP绑定，如果绑定IP后，既然玩家的账号未被开启，同样可以在该网吧内进行登录游戏。
	// IP字段为空则不通过此检测
	bool IPIsAllowed(DWORD dwIP);
	bool IPIsForbidded(DWORD dwIP);

	// 现主要是针对GM登录进行控制，只允许他们在指定IP列表中允许的范围内进行登录游戏。
	// IP列表为空则不通过此检测
	bool IsBetweenIP(const char *szCdkey, DWORD dwIP);
	
};
