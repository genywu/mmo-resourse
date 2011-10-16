



#include "stdafx.h"
#include "Sprite.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSprite::CSprite()
{

}

CSprite::~CSprite()
{
	Release();
}

void CSprite::Release()
{

}

bool CSprite::DecodeFromDataBlock(DBReadSet& dbRead)
{
	long lGoodsNum = dbRead.GetLongFromByteArray();
	m_Pixie.lItemGoodsNum = lGoodsNum;
	long * pItemList = new long[lGoodsNum];
	DWORD* pItemUse = new DWORD[lGoodsNum];
	int j = 0;
	for(int i=0;i<lGoodsNum;i++)
	{
		pItemList[i] = dbRead.GetLongFromByteArray();
		pItemUse[i] = dbRead.GetLongFromByteArray();
	}
	tagAutoSupplyData mAutoData[AutoSupplyTypeNum];
	dbRead.GetBufferFromByteArray(mAutoData,sizeof(tagAutoSupplyData)*AutoSupplyTypeNum);
	for(int i=0;i<AutoSupplyTypeNum;i++)
	{
		m_AutoSupplySetup[i].lGoodsIndex = mAutoData[i].lGoodsIndex;
		m_AutoSupplySetup[i].lSpringScale = mAutoData[i].lSpringScale;
		m_AutoSupplySetup[i].bActive = mAutoData[i].bActive;
		//
		m_AutoSupplySetupEx[i].lGoodsIndex = m_AutoSupplySetup[i].lGoodsIndex;
		m_AutoSupplySetupEx[i].lSpringScale= m_AutoSupplySetup[i].lSpringScale;
		m_AutoSupplySetupEx[i].bActive = m_AutoSupplySetup[i].bActive;
	}
	SAFE_DELETE_ARRAY(pItemList);
	SAFE_DELETE_ARRAY(pItemUse);


	//自动喝药数据
	dbRead.GetBufferFromByteArray(&m_SetupOnServer,sizeof(tagSetupOnServer));
	return true;
}