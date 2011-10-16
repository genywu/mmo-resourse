#include "StdAfx.h"
#include "ServerRegion.h"

#include "Area.h"
#include "Business/CBusinessManager.h"
#include "CityGate.h"
#include "Colony.h"
#include "Collection.h"
#include "Country/CountryParam.h"
#include "Goods/CGoods.h"
#include "Goods/CGoodsFactory.h"
#include "Monster.h"
#include "OrganizingSystem/GameOrganizingCtrl.h"
#include "OrganizingSystem/GSTeam.h"
#include "Pet.h"
#include "Player.h"
#include "RgnManager.h"
#include "StateAI/AIEventSender.h"
#include "StateAI/MonsterAI.h"
#include "StateAI/Helper/RunByRoad.h"
#include "../Server/GameManager.h"

#include "../../../Public/Common/NetSession.h"
#include "../../../Public/Common/NetSessionManager.h"
#include "../../../Public/Common/DeputyOccu.h"
#include "../../../Public/Setup/GlobalSetup.h"
#include "../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../../../Public/ServerPublic/Server/Common/VariableList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long g_lTotalMonster = 0;
long g_lTotalNpc = 0;


CServerRegion::CServerRegion()
: CRegion(NULL_GUID), m_ID(0), m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_lRgnDelTimerID(-1),b_IsForceDelFlag(false)
{
	m_lAreaX = 0;
	m_lAreaY = 0;
	m_pArea = NULL;

	//	m_lNpcID = 1;
	//m_lMonsterID = 1;

	m_lCurWeatherTimeSegment	= 0;
	m_lCurWeatherCount			= 0;

	memset(&m_Param,0,sizeof(tagRegionParam));
	m_lWarNum = 0;
	m_CityState=CIS_NO;
	m_WarRegionType = RWT_Normal;

	m_bNoPk = 0;
	m_bNoContribute = false;
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;

	m_bKickOutPlayer= false;
	m_lKickOutPlayerTime = 0;
	m_dwLastTime = timeGetTime();
	m_pVariableList = OBJ_CREATE(VariableList);
	m_bIsHibernate=TRUE;
	//场景天气定时器
	m_lWeatherTimerID = -1;

	//添加Shape的对象列表
	m_AddShapes.clear();
	m_lAddShapeTimerID = -1;
	RegisterAddShapeTimer();

	InitNameValueMap();
	m_bStrongPintFlag=false;
}

CServerRegion::CServerRegion(long id)
: CRegion(NULL_GUID), m_ID(id), m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_lRgnDelTimerID(-1),b_IsForceDelFlag(false)
{
	m_lAreaX = 0;
	m_lAreaY = 0;
	m_pArea = NULL;

	//	m_lNpcID = 1;
	//m_lMonsterID = 1;

	m_lCurWeatherTimeSegment = 0;
	m_lCurWeatherCount = 0;

	memset(&m_Param,0,sizeof(tagRegionParam));
	m_lWarNum = 0;
	m_CityState=CIS_NO;
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;

	m_bNoPk = 0;
	m_bNoContribute = false;

	m_bKickOutPlayer= false;
	m_lKickOutPlayerTime = 0;
	m_dwLastTime = timeGetTime();
	m_pVariableList = OBJ_CREATE(VariableList);
	m_bIsHibernate=TRUE;
	//场景天气定时器
	m_lWeatherTimerID = -1;
	//添加Shape的对象列表
	m_AddShapes.clear();
	m_lAddShapeTimerID = -1;
	RegisterAddShapeTimer();

	InitNameValueMap();
	m_bStrongPintFlag=false;
}
CServerRegion::CServerRegion(const CGUID& guid)
:CRegion(guid), m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_lRgnDelTimerID(-1),b_IsForceDelFlag(false)
{
	m_lAreaX = 0;
	m_lAreaY = 0;
	m_pArea = NULL;

	//	m_lNpcID = 1;
	//m_lMonsterID = 1;

	m_lCurWeatherTimeSegment = 0;
	m_lCurWeatherCount = 0;

	memset(&m_Param,0,sizeof(tagRegionParam));
	m_lWarNum = 0;
	m_CityState=CIS_NO;
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;

	m_bNoPk = 0;
	m_bNoContribute = false;

	m_bKickOutPlayer= false;
	m_lKickOutPlayerTime = 0;
	m_dwLastTime = timeGetTime();
	m_pVariableList = OBJ_CREATE(VariableList);
	m_bIsHibernate=TRUE;
	//场景天气定时器
	m_lWeatherTimerID = -1;
	//添加Shape的对象列表
	m_AddShapes.clear();
	m_lAddShapeTimerID = -1;
	RegisterAddShapeTimer();

	InitNameValueMap();
	m_bStrongPintFlag=false;
}
CServerRegion::CServerRegion(void* pParam)
:CRegion(*((CGUID*)pParam)), m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_lRgnDelTimerID(-1),b_IsForceDelFlag(false)
{
	m_lAreaX = 0;
	m_lAreaY = 0;
	m_pArea = NULL;

	//	m_lNpcID = 1;
	//m_lMonsterID = 1;

	m_lCurWeatherTimeSegment = 0;
	m_lCurWeatherCount = 0;

	memset(&m_Param,0,sizeof(tagRegionParam));
	m_lWarNum = 0;
	m_CityState=CIS_NO;
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;

	m_bNoPk = false;
	m_bNoContribute = false;

	m_bKickOutPlayer= false;
	m_lKickOutPlayerTime = 0;
	m_dwLastTime = timeGetTime();
	m_pVariableList = OBJ_CREATE(VariableList);
	m_bIsHibernate=TRUE;
	//场景天气定时器
	m_lWeatherTimerID = -1;
	//添加Shape的对象列表
	m_AddShapes.clear();
	m_lAddShapeTimerID = -1;
	RegisterAddShapeTimer();

	InitNameValueMap();
}
CServerRegion::CServerRegion(const CGUID& guid, bool bNoPk)
:CRegion(guid), m_pVariableList(NULL),m_DupRgnPlayerNumMax(1024),
b_OperFlag(false),m_DelTime(0),m_CostValue(0),m_lRgnDelTimerID(-1),b_IsForceDelFlag(false),m_dwLastWarningTime(0)
{
	m_lAreaX = 0;
	m_lAreaY = 0;
	m_pArea = NULL;

	//	m_lNpcID = 1;
	//m_lMonsterID = 1;

	m_lCurWeatherTimeSegment = 0;
	m_lCurWeatherCount = 0;

	memset(&m_Param,0,sizeof(tagRegionParam));
	m_lWarNum = 0;
	m_CityState=CIS_NO;
	m_Ride=true;
	m_ChangeEquip = true;
	m_eSpaceType = RST_Land;	
	m_WarRegionType = RWT_Normal;

	m_bNoPk = bNoPk;
	m_bNoContribute = false;

	m_bKickOutPlayer= false;
	m_lKickOutPlayerTime = 0;
	m_dwLastTime = timeGetTime();
	m_pVariableList = OBJ_CREATE(VariableList);
	m_bIsHibernate=TRUE;
	//场景天气定时器
	m_lWeatherTimerID = -1;
	//添加Shape的对象列表
	m_AddShapes.clear();
	m_lAddShapeTimerID = -1;
	RegisterAddShapeTimer();

	InitNameValueMap();

	m_bStrongPintFlag=false;
}

CServerRegion::~CServerRegion(void)
{
	DeleteAllChildObject();
	if(m_pVariableList)
		OBJ_RELEASE(VariableList, m_pVariableList);

	//test.调试信息，如果在释放对象的时候，没有把定时器注销完，提示严重错误
	int nTimeNum = GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Cancel(this);
	/*if(nTimeNum > 0 && !g_bGameThreadExit)
	{
	Log4c::Error(ROOT_MODULE,"严重错误，释放对象(Type:%d)时候，有%d个定时器未注销!",GetType(), nTimeNum);
	}*/
	ReleaseAllColony();

	//DelectAllScripByShape(GetExID());
}


// 初始化
void CServerRegion::InitNameValueMap()
{
	m_mapNameValue.clear();
	tagPty st;// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool
	//场景类型
	st.SetType(4); st.pValue = &m_lRgnType;		st.attrEnumValue=R_ATTR_RGNTYPE;		m_mapNameValue["RgnType"]=st;
	//功勋系数
	st.SetType(3); st.pfValue = &m_fMeriScale;	st.attrEnumValue=R_ATTR_MERISCALE;		m_mapNameValue["MeriScale"]=st;
	//国家
	st.SetType(0); st.plValue = &m_btCountry;	st.attrEnumValue=R_ATTR_COUNTRY;		m_mapNameValue["RgnCountry"]=st;
	//是否允许PK
	st.SetType(0); st.plValue = &m_bNoPk;		st.attrEnumValue=R_ATTR_ISPK;			m_mapNameValue["IsPk"]=st;
	//是否有贡献
	st.SetType(6); st.pbValue = &m_bNoContribute;st.attrEnumValue=R_ATTR_ISCONTRIBUTE;	m_mapNameValue["IsContribute"]=st;
	//是否可以坐骑
	st.SetType(6); st.pbValue = &m_Ride;			st.attrEnumValue=R_ATTR_ISRIDE;			m_mapNameValue["IsRide"]=st;
	//场景空间类型
	st.SetType(2); st.pdwValue = (DWORD*)&m_eSpaceType;		st.attrEnumValue=R_ATTR_SPACETYPE;		m_mapNameValue["SpaceType"]=st;	
	//场景战斗类型
	st.SetType(2); st.pdwValue	= (DWORD*)&m_WarRegionType;	st.attrEnumValue=R_ATTR_WARTYPE;		m_mapNameValue["WarType"]=st;	

	//是否使用复活
	st.SetType(6);	st.pbValue=&m_stSetup.bReLiveIsUsed;	st.attrEnumValue=0;						m_mapNameValue["ReLiveIsUsed"]=st;
	//复活类型
	st.SetType(6);	st.pbValue=&m_stSetup.bReLivePointFlag;	st.attrEnumValue=0;						m_mapNameValue["ReLivePointFlag"]=st;
	//复活场景ID
	st.SetType(4);	st.pValue=&m_stSetup.lReLiveRegionID;	st.attrEnumValue=0;						m_mapNameValue["ReLiveRegionID"]=st;
	//复活坐标
	st.SetType(4);	st.pValue=&m_stSetup.lReLivePosX1;				st.attrEnumValue=0;						m_mapNameValue["ReLivePosX1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lReLivePosX2;				st.attrEnumValue=0;						m_mapNameValue["ReLivePosX2"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lReLivePosY1;				st.attrEnumValue=0;						m_mapNameValue["ReLivePosY1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lReLivePosY2;				st.attrEnumValue=0;						m_mapNameValue["ReLivePosY2"]=st;

	//是否使用下线回程
	st.SetType(6);	st.pbValue=&m_stSetup.bLogoutIsUsed;			st.attrEnumValue=0;						m_mapNameValue["LogoutIsUsed"]=st;
	//下线回程场景ID
	st.SetType(4);	st.pValue=&m_stSetup.lLogoutRegionID;			st.attrEnumValue=0;						m_mapNameValue["LogoutRegionID"]=st;
	//下线回程场景坐标
	st.SetType(4);	st.pValue=&m_stSetup.lLogoutPosX1;				st.attrEnumValue=0;						m_mapNameValue["LogoutPosX1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lLogoutPosX2;				st.attrEnumValue=0;						m_mapNameValue["LogoutPosX2"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lLogoutPosY1;				st.attrEnumValue=0;						m_mapNameValue["LogoutPosY1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lLogoutPosY2;				st.attrEnumValue=0;						m_mapNameValue["LogoutPosY2"]=st;

	//是否使用符文石回城
	st.SetType(6);	st.pbValue=&m_stSetup.bRuneStoneIsUsed;			st.attrEnumValue=0;						m_mapNameValue["RuneStoneIsUsed"]=st;
	//符文石回城场景ID
	st.SetType(4);	st.pValue=&m_stSetup.lRuneStoneRegionID;		st.attrEnumValue=0;						m_mapNameValue["RuneStoneRegionID"]=st;
	//符文石回城场景坐标
	st.SetType(4);	st.pValue=&m_stSetup.lRuneStonePosX1;			st.attrEnumValue=0;						m_mapNameValue["RuneStonePosX1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lRuneStonePosX2;			st.attrEnumValue=0;						m_mapNameValue["RuneStonePosX2"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lRuneStonePosY1;			st.attrEnumValue=0;						m_mapNameValue["RuneStonePosY1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lRuneStonePosY2;			st.attrEnumValue=0;						m_mapNameValue["RuneStonePosY2"]=st;
	//符文石回城点是否可以记录
	st.SetType(6);	st.pbValue=&m_stSetup.bRuneStoneCanRecord;		st.attrEnumValue=0;						m_mapNameValue["RuneStoneCanRecord"]=st;


	//是否使用副本回城点
	st.SetType(6);	st.pbValue=&m_stSetup.bDupRgnIsUsed;			st.attrEnumValue=0;						m_mapNameValue["DupRgnIsUsed"]=st;
	//副本回城点场景ID
	st.SetType(4);	st.pValue=&m_stSetup.lDupRgnRegionID;			st.attrEnumValue=0;						m_mapNameValue["DupRgnRegionID"]=st;
	//副本回城点场景坐标
	st.SetType(4);	st.pValue=&m_stSetup.lDupRgnPosX1;				st.attrEnumValue=0;						m_mapNameValue["DupRgnPosX1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lDupRgnPosX2;				st.attrEnumValue=0;						m_mapNameValue["DupRgnPosX2"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lDupRgnPosY1;				st.attrEnumValue=0;						m_mapNameValue["DupRgnPosY1"]=st;
	st.SetType(4);	st.pValue=&m_stSetup.lDupRgnPosY2;				st.attrEnumValue=0;						m_mapNameValue["DupRgnPosY2"]=st;
	//原地复活开关
	st.SetType(6);	st.pbValue=&m_stSetup.bReLiveCurPosFlag;		st.attrEnumValue=0;						m_mapNameValue["ReLiveCurPosFlag"]=st;
	//禁制复活开关
	st.SetType(6);	st.pbValue=&m_stSetup.bForbiddenFlag;			st.attrEnumValue=0;						m_mapNameValue["ForbiddenFlag"]=st;
}

bool  CServerRegion::SetAttribute(char* attrName, long value)
{
	map<string, tagPty>::iterator itr =  m_mapNameValue.find(attrName);
	if (itr != m_mapNameValue.end())
	{
		switch (itr->second.lType)
		{
		case 0:	// byte
			*(itr->second.plValue) = (BYTE)value;
			break;
		case 1:	// word
			*(itr->second.pwValue) = (WORD)value;
			break;
		case 2:	// dword
			*(itr->second.pdwValue) = (DWORD)value;
			break;
		case 3:	// float
			*(itr->second.pfValue) = (float)value;
			break;
		case 4:	// long
			*(itr->second.pValue) = (long)value;
			break;
		case 5:	// short
			*(itr->second.psValue) = (short)value;
			break;
		case 6:	// bool
			*(itr->second.pbValue) = value==0?false:true;
			break;
		}
	}
	return true;
}
long  CServerRegion::GetAttribute(char* attrName)
{
	long value = INVILID_VALUE;
	map<string, tagPty>::iterator itr =m_mapNameValue.find(attrName);
	if (itr != m_mapNameValue.end())
	{
		switch (itr->second.lType)
		{
		case 0:	// byte
			return *(itr->second.plValue);

		case 1:	// word
			return *(itr->second.pwValue);

		case 2:	// dword
			return *(itr->second.pdwValue);

		case 3:	// float
			return (long)*(itr->second.pfValue);

		case 4:	// long
			return *(itr->second.pValue);

		case 5:	// short
			return *(itr->second.psValue);

		case 6:	// bool
			return *(itr->second.pbValue);
		}
	}
	return value;
}
bool  CServerRegion::ChangeAttribute(char* attrName, long value)
{
	return true;
}
//超时响应
long CServerRegion::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	DWORD dwStartTime = timeGetTime();
	long lTimerType = 200;
	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;

	if(pTimerVar)
		lTimerType =pTimerVar ->lvar1<<16 | pTimerVar->TimerType;

	if (ChildObjType_DeleteRgn == pTimerVar->TimerType)
	{
		OnDeleteRgnTimeout(timerid);
	}
	else if (ChildObjType_ReBornObj== pTimerVar->TimerType)
	{
		OnRebornObject(timerid,( stDestObj*)pTimerVar->pvar);
	}
	else if (ChildObjType_AddObj== pTimerVar->TimerType)
	{
		//stDestObj* pDestObj=(stDestObj*)pTimerVar->pvar;
		//if (pDestObj == NULL)	return 0;
		CShape* pShape= (CShape*)pTimerVar->lvar;
		if (pShape== NULL) return 0;

		DelReboreTimerID(pShape->GetExID());
		pShape->SetPosXY(pShape->GetPosX(),pShape->GetPosY());
		long lType = pShape->GetType();

		// 发送消息给玩家
		//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pShape->GetExID());
		//MsgSend.Add( pShape->GetType() );
		//MsgSend.Add( pShape->GetExID() );
		//DBWriteSet setWriteDB;
		//MsgSend.GetDBWriteSet(setWriteDB);
		//if (pShape->GetType()==TYPE_PLAYER)
		//	((CPlayer*)pShape)->CodeToDataBlock_ForClient( setWriteDB, false );
		//else if (pShape->GetType()==TYPE_GOODS)
		//	((CGoods*)pShape) -> SerializeForOldClient( setWriteDB );
		//else
		//	pShape->CodeToDataBlock( setWriteDB, false );
		//MsgSend.Add( (char)0 );		// 是否出现特效
		//MsgSend.SendToAround(pShape);
		AddShapeToAround(pShape);

	}
	else if ( ChildObjType_DeleteObj== pTimerVar->TimerType)
	{
		if (pTimerVar->pvar==NULL)  return 0;
		stDestObj destobj=*((stDestObj*)pTimerVar->pvar);
		SAFE_DELETE(((tagTimerVar*)pTimerVar)->pvar);
		if (destobj.nDestType== TYPE_MONSTER )
		{
			CMonster* pMonster= FindMonsterByID(destobj.DestID);
			if (pMonster!=NULL)
			{
				DelMonster(pMonster);
			}
		}
		else if (destobj.nDestType== TYPE_COLLECTION)
		{
			CCollection *pCollecton= (CCollection*)FindChildObject(destobj.nDestType,destobj.DestID);
			if (pCollecton!= NULL)
			{
				DelCollection(pCollecton);
			}
		}
		else
		{

		}
	}
	else if (ChildObjType_Weather == pTimerVar->TimerType)
	{
		UnRegisterWeatherTimer();
		OnChangeWeather();
		RegisterWeatherTimer();
	}
	else if(ChildObjType_AddShapeToClient== pTimerVar->TimerType)
	{
		OnAddShapeTimeOut(timerid);
	}
	else
		return 0;

	GetGame()->AddTimerExpendTime(lTimerType,timeGetTime()-dwStartTime);
	return 1;
}
//当定时器被取消的调用
void CServerRegion::OnTimerCancel(DWORD timerid,const void* var)
{
	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;
	//if (ChildObjType_ReBornOcj== pTimerVar->TimerType)
	//{
	//	stDestObj* DestObj= (stDestObj*)pTimerVar->pvar;
	//	SAFE_DELETE(DestObj);
	//}
	CBaseObject::OnTimerCancel(timerid,var);
}

void CServerRegion::AddRebornTimerID(CGUID guid,long lTimerID)
{
	m_mReBornTimerIDList[guid]= lTimerID;
}

void CServerRegion::DelReboreTimerID(CGUID guid)
{
	itGuid2Long it= m_mReBornTimerIDList.find(guid);
	if (it!= m_mReBornTimerIDList.end())
	{
		GameManager::GetInstance()->Cancel((*it).second);
		m_mReBornTimerIDList.erase(it);
	}
}

VariableList* CServerRegion::GetVariableList(void)
{
	return m_pVariableList;
}
void CServerRegion::SetVariableList(VariableList* vl)
{
	if(vl==NULL)
		return;

	//m_pVariableList->SetVarList(vl->GetVarList());
	m_pVariableList->RemoveAll();
	VarList::Copy( vl, m_pVariableList );
}
void CServerRegion::UpdateVariableList(VariableList* vl)
{
	if(vl==NULL)
		return ;

	//m_pVariableList->UpdateVarList(vl->GetVarList());
	VarList::Copy( vl, m_pVariableList );
}
//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////


void CServerRegion::ChangeWeather( long guid )
{
	vector<tagWeatherTime::tagOption::tagWeather> vWeather;
	tagWeatherTime::tagOption::tagWeather twNew;
	twNew.lWeatherIndex	= guid;
	twNew.dwFogColor	= 0x0;
	vWeather.push_back(twNew);
	m_vectorCurWeather = vWeather;
	SendWeatherInfo();
}


void CServerRegion::RegisterWeatherTimer()
{
	if (m_vectorWeatherSetup.size())
	{
		void* pVar =  OBJ_CREATE(tagTimerVar);	
		((tagTimerVar*)pVar)->TimerType = ChildObjType_Weather;
		long lTimerID = GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Schedule(this, pVar, m_vectorWeatherSetup[m_lCurWeatherTimeSegment].lTime*60000 + timeGetTime());
		++m_lCurWeatherTimeSegment;
		m_lRgnDelTimerID = lTimerID;
	}
}

void CServerRegion::OnChangeWeather()
{
	if (m_vectorWeatherSetup.size())
	{
		if (m_lCurWeatherTimeSegment >= (long)m_vectorWeatherSetup.size())
		{
			m_lCurWeatherTimeSegment = 0;
		}
		//改变天气
		tagWeatherTime* pst = &m_vectorWeatherSetup[m_lCurWeatherTimeSegment];
		if ((long)pst->vectorOption.size()==0)
		{
			m_vectorCurWeather.clear();
			SendWeatherInfo();
		}
		else
		{
			long lOdds = random(100);
			for (int i=0; i<(long)pst->vectorOption.size(); i++)
			{
				if (lOdds < pst->vectorOption[i].lOdds)
				{
					m_vectorCurWeather = pst->vectorOption[i].vectorWeather;
					SendWeatherInfo();
					break;
				}
			}
		}
	}

}

void CServerRegion::UnRegisterWeatherTimer()
{
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Cancel(m_lWeatherTimerID, (const void**)&pVar);
	m_lWeatherTimerID = -1;
}


void CServerRegion::ClearPlayerAI()
{
	if(m_bKickOutPlayer == true)
	{
		DWORD dwCurTime = timeGetTime();
		DWORD dwTime = dwCurTime - m_dwLastTime;
		if(m_lKickOutPlayerTime <= (long)dwTime)
		{
			m_bKickOutPlayer = false;
			m_lKickOutPlayerTime = 0;
			//剔出该场景的所有玩家
			KickOutAllPlayerToReturnPoint();
		}
		else
		{
			//10秒以上用5秒的间隔
			if(m_lKickOutPlayerTime > 10000)
			{
				if(dwTime > 5000)
				{
					m_lKickOutPlayerTime -= dwTime;
					m_dwLastTime = dwCurTime;

					char strSend[256]="";
					_snprintf(strSend, 256, "你将被移除该场景.剩余时间%d秒.",m_lKickOutPlayerTime/1000);
					CMessage msg( MSG_S2C_OTHER_ADDSYSTEMINFO );
					msg.Add( static_cast<DWORD>(0xFFFFFFFF) );
					msg.Add( strSend );
					msg.SendToRegion(this);
				}
			}
			else
			{
				if(dwTime > 1000)
				{
					m_lKickOutPlayerTime -= dwTime;
					m_dwLastTime = dwCurTime;

					char strSend[256]="";
					_snprintf(strSend, 256, "你将被移除该场景.剩余时间%d秒.",m_lKickOutPlayerTime/1000);
					CMessage msg( MSG_S2C_OTHER_ADDSYSTEMINFO );
					msg.Add( static_cast<DWORD>(0xFFFFFFFF) );
					msg.Add( strSend );
					msg.SendToRegion(this);
				}
			}
		}
	}
}

void	CServerRegion::StartClearPlayerOut(long lCountDownTime)
{
	m_bKickOutPlayer = true;
	m_lKickOutPlayerTime = lCountDownTime;
	m_dwLastTime = timeGetTime();
}

// 处理shape改变area
void CServerRegion::OnShapeChangeArea(CShape* pShape,long lNextAreaX,long lNextAreaY)
{
	if(pShape == NULL)	return;
	if(pShape->GetArea() == NULL ) return;
	if(pShape->GetArea()->GetX() == lNextAreaX &&
		pShape->GetArea()->GetY() == lNextAreaY ) return;

	long nx = lNextAreaX;
	long ny = lNextAreaY;
	list<CArea*>	listOldArea, listNewArea;
	long lType = pShape->GetType();

	// 计算出原来和现在的9个区域
	for(int i=0;i<9;i++)
	{
		CArea* pArea = GetArea(pShape->GetArea()->GetX() + _area[i][0], pShape->GetArea()->GetY() + _area[i][1]);
		if( pArea )
		{
			listOldArea.push_back(pArea);
		}

		pArea = GetArea(nx + _area[i][0], ny + _area[i][1]);
		if( pArea )
		{
			listNewArea.push_back(pArea);			
		}
	}

	// 新区域 --------------------------------------------------------------------------------
	list<CArea*>	listSendNew = listNewArea;
	list<CArea*>::iterator it=listOldArea.begin();
	for(; it!=listOldArea.end(); it++)
	{
		listSendNew.remove(*it);
	}

	// 发送消息给新区域的玩家

	//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pShape->GetExID());
	//MsgSend.Add( pShape->GetType() );
	//MsgSend.Add( pShape->GetExID() );
	//DBWriteSet setWriteDB;
	//MsgSend.GetDBWriteSet(setWriteDB);
	//if (pShape->GetType()==TYPE_PLAYER)
	//	((CPlayer*)pShape)->CodeToDataBlock_ForClient( setWriteDB, false );
	//else if (pShape->GetType()==TYPE_GOODS)
	//	((CGoods*)pShape) -> SerializeForOldClient( setWriteDB );
	//else
	//	pShape->CodeToDataBlock( setWriteDB, false );
	//MsgSend.Add( (char)0 );		// 是否出现特效



	for(it=listSendNew.begin(); it!=listSendNew.end(); it++)
	{
		CArea* pArea = *it;
		if( pArea->GetNumShapes() )
		{
			// 返回信息给Area上存在的玩家
			//MsgSend.SendToArea(*it, (CPlayer*)pShape);
			AddShapeToArea(*it,pShape);

			// 把新区域的SHAPE发送给玩家
			if( pShape->GetType() == TYPE_PLAYER )
			{
				//itBaseObject it;
				//for(it=pArea->GetObjectList()->begin(); it!=pArea->GetObjectList()->end(); it++)
				vector<CShape*> vShapes;
				pArea -> GetAllShapes( vShapes );
				for( size_t i = 0; i < vShapes.size(); i ++ )
				{
					CShape* pTemptShape = dynamic_cast<CShape*>( vShapes[i] );
					if( pTemptShape )
					{
						if( pTemptShape->GetType() == TYPE_PLAYER && pTemptShape == pShape )
						{
						}
						else
						{
							//CMessage Msg(MSG_S2C_RGN_ADDSHAPE,pTemptShape->GetExID());
							//Msg.Add( pTemptShape->GetType() );
							//Msg.Add( pTemptShape->GetExID() );
							//DBWriteSet setWriteDB;
							//Msg.GetDBWriteSet(setWriteDB);
							//if (pTemptShape->GetType()==TYPE_PLAYER)
							//	((CPlayer*)pTemptShape)->CodeToDataBlock_ForClient( setWriteDB, false );
							//else if (pTemptShape->GetType()==TYPE_GOODS)
							//	((CGoods*)pTemptShape) -> SerializeForOldClient( setWriteDB );
							//else
							//	pTemptShape->CodeToDataBlock( setWriteDB, false );
							//Msg.Add( (char)false );		// 是否出现特效
							//Msg.SendToPlayer( ((CPlayer*)pShape)->GetExID() ,false);

							AddShapeToPlayer(pTemptShape,pShape->GetExID());
						}
					}
				}
			}
		}
	}

	// 老区域 --------------------------------------------------------------------------------
	listSendNew = listOldArea;
	it=listNewArea.begin();
	for(; it!=listNewArea.end(); it++)
	{
		listSendNew.remove(*it);
	}
	//从老区域中删除该对象
	for(it=listSendNew.begin(); it!=listSendNew.end(); it++)
	{
		//CMessage msg( MSG_S2C_RGN_DELSHAPE );
		//msg.Add( pShape->GetType() );
		//msg.Add( pShape->GetExID() );
		//msg.Add( (long)0 );
		CArea* pArea = *it;
		if( pArea->GetNumShapes() )
		{
			// 返回信息给Area上存在的玩家
			//msg.SendToArea(*it, (CPlayer*)pShape);
			DelShapeToArea(*it, pShape);

			if( pShape->GetType() == TYPE_PLAYER )
			{
				vector<CShape*> vShapes;
				pArea -> GetAllShapes( vShapes );
				for( size_t i = 0; i < vShapes.size(); i ++ )
				{
					CShape* pTemptShape = dynamic_cast<CShape*>( vShapes[i] );
					if( pTemptShape )
					{
						if( pTemptShape->GetType() == TYPE_PLAYER && pTemptShape == pShape ||
							pTemptShape->GetType() == TYPE_PET && ((CMoveShape*)pTemptShape)->GetHost() == pShape )
						{
						}
						else
						{
							/*CMessage Delmsg( MSG_S2C_RGN_DELSHAPE );
							Delmsg.Add( pTemptShape->GetType() );
							Delmsg.Add( pTemptShape->GetExID() );
							Delmsg.Add( (long)0 );
							Delmsg.SendToPlayer( ((CPlayer*)pShape)->GetExID() );*/
							DelShapeToPlayer(pTemptShape,pShape->GetExID());
						}
					}
				}
			}
		}
	}

	CArea* oldArea = pShape->GetArea();
	oldArea->RemoveObject(pShape);
	CArea* pArea = GetArea(nx, ny);
	if( pArea )
	{
		pArea->AddObject(pShape);
		pShape->SetArea(pArea);
	}
}

// 读取
BOOL CServerRegion::Load()
{
	if( !CRegion::Load() )
	{
		return false;
	}

	CreateAreaArray();
	return true;
}

// 存储
BOOL CServerRegion::Save()
{
	return CRegion::Save();
}

// 新建
BOOL CServerRegion::New()
{
	return CRegion::New();
}
bool CServerRegion::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	/*CRegion::CodeToDataBlock(setWriteDB);

	setWriteDB.AddToByteArray((BYTE)m_WarRegionType);
	setWriteDB.AddToByteArray((BYTE)m_eSpaceType);
	setWriteDB.AddToByteArray((BYTE)m_Ride);
	setWriteDB.AddToByteArray(m_dwAreaId);
	setWriteDB.AddToByteArray((char)m_bNoPk);
	setWriteDB.AddToByteArray((char)m_bNoContribute);*/

	return true;
}
// 添加到CByteArray
bool CServerRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CRegion::AddToByteArray(pByteArray, bExData);
	_AddToByteArray(pByteArray,(BYTE)m_WarRegionType);
	_AddToByteArray(pByteArray,(BYTE)m_eSpaceType);
	_AddToByteArray(pByteArray,(BYTE)m_Ride);
	_AddToByteArray(pByteArray,(BYTE)m_ChangeEquip);
	_AddToByteArray(pByteArray,m_dwAreaId);
	_AddToByteArray(pByteArray,(BYTE)m_bNoPk);
	_AddToByteArray(pByteArray,(char)m_bNoContribute);
	_AddToByteArray(pByteArray,(BYTE)m_bRelive);
	_AddToByteArray(pByteArray,(BYTE)m_bDeadReturn);
	return true;
}

bool CServerRegion::AddSetupToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	_AddToByteArray(pByteArray,&m_stSetup,sizeof(tagRegionSetup));

	//添加禁止使用的物品
	long lSize = (long)m_ForbidMakeGoods.size();
	_AddToByteArray(pByteArray,lSize);
	for(set<string>::iterator it= m_ForbidMakeGoods.begin() ;it != m_ForbidMakeGoods.end();it++)
	{
		_AddToByteArray(pByteArray,(*it).c_str());
	}

	return true;
	//////////////////////////////
	//char strTemp[256];
	////_GetBufferFromByteArray(pByte,pos,&m_stSetup,sizeof(tagRegionSetup));

	////得到禁止使用的物品
	////m_ForbidMakeGoods.clear();
	////long lForbidGoodsNum = _GetLongFromByteArray(pByte,pos);
	////m_ForbidMakeGoods.clear();
	//for(int i=0;i < lForbidGoodsNum;i++)
	//{
	//	_GetStringFromByteArray(pByte, pos, strTemp);
	//	m_ForbidMakeGoods.insert(strTemp);
	//}

	//return true;
}
// 解码
bool CServerRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	if(!pByte) return false;

	CRegion::DecordFromByteArray(pByte, pos, bExData);	
	m_ID = m_lID;
	m_WarRegionType = (eRWT)_GetByteFromByteArray(pByte, pos);
	m_eSpaceType = (eRST)_GetByteFromByteArray(pByte, pos);
	m_dwAreaId=_GetDwordFromByteArray(pByte,pos);
	char ch = _GetCharFromByteArray(pByte, pos);
	m_bNoPk = ch;
	ch = _GetCharFromByteArray(pByte, pos);
	m_bNoContribute = ch==0?false:true;	
	ch=_GetByteFromByteArray(pByte,pos);
	m_Ride= (ch==0?false:true);
	ch=_GetByteFromByteArray(pByte,pos);
	m_ChangeEquip= (ch==0?false:true);
	ch = _GetByteFromByteArray(pByte,pos);
	m_bRelive = (ch==0?false:true);
	ch = _GetByteFromByteArray(pByte,pos);
	m_bDeadReturn = (ch==0?false:true);


	CreateAreaArray();
	char strTemp[256];

	//路径
	m_vecRoadsList.clear();

	LONG lRoadsNum=_GetLongFromByteArray(pByte, pos);
	LONG lPointNumOfRoad=0;
	POINT Point;
	vector<POINT> road;
	for (int a=0; a<lRoadsNum; a++)
	{
		road.clear();

		lPointNumOfRoad=_GetLongFromByteArray(pByte, pos);
		for (int b=0; b< lPointNumOfRoad; b++)
		{
			Point.x=_GetLongFromByteArray(pByte, pos);
			Point.y=_GetLongFromByteArray(pByte, pos);
			road.push_back(Point);
		}
		m_vecRoadsList.push_back(road);
	}

	//monster 列表
	m_mpMonster.clear();
	long lMonsterNum = _GetLongFromByteArray(pByte, pos);
	tagMonster st;
	for(int i=0; i<lMonsterNum; i++)
	{
		st.vectorMonsterList.clear();
		_GetBufferFromByteArray(pByte, pos, &st, sizeof(tagMonster)-sizeof(vector<tagMonsterList>)-sizeof(long)*2);
		long lNum = _GetLongFromByteArray(pByte, pos);
		for (int j=0; j<lNum; j++)
		{
			tagMonsterList stMonsterList;
			_GetBufferFromByteArray(pByte, pos, &stMonsterList, sizeof(tagMonsterList)-sizeof(string)*3);
			stMonsterList.strName = _GetStringFromByteArray(pByte, pos, strTemp);
			stMonsterList.strScript = _GetStringFromByteArray(pByte, pos, strTemp);
			stMonsterList.strTalkScript=_GetStringFromByteArray(pByte, pos, strTemp);
			st.vectorMonsterList.push_back(stMonsterList);
		}

		// 添加进场景
		st.lLivingCount = 0;
		st.dwLastResetTime = timeGetTime() - st.lStartTime;

		m_mpMonster[st.lIndex] = st;

		AddMonsterRect(st, st.lNum);
	}

	// 统计NPC名字列表
	m_lNpcNameListNum = 0;
	hash_map<CGUID, CMonster*>::iterator itMonster = m_mMonsters.begin();
	for( ; itMonster != m_mMonsters.end(); itMonster ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(itMonster -> second);
		if (pMonster && pMonster->GetNpcType()== NT_Normal )//&& pMonster->GetAI()!=NULL && pMonster->GetAI()->)
		{
			//CMonsterAI* pAI=dynamic_cast<CMonsterAI*>(pMonster->GetAI());
			//if (pAI==NULL || pAI->HasRoad())	continue;	
			string name;
			if (strcmp(pMonster->GetTitleName(),"0")!=0)
			{
				name+=pMonster->GetTitleName();
				name+=" ";
			}

			name+= pMonster->GetName();

			_AddToByteArray(&m_vectorNpcNameList, (char*)name.c_str());
			_AddToByteArray(&m_vectorNpcNameList, pMonster->GetTileX());
			_AddToByteArray(&m_vectorNpcNameList, pMonster->GetTileY());
			m_lNpcNameListNum ++;
		}
	}

	//Collection 列表
	m_listCollection.clear();
	long lCollectNum = _GetLongFromByteArray(pByte, pos);
	tagCollection cg;
	for (int a=0; a<lCollectNum; a++)
	{
		cg.vectorCollectionList.clear();
		_GetBufferFromByteArray(pByte,pos,&cg,sizeof(tagCollection)-sizeof(vector<tagCollectionList>));
		long lNum=_GetLongFromByteArray(pByte,pos);
		for (int i=0; i<lNum; i++)
		{
			tagCollectionList cl;
			_GetBufferFromByteArray(pByte,pos,&cl,sizeof(tagCollectionList));
			cg.vectorCollectionList.push_back(cl);
		}

		m_listCollection.push_back(cg);

		AddCollectionRect(cg,cg.lNum);
	}

	//门
	m_mapGate.clear();
	long lGatenum=_GetLongFromByteArray(pByte,pos);
	for(int i=0;i<lGatenum;i++)
	{	
		tagGateProperty gate;
		_GetBufferFromByteArray(pByte,pos,&gate,sizeof(tagGateProperty)-sizeof(string)*6);
		char str[1024];
		_GetStringFromByteArray(pByte, pos, str);
		gate.strOrigName=str;
		_GetStringFromByteArray(pByte, pos, str);
		gate.strAttack=str;
		_GetStringFromByteArray(pByte, pos, str);
		gate.strOpen=str;
		_GetStringFromByteArray(pByte, pos, str);
		gate.strClose=str;
		_GetStringFromByteArray(pByte, pos, str);
		gate.strDestory=str;
		_GetStringFromByteArray(pByte, pos, str);
		gate.strRefresh=str;
		m_mapGate[gate.lIndex]=gate;
		AddGate(&gate);
	}

	// 天气
	m_vectorWeatherSetup.clear();
	long lWeatherTimerNum = _GetLongFromByteArray(pByte, pos);
	for (int i=0; i<lWeatherTimerNum; i++)
	{
		tagWeatherTime st;
		st.lTime = _GetLongFromByteArray(pByte, pos);
		long lOptionNum = _GetLongFromByteArray(pByte, pos);
		for (int j=0; j<lOptionNum; j++)
		{
			tagWeatherTime::tagOption stOption;
			stOption.lOdds = _GetLongFromByteArray(pByte, pos);
			long lWeatherNum = _GetLongFromByteArray(pByte, pos);
			for (int k=0; k<lWeatherNum; k++)
			{
				tagWeatherTime::tagOption::tagWeather stWeather;
				_GetBufferFromByteArray(pByte, pos, &stWeather, sizeof(tagWeatherTime::tagOption::tagWeather));
				stOption.vectorWeather.push_back(stWeather);
			}
			st.vectorOption.push_back(stOption);
		}
		m_vectorWeatherSetup.push_back(st);

		m_lCurWeatherCount = m_vectorWeatherSetup[0].lTime * 60 - 1;
	}

	DecordSetupFromByteArray(pByte,pos,bExData);


	//税收参数
	_GetBufferFromByteArray(pByte, pos, &m_Param, sizeof(tagRegionParam));

	//// 变量列表
	////m_pVariableList->DecordFromByteArray(pByte, pos);
	//char strName[256];
	//long lVarNum;
	//long lDataSize;
	//long lArray;

	////LoadVarList("data/variable.ini", "VariableList", GetGame()->GetVariableList_FileData());

	////普通变量个数
	//lVarNum = _GetLongFromByteArray(pByte,pos);  
	////GUID个数
	//int num = _GetLongFromByteArray( pByte, pos );		// 变量个数

	//lDataSize=_GetLongFromByteArray(pByte,pos);

	//double value = 0;

	//for(int i=0; i<lVarNum; i++)
	//{
	//	_GetStringFromByteArray(pByte, pos, strName);
	//	lArray = _GetLongFromByteArray(pByte,pos);
	//	if( lArray == 0 )	// 个体变量
	//	{
	//		//SetVarValue(strName, 0, *(long*)(&pByte[pos]) );
	//		value = _GetDoubleFromByteArray(pByte,pos);
	//		GetVariableList()->AddVar(strName, value);
	//		//pos+=4;
	//	}
	//	else if(lArray > 0)	// 数组
	//	{
	//		GetVariableList()->AddVar(strName, lArray, 0.0f);

	//		for(int j=0; j<lArray; j++)
	//		{
	//			value = _GetDoubleFromByteArray(pByte,pos);
	//			GetVariableList()->SetVarValue(strName, j, value);
	//			//AddVar(strName, j, *(long*)(&pByte[pos]) );
	//			//pos+=4;
	//		}
	//	}
	//	else
	//	{		//字符串
	//		//SetVarValue(strName,(char*)(&pByte[pos]) );
	//		char strValue[1024];
	//		_GetStringFromByteArray(pByte, pos, strValue);
	//		GetVariableList()->AddVar(strName, strValue);
	//	}
	//}

	//// GUID
	//char GuidName[256];
	//CGUID tGuid;
	//for(int i=0; i<num; i++)
	//{
	//	_GetStringFromByteArray( pByte, pos, GuidName );	// 名字	
	//	_GetBufferFromByteArray(pByte, pos, tGuid);	// GUID变量
	//}

	VarList::Decode( m_pVariableList, pByte, (size_t) -1, pos );
	// 解码据点信息
	m_bStrongPintFlag = _GetByteFromByteArray(pByte, pos) ? true : false ;
	long lSPnum = _GetLongFromByteArray(pByte, pos);
	tagStrongPoint tmpSP;
	for(int i=0; i<lSPnum; i++)
	{
		_GetBufferFromByteArray(pByte, pos, &tmpSP, sizeof(tagStrongPoint));
		m_vecStrongPoint.push_back(tmpSP);
	}

	return true;
}

// 解码Setup
bool CServerRegion::DecordSetupFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	char strTemp[256];
	_GetBufferFromByteArray(pByte,pos,&m_stSetup,sizeof(tagRegionSetup));

	//得到禁止使用的物品
	m_ForbidMakeGoods.clear();
	long lForbidGoodsNum = _GetLongFromByteArray(pByte,pos);
	m_ForbidMakeGoods.clear();
	for(int i=0;i < lForbidGoodsNum;i++)
	{
		_GetStringFromByteArray(pByte, pos, strTemp);
		m_ForbidMakeGoods.insert(strTemp);
	}

	return true;
}

//添加参数到ByteArray
bool CServerRegion::AddRegionParamToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	////添加场景相关参数
	_AddToByteArray(pByteArray, (BYTE*)&m_Param, sizeof(tagRegionParam));
	return true;
}

BOOL CServerRegion::DoesRecallWhenLost(CPlayer* pPlayer)
{
	//	return m_stSetup.bDoesRecallWhenLost;
	return TRUE;
}

// 添加Monster
BOOL CServerRegion::AddMonsterRect(tagMonster& st, long lMonsterNum, bool bGMCreate)
{
	tagMonster* pst = &st;//GetMonsterRefeash(st.lIndex);

	// 创建
	for(int n=0; n<lMonsterNum; n++)
	{
		// 获取刷新的怪物
		tagMonsterList* pstMonster = NULL;
		int lOdds = random(100);
		int lNum = (int)pst->vectorMonsterList.size();
		for (int i=0; i<lNum; i++)
		{
			if (lOdds<pst->vectorMonsterList[i].wOdds)
			{
				pstMonster = &pst->vectorMonsterList[i];
				break;
			}
		}

		if (pstMonster==NULL)
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_0",st.lIndex));//编号=%d的怪物没有机会出生
			continue;
		}

		// 寻找刷新点
		long x, y;
		bool bRet = GetRandomPosInRange(x, y, pst->rectRange.left, pst->rectRange.top, pst->rectRange.right-pst->rectRange.left, pst->rectRange.bottom-pst->rectRange.top);
		if( bRet == false )
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_1",pstMonster->strName.c_str()));//无法为Monster “%s” 找到合适的位置！
		}

		CMonster *pMonster = AddMonster(pstMonster->strName.c_str(), x, y, pst->lDir, bGMCreate);



		if (pMonster)
		{
			if (pstMonster->wRoadIndex>0  )
			{
				int road_index = pstMonster->wRoadIndex - 1; // 配置中以1为基准
				if( road_index < 0 || road_index >= (long)m_vecRoadsList.size())
				{
					Log4c::Trace(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_4",pstMonster->strName.c_str()));
				}
				else
				{
					//CMonsterAI* ai=dynamic_cast<CMonsterAI*>(pMonster->GetAI());////加入路径
					//if (ai!= NULL ) 
					//{
					//	ai->AddRoad(m_vecRoadsList[road_index]);
					//}
					StateAI::MonsterAI *ai = dynamic_cast<StateAI::MonsterAI*>( pMonster->GetAI() );
					ai->GetRunByRoad().SetRoad( m_vecRoadsList[road_index] );
				}
			}
			pMonster->SetRefeashIndex(pst->lIndex);
			pst->lLivingCount++;


			if (!pstMonster->strScript.empty() && pstMonster->strScript != "0")
				pMonster->SetScriptFile(pstMonster->strScript.c_str());
			if (!pstMonster->strTalkScript.empty() && pstMonster->strTalkScript != "0")
			{
				pMonster->SetTalkScriptFile(pstMonster->strTalkScript.c_str());
			}


		}
		if (pMonster!= NULL && pstMonster->wSign!=0)
		{
			pMonster->SetColonyDistance(pstMonster->wLeaderDistance);
			CColony* pColony= GetOneColony(pstMonster->wSign);
			if (pColony==0)return true;
			WORD AddRsl= pColony->AddMember(pstMonster->wLeaderSign==1,pMonster);
			if (AddRsl== -1) return true;
			pMonster->SetCollectionID(AddRsl);

		}
	}

	return true;
}

// 在一个区域内添加多个怪物，不刷新，GM创建。
BOOL CServerRegion::AddMonster(const char* strOrginName, long count, long left, long top, long right, long bottom)
{
	for(int n=0; n<count; n++)
	{
		// 寻找刷新点
		long x, y;
		bool bRet = GetRandomPosInRange(x, y, left, top, right-left, bottom-top);
		if( bRet == false )
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_1",strOrginName));//编号=%d的怪物没有机会出生
		}

		CMonster *pMonster = AddMonster(strOrginName, x, y, -1, true);
		//if (pMonster)
		//{
		//	//pMonster->GetProperty()->wSign = pstMonster->wSign;
		//	//pMonster->GetProperty()->wLeaderSign = pstMonster->wLeaderSign;
		//	//pMonster->GetProperty()->wLeaderDistance = pstMonster->wLeaderDistance;
		//	//pMonster->GetProperty()->lRefeashIndex = pst->lIndex;
		//	//pMonster->SetScriptFile(pstMonster->strScript.c_str());
		//}
		//else 
		return false;
	}
	return true;
}

CMonster* CServerRegion::AddMonster(long lGID, long x, long y, long lDir, bool bGMCreate)
{
	CMonster* pMonster = NULL;
	CMonsterList::tagMonster* p = CMonsterList::GetProperty(lGID);
	if( p )
	{

		pMonster=GetOneMonster();
		if( pMonster )
		{
			pMonster -> SetName( p ->cName );//strName.c_str() );
			pMonster -> SetGraphicsID( p -> dwPicID );

			//..........monster 属性初始化  
			//基本属性 
			pMonster->Init();
			pMonster->InitProperty(p);
			if(pMonster->GetNpcType()==NT_Normal && pMonster->GetCountry()==8)
			{
				pMonster->SetCountry(GetCountry());
			}
			//...........................

			pMonster->SetPosXY( x+0.5f, y+0.5f );

			StateAI::MonsterAI *monsterAI = static_cast<StateAI::MonsterAI*>( pMonster->GetAI() );
			monsterAI->SetBornPos( x, y );
			//CMonsterAI* pMonsterAI= dynamic_cast<CMonsterAI*>(pMonster->GetAI());//pMonster->
			//if (pMonsterAI!= NULL)
			//{
			//	pMonsterAI->RecordBornPoint(x,y);
			//}


			//##这个守卫列表不知道用来做啥，到时候看能不能精简
			//if( p ->wAIModel  == CAIFactory::CITY_GUARD_WITH_SWORD ||
			//	p ->wAIModel  == CAIFactory::CITY_GUARD_WITH_BOW )
			//{
			//	if( bGMCreate == false )
			//	{
			//		AddGurdMonster( pMonster->GetExID());
			//	}
			//}

			if( lDir <0 || lDir > 7 )
			{
				pMonster->SetDir( random(8) );
			}
			else
			{
				pMonster->SetDir(lDir);
			}

			//##最后添加到怪物哈西表中
			AddObject( pMonster );


			// 发送消息给玩家
			//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pMonster->GetExID());
			//MsgSend.Add( pMonster->GetType() );
			//MsgSend.Add( pMonster->GetExID());
			//DBWriteSet setWriteDB;
			//MsgSend.GetDBWriteSet(setWriteDB);
			//pMonster->CodeToDataBlock(setWriteDB,false);
			//MsgSend.Add( (char)0 );		// 是否出现特效
			//MsgSend.SendToAround(pMonster);

			AddShapeToAround(pMonster);

			g_lTotalMonster++;
		}
	}

	return pMonster;
}

// 添加一个Monster
CMonster* CServerRegion::AddMonster(const char* strOrginName, long x, long y, long lDir, bool bGMCreate)
{
	CMonster* pMonster = NULL;
	CMonsterList::tagMonster* p = CMonsterList::GetPropertyByOrginName(strOrginName);
	if( p )
	{
		pMonster = GetOneMonster();
		if( pMonster )
		{
			pMonster -> SetName( p ->cName );
			pMonster -> SetGraphicsID( p -> dwPicID );

			//..........monster 属性初始化  
			pMonster->InitProperty(p);
			if( ( pMonster->GetNpcType()==NT_Normal || pMonster->GetNpcType() == NT_Guard ) && pMonster->GetCountry()==8 )
			{
				pMonster->SetCountry(GetCountry());
			}
			//...........................
			////			//一些必须在获取基本属性后要作得初始化工作
			pMonster->Init();
			StateAI::MonsterAI *monsterAI = static_cast<StateAI::MonsterAI*>( pMonster->GetAI() );
			monsterAI->SetBornPos( x, y );
			//CMonsterAI* pMOnsterAI= static_cast<CMonsterAI*>(pMonster->GetAI());//pMonster->
			//if (pMOnsterAI!= NULL)
			//{
			//	pMOnsterAI->RecordBornPoint(x,y);
			//}

			//if(	p ->wAIModel  == CAIFactory::CITY_GUARD_WITH_SWORD ||
			//	p ->wAIModel  == CAIFactory::CITY_GUARD_WITH_BOW )
			//{
			//	if( bGMCreate == false )
			//	{
			//		AddGurdMonster( pMonster->GetExID());
			//	}
			//}

			if( lDir <0 || lDir > 7 )
			{
				pMonster->SetDir( random(8) );
			}
			else
			{
				pMonster->SetDir(lDir);
			}

			pMonster->SetFather( this );
			pMonster->SetPosXY( x+0.5f, y+0.5f );
			AddObject( pMonster );

			// 发送消息给玩家
			//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pMonster->GetExID());
			//MsgSend.Add( pMonster->GetType() );
			//MsgSend.Add( pMonster->GetExID() );
			//DBWriteSet setWriteDB;
			//MsgSend.GetDBWriteSet(setWriteDB);
			//pMonster->CodeToDataBlock(setWriteDB,false);
			//MsgSend.Add( (char)0 );		// 是否出现特效
			//MsgSend.SendToAround(pMonster);
			AddShapeToAround(pMonster);

			g_lTotalMonster++;
		}
	}

	return pMonster;
}
CGate*	CServerRegion::AddGate(tagGateProperty* pGateProperty)
{
	CGate *pGate=NULL;
	if(!pGateProperty)return NULL;
	GateList::tagGate *p=GateList::GetPropertyByOrigName(pGateProperty->strOrigName.c_str());
	if(p)
	{
		pGate=GetOneGate();
		if(!pGate)return NULL;

		pGate->InitSetProperty(pGateProperty);
		pGate->InitProperty(p);	

		AddObject(pGate);
		pGate->SetGate(pGate->GetState());
		pGate->UpdateAttributeToClient();
		//向客户端发送消息
		//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pGate->GetExID());
		//MsgSend.Add( pGate->GetType() );
		//MsgSend.Add( pGate->GetExID() );
		//DBWriteSet setWriteDB;
		//MsgSend.GetDBWriteSet(setWriteDB);
		//pGate->CodeToDataBlock(setWriteDB,false);
		//MsgSend.Add( (char)0 );		// 是否出现特效
		//MsgSend.SendToAround(pGate);

		AddShapeToAround(pGate);

	}
	return pGate;
}

// 获取刷新怪物的结构指针
CServerRegion::tagMonster* CServerRegion::GetMonsterRefeash(long lIndex)
{
	map<DWORD,tagMonster>::iterator it = m_mpMonster.find (lIndex);
	if (it != m_mpMonster.end ())
	{
		return & ((*it).second);
	}

	return NULL;
}

CMonster* CServerRegion::GetOneMonster()
{
	CMonster* pMonster= OBJ_CREATE(CMonster);
	if(pMonster)
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		pMonster->SetExID(guid);
	}
	return pMonster;
}

CGate* CServerRegion::GetOneGate()
{
	CGate* pGate=  OBJ_CREATE(CGate);
	if(pGate)
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		pGate->SetExID(guid);
	}
	return pGate;
}
void CServerRegion::DelMonster(CMonster* pMonster)
{
	if (pMonster== NULL) return;

	pMonster->CleanDelEvent();
	pMonster->CleanBeneficiary();
	pMonster->SetState(CShape::STATE_DELET);
	DelGurdMonster( pMonster->GetExID() );
	int iFreshIndex=pMonster->GetRefeashIndex();
	CServerRegion::tagMonster* pRefresh=GetMonsterRefeash( iFreshIndex);


	//CMonsterAI *pAI = static_cast<CMonsterAI*>( pMonster->GetAI() );
	//if( pAI != NULL )
	//{
	//	pAI->Reset();
	//}

	if (pRefresh!= NULL)
	{
		OnRemoveObject(pMonster,pRefresh->lResetTime);
	}
	else
	{
		OnDeleteObject(pMonster);
	}
}

//! 删除原始名为pMonsterName的怪物，返回删除数量
long CServerRegion::DelMonster(const char *pMonsterName)
{
	if(NULL == pMonsterName)
		return 0;
	set<CMonster*> setMonster;
	//! DelMonster(CMonster* pMonster)接口会修改m_mMonsters的结构，因此，需要吧要处理的对象先缓存
	for (hash_map<CGUID, CMonster*>::iterator iteMonster = m_mMonsters.begin(); m_mMonsters.end() != iteMonster; ++iteMonster)
	{
		if(0 == stricmp(pMonsterName, iteMonster->second->GetOriginName()))
			setMonster.insert(iteMonster->second);
	}

	long lRe = (long)setMonster.size();
	for (set<CMonster*>::iterator iteMonster = setMonster.begin(); setMonster.end() != iteMonster; ++iteMonster)
	{
		DelMonster(*iteMonster);
	}
	return lRe;
}

void CServerRegion::DelCollection(CCollection* pCollection)
{
	pCollection->ChangeStateTo(CShape::STATE_DIED);
	pCollection->SetBlock(pCollection->GetTileX(),pCollection->GetTileY(),CRegion::BLOCK_NO);

	CServerRegion::tagCollection* pTc=GetCollectionRefeash(pCollection->GetRefreshIndex());
	if (pTc!=NULL)
	{
		OnRemoveObject(pCollection,pTc->lResetTime);
		pCollection->CleanDelEvent();
	}
	else
	{
		OnDeleteObject(pCollection);
	}
	return ;
}

BOOL CServerRegion::AddOneMonster(CMonster*pMonster)
{
	if (pMonster==NULL )
	{
		return FALSE;
	}

	int iFreshIndex=pMonster->GetRefeashIndex();
	CServerRegion::tagMonster* pRefresh=GetMonsterRefeash( iFreshIndex);

	if (pRefresh==NULL)
	{
		GameManager::GetInstance()->FreeObject((CBaseObject**)&pMonster);
		return FALSE;
	}

	long x, y;
	bool bRet = GetRandomPosInRange(x, y, pRefresh->rectRange.left, pRefresh->rectRange.top, pRefresh->rectRange.right-pRefresh->rectRange.left, pRefresh->rectRange.bottom-pRefresh->rectRange.top);
	//
	pMonster->ResumeAll();
	pMonster->SetPosXY( x+0.5f, y+0.5f );
	pMonster->SetDir( random(8) );

	//CMonsterAI* pMonsterAI= dynamic_cast<CMonsterAI*>(pMonster->GetAI());//pMonster->
	//if (pMonsterAI!= NULL)
	//{
	//	pMonsterAI->RecordBornPoint(x,y);
	//	pMonsterAI->StartActiveAct();
	//}

	//if(	pMonster ->GetAIType()  == CAIFactory::CITY_GUARD_WITH_SWORD ||
	//	pMonster ->GetAIType()  == CAIFactory::CITY_GUARD_WITH_BOW )
	//{
	//	AddGurdMonster( pMonster->GetExID());
	//}

	AddObject( pMonster );

	// 发送消息给玩家
	//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pMonster->GetExID());
	//MsgSend.Add( pMonster->GetType() );
	//MsgSend.Add( pMonster->GetExID() );
	//DBWriteSet setWriteDB;
	//MsgSend.GetDBWriteSet(setWriteDB);
	//pMonster->CodeToDataBlock(setWriteDB,false);
	//MsgSend.Add( (char)0 );		// 是否出现特效
	//MsgSend.SendToAround(pMonster);
	AddShapeToAround(pMonster);

	g_lTotalMonster++;
	return TRUE;
}
//Colony
CColony* CServerRegion::GetOneColony(DWORD dwColonyIndex)
{
	if (dwColonyIndex==0)	return NULL;

	map<DWORD,CColony*>::iterator it= m_mapColony.find(dwColonyIndex);
	if (it!= m_mapColony.end())
	{
		return it->second;
	}
	else
	{
		CColony* pColony= OBJ_CREATE(CColony);
		m_mapColony[dwColonyIndex]=pColony;
		pColony->SetID(dwColonyIndex);
		pColony->SetFather(this);
		return  pColony;
	}
}
void	CServerRegion::ReleaseAllColony()
{
	map<DWORD,CColony*>::iterator it= m_mapColony.begin();
	while (it!= m_mapColony.end())
	{
		OBJ_RELEASE(CColony, it->second);
		it=m_mapColony.erase(it);
	}
}
//Collection
BOOL CServerRegion::AddCollectionRect(tagCollection& st, long lNum)// 添加一个刷新区域的Collection
{
	tagCollection* pst = &st;
	if (NULL==pst)
	{
		return FALSE;
	}
	for(int n=0; n<lNum; n++)
	{
		// 获取刷新的怪物
		tagCollectionList* pstCollection = NULL;
		int lOdds = random(100);
		int lNum = (int)pst->vectorCollectionList.size();
		for (int i=0; i<lNum; i++)
		{
			if (lOdds<pst->vectorCollectionList[i].lOdds)
			{
				pstCollection = &pst->vectorCollectionList[i];
				break;
			}
		}

		if (pstCollection==NULL)
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_2",st.lIndex));//编号=%d的采集物没有机会出生！
			continue;
		}

		// 寻找刷新点
		long x, y;
		bool bRet = GetRandomPosInRange(x, y, pst->rectRange.left, pst->rectRange.top, pst->rectRange.right-pst->rectRange.left, pst->rectRange.bottom-pst->rectRange.top);
		if( bRet == false )
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_3",pstCollection->cName));// 无法为Collection “%s” 找到合适的位置！
		}

		const CCollectionLayout* pColLay=CDeputyOccuSystem::GetCollectionLayout(pstCollection->cName);
		if (NULL==pColLay)
		{break;}

		CCollection* pCollection=GetOneCollection();
		if (NULL==pCollection)
		{break;}

		pCollection->Init(pColLay);
		AddCollection_i (pCollection,x,y,pst->lDir);

		pCollection->SetRefreshIndex(pst->lIndex);
		pst->lLivingCount++;
	}
	return TRUE;
}

BOOL CServerRegion::AddCollection(const char* strOrginName, long count, long left, long right, long top, long bottom)
{
	const CCollectionLayout* pst = CDeputyOccuSystem::GetCollectionLayout(strOrginName);
	if (NULL==pst){return FALSE;}

	for (int a=0; a<count; a++)
	{

		long x, y;
		bool bRet = GetRandomPosInRange(x, y, left, top, right-left, bottom-top);
		if( bRet == false )
		{
			Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_3",strOrginName));
			break;
		}

		CCollection* pCollection=GetOneCollection();
		if (NULL == pCollection)break;

		pCollection->Init(pst);
		AddCollection_i (pCollection,x,y,-1);
	}
	return TRUE;
}
BOOL CServerRegion::AddCollection_i (CCollection*collection ,long x,long y,long lDir)
{
	if (NULL==collection)
	{
		return FALSE;
	}
	if (lDir==-1)
	{
		lDir=random(8);
	}

	collection->SetDir(lDir%8);
	collection->SetState(CShape::STATE_PEACE);
	collection->SetHP(1);
	AddObject( collection );
	collection->SetPosXY( x+0.5f, y+0.5f );

	// 发送消息给玩家
	//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,collection->GetExID());
	//MsgSend.Add( collection->GetType() );
	//////
	//MsgSend.Add( collection->GetExID() );
	//DBWriteSet setWriteDB;
	//MsgSend.GetDBWriteSet(setWriteDB);
	//collection->CodeToDataBlock(setWriteDB,false);
	//MsgSend.Add( (char)0 );		// 是否出现特效
	//MsgSend.SendToAround(collection);

	AddShapeToAround(collection);

	//g_lTotalMonster++;
	return TRUE;

}

CCollection* CServerRegion::AddCollection(const char* strOrginName,long x, long y, long lDir)
{
	const CCollectionLayout* pst = CDeputyOccuSystem::GetCollectionLayout(strOrginName);
	if (NULL==pst){return FALSE;}

	CCollection* pCollection=GetOneCollection();
	if (NULL == pCollection)return 0;

	pCollection->Init(pst);
	AddCollection_i (pCollection,x,y,lDir);
	return pCollection;
}

CServerRegion::tagCollection* CServerRegion::GetCollectionRefeash(long lIndex)
{
	if (lIndex<0)
	{
		return NULL;
	}
	for(list<tagCollection>::iterator it=m_listCollection.begin(); it!=m_listCollection.end(); it++)
	{
		if((*it).lIndex == lIndex)
		{
			return &(*it);
		}
	}
	return NULL;
}
BOOL CServerRegion::RefreshCollection(CCollection* pCollection)
{
	if (NULL==pCollection)return FALSE;

	tagCollection* pst = GetCollectionRefeash(pCollection->GetRefreshIndex());
	if (NULL==pst) 
	{
		DeleteOneCollection(pCollection);
		return FALSE;
	}

	long x, y;
	bool bRet = GetRandomPosInRange(x, y, pst->rectRange.left, pst->rectRange.top, pst->rectRange.right-pst->rectRange.left, pst->rectRange.bottom-pst->rectRange.top);
	if( bRet == false )
	{
		Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_3",pCollection->GetName()));//无法为Collection “%s” 找到合适的位置！
	}

	const CCollectionLayout* pColLay= CDeputyOccuSystem::GetCollectionLayout(pCollection->GetCollectionName().c_str());
	if (NULL == pColLay)
	{
		DeleteOneCollection(pCollection);
		return FALSE;
	}

	pCollection->Init(pColLay);
	AddCollection_i (pCollection,x,y,pst->lDir);

	return TRUE;
}

BOOL CServerRegion::DeleteOneCollection(CCollection*)
{return TRUE;}
CCollection* CServerRegion::GetOneCollection()
{
	CCollection* pCollection = OBJ_CREATE(CCollection);
	if(pCollection)
	{
		CGUID guid;
		CGUID::CreateGUID(guid);
		pCollection->SetExID(guid);
		pCollection->SetType(TYPE_COLLECTION);
	}
	return pCollection;
}


/////////////////////////////////////////////////////////////////////
// 场景管理
/////////////////////////////////////////////////////////////////////
// 使OBJECT重生
void CServerRegion::ReBornObject(CBaseObject* pObject)
{
	//##首先确定X,Y坐标的有效性
	CShape* pShape = dynamic_cast<CShape*>( pObject );
	CPlayer* pRecordPlayer=NULL;
	if( pShape == NULL ) return;

	pShape -> SetFather( this );
	long x = pShape->GetTileX();
	long y = pShape->GetTileY();

	// 位置不在地图范围内
	if( ( x < 0 || x >= GetWidth() || y < 0 || y >= GetHeight()) && m_pCell )
	{
		GetRandomPos(x, y);
	}

	pShape->SetPosXY( x+0.5f, y+0.5f );


	//##最后添加到所属的Area中。
	CArea* pArea = GetArea(x/AREA_WIDTH, y/AREA_HEIGHT);
	if( pArea )
	{
		if (pRecordPlayer!=NULL)
		{
			WakeUp();
		}

		pArea->AddObject(pShape);
		pShape -> SetArea( pArea );

	}
	else
	{
		DeleteChildObject( pShape );
	}
}
// 添加object
void CServerRegion::AddObject(CBaseObject* pObject)
{
	//##首先确定X,Y坐标的有效性
	CShape* pShape = dynamic_cast<CShape*>( pObject );
	CShape* pRecordObj = NULL;
	if( pShape == NULL ) return;

	pShape -> SetFather( this );

	long x = pShape->GetTileX();
	long y = pShape->GetTileY();

	//##其次,添加到所属的哈希表中.
	switch( pShape -> GetType() )
	{
	case TYPE_MONSTER:
		{
			CMonster* pMonster = (CMonster*)( pShape );
			m_mMonsters[pMonster -> GetExID()] = pMonster;
		}
		break;

	case TYPE_PET:			// 宠物
		{
			//PutoutLog("Pet", LT_DEFAULT, "PET...");
			CPet* pPet = (CPet*)(pShape);
			m_Pets[pPet->GetExID()] = pPet;
		}
		break;

	case TYPE_PLAYER:
		{
			if( m_setPlayers.empty() )
			{
				pRecordObj = pShape;
			}
			m_setPlayers.insert(pShape -> GetExID());
		}
		break;

	case TYPE_GOODS:
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( pShape );
			m_mGoods[pGoods ->GetExID()] = pGoods;
		}
		break;

	case TYPE_COLLECTION:
		{
			CCollection* pCollection= dynamic_cast<CCollection*>(pShape);
			m_mCollection[pCollection->GetExID()] = pCollection;
		}
		break;
	case TYPE_CITYGATE:
		{
			CBuild* pBulid=dynamic_cast<CBuild*>(pShape);
			m_mBuild[pBulid->GetExID()]=pBulid;
		}
		break;

	default:
		{
			//INT64 nHashValue = GetHashValue( pShape -> GetType(), pShape -> GetExID() );
			m_mOtherShapes[pShape -> GetExID()] = pShape;
		}
		break;
	}

	//##最后添加到所属的Area中。
	CArea* pArea = GetArea(x/AREA_WIDTH, y/AREA_HEIGHT);
	if( pArea )
	{
		if( pRecordObj != NULL )
		{
			WakeUp();
		}

		pArea->AddObject(pShape);
		pShape->SetArea( pArea );

	}
	else
	{
		Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_6",GetName(),pShape->GetName(),x, y));
		assert(0);
	}
}

void CServerRegion::DeleteChildObject(LONG lType, const CGUID& guid)
{
	CBaseObject* pObj = FindChildObject( lType, guid);
	if( pObj )
	{
		DeleteChildObject( pObj );
	}
}

void CServerRegion::DeleteChildObject(CBaseObject* pObject)
{
	CShape* pShape = dynamic_cast<CShape*>(pObject);
	if( pShape )
	{
		//取消计数和引用
		if(pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER)
			DecCellObjNum(pShape->GetTileX(),pShape->GetTileY());
		if(pShape->GetType() == TYPE_MONSTER)
			pShape->SetBlock(pShape->GetTileX(),pShape->GetTileY(),BLOCK_NO);
		//使用脚本DelCollection()删除采集物时,取消其阻挡
		if(pShape->GetType() == TYPE_COLLECTION)
			pShape->SetBlock(pShape->GetTileX(),pShape->GetTileY(),BLOCK_NO);

		CArea* pArea = pShape->GetArea();
		if( pArea )
		{
			pArea->RemoveObject(pObject);
		}

		//##从哈希表中删除
		switch( pShape -> GetType() )
		{
		case TYPE_MONSTER:
			{
				hash_map<CGUID, CMonster*>::iterator it = m_mMonsters.find( pShape -> GetExID() );
				if( it != m_mMonsters.end() )
				{
					m_mMonsters.erase( it );
				}
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pShape);
			}
			break;
		case TYPE_PLAYER:
			{
				hsGUIDIt it = m_setPlayers.find(pShape -> GetExID());
				if( it != m_setPlayers.end() )
				{
					m_setPlayers.erase( it );
				}
			}
			break;

		case TYPE_PET:
			{
				hash_map<CGUID, CPet*>::iterator it = m_Pets.find(pShape->GetExID());
				if( it != m_Pets.end() )
				{
					m_Pets.erase( it );
				}
			}
			break;

		case TYPE_GOODS:
			{
				goodIt it = m_mGoods.find( pShape ->GetExID() );
				if( it != m_mGoods.end() )
				{
#ifdef _GOODSLOG1_
					if(it->second)
					{
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						it->second->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:CServerRegion::DeleteChildObject][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							it->second->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(it->second->GetBasePropertiesIndex()),srcGoodsGUID,it->second->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
					}			
#endif	
					CGoods* pGoods = (it->second);					
					m_mGoods.erase( it );
					CGoodsFactory::GarbageCollect(&pGoods,100);
				}
			}
			break;
		case TYPE_COLLECTION:
			{

				hash_map<CGUID, CCollection*,hash_guid_compare>::iterator it  = m_mCollection.find(pShape -> GetExID() );
				if( it != m_mCollection.end() )
				{
					//SAFE_DELETE( it -> second );
					m_mCollection.erase( it );
				}
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pShape);
			}
			break;
		case TYPE_CITYGATE:
			{
				hash_map<CGUID,CBuild*,hash_guid_compare>::iterator it=m_mBuild.find(pShape->GetExID());
				if ( it!=m_mBuild.end() )
				{
					m_mBuild.erase(it);
				}
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pShape);
			}
			break;
		default:
			{
				//INT64 nHashValue = GetHashValue( pShape -> GetType(), pShape -> GetExID() );
				hash_map<CGUID, CShape*>::iterator it = m_mOtherShapes.find( pShape -> GetExID() );
				if( it != m_mOtherShapes.end() )
				{
					OBJ_RELEASE(CShape,  it -> second );
					m_mOtherShapes.erase( it );
				}
			}
			break;
		}

	}

}

// 移出OBJECT
void CServerRegion::RemoveObject(CBaseObject* pObject)
{
	CShape* pShape = dynamic_cast<CShape*>(pObject);
	if( pShape )
	{
		CArea* pArea = pShape->GetArea();
		if( pArea )
		{
			pArea->RemoveObject(pObject);
			pShape->SetArea(NULL);
		}
		//取消计数和引用
		if(pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER)
			DecCellObjNum(pShape->GetTileX(),pShape->GetTileY());
		if(pShape->GetType() == TYPE_MONSTER)
			pShape->SetBlock(pShape->GetTileX(),pShape->GetTileY(),BLOCK_NO);


		//##从哈希表中删除
		switch( pShape -> GetType() )
		{
		case TYPE_MONSTER:
			{
				hash_map<CGUID, CMonster*>::iterator it = m_mMonsters.find( pShape -> GetExID() );
				if( it != m_mMonsters.end() )
				{
					m_mMonsters.erase( it );
				}
			}
			break;
		case TYPE_PLAYER:
			{
				hsGUIDIt it = m_setPlayers.find( pShape -> GetExID() );
				if( it != m_setPlayers.end() )
				{
					m_setPlayers.erase( it );
					if (m_setPlayers.size()==0)
					{
						Hibernate();
					}
				}
			}
			break;
		case TYPE_PET:
			{
				hash_map<CGUID, CPet*>::iterator it = m_Pets.find(pShape->GetExID());
				if( it != m_Pets.end() )
				{
					m_Pets.erase( it );
					pShape->SetFather(NULL);
				}
			}
			break;
		case TYPE_GOODS:
			{
				goodIt it = m_mGoods.find( pShape -> GetExID() );
				if( it != m_mGoods.end() )
				{
					m_mGoods.erase( it );
					pShape->SetFather(NULL);
				}
			}
			break;
		case TYPE_COLLECTION:
			{
				hash_map<CGUID, CCollection*, hash_guid_compare>::iterator it= m_mCollection.begin();
				if (it != m_mCollection.end())
				{
					m_mCollection.erase( it );
				}
			}
			break;
		case TYPE_CITYGATE:
			{
				hash_map<CGUID, CBuild*, hash_guid_compare>::iterator it= m_mBuild.begin();
				if (it != m_mBuild.end())
				{
					m_mBuild.erase( it );
				}
			}
			break;
		default:
			{
				//INT64 nHashValue = GetHashValue( pShape -> GetType(), pShape -> GetExID() );
				hash_map<CGUID, CShape*>::iterator it = m_mOtherShapes.find( pShape -> GetExID() );
				if( it != m_mOtherShapes.end() )
				{
					//SAFE_DELETE( it -> second );
					m_mOtherShapes.erase( it );
				}
			}
			break;
		}
	}
}

// 删除所有子OBJECT(pExpect = 除了)
void CServerRegion::DeleteAllChildObject(CBaseObject* pExpect)
{
	//##删除所有的Area
	SAFE_DELETE_ARRAY( m_pArea );

	//##删除掉所有的怪物, 宠物, NPC和物品. 玩家由上一层管理
	hash_map<CGUID, CMonster*>::iterator itMonster = m_mMonsters.begin();
	for( ; itMonster != m_mMonsters.end(); itMonster ++ )
	{
		itMonster->second->	ClearActiveSkills();
		GameManager::GetInstance()->FreeObject((CBaseObject**)&(itMonster->second));
	}
	m_mMonsters.clear();

	hash_map<CGUID, CCollection*, hash_guid_compare>::iterator itCollection=m_mCollection.begin();
	for (; itCollection!= m_mCollection.end(); itCollection++)
	{
		GameManager::GetInstance()->FreeObject((CBaseObject**)&(itCollection->second));
	}
	m_mCollection.clear();

	goodIt itGoods = m_mGoods.begin();
	for( ; itGoods != m_mGoods.end(); itGoods ++ )
	{
#ifdef _GOODSLOG1_
		if(itGoods->second)
		{
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			itGoods->second->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:CServerRegion::DeleteChildObject][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				itGoods->second->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(itGoods->second->GetBasePropertiesIndex()),srcGoodsGUID,itGoods->second->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
		}			
#endif	
		CGoodsFactory::GarbageCollect(&(itGoods->second),101);
	}
	m_mGoods.clear();

	hash_map<CGUID, CBuild*, hash_guid_compare>::iterator itBuild=m_mBuild.begin();
	for (; itBuild!= m_mBuild.end(); itBuild++)
	{
		GameManager::GetInstance()->FreeObject((CBaseObject**)&(itBuild->second));
	}
	m_mBuild.clear();
	//hash_map<CGUID, CMonster*>::iterator itNpc = m_mNpcs.begin();
	//for( ; itNpc != m_mNpcs.end(); itNpc ++ )
	//{
	//	SAFE_DELETE( itNpc -> second );
	//}
	//m_mNpcs.clear();

	hash_map<CGUID, CShape*>::iterator itShape = m_mOtherShapes.begin();
	for( ; itShape != m_mOtherShapes.end(); itShape ++ )
	{
		OBJ_RELEASE(CShape, itShape -> second );
	}
	m_mOtherShapes.clear();

	//##这儿只清理掉玩家的ID
	m_setPlayers.clear();
}


//查找子object!
bool CServerRegion::FindChildObject(CBaseObject* p)
{
	if(p==NULL)	return false;
	if( FindChildObject( p -> GetType(), p->GetExID() ) )
	{
		return true;
	}

	return false;
}

//查找子object!
CBaseObject* CServerRegion::FindChildObject(long lType,const CGUID& guid)
{
	CBaseObject* pResult = NULL;

	switch( lType )
	{
	case TYPE_MONSTER:
		{
			hash_map<CGUID, CMonster*>::iterator it = m_mMonsters.find( guid );
			if( it != m_mMonsters.end() )
			{
				pResult = it -> second;
			}
		}
		break;

	case TYPE_PET:
		{
			hash_map<CGUID, CPet*>::iterator it = m_Pets.find( guid );
			if( it != m_Pets.end() )
			{
				pResult = it->second;
			}
		}
		break;

	case TYPE_PLAYER:
		{
			hsGUIDIt it = m_setPlayers.find( guid );
			if( it != m_setPlayers.end() )
			{
				pResult = GetGame() -> FindPlayer( guid );
			}
		}
		break;

	case TYPE_GOODS:
		{
			goodIt it = m_mGoods.find( guid );
			if( it != m_mGoods.end() )
			{
				CBaseObject* pGoods = (*it).second;
				if( pGoods && pGoods->GetType() != TYPE_GOODS)
				{
					m_mGoods.erase(it);
					Log4c::Warn(ROOT_MODULE, GetText("GS_RGNSERVERREGION_7"));//场景中的物品异常
				}
				else
				{
					pResult = (*it).second;
				}
			}
		}
		break;

	case TYPE_COLLECTION:
		{
			hash_map<CGUID, CCollection*, hash_guid_compare>::iterator it=m_mCollection.find(guid);
			if (it != m_mCollection.end())
			{
				pResult = it ->second;
			}
		}
		break;

	case TYPE_CITYGATE:
		{
			hash_map<CGUID, CBuild*, hash_guid_compare>::iterator it=m_mBuild.find(guid);
			if (it != m_mBuild.end())
			{
				pResult = it ->second;
			}
		}

	default:
		{
			//INT64 nHashValue = GetHashValue( lType, guid );
			hash_map<CGUID, CShape*>::iterator it = m_mOtherShapes.find( guid );
			if( it != m_mOtherShapes.end() )
			{
				pResult = it -> second;
			}
		}
		break;
	}

	return pResult;
}

// 按名字查找
CBaseObject* CServerRegion::FindChildObjectByName(long lType, const char* strName)
{
	CBaseObject* pResult = NULL;

	switch( lType )
	{
	case TYPE_MONSTER:
		{
			for (hash_map<CGUID, CMonster*>::iterator it = m_mMonsters.begin(); it!=m_mMonsters.end(); it++)
			{
				if (strcmp(it->second->GetName(), strName)==0)
				{
					pResult = it -> second;
					break;
				}
			}
		}
		break;

	case TYPE_PLAYER:
		{
			for (hsGUIDIt it = m_setPlayers.begin(); it!=m_setPlayers.end(); it++)
			{
				CPlayer* pPlayer = GetGame() -> FindPlayer( *it );
				if (pPlayer)
				{
					if (strcmp(pPlayer->GetName(), strName)==0)
					{
						pResult = pPlayer;
						break;
					}
				}
			}
		}
		break;

	case TYPE_PET:
		{
			hash_map<CGUID, CPet*>::iterator it = m_Pets.begin();
			for ( ; it != m_Pets.end(); ++it )
			{
				pResult = it->second;
				if ( pResult != NULL &&
					0 == strcmp(pResult->GetName(), strName) )
				{
					break;
				}
			}
		}
		break;

	case TYPE_GOODS:
		{
			for (goodIt it = m_mGoods.begin(); it!=m_mGoods.end(); it++)
			{
				if (strcmp(it->second->GetName(), strName)==0)
				{
					pResult = it -> second;
					break;
				}
			}
		}
		break;

	case TYPE_COLLECTION:
		{
			hash_map<CGUID, CCollection*, hash_guid_compare>::iterator it=m_mCollection.begin();
			for (; it!= m_mCollection.end(); it++)
			{
				if (strcmp(it->second->GetName(),strName)==0)
				{
					pResult = it ->second;
					break;
				}
			}
		}
		break;

	case TYPE_CITYGATE:
		{
			hash_map<CGUID, CShape*, hash_guid_compare>::iterator it=m_mOtherShapes.begin();
			for (;it!=m_mOtherShapes.end();it++)
			{
				if (strcmp(it->second->GetName(),strName)==0)
				{
					pResult = it->second;
				}
			}
		}
		break;
	}

	return pResult;
}

// 查找pShape周围的object
CBaseObject* CServerRegion::FindAroundObject(CShape* pShape, long type, const CGUID& guid)
{
	if( pShape == NULL || pShape -> GetArea() == NULL )
	{
		return NULL;
	}

	CShape* pResult = dynamic_cast<CShape*>( FindChildObject(type, guid) );

	if( pResult && pResult -> GetArea() )
	{
		if( abs( pResult -> GetArea() -> GetX() -  pShape -> GetArea() -> GetX() ) <= 1 &&
			abs( pResult -> GetArea() -> GetY() -  pShape -> GetArea() -> GetY() ) <= 1 )
		{
			//##通过，返回这个Shape
		}
		else
		{
			pResult = NULL;
		}
	}
	else 
	{
		pResult = NULL;
	}

	return pResult;
}

//查找类型匹配的OBJECT
CBaseObject* CServerRegion::FindAroundObject(CShape* pMoveShape, long type)
{
	if (pMoveShape==NULL || pMoveShape->GetArea() == NULL)
		return NULL;

	BEGIN_LOOP_AROUND_SHAPE_BY_TYPE(this, pMoveShape->GetArea()->GetX(), pMoveShape->GetArea()->GetY(), type)
	{
		if( pShape != pMoveShape )
		{
			CMoveShape* pMoveable = dynamic_cast<CMoveShape*>( pShape );
			if( pMoveable && pMoveable -> IsDied() )
			{
				continue;
			}
			return pShape;
		}
	}
	END_LOOP_AROUND_SHAPE_BY_TYPE
		return NULL;
}

void CServerRegion::SendMsgToAroundShape(CShape* pShape,CMessage* pMsg)
{
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>( pShape );
	if( pMoveShape && pMoveShape -> GetArea() )
	{
		BEGIN_LOOP_AROUND_PLAYER(this, pMoveShape)
		{
			if( pPlayer && pPlayer -> IsDied() == FALSE )
			{
				pMsg -> SendToPlayer( ( pPlayer->GetExID() ) );
			}
		}
		END_LOOP_AROUND_PLAYER
	}
}

//查找最近的OBJECT
CBaseObject* CServerRegion::FindNearestAroundObject(CShape* pMoveShape,long type)
{
	if (pMoveShape==NULL || pMoveShape->GetArea() == NULL)
		return NULL;

	WORD wDistance=0xffff;
	CShape* pNearestShape=NULL;
	for(int i=0;i<9;i++)
	{
		WORD wTempDistance=0xffff;
		CBaseObject* pObject=FindNearestObjectInArea(pMoveShape,type,
			pMoveShape->GetArea()->GetX()+_area[i][0],
			pMoveShape->GetArea()->GetX()+_area[i][1],wTempDistance);
		if(pObject)
		{
			if(wTempDistance<wDistance)
			{
				wDistance=wTempDistance;
				pNearestShape=dynamic_cast<CShape*>(pObject);
			}
		}
	}
	return pNearestShape;
}

CBaseObject* CServerRegion::FindNearestObjectInArea(CShape* pMoveShape,long type,long x,long y,WORD& wDistance)
{	
	if (pMoveShape==NULL || pMoveShape->GetArea() == NULL)
		return NULL;

	CShape* pNearestShape=NULL;
	CArea* pArea =GetArea(pMoveShape->GetArea()->GetX(),pMoveShape->GetArea()->GetY());
	if(pArea)
	{
		vector<CShape*> vOut;
		pArea -> FindShapes( type, vOut );
		vector<CShape*>::iterator it = vOut.begin();
		for( ; it != vOut.end(); it++ )
		{
			CShape* pShape = (*it);
			if(pShape && pShape!=pMoveShape)
			{
				CMoveShape* pMoveable = dynamic_cast<CMoveShape*>(pShape);
				if( pMoveable && pMoveable -> IsDied() )
				{
					continue;
				}
				long lDistance=pMoveShape->Distance(pShape);
				if((WORD)lDistance < wDistance)
				{
					pNearestShape=pShape;
					wDistance=(WORD)lDistance;
				}
			}
		}
	}
	return pNearestShape;
}

void CServerRegion::FindAroundObject( CShape* pFinder, long type, vector<CShape*>& vOut )
{
	if (pFinder == NULL || pFinder->GetArea() == NULL)
		return;

	BEGIN_LOOP_AROUND_SHAPE_BY_TYPE(this, pFinder->GetArea()->GetX(), pFinder->GetArea()->GetY(), type)
	{
		if( pShape!=pFinder )
		{
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>( pShape );
			if( pMoveShape && pMoveShape -> IsDied() )
			{
				continue;
			}
			vOut.push_back( pShape );
		}
	}
	END_LOOP_AROUND_SHAPE_BY_TYPE
}


//在一个范围里找寻指定数量最近的玩家
void CServerRegion::FindLimitPlayersAround(CMoveShape* pMoveShape,long lMaxNum,
										   hashsetGUID& vOut,const CGUID& guidExceptID)
{
	vOut.clear();
	hashsetGUID setPlayers;	//以玩家距离排序的集合
	long lX = pMoveShape->GetTileX();
	long lY = pMoveShape->GetTileY();
	long lAreaX = lX/AREA_WIDTH;
	long lAreaY = lY/AREA_HEIGHT;

	for(int i=0; i<9; i++)
	{
		CArea* pArea = GetArea(lAreaX+_area[i][0], lAreaY+_area[i][1]);
		if( pArea )
		{
			setPlayers.clear();
			pArea -> FindShapes( TYPE_PLAYER, setPlayers );			
			vOut.insert(setPlayers.begin(),setPlayers.end());
		}
	}
	//是否可以向更远处的玩家发送消息
	if( (long)vOut.size() <= lMaxNum)	return;

	multimap<long,CGUID>	MutiPlayers;
	long lType = 0;
	CGUID guidTarget = NULL_GUID;
	hsGUIDIt it = vOut.begin();
	for(;it != vOut.end();it++)
	{
		CPlayer* pPlayer = GetGame()->FindPlayer((*it));
		if(pPlayer)
		{
			long lDistance = Distance(lX,lY,pPlayer->GetTileX(),pPlayer->GetTileY());
			MutiPlayers.insert(pair<long,CGUID>(lDistance,pPlayer->GetExID()));
		}
	}
	vOut.clear();
	long lSize = 0;
	multimap<long,CGUID>::iterator itMul = MutiPlayers.begin();
	for(;itMul != MutiPlayers.end();itMul++)
	{
		++lSize;
		vOut.insert((*itMul).second);
		if(lSize >= lMaxNum)
			break;
	}
	vOut.insert(pMoveShape->GetExID());
	if(guidExceptID != NULL_GUID)
		vOut.erase(NULL_GUID);
	return;
}
//在周围找出指定距离内的玩家
void CServerRegion::FindNearestPlayer(long lX,long lY,long lDis,vector<CGUID>& vOut)
{
	hashsetGUID setPlayers;	//以玩家距离排序的集合

	long lAreaX = lX/AREA_WIDTH;
	long lAreaY = lY/AREA_HEIGHT;
	for(int i=0; i<9; i++)
	{
		CArea* pArea = GetArea(lAreaX+_area[i][0], lAreaY+_area[i][1]);
		if( pArea )
		{
			setPlayers.clear();
			pArea -> FindShapes( TYPE_PLAYER, setPlayers );
			for( hsGUIDIt it = setPlayers.begin(); it != setPlayers.end(); it++ )
			{
				CPlayer* pPlayer = GetGame()->FindPlayer((*it));
				if(pPlayer)
				{
					long lDistance = Distance(lX,lY,pPlayer->GetTileX(),pPlayer->GetTileY());
					if(lDistance <= lDis)
						vOut.push_back((*it));
				}
			}
		}
	}
	return;
}

void CServerRegion::FindAroundPlayer( CShape* pFinder, vector<CPlayer*>& vOut )
{
	if (pFinder == NULL || pFinder->GetArea() == NULL)
		return;

	BEGIN_LOOP_AROUND_PLAYER( this, pFinder )
	{
		vOut.push_back( pPlayer );
	}
	END_LOOP_AROUND_PLAYER
}

//void CServerRegion::FindAroundPets(CShape* pFinder, vector<CMonster*>& vOut)
//{
//	if (pFinder == NULL || pFinder->GetArea() == NULL)
//		return;
//
//	CMonster* pMonster = NULL;
//	BEGIN_LOOP_AROUND_SHAPE_BY_TYPE( this, pFinder -> GetArea() -> GetX(), pFinder -> GetArea() -> GetY(), TYPE_PET )
//	{
//		pMonster = dynamic_cast<CMonster*>( pShape );
//		if( pMonster && pMonster -> DoesCreatureBeenTamed() )
//		{
//			vOut.push_back( pMonster );
//		}
//	}
//	END_LOOP_AROUND_PLAYER
//}

CMonster*	CServerRegion::FindMonsterByID(const CGUID& guid)
{
	hash_map<CGUID, CMonster*>::iterator it = m_mMonsters.find( guid );
	if( it != m_mMonsters.end() )
	{
		return it -> second;
	}
	return NULL;
}

/// 在场景上根据宠物GUID查找宠物
CPet* CServerRegion::FindPetByID(const CGUID& guid)
{
	hash_map<CGUID, CPet*>::iterator it = m_Pets.find(guid);
	if (it!=m_Pets.end())
	{
		return it->second;
	}
	return NULL;
}

// 创建Area数组
void CServerRegion::CreateAreaArray()
{
	// 建立场景分区
	m_lAreaX = GetWidth() / AREA_WIDTH;
	m_lAreaY = GetHeight() / AREA_HEIGHT;
	if( GetWidth()%AREA_WIDTH != 0 )
	{
		m_lAreaX++;
	}
	if( GetHeight()%AREA_HEIGHT != 0 )
	{
		m_lAreaY++;
	}

	SAFE_DELETE_ARRAY(m_pArea);
	m_pArea = new CArea[m_lAreaX*m_lAreaY];
	for(int i=0; i<m_lAreaX; i++)
	{
		for(int j=0; j<m_lAreaY; j++)
		{
			m_pArea[m_lAreaX*j+i].SetX(i);
			m_pArea[m_lAreaX*j+i].SetY(j);
			m_pArea[m_lAreaX*j+i].SetFather(this);
		}
	}
}

// 获取区域指针
CArea* CServerRegion::GetArea(long x, long y)
{
	if( x < 0 || x >= m_lAreaX || y < 0 || y >= m_lAreaY )
	{
		return NULL;
	}

	return &m_pArea[m_lAreaX * y + x];
}


// 获取一个点上是否有Shape
CShape* CServerRegion::GetShape(long x, long y)
{
	BEGIN_LOOP_AROUND_SHAPE(this, x/AREA_WIDTH, y/AREA_HEIGHT)
	{
		if ( abs(pShape->GetTileX()-x)<=pShape->GetFigure(CShape::DIR_RIGHT) && abs(pShape->GetTileY()-y)<=pShape->GetFigure(CShape::DIR_UP))
		{
			return pShape;
		}
	}
	END_LOOP_AROUND_SHAPE;

	return NULL;
}

void CServerRegion::GetShape( LONG lX, LONG lY, vector<CShape*>& vOut )
{
	BEGIN_LOOP_AROUND_SHAPE(this, lX/AREA_WIDTH, lY/AREA_HEIGHT)
	{
		if ( abs(pShape->GetTileX()-lX)<=pShape->GetFigure(CShape::DIR_RIGHT) && abs(pShape->GetTileY()-lY)<=pShape->GetFigure(CShape::DIR_UP))
		{
			vOut.push_back( pShape );
		}
	}
	END_LOOP_AROUND_SHAPE;
}

// 获取爆出物品的位置，(sx,s)为原点， lStartPos是从第几个位置开始计算(共49个位置)
// 返回暴出位置的顺序值(0-47)
long CServerRegion::GetDropGoodsPos(long sx, long sy, long& dx, long& dy, long lStartPos)
{
	// 暴出物品的位置顺序
	static int Pos[49][2] = 
	{
		// 脚底下
		0, 0,

		// 第一圈
		-1,-1, 0,-1, 1,-1,
		-1, 0,	   1, 0,
		-1, 1, 0, 1, 1, 1,

		// 第二圈
		-2,-2, -1,-2, 0,-2, 1,-2, 2,-2,
		-2,-1,					 2,-1,
		-2, 0,					 2, 0,
		-2, 1,					 2, 1,
		-2, 2, -1, 2, 0, 2, 1, 2, 2, 2,

		// 第三圈
		-3,-3, -2,-3, -1,-3, 0,-3, 1,-3, 2,-3, 3,-3,
		-3,-2,								   3,-2,
		-3,-1,								   3,-1,
		-3, 0,								   3, 0,
		-3, 1,								   3, 1,
		-3, 2,								   3, 2,
		-3, 3, -2, 3, -1, 3, 0, 3, 1, 3, 2, 3, 3, 3
	};

	// 位置和顺序的对应关系
	static int Pos2Order[7][7] = 
	{
		25, 26, 27, 28, 29, 30, 31,
		32,  9, 10, 11, 12, 13, 33,
		34, 14,  1,  2,  3, 15, 35,
		36, 16,  4,  0,  5, 17, 37,
		38, 18,  6,  7,  8, 19, 39,
		40, 20, 21, 22, 23, 24, 41,
		42, 43, 44, 45, 46, 47, 48
	};

	// 先计算出每个格子上的物品数量
	BYTE PosNum[49];
	memset(PosNum, 0, 49);

	int areax = sx / AREA_WIDTH;
	int areay = sy / AREA_HEIGHT;
	int area_x = sx % AREA_WIDTH;
	int area_y = sy % AREA_HEIGHT;

	// 计算需要遍历的AREA
	POINT ptArea = {0,0};
	list<POINT> listLoopArea;
	listLoopArea.push_back(ptArea);

	if( area_x < 3 )
	{
		ptArea.x = -1;
		ptArea.y = 0;
		listLoopArea.push_back(ptArea);

		if (area_y < 3)						// 左上角
		{
			ptArea.x = -1;
			ptArea.y = -1;
			listLoopArea.push_back(ptArea);

			ptArea.x = 0;
			ptArea.y = -1;
			listLoopArea.push_back(ptArea);
		}
		else if (area_y > AREA_HEIGHT - 3)	// 左下角
		{
			ptArea.x = -1;
			ptArea.y = 1;
			listLoopArea.push_back(ptArea);

			ptArea.x = 0;
			ptArea.y = 1;
			listLoopArea.push_back(ptArea);
		}
		else								// 左边
		{
		}
	}
	else if( area_x >= AREA_WIDTH - 3 )
	{
		ptArea.x = 1;
		ptArea.y = 0;
		listLoopArea.push_back(ptArea);

		if (area_y < 3)						// 右上角
		{
			ptArea.x = 1;
			ptArea.y = -1;
			listLoopArea.push_back(ptArea);

			ptArea.x = 0;
			ptArea.y = -1;
			listLoopArea.push_back(ptArea);
		}
		else if (area_y > AREA_HEIGHT - 3)	// 右下角
		{
			ptArea.x = 0;
			ptArea.y = 1;
			listLoopArea.push_back(ptArea);

			ptArea.x = 1;
			ptArea.y = 1;
			listLoopArea.push_back(ptArea);
		}
		else								// 右边
		{
		}
	}
	else
	{
		if (area_y < 3)						// 中上
		{
			ptArea.x = 0;
			ptArea.y = -1;
			listLoopArea.push_back(ptArea);
		}
		else if (area_y > AREA_HEIGHT - 3)	// 中下
		{
			ptArea.x = 0;
			ptArea.y = 1;
			listLoopArea.push_back(ptArea);
		}
		else								// 中
		{
		}
	}

	// 遍历area
	for(list<POINT>::iterator it=listLoopArea.begin(); it!=listLoopArea.end(); it++)
	{
		CArea* pArea = ((CServerRegion*)this)->GetArea(areax+(*it).x, areay+(*it).y);
		if( pArea )
		{
			vector<CShape*> vGoods;
			pArea -> FindShapes( TYPE_GOODS, vGoods );
			for( size_t i = 0; i < vGoods.size(); i ++ )
			{
				CShape* pShape = vGoods[i];
				if( pShape )
				{
					int x = pShape->GetTileX() - sx;
					int y = pShape->GetTileY() - sy;
					if( x > -4 && x < 4 && y > -4 && y < 4 )
					{
						PosNum[ Pos2Order[3+y][3+x] ]++;
					}
				}
			}
		}
	}

	// 开始寻找位置
	int n = 0;
	int lLoopNum = 1;
	while(n++<1000)
	{
		for(int i=lStartPos; i<49; i++)
		{
			if( PosNum[i] < lLoopNum )
			{
				dx = sx + Pos[i][0];
				dy = sy + Pos[i][1];

				BYTE block = GetBlock(dx, dy);

				// 首先不是地形阻挡
				if (block == BLOCK_NO || block == BLOCK_AIM)
				{
					// 不是切换点
					if (GetSwitch(dx, dy)==NULL)
					{
						//  怪物阻挡
						if (block == BLOCK_AIM)
						{
							CShape *pShape = GetShape(dx, dy);
							if (pShape==NULL || pShape->GetType() != TYPE_MONSTER)	// 不是NPC，该格才能掉落
							{
								return ++i;
							}
						}
						// 空的不是阻挡的点
						else
						{
							return ++i;
						}
					}
				}
			}
		}

		lStartPos = 0;
		lLoopNum++;
	}

	return -1;
}

void CServerRegion::OnRefreshRegion(long lWarNum)
{
	//if (m_listDeadMonster.size()==0)
	//{
	//	return;
	//}

	//list<CBaseObject*>::iterator it= m_listDeadMonster.begin();

	//for (;it!=m_listDeadMonster.end();)
	//{
	//	CMonster* pMonster=dynamic_cast<CMonster*>(*it);
	//	if (pMonster==NULL )//|| pMonster->GetCanReburn()==FALSE)
	//	{
	//		m_listDeadMonster.erase(it);
	//		continue;
	//	}

	//	int iFreshIndex=pMonster->GetRefeashIndex();
	//	CServerRegion::tagMonster* pRefresh=GetMonsterRefeash( iFreshIndex);

	//	if (pRefresh==NULL)
	//	{
	//		m_listDeadMonster.erase(it);
	//		continue;
	//	}

	//	long x, y;
	//		bool bRet = GetRandomPosInRange(x, y, pRefresh->rectRange.left, pRefresh->rectRange.top, pRefresh->rectRange.right-pRefresh->rectRange.left, pRefresh->rectRange.bottom-pRefresh->rectRange.top);
	//	
	//	pMonster->SetPosXY( x+0.5f, y+0.5f );
	//	pMonster->SetDir( random(8) );
	//	pMonster->ResumeAll();
	//	pMonster->GetAI()->LoseTarget();
	//	pMonster->GetAI()->StartCycle();

	//	if(	pMonster ->GetAIType()  == CAIFactory::CITY_GUARD_WITH_SWORD ||
	//		pMonster ->GetAIType()  == CAIFactory::CITY_GUARD_WITH_BOW )
	//	{
	//			AddGurdMonster( pMonster->GetExID());
	//	}

	//	AddObject( pMonster );

	//	// 发送消息给玩家
	//	vector<BYTE>	array;
	//	pMonster->AddToByteArray( &array, false );
	//	CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE);
	//	MsgSend.Add( pMonster->GetType() );
	//	//MsgSend.Add( pMonster->GetID() );
	//	MsgSend.Add( pMonster->GetExID() );
	//	MsgSend.Add( (long)array.size() );
	//	MsgSend.AddEx( &array[0] , (long)array.size() );
	//	MsgSend.Add( (char)0 );		// 是否出现特效
	//	MsgSend.SendToAround(pMonster);


	//	g_lTotalMonster++;
	//	m_listDeadMonster.erase(it);
	//	it=m_listDeadMonster.begin();

	//}
}

void CServerRegion::SendWeatherInfo(CPlayer *pPlayer/* =NULL */)
{
	CMessage msg(MSG_S2C_RGN_CHANGEWEATHER);
	msg.Add((long)m_vectorCurWeather.size());
	for (int j=0; j<(long)m_vectorCurWeather.size(); j++)
	{
		msg.Add(m_vectorCurWeather[j].lWeatherIndex);
		msg.Add(m_vectorCurWeather[j].dwFogColor);
	}

	if (pPlayer)
	{
		msg.SendToPlayer(pPlayer->GetExID());
	}
	else
		msg.SendToRegion(this);
}

void CServerRegion::GetReLiveReturnPoint(CPlayer* pPlayer, long &lRegion, long flag/*是那一种复活方式*/, long &l, long &t, long &r, long &b, long &d)
{
	//复活点类型（flag） 0：原地复活 1：指定地图复活

	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	if(flag == 0) // 原地复活
	{
		lRegion = pPlayer->GetRegionID();
		l		= m_stSetup.lReLivePosX1;
		t		= m_stSetup.lReLivePosY1;
		r		= m_stSetup.lReLivePosX2;
		b		= m_stSetup.lReLivePosY2;
		d		= -1;
	}
	else// 指定地图复活
	{
		lRegion = m_stSetup.lReLiveRegionID;
		l		= m_stSetup.lReLivePosX1;
		t		= m_stSetup.lReLivePosY1;
		r		= m_stSetup.lReLivePosX2;
		b		= m_stSetup.lReLivePosY2;
		d		= -1;
	}
}
void CServerRegion::GetLogoutReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lLogoutRegionID;
	l		= m_stSetup.lLogoutPosX1;
	t		= m_stSetup.lLogoutPosY1;
	r		= m_stSetup.lLogoutPosX2;
	b		= m_stSetup.lLogoutPosY2;
	d		= -1;
}
void CServerRegion::GetRuneStoneReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lRuneStoneRegionID;
	l		= m_stSetup.lRuneStonePosX1;
	t		= m_stSetup.lRuneStonePosY1;
	r		= m_stSetup.lRuneStonePosX2;
	b		= m_stSetup.lRuneStonePosY2;
	d		= -1;
}
void CServerRegion::GetDupRgnReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d)
{
	if( !pPlayer )
	{
		lRegion = l = t = r = b = d = 0;
		return;
	}

	lRegion = m_stSetup.lDupRgnRegionID;
	l		= m_stSetup.lDupRgnPosX1;
	t		= m_stSetup.lDupRgnPosY1;
	r		= m_stSetup.lDupRgnPosX2;
	b		= m_stSetup.lDupRgnPosY2;
	d		= -1;
}

//处理Player进入场景
void CServerRegion::PlayerEnter(CPlayer* pPlayer,long lUseGoods)
{
	if(NULL == pPlayer)	return;
	long x=0, y=0;
	//如果出生的地方有阻挡,那么在周围找寻空格子
	if (GetBlock(pPlayer->GetTileX(), pPlayer->GetTileY()) != CRegion::BLOCK_NO)
	{
		GetRandomPosInRange(x, y, pPlayer->GetTileX(), pPlayer->GetTileY(), 3,3);

		pPlayer->SetPosition(x,y);
		pPlayer->ProduceEvent(ET_Move);
		pPlayer->ProduceEvent(ET_EventBreak);
	}

	// 如果是副本则检查副本当前人数 == 0,开启删除副本定时器
	if((eRgnType)GetRgnType() != RGN_NORMAL && (eRgnType)GetRgnType() != RGN_PERSONAL_HOUSE)
	{
		// 取消定时删除
		CancelDeleteRgnTimer();
	}

	AddObject(pPlayer);

	// 返回信息给场景上已经存在的玩家
	/*CMessage Msg(  MSG_S2C_RGN_ADDSHAPE ,pPlayer->GetExID());
	Msg.Add( pPlayer->GetType() );
	Msg.Add( pPlayer->GetExID() );
	DBWriteSet setWriteDB;
	Msg.GetDBWriteSet(setWriteDB);
	pPlayer->CodeToDataBlock_ForClient(setWriteDB,false);
	Msg.Add( (char)lUseGoods );
	Msg.SendToAround(pPlayer, pPlayer);*/

	AddShapeToAround(pPlayer);

	// 返回当前天气
	SendWeatherInfo(pPlayer);
	// 把场景上的shape信息发给新玩家
	if( pPlayer->GetArea() )
	{
		BEGIN_LOOP_AROUND_SHAPE(this, pPlayer->GetArea()->GetX(), pPlayer->GetArea()->GetY());
		{
			if( pShape->GetType() == TYPE_PLAYER && pShape->GetExID() == pPlayer -> GetExID() )
			{
			}
			else
			{
				//CMessage Msg( MSG_S2C_RGN_ADDSHAPE,pShape->GetExID() );
				//Msg.Add( pShape->GetType() );
				//Msg.Add( pShape->GetExID() );
				//DBWriteSet setWriteDB;
				//Msg.GetDBWriteSet(setWriteDB);
				//if (pShape->GetType()==TYPE_PLAYER)
				//	((CPlayer*)pShape)->CodeToDataBlock_ForClient( setWriteDB, false );
				//else if (pShape->GetType()==TYPE_GOODS)
				//	((CGoods*)pShape) -> SerializeForOldClient( setWriteDB );
				//else
				//	pShape->CodeToDataBlock( setWriteDB, false );
				//Msg.Add( (char)false );		// 是否出现特效
				//Msg.SendToPlayer(pPlayer->GetExID(),false);
				AddShapeToPlayer(pShape,pPlayer->GetExID());
			}
		}
		END_LOOP_AROUND_SHAPE;
	}

	//! 调用队伍的地图改变接口
	const CGUID &TeamID = pPlayer->GetTeamID();
	if (NULL_GUID != TeamID)
	{
		GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamID)));
		if(NULL != pTeam) pTeam->MemberMapChange(pPlayer->GetExID());
		if ( pTeam->GetLeader() == pPlayer->GetExID() 
			/*&& pPlayer->GetCurrentProgress() == CPlayer::PROGRESS_FOLLOW_LEADER */ 
			)
		{
			pTeam->SendAllMemberLeaderChangeRgn( pPlayer->GetExID(),GetID(),x,y);
		}
	}
	// 跑商系统：当玩家切换场景后，将开启刷新商品限购额定时器
	if( pPlayer->IsBusinessMan() )
	{
		GetInst( Business::CBusinessManager ).StartRefreshBuyLimitTimer( GetResourceID(), pPlayer->GetExID() );
	}

	//##当玩家完全进入场景后,通知该玩家的类.
	pPlayer -> OnEnterRegion();
	//! 发送联系人地图改变消息
	CMessage msg(MSG_S2W_LINKMAN_MAP_CHANGE);
	msg.Add(pPlayer->GetExID());
	msg.Add(pPlayer->GetRegionID());
	msg.Add((long)pPlayer->GetCurRgnType());
	msg.Add(pPlayer->GetRegionGUID());
	msg.Add(this->GetName());
	msg.Send();
}

//处理Player退出场景
void CServerRegion::PlayerExit(CPlayer* pPlayer)
{
	if(NULL == pPlayer)	return;
	//从场景中移出

	//清楚选择的目标对象	
	long lOldType = 0;
	CGUID OldGuid;
	pPlayer->GetTarget(OldGuid,lOldType);
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(FindChildObject(lOldType,OldGuid));
	if(pMoveShape)
		pMoveShape->RemoveSelectSelfPlayer(pPlayer->GetExID());
	pPlayer->SetTarget(NULL_GUID,0);	

	CGUID PlayerID = pPlayer->GetExID();
	//	RemoveObject(pPlayer);
	DeleteChildObject(pPlayer);

	m_AddShapes.erase(PlayerID);
	itAddShape it = m_AddShapes.begin();
	for(;it != m_AddShapes.end();)
	{
		tagAddShapeToClient& AddShapeClient = (*it).second;
		AddShapeClient.Players.erase(PlayerID);
		if(AddShapeClient.Players.size() == 0)
		{
			it = m_AddShapes.erase(it);
			continue;
		}
		++it;
	}
}

// 让player回城
bool CServerRegion::BackToCity(CPlayer* pPlayer)
{

	BYTE byCountry = GetCountry();

	// 新回城
	long region, l, t, r, b, d;
	region = l = t = r = b = d = -1;

	// 回国都
	region = GetCountryParam()->GetCountryMainRegionID(byCountry);

	//根据模板RgnID查找GUID
	CServerRegion* pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, region);
	if (pReturnRegion)
	{
		l = GetCountryParam()->GetCountryMainRectLeft(byCountry);
		t = GetCountryParam()->GetCountryMainRectTop(byCountry);
		r = GetCountryParam()->GetCountryMainRectRight(byCountry);
		b = GetCountryParam()->GetCountryMainRectBottom(byCountry);
	}

	// [071127 AHC REGION]
	CGUID rgnGUID;
	if(pReturnRegion)
		rgnGUID = pReturnRegion->GetExID();
	else
		rgnGUID = NULL_GUID;
	if(pPlayer->GetPkValue()>20)
	{
		long lRanX=0,lRanY=0;
		pReturnRegion->GetRandomPosInRange(lRanX,lRanY,360,318,370-360,328-318);
		pPlayer->ChangeRegion(RGN_NORMAL,rgnGUID,lRanX,lRanY,pPlayer->GetDir(),region);
	}
	else
	{
		pPlayer->ChangeRegion(RGN_NORMAL, rgnGUID, l, t, pPlayer->GetDir(), region);
	}

	return true;
}

void CServerRegion::SetGoodsProtection( CPlayer* pPlayer, CGoods* pGoods,BOOL bTeamProtected )
{
	if( pPlayer && pGoods )
	{
		CArea* pArea = pGoods -> GetArea();
		if( pArea )
		{
			pArea -> SetGoodsProtection( pPlayer, pGoods,bTeamProtected );
		}
	}
}

eGoodsPickValue CServerRegion::CanPickUpGoods( CPlayer* pPlayer, CGoods* pGoods )
{
	eGoodsPickValue bResult = eGPV_Cant;
	if( pPlayer && pGoods )
	{
		CArea* pArea = pGoods -> GetArea();
		if( pArea )
		{
			bResult = pArea -> CanPickUpGoods( pPlayer, pGoods );
		}
	}
	return bResult;
}

// 刷新地图阻挡
void CServerRegion::RefeashBlock()
{
	// 清除
	for(int i=0; i<GetWidth(); i++)
	{
		for(int j=0; j<GetHeight(); j++)
		{
			if (GetBlock(i,j)==BLOCK_SHAPE)
			{
				SetBlock(i,j,BLOCK_NO);
			}
		}
	}

	// 设置
	CMoveShape* pMoveShape = NULL;
	long num = GetAreaX() * GetAreaY();
	for(int i=0; i<num; i++)
	{
		CArea* pArea = GetArea(i);
		if( pArea )
		{
			vector<CShape*> vOut;
			pArea -> GetAllShapes( vOut );
			for( size_t k = 0; k < vOut.size(); k ++ )
			{
				pMoveShape = dynamic_cast<CMoveShape*>( vOut[k] );
				if (pMoveShape && pMoveShape->IsDied()==FALSE)
				{					
					SetBlock(pMoveShape->GetTileX(), pMoveShape->GetTileY(), BLOCK_SHAPE);
				}
			}
		}
	}
}

//查找该物品是否是禁止物品
bool CServerRegion::FindForbidGood(const char* strOriName)
{
	if(m_ForbidMakeGoods.find(strOriName) != m_ForbidMakeGoods.end())
		return true;
	return false;
}

//税收系统
//调整税率
void CServerRegion::AdjustTaxRate(CPlayer* pPlayer)
{
	if(pPlayer==NULL) return;
	if(NULL_GUID != GetOwnedCityFaction() && pPlayer->GetFactionID() != GetOwnedCityFaction()) return;

	class PlayerAdjustTaxRate:public IAsyncCaller,public IAsyncCallback
	{
	public:
		PlayerAdjustTaxRate(const CGUID& PlayerID,CServerRegion* pRegion)
			:m_PlayerID(PlayerID),m_pRegion(pRegion)
		{
		}
		//释放自己
		void Release(){	delete this; }

		void DoAsyncCall(const CGUID& guid,long lPassWord,va_list va)
		{
			long lMaxTaxRate = va_arg(va,long);
			long lCurTaxRate = va_arg(va,long);
			CPlayer* pPlayer = va_arg(va,CPlayer*);
			//CMessage msg(MSG_S2C_CITY_RequestInputTaxRate);
			//msg.Add(guid);
			//msg.Add(lPassWord);
			//msg.Add(lMaxTaxRate);
			//msg.Add(lCurTaxRate);
			//msg.SendToPlayer(pPlayer->GetExID());
		}

		void OnAsyncCallback(tagAsyncResult& Result)
		{
			if(Result.Result == AR_OK)
			{
				//long lTaxRate = va_arg(Result.Arguments,DWORD);
				//if(lTaxRate>=0 && lTaxRate<=m_pRegion->GetMaxTaxRate())
				//{
				//	m_pRegion->SetTaxRateAndUpdate(lTaxRate);

				//	DWORD dwColor = MAKELONG(MAKEWORD(254,237), MAKEWORD(218,255));
				//	char strSend[256]="";
				//	_snprintf(strSend, 256, "城主把 %s 的税率设为%d%%。",m_pRegion->GetName(),lTaxRate);

				//	GetGame()->SendNotifyMessage(strSend,m_pRegion,dwColor);

				//	


				//	//输出税收信息
				//	CPlayer* pPlayer = (CPlayer*)(m_pRegion->FindChildObject(TYPE_PLAYER,m_PlayerID));
				//	if(pPlayer)
				//	{
				//		//char strWarInfo[256]="";
				//		//_snprintf(strWarInfo,"%s 帮会的 %s 玩家把 %s 的税率设为%d%%。",pPlayer->GetFactionName().c_str(),
				//		//	pPlayer->GetName(),m_pRegion->GetName(),lTaxRate);
				//		//PutWarDebugString(strWarInfo);
				//	}
				//}
				/***********************************************************************/
				/*  zhaohang 2010-11-8 fix 剔除颜色，直接将颜色加入字符串中，客户端能自动解析*/
				/***********************************************************************/
				long lTaxRate = va_arg(Result.Arguments,DWORD);
				if(lTaxRate>=0 && lTaxRate<=m_pRegion->GetMaxTaxRate())
				{
					m_pRegion->SetTaxRateAndUpdate(lTaxRate);

					//DWORD dwColor = MAKELONG(MAKEWORD(254,237), MAKEWORD(218,255));
					char strSend[256]="";
					_snprintf(strSend, 256, "城主把 %s 的税率设为%d%%。",m_pRegion->GetName(),lTaxRate);

					GetGame()->SendNotifyMessage(strSend,m_pRegion);
					//输出税收信息
					CPlayer* pPlayer = (CPlayer*)(m_pRegion->FindChildObject(TYPE_PLAYER,m_PlayerID));
					if(pPlayer)
					{
						//char strWarInfo[256]="";
						//_snprintf(strWarInfo,"%s 帮会的 %s 玩家把 %s 的税率设为%d%%。",pPlayer->GetFactionName().c_str(),
						//	pPlayer->GetName(),m_pRegion->GetName(),lTaxRate);
						//PutWarDebugString(strWarInfo);
					}
				}
			}
		}
	private:
		CGUID m_PlayerID;
		CServerRegion* m_pRegion;
	};
	//建立一个Session
	CNetSession* pSession = GetNetSessionManager()->CreateSession(pPlayer->GetExID());
	//建立并设置异步调用接口
	PlayerAdjustTaxRate* pInstance = new PlayerAdjustTaxRate(pPlayer->GetExID(),this);
	pSession->SetCallbackHandle(pInstance);
	//赋予超时时间,并开始异步处理//等两分钟
	pSession->Beging(2000,pInstance,GetMaxTaxRate(),GetTaxRate(),pPlayer);
}

void CServerRegion::SetTaxRateAndUpdate(long lTaxRate)//得到税率
{
	if(lTaxRate < 0 || lTaxRate > GetMaxTaxRate())	return;

	if(lTaxRate != m_Param.lCurrentTaxRate)
	{
		m_Param.lCurrentTaxRate=lTaxRate;

		//CMessage msg(MSG_S2C_CITY_UpdateTaxRate);
		//msg.Add(GetID());
		//msg.Add(lTaxRate);
		//msg.SendToRegion(this);

		UpdateTaxToWorldServer();
	}
}

void CServerRegion::AddBusinessTaxMoney(long lMoney)
{
	if(m_Param.lSupRegionID > 0)
	{
		long lSupMoney  = (long)(lMoney*m_Param.lTurnInTaxRate/100.0f);
		lMoney -= lSupMoney;
		// [RGN TEST]
		CServerRegion* pSupRegion = GetGame()->FindRegion(NULL_GUID); //m_Param.lSupRegionID);
		if(pSupRegion)
		{
			pSupRegion->AddBusinessTaxMoney(lSupMoney);
		}
		else
		{			
		}
	}
	m_Param.dwBusinessTotalTax+=lMoney;
	GetGame()->UpdateSaveRegion(this);
}

void CServerRegion::AddTaxMoney(long lMoney)
{
	//是否上缴到上级场景
	if(m_Param.lSupRegionID > 0)
	{
		long lSupMoney  = (long)(lMoney*m_Param.lTurnInTaxRate/100.0f);
		lMoney -= lSupMoney;
		// [RGN TEST]
		CServerRegion* pSupRegion = GetGame()->FindRegion(NULL_GUID); //m_Param.lSupRegionID);
		if(pSupRegion)
		{
			pSupRegion->AddTaxMoney(lSupMoney);
		}
		else
		{
			/*
			CMessage msg(MSG_S2W_CITY_AddTaxMoney);
			msg.Add(m_Param.lSupRegionID);
			msg.Add(lSupMoney);
			msg.Send();*/
		}
	}
	m_Param.dwTotalTax+=lMoney;
	GetGame()->UpdateSaveRegion(this);
	/*
	if(m_Param.dwTodayTotalTax+lMoney<4000000000)
	{
	m_Param.dwTodayTotalTax+=lMoney;
	}
	else
	{
	m_Param.dwTodayTotalTax=4000000000;

	}*/
	//UpdateTaxToWorldServer();
}	//加税钱
void CServerRegion::CollectTodayTax()
{
	long lCollectTax = m_Param.dwTodayTotalTax;
	if(m_Param.dwTotalTax+ m_Param.dwTodayTotalTax < 4000000000)
	{
		m_Param.dwTotalTax+=m_Param.dwTodayTotalTax;
	}
	else
	{
		m_Param.dwTotalTax=4000000000;
	}
	m_Param.dwTodayTotalTax=0;

// 	char strTaxInfo[256];
// 	_snprintf(strTaxInfo, 256, "系统收取了 %s 场景的税金为 %d,TotalTax=%d",GetName(),lCollectTax,
// 		m_Param.dwTodayTotalTax);
// 	PutWarDebugString(strTaxInfo);
    Log4c::Trace(ROOT_MODULE,"系统收取了 %s 场景的税金为 %d,TotalTax=%d",GetName(),lCollectTax,m_Param.dwTodayTotalTax);

	UpdateTaxToWorldServer();
}

void CServerRegion::UpdateTaxToWorldServer()
{
	//CMessage msg(MSG_S2W_CITY_UpdateTaxRate);
	//msg.Add(GetID());
	//msg.Add(GetTodayTaxPayment());
	//msg.Add(GetTotalTaxPayment());
	//msg.Add(GetTaxRate());
	//msg.Send();
}

///获取税金
void CServerRegion::ObtainTaxPayment(CPlayer* pPlayer)
{
	if(pPlayer==NULL) return;
	if(NULL_GUID != GetOwnedCityFaction() && pPlayer->GetFactionID() != GetOwnedCityFaction()) return;

	class PlayerObtainTaxPayment:public IAsyncCaller,public IAsyncCallback
	{
	public:
		PlayerObtainTaxPayment(const CGUID& PlayerID,CServerRegion* pRegion)
			:m_PlayerID(PlayerID),m_pRegion(pRegion)
		{
		}
		//释放自己
		void Release(){	delete this; }

		void DoAsyncCall(const CGUID& guid,long lPassWord,va_list va)
		{
			DWORD dwTotalTax = va_arg(va,DWORD);
			CPlayer* pPlayer = va_arg(va,CPlayer*);
			dwTotalTax = min(dwTotalTax,999999999-pPlayer->GetMoney());
			//CMessage msg(MSG_S2C_CITY_RequestInputTaxPayment);
			//msg.Add(guid);
			//msg.Add(lPassWord);
			//msg.Add(dwTotalTax);
			//msg.SendToPlayer(pPlayer->GetExID());
		}

		void OnAsyncCallback(tagAsyncResult& Result)
		{
			if(Result.Result == AR_OK)
			{
				DWORD dwMoney = va_arg(Result.Arguments,DWORD);
				if(dwMoney > 0 && dwMoney <= m_pRegion->GetTotalTaxPayment())
				{
					CPlayer* pPlayer = (CPlayer*)(m_pRegion->FindChildObject(TYPE_PLAYER,m_PlayerID));
					if (pPlayer && pPlayer->GetMoney()+dwMoney<999999999)
					{
						pPlayer->SetMoney(pPlayer->GetMoney()+dwMoney,10);
						m_pRegion->SetTotalTaxPayment(m_pRegion->GetTotalTaxPayment()-dwMoney);
						m_pRegion->UpdateTaxToWorldServer();

						//输出税收信息
						//char strWarInfo[256]="";
						//_snprintf(strWarInfo,"{%s}帮会的{%s}玩家收取{%s}的税金为%d。",pPlayer->GetFactionName().c_str(),
						//	pPlayer->GetName(),m_pRegion->GetName(),dwMoney);
						//PutWarDebugString(strWarInfo);
						//CMessage msgWS(MSG_S2W_OTHER_BROADCAST);
						//msgWS.Add((long)0);
						//msgWS.Add((long)0);
						//msgWS.Add((DWORD)0xFFFFFFFF);
						//msgWS.Add((DWORD)0xFF006EE1);
						//msgWS.Add(strWarInfo);
						//msgWS.Send();
					}
				}
			}
		}
	private:
		CGUID m_PlayerID;
		CServerRegion* m_pRegion;
	};
	//建立一个Session
	CNetSession* pSession = GetNetSessionManager()->CreateSession(pPlayer->GetExID());
	//建立并设置异步调用接口
	PlayerObtainTaxPayment* pInstance = new PlayerObtainTaxPayment(pPlayer->GetExID(),this);
	pSession->SetCallbackHandle(pInstance);
	//赋予超时时间,并开始异步处理//等两分钟
	pSession->Beging(2000,pInstance,GetTotalTaxPayment(),pPlayer);
}


//剔出所有玩家到复活点
void	CServerRegion::KickOutAllPlayerToReturnPoint()
{
	CPlayer* pPlayer = NULL;
	hsGUIDIt it = m_setPlayers.begin();
	for(; it != m_setPlayers.end() ;it++)
	{
		pPlayer = GetGame() -> FindPlayer( (*it) );
		if(pPlayer)
		{
			long lX, lY;
			long region, l, t, r, b, d;

			region = l = t = r = b = d = -1;
			GetReturnPoint(pPlayer,region , l, t, r ,b ,d);

			long sx = l;
			long sy = t;
			long w = r - sx;
			long h = b - sy;

			//根据模板RgnID查找GUID
			CServerRegion* pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, region);

			if (pReturnRegion==NULL)
			{
				lX = sx + w/2;
				lY = sy + h/2;
			}
			else
				pReturnRegion->GetRandomPosInRange(lX, lY, sx, sy, w, h);

			// [071127 AHC REGION]
			CGUID rgnGUID;
			if(pReturnRegion)
				rgnGUID = pReturnRegion->GetExID();
			else
				rgnGUID = NULL_GUID;
			pPlayer->ChangeRegion(RGN_NORMAL, rgnGUID, lX, lY, pPlayer->GetDir());
		}
	}
}

//剔出所有玩家到复活点除开本城的拥有者
void	CServerRegion::KickOutAllPlayerToReturnPointExceptOwner()
{
	CPlayer* pPlayer = NULL;
	hsGUIDIt it = m_setPlayers.begin();
	for(; it != m_setPlayers.end() ;it++)
	{
		pPlayer = GetGame() -> FindPlayer( (*it) );
		if(pPlayer && (pPlayer->GetFactionID() == NULL_GUID || pPlayer->GetFactionID() != GetOwnedCityFaction()) )
		{
			long lX, lY;
			long region, l, t, r, b, d;
			region = l = t = r = b = d = -1;

			GetReturnPoint(pPlayer,region , l, t, r ,b ,d);

			long sx = l;
			long sy = t;
			long w = r - sx;
			long h = b - sy;

			//根据模板RgnID查找GUID
			CServerRegion* pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, region);
			if(pReturnRegion == NULL)
				pReturnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, region);

			if (pReturnRegion==NULL)
			{
				lX = sx + w/2;
				lY = sy + h/2;
			}
			else
				pReturnRegion->GetRandomPosInRange(lX, lY, sx, sy, w, h);

			CGUID rgnGUID;
			if(pReturnRegion)
				rgnGUID = pReturnRegion->GetExID();
			else
				rgnGUID = NULL_GUID;
			// [071127 AHC REGION]
			pPlayer->ChangeRegion(RGN_NORMAL, rgnGUID, lX, lY, pPlayer->GetDir());
		}
	}
}

//从所有玩家背包里闪出一个物品
void	CServerRegion::DelOneGoodFromAllPlayer(char* strGoodName)
{
	if(strGoodName == NULL)	return;

	CPlayer* pPlayer = NULL;
	hsGUIDIt it = m_setPlayers.begin();
	for(; it != m_setPlayers.end() ;it++)
	{
		pPlayer = GetGame() -> FindPlayer( (*it) );
		if(pPlayer)
			pPlayer->DeleteGoodsInPacket(strGoodName);
	}
}

// 添加NPC名字列表
//BYTE* CServerRegion::GetNpcNameList()
//{
//	return &m_vectorNpcNameList[0];
//}

// NPC名字列表长度包含头4个字节
//long CServerRegion::GetNpcNameListLength()
//{
//	return m_vectorNpcNameList.size();
//}

// 获取NPC数量
long CServerRegion::GetNpcNameListNum()
{
	return m_lNpcNameListNum;
}

void CServerRegion::WakeUp()
{
	m_bIsHibernate=FALSE;
	hash_map<CGUID, CMonster*, hash_guid_compare>::iterator it=	m_mMonsters.begin();
	while (it!= m_mMonsters.end())
	{
		CMonster* pMonster=(*it).second;
		if (pMonster!=NULL && pMonster->GetAIHangupType()==1 /*&& pMonster->GetAI()->IsHibernated()*/)
		{
			AI_EVENT_SENDER( pMonster->GetAI() ).Wakeup();
			//pMonster->GetAI()->WakeUp();
		}
		it++;
	}
}

//!	注册一个删除副本定时器，并返回定时器ID
long CServerRegion::RegisterDeleteRgnTimerID(long lFutureTime)
{
	//! m_PlayerTimer对象会自动释放pVar
	void* pVar =  OBJ_CREATE(tagTimerVar);
	//void* pVar = GameManager::GetInstance()->CreateBlock(sizeof(tagTimerVar));
	/*assert(NULL != pVar);*/
	((tagTimerVar*)pVar)->TimerType = ChildObjType_DeleteRgn;

	long lTimerID = GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Schedule(this, pVar, lFutureTime + timeGetTime());
	m_lRgnDelTimerID = lTimerID;
	return lTimerID;
}
//!	取消一个删除副本的Timer
void CServerRegion::CancelDeleteRgnTimer(void)
{
	void* pVar = NULL;
	GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Cancel(m_lRgnDelTimerID, (const void**)&pVar);
	m_lRgnDelTimerID = 0;
}


//!	响应删除副本定时器触发的事件
void CServerRegion::OnDeleteRgnTimeout(long lTimerID)
{
	// 发送删除消息到WS,回应后删除该Region
	GameManager::GetInstance()->GetRgnManager()->DeleteRgnMsg(NULL_GUID, (eRgnType)GetRgnType(), GetOwnerGUID(), GetExID());

}
//被删除
void CServerRegion::OnDeleteObject(CShape* pShape)
{
	/*CMessage Msg( MSG_S2C_RGN_DELSHAPE );
	Msg.Add( pShape->GetType() );
	Msg.Add( pShape->GetExID() );
	Msg.Add( (long)0 );
	Msg.SendToAround( pShape);*/

	DelShapeToAround(pShape);

	DeleteChildObject(pShape);
}
//被移出
void CServerRegion::OnRemoveObject(CShape* pShape,LONG lTime)
{
	if (pShape== NULL)
	{
		return;
	}

	if (pShape->GetType()== TYPE_MONSTER || pShape->GetType()== TYPE_COLLECTION)
	{
		/*Message Msg( MSG_S2C_RGN_DELSHAPE );
		Msg.Add( pShape->GetType() );
		Msg.Add( pShape->GetExID() );
		Msg.Add( (long)0 );
		Msg.SendToAround( pShape);*/
		DelShapeToAround(pShape);
	}
	tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);
	stDestObj*   pDestObj= new stDestObj;
	pDestObj->nDestType= pShape->GetType();
	pDestObj->DestID = pShape->GetExID();

	pTimvar->TimerType = ChildObjType_ReBornObj;
	pTimvar->pvar = pDestObj;
	long lTimerID=GameManager::GetInstance()->Schedule(this,(void*)pTimvar,timeGetTime()+lTime,0);
	if (lTimerID==-1)
	{
		SAFE_DELETE(pDestObj);
		OBJ_RELEASE(tagTimerVar, pTimvar);
		RemoveObject(pShape);
	}
	else
	{
		CArea* pArea= pShape->GetArea();
		pArea->RemoveObject(pShape);
		AddRebornTimerID(pShape->GetExID(),lTimerID);
	}
}
//! 被移出一段时间
void CServerRegion::OnRemoveSomeTime(CShape* pShape,LONG lTime)
{
	if (pShape== NULL)
	{
		return;
	}

	tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);

	pTimvar->TimerType = ChildObjType_AddObj;
	pTimvar->lvar=(long)pShape;

	long lTimerID=GameManager::GetInstance()->Schedule(this,(void*)pTimvar,timeGetTime()+lTime,0);
	//if (lTimerID==-1)
	//{
	//	//SAFE_DELETE(pDestObj);
	//	SAFE_DELETE(pTimvar);
	//	RemoveObject(pShape);
	//}
	//else
	//{
	//	RemoveObject(pShape);
	//	AddRebornTimerID(pShape->GetExID(),lTimerID);
	//}
}
//! 响应重生Object定时器触发事件
void CServerRegion::OnRebornObject(LONG lTimerID,stDestObj* pDestObj)
{
	if (pDestObj== NULL)	return;

	CBaseObject* pBaseObj= FindChildObject(pDestObj->nDestType,pDestObj->DestID);
	if (pBaseObj== NULL)
	{
		return;
	}
	DelReboreTimerID(pBaseObj->GetExID());
	int type= pBaseObj->GetType();
	switch(type)
	{
	case TYPE_MONSTER:
		{
			CMonster* pMonster= dynamic_cast<CMonster*>(pBaseObj);
			int iRefreshIndex= pMonster->GetRefeashIndex();
			CServerRegion::tagMonster* pRefresh=GetMonsterRefeash( iRefreshIndex);

			if (pRefresh==NULL)
			{
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pMonster);//CreateObject(TYPE_MONSTER, guid))
				return ;
			}

			long x, y;
			bool bRet = GetRandomPosInRange(x, y, pRefresh->rectRange.left, pRefresh->rectRange.top, pRefresh->rectRange.right-pRefresh->rectRange.left, pRefresh->rectRange.bottom-pRefresh->rectRange.top);

			// 随机选择*.monster中某编号怪物
			tagMonsterList *pMonsterProper = NULL;
			if( pRefresh->vectorMonsterList.size() > 0 )
			{
				int iIndex = random( 100 );
				for( size_t i = 0; i < pRefresh->vectorMonsterList.size(); ++ i )
				{
					if( pRefresh->vectorMonsterList[i].wOdds > iIndex )
					{
						pMonsterProper = &pRefresh->vectorMonsterList[i];
						break;
					}
				}
			}

			// 判断是重新创建monster对象，还是使用之前死亡的monster(怪物列表中必须保证有100%出生的怪物)
			if( pMonsterProper == NULL || strcmp( pMonster->GetOriginName(), pMonsterProper->strName.c_str() ) == 0 )
			{
				pMonster->SetPosXY( x+0.5f, y+0.5f );
				pMonster->SetDir( random(8) );

				StateAI::MonsterAI *ai = static_cast<StateAI::MonsterAI*>( pMonster->GetAI() );
				ai->Reset();
				pMonster->ResumeAll();
				ai->SetBornPos( x, y );
				AI_EVENT_SENDER( ai ).OnReborn();
				//CMonsterAI * pMonsterAI= dynamic_cast<CMonsterAI*>(pMonster->GetAI());
				//if (pMonsterAI!= NULL)
				//{
				//	pMonster->CleanHpBar();
				//	pMonsterAI->ResumeAll( true );
				//	pMonsterAI->RecordBornPoint(x,y);
				//	pMonsterAI->OnBoring();
				//	pMonsterAI->OnReBorn();
				//}
				ReBornObject(pMonster);



				CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pMonster->GetExID());
				MsgSend.Add( pMonster->GetType() );
				MsgSend.Add( pMonster->GetExID() );
				DBWriteSet setWriteDB;
				MsgSend.GetDBWriteSet(setWriteDB);
				pMonster->CodeToDataBlock(setWriteDB,false);
				MsgSend.Add( (char)0 );		// 是否出现特效
				MsgSend.SendToAround(pMonster);
			}
			else
			{
				// delete the died monster first
				RemoveObject( pMonster ); 
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pMonster);
				// create a new monster object 
				CMonster *pMonster = AddMonster( pMonsterProper->strName.c_str(), x, y, pRefresh->lDir );
				if( pMonster != NULL )
				{
					// 加入路径
					if( pMonsterProper->wRoadIndex > 0 )
					{
						int road_index = pMonsterProper->wRoadIndex - 1;
						if( road_index < 0 || road_index >= (long)m_vecRoadsList.size() )
						{
							Log4c::Warn(ROOT_MODULE,FormatText("GS_RGNSERVERREGION_4",pMonsterProper->strName.c_str()));// 无法为Monster “%s” 添加路径！
						}
						else
						{
							//CMonsterAI *ai = dynamic_cast<CMonsterAI*>( pMonster->GetAI() );
							//if( ai != NULL )
							//{
							//	ai->AddRoad( m_vecRoadsList[road_index] );
							//}
							StateAI::MonsterAI *ai = dynamic_cast<StateAI::MonsterAI*>( pMonster->GetAI() );
							ai->GetRunByRoad().SetRoad( m_vecRoadsList[road_index] );
						}
					}
					pMonster->SetRefeashIndex( pRefresh->lIndex );
					pRefresh->lLivingCount++;

					// 加入脚本
					if( !pMonsterProper->strScript.empty() && pMonsterProper->strScript != "0" )
					{
						pMonster->SetScriptFile( pMonsterProper->strScript.c_str() );
					}
					if( !pMonsterProper->strTalkScript.empty() && pMonsterProper->strTalkScript != "0" )
					{
						pMonster->SetTalkScriptFile( pMonsterProper->strTalkScript.c_str() );
					}

					// 集群配置处理
					if( pMonsterProper->wSign != 0 )
					{
						pMonster->SetColonyDistance( pMonsterProper->wLeaderDistance );
						CColony *pColony = GetOneColony( pMonsterProper->wSign );
						if( pColony !=0 )
						{
							WORD AddRsl = pColony->AddMember( pMonsterProper->wLeaderSign == 1, pMonster );
							if( AddRsl != -1)
							{
								pMonster->SetCollectionID( AddRsl );
							}
						}
					}
				}// if
			}//else
			return ;
		}
		break;
	case TYPE_COLLECTION:
		{
			CCollection* pCollection = dynamic_cast<CCollection*>(pBaseObj);
			int iRefreshIndex= pCollection->GetRefreshIndex();//GetRefeashIndex();
			CServerRegion::tagCollection* pRefresh=GetCollectionRefeash( iRefreshIndex);
			const CCollectionLayout* pCollectionLayout= CDeputyOccuSystem::GetCollectionLayout(pCollection->GetCollectionName().c_str());

			if (pRefresh==NULL || pCollectionLayout==NULL)
			{
				GameManager::GetInstance()->FreeObject((CBaseObject**)&pCollection);
				return ;
			}

			long x, y;
			bool bRet = GetRandomPosInRange(x, y, pRefresh->rectRange.left, pRefresh->rectRange.top, pRefresh->rectRange.right-pRefresh->rectRange.left, pRefresh->rectRange.bottom-pRefresh->rectRange.top);

			pCollection->Init(pCollectionLayout);
			pCollection->SetPosXY( x+0.5f, y+0.5f );
			pCollection->SetDir(random(8));
			pCollection->SetState(CShape::STATE_PEACE);
			pCollection->SetHP(1);

			ReBornObject(pCollection);

			// 发送消息给玩家
			//CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE,pCollection->GetExID());
			//MsgSend.Add( pCollection->GetType() );
			//MsgSend.Add( pCollection->GetExID() );
			//DBWriteSet setWriteDB;
			//MsgSend.GetDBWriteSet(setWriteDB);
			//pCollection->CodeToDataBlock(setWriteDB,false);
			//MsgSend.Add( (char)0 );		// 是否出现特效
			//MsgSend.SendToAround(pCollection);
			AddShapeToAround(pCollection);

			return ;
		}
		break;
	default:
		{
			//输出错误
		}
	}

}

void CServerRegion::RunScriptAllPlayer(char* strPath)
{
	//产生零时值，避免在脚本里执行ChangeRgn函数时,该函数陷入死循环
	hashsetGUID temptSetPlayers = m_setPlayers;
	hsGUIDIt it = temptSetPlayers.begin();
	for(;it!=temptSetPlayers.end();it++)
	{
		CPlayer* pPlayer=GetGame()->FindPlayer((*it));
		if(pPlayer)
		{
			pPlayer->PlayerRunScript(strPath);
		}
	}
}

long CServerRegion::GetDoorState(long id)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			switch((*it->second).GetState())
			{
			case CShape::STATE_FIGHT:
				return 0;//关闭状态
			case CShape::STATE_PEACE:
				return 1;//打开状态
			case CShape::STATE_DIED:
				return 2;//损毁状态
			default:
				return -1;
			}
		}
	}
	return -1;
}

bool CServerRegion::SetDoorState(long id,WORD n)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			CGate* pGate=dynamic_cast<CGate*>(it->second);
			if (pGate)
			{
				pGate->SetGate(n);
				pGate->UpdateAttributeToClient();
			}		
			break;
		}
	}
	return false;
}

int CServerRegion::GetDoorDestroySwitch(long id)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			return (*it->second).IsCanBeenAttack();
		}
	}
	return -1;
}

bool CServerRegion::SetDoorDestroySwitch(long id,bool n)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			CBuild* pBuild=it->second;
			if (pBuild)
			{
				pBuild->SetAttribute("bUnAttack",n);
				pBuild->UpdateAttributeToClient();
			}
			return true;
		}
	}
	return false;
}

long CServerRegion::GetDoorMaxHP(long id)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			return (*it->second).GetMaxHP();
		}
	}
	return -1;
}

bool CServerRegion::SetDoorMaxHP(long id,WORD n)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			CBuild* pBuild=it->second;
			if (pBuild)
			{
				if (n<pBuild->GetHP())
				{
					pBuild->SetAttribute("Hp",n);
				}
				pBuild->SetAttribute("MaxHp",n);
				pBuild->UpdateAttributeToClient();
			}
			return true;
		}
	}
	return false;
}

long CServerRegion::GetDoorHP(long id)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			string str="Hp";
			return (*it->second).GetAttribute(str);
		}
	}
	return -1;
}

bool CServerRegion::SetDoorHP(long id,WORD n)
{
	hash_map<CGUID, CBuild*>::iterator it = m_mBuild.begin();
	for (;it!=m_mBuild.end();it++)
	{
		if(id == (*it->second).GetIndex())
		{
			CShape* pShape=dynamic_cast<CShape*>( it->second );
			if (pShape)
			{
				if (pShape->GetState()==CShape::STATE_DIED)
				{
					return false;
				}
				CBuild* pBuild= it->second;
				pBuild->SetHP(n);
				pBuild->UpdateAttributeToClient();
			}
			return true;
		}
	}
	return false;
}

void CServerRegion::RegisterAddShapeTimer()
{
	tagTimerVar* pTimerVar = OBJ_CREATE(tagTimerVar);
	pTimerVar->TimerType = (eChildObjType)ChildObjType_AddShapeToClient;
	m_lAddShapeTimerID = GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Schedule(this, pTimerVar,timeGetTime()+1000,GlobalSetup::GetSetup()->lSendAddShapeMsgInteTime);
}
//添加对象到周围
void CServerRegion::AddShapeToAround(CShape* pShape)
{
	if (pShape == NULL || pShape->GetArea() == NULL)
		return;

	vector<CGUID> vTemptPlayer;
	BEGIN_LOOP_AROUND_PLAYER( this, pShape )
	{
		if( pPlayer != pShape )
			vTemptPlayer.push_back( pPlayer ->GetExID());
	}
	END_LOOP_AROUND_PLAYER
		if(vTemptPlayer.size() == 0)
			return;

	itAddShape itAddShape = m_AddShapes.find(pShape->GetExID());
	if(itAddShape == m_AddShapes.end())
	{
		tagAddShapeToClient TemptAddShape;
		TemptAddShape.lType = pShape->GetType();
		TemptAddShape.dwSendTime = timeGetTime();
		TemptAddShape.Players.clear();
		m_AddShapes.insert(pair<CGUID,CServerRegion::tagAddShapeToClient>(pShape->GetExID(),TemptAddShape));
		itAddShape = m_AddShapes.find(pShape->GetExID());
	}

	tagAddShapeToClient& AddShapeClient = (*itAddShape).second;
	vector<CGUID>::iterator it = vTemptPlayer.begin();
	for(; it != vTemptPlayer.end(); ++it)
	{
 		if(pShape->GetType() == TYPE_PET)
 		{
 			CPet* pPet = dynamic_cast<CPet*>(pShape);
 			if( pPet != NULL )
 			{
 				if (!pPet->IsVisible(TYPE_PLAYER, *it))
 					continue;
 			}
 		}

		AddShapeClient.Players.insert((*it));
	}

	if( TYPE_PET == pShape->GetType() )
	{
		AddShapeClient.Players.erase(((CMoveShape*)pShape)->GetExID());
	}
}

void CServerRegion::AddShapeToArea(CArea* pArea,CShape* pShape)
{
	if (pShape == NULL || pArea == NULL)
		return;

	hashsetGUID setPlayers;
	pArea -> FindShapes( TYPE_PLAYER, setPlayers );	
	setPlayers.erase(pShape->GetExID());
	if(setPlayers.size() == 0)
		return;

	/*if(pShape->GetType() == TYPE_MONSTER)
	{
	CMonster *pMonster = dynamic_cast<CMonster*>(pShape);
	if(pMonster && pMonster->GetNpcType() == NT_Normal)
	{
	SendAddShapeMsgToClient(pShape,setPlayers);
	return;
	}
	}*/

	itAddShape itAddShape = m_AddShapes.find(pShape->GetExID());
	if(itAddShape == m_AddShapes.end())
	{
		tagAddShapeToClient TemptAddShape;
		TemptAddShape.lType = pShape->GetType();
		TemptAddShape.dwSendTime = timeGetTime();
		TemptAddShape.Players.clear();
		m_AddShapes.insert(pair<CGUID,CServerRegion::tagAddShapeToClient>(pShape->GetExID(),TemptAddShape));
		itAddShape = m_AddShapes.find(pShape->GetExID());
	}

	tagAddShapeToClient& AddShapeClient = (*itAddShape).second;

	AddShapeClient.Players.insert( setPlayers.begin(),setPlayers.end());
}
//添加对象到指定对象
void CServerRegion::AddShapeToPlayer(CShape* pShape,const CGUID& PlayerID)
{
	if (pShape == NULL || pShape->GetExID() == PlayerID || PlayerID == NULL_GUID)
		return;

	/*if(pShape->GetType() == TYPE_MONSTER)
	{
	CMonster *pMonster = dynamic_cast<CMonster*>(pShape);
	if(pMonster && pMonster->GetNpcType() == NT_Normal)
	{
	hashsetGUID setPlayers;
	setPlayers.insert(PlayerID);
	SendAddShapeMsgToClient(pShape,setPlayers);
	return;
	}
	}*/

	itAddShape itAddShape = m_AddShapes.find(pShape->GetExID());
	if(itAddShape == m_AddShapes.end())
	{
		tagAddShapeToClient TemptAddShape;
		TemptAddShape.lType = pShape->GetType();
		TemptAddShape.dwSendTime = timeGetTime();
		TemptAddShape.Players.clear();
		m_AddShapes.insert(pair<CGUID,CServerRegion::tagAddShapeToClient>(pShape->GetExID(),TemptAddShape));
		itAddShape = m_AddShapes.find(pShape->GetExID());
	}

	tagAddShapeToClient& AddShapeClient = (*itAddShape).second;

	AddShapeClient.Players.insert(PlayerID);
}
//向周围玩家删除该对象
void CServerRegion::DelShapeToAround(CShape* pShape)
{
	if (pShape == NULL || pShape->GetArea() == NULL)
		return;

	SendPlayers vTemptPlayer;
	BEGIN_LOOP_AROUND_PLAYER( this, pShape )
	{
		if(pPlayer != pShape)
			vTemptPlayer.insert( pPlayer ->GetExID());
	}
	END_LOOP_AROUND_PLAYER

		if( TYPE_PET == pShape->GetType() )
		{
			CMoveShape* pHost = ((CMoveShape*)pShape)->GetHost(); 
			if (pHost != NULL)
			{
				vTemptPlayer.erase(pHost->GetExID());
			}
		}

		if(vTemptPlayer.size() == 0)
			return;

		itAddShape itAdd = m_AddShapes.find(pShape->GetExID());
		if(itAdd != m_AddShapes.end() )
		{
			tagAddShapeToClient& AddShapeClient = (*itAdd).second;
			itSendPlayer itSend = vTemptPlayer.begin();
			for(;itSend != vTemptPlayer.end();)
			{
				itSendPlayer itFind = AddShapeClient.Players.find((*itSend));
				if(itFind != AddShapeClient.Players.end())
				{
					AddShapeClient.Players.erase(itFind);
					itSend = vTemptPlayer.erase(itSend);
				}
				else
				{
					itSend++;
				}
			}
			if(AddShapeClient.Players.size() == 0)
				m_AddShapes.erase(itAdd);
		}

		if(vTemptPlayer.size() == 0)
			return;
		CMessage DelShapeMsg( MSG_S2C_RGN_DELSHAPE );
		DelShapeMsg.Add( pShape->GetType() );
		DelShapeMsg.Add( pShape->GetExID() );
		DelShapeMsg.Add( (long)0 );
		DelShapeMsg.SendToPlayers( vTemptPlayer );

}

void CServerRegion::DelShapeToArea(CArea* pArea,CShape* pShape)
{
	if (pShape == NULL || pArea == NULL)
		return;

	hashsetGUID setPlayers;
	pArea -> FindShapes( TYPE_PLAYER, setPlayers );	
	setPlayers.erase(pShape->GetExID());

	itAddShape itAdd = m_AddShapes.find(pShape->GetExID());
	if(itAdd != m_AddShapes.end() )
	{
		tagAddShapeToClient& AddShapeClient = (*itAdd).second;
		itSendPlayer itSend = setPlayers.begin();
		for(;itSend != setPlayers.end();)
		{
			itSendPlayer itFind = AddShapeClient.Players.find((*itSend));
			if(itFind != AddShapeClient.Players.end())
			{
				AddShapeClient.Players.erase(itFind);
				itSend = setPlayers.erase(itSend);
			}
			else
			{
				itSend++;
			}
		}
		if(AddShapeClient.Players.size() == 0)
			m_AddShapes.erase(itAdd);
	}
	if(setPlayers.size() == 0)
		return;
	CMessage DelShapeMsg( MSG_S2C_RGN_DELSHAPE );
	DelShapeMsg.Add( pShape->GetType() );
	DelShapeMsg.Add( pShape->GetExID() );
	DelShapeMsg.Add( (long)0 );
	DelShapeMsg.SendToPlayers( setPlayers );
}
//向指定客户端删除该对象
void CServerRegion::DelShapeToPlayer(CShape* pShape,const CGUID& PlayerID)
{
	if (pShape == NULL || pShape->GetExID() == PlayerID || PlayerID == NULL_GUID)
		return;

	bool bSend = true;
	itAddShape itAdd = m_AddShapes.find(pShape->GetExID());
	if(itAdd != m_AddShapes.end() )
	{
		tagAddShapeToClient& AddShapeClient = (*itAdd).second;		
		itSendPlayer itFind = AddShapeClient.Players.find(PlayerID);
		if(itFind != AddShapeClient.Players.end())
		{
			AddShapeClient.Players.erase(itFind);
			bSend = false;
		}
		if(AddShapeClient.Players.size() == 0)
			m_AddShapes.erase(itAdd);
	}
	if(!bSend)		return;

	CMessage DelShapeMsg( MSG_S2C_RGN_DELSHAPE );
	DelShapeMsg.Add( pShape->GetType() );
	DelShapeMsg.Add( pShape->GetExID() );
	DelShapeMsg.Add( (long)0 );
	DelShapeMsg.SendToPlayer(PlayerID);
}

//发送添加消息到客户端
void CServerRegion::SendAddShapeMsgToClient()
{
	if(m_AddShapes.size() == 0)	return;

	DWORD dwTime = 0xFFFFFFFF;
	CGUID guid;
	hashsetGUID OutPlayers;
	//寻找时间最早的发送
	itAddShape it = m_AddShapes.begin();
	for(;it != m_AddShapes.end();)
	{
		//给距离最近的玩家发送
		OutPlayers.clear();

		tagAddShapeToClient& AddShapeClient = (*it).second;
		CShape* pShape = dynamic_cast<CShape*>(FindChildObject(AddShapeClient.lType,(*it).first));
		if(!pShape)
		{
			it = m_AddShapes.erase(it);
			continue;
		}		
		//如果是NPC,则立即发送给客户端
		if(pShape->GetType() == TYPE_MONSTER)
		{
			CMonster *pMonster = dynamic_cast<CMonster*>(pShape);
			if(pMonster && pMonster->GetNpcType() == NT_Normal)
			{
				SendAddShapeMsgToClient(pShape,AddShapeClient.Players);
				it = m_AddShapes.erase(it);
				continue;
			}
		}
		//向距离最近的玩家立即发送消息
		GetNearestShapes(pShape,AddShapeClient.Players,OutPlayers);
		SendAddShapeMsgToClient(pShape,OutPlayers);
		if(AddShapeClient.Players.size() == 0)
		{
			it = m_AddShapes.erase(it);
			continue;
		}

		if( (*it).second.dwSendTime < dwTime )
		{
			dwTime = (*it).second.dwSendTime;
			guid = (*it).first;
		}
		++it;
	}
	//至少发送一个完整的节点
	it = m_AddShapes.find(guid);
	if(it != m_AddShapes.end())
	{
		tagAddShapeToClient& AddShapeClient = (*it).second;
		CShape* pShape = dynamic_cast<CShape*>(FindChildObject(AddShapeClient.lType,(*it).first));
		SendAddShapeMsgToClient(pShape,AddShapeClient.Players);
		m_AddShapes.erase(it);
	}
	return;
}

void CServerRegion::SendAddShapeMsgToClient(CShape* pShape,hashsetGUID& guidPlayers)
{
	if(pShape && guidPlayers.size() > 0)
	{
		long lType = pShape->GetType();
		CMessage MsgSend( MSG_S2C_RGN_ADDSHAPE);
		MsgSend.Add( lType );
		MsgSend.Add( pShape->GetExID() );
		DBWriteSet setWriteDB;
		MsgSend.GetDBWriteSet(setWriteDB);
		switch( lType )
		{
		case TYPE_PLAYER:
			{
				((CPlayer*)pShape)->CodeToDataBlock_ForClient( setWriteDB, false );
			}
			break;

		case TYPE_GOODS:
			{
				((CGoods*)pShape) -> SerializeForRgnDisplay( setWriteDB );
			}
			break;

		case TYPE_PET:
			{
				CMoveShape* pHost = ((CPet*)pShape)->GetHost();
				if ( pHost != NULL)
				{
					guidPlayers.erase(pHost->GetExID());
				}
				if (!guidPlayers.empty())
				{
					MsgSend.Add(((CPet*)pShape)->GetPetType());
					MsgSend.Add(char(0));		// 其他玩家标志位
					((CPet*)pShape)->CodeToDataBlock_ForClient(setWriteDB, false);
				}
			}
			break;

		default:
			{
				pShape->CodeToDataBlock( setWriteDB, false );
			}
			break;
		}
		MsgSend.Add( (char)0 );		// 是否出现特效
		MsgSend.SendToPlayers(guidPlayers);
	}
}
//定时器触发向客户端添加消息
long CServerRegion::OnAddShapeTimeOut(DWORD timerid)
{
	SendAddShapeMsgToClient();
	return 0;
}

//得到对象和集合中的玩家距离小于等于5格的集合
void CServerRegion::GetNearestShapes(CShape* pShape,hashsetGUID& InPlayers,hashsetGUID& OutPlayers)
{
	hsGUIDIt  it = InPlayers.begin();
	for(;it != InPlayers.end();)
	{
		CPlayer* pPlayer = GetGame()->FindPlayer((*it));
		if(!pPlayer)
		{
			it = InPlayers.erase(it);
			continue;
		}
		//如果该玩家的网络传输不阻塞并且距离小于规定格数
		if( !pPlayer->GetNetTransferStatus() && pShape->Distance(pPlayer->GetTileX(),pPlayer->GetTileY()) <= GlobalSetup::GetSetup()->lSendAddSahpeMsgDistance )
		{
			OutPlayers.insert((*it));
			it = InPlayers.erase(it);			
		}
		else
		{
			++it;
		}
	}
}
long CServerRegion::GetStrongPointState(const char* szName)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				return (*it).lState;
			}
		}
	}
	return -1;
}
void CServerRegion::SetStrongPointState(const char* szName, long lState)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				(*it).lState = lState;
				return;
			}
		}
	}
}
void CServerRegion::GetStrongPointPos(const char* szName, long& lX, long& lY)
{
	if(szName)
	{
		std::vector<tagStrongPoint>::iterator it = m_vecStrongPoint.begin();
		for(; it != m_vecStrongPoint.end(); it++)
		{
			if( strcmp( (*it).szName, szName) == 0 ) // 找到相同名字
			{
				lX = (*it).lPosX;
				lY = (*it).lPosY;
				return;
			}
		}
	}
}
long CServerRegion::GetCampPlayerNum(eCampType type)
{
	long lRet = 0;
	hsGUIDIt it = m_setPlayers.begin();
	for(; it != m_setPlayers.end(); it++)
	{
		CPlayer* pPlayer = GetGame()->FindPlayer(*it);
		if(pPlayer)
		{
			if(type == eCampUnion)
			{
				if(pPlayer->GetCountry() % 2 == type)
					lRet++;
			}
			else if(type == eCampEmpire)
			{
				if(pPlayer->GetCountry() % 2 == type)
					lRet++;
			}
		}
	}
	return lRet;
}