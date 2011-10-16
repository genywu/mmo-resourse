/*
=========================================================================
备注：暂未使用。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CGoodsTestDlg.cpp
*  功能： 物品相关
*  当前版本：1.2.0
*  作者： LGR
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：2009
*
============================================================
*/

#include "stdafx.h"
#include "GameTest.h"
#include "GoodsTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGoodsTestDlg, CDialog)

CGoodsTestDlg::CGoodsTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoodsTestDlg::IDD, pParent)
{

}

CGoodsTestDlg::~CGoodsTestDlg()
{
}

void CGoodsTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGoodsTestDlg, CDialog)
END_MESSAGE_MAP()

