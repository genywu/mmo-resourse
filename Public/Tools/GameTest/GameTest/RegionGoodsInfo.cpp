/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CRegionGoodsInfo.cpp
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

#include "stdafx.h"
#include "GameTest.h"
#include "RegionGoodsInfo.h"
#include ".\regiongoodsinfo.h"
#include "../App/Goods.h"
#include "../Script/ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CRegionGoodsInfo, CDialog)
CRegionGoodsInfo::CRegionGoodsInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CRegionGoodsInfo::IDD, pParent)
{
}

CRegionGoodsInfo::~CRegionGoodsInfo()
{
	list<staRgnGoods*> p1 = GetRegionGoods();
	list<staRgnGoods*>::iterator p2 = p1.begin();
	list<staRgnGoods*>::iterator pTemp;
	if (!p1.empty())
	{
		for (; p2!=p1.end();)
		{
			pTemp = p2;
			++p2;
			delete *pTemp;
		}
	}
	m_pRegionGoods.clear();
}

void CRegionGoodsInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RegionGoods, m_ctlRegionGoodsList);
}


BEGIN_MESSAGE_MAP(CRegionGoodsInfo, CDialog)
END_MESSAGE_MAP()


BOOL CRegionGoodsInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i=1;
	m_ctlRegionGoodsList.InsertColumn(i++,"Name",10,80,10);
	m_ctlRegionGoodsList.InsertColumn(i++,"X",10,40,10);
	m_ctlRegionGoodsList.InsertColumn(i++,"Y",10,40,10);
	return TRUE;

}

void CRegionGoodsInfo::SetDisplayGoods(CRegion* pRegion)
{
	if ( pRegion == NULL) return;
	//清除控件
	m_ctlRegionGoodsList.DeleteAllItems();
	//清除容器
	if( !m_pRegionGoods.empty())
	{
		list<staRgnGoods*>::iterator it = m_pRegionGoods.begin();
		for(;it!=m_pRegionGoods.end();)
		{
			staRgnGoods* pGoods = (*it);
			SAFE_DELETE(pGoods);
			m_pRegionGoods.erase(it++);
		}
	}

	int i=0;
	list<CBaseObject*>* pObjectList = pRegion->GetObjectList();
	if( !pObjectList->empty())
	{
		list<CBaseObject*>::iterator it = pObjectList->begin();
		for(;it!=pObjectList->end();it++)
		{
			char str[128];
			CGoods *pGoods = (CGoods*)(*it);
			if( pGoods && pGoods->GetType() == TYPE_GOODS )
			{
				staRgnGoods *pTempGoods = new staRgnGoods; 
				pTempGoods->guid = pGoods->GetExID();
				pTempGoods->lType = TYPE_GOODS;
				//
				sprintf(str,"%s",pGoods->GetName());
				m_ctlRegionGoodsList.InsertItem(i,str);
				sprintf(str,"%d",pGoods->GetTileX());
				m_ctlRegionGoodsList.SetItemText(i,1,str);
				sprintf(str,"%d",pGoods->GetTileY());
				m_ctlRegionGoodsList.SetItemText(i,2,str);
				//
				m_ctlRegionGoodsList.SetItemData(i,(DWORD_PTR)pTempGoods);
				m_pRegionGoods.push_back(pTempGoods);
			}
		}
	}

}
//void CRegionGoodsInfo::OnNMRclickListRegiongoods(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	POINT   _point; 
//	::GetCursorPos(&_point);  
//	CMenu menu;
//	menu.LoadMenu(IDR_RegionGoodsInfo);
//	menu.GetSubMenu(0)
//		->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
//		_point.x,_point.y,this);  
//	*pResult = 0;
//}
