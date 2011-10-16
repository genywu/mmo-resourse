/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CRegionGoodsInfo.h
*  功能：单个玩家所在场景物品显示列表
*  当前版本：1.1.0
*  作者：
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期： 
*
============================================================
*/

#pragma once
#include "afxcmn.h"
#include "..\\App\\Region.h"

class CRegionGoodsInfo : public CDialog
{
	DECLARE_DYNAMIC(CRegionGoodsInfo)

public:

	CRegionGoodsInfo(CWnd* pParent = NULL); 
	virtual ~CRegionGoodsInfo();

	enum { IDD = IDD_PROPPAGE_Region_Goods_Info };

protected:

	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()

public:

	struct staRgnGoods{
		CGUID guid;
		long lType;
	};

private:
	list<CRegionGoodsInfo::staRgnGoods*>	m_pRegionGoods;
	
public:
	list<CRegionGoodsInfo::staRgnGoods*>	GetRegionGoods()	{ return m_pRegionGoods;}
private:
	CListCtrl m_ctlRegionGoodsList;
public:
	virtual BOOL OnInitDialog();
	void SetDisplayGoods(CRegion* pRegion);
};
