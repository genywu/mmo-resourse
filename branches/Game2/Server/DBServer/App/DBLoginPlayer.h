#ifndef DB_LOGIN_PLAYER_H
#define DB_LOGIN_PLAYER_H
#include "BaseDBEntity.h"
#include "../public/DBDefine.h"

using namespace std;


class CDBLoginPlayer : public CBaseDBEntity
{
private:
	CDBLoginPlayer(void);
public:
	CDBLoginPlayer(const CGUID& id);
	virtual ~CDBLoginPlayer(void);

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void AddToByteArray(vector<BYTE>& pBA);
	virtual CDBLoginPlayer& operator=(CDBLoginPlayer& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
		}
		return *this;
	}

	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);
	void				InitProperty(void);

	void SetAccount(const char* szCdkey, long size) { if(szCdkey) m_strCdkey = szCdkey; }
	const char* GetAccount(void) { return m_strCdkey.c_str(); }

	void SetLogoutTime(long time) { m_LogoutTime = time; }
	long GetLogoutTime(void)	{ return m_LogoutTime; }
private:
	long			m_LogoutTime;
	string			m_strCdkey;
};

#endif//DB_LOGIN_PLAYER_H