#pragma once
#include "shape.h"
#include "..\server\setup\GoodsList.h"

class CGoods :
	public CShape
{
public:
	CGoods(void);
	virtual ~CGoods(void);
	DWORD GetIndex(){return m_dwIndex;}
	void  SetIndex(DWORD dwVal){m_dwIndex=dwVal;}

private:
	DWORD	m_dwIndex;		// 编号
	long	lNum;			//数量
	vector<CGoodsList::tagEffect>	m_vectorEffect;		// 扩展属性列表
	vector<CGoodsList::tagFixValues>		m_vecFixValues; //修正属性列表


public:
	CGoodsList::tagGoods* GetProperty()	{return CGoodsList::GetProperty(m_dwIndex);}
	vector<CGoodsList::tagEffect>*		GetEffect()	{return &m_vectorEffect;}
	vector<CGoodsList::tagFixValues>* GetFixs(){return &m_vecFixValues;}

	//得到编号为wType的扩展属性值,nWhich指定值1还是值2
	DWORD GetEffectVal(WORD wType,int nWhich);

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	bool AddToByteArray_ForClient(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	void SetNum(long lVal){lNum=lVal;}
	long GetNum(){return lNum;}
};
