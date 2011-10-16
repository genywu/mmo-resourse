#ifndef DB_LOGIN_PLAYER_H
#define DB_LOGIN_PLAYER_H
#include "BaseDBEntity.h"
#include "../public/DBDefine.h"
using namespace std;

class CDBLoginPlayer : public CBaseDBEntity
{
public:
	CDBLoginPlayer(void);
	CDBLoginPlayer(const CGUID& guid);
	~CDBLoginPlayer(void);

	// override = operator
	CDBLoginPlayer& operator=(CDBLoginPlayer& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			m_GUID = right.m_GUID;
		}
		return *this;
	}

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void AddToByteArray(vector<BYTE>& pBA);

	// 初始化属性配置
	void				InitProperty();

	void SetAccount(const char* szCdkey, long size) { if(szCdkey) m_strCdkey = szCdkey; }
	const char* GetAccount(void) { return m_strCdkey.c_str(); }

	// 删除时间
	long GetDelTime(void) { return m_DelTime; }
	void SetDelTime(long value) { m_DelTime = value; }

	long					GetMailInitLoadFlag(void)   {return m_lMailInitLoadFlag;}
	void					SetMailInitLoadFlag(long flag) { m_lMailInitLoadFlag = flag; }

private:
	string m_strCdkey;
	
	long    m_DelTime;
	long					m_lMailInitLoadFlag;
};

#endif//DB_LOGIN_PLAYER_H