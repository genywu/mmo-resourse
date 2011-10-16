#include "StdAfx.h"
#include ".\goods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGoods::CGoods(void)
{
	SetType(TYPE_GOODS);
	m_dwIndex = 0;
}

CGoods::~CGoods(void)
{
}

//wType:扩展属性编号   nWhich:值1还是值2
DWORD CGoods::GetEffectVal(WORD wType,int nWhich)
{
	vector<CGoodsList::tagEffect>::iterator iter=GetEffect()->begin();
	for(;iter!=GetEffect()->end();iter++)
	{
		if(iter->wType==wType)
		{
			if(nWhich==1)
				return iter->dwValue1;
			else if(nWhich==2)
				return iter->dwValue2;
		}
	}
	return 0;
}

// 添加到CByteArray
bool CGoods::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CGoodsList::tagGoods *pstGood=GetProperty();
	_AddToByteArray(pByteArray, GetIndex());//压入物品INDEX
	_AddToByteArray(pByteArray,GetID());
	_AddToByteArray(pByteArray,GetNum());
	/*
	_AddToByteArray(pByteArray,(long)(GetEffect()->size()));//先压该物品的EFFECT个数
	vector<CGoodsList::tagEffect>::iterator EffectIter;
	for(EffectIter=GetEffect()->begin();EffectIter!=GetEffect()->end();EffectIter++)
	{
		_AddToByteArray(pByteArray,(BYTE*)&(*EffectIter),(long)sizeof(CGoodsList::tagEffect));
	}
	*/
	_AddToByteArray(pByteArray,(long)(GetFixs()->size()));
	vector<CGoodsList::tagFixValues>::iterator FixIter=GetFixs()->begin();
	for(;FixIter!=GetFixs()->end();FixIter++)
	{
		_AddToByteArray(pByteArray,(BYTE*)&(*FixIter),(long)sizeof(CGoodsList::tagFixValues));
	}
	return true;
}

// 添加到CByteArray
bool CGoods::AddToByteArray_ForClient(vector<BYTE>* pByteArray, bool bExData)
{
	_AddToByteArray(pByteArray, GetIndex());//压入物品INDEX
	_AddToByteArray(pByteArray, GetID());
	_AddToByteArray(pByteArray,GetNum());

	//物品的基本属性
	CGoodsList::tagGoods *pstGood=GetProperty();
	if (pstGood)
	{
		_AddToByteArray(pByteArray,const_cast<char*>(pstGood->BaseProperty.strName.c_str()));
		_AddToByteArray(pByteArray, pstGood->BaseProperty.dwValue);		//	价格
		_AddToByteArray(pByteArray, pstGood->BaseProperty.lX_Size);		//	物品X方向大小
		_AddToByteArray(pByteArray, pstGood->BaseProperty.lY_Size);		//	物品y方向大小
		_AddToByteArray(pByteArray, pstGood->BaseProperty.dwType);		//	物品类型
		_AddToByteArray(pByteArray, pstGood->BaseProperty.dwWeight);		//	重量
	}

	_AddToByteArray(pByteArray,GetEffectVal(37,2));//道具的当前耐久
	_AddToByteArray(pByteArray,(long)(GetFixs()->size()));
	vector<CGoodsList::tagFixValues>::iterator FixIter=GetFixs()->begin();
	for(;FixIter!=GetFixs()->end();FixIter++)
	{
		_AddToByteArray(pByteArray,(BYTE*)&(*FixIter),(long)sizeof(CGoodsList::tagFixValues));
	}
	_AddToByteArray(pByteArray, GetPosX());
	_AddToByteArray(pByteArray, GetPosY());

	return true;
}

// 解码
bool CGoods::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	DWORD dwIndex=_GetDwordFromByteArray(pByte,pos);
	SetIndex(dwIndex);
	long lID=_GetLongFromByteArray(pByte,pos);
	SetID(lID);
	long lNum=_GetLongFromByteArray(pByte,pos);
	SetNum(lNum);

	CGoodsList::tagGoods* pstGoods=CGoodsList::GetProperty(GetIndex());
	if(pstGoods)
	{
		vector<CGoodsList::tagEffect>::iterator EffectIter=pstGoods->vectorEffect.begin();
		for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
		{
			GetEffect()->push_back(*EffectIter);
		}
		long lFixSize=_GetLongFromByteArray(pByte,pos);
		for(int j=0;j<lFixSize;j++)
		{
			CGoodsList::tagFixValues stFixVal;
			_GetBufferFromByteArray(pByte,pos,&stFixVal,(long)sizeof(CGoodsList::tagFixValues));
			GetFixs()->push_back(stFixVal);
			EffectIter=GetEffect()->begin();
			for(;EffectIter!=GetEffect()->end();EffectIter++)
			{
				if(EffectIter->wType==stFixVal.wType)
				{
					EffectIter->dwValue1+=stFixVal.lFixVal1;
					EffectIter->dwValue2+=stFixVal.lFixVal2;
					break;
				}
			}
		}
		return true;
	}
	return false;
}
