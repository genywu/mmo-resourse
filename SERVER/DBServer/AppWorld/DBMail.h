#ifndef DB_MAIL_H
#define DB_MAIL_H
#include "BaseDBEntity.h"
#include "../public/DBDefine.h"

using namespace std;

class CDBMailGoodsGroup;

class CDBMail : public CBaseDBEntity
{
private:
	CDBMail(void);
public:
	CDBMail(const CGUID& id);
	virtual ~CDBMail(void);

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void AddToByteArray(vector<BYTE>& pBA);

	virtual CDBMail& operator=(CDBMail& right);

	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);
	void				InitProperty(void);

	CDBMailGoodsGroup* GetGoodsGroupPtr(void) { return m_pGoodsGroupPtr; }
	const CGUID& GetOwnerID(void) { return m_OwnerID; }
	void		 SetOwnerID(const CGUID& guid) { m_OwnerID = guid; }
private:
	CDBMailGoodsGroup* m_pGoodsGroupPtr;
	CGUID				m_OwnerID;
};

#endif//DB_LOGIN_PLAYER_H