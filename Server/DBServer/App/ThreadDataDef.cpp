#include "StdAfx.h"
#include "ThreadDataDef.h"
#include "DBLoginPlayer.h"
#include "DBPlayer.h"
#include "dbregion.h"
#include "DBDupRgn.h"
#include "DBRgnGoods.h"
#include "DBFaction.h"
#include "DBCountry.h"
#include "DBGoodsGroup.h"
#include "DBGenVarGroup.h"
#include "DBRgnGoodsGroup.h"
#include "DBLinkManGroup.h"
#include "DBFactionApplyGroup.h"
#include "DBFactionMemberGroup.h"
#include "DBMail.h"
#include "DBMailGroup.h"
#include "DBEntityDef.h"
#include "DBentityManager.h"
#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LoadThreadData::LoadThreadData(const CGUID& guid, DB_OBJ_TYPE type)
:dbObj(NULL),loadObj(NULL)
{
	switch(type)
	{
	case DB_GENVARGROUP:
		dbObj = new CDBGenVarGroup(guid);
		break;
	case DB_MAIL:
		dbObj = new CDBMail(guid);
		break;
	case DB_MAILGROUP:
		dbObj = new CDBMailGroup(guid);
		break;
	case DB_FACTIONAPPLYGROUP:
		dbObj = new CDBFactionApplyGroup(guid);
		break;
	case DB_FACTIONMEMBERGROUP:
		dbObj = new CDBFactionMemberGroup(guid);
		break;
	case DB_LINKMANGROUP:
		dbObj = new CDBLinkmanGroup(guid);
		break;
	case DB_GOODSGROUP:
		dbObj = new CDBGoodsGroup(guid);
		break;
	case DB_PLAYER:
		dbObj = new CDBPlayer(guid);
		break;
	case DB_LOGINPLAYER:
		dbObj = new CDBLoginPlayer(guid);
		break;
	case DB_REGION:
		dbObj = new CDBRegion(guid);
		break;
	case DB_PHDUPRGN:
		dbObj = new CDBDupRgn(guid);
		break;
	case DB_PHRGNGOODS:
		dbObj = new CDBRgnGoods(guid);
			break;
	case DB_PHRGNGOODSGROUP:
		dbObj = new CDBRgnGoodsGroup(guid);
				break;
	case DB_COUNTRY:
		dbObj = new CDBCountry(guid);
		break;
	}
	loadObj  = new LoadObj;
	loadObj->objGUID = guid;
	loadObj->objType = type;
}
LoadThreadData::LoadThreadData()
:dbObj(NULL),loadObj(NULL)
{
	
}
LoadThreadData::~LoadThreadData()
{
	SAFE_DELETE(dbObj);
	SAFE_DELETE(loadObj);
}
LoadThreadData& LoadThreadData::operator=(const LoadThreadData& right)
{
	if(this != &right)
	{
		dbObj   = right.dbObj;
		loadObj = right.loadObj;
	}

	return *this;
}
SaveThreadData::SaveThreadData()
:dbObj(NULL),saveObj(NULL)
{
}
SaveThreadData::SaveThreadData(const CGUID& guid, DB_OBJ_TYPE type)
:dbObj(NULL),saveObj(NULL)
{
	switch(type)
	{
	case DB_GENVARGROUP:
		dbObj = new CDBGenVarGroup(guid);
		break;
	case DB_MAILGROUP:
		dbObj = new CDBMailGroup(guid);
		break;
	case DB_FACTIONAPPLYGROUP:
		dbObj = new CDBFactionApplyGroup(guid);
		break;
	case DB_FACTIONMEMBERGROUP:
		dbObj = new CDBFactionMemberGroup(guid);
		break;
		case DB_PHRGNGOODSGROUP:
			dbObj = new CDBRgnGoodsGroup(guid);
			break;
		case DB_LINKMANGROUP:
			dbObj = new CDBLinkmanGroup(guid);
			break;
		case DB_PLAYER:
			dbObj = new CDBPlayer(guid);
			break;
		case DB_LOGINPLAYER:
			dbObj = new CDBLoginPlayer(guid);
			break;
		case DB_REGION:
			dbObj = new CDBRegion(guid);
			break;
		case DB_PHDUPRGN:
			dbObj = new CDBDupRgn(guid);
			break;
		case DB_FACTION:
			dbObj = new CDBFaction(guid);
			break;
		case DB_GOODSGROUP:
			dbObj = new CDBGoodsGroup(guid);
	}
	saveObj  = new SaveObj;
	saveObj->objType = type;
}
SaveThreadData::~SaveThreadData()
{
	SAFE_DELETE(dbObj);
	SAFE_DELETE(saveObj);
}