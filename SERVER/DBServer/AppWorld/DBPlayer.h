#ifndef DB_PLAYER_H
#define DB_PLAYER_H
#include "BaseDBEntity.h"
#include "DBGood.h"
#include "../public/DepotDef.h"
#include "../public/LinkmanDef.h"
#include <string>
#include <list>
#include <map>
#include "../public/DBDefine.h"
#include "../public/AutoSupplyData.h"
#include "../public/PlayerSetupOnServer.h"

struct SaveThreadData;

using namespace std;

class CDBMail;
class CDBLoginPlayer;
class CDBGoodsGroup;
class CDBLinkmanGroup;
class CDBMailGroup;

class CDBPlayer : public CBaseDBEntity
{
public:
	enum EQUIPMENT_COLUMN 
	{	
		EC_HEAD,				//	1：头盔
		EC_NECKLACE,			//	2：项链
		EC_WING,				//	3：翅膀
		EC_BODY,				//	4：盔甲
		EC_BACK,				//	5：腰带
		EC_GLOVE,			//6：手套	
		EC_BOOT,			//7：鞋子
		EC_HEADGEAR,			//	8：头饰
		EC_FROCK,			//	9：外套		
		EC_LRING,			//	10：左戒指
		EC_RRING,			//	11：右戒指		
		EC_MEDAL1,			//	12：勋章1
		EC_MEDAL2,			//	13	勋章2
		EC_MEDAL3,			//	14	勋章3
		EC_FAIRY,			//	15：小精灵
		EC_WEAPON,			//	16：武器
		EC_WEAPON2,			//	17：副手武器
		EC_TOTAL,

		EC_PACK1=10001,
		EC_PACK2=10002,
		EC_PACK3=10003,
		EC_PACK4=10004,
		EC_PACK5=10005
	};

private:
	CDBPlayer();
public:
	CDBPlayer(const CGUID& guid);
	virtual ~CDBPlayer();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void SaveForThread(long msgid, BYTE* msgBuf, long& msgBufPos, SaveObj* saveData, long bufSize=-1);

	void AddToByteArray(vector<BYTE>& pBA);

	void SetAccount(const char* szCdkey, long size) { if(szCdkey) m_strCdkey = szCdkey; }
	const char* GetAccount(void) { return m_strCdkey.c_str(); }

	virtual CDBPlayer& operator=(CDBPlayer& right);
	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);
	void				InitProperty(void);

	CDBGoodsGroup* GetGoodsGroupPtr(void);
	CDBLinkmanGroup* GetLinkmanGroupPtr(void);
	CDBMailGroup* GetMailGroupPtr(void);

	void SetGoodsGroupPtr(CDBGoodsGroup* goodsGroup);
	void GetLinkmanGroupPtr(CDBLinkmanGroup* linkGroup);
	void GetMailGroupPtr(CDBMailGroup* mailGroup);

	// CDBMail
	// 添加一个CDBMail到Map末尾
	void AddOneDBMail(CDBMail* mail);
	// 删除一个CDBMail
	void DelOneDBMail(CDBMail* mail);
	// 清空链表
	void ClearDBMailMap(void);


private:
	CDBGoodsGroup* p_GoodsGroup;
	CDBLinkmanGroup* p_LinkmanGroup;
	CDBMailGroup* p_MailGroup;

private:
	string			m_strCdkey;
};

#endif//DB_PLAYER_H
