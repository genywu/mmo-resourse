#include "StdAfx.h"
#include "DBRegion.h"
#include "DBentityManager.h"
#include "WorldRegion.h"
#include "GlobalRgnManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBRegion::CDBRegion()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);

	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
}

CDBRegion::CDBRegion(const CGUID& id)
:CBaseDBEntity(id)
{
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
}
CDBRegion::~CDBRegion()
{

}

void CDBRegion::OpenAttrFlagInVector(DB_REGION_ATTR_TYPE attrType)
{
	if(attrType == DB_REGION_ATTR_BUF_REGIONPARAM)
	{
		m_SendAttrEnumVector[0] = 1;
	}
}
// 关闭准备发送的属性在Vector中的相应位置的标志
void CDBRegion::CloseAttrFlagInVector(DB_REGION_ATTR_TYPE attrType)
{
	if(attrType == DB_REGION_ATTR_BUF_REGIONPARAM)
	{
		m_SendAttrEnumVector[0] = 0;
	}
}

// virtual interfaces
void CDBRegion::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	//CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
	switch(op)
	{
	case DB_OPER_SAVE:
		Save(msgid, msgBuf, msgBufPos);
		break;
	case DB_OPER_LOAD:
		Load(msgid, msgBuf, msgBufPos, bufSize);
		break;
	}
}

void CDBRegion::Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{	
	long lRegionId=_GetLongFromByteArray(msgBuf,msgBufPos);
	SetID(lRegionId);
	//通过regionid找到CWorldRegion
	CWorldRegion* pWorldRegion=GetGame()->GetGlobalRgnManager()->GetWorldRegionByRgnId(lRegionId);
	BYTE bLoadFromDB=_GetByteFromByteArray(msgBuf,msgBufPos);
	if(bLoadFromDB==1)
	{
		//更新从数据库读取的数据
		tagRegionParam stParam;
		_GetBufferFromByteArray(msgBuf,msgBufPos,&stParam,sizeof(tagRegionParam));
		tagRegionParam& stDBRegionParam=GetRegionParam();
		stDBRegionParam.OwnedFactionID=stParam.OwnedFactionID;
		stDBRegionParam.OwnedUnionID=stParam.OwnedUnionID;
		stDBRegionParam.lCurrentTaxRate=stParam.lCurrentTaxRate;
		stDBRegionParam.dwTotalTax=stParam.dwTotalTax;
		stDBRegionParam.dwTodayTotalTax=stParam.dwTodayTotalTax;	
		stDBRegionParam.dwBusinessTotalTax=stParam.dwBusinessTotalTax;
		stDBRegionParam.lBusinessTaxRate=stParam.lBusinessTaxRate;
		stDBRegionParam.lYuanbaoTaxRate=stParam.lYuanbaoTaxRate;
		
		if(pWorldRegion)
		{
			::tagRegionParam stRegionParam;
			memcpy(&stRegionParam,&stDBRegionParam,sizeof(tagRegionParam));
			::tagRegionParam& stRegionParam2=pWorldRegion->GetRegionParam();
			stRegionParam.lID=stRegionParam2.lID;
			stRegionParam.lTurnInTaxRate=stRegionParam2.lTurnInTaxRate;
			stRegionParam.lSupRegionID=stRegionParam2.lSupRegionID;
			stRegionParam.lMaxTaxRate=stRegionParam2.lMaxTaxRate;
			pWorldRegion->SetRegionParam(stRegionParam);
		}
	}
	else
	{
		//更新为默认配置

	}
}

void CDBRegion::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
{
	char szGuid[128];
	GetExID().tostring(szGuid);
	//AddLogText("DB2W[%d] : DB_OPER_SAVE_Region, RegionID[%s]...OK!", msgid, szGuid);
}

void CDBRegion::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
{
	// 传递来的消息前面已经加入了
	// OPEntityID:    4byte, 操作实体的ID值
	// DBOP Type:     1byte, 数据库操作类型（枚举值）
	// MSG ID:        4byte, 该消息的MSGID
	// Entity Num:    2byte, 该实体个数
	if(DB_OPER_DELETE == opType) // DELETE操作
	{
		return;
	}

	WORD attrNum = 0;
	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
	{
		if(m_SendAttrEnumVector[i] != 0)
			attrNum++;
	}
	_AddToByteArray(&pBA, (long)m_ID); // 实体ID
	_AddToByteArray(&pBA, attrNum); // 属性种类个数

	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
	{
		if(m_SendAttrEnumVector[i] != 0 
			&& ( DB_OPER_SAVE == opType || DB_OPER_UPDATE == opType || DB_OPER_LOAD == opType) )// 标志打开并且为SAVE/UPDATE/CREATE操作
		{
			if(DB_REGION_ATTR_BUF_REGIONPARAM)
			{
				_AddToByteArray(&pBA, (WORD)1);
				_AddToByteArray(&pBA, (long)DB_REGION_ATTR_BUF_REGIONPARAM);
				if(DB_OPER_LOAD != opType)
				{
					_AddToByteArray(&pBA, &GetRegionParam(), sizeof(tagRegionParam));
				}
				else
					_AddToByteArray(&pBA, (BYTE)0);
			}
		}
	}

}


