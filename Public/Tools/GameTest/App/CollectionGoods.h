#pragma once
#include "../App/MoveShape.h"

const int GOODSNAME_LEN_ =50;	//物品名字长度

struct tagConditionGoods
{
	char	cGoodsName[GOODSNAME_LEN_];
	DWORD	dwGoodsIndex;
	int		iNumType;
	int		iNum;
};

class CCollectionGoods
	: public CMoveShape
{
public:
	CCollectionGoods();
	~CCollectionGoods() {};
protected:
private:
	long  m_lCreatorType;
	CGUID m_CreatorGuid;
public:
	void SetAction(WORD lState);

	virtual bool DecordFromDataBlock(DBReadSet& readDB,bool bExData = true);
	// 获得Monster创建者的类型和ID
	long GetCreatorType()		{return m_lCreatorType;}
	CGUID GetCreatorID()		{return m_CreatorGuid;}
	// 设置Monster创建者的类型和ID
	void SetCreatorType(long type)		{m_lCreatorType = type;}
	void SetCreatorID(CGUID guid)		{m_CreatorGuid = guid;}
};