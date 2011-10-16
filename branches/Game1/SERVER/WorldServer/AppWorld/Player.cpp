#include "StdAfx.h"
#include ".\player.h"
#include ".\Goods\CGoods.h"
#include "goods/CGoodsFactory.h"
#include ".\setup\playerlist.h"
#include "..\..\..\nets\networld\message.h"
//#include "..\dbaccess\worlddb\rsplayer.h"

#include "..\setup\globesetup.h"
#include "WorldRegion.h"
#include ".\Container\CWallet.h"
#include "Listener/CSeekGoodsListener.h"
#include "Country\Country.h"
#include "Country\CountryParam.h"
#include "Country\CountryHandler.h"
#include "..\public\dupliregionsetup.h"

#include "DBentity/entityManager.h"

#include "GlobalRgnManager.h"
#include "..\AppWorld\NewSkill\NewSkillFactory.h"
#include "Pet.h"
#include "PlayerMan/PlayerMan.h"
#include "Linkman/LinkmanSystem.h"
#include "AntiCheat/CConfiger.h"

#include ".\setup\CountTimerList.h"
#include "organizingsystem\SentaiSystem.h"

CPlayer::tagCountTimer CPlayer::NULL_TIMER;

CPlayer::CPlayer(void)
: m_lCurrTitleType(ePTT_Hide)
,m_lFacDemiseWarning(0)
,m_lFacDisbandWarning(0)
, m_dwCountTimerID(-1)
,m_SentaiPoints(0)
,m_PenaltyPoints(0)
{
	m_CurPRgnNum = 0;
	m_CurTRgnNum = 0;

	SetType(TYPE_PLAYER);

	m_lVariableNum = 0;		// 变量个数
	m_lGuidNum = 0;            // GUIDNum
	memset(&m_Property,0,sizeof(m_Property));
	memset(&m_BaseFightProperty,0,sizeof(m_BaseFightProperty));
	memset(&m_FactFightProperty,0,sizeof(m_FactFightProperty));
	memset(&m_AppendProperty,0,sizeof(m_AppendProperty));

	m_lVariableNum = 0;			// 变量个数
	m_pVariableData = NULL;		// 变量数据
	m_lVariableDataLength = 0;	// 变量数据长度


	m_lInvestLimitTimeMark=0;
	m_cOriginPack=MP_NEW CVolumeLimitGoodsContainer();
	m_cOriginPack->SetContainerVolume(5*4);

	m_pSubpackContainer=MP_NEW CSubpackContainer();	

	m_mapGoodsIdCoolDown.clear();
	m_mapClassIdCoolDown.clear();

	m_bSaveDepot =false;
	m_lSilienceTime = 0;
	m_dwMurdererTime = 0;

	//m_dwCreateNo = 0;
	//m_dwLogoutTime = 0;

	m_lFightStateCount = 0;

	m_TeamGuid = NULL_GUID;

	m_PlayerDepot.BindPlayer(this);

	memset(m_CardPwd, 0, PASSWORD_SIZE);
	m_CardPwdThawTime = 0;
	
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_YUANBAO,list<DWORD>()));
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_WEIMIAN,list<DWORD>()));
	m_mapIncShopCur10.insert(IncShopCur10Map::value_type(CIncrementShopList::TM_TICKET,list<DWORD>()));

	m_mapLimitGoodsRecord.clear();
	m_vecTradeList.clear();

	m_GsSocket = 0;

	m_lTicket = 0;
	m_lNewMail.clear();										//新邮件列表
	m_lReceiveMail.clear();									//已收邮件列表

	m_FuncKey.clear();

	memset(m_szFactionName, 0, MAXSIZE_FactionName);
	memset(m_szUnionName, 0, MAXSIZE_FactionName);

	m_LoginTime = 0;

	m_mapQuestIndex.clear();
	m_lPersonalCredit = 0;
	m_VecLottery.clear();
	m_lPersonalCreditTotal=0;
	m_iWorldCreditVelue=0;
	m_iLotteryLevel=0;
	m_lPersonalCreditVelue=0;
}


CPlayer::~CPlayer(void)
{
	ReleaseAllPets();
	m_cEquipment.Release();			////装备的释放
	gcPacks();
	m_cWallet.Release();
	m_cGem.Release();
	ReleaseNpcShopSellGoods();
	ClearMailMap();
	M_FREE(m_pVariableData,m_lVariableDataLength);	////仓库中物品的释放
	m_PlayerDepot.Release();		//! 释放仓库对象
	m_lNewMail.clear();										//新邮件列表
	m_lReceiveMail.clear();									//已收邮件列表
	m_mapQuestIndex.clear();
		m_FuncKey.clear();
}

VOID CPlayer::gcPacks()
{
	MP_DELETE(m_cOriginPack);
	MP_DELETE(m_pSubpackContainer);
}

VOID CPlayer::PushbakNpcShopSellGoods(CGoods* pGoods,DWORD dwPrice)
{
	if(!pGoods)
		return;
	CPlayer::tagShopSelledGoods stSelledGoods;
	stSelledGoods.pGoods=pGoods;
	stSelledGoods.dwSelledPrice=dwPrice;
	if(m_listNpcShopSellGoods.size()>=10)
	{
		list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();		
		CGoodsFactory::GarbageCollect(&((*it).pGoods));
		m_listNpcShopSellGoods.erase(it);

		m_listNpcShopSellGoods.push_back(stSelledGoods);
	}
	else
	{
		m_listNpcShopSellGoods.push_back(stSelledGoods);
	}
}

CGoods* CPlayer::FetchNpcShopSellGoods(const CGUID& guid)
{
	CGoods* pGoods=NULL;
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		if(guid==((*it).pGoods)->GetExID())
		{
			pGoods=(*it).pGoods;
			m_listNpcShopSellGoods.erase(it);
			break;
		}
	}
	return pGoods;
}

CGoods* CPlayer::FindNpcShopSellGoods(CGUID& guid,DWORD* dwPrice)
{
	CGoods* pGoods=NULL;
	*dwPrice=0;
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		if(guid==((*it).pGoods)->GetExID())
		{
			pGoods=(*it).pGoods;
			*dwPrice=(*it).dwSelledPrice;			
			break;
		}
	}
	return pGoods;
}

VOID CPlayer::ReleaseNpcShopSellGoods()
{
	list<CPlayer::tagShopSelledGoods>::iterator it=m_listNpcShopSellGoods.begin();
	for(;it!=m_listNpcShopSellGoods.end();it++)
	{
		CGoods* pGoods=(*it).pGoods;

		if(pGoods)			
			CGoodsFactory::GarbageCollect(&((*it).pGoods));
	}
	m_listNpcShopSellGoods.clear();
}

hash_map<CGUID,CGoods*,hash_guid_compare>* CPlayer::GetSubPackGoodsMap(int i)
{
	CSubpackContainer::tagSubpack* pSubpack=m_pSubpackContainer->GetSubpack(i);
	if(pSubpack)
	{
		return pSubpack->pSubpackContainer->GetGoodsMap();
	}
	return NULL;
}

VOID CPlayer::setSubPackGoods(BYTE pos,CGoods** pGoods)
{
	if(pos<0 || pos>=CSubpackContainer::SUBPACK_NUM)
		return;
	m_pSubpackContainer->GetSubpack(pos)->pGoods=*pGoods;	
}

BOOL CPlayer::SerializeSubpack(vector<BYTE>* pStream,PLAYER_EXTEND_ID extendId)
{

	int nIndex=extendId-PEI_PACK1;
	if(m_pSubpackContainer->GetSubpack(nIndex)->pGoods)
	{
		_AddToByteArray(pStream,static_cast<DWORD>(extendId));
		m_pSubpackContainer->GetSubpack(nIndex)->pGoods->Serialize(pStream);
		m_pSubpackContainer->GetSubpack(nIndex)->pSubpackContainer->Serialize(pStream);
	}
	else
	{
		_AddToByteArray(pStream,static_cast<DWORD>(0));
	}
	return TRUE;
}

BOOL CPlayer::UnserializeSubpack(BYTE* pStream, LONG& lOffset)
{	
	BYTE btVal=_GetByteFromByteArray(pStream,lOffset);
	DWORD dwExtendId=_GetDwordFromByteArray(pStream,lOffset);
	PLAYER_EXTEND_ID ecColumn = static_cast<PLAYER_EXTEND_ID>(dwExtendId); 			
	if(btVal)
	{		
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,30 );		
		if( pGoods )
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(72);
#endif
			m_pSubpackContainer->Add( ecColumn, pGoods, NULL );			
			m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->SetContainerVolume(pGoods);
		}				
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->Unserialize(pStream,lOffset);
	}
	else
	{
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pGoods=NULL;
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->Clear();
	}
	return TRUE;
}


BOOL CPlayer::UnserializeSubpack(DBReadSet& setReadDB)
{
	BYTE btVal=setReadDB.GetByteFromByteArray();
	DWORD dwExtendId=setReadDB.GetDwordFromByteArray();
	PLAYER_EXTEND_ID ecColumn = static_cast<PLAYER_EXTEND_ID>(dwExtendId); 			
	if(btVal)
	{		
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,31 );		
		if( pGoods )
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(72);
#endif
			m_pSubpackContainer->Add( ecColumn, pGoods, NULL );			
			m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->SetContainerVolume(pGoods);
		}				
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->Unserialize(setReadDB);
	}
	else
	{
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pGoods=NULL;
		m_pSubpackContainer->GetSubpack(ecColumn-PEI_PACK1)->pSubpackContainer->Clear();
	}
	return TRUE;
}


// 读取 从数据库中
bool CPlayer::LoadData()
{
	this->SetEnergy(this->GetEnergy ());
	SetGraphicsID( GetSex()+1);
	return true;
}

void CPlayer::LoadDefaultProperty(BYTE lOccupation,BYTE lSex, BYTE byCountry,BYTE byConstellation)
{
	long lX=-1, lY=-1;

	// 根据国家，来设置出生地，随机在原始场景和副本中产生。
	BYTE temp = byCountry;
	long lRegionID = GetCountryParam()->m_mpStartRegions[byCountry];

	CGUID RegionID;
	CGlobalRgnManager::tagRegion* pst = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);

	long lDir = 0;
	char szLogingInfo[512]="";
	if(pst)
	{
		// 为玩家添加分线场景ID
		if(pst->pRegion && pst->pRegion->GetLinedIdFlag() > 0) // 是分线场景
		{
			map<long, vector<long>>::iterator itr = GetGame()->GetGlobalRgnManager()->GetLinedRgnIdByLinedIdMap().find(pst->pRegion->GetLinedIdFlag());
			if(itr != GetGame()->GetGlobalRgnManager()->GetLinedRgnIdByLinedIdMap().end())
			{
				unsigned int randPos = random((int)itr->second.size());
				if(randPos >= 0 && randPos < (unsigned int)itr->second.size())
					lRegionID = itr->second[randPos];
			}
		}

		long sx = GetCountryParam()->m_mpStartRect[byCountry].left;
		long sy = GetCountryParam()->m_mpStartRect[byCountry].top;
		long w = GetCountryParam()->m_mpStartRect[byCountry].right - sx;
		long h = GetCountryParam()->m_mpStartRect[byCountry].bottom - sy;
		pst->pRegion->GetRandomPosInRange(lX, lY, sx, sy, w, h);
		lDir = GetCountryParam()->m_mpStartDir[byCountry];

		_snprintf(szLogingInfo,512,"创建角色Name:%d,regionid:%d,sx:%d,sy:%d,w:%d,h:%d,posx:%d,posy:%d,dir:%d",
			GetName(),lRegionID,sx,sy,w,h,lX,lY,lDir);
		
	}
	else
	{
		_snprintf(szLogingInfo,512,"创建角色%s时,未找到场景regionid:%d",GetName(),lRegionID);
	}
	PutStringToFile("Login_WS_Info",szLogingInfo);
	
	SetRegionID(lRegionID);
	SetPosXY(lX+0.5f, lY+0.5f);
	SetDir(lDir);
	SetGraphicsID(lSex+1);
	SetRegionExID(RegionID);

	//战斗属性在gs上初始化
	memset(&m_Property,0,sizeof(tagPFPro));
		
	SetCharged(false);
	//国家
	m_Property.byCountry = byCountry;
	m_Property.byOccu = (eOccupation)lOccupation;
	m_Property.bySex = lSex;
	m_Property.wLevel = 1;
	m_Property.byConst = (eConstellation)byConstellation;
	//职业等级
	m_Property.byOccuLvl[m_Property.byOccu]=1;
	m_Property.dwHp=0xFFFFFFFF;
	m_Property.wMp=0xFFFF;
	time_t tTime;
	time(&tTime);
	m_Property.dwLastExitGameTime = (DWORD)tTime;
}

void CPlayer::AddSkill(DWORD dwID,WORD Lv,WORD wCool)
{
	tagSkill stSkill;
	stSkill.wID = dwID;
	stSkill.wLevel = Lv;
	stSkill.wCooldown = wCool;
	m_listSkillID.push_back(stSkill);
}

void CPlayer::InitSkill()
{

}

VOID CPlayer::SetCredit(DWORD dwAreaId,DWORD dwCredit)
{
	m_mapCredit[dwAreaId]=dwCredit;
}

VOID CPlayer::AddCredit(DWORD dwAreaId,long lVal)
{
	map<DWORD,DWORD>::iterator it=m_mapCredit.find(dwAreaId);
	if(it!=m_mapCredit.end())
		it->second+=lVal;
	else
		m_mapCredit[dwAreaId]=lVal;
}

DWORD CPlayer::GetCredit(DWORD dwAreaId)
{
	map<DWORD,DWORD>::iterator it=m_mapCredit.find(dwAreaId);
	if(it!=m_mapCredit.end())
		return it->second;
	return 0;
}

// 更新属性
void CPlayer::UpdateProperty()
{
	CGlobeSetup::tagSetup *pSetup = CGlobeSetup::GetSetup();
	this->SetStrenth (this->GetBaseStrenth ());
	this->SetAgility(this->GetBaseAgility());	
	this->SetConsti (this->GetBaseConsti ());	
	this->SetIntell (this->GetBaseIntell ());	
	this->SetMaxHP ( this->GetBaseMaxHP ()+ (WORD)(this->GetConsti()*pSetup->fCon2MaxHp[GetOccupation()]));	
	this->SetMaxMP ( this->GetBaseMaxMP () +(WORD)(this->GetIntell()*pSetup->fInt2MaxMp[GetOccupation()]) );	
	this->SetMaxEnergy ( this->GetBaseMaxEnergy ());	
	this->SetMinAtk ( this->GetBaseMinAtk () + (WORD)(this->GetAgility()*pSetup->fDex2MinAtk[GetOccupation()]));	
	this->SetMaxAtk ( this->GetBaseMaxAtk () + (WORD)(this->GetStrenth()*pSetup->fStr2MaxAtk[GetOccupation()]) );	
	this->SetHit ( this->GetBaseHit ());	
	this->SetDef ( this->GetBaseDef () +(WORD)(this->GetConsti()*pSetup->fCon2Def[GetOccupation()]) );	
	this->SetDodge (this->GetBaseDodge ());	
	this->SetHpRecoverSpeed ( this->GetBaseHpRecoverSpeed ());	
	this->SetMpRecoverSpeed ( this->GetBaseMpRecoverSpeed ());	
}


// 添加到CByteArray
bool CPlayer::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CMoveShape::AddToByteArray(pByteArray, bExData);

	if( bExData )
	{
		int n=(long)sizeof(tagPlayerProperty);
        _AddToByteArray(pByteArray, (BYTE*)&m_Property,n);
		_AddToByteArray(pByteArray, (char*)GetAccount());
		_AddToByteArray(pByteArray, GetTeamID());

		_AddToByteArray(pByteArray, (long)m_listSkillID.size());
		list<tagSkill>::iterator SkillIter;
		for(SkillIter=m_listSkillID.begin();SkillIter!=m_listSkillID.end();SkillIter++)
		{
			_AddToByteArray(pByteArray,static_cast<DWORD>((*SkillIter).wID));
			_AddToByteArray(pByteArray,static_cast<DWORD>((*SkillIter).wLevel));
			_AddToByteArray(pByteArray,static_cast<DWORD>((*SkillIter).wCooldown));
		}

		//##异常状态
		_AddToByteArray( pByteArray, static_cast<DWORD>( m_vExStates.size() ) );
		if( m_vExStates.size() > 0 )
		{
			_AddToByteArray( pByteArray, &m_vExStates[0], static_cast<LONG>( m_vExStates.size() ) );
		}

			

		_AddToByteArray(pByteArray,m_lInvestLimitTimeMark);
		_AddToByteArray(pByteArray,(DWORD)m_mapInvestRecord.size());
		map<DWORD,DWORD>::iterator recordIter=m_mapInvestRecord.begin();
		for(;recordIter!=m_mapInvestRecord.end();recordIter++)
		{
			_AddToByteArray(pByteArray,recordIter->first);
			_AddToByteArray(pByteArray,recordIter->second);
		}

		m_pSubpackContainer->Serialize(pByteArray,bExData);
		

		//装备的序列化
		m_cEquipment.Serialize(pByteArray,bExData);

		//! 勋章
		m_MedalContainer.AddToByteArray(*pByteArray);

		//原始背包序列化
		getOriginPack()->Serialize(pByteArray,bExData);

		_AddToByteArray(pByteArray,(DWORD)m_mapCredit.size());
		map<DWORD,DWORD>::iterator creditIter=m_mapCredit.begin();
		for(;creditIter!=m_mapCredit.end();creditIter++)
		{
			_AddToByteArray(pByteArray,creditIter->first);
			_AddToByteArray(pByteArray,creditIter->second);
		}
		
		m_cWallet.Serialize( pByteArray,bExData );
		m_cSilverWallet.Serialize(pByteArray,bExData);
		m_cGem.Serialize( pByteArray,bExData );		

		_AddToByteArray(pByteArray,m_lTicket);

		//! 仓库
		m_PlayerDepot.AddToByteArray_ForServer(pByteArray);
		//! 小精灵系统
		m_SpriteSystem.AddToByteArray(*pByteArray);
		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.AddToByteArray(*pByteArray);
		//普通变量个数
		_AddToByteArray(pByteArray, GetVariableNum());	// 数量
		//GUID变量个数
		_AddToByteArray(pByteArray, GetGuidNum());	// 数量
		_AddToByteArray(pByteArray,GetVariableDataLength());
		_AddToByteArray(pByteArray, GetVariableData(), GetVariableDataLength());

		//SilenceTime
		_AddToByteArray(pByteArray, GetSilenceTime());

		//Godmode
		if( IsGod() )
			_AddToByteArray(pByteArray, static_cast<char>(1) );
		else 
			_AddToByteArray(pByteArray, static_cast<char>(0) );

		_AddToByteArray( pByteArray, GetMurdererTime() );

		//##战斗状态时间
		_AddToByteArray( pByteArray, GetFightStateCount() );

		//添加是否登陆
		_AddToByteArray( pByteArray,(char)m_bLogin);

		//添加任务数据
		AddQuestDataToByteArray(pByteArray);
		//添加俑兵任务
		AddMerQuestDataByteArray(pByteArray);

		//添加副职业数据
		m_DOccuRecord.AddDeputyOccuDataByteArray(pByteArray);

		// 国家
		_AddToByteArray( pByteArray, this->GetCountry() );
		_AddToByteArray( pByteArray, this->GetCountryContribute() );

		//组织系统
		AddOrgSysToByteArray(pByteArray);

		// 个人副本
		_AddToByteArray(pByteArray, GetPersonalRgnGUID());
		// 个人房屋GUID
		_AddToByteArray(pByteArray, this->GetPersonalHouseRgnGUID());
		// 组队副本
		GetGame()->GetGlobalRgnManager()->RgnGuidCode(GetExID(), *pByteArray);
		// 当前个人副本数
		_AddToByteArray(pByteArray, this->GetCurPRgnNum());
		// 当前队伍副本数
		_AddToByteArray(pByteArray, this->GetCurTRgnNum());

		//! 当前称号类型
		_AddToByteArray(pByteArray, GetTitleType());
		// 是否第一次登录
		_AddToByteArray(pByteArray, (BYTE)GetFirstLogin());
	}

	return true;
}

bool CPlayer::AddOrgSysToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,GetFactionID());
	_AddToByteArray(pByteArray,GetUnionID());
	_AddToByteArray(pByteArray, m_lFacDemiseWarning);
	_AddToByteArray(pByteArray, m_lFacDisbandWarning);

	return true;
}

CMail	*CPlayer::GetMail(CGUID &guid)	
{
	map<CGUID,CMail*>::iterator it=m_mapMail.find(guid);
	if (it!=m_mapMail.end())
	{
		return (*it).second;
	}	
	return NULL;
	
}
void	CPlayer::ClearMailMap()
{
	m_mapMail.clear();
	map<CGUID,CMail*>::iterator it=m_mapMail.begin();
	for (;it!=m_mapMail.end();)
	{		
		it = m_mapMail.erase(it);
	}
}

void	CPlayer::EraseMail(const CGUID &guid)
{
	list<CGUID>::iterator it = find(m_lReceiveMail.begin(),m_lReceiveMail.end(),guid);
	if (it!=m_lReceiveMail.end())
	{
		it = m_lReceiveMail.erase(it);
	}
}
void	CPlayer::DeleteMail(list<CGUID> &lguid)
{
	
}
// 解码
bool CPlayer::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
#ifdef _RUNSTACKINFO1_
	char pszStackInfo[1024]="";
	_snprintf(pszStackInfo,1024,"CPlayer::DecordFromByteArray(Name:%s) Start", GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
	m_listSkillID.clear();
	m_vExStates.clear();
	m_mapSysMail.clear();

	CMoveShape::DecordFromByteArray(pByte, pos, bExData);

	if( bExData )
	{
		char strTemp[256];
		_GetBufferFromByteArray(pByte, pos, &m_Property, (long)sizeof(tagPPro));
		_GetStringFromByteArray(pByte, pos, strTemp);
		SetAccount(strTemp);

		_GetBufferFromByteArray(pByte, pos, m_TeamGuid);

		// skill
		long lSkillNum = _GetLongFromByteArray(pByte, pos);
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord Skill(Num:%d)",lSkillNum);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		tagSkill stSkill;
		for(int i=0; i<lSkillNum; i++)
		{			
			stSkill.wID = _GetDwordFromByteArray(pByte,pos);
			stSkill.wLevel = _GetDwordFromByteArray(pByte,pos);
			stSkill.wCooldown = _GetDwordFromByteArray(pByte,pos);
			m_listSkillID.push_back(stSkill);
		}

		//##异常状态
		DWORD dwBytes = _GetDwordFromByteArray( pByte, pos );
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord buff(Num:%d)",dwBytes);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		if( dwBytes > 0 )
		{
			SetExStates( &pByte[pos], dwBytes );
			pos += dwBytes;
		}
		/* NEW */
		m_lInvestLimitTimeMark=_GetLongFromByteArray(pByte,pos);
		DWORD dwRecordSize=_GetDwordFromByteArray(pByte,pos);
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord Invest(Num:%d)",dwRecordSize);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		m_mapInvestRecord.clear();
		for(DWORD i=0;i<dwRecordSize;i++)
		{
			DWORD dwSpotId=_GetDwordFromByteArray(pByte,pos);
			DWORD dwInvestValue=_GetDwordFromByteArray(pByte,pos);
			m_mapInvestRecord[dwSpotId]=dwInvestValue;
		}
		
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord SubpackContainer");
#endif
		m_pSubpackContainer->Unserialize(pByte,pos);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_cEquipment");
#endif
		/* NEW */
		m_cEquipment.Release();
		m_cEquipment.Unserialize(pByte,pos,bExData);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_MedalContainer");
#endif
		//! 勋章
		m_MedalContainer.Decode(pByte, pos);

		
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord getOriginPack");
#endif
		/* NEW */
		getOriginPack()->Release();
		getOriginPack()->SetContainerVolume(5*4);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(782);
#endif
		getOriginPack()->Unserialize(pByte,pos,bExData);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_mapCredit");
#endif
		m_mapCredit.clear();

		DWORD dwCreditSize=_GetDwordFromByteArray(pByte,pos);
		for(DWORD i=0;i<dwCreditSize;i++)
		{
			DWORD dwAreaId=_GetDwordFromByteArray(pByte,pos);
			DWORD dwCredit=_GetDwordFromByteArray(pByte,pos);
			m_mapCredit[dwAreaId]=dwCredit;
		}
		
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_cWallet");
#endif
		m_cWallet.Release();
		m_cWallet.Unserialize(pByte,pos,bExData);

		m_cSilverWallet.Release();
		m_cSilverWallet.Unserialize(pByte,pos,bExData);

		m_cGem.Release();
		m_cGem.Unserialize(pByte,pos,bExData);

		m_lTicket=_GetDwordFromByteArray(pByte,pos);

		//! 仓库
		m_PlayerDepot.Release();
		m_PlayerDepot.GetFormByteArray_FromServer(pByte, pos);
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_SpriteSystem");
#endif
		//! 小精灵系统
		m_SpriteSystem.GetFormByteArray(pByte, pos);
		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.GetFormByteArray(pByte, pos);
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord VariableData");
#endif
		//VarialbeList
		//普通变量个数
		long tNum =_GetLongFromByteArray(pByte, pos);
		//GUID个数
		long guidNum = _GetLongFromByteArray(pByte, pos);
		SetVariableNum(tNum);
		SetGuidNum(guidNum);
		//变量数组长度
		long varLength = _GetLongFromByteArray(pByte, pos);

		SetVariableDataLength(varLength);
		char* pData = (char*)M_ALLOC(varLength);
		SetVariableData((char*)_GetBufferFromByteArray(pByte, pos, pData, varLength));

		// Silence
		SetSilenceTime(_GetLongFromByteArray(pByte, pos));

		// GodMode
		if( _GetCharFromByteArray(pByte, pos) )
			God();
		else 
			God( false );

		//##杀人数消亡时间
		DWORD dwMurdererTime		= _GetDwordFromByteArray( pByte, pos );
		SetMurdererTime( dwMurdererTime );

		//##战斗状态时间
		LONG lFightStateTime		= _GetLongFromByteArray( pByte, pos );
		SetFightStateCount( lFightStateTime );

		m_bLogin = _GetCharFromByteArray( pByte,pos )==0?false:true;
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord QuestData");
#endif
		//得到任务数据
		DecordQuestDataFromByteArray(pByte,pos);
		//解俑兵任务
		DecordMerQuestDataFromByteArray(pByte,pos);
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord OccuRecord");
#endif
		//得到副职业数据
		m_DOccuRecord.DecordDeputyOccuDataFromByteArray(pByte,pos);

		// 国家
		this->SetCountry (_GetByteFromByteArray( pByte, pos ) );
		this->SetCountryContribute( _GetLongFromByteArray( pByte, pos ) );
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord PersonalRgn");
#endif
		CGUID tGUID;
		// 个人副本GUID
		_GetBufferFromByteArray(pByte, pos, tGUID);
		SetPersonalRgnGUID(tGUID);
		// 个人房屋GUID
		_GetBufferFromByteArray(pByte, pos, tGUID);
		SetPersonalHouseRgnGUID(tGUID);
		// 组队副本GUID
		GetGame()->GetGlobalRgnManager()->RgnGuidDecode(GetExID(), pByte, pos);

		// 当前个人副本数
		SetCurPRgnNum(_GetLongFromByteArray(pByte, pos));
		// 当前队伍副本数
		SetCurTRgnNum(_GetLongFromByteArray(pByte, pos));

		//! 当前称号类型
		SetTitleType(_GetLongFromByteArray(pByte, pos));
		// 是否第一次登录
		SetFirstLogin( (0 != _GetByteFromByteArray(pByte, pos)) );

		//邮件
		long lNum = _GetLongFromByteArray(pByte,pos);
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord Invest(Num:%d)",lNum);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif		
	}

#ifdef _RUNSTACKINFO1_
	_snprintf(pszStackInfo,1024,"CPlayer::DecordFromByteArray(Name:%s) End", GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
	return true;
}

bool CPlayer::CodeHotKeyData(DBWriteSet& setWriteDB,bool bExData)
{
	DWORD dwSize = m_FuncKey.size();
	setWriteDB.AddToByteArray((DWORD)dwSize);
	if (dwSize!=0)
	{
		MAPFKEYITER KeyIter_b = m_FuncKey.begin();
		MAPFKEYITER KeyIter_e = m_FuncKey.end();
		for (;KeyIter_b!=KeyIter_e;KeyIter_b++)
		{
			setWriteDB.AddToByteArray((DWORD)KeyIter_b->first);
			setWriteDB.AddToByteArray((BYTE*)&KeyIter_b->second,(long)sizeof(MAPKEY));
		}
	}
	return true;
}

// 解码玩家的功能热键设置数据
bool CPlayer::DecodeHotKeyData(DBReadSet& setReadDB,bool bExData)
{
	DWORD dwSize = setReadDB.GetDwordFromByteArray();
	if (dwSize!=0)
	{
		m_FuncKey.clear();
		for (DWORD i = 0;i<dwSize;i++)
		{
			FKMS index = (FKMS)setReadDB.GetDwordFromByteArray();
			MAPKEY mapkey;
			setReadDB.GetBufferFromByteArray(&mapkey,(long)sizeof(MAPKEY));
			m_FuncKey.insert(pair<FKMS,MAPKEY>(index,mapkey));
		}
	}
	return true;
}

bool CPlayer::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData )
{
	CMoveShape::CodeToDataBlock(setWriteDB, bExData);

	if( bExData )
	{
		int n=(long)sizeof(tagPlayerProperty);
        setWriteDB.AddToByteArray((BYTE*)&m_Property,n);
		setWriteDB.AddToByteArray( (char*)GetAccount());
		setWriteDB.AddToByteArray( GetTeamID());

		//m_listSkillID
		setWriteDB.AddToByteArray( (long)m_listSkillID.size());
		list<tagSkill>::iterator SkillIter;
		for(SkillIter=m_listSkillID.begin();SkillIter!=m_listSkillID.end();SkillIter++)
		{
			setWriteDB.AddToByteArray(static_cast<DWORD>((*SkillIter).wID));
			setWriteDB.AddToByteArray(static_cast<DWORD>((*SkillIter).wLevel));
			setWriteDB.AddToByteArray(static_cast<DWORD>((*SkillIter).wCooldown));
		}

		//##异常状态
		setWriteDB.AddToByteArray( static_cast<DWORD>( m_vExStates.size() ) );
		if( m_vExStates.size() > 0 )
		{
			setWriteDB.AddToByteArray( &m_vExStates[0], static_cast<LONG>( m_vExStates.size() ) );
		}

		setWriteDB.AddToByteArray(m_lInvestLimitTimeMark);
		setWriteDB.AddToByteArray((DWORD)m_mapInvestRecord.size());
		map<DWORD,DWORD>::iterator recordIter=m_mapInvestRecord.begin();
		for(;recordIter!=m_mapInvestRecord.end();recordIter++)
		{
			setWriteDB.AddToByteArray(recordIter->first);
			setWriteDB.AddToByteArray(recordIter->second);
		}
		m_pSubpackContainer->Serialize(setWriteDB);
		//装备的序列化
		m_cEquipment.Serialize(setWriteDB);
		//! 勋章
		m_MedalContainer.CodeToDataBlock(setWriteDB);

		//原始背包序列化
		getOriginPack()->Serialize(setWriteDB);

		setWriteDB.AddToByteArray((DWORD)m_mapCredit.size());
		map<DWORD,DWORD>::iterator creditIter=m_mapCredit.begin();
		for(;creditIter!=m_mapCredit.end();creditIter++)
		{
			setWriteDB.AddToByteArray(creditIter->first);
			setWriteDB.AddToByteArray(creditIter->second);
		}
		
		m_cWallet.Serialize( setWriteDB,bExData );
		m_cSilverWallet.Serialize(setWriteDB,bExData);
		m_cGem.Serialize( setWriteDB,bExData );

		//回购		
		setWriteDB.AddToByteArray((DWORD)m_listNpcShopSellGoods.size());
		list<tagShopSelledGoods>::iterator sellGoodsIter= m_listNpcShopSellGoods.begin();
		for(;sellGoodsIter!=m_listNpcShopSellGoods.end();sellGoodsIter++)
		{
			CGoods* pGoods=(*sellGoodsIter).pGoods;
			if(pGoods)			
			{
				pGoods->Serialize(setWriteDB);
				setWriteDB.AddToByteArray((*sellGoodsIter).dwSelledPrice);
			}
		}	

		//物品冷却
		setWriteDB.AddToByteArray((DWORD)m_mapClassIdCoolDown.size());
		map<DWORD,DWORD>::iterator classIdCooldownIter=m_mapClassIdCoolDown.begin();
		for(;classIdCooldownIter!=m_mapClassIdCoolDown.end();classIdCooldownIter++)
		{
			setWriteDB.AddToByteArray(classIdCooldownIter->first);
			setWriteDB.AddToByteArray(classIdCooldownIter->second);
		}

		setWriteDB.AddToByteArray((DWORD)m_mapGoodsIdCoolDown.size());
		map<DWORD,DWORD>::iterator goodsIdCooldownIter=m_mapGoodsIdCoolDown.begin();
		for(;goodsIdCooldownIter!=m_mapGoodsIdCoolDown.end();goodsIdCooldownIter++)
		{
			setWriteDB.AddToByteArray(goodsIdCooldownIter->first);
			setWriteDB.AddToByteArray(goodsIdCooldownIter->second);
		}
		setWriteDB.AddToByteArray(m_lTicket);
		//! 仓库
		m_PlayerDepot.CodeToDataBlock_ForServer(setWriteDB);

		//! 卡片密码
		setWriteDB.AddToByteArray(m_CardPwd, PASSWORD_SIZE);
		setWriteDB.AddToByteArray(m_CardPwdThawTime);

		//! 小精灵系统
		m_SpriteSystem.CodeToDataBlock(setWriteDB);
		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.CodeToDataBlock(setWriteDB);

		//普通变量个数
		setWriteDB.AddToByteArray( GetVariableNum());	// 数量
		//GUID变量个数
		setWriteDB.AddToByteArray( GetGuidNum());	// 数量
		setWriteDB.AddToByteArray(GetVariableDataLength());
		setWriteDB.AddToByteArray( GetVariableData(), GetVariableDataLength());

		//SilenceTime
		setWriteDB.AddToByteArray( GetSilenceTime());

		//Godmode
		if( IsGod() )
			setWriteDB.AddToByteArray( static_cast<char>(1) );
		else 
			setWriteDB.AddToByteArray( static_cast<char>(0) );

		setWriteDB.AddToByteArray( GetMurdererTime() );

		//##战斗状态时间
		setWriteDB.AddToByteArray( GetFightStateCount() );


		//添加是否登陆
		setWriteDB.AddToByteArray((char)m_bLogin);

		//添加任务数据
		CodeQuestDataToDataBlock(setWriteDB);
		//添加俑兵任务
		CodeMerQuestDataToDataBlock(setWriteDB);

		//添加副职业数据
		m_DOccuRecord.CodeToDataBlock(setWriteDB);
	
		// 最近购买的10种商品列表. by Fox.		2008-02-21
		size_t nSize=m_mapIncShopCur10.size();
		setWriteDB.AddToByteArray(static_cast<DWORD>(nSize));
		IncShopCur10Iter IncShopCurIt=m_mapIncShopCur10.begin();
		for(;IncShopCurIt!=m_mapIncShopCur10.end();IncShopCurIt++)
		{
			setWriteDB.AddToByteArray(static_cast<DWORD>(IncShopCurIt->second.size()));
			list<DWORD>::iterator goodsIdIt=IncShopCurIt->second.begin();
			for(;goodsIdIt!=IncShopCurIt->second.end();goodsIdIt++)
			{
				setWriteDB.AddToByteArray(*goodsIdIt);
			}
		}

		//增值商店限量购买记录
		setWriteDB.AddToByteArray((DWORD)m_mapLimitGoodsRecord.size());
		map<DWORD,DWORD>::iterator buyRecordIt=m_mapLimitGoodsRecord.begin();
		for(;buyRecordIt!=m_mapLimitGoodsRecord.end();buyRecordIt++)
		{
			setWriteDB.AddToByteArray(buyRecordIt->first);
			setWriteDB.AddToByteArray(buyRecordIt->second);
		}

		//增值商店交易全记录
		setWriteDB.AddToByteArray( (DWORD)m_vecTradeList.size() );
		CIncrementShopList::ITR_BUYLIST buyListIt = m_vecTradeList.begin();
		for ( ; buyListIt != m_vecTradeList.end(); ++buyListIt )
		{
			setWriteDB.AddToByteArray( buyListIt->strBuyTime.c_str() );
			setWriteDB.AddToByteArray( buyListIt->dwPriceNum );
			setWriteDB.AddToByteArray( buyListIt->dwPriceType );
			setWriteDB.AddToByteArray( buyListIt->dwGoodsIdx );
			setWriteDB.AddToByteArray( buyListIt->dwBuyNum );  
		}

		//系统邮件列表
		long lNum = m_mapSysMail.size();
		setWriteDB.AddToByteArray(lNum);
		map<long,long>::iterator it=m_mapSysMail.begin();
		for (;it!=m_mapSysMail.end();++it)
		{
			setWriteDB.AddToByteArray((*it).first);
			setWriteDB.AddToByteArray((*it).second);
		}
		
		// 是否第一次登录
		setWriteDB.AddToByteArray( (BYTE)GetFirstLogin());
		//组织系统
		CodeOrgSysToDataBlock(setWriteDB);

		//! 当前称号类型
		setWriteDB.AddToByteArray( GetTitleType());

		// 个人副本
		setWriteDB.AddToByteArray( GetPersonalRgnGUID());
		// 个人房屋GUID
		setWriteDB.AddToByteArray( this->GetPersonalHouseRgnGUID());
		// 当前个人副本数
		setWriteDB.AddToByteArray( this->GetCurPRgnNum());
		// 当前队伍副本数
		setWriteDB.AddToByteArray( this->GetCurTRgnNum());

		// Fox@20081225 宠物
		GetInst(CPetCtrl).CodeToDataBlock(setWriteDB, this);

		// 编码商业背包
		m_BusinessContainer.EncodeForGS( setWriteDB );

		// 保存状态
		setWriteDB.AddToByteArray((long)m_wState);

		setWriteDB.AddToByteArray((BYTE)GetShowFashion());

		// 编码反外挂相关数据 BAIYUN@20090603
		if( GetInst( AntiCheat::CConfiger).IsEnable() )
		{
		setWriteDB.AddToByteArray(&this->m_ACProperty, sizeof(AntiCheatData));
		}
		ReleaseNpcShopSellGoods();

		// 定时器
		setWriteDB.AddToByteArray((long)GetCountTimerNum());
		map<DWORD, tagCountTimer>::iterator timeritr = m_mapCountTimer.begin();
		for(; timeritr!=m_mapCountTimer.end(); timeritr++)
		{
			CPlayer::tagCountTimer& timer = timeritr->second;
			setWriteDB.AddToByteArray((char)timer.m_bCountType);
			setWriteDB.AddToByteArray((DWORD)timer.m_dwCountTime);
			setWriteDB.AddToByteArray((DWORD)timer.m_dwID);
			setWriteDB.AddToByteArray((DWORD)timer.m_dwRetTime);
			setWriteDB.AddToByteArray((DWORD)timer.m_dwStartRetTime);
			setWriteDB.AddToByteArray((DWORD)timer.m_dwTimerType);
		}

		//! 战场积分
		setWriteDB.AddToByteArray(m_SentaiPoints);
		//任务引导
		setWriteDB.AddToByteArray((DWORD)m_mapQuestIndex.size());
		map<long,CQuestIndexXml::tagQuestIndexForServer>::iterator questindexIter=m_mapQuestIndex.begin();
		for(;questindexIter!=m_mapQuestIndex.end();questindexIter++)
		{
			CQuestIndexXml::tagQuestIndexForServer& QuestIndex = questindexIter->second;
			setWriteDB.AddToByteArray((long)QuestIndex.lQuestID);
			setWriteDB.AddToByteArray((long)QuestIndex.lNumSum);
			setWriteDB.AddToByteArray((long)QuestIndex.lNumParam);
			setWriteDB.AddToByteArray((long)QuestIndex.iState);
			setWriteDB.AddToByteArray((DWORD)QuestIndex.lReSetTime);
			setWriteDB.AddToByteArray((DWORD)QuestIndex.lReSetTimeToXml);
		}
			// 功能映射热键
		CodeHotKeyData(setWriteDB);
		//彩票系统 
		setWriteDB.AddToByteArray((DWORD)m_VecLottery.size());
		vector<tagLottery>::iterator LotteryIter=m_VecLottery.begin();
		for(;LotteryIter!=m_VecLottery.end();LotteryIter++)
		{
			setWriteDB.AddToByteArray((long)LotteryIter->iFieldID);
			setWriteDB.AddToByteArray((long)LotteryIter->dwGoodsIndex);
			setWriteDB.AddToByteArray((long)LotteryIter->lConstel);
		}
		setWriteDB.AddToByteArray((long)GetPersonalCredit());
		setWriteDB.AddToByteArray((long)GetPersonalCreditTotal());
		setWriteDB.AddToByteArray((long)GetPersonalCreditVelue());
		setWriteDB.AddToByteArray((long)GetWorldCreditVelue());
		setWriteDB.AddToByteArray((long)GetLotteryLevel());
	}
	return true;
}
bool CPlayer::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
#ifdef _RUNSTACKINFO1_
	char pszStackInfo[1024]="";
	_snprintf(pszStackInfo,1024,"CPlayer::DecodeFromDataBlock(Name:%s) Start", GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
	m_listSkillID.clear();
	m_vExStates.clear();
	m_mapSysMail.clear();

	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	if( bExData )
	{
		ReleaseNpcShopSellGoods();
		char strTemp[256];
		setReadDB.GetBufferFromByteArray( &m_Property, (long)sizeof(tagPPro));
		setReadDB.GetStringFromByteArray( strTemp,256);
		SetAccount(strTemp);
		setReadDB.GetBufferFromByteArray( m_TeamGuid);

		// skill
		long lSkillNum = setReadDB.GetLongFromByteArray( );
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord Skill(Num:%d)",lSkillNum);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		tagSkill stSkill;
		for(int i=0; i<lSkillNum; i++)
		{			
			stSkill.wID = setReadDB.GetDwordFromByteArray( );
			stSkill.wLevel = setReadDB.GetDwordFromByteArray( );
			stSkill.wCooldown = setReadDB.GetDwordFromByteArray( );
			m_listSkillID.push_back(stSkill);
		}

		//##异常状态
		DWORD dwBytes = setReadDB.GetDwordFromByteArray(   );
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord buff(Num:%d)",dwBytes);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		BYTE* pTemptByte = (BYTE*)M_ALLOC(dwBytes);
		setReadDB.GetBufferFromByteArray(pTemptByte,dwBytes);
		if( dwBytes > 0 )
			SetExStates( pTemptByte, dwBytes );
		M_FREE(pTemptByte,dwBytes);

		m_lInvestLimitTimeMark=setReadDB.GetLongFromByteArray( );
		DWORD dwRecordSize=setReadDB.GetDwordFromByteArray( );
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStackInfo,1024,"Decord Invest(Num:%d)",dwRecordSize);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
		m_mapInvestRecord.clear();
		for(DWORD i=0;i<dwRecordSize;i++)
		{
			DWORD dwSpotId=setReadDB.GetDwordFromByteArray( );
			DWORD dwInvestValue=setReadDB.GetDwordFromByteArray( );
			m_mapInvestRecord[dwSpotId]=dwInvestValue;
		}
		
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord SubpackContainer");
#endif
		m_pSubpackContainer->Unserialize( setReadDB );

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_cEquipment");
#endif
		/* NEW */
		m_cEquipment.Release();
		m_cEquipment.Unserialize(setReadDB ,bExData);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_MedalContainer");
#endif
		//! 勋章
		m_MedalContainer.DecodeFromDataBlock( setReadDB);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord getOriginPack");
#endif
		/* NEW */
		getOriginPack()->Release();
		getOriginPack()->SetContainerVolume(5*4);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(782);
#endif
		getOriginPack()->Unserialize(setReadDB ,bExData);

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_mapCredit");
#endif
		m_mapCredit.clear();

		DWORD dwCreditSize=setReadDB.GetDwordFromByteArray( );
		for(DWORD i=0;i<dwCreditSize;i++)
		{
			DWORD dwAreaId=setReadDB.GetDwordFromByteArray( );
			DWORD dwCredit=setReadDB.GetDwordFromByteArray( );
			m_mapCredit[dwAreaId]=dwCredit;
		}


#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_cWallet");
#endif
		m_cWallet.Release();
		m_cWallet.Unserialize(setReadDB ,bExData);

		m_cSilverWallet.Release();
		m_cSilverWallet.Unserialize(setReadDB ,bExData);

		m_cGem.Release();
		m_cGem.Unserialize(setReadDB ,bExData);

		DWORD dwSellGoodsSize=setReadDB.GetDwordFromByteArray();
		for(DWORD i=0;i<dwSellGoodsSize;i++)
		{
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,32 );
			DWORD dwSellPrice=setReadDB.GetDwordFromByteArray();
			if( pGoods )
			{
				PushbakNpcShopSellGoods(pGoods,dwSellPrice);
			}
		}

		//物品冷却
		m_mapClassIdCoolDown.clear();
		DWORD dwCooldownSize=setReadDB.GetDwordFromByteArray();
		DWORD dwCooldownId=0,dwCooldownTime=0;
		for(DWORD i=0;i<dwCooldownSize;i++)
		{
			dwCooldownId=setReadDB.GetDwordFromByteArray();
			dwCooldownTime=setReadDB.GetDwordFromByteArray();
			m_mapClassIdCoolDown[dwCooldownId]=dwCooldownTime;
		}

		m_mapGoodsIdCoolDown.clear();
		dwCooldownSize=setReadDB.GetDwordFromByteArray();
		for(DWORD i=0;i<dwCooldownSize;i++)
		{
			dwCooldownId=setReadDB.GetDwordFromByteArray();
			dwCooldownTime=setReadDB.GetDwordFromByteArray();
			m_mapGoodsIdCoolDown[dwCooldownId]=dwCooldownTime;
		}

		m_lTicket=setReadDB.GetDwordFromByteArray( );

		//! 仓库
		m_PlayerDepot.Release();
		m_PlayerDepot.DecodeFormDataBlock_FromServer( setReadDB);

		//! 卡片密码
		setReadDB.GetBufferFromByteArray(m_CardPwd, PASSWORD_SIZE);
		m_CardPwdThawTime = setReadDB.GetDwordFromByteArray();

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord m_SpriteSystem");
#endif
		//! 小精灵系统
		m_SpriteSystem.DecodeFromDataBlock( setReadDB);
		//! 玩家需要保存在服务器的设置
		m_SetupOnServer.DecodeFromDataBlock( setReadDB );

#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord VariableData");
#endif
		//VarialbeList
		//普通变量个数
		long tNum =setReadDB.GetLongFromByteArray( );
		//GUID个数
		long guidNum = setReadDB.GetLongFromByteArray( );
		SetVariableNum(tNum);
		SetGuidNum(guidNum);
		//变量数组长度
		long varLength = setReadDB.GetLongFromByteArray( );
		char* pData = (char*)M_ALLOC(varLength);
		SetVariableData((char*)setReadDB.GetBufferFromByteArray( pData, varLength));
		SetVariableDataLength(varLength);
		// Silence
		SetSilenceTime(setReadDB.GetLongFromByteArray( ));
		// GodMode
		if( setReadDB.GetCharFromByteArray( ) )
			God();
		else 
			God( false );
		//##杀人数消亡时间
		DWORD dwMurdererTime		= setReadDB.GetDwordFromByteArray(   );
		SetMurdererTime( dwMurdererTime );

		//##战斗状态时间
		LONG lFightStateTime		= setReadDB.GetLongFromByteArray(   );
		SetFightStateCount( lFightStateTime );


		m_bLogin = setReadDB.GetCharFromByteArray(   )==0?false:true;
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord QuestData");
#endif
		//得到任务数据
		DecodeQuestDataFromDataBlock( setReadDB );
		//解俑兵任务
		DecodeMerQuestDataFromDataBlock( setReadDB );
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord OccuRecord");
#endif
		//得到副职业数据
		m_DOccuRecord.DecodeFromDataBlock( setReadDB );
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decord PersonalRgn");
#endif
		CGUID tGUID;
		// 个人副本GUID
		setReadDB.GetBufferFromByteArray( tGUID);
		SetPersonalRgnGUID(tGUID);
		// 个人房屋GUID
		setReadDB.GetBufferFromByteArray( tGUID);
		SetPersonalHouseRgnGUID(tGUID);
		// 当前个人副本数
		SetCurPRgnNum(setReadDB.GetLongFromByteArray( ));
		// 当前队伍副本数
		SetCurTRgnNum(setReadDB.GetLongFromByteArray( ));

		//-------------------------------------------------------------------------------
		// 最近购买的10种商品列表. by Fox.		2008-02-21
		DWORD dwSize = setReadDB.GetDwordFromByteArray( );
		ClearIncShopCur10();
		IncShopCur10Iter IncShopIt=m_mapIncShopCur10.begin();
		for(;IncShopIt!=m_mapIncShopCur10.end();IncShopIt++)
		{
			DWORD dwRecordSize=setReadDB.GetDwordFromByteArray();
			DWORD dwId=0;
			for (DWORD i=0;i<dwRecordSize;i++)
			{
				dwId=setReadDB.GetDwordFromByteArray();
				IncShopIt->second.push_back(dwId);
			}
		}

		//增值商店限量购买记录列表
		m_mapLimitGoodsRecord.clear();
		DWORD dwBuyRecordSize=setReadDB.GetDwordFromByteArray();
		for(DWORD i=0;i<dwBuyRecordSize;i++)
		{
			DWORD dwBuyGoodsId=setReadDB.GetDwordFromByteArray();
			DWORD dwBuyGoodsNum=setReadDB.GetDwordFromByteArray();
			m_mapLimitGoodsRecord[dwBuyGoodsId]=dwBuyGoodsNum;
		}

		//增值商店交易全记录
		m_vecTradeList.clear();
		DWORD dwTradeListNum = setReadDB.GetDwordFromByteArray();
		for ( DWORD i = 0; i < dwTradeListNum; ++i )
		{
			setReadDB.GetStringFromByteArray( strTemp, 256 );
			DWORD dwPriceNum = setReadDB.GetDwordFromByteArray();
			DWORD dwPriceType = setReadDB.GetDwordFromByteArray();
			DWORD dwGoodsIdx = setReadDB.GetDwordFromByteArray();
			DWORD dwBuyNum = setReadDB.GetDwordFromByteArray();
			CIncrementShopList::IncShopTradeLog stIncTradeElem( strTemp, dwPriceNum, dwPriceType, dwGoodsIdx, dwBuyNum );
			m_vecTradeList.push_back( stIncTradeElem );
		}

		//! 当前称号类型
		SetTitleType(setReadDB.GetLongFromByteArray( ));
		// 是否第一次登录
		SetFirstLogin( (0 != setReadDB.GetByteFromByteArray( )) );

		//系统邮件列表
		long lNum = setReadDB.GetLongFromByteArray();
		for (int i=0; i<lNum; ++i)
		{
			long lID = setReadDB.GetLongFromByteArray();
			long lTime = setReadDB.GetLongFromByteArray();
			AddSysMail(lID,lTime);
		}

		// 宠物列表
		GetInst(CPetCtrl).DecodeFromDataBlock(setReadDB, this);

		// 解码商业背包
		m_BusinessContainer.DecodeFromGS( this, setReadDB );

		// 保存状态
		m_wState = setReadDB.GetLongFromByteArray();

		SetShowFashion(setReadDB.GetByteFromByteArray());

		// 解码反外挂相关数据 BAIYUN@20090603
		if( GetInst( AntiCheat::CConfiger ).IsEnable() )
		{
			BYTE byte = setReadDB.GetByteFromByteArray();
			if(byte==1)
			{
				AntiCheatData tmpACData;
				setReadDB.GetBufferFromByteArray(&tmpACData, sizeof(AntiCheatData));
				SetACProperty(&tmpACData);
			}
		}

		// 定时器
		ClearCountTimer();
		long CountTimerNum = setReadDB.GetLongFromByteArray();
		for(int i=0; i<CountTimerNum; i++)
		{
			tagCountTimer timer;
			timer.m_bCountType = (0 != setReadDB.GetByteFromByteArray());
			timer.m_dwCountTime = setReadDB.GetDwordFromByteArray();
			timer.m_dwID = setReadDB.GetDwordFromByteArray();
			timer.m_dwRetTime = setReadDB.GetDwordFromByteArray();
			timer.m_dwStartRetTime = setReadDB.GetDwordFromByteArray();
			timer.m_dwTimerType = setReadDB.GetDwordFromByteArray();
			AddCountTimer(timer);
		}

		//! 战场积分
		m_SentaiPoints = setReadDB.GetLongFromByteArray();

		//! 任务引导
		m_mapQuestIndex.clear();
		DWORD dwQuestIndexSize=setReadDB.GetDwordFromByteArray();
#ifdef _RUNSTACKINFO1_
		_snprintf(pszStatckInfo,1024,"Decode Credit(size:%d) start.",dwCreditSize);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		for(DWORD i=0;i<dwQuestIndexSize;i++)
		{
			CQuestIndexXml::tagQuestIndexForServer QuestIndex;
			QuestIndex.lQuestID=setReadDB.GetLongFromByteArray();
			QuestIndex.lNumSum =setReadDB.GetLongFromByteArray();
			QuestIndex.lNumParam=setReadDB.GetLongFromByteArray();
			QuestIndex.iState=(int)setReadDB.GetLongFromByteArray();
			QuestIndex.lReSetTime=setReadDB.GetDwordFromByteArray();
			m_mapQuestIndex[QuestIndex.lQuestID]=QuestIndex;
		}
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Decode SubpackContainer start.");
#endif
	// 功能映射热键
		DecodeHotKeyData(setReadDB);
	// 彩票系统
		m_VecLottery.clear();
		DWORD dwLotterySize=setReadDB.GetDwordFromByteArray();
		for(int i=0;i<dwLotterySize;i++)
		{
			tagLottery tagLottery;
			tagLottery.iFieldID = setReadDB.GetLongFromByteArray();
			tagLottery.dwGoodsIndex = setReadDB.GetLongFromByteArray();
			tagLottery.lConstel = setReadDB.GetLongFromByteArray();
			m_VecLottery.push_back(tagLottery);
		}
        SetPersonalCredit(setReadDB.GetLongFromByteArray());
		SetPersonalCreditTotal(setReadDB.GetLongFromByteArray());
		SetPersonalCreditVelue(setReadDB.GetLongFromByteArray());
		SetWorldCreditVelue(setReadDB.GetLongFromByteArray());
		SetLotteryLevel(setReadDB.GetLongFromByteArray());
	}
#ifdef _RUNSTACKINFO1_
	_snprintf(pszStackInfo,1024,"CPlayer::DecodeFromDataBlock(Name:%s) End", GetName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif
	return true;
}
bool CPlayer::CodeOrgSysToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(GetTeamID());
	setWriteDB.AddToByteArray(GetFactionID());
	setWriteDB.AddToByteArray(GetUnionID());
	setWriteDB.AddToByteArray(m_lFacDemiseWarning);
	setWriteDB.AddToByteArray(m_lFacDisbandWarning);
	return true;
}


//检查在背包中是否存在某个名字的物品
long CPlayer::CheckGoodsInPacket(char* strGoodName)
{
	long lRet = 0;
	if(strGoodName == NULL)
		return lRet;
	CSeekGoodsListener lSeeker;
	lSeeker.SetTarget( strGoodName );

	getOriginPack()->TraversingContainer( &lSeeker );
	for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>(getOriginPack()->Find(TYPE_GOODS, lSeeker.m_vGoodsID[i]) );
		if( pGoods )
		{
			lRet += pGoods -> GetAmount();
		}
	}
	return lRet;
}


//添加新邮件列表
void	CPlayer::AddMailList(CGUID &MailGuid)
{
	list<CGUID>::iterator it = find(m_lNewMail.begin(),m_lNewMail.end(),MailGuid);
	if (it == m_lNewMail.end())
	{
		m_lNewMail.push_back(MailGuid);
	}
}

//获取邮件列表
list<CGUID>      &CPlayer::GetNewMailList()
{
	return m_lNewMail;
}


//添加已收邮件列表
void	CPlayer::AddReceiveMailList(CGUID MailGuid)	
{
	list<CGUID>::iterator it = find(m_lReceiveMail.begin(),m_lReceiveMail.end(),MailGuid);
	if (it == m_lReceiveMail.end())
	{
		m_lReceiveMail.push_back(MailGuid);
	}
}

//获取已收邮件列表
list<CGUID>		&CPlayer::GetReceiveMailList()
{
	return m_lReceiveMail;
}

DWORD CPlayer::GetMoney()
{
	return m_cWallet.GetGoldCoinsAmount();
}

DWORD CPlayer::GetGem()
{
	return m_cGem.GetGoldCoinsAmount();
}

DWORD   CPlayer::GetSilver()
{
	return m_cSilverWallet.GetSilverCoinsAmount();
}

void	CPlayer::AddSysMail(long lID,long lEndTime)
{
	map<long,long>::iterator it = m_mapSysMail.find(lID);
	if (it==m_mapSysMail.end())
	{
		m_mapSysMail[lID] = lEndTime;
	}
}

map<long,long>  &CPlayer::GetSysMail()
{
	return m_mapSysMail;
}
//给客户端发信

//添加任务数据到ByteArray
BOOL CPlayer::AddQuestDataToByteArray(vector<BYTE>* pByteArray)
{
	return m_RoleQueue.AddQuestDataByteArray(pByteArray);
}

BOOL CPlayer::DecordQuestDataFromByteArray(BYTE* pByte, long& pos)
{
	return m_RoleQueue.DecordQuestDataFromByteArray(pByte,pos);
}

BOOL CPlayer::CodeQuestDataToDataBlock(DBWriteSet& setWriteDB)
{
	return m_RoleQueue.CodeToDataBlock(setWriteDB);
}
BOOL CPlayer::DecodeQuestDataFromDataBlock(DBReadSet& setReadDB)
{
	return m_RoleQueue.DecodeFromDataBlock(setReadDB);
}
//得到一个任务的状态(0:存在但没完成,1:存在且完成,2:没有)
long CPlayer::GetQuestState(WORD wQuestID)
{
	if(m_RoleQueue.GetRoleFinishedRecord(wQuestID))
	{
		return 1;
	}
	else if (m_RoleQueue.GetRecordByID(wQuestID)!=0)
	{
		return 0;
	}
	else 
	{
		return 0;
	}
}

//从数据库添加一个任务记录
void CPlayer::AddFinishedQuestFromDB(WORD wQuestID)
{
	m_RoleQueue.AddFinishedRoleRecord(wQuestID);

}

BOOL CPlayer::AddQuestFromDB(CRoleRecord* rolerecord)
{
	return m_RoleQueue.AddOneRecord(rolerecord);
}

//添加俑兵任务数据到ByteArray
bool CPlayer::AddMerQuestDataByteArray(vector<BYTE>* pByteArray)
{
	return m_MerQuestSys.AddDataByteArray(pByteArray);
}

//解俑兵任务数据
bool CPlayer::DecordMerQuestDataFromByteArray(BYTE* pByte, long& pos)
{
	return m_MerQuestSys.DecordDataFromByteArray(pByte,pos);
}

bool CPlayer::CodeMerQuestDataToDataBlock(DBWriteSet& setWriteDB)
{
	return m_MerQuestSys.CodeToDataBlock(setWriteDB);
}
bool CPlayer::DecodeMerQuestDataFromDataBlock(DBReadSet& setReadDB)
{
	return m_MerQuestSys.DecodeFromDataBlock(setReadDB);
}

// 更改名字
int CPlayer::ChangeName(const char *szNewName)
{
	if(!szNewName)
		return 1;	// 非法玩家名

	// 检查该玩家是否允许更名 出配置文件读取更名特殊符号，只要含有该符号，则允许更名
	const char *res = strstr( GetName(), CGlobeSetup::GetSetup()->strSpeStr );
	if( !res ) 
		return 2;	// 玩家不能使用该功能，因为该玩家名字没含有该更名字符

	if( !GetGame()->CheckInvalidString(string(szNewName)) ) // 无名对象，只做效验名字是否合法。
		return 3;	// 玩家新名字不合法，失败。

	if( GetGame()->IsNameExistInMapPlayer(szNewName) )
		return 4;	// 在线玩家使用了该NAME，更名失败

	SetName(szNewName);

	return 0; // 更名成功
}

int CPlayer::ChangeCountry(BYTE c)
{
	if( this->GetCountry()==c )
	{
		return -1; // 非法判国
	}

	// 如果没帮会，才可以判国
	if(NULL_GUID != GetFactionID())
	{
		return -3; // 先退出帮派
	}

	if( GetCountryHandler()->GetCountry(GetCountry())->IsWar() )
	{
		return -4; // 国站中
	}

	// 更改国是否存在
	if( !GetCountryHandler()->GetCountry(c) )
	{

		return -5; // 目的国非法
	}

	SetCountry(c);	// 更改国家ID
	// 更新到GameServer
	////////////
	return c;
}

//根据ID得到物品 
CGoods* CPlayer::GetGoodsById(const CGUID& guid)
{	

	CGoods* pGoods = (CGoods*)getOriginPack()->Find(TYPE_GOODS,guid);
	if(pGoods != NULL)
		return pGoods;

	pGoods = (CGoods*)m_cEquipment.Find(TYPE_GOODS,guid);
	if(pGoods != NULL)
		return pGoods;


	pGoods = (CGoods*)m_cWallet.Find(TYPE_GOODS,guid);
	if(pGoods != NULL)
		return pGoods;

	pGoods = (CGoods*)m_cGem.Find(TYPE_GOODS,guid);
	if(pGoods != NULL)
		return pGoods;
	return NULL;

}

const CGUID& CPlayer::GetPersonalRgnGUID(void)
{
	return m_PersonalRgnGUID;
}
const CGUID& CPlayer::GetPersonalHouseRgnGUID(void)
{
	return m_PersonalHouseRgnGUID;
}
void CPlayer::SetPersonalRgnGUID(const CGUID& guid)
{
	m_PersonalRgnGUID = guid;
}
void CPlayer::SetPersonalHouseRgnGUID(const CGUID& guid)
{
	m_PersonalHouseRgnGUID = guid;
}


BOOL CPlayer::UnserializeSubDepot(BYTE* pStream, LONG& lOffset)
{	
	CSubpackContainer *pContainer = (CSubpackContainer*)m_PlayerDepot.FindContainer(eDCT_Secondary);
	assert(NULL != pContainer);
	if(NULL == pContainer) return FALSE;

	BYTE btVal=_GetByteFromByteArray(pStream,lOffset);
	LONG eContainerID=_GetDwordFromByteArray(pStream,lOffset);

	if(btVal)
	{		
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,33 );		
		if( pGoods )
		{
			pContainer->Add( eContainerID - eDCT_Secondary1 + 10001, pGoods, NULL );			
			pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->SetContainerVolume(pGoods);
		}				
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->Unserialize(pStream,lOffset);
	}
	else
	{
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pGoods=NULL;
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->Clear();
	}
	return TRUE;
}

BOOL CPlayer::UnserializeSubDepot(DBReadSet& setReadDB)
{
	CSubpackContainer *pContainer = (CSubpackContainer*)m_PlayerDepot.FindContainer(eDCT_Secondary);
	assert(NULL != pContainer);
	if(NULL == pContainer) return FALSE;

	BYTE btVal=setReadDB.GetByteFromByteArray();
	LONG eContainerID=setReadDB.GetDwordFromByteArray();

	if(btVal)
	{		
		pContainer->Release(eContainerID - eDCT_Secondary1);

		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,34);		
		if( pGoods )
		{
			pContainer->Add( eContainerID - eDCT_Secondary1 + 10001, pGoods, NULL );			
			pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->SetContainerVolume(pGoods);
		}				
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->Unserialize(setReadDB);
	}
	else
	{
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pGoods=NULL;
		pContainer->GetSubpack(eContainerID - eDCT_Secondary1)->pSubpackContainer->Clear();
	}
	return TRUE;
}

BOOL CPlayer::SerializeSubDepot(vector<BYTE>* pStream, LONG eContainerID)
{
	CSubpackContainer *pContainer = (CSubpackContainer*)m_PlayerDepot.FindContainer(eContainerID);
	assert(NULL != pContainer);
	if(NULL == pContainer) return FALSE;

	LONG lIndex = eContainerID - eDCT_Secondary1;
	if(pContainer->GetSubpack(lIndex)->pGoods)
	{
		_AddToByteArray(pStream,static_cast<DWORD>(eContainerID));
		pContainer->GetSubpack(lIndex)->pGoods->Serialize(pStream);
		pContainer->GetSubpack(lIndex)->pSubpackContainer->Serialize(pStream);
	}
	else
	{
		_AddToByteArray(pStream,static_cast<DWORD>(0));
	}
	return TRUE;
}


void CPlayer::SetFactionID(const CGUID& FactionID)
{
	m_FactionID=FactionID;
	if(NULL_GUID == m_FactionID)
	{
		memset(m_szFactionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(m_FactionID);
		assert(NULL != pFaction);//! 这里断言失败，说明过早调用了SetFactionID()接口，应用SetFactionInfo()代替
		if(NULL != pFaction)
			memmove(m_szFactionName, pFaction->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_FACTION, this);
}

void CPlayer::SetFactionInfo(CFaction *pFaction)
{
	if(NULL == pFaction)
	{
		m_FactionID = NULL_GUID;
		memset(m_szFactionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		m_FactionID = pFaction->GetExID();
		memmove(m_szFactionName, pFaction->GetName(), MAXSIZE_FactionName - 1);
	}	

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_FACTION, this);
}

void CPlayer::SetUnionID(const CGUID& UnionID)
{
	m_UnionID=UnionID;
	if(NULL_GUID == m_UnionID)
	{
		memset(m_szUnionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		CUnion *pUnion = GetOrganiCtrl()->GetUnionOrganizing(m_UnionID);
		assert(NULL != pUnion);//! 这里断言失败，说明过早调用了SetUnionID()接口，应用SetUnionInfo()代替
		if(NULL != pUnion)
			memmove(m_szUnionName, pUnion->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_UNION, this);
}

void CPlayer::SetUnionInfo(CUnion *pUnion)
{
	if(NULL == pUnion)
	{
		m_UnionID = NULL_GUID;
		memset(m_szUnionName, 0, MAXSIZE_FactionName);
	}
	else
	{
		m_UnionID = pUnion->GetExID();
		memmove(m_szUnionName, pUnion->GetName(), MAXSIZE_FactionName - 1);
	}

	// Fox@20081113 刷新角色帮会名称
	GetInst(CPlayerMan).RefreshElem(PET_UNION, this);
}

CPet* CPlayer::CreatePet(const CGUID& rID)
{
	CPet* pPet = MP_NEW CPet;
	if( NULL_GUID !=  rID )
	{
		pPet->SetExID(rID);
		m_Pets[rID] = pPet;
	}

	return pPet;
}

void CPlayer::AddPet(CPet* pPet)
{
	m_Pets[pPet->GetExID()] = pPet;
}

void CPlayer::DelPet(const CGUID& rID)
{
	ITR_PET itr = m_Pets.find(rID);
	if( itr != m_Pets.end() )
	{
		DelPet(itr);
	}
}

void CPlayer::DelPet(ITR_PET& itr)
{
	MP_DELETE(itr->second);
	m_Pets.erase(itr);
}

void CPlayer::ReleaseAllPets(void)
{
	ITR_PET itr = m_Pets.begin();
	while( itr != m_Pets.end() )
	{
		DelPet(itr++);
	}
	m_Pets.clear();
}


void CPlayer::ClearIncShopCur10()
{
	IncShopCur10Iter it=m_mapIncShopCur10.begin();
	for(;it!=m_mapIncShopCur10.end();it++)
	{
		it->second.clear();
	}
}

//(增值商店限量)添加记录
void CPlayer::AddLimitGoodsRecord(DWORD dwGoodsId,DWORD dwGoodsNum)
{
	map<DWORD,DWORD>::iterator recordIt=m_mapLimitGoodsRecord.find(dwGoodsId);
	if(recordIt==m_mapLimitGoodsRecord.end())
		m_mapLimitGoodsRecord[dwGoodsId]=dwGoodsNum;
	else	
		recordIt->second+=dwGoodsNum;	
}

//增值商店添加交易记录
void CPlayer::AddIncTradeRecord( CIncrementShopList::IncShopTradeLog stIncTradeElem )
{
	m_vecTradeList.push_back( stIncTradeElem );
}
void CPlayer::AddCountTimer(CPlayer::tagCountTimer& timer)
{
	if(m_mapCountTimer.size()<CCountTimerList::GetMaxTimerNum())
	{
		m_mapCountTimer[timer.m_dwID] = timer;
	}
}
CPlayer::tagCountTimer& CPlayer::GetCountTimer(DWORD id)
{
	map<DWORD, tagCountTimer>::iterator it = m_mapCountTimer.find(id);
	if(it!=m_mapCountTimer.end())
		return it->second;
	return NULL_TIMER;
}
void CPlayer::DelCountTimer(DWORD id)
{
	map<DWORD, tagCountTimer>::iterator it = m_mapCountTimer.find(id);
	if(it != m_mapCountTimer.end())
	{
		m_mapCountTimer.erase(it);
	}
}
DWORD CPlayer::GetCountTimerNum(void)
{
	return m_mapCountTimer.size();
}

void CPlayer::ClearCountTimer(void)
{
	m_mapCountTimer.clear();
}
DWORD CPlayer::GetCurCountTimerID(void)
{
	return m_dwCountTimerID;
}

map<DWORD, CPlayer::tagCountTimer>& CPlayer::GetTimerMap(void)
{
	return m_mapCountTimer;
}

void CPlayer::SetSentaiPoints(long Points, bool SendMsg)
{
	if(m_SentaiPoints != Points)
	{
		//! 正在排队的不能修改积分
		if(GetInst(CSentaiSystem).IsQueuing(GetTeamID()))
			return;

		if(SendMsg)
		{
			WSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(GetTeamID());
			if (NULL != pTeam)
			{
				pTeam->OnMebereChangePoint(GetExID(), Points - m_SentaiPoints);
			}
		}

		m_SentaiPoints = Points;

		if(SendMsg)
		{
			CMessage msg(MSG_W2S_SENTAI_UPDATE_POINT);
			msg.Add(GetExID());
			msg.Add(m_SentaiPoints);
			msg.SendToSocket(GetGsSocket());
		}
	}
}

void CPlayer::AddQuestIndex(CQuestIndexXml::tagQuestIndexForServer tagQuestIndex)
{
	m_mapQuestIndex[tagQuestIndex.lQuestID]=tagQuestIndex;
}