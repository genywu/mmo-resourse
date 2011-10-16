#include "StdAfx.h"
#include "Union.h"

#include "OrganizingCtrl.h"
#include "../DBEntity/EntityManager.h"

//! 用于从DB数据构造以存在的对象
CUnion::CUnion(void)
:m_dwDataChanged(eFDCF_All)
{

}
//! 用于游戏中玩家创建新的对象
CUnion::CUnion(const CGUID &Guid, const char *pName, CFaction *pMaster)
:m_dwDataChanged(eFDCF_All)
{
	if(NULL == pName)
		return;
	SetExID(Guid);
	SetName(pName);

	m_UnionBaseData.CreateTime = (long)time(NULL); //! 创建时间
	if(NULL == pMaster)
		throw;
	m_UnionBaseData.lCountryID		= pMaster->GetCountry();	//! 所属国家
	m_UnionBaseData.MasterGuid		= pMaster->GetMasterID();	//! 盟主ID
	m_UnionBaseData.MasterFacGuid	= pMaster->GetExID();		//! 盟主所在工会
	
	LONG lSize = (long)strlen(pMaster->GetMasterName());
	lSize = (MAXSIZE_FactionName - 1 >= lSize ) ? lSize : (MAX_MEMBER_NAME_SIZE - 1);
	memmove(m_UnionBaseData.szMasterName, pMaster->GetMasterName(), lSize);

	AddNewMember(pMaster, TRUE);
}
//! 
CUnion::~CUnion(void)
{

}

//! 第一次保存
void CUnion::Insert(VOID)
{
	CEntityGroup* pTmpUnion = NULL;//GetGame().GetEntityManager()->NewBaseEntity(COM_FACTION, NULL_GUID,COM_FACTION, NULL_GUID);
	CEntityManager::DBEntityComponentMapItr facMapItr = GetGame().GetEntityManager()->GetBaseEntityMap().find(string("[union_group]"));
	if(facMapItr != GetGame().GetEntityManager()->GetBaseEntityMap().end())
	{
		if(facMapItr->second.size() == 0)
		{
			Log4c::Warn(ROOT_MODULE, GetText("WS_UNION_10"));
			return;
		}
		CEntityGroup* pUnionGroup = (CEntityGroup*)(facMapItr->second.begin()->second);

		pTmpUnion = (CEntityGroup*)pUnionGroup->FindEntityBytravelTree(GetExID());
		if(pTmpUnion == NULL)
		{
			pTmpUnion = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[union]"), GetExID());
			pUnionGroup->AddChild(pTmpUnion);
		}
	}
	if(pTmpUnion)
	{
		GetGame().GetEntityManager()->CUnionToCDBUnion(this, pTmpUnion);
		GetGame().GetEntityManager()->CreateInsertUnionSession(pTmpUnion);
	}
}
//! 保存
void CUnion::Save(VOID)
{
	if(eFDCF_Not == m_dwDataChanged) 
		return;
	CEntityGroup* pTmpUnion = NULL;//GetGame().GetEntityManager()->NewBaseEntity(COM_FACTION, NULL_GUID,COM_FACTION, NULL_GUID);
	CEntityManager::DBEntityComponentMapItr facMapItr = GetGame().GetEntityManager()->GetBaseEntityMap().find(string("[union_group]"));
	if(facMapItr != GetGame().GetEntityManager()->GetBaseEntityMap().end())
	{
		if(facMapItr->second.size() == 0)
		{
			Log4c::Warn(ROOT_MODULE, GetText("WS_UNION_11"));
			return;
		}
		CEntityGroup* pUnionGroup = (CEntityGroup*)(facMapItr->second.begin()->second);

		pTmpUnion = (CEntityGroup*)pUnionGroup->FindEntityBytravelTree(GetExID());
		if(pTmpUnion == NULL)
		{
			pTmpUnion = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[union]"), GetExID());
			pUnionGroup->AddChild(pTmpUnion);
		}
	}
	if(pTmpUnion)
	{
		GetGame().GetEntityManager()->CUnionToCDBUnion(this, pTmpUnion);
		GetGame().GetEntityManager()->CreateSaveUnionSession(pTmpUnion);
		SAFE_DELETE(pTmpUnion);
		//m_dwDataChanged = eFDCF_Not;
	}
}
//! 编码
BOOL CUnion::AddToByteArray(vector<BYTE>& ByteArray)
{
	//! 基本资料
	_AddToByteArray(&ByteArray, &m_UnionBaseData, sizeof(tagUnionBaseData));

	//! 成员列表
	_AddToByteArray(&ByteArray, (LONG)m_mapUnionMemInfo.size());
	for (map<CGUID, tagUnionMemInfo>::iterator ite = m_mapUnionMemInfo.begin(); ite != m_mapUnionMemInfo.end(); ++ite)
	{
		_AddToByteArray(&ByteArray, &(ite->second), sizeof(tagUnionMemInfo));
	}
					;
	return TRUE;
}
//! 解码
BOOL CUnion::Decode(BYTE* pData, LONG &lPos)
{
	throw;
	return TRUE;
}

//! 得到组织ID
const CGUID&	CUnion::GetExID()const
{ 
	return m_UnionBaseData.guid; 
}
//! 设置组织ID
const CGUID&	CUnion::SetExID(const CGUID& guid) 
{ 
	return m_UnionBaseData.guid = guid; 
}
//! 得到组织的名字
const char* CUnion::GetName()const
{
	return m_UnionBaseData.szName;
}
//! 设置帮会的名字
void CUnion::SetName(const char* pName)
{
	string str("");
	str.c_str();
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (MAXSIZE_FactionName - 1 >= lSize ) ? lSize : (MAXSIZE_FactionName - 1);
		memmove(m_UnionBaseData.szName, pName, lSize);
	}
}

//! 得到盟主ID
const CGUID&	CUnion::GetMasterID()const
{ 
	return m_UnionBaseData.MasterGuid; 
}
//! 设置盟主ID
const CGUID&	CUnion::SetMasterID(const CGUID& guid) 
{ 
	return m_UnionBaseData.MasterGuid = guid; 
}
//! 得到盟主的名字
const char* CUnion::GetMasterName()const
{
	return m_UnionBaseData.szMasterName;
}
//! 设置盟主的名字
void CUnion::SetMasterName(const char* pName)
{
	string str("");
	str.c_str();
	if (NULL != pName)
	{
		LONG lSize = (long)strlen(pName);
		lSize = (MAX_MEMBER_NAME_SIZE - 1 >= lSize ) ? lSize : (MAX_MEMBER_NAME_SIZE - 1);
		memmove(m_UnionBaseData.szMasterName, pName, lSize);
	}
}


//! 添加新成员
BOOL CUnion::AddNewMember(CFaction *pMember, BOOL bIsMaster)
{
	if(NULL == pMember)
		return FALSE;

	assert(m_mapUnionMemInfo.end() == m_mapUnionMemInfo.find(pMember->GetExID()));
	if(m_mapUnionMemInfo.end() != m_mapUnionMemInfo.find(pMember->GetExID()))
		return FALSE;

	tagUnionMemInfo UnionMemInfo;
	UnionMemInfo.lJoinDate = (long)time(NULL);
	UnionMemInfo.lPurview = (bIsMaster ? eUMP_All : eUMP_Null);
	UnionMemInfo.MemberGuid = pMember->GetExID();

	LONG lSize = (long)strlen(pMember->GetName());
	lSize = (MAXSIZE_FactionName - 1 >= lSize ) ? lSize : (MAX_MEMBER_NAME_SIZE - 1);
	memmove(UnionMemInfo.szName, pMember->GetName(), lSize);

	m_mapUnionMemInfo[UnionMemInfo.MemberGuid] = UnionMemInfo;
	pMember->SetSuperior(this);
	m_dwDataChanged |= eFDCF_Member;

	return TRUE;
}


//! 删除成员
BOOL CUnion::DelMember(const CGUID& MemberGuid)
{
	if(NULL_GUID == MemberGuid || MemberGuid == m_UnionBaseData.MasterFacGuid)
		return FALSE;

	map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.find(MemberGuid);
	if(m_mapUnionMemInfo.end() == iteMember)
		return FALSE;

	m_mapUnionMemInfo.erase(iteMember);
	m_dwDataChanged |= eFDCF_Member;
	return TRUE;
}

//! 修改盟主
BOOL CUnion::ChangeMaster(const CGUID& NewMasterGuid)
{
	if(NULL_GUID == NewMasterGuid || NewMasterGuid == m_UnionBaseData.MasterFacGuid)
		return FALSE;

	map<CGUID, tagUnionMemInfo>::iterator iteNewMaster = m_mapUnionMemInfo.find(NewMasterGuid);
	if(m_mapUnionMemInfo.end() == iteNewMaster)
		return FALSE;
	map<CGUID, tagUnionMemInfo>::iterator iteOldMaster = m_mapUnionMemInfo.find(m_UnionBaseData.MasterFacGuid);
	assert(m_mapUnionMemInfo.end() != iteOldMaster);
	if (m_mapUnionMemInfo.end() == iteOldMaster)
		return FALSE;

	CFaction *pNewMaster = GetOrganiCtrl()->GetFactionOrganizing(NewMasterGuid);
	if(NULL == pNewMaster)
		return FALSE;
	iteOldMaster->second.lPurview	= eUMP_Null;
	iteNewMaster->second.lPurview	= eUMP_All;
	m_UnionBaseData.MasterGuid		= pNewMaster->GetMasterID();
	m_UnionBaseData.MasterFacGuid	= NewMasterGuid;
	memmove(m_UnionBaseData.szMasterName, iteNewMaster->second.szName, MAX_MEMBER_NAME_SIZE);

	m_dwDataChanged |= (eFDCF_Member | eFDCF_BaseInfo);
	return TRUE;
}

//! 设置成员权限
BOOL CUnion::SetPurview(const CGUID& MemberGuid, DWORD dwNewPurview)
{
	if(NULL_GUID == MemberGuid || MemberGuid == m_UnionBaseData.MasterFacGuid)
		return FALSE;

	map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.find(MemberGuid);
	if(m_mapUnionMemInfo.end() == iteMember)
		return FALSE;

	if(iteMember->second.lPurview == dwNewPurview)
		return FALSE;

	iteMember->second.lPurview = dwNewPurview;
	m_dwDataChanged |= eFDCF_Member;
	return TRUE;
}

//! 解散
BOOL CUnion::Disband(VOID)
{
	BOOL bRe = TRUE;
	map<CGUID, tagUnionMemInfo>::iterator iteMember = m_mapUnionMemInfo.begin();
	for (; m_mapUnionMemInfo.end() != iteMember; ++iteMember)
	{
		CFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(iteMember->first);
		if (NULL != pFaction)
			pFaction->SetSuperior(NULL);
		else
			bRe = FALSE;
	}
	assert(bRe);
	return bRe;
}

