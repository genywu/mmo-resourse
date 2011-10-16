#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "commui.h"
#include "commdb.h"
#include "map.h"
#include "cmdmgr.h"
#include "editwnd.h"
#include "wndselectbuild.h"

WndSelectBuild::WndSelectBuild(void):
m_dwCurTabLabel(0xFFFFFFFF),
m_pScriptBuild(NULL),
m_wCurBuildID(0xFFFF)
{
}

WndSelectBuild::~WndSelectBuild(void)
{
}

BOOL WndSelectBuild::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	//建立窗口
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTab = { 0,0,206,25 };
	m_pTabCtl = new ui::TabCtl;
	m_pTabCtl->Create("TABCTL",&rcTab,dwStyle,this);

	RECT rcListBoxBuild = { 3,25,204,224};
	m_pListBoxBuild = new ui::ListBox;
	m_pListBoxBuild->Create("BUILD",&rcListBoxBuild,dwStyle,this);
	//m_pListBoxBuild->SetBkColor(0xaa1F334A);

	//SetBkColor(0xa0112B51);

	//读取类型目录
	RefreshBuildType();

	return TRUE;
}

void WndSelectBuild::Destroy()
{
	SAFEDESTROY(m_pScriptBuild);
	SAFEDESTROY(m_pListBoxBuild);
	SAFEDESTROY(m_pTabCtl);

	ui::Wnd::Destroy();
}

void WndSelectBuild::Update()
{
		//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		RefreshFileListBox();

		if (m_pListBoxBuild->IsSelChanged())
		{
			DWORD dwIndex = m_pListBoxBuild->GetSelItem();
			if (m_pListBoxBuild->GetItemCount() > 0)
			{
				m_wCurBuildID = (WORD)((dwIndex & 0x0fff) | ((m_dwCurTabLabel << 12) & 0xf000));
			}
		}

		ui::Wnd::UpdateChild();
	}	
}

void WndSelectBuild::RefreshBuildType()
{
	utility::File	file;
	SAFEDESTROY(m_pScriptBuild);
	if (!file.Open("model\\build\\build"))
	{
		OutputConsole("error: can't open build!\n");
		MessageBox(NULL,"打开[建筑]配置文件失败！","错误",MB_OK);
		PostQuitMessage(-1);
		return ;
	}

	m_pScriptBuild = new utility::Text;

	m_pScriptBuild->Create(&file);

	file.Close();

	utility::Text::Node *pBuildNode = m_pScriptBuild->GetRootNode()->GetChildNode("build");
	ASSERT(pBuildNode);

	char szTempName[_MAX_PATH];
	for(DWORD dwTypeNode = 0; dwTypeNode < pBuildNode->GetChildCount(); dwTypeNode++)
	{
		utility::Text::Node *pTypeNode = pBuildNode->GetChildNode(dwTypeNode);
		_snprintf(szTempName,_MAX_PATH,"%s(%s)",pTypeNode->GetVar("type")->GetStringValue(),pTypeNode->GetName());
		m_pTabCtl->AddLabel(szTempName);

   }
	

	if (m_pTabCtl->GetLabelCount() > 0)
	{
		m_pTabCtl->SetHighLightLabel(0);
		RefreshFileListBox();
	}
}

void WndSelectBuild::RefreshFileListBox()
{
	DWORD dwTableSel = m_pTabCtl->GetHighLightLabel();
	char szTempName[_MAX_PATH];
	if ( m_pTabCtl->GetLabelCount() > 0 && dwTableSel != m_dwCurTabLabel)
	{
		m_pListBoxBuild->DeleteAllItem();

		m_dwCurTabLabel = dwTableSel;
		//刷新列表框.

		utility::Text::Node *pBuildNode = m_pScriptBuild->GetRootNode()->GetChildNode("build");
		ASSERT(pBuildNode);

		utility::Text::Node *pTypeNode = pBuildNode->GetChildNode(m_dwCurTabLabel);

		for(DWORD n = 0; n < pTypeNode->GetChildCount(); n ++)
		{
			utility::Text::Node *pNode = pTypeNode->GetChildNode(n);
			_snprintf(szTempName,_MAX_PATH,"%s(%s)",pNode->GetVar("name")->GetStringValue(),pNode->GetName());
			m_pListBoxBuild->InsertItem(szTempName);
		}
	}
}
void WndSelectBuild::Refresh(void)
{
	//建立窗口
	m_pTabCtl->DeleteAllLabel();
	//读取类型目录
	RefreshBuildType();
}

void WndSelectBuild::Select(WORD wBuildID)
{
	DWORD dwLabel = (wBuildID & 0xf000) >> 12;
	DWORD dwIndex = (wBuildID & 0x0fff);
	//设置label类型
	m_dwCurTabLabel = dwLabel;
	m_pTabCtl->SetHighLightLabel(dwLabel);

	//刷新类型列表	
	RefreshFileListBox();

	//选中当前id

	m_pListBoxBuild->SetSelItem(dwIndex);

	m_wCurBuildID = (WORD)((dwIndex & 0x0fff) | ((dwLabel << 12) & 0xf000));
}
