/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：CMonsterInfo.cpp
*  功能：单个玩家怪物显示列表
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
#include "MonsterInfo.h"
#include ".\monsterinfo.h"
#include "../Script/ScriptSys.h"
#include "../App/Player.h"
#include "../App/Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CScriptSys;

IMPLEMENT_DYNAMIC(CMonsterInfo, CDialog)
CMonsterInfo::CMonsterInfo(CWnd* pParent /*=NULL*/)
: CDialog(CMonsterInfo::IDD, pParent)
{
}


CMonsterInfo::~CMonsterInfo()
{
	if(!m_pMonster.empty())
	{
		list<staMsInfo*>::iterator it = m_pMonster.begin();
		for(;it!=m_pMonster.end();)
		{
			staMsInfo* pM = (*it);
			SAFE_DELETE(pM);
			m_pMonster.erase(it++);
		}
	}
	m_pMonster.clear();
}


void CMonsterInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Monster, m_listMonster);
	DDX_Control(pDX, IDC_LIST_Npc, m_listNpc);
}



BEGIN_MESSAGE_MAP(CMonsterInfo, CDialog)
END_MESSAGE_MAP()

BOOL CMonsterInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listMonster.InsertColumn(1,"名字",80,80,10);
	m_listMonster.InsertColumn(2,"x",10,40,10);
	m_listMonster.InsertColumn(3,"y",10,40,10);

	m_listNpc.InsertColumn(1,"名字",80,80,10);
	m_listNpc.InsertColumn(2,"x",10,40,10);
	m_listNpc.InsertColumn(3,"y",10,40,10);

	return TRUE;
}


// CMonsterInfo 消息处理程序
void CMonsterInfo::SetDisplayMonster(CRegion* pRegion)
{
	if ( pRegion == NULL ) return;
	m_listMonster.DeleteAllItems();
	m_listNpc.DeleteAllItems();

	if(!m_pMonster.empty())
	{
		list<staMsInfo*>::iterator it = m_pMonster.begin();
		for(;it!=m_pMonster.end();)
		{
			staMsInfo* m = (*it);
			SAFE_DELETE(m);
			m_pMonster.erase(it++);
		}
	}
	m_pMonster.clear();

	list<CBaseObject*> *pObjectList = pRegion->GetObjectList();
	int i =0,j=0,k=0;
	list<CBaseObject*>::iterator it = pObjectList->begin(); 
	for(;it != pObjectList->end();++it)
	{
		char str[128];
		CMonster *pMonster = (CMonster*)(*it);
		if( pMonster && pMonster->GetType() == TYPE_MONSTER )
		{
			staMsInfo *pM = new staMsInfo;
			if( pMonster->GeteNpcType()==NT_Normal || pMonster->GeteNpcType()==NT_Guard )
			{
				sprintf(str,"%s",pMonster->GetName());
				m_listNpc.InsertItem(k,str);
				sprintf(str,"%d",(long)pMonster->GetPosX());
				m_listNpc.SetItemText(k,1,str);
				sprintf(str,"%d",(long)pMonster->GetPosY());
				m_listNpc.SetItemText(k,2,str);

				pM->gID = pMonster->GetExID();
				pM->lType= pMonster->GetType();
				pM->name = pMonster->GetName();
				m_listNpc.SetItemData(k,(DWORD_PTR)pM);
				k++;
			}
			else
			{
				sprintf(str,"%s",pMonster->GetName());
				m_listMonster.InsertItem(i,str);
				if((long)pMonster->GetDestX()==0 || (long)pMonster->GetDestY()==0)
				{
					sprintf(str,"%d",(long)pMonster->GetTileX());
					m_listMonster.SetItemText(i,1,str);
					sprintf(str,"%d",(long)pMonster->GetTileY());
					m_listMonster.SetItemText(i,2,str);
				}
				else
				{
					sprintf(str,"%d",(long)pMonster->GetDestX());
					m_listMonster.SetItemText(i,1,str);
					sprintf(str,"%d",(long)pMonster->GetDestY());
					m_listMonster.SetItemText(i,2,str);
				}

				pM->gID= pMonster->GetExID();
				pM->lType = pMonster->GetType();
				pM->name = pMonster->GetName();
				m_listMonster.SetItemData(i,(DWORD_PTR)pM);
				i++;
			}
			m_pMonster.push_back(pM);
		}
	}
}