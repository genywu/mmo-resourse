#pragma once
#include "..\myadobase.h"
#include "../public/GUID.h"
#include <map>
#include "../public/DBDefine.h"
//#include "..\..\server\dbserver\appworld\DBCityWar.h"
class CBaseDBEntity;
class CDBMail;
class CDBMailGoods;
class CDBMailGoodsGroup;
class CDBMailGroup;

class CRsMail :
	public CMyAdoBase
{
public:
	CRsMail(void);
	~CRsMail(void);

public:

	//! 查询邮件
	bool	QueryPlayerMails(long curTime, CDBMailGroup* mailGroup, _ConnectionPtr& cn);
	//! 读取玩家的所有邮件信息
	bool	LoadPlayerMails(const CGUID& playerID, CDBMailGroup* mailGroup, _ConnectionPtr& cn);
	//! 读取邮件信息
	void    LoadMail(CDBMail* mail, _RecordsetPtr& pRs, _ConnectionPtr& cn);
	//! 保存玩家的所有邮件信息
	DB_OPERATION_RESULT_TYPE	SavePlayerMails(CDBMailGroup* mailGroup, _ConnectionPtr& cn);
	//! 保存邮件信息
	void    SaveMail(CDBMail* mail, _ConnectionPtr& cn);
	//! 删除玩家的所有邮件信息
	DB_OPERATION_RESULT_TYPE	DeletePlayerMails(const CGUID& playerID, _ConnectionPtr& cn);
	//! 删除邮件信息
	DB_OPERATION_RESULT_TYPE	DeleteMails(CDBMailGroup* mailsGroup, _ConnectionPtr& cn);
	//! 删除邮件信息
	void    DeleteMail(const CGUID& guid, _ConnectionPtr& cn);

	//! 读取邮件的物品信息
	DB_OPERATION_RESULT_TYPE		LoadMailGoods(const CGUID& mailGUID, CDBMailGoodsGroup* goodsGroup, _ConnectionPtr& cn);
	//! 保存邮件的物品信息
	static DB_OPERATION_RESULT_TYPE SaveMailGoods(const CGUID& mailGUID, set<CDBMailGoods*>& goodsGroup, _ConnectionPtr& cn);

	//! 删除玩家的所有邮件物品信息
	bool	DeletePlayerMailsGoods(set<CGUID>& mailSet, _ConnectionPtr& cn);
	//! 删除邮件的物品信息
	void    DeleteMailGoods(const CGUID& mailID);

	//! 数据库里添加系统信件
	void	InsertSysMailToGroup(CDBMail* pMail, MAIL_LEVEL_OPER_FLAG levelOpFlag, long level, long sex, long* countryID, long countryNum, _ConnectionPtr& cn);
	//存Buffer
	static bool SaveBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &pRs);
	//读Buffer
	bool LoadBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &pRs);
};
