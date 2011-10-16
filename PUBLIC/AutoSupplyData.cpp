
#include "StdAfx.h"
#include "public.h"
#include "AutoSupplyData.h"


AutoSupplyData::AutoSupplyData(void)
{
	Reset();
}
AutoSupplyData::~AutoSupplyData(void)
{

}

#define DATA_SIZE sizeof(tagAotoSupplyNode) * AotoSupplyTypeNum

//! 编码
void AutoSupplyData::AddToByteArray(vector<BYTE> &ByteArray)
{
	_AddToByteArray(&ByteArray, m_AotoSupplySetup, DATA_SIZE);
}

//! 解码
void AutoSupplyData::GetFormByteArray(BYTE *pByteArray, long &lPos)
{
	_GetBufferFromByteArray(pByteArray, lPos, m_AotoSupplySetup, DATA_SIZE);
}

//!							编码
void AutoSupplyData::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(m_AotoSupplySetup, DATA_SIZE);
}
	//!							解码
void AutoSupplyData::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	setReadDB.GetBufferFromByteArray(m_AotoSupplySetup, DATA_SIZE);
}

//! 重置对象
void AutoSupplyData::Reset(void)
{
	memset(m_AotoSupplySetup, 0, DATA_SIZE);
}

//! 获得一个设置（uType）
const tagAotoSupplyNode* AutoSupplyData::GetOneSetup(LONG eType)
{
	if (AotoSupplyTypeNum > eType)
	{
		return &(m_AotoSupplySetup[eType]);
	}
	return NULL;
}

//! 修改一个设置
LONG AutoSupplyData::SetOneSetup(LONG eType, const tagAotoSupplyNode &AotoSupplyNode)
{
	if (AotoSupplyTypeNum <= eType)
		return eASET_TypeWithout;

	//! 相同的对象值不重新设置
	if(0 == memcmp(&(m_AotoSupplySetup[eType]), &AotoSupplyNode, sizeof(tagAotoSupplyNode)))
		return eASET_NotChange;

	//! 判断值的逻辑合法性
	if(	0 >= AotoSupplyNode.lGoodsIndex && 
		0 >= AotoSupplyNode.lSpringScale && 
		A_Hundred_Percent <= AotoSupplyNode.lSpringScale
	)
		return eASET_ValueErr;
	
	memmove(&(m_AotoSupplySetup[eType]), &AotoSupplyNode, sizeof(tagAotoSupplyNode));
	return eASET_NotErr;
}