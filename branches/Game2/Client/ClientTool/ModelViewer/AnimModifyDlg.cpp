#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\model.h"
#include "..\..\gameengine\gamemodel.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "AnimModifyDlg.h"
#include "wndviewer.h"
#include "WndModel.h"
#include "WndAction.h"
#include "AppClient\RoleModel.h"
#include "AppClient\BindModel.h"

WndAnimModify::WndAnimModify(void)
{
}

WndAnimModify::~WndAnimModify(void)
{
}

BOOL WndAnimModify::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTextTimeLength= { 2 + 24, 3, 41 + 24, 25 };
	m_pStaticTimeLength = new ui::Static;
	m_pStaticTimeLength->Create("时间",&rcTextTimeLength,FWS_VISIBLE ,this);

	RECT rcCurTime = {  36 + 24, 4, 36 + 47 + 12, 4 + 19};
	m_pEditCurTime = new ui::EditBox;
	m_pEditCurTime->Create("CurTime",&rcCurTime,FWS_DISABLE | FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

    RECT rcTimeLength = { 90 + 12, 4, 135, 23 };
	m_pEditTimeLength = new ui::EditBox;
	m_pEditTimeLength->Create("TimeLength",&rcTimeLength,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	return TRUE;
}
void WndAnimModify::Destroy()
{
	SAFEDESTROY(m_pStaticTimeLength);
	SAFEDESTROY(m_pEditCurTime);
    SAFEDESTROY(m_pEditTimeLength);

	ui::Wnd::Destroy();
}

void WndAnimModify::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pEditTimeLength->CheckEnter())
		{
			//应用
			char szTemp[256];
			m_pEditTimeLength->GetEditText(szTemp,256);
			DWORD dwTime = atoi(szTemp);
			if (dwTime == 0)
			{
				dwTime = 1;
				m_pEditTimeLength->Clear();
				itoa(dwTime,szTemp,10);
				m_pEditTimeLength->InsertText(0,szTemp,lstrlen(szTemp));
			}

			WndViewer				* pWnd		  = (WndViewer *)GetParent();
			AnimInfo::tagActionInfo * pActionInfo = pWnd->GetAnimInfo()->GetActionInfo();
			GameModel::Action		* pAction	  = CGameModelManager::GetInstance()->GetActionAnim(pWnd->GetModel()->GetGameModelID(), pActionInfo->dwCurAction);
			pAction->GetAnimJoint()->SetTimeLength(dwTime);

			//读取文件名
			char szCode[5]  = { 0 };
			*(DWORD*)szCode = pAction->GetName();

			int iCurrentModel = 0;
			const char * pszFilePathName = NULL;
			switch (pWnd->m_pWndAction->GetCurrentActionType())
			{
			case AT_ROLE:
				{
					iCurrentModel			= pWnd->m_pWndModels->GetSelRoleModel();
					CRoleModel * pRoleModel = pWnd->m_vRoleModels[iCurrentModel];
					pszFilePathName = pRoleModel->GetFilePathName().c_str();
				}
				break;
			case AT_BIND:
				{
					iCurrentModel			= pWnd->m_pWndModels->GetSelBindModel();
					CBindModel * pBindModel = pWnd->m_vBindModels[iCurrentModel];
					pszFilePathName = pBindModel->GetFilePathName().c_str();
				}
				break;
			}

			//读取文件
			utility::File file;
			if (file.Open(pszFilePathName))
			{
				utility::Text script;
				script.Create(&file);
				utility::Text::Node *pActionNode = script.GetRootNode()->GetChildNode("action");
				utility::Text::Node *pAnimNode   = pActionNode->GetChildNode(szCode);
				ASSERT(pAnimNode);
				
				char szPath[_MAX_PATH];
				utility::File::GetFilePath(pszFilePathName, szPath);
				strcat(szPath,"\\");
				strcat(szPath,pAnimNode->GetVar("file")->GetStringValue());
				render::Interface * pInterface = render::Interface::GetInstance();

				//写入新文件
				utility::File fileInput;
				if (fileInput.Open(szPath))
				{
					int iLength = fileInput.GetLength();
					BYTE *p = new BYTE[iLength];
					iLength = fileInput.GetLength();
					memcpy(p,fileInput.GetData(),iLength);
					fileInput.Close();
					*(DWORD*)(p + sizeof(DWORD) * 3) = pAction->GetAnimJoint()->GetTimeLength();

					FILE *fp = fopen(szPath,"wb");
					VERIFY(fp);
					fwrite(p,iLength,1,fp);
					script.Destroy();
					fclose(fp);
					file.Close();
					
				}
				else
				{
					MessageBox(pInterface->GetWndHandle(),"写入完成","提示",MB_OK);
				}
			}		
		}                      

		ui::Wnd::UpdateChild();
	}
}

void WndAnimModify::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		ui::Wnd::RenderChild();
	}
}

void WndAnimModify::SetTimeLength(DWORD dwTimeLength)
{
	char szTemp[16];
	itoa(dwTimeLength,szTemp,10);
	m_pEditTimeLength->Clear();
	m_pEditTimeLength->InsertText(0,szTemp,16);
}


void WndAnimModify::SetCurTime(DWORD dwElapseTime)
{
	char szTemp[16];
	itoa(dwElapseTime,szTemp,10);
	m_pEditCurTime->Clear();
	m_pEditCurTime->InsertText(0,szTemp,16);
}


DWORD WndAnimModify::GetTimeLength()
{
	char szTemp[16];
	m_pEditTimeLength->GetEditText(szTemp, 16);
	return (DWORD)atoi(szTemp);
}