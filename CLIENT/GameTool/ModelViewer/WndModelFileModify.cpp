#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndModel.h"
#include "WndModelTree.h"
#include "WndModelFileModify.h"
#include <fstream>

WndModelModify::WndModelModify()
{
	m_pBtnAddGroup = NULL;					//添加GROUP
	m_pBtnDelGroup = NULL;					//删除最后一组Group
	m_pBtnAddAction = NULL;					//添加动作
	m_pBtnDelAction = NULL;					//删除动作
	m_pBtnSetActionFile = NULL;				//指定动作anim文件
	m_pBtnAddPart = NULL;					//添加part组
	m_pBtnDelPart = NULL;					//删除part组
	m_pBtnSetPartFile = NULL;				//设置part文件

	m_pBtnAddTrail_emitter = NULL;			//添加尾迹发射器组
	m_pBtnDelTrail_emitter = NULL;			//删除尾迹发射器组
	m_pBtnSetTrail_MeshFile = NULL;			//设置尾迹MESH文件
	m_pBtnSetTrail_TrailFile = NULL;		//设置尾迹Trail文件

	m_pBtnAddParticle_emitter = NULL;		//添加粒子发射器组
	m_pBtnDelParticle_emitter = NULL;		//删除粒子发射器组
	m_pBtnSetParticle_MeshFile = NULL;		//设置粒子MESH文件
	m_pBtnSetParticle_ParticleFile = NULL;	//设置粒子Trail文件
	m_pEditNumber = NULL;					//设置粒子数量

	m_pConfirm = NULL;
	m_pCancel = NULL;
	m_pNode = NULL;							//当前编辑的Node
}

void WndModelModify::Destroy()
{
	SAFEDESTROY(m_pBtnAddGroup);
	SAFEDESTROY(m_pBtnDelGroup);
	
	SAFEDESTROY(m_pBtnAddAction);
	SAFEDESTROY(m_pBtnDelAction);
	SAFEDESTROY(m_pBtnSetActionFile);

	SAFEDESTROY(m_pBtnAddPart);
	SAFEDESTROY(m_pBtnDelPart);
	SAFEDESTROY(m_pBtnSetPartFile);

	SAFEDESTROY(m_pBtnAddTrail_emitter);
	SAFEDESTROY(m_pBtnDelTrail_emitter);
	SAFEDESTROY(m_pBtnSetTrail_MeshFile);
	SAFEDESTROY(m_pBtnSetTrail_TrailFile);

	SAFEDESTROY(m_pBtnAddParticle_emitter);
	SAFEDESTROY(m_pBtnDelParticle_emitter);
	SAFEDESTROY(m_pBtnSetParticle_MeshFile);
	SAFEDESTROY(m_pBtnSetParticle_ParticleFile);
	SAFEDESTROY(m_pEditNumber);
	SAFEDESTROY(m_pConfirm);
	SAFEDESTROY(m_pCancel);
	m_pNode = NULL;
	ui::Wnd::Destroy();
}

WndModelModify::~WndModelModify()
{

}
void WndModelModify::CaculateWhichToDisplay()
{
	if (!m_pNode)
		return;
	//////SetAllVisibleFlase/////////////
	m_pBtnAddGroup->ShowWnd(false);					//添加GROUP
	m_pBtnDelGroup->ShowWnd(false);					//删除最后一组Group
	m_pBtnAddAction->ShowWnd(false);					//添加动作
	m_pBtnDelAction->ShowWnd(false);					//删除动作
	m_pBtnSetActionFile->ShowWnd(false);				//指定动作anim文件
	m_pBtnAddPart->ShowWnd(false);					//添加part组
	m_pBtnDelPart->ShowWnd(false);					//删除part组
	m_pBtnSetPartFile->ShowWnd(false);				//设置part文件

	m_pBtnAddTrail_emitter->ShowWnd(false);			//添加尾迹发射器组
	m_pBtnDelTrail_emitter->ShowWnd(false);			//删除尾迹发射器组
	m_pBtnSetTrail_MeshFile->ShowWnd(false);			//设置尾迹MESH文件
	m_pBtnSetTrail_TrailFile->ShowWnd(false);		//设置尾迹Trail文件

	m_pBtnAddParticle_emitter->ShowWnd(false);		//添加粒子发射器组
	m_pBtnDelParticle_emitter->ShowWnd(false);		//删除粒子发射器组
	m_pBtnSetParticle_MeshFile->ShowWnd(false);		//设置粒子MESH文件
	m_pBtnSetParticle_ParticleFile->ShowWnd(false);	//设置粒子Trail文件
	m_pEditNumber->ShowWnd(false);					//设置粒子数量

	m_pConfirm->ShowWnd(false);
	m_pCancel->ShowWnd(false);
	ShowWnd(false);
	/////////////////////////////////////
	RECT rcContol2;
	rcContol2.left = 480;
	rcContol2.top = 120;
	rcContol2.right = rcContol2.left + 162;
	rcContol2.bottom = rcContol2.top + 25;

	const char *name = m_pNode->GetName();
	const char *value = m_pNode->GetValue();
	const char *parentName = NULL;				
	const char *GrandFatherName = NULL;
	const char *GreatGrandFatherName = NULL;
	ui::TreeView::Node *pCurNodeParent = m_pNode->GetParent();
	ui::TreeView::Node *pNodeGrandfather = NULL;
	ui::TreeView::Node *pNodeGreatGrandfather = NULL;
	if(pCurNodeParent)
	{
		parentName = pCurNodeParent->GetName();
		pNodeGrandfather = pCurNodeParent->GetParent();
		if(pNodeGrandfather)
		{
			GrandFatherName = pNodeGrandfather->GetName();
			pNodeGreatGrandfather = pNodeGrandfather->GetParent();
			if (pNodeGreatGrandfather)
				GreatGrandFatherName = pNodeGreatGrandfather->GetName();
		}
	}

	if (strcmp(name,"animuvcircle") == 0 || strcmp(name,"animcolorcircle") == 0 ||
		strcmp(name,"animvisibilitycircle") == 0 || strcmp(name,"animtexturecircle") == 0 )
	{
		m_pEditNumber->SetEditText(m_pNode->GetValue());
		m_pEditNumber->ShowWnd(true);
		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if (strcmp(name,"group") == 0 && strcmp(parentName,"main") == 0)
	{
		m_pBtnDelGroup->SetWndRect(&rcContol2);
		m_pBtnAddGroup->ShowWnd(true);
		m_pBtnDelGroup->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if (strcmp(parentName,"group") == 0 && strcmp(GrandFatherName,"main") == 0)
	{
		rcContol2.top = 42;
		rcContol2.bottom = 42 + 25;
		m_pEditNumber->SetEditText(name);
		m_pEditNumber->ShowWnd(true);
		m_pEditNumber->SetWndRect(&rcContol2);
		rcContol2.top += 30;
		rcContol2.bottom = rcContol2.top + 25;
		m_pBtnAddPart->SetWndRect(&rcContol2);
		rcContol2.top += 30;
		rcContol2.bottom = rcContol2.top + 25;
		m_pBtnDelPart->SetWndRect(&rcContol2);
		m_pBtnAddPart->ShowWnd(true);
		m_pBtnDelPart->ShowWnd(true);
		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if (GrandFatherName && strcmp(GrandFatherName,"group") == 0 && GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0)
	{
		m_pEditNumber->SetEditText(name);
		m_pEditNumber->ShowWnd(true);
		rcContol2.top -= 35;
		rcContol2.bottom -= 35;
		m_pBtnSetPartFile->SetWndRect(&rcContol2);				//设置part
		m_pBtnSetPartFile->ShowWnd(true);
		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if (strcmp(name,"action") == 0 && strcmp(parentName,"main") == 0)
	{
		m_pBtnAddAction->ShowWnd(true);								//添加动作
		m_pBtnDelAction->ShowWnd(true);
		m_pBtnDelAction->SetWndRect(&rcContol2);					//删除动作
		ShowWnd(true);
		return;
	}
	else if (strcmp(parentName,"action") == 0 && GrandFatherName &&strcmp(GrandFatherName,"main") == 0)
	{
		m_pEditNumber->SetEditText(name);
		m_pEditNumber->ShowWnd(true);
		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if(GrandFatherName && strcmp(GrandFatherName,"action") == 0 && GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0)
	{
		rcContol2.top -= 30;
		rcContol2.bottom -= 30;
		m_pBtnSetActionFile->SetWndRect(&rcContol2);
		m_pBtnSetActionFile->ShowWnd(true);
		ShowWnd(true);
		return;
	}
	else if (strcmp(name,"particle_emitter") == 0 && strcmp(parentName,"main") == 0)
	{
		m_pBtnAddParticle_emitter->ShowWnd(true);
		m_pBtnDelParticle_emitter->ShowWnd(true);
		m_pBtnDelParticle_emitter->SetWndRect(&rcContol2);
		ShowWnd(true);
		return;
	}
	else if (strcmp(parentName,"particle_emitter") == 0 && GrandFatherName && strcmp(GrandFatherName,"main") == 0)
	{
		m_pEditNumber->SetEditText(value);
		m_pEditNumber->ShowWnd(true);
		rcContol2.top -= 30;
		rcContol2.bottom -= 30;
		m_pBtnSetParticle_ParticleFile->ShowWnd(true);
		m_pBtnSetParticle_ParticleFile->SetWndRect(&rcContol2);
		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
	}
	else if (GrandFatherName && strcmp(GrandFatherName,"particle_emitter") == 0 && GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0)
	{
		if (strcmp(name,"shape") == 0)				//粒子发射器增，删，改
		{		
			rcContol2.top -= 30;								
			rcContol2.bottom -= 30;
			m_pBtnSetParticle_MeshFile->ShowWnd(true);
			m_pBtnSetParticle_MeshFile->SetWndRect(&rcContol2);
			ShowWnd(true);
		}
		else if (strcmp(name,"number") == 0)
		{
			m_pEditNumber->SetEditText(value);
			m_pEditNumber->ShowWnd(true);
			m_pConfirm->ShowWnd(true);
			m_pCancel->ShowWnd(true);
			ShowWnd(true);
		}
		return;
	}
	else if (strcmp(name,"trail_emitter") == 0 && strcmp(parentName,"main") == 0)
	{
		m_pBtnAddTrail_emitter->ShowWnd(true);
		m_pBtnDelTrail_emitter->ShowWnd(true);
		m_pBtnDelTrail_emitter->SetWndRect(&rcContol2);
		ShowWnd(true);
		return;
	}
	else if (strcmp(parentName,"trail_emitter") == 0 && GrandFatherName &&strcmp(GrandFatherName,"main") == 0)
	{
		rcContol2.top = 42;
		rcContol2.bottom = 42 + 25;
		m_pEditNumber->ShowWnd(true);
		m_pEditNumber->SetWndRect(&rcContol2);
		m_pEditNumber->SetEditText(name);
		rcContol2.top += 30;								//尾迹发射器增，删，改
		rcContol2.bottom = rcContol2.top + 25;

		m_pBtnSetTrail_MeshFile->ShowWnd(true);
		m_pBtnSetTrail_MeshFile->SetWndRect(&rcContol2);
		rcContol2.top += 30;
		rcContol2.bottom = rcContol2.top + 25;
		m_pBtnSetTrail_TrailFile->ShowWnd(true);
		m_pBtnSetTrail_TrailFile->SetWndRect(&rcContol2);

		m_pConfirm->ShowWnd(true);
		m_pCancel->ShowWnd(true);
		ShowWnd(true);
		return;
	}
}
void WndModelModify::Update()
{

	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();		
		if(pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
		{
			if(!m_pNode)
				return;
			const char *name = m_pNode->GetName();
			const char *value = m_pNode->GetValue();
			const char *parentName = NULL;				
			const char *GrandFatherName = NULL;
			const char *GreatGrandFatherName = NULL;
			ui::TreeView::Node *pCurNodeParent = m_pNode->GetParent();
			ui::TreeView::Node *pNodeGrandfather = NULL;
			ui::TreeView::Node *pNodeGreatGrandfather = NULL;
			if(pCurNodeParent)
			{
				parentName = pCurNodeParent->GetName();
				pNodeGrandfather = pCurNodeParent->GetParent();
				if(pNodeGrandfather)
				{
					GrandFatherName = pNodeGrandfather->GetName();
					pNodeGreatGrandfather = pNodeGrandfather->GetParent();
					if (pNodeGreatGrandfather)
						GreatGrandFatherName = pNodeGreatGrandfather->GetName();
				}
			}
			WndViewer  * pWndViewer	= (WndViewer *)GetParent();
			WndModelTree *pWndModelTree = pWndViewer->GetWndModelTree();
			ui::TreeView *pTreeView = pWndModelTree->GetModelTreeView();
			
			RECT rcControl;
			//点击取消
			m_pCancel->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pCancel-IsVisible())		
			{
				ShowWnd(false);
			}
			//确定并保存
			m_pConfirm->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pConfirm-IsVisible())		
			{

				char strText[128] = "";
				m_pEditNumber->GetEditText(strText,127);
				if (strlen(strText) == 0 && m_pEditNumber->IsVisible())
				{
					MessageBox(NULL,"不能设置成空字符","错误",MB_OK);
					return;
				}
				if (strcmp(parentName,"timer")==0 &&(strcmp(name,"animuvcircle") == 0 || strcmp(name,"animcolorcircle") == 0 ||
					strcmp(name,"animvisibilitycircle") == 0 || strcmp(name,"animtexturecircle") == 0) )
				{
					m_pNode->SetValue(strText);
				} 
				else if(GrandFatherName && (strcmp(parentName,"group")==0 || strcmp(GrandFatherName,"group")==0))
				{
					m_pNode->SetName(strText);
				}
				else if (strcmp(parentName,"trail_emitter") == 0 && GrandFatherName &&strcmp(GrandFatherName,"main") == 0)
				{
					m_pNode->SetName(strText);
				}
				else if (GrandFatherName && strcmp(GrandFatherName,"particle_emitter") == 0 
					&& GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0 && strcmp(name,"number") == 0)
				{
					m_pNode->SetValue(strText);
				}
				else if (strcmp(parentName,"particle_emitter") == 0 && GrandFatherName && strcmp(GrandFatherName,"main") == 0)
				{
					m_pNode->SetName(strText);
				}
				else if (strcmp(name,"action") == 0 && strcmp(parentName,"main") == 0)
				{
					m_pNode->SetName(strText);
				}
				else if (strcmp(parentName,"action") == 0 && GrandFatherName &&strcmp(GrandFatherName,"main") == 0)
				{
					m_pNode->SetName(strText);
				}
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			//添加Group
			m_pBtnAddGroup->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddGroup->IsVisible())
			{
				ui::TreeView::Node *pNewGroupNode = new ui::TreeView::Node;
				pNewGroupNode->SetName("新增加的Group");
				pNewGroupNode->SetParent(m_pNode);
				pTreeView->AppendChildNode(m_pNode,pNewGroupNode);

				//默认增加一个part
				ui::TreeView::Node *pNewPartNode = new ui::TreeView::Node;
				pNewPartNode->SetName("默认part");
				pTreeView->AppendChildNode(pNewGroupNode,pNewPartNode);

				//设置一个默认part文件
				ui::TreeView::Node *pNewPartFileNode = new ui::TreeView::Node;
				pNewPartFileNode->SetName("file");
				pNewPartFileNode->SetValue("armo0001.part");
				pTreeView->AppendChildNode(pNewPartNode,pNewPartFileNode);
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			m_pBtnDelGroup->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddGroup->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);			//删除最后一个Group
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}

			m_pBtnAddPart->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddPart->IsVisible())
			{
				ui::TreeView::Node *pNewPartNode = new ui::TreeView::Node;
				pNewPartNode->SetName("新增加的Part");
				pTreeView->AppendChildNode(m_pNode, pNewPartNode);
				
				//设置一个默认part文件
				ui::TreeView::Node *pNewPartFileNode = new ui::TreeView::Node;
				pNewPartFileNode->SetName("file");
				pNewPartFileNode->SetValue("armo0001.part");
				pTreeView->AppendChildNode(pNewPartNode, pNewPartFileNode);
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}

			m_pBtnDelPart->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnDelPart->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);			//删除最后一个Group
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}

			//设置part文件
			m_pBtnSetPartFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetPartFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"part");
				if (name.length())
				{
					std::vector<ui::TreeView::Node*> *pChildList = m_pNode->GetChildList();
					ui::TreeView::Node *pChildNode = (*pChildList)[0];
					pChildNode->SetValue(name.c_str());
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}
			
			//添加尾迹发射器
			m_pBtnAddTrail_emitter->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddTrail_emitter->IsVisible())
			{
				ui::TreeView::Node *pNewTrailNode = new ui::TreeView::Node;
				pNewTrailNode->SetName("新增加的Trail");
				pTreeView->AppendChildNode(m_pNode, pNewTrailNode);
				
				//添加一个默认的trail
				ui::TreeView::Node *pNewTrail_ShapeNode = new ui::TreeView::Node;
				pNewTrail_ShapeNode->SetName("shape");
				pNewTrail_ShapeNode->SetValue("mesh\\trailX1.mesh");
				pTreeView->AppendChildNode(pNewTrailNode, pNewTrail_ShapeNode);

				ui::TreeView::Node *pNewTrail_TrailNode = new ui::TreeView::Node;
				pNewTrail_TrailNode->SetName("trail");
				pNewTrail_TrailNode->SetValue("blade.trail");
				pTreeView->AppendChildNode(pNewTrailNode, pNewTrail_TrailNode);
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}

			//删除尾迹发射器
			m_pBtnDelTrail_emitter->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnDelTrail_emitter->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);			//删除最后一个Group
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			
			//设置TrailMesh文件
			m_pBtnSetTrail_MeshFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetTrail_MeshFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"mesh");
				char strTile[128] = "mesh\\";
				if (name.length())
				{
					strcat(strTile,name.c_str());
					std::vector<ui::TreeView::Node*> *pChildList = m_pNode->GetChildList();
					ui::TreeView::Node *pChildNode = (*pChildList)[0];
					pChildNode->SetValue(strTile);
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}
			//设置Trail trail文件
			m_pBtnSetTrail_TrailFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetTrail_TrailFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"trail");
				if (name.length())
				{
					std::vector<ui::TreeView::Node*> *pChildList = m_pNode->GetChildList();
					ui::TreeView::Node *pChildNode = (*pChildList)[1];
					pChildNode->SetValue(name.c_str());
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}

			//添加particle_emitter
			m_pBtnAddParticle_emitter->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddParticle_emitter->IsVisible())
			{
				ui::TreeView::Node *pNewParticleNode = new ui::TreeView::Node;
				pNewParticleNode->SetName("新增加的Particle");
				pTreeView->AppendChildNode(m_pNode,pNewParticleNode);

				//创建默认的各节点信息
				ui::TreeView::Node *pNewParticleNode_Shap = new ui::TreeView::Node;
				pNewParticleNode_Shap->SetName("shape");
				pNewParticleNode_Shap->SetValue("mesh\\fire.mesh");
				pTreeView->AppendChildNode(pNewParticleNode,pNewParticleNode_Shap);

				ui::TreeView::Node *pNewParticleNode_TYPE = new ui::TreeView::Node;
				pNewParticleNode_TYPE->SetName("type");
				pNewParticleNode_TYPE->SetValue("LINE");
				pTreeView->AppendChildNode(pNewParticleNode,pNewParticleNode_TYPE);

				ui::TreeView::Node *pNewParticleNode_NUMBER = new ui::TreeView::Node;
				pNewParticleNode_NUMBER->SetName("number");
				pNewParticleNode_NUMBER->SetValue("1");
				pTreeView->AppendChildNode(pNewParticleNode,pNewParticleNode_NUMBER);

				ui::TreeView::Node *pNewParticleNode_PARTICLE = new ui::TreeView::Node;
				pNewParticleNode_PARTICLE->SetName("particle");
				pNewParticleNode_PARTICLE->SetValue("blink4.particle");
				pTreeView->AppendChildNode(pNewParticleNode,pNewParticleNode_PARTICLE);

				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			//删除particle_emitter
			m_pBtnDelParticle_emitter->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnDelParticle_emitter->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);			
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			//设置Particle_MeshFile
			m_pBtnSetParticle_MeshFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetParticle_MeshFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"mesh");
				char strTile[128] = "mesh\\";
				if (name.length())
				{
					strcat(strTile,name.c_str());
					
					m_pNode->SetValue(strTile);
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}
			//设置Particle_particleFile
			m_pBtnSetParticle_ParticleFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetParticle_ParticleFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"particle");
				if (name.length())
				{
					std::vector<ui::TreeView::Node*> *pChildList = m_pNode->GetChildList();
					ui::TreeView::Node *pChildNode = (*pChildList)[3];
					pChildNode->SetValue(name.c_str());
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}
			//添加Action
			m_pBtnAddAction->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnAddAction->IsVisible())
			{
				ui::TreeView::Node *pNewActionNode = new ui::TreeView::Node;
				pNewActionNode->SetName("新增加的ACTION");
				pTreeView->AppendChildNode(m_pNode,pNewActionNode);

				//创建一个默认的action
				ui::TreeView::Node *pNewActionFile = new ui::TreeView::Node;
				pNewActionFile->SetName("file");
				pNewActionFile->SetValue("stnd.anim");
				pTreeView->AppendChildNode(pNewActionNode,pNewActionFile);
				
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			//删除Action
			m_pBtnDelAction->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnDelAction->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);
				pWndModelTree->SaveModelFile();
				ShowWnd(false);
			}
			//设置动作文件
			m_pBtnSetActionFile->GetWndRect(&rcControl);
			if (::PtInRect(&rcControl,pInputInfo->ptMouse) && m_pBtnSetActionFile->IsVisible())
			{
				string name = "";
				OpenFileNamePath(&name,"anim");
				if (name.length())
				{
					m_pNode->SetValue(name.c_str());
					pWndModelTree->SaveModelFile();
					ShowWnd(false);
				}
			}
		}
	}
}
void WndModelModify::Render()
{
	string sTitle;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
		if(m_pNode)
		{
			string sTitle = " 编辑:";
			sTitle	 += m_pNode->GetName();
			if(strcmp(m_pNode->GetValue(),"")!=0)
			{
				sTitle += " = ";
				sTitle += m_pNode->GetValue();
			}
			render::Interface *pInterface = render::Interface::GetInstance();
			render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

			RECT &rc = m_tWindowInfo.rcRect;

			int iX = rc.left, iY = rc.top;//,iW = rc.right - rc.left + 1,;

			int iTextLen = (int)sTitle.size();
			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iX + 1 + 4,iY + 1 + 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX - 1+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX + 1+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX - 1+ 4,iY + 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX + 1+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX - 1+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX+ 4,iY + 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iX+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_WHITE);	
		}
		ui::Wnd::RenderChild();	
	} 
}
BOOL WndModelModify::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	m_pBtnAddGroup = new ui::Button;					//添加GROUP
	m_pBtnDelGroup = new ui::Button;					//删除最后一组Group
	m_pBtnAddAction = new ui::Button;					//添加动作
	m_pBtnDelAction = new ui::Button;					//删除动作
	m_pBtnSetActionFile = new ui::Button;				//指定动作anim文件
	m_pBtnAddPart = new ui::Button;					//添加part组
	m_pBtnDelPart = new ui::Button;					//删除part组
	m_pBtnSetPartFile = new ui::Button;				//设置part文件

	m_pBtnAddTrail_emitter = new ui::Button;			//添加尾迹发射器组
	m_pBtnDelTrail_emitter = new ui::Button;			//删除尾迹发射器组
	m_pBtnSetTrail_MeshFile = new ui::Button;			//设置尾迹MESH文件
	m_pBtnSetTrail_TrailFile = new ui::Button;		//设置尾迹Trail文件

	m_pBtnAddParticle_emitter = new ui::Button;		//添加粒子发射器组
	m_pBtnDelParticle_emitter = new ui::Button;		//删除粒子发射器组
	m_pBtnSetParticle_MeshFile = new ui::Button;		//设置粒子MESH文件
	m_pBtnSetParticle_ParticleFile = new ui::Button;	//设置粒子Trail文件
	m_pEditNumber = new ui::EditBox;					//设置粒子数量

	m_pConfirm = new ui::Button;
	m_pCancel = new ui::Button;

	RECT rcContol = {30, 26, 35+157 , 26+25 };
	
	RECT rcConfirm = {15, 110, 15 + 90, 110+25};
	m_pConfirm->Create("确定/保存",&rcConfirm, FWS_VISIBLE ,this);
	
	RECT rcCancel = {123, 110, 123 + 90, 110+25};
	m_pCancel->Create("取消", &rcCancel, FWS_VISIBLE,this);

	m_pBtnAddGroup->Create("添加Group", &rcContol, FWS_VISIBLE,this);
	m_pBtnDelGroup->Create("删除Group", &rcContol, FWS_VISIBLE,this);

	m_pBtnAddAction->Create("添加Action", &rcContol, FWS_VISIBLE,this);
	m_pBtnDelAction->Create("删除Action", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetActionFile->Create("设置动作文件", &rcContol, FWS_VISIBLE,this);

	m_pBtnAddPart->Create("添加Part", &rcContol, FWS_VISIBLE,this);
	m_pBtnDelPart->Create("删除Part", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetPartFile->Create("设置part文件", &rcContol, FWS_VISIBLE,this);

	m_pBtnAddTrail_emitter->Create("添加Trail", &rcContol, FWS_VISIBLE,this);
	m_pBtnDelTrail_emitter->Create("删除Trail", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetTrail_MeshFile->Create("设置TrailMesh文件", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetTrail_TrailFile->Create("设置Trail文件", &rcContol, FWS_VISIBLE,this);

	m_pBtnAddParticle_emitter->Create("添加Particle", &rcContol, FWS_VISIBLE,this);
	m_pBtnDelParticle_emitter->Create("删除Particle", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetParticle_MeshFile->Create("设置ParticleMesh文件", &rcContol, FWS_VISIBLE,this);
	m_pBtnSetParticle_ParticleFile->Create("设置Particle文件", &rcContol, FWS_VISIBLE,this);
	m_pEditNumber->Create("数字编辑", &rcContol, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE,this);
	return TRUE;
}

const char * WndModelModify::OpenFileNamePath(string *filestring,const char * postfix)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	OPENFILENAME ofn;								// common dialog box structure
	static char szFile[_MAX_PATH] = "";		// buffer for file name
	char strFilter[_MAX_PATH]	= "所有";//"所有part文件  \0*.part";
	
	strcat(strFilter,postfix);
	strcat(strFilter,"文件 (*.");
	strcat(strFilter,postfix);
	strcat(strFilter,")");
	char  cha[4] = "*.";
	
	char * szTemp = &(strFilter[strlen(strFilter) + 1]);
	
	szTemp[0] = cha[0];
	szTemp[1] = cha[1];
	szTemp[2] = postfix[0];
	szTemp[3] = postfix[1];
	szTemp[4] = postfix[2];
	szTemp[5] = postfix[3];
	szTemp[6] = postfix[4];
	szTemp[7] = postfix[5];
	szTemp[8] = postfix[6];
	szTemp[9] = postfix[7];
	szTemp[10] = postfix[8];
	szTemp[11] = postfix[9];
	//  所有part文件\0*.part\0
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pInterface->GetWndHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = "";
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	static char szOldDirectory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH ,szOldDirectory);
	char strName[128] = "";
	if(GetOpenFileName(&ofn))
	{
		*filestring = utility::File::GetFileName(szFile);
	}
	return (*filestring).c_str();
}

