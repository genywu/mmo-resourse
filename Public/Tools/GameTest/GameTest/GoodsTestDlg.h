/*
=========================================================================
备注：暂未使用。2010.10.28  BY:LGR
=========================================================================
*/

/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CGoodsTestDlg.h
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

#pragma once

class CGoodsTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CGoodsTestDlg)

public:
	CGoodsTestDlg(CWnd* pParent = NULL);
	virtual ~CGoodsTestDlg();

	enum { IDD = IDD_DIALOG_GOODS_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
