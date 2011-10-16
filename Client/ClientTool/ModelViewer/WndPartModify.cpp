#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndModel.h"
#include "WndModelTree.h"
#include "WndModelFileModify.h"
#include "WndPartModify.h"
#include "WndPartTree.h"
WndPartModify::WndPartModify(void)
{
	m_Check[0] = NULL;
	m_Check[1] = NULL;
	m_pBtnConfirm = NULL;		//确定保存
	m_pBtnCancel = NULL;		//取消
	m_pCombo = NULL;
	m_pNode = NULL;
	m_pBtnSetMeshFile = NULL;

	m_pAnimcolorCheck = NULL;		
	m_pAnimuvCheck = NULL;
	m_pAnimvisibilityCheck = NULL;
	m_pAnimtextureCheck = NULL;

	m_pBtnSetAnimcolor = NULL;
	m_pBtnSetAnimuv = NULL;
	m_pBtnSetAnimvisibility = NULL;
	m_pBtnSetAnimtexture = NULL;

	m_pBtnAddRenderMesh = NULL;	
	m_pBtnDelRenderMesh = NULL;	

	m_pBtnAddLevel = NULL;
	m_pBtnDelLevel = NULL;

	m_pBtnAddMeshIndex = NULL;
	m_pBtnDelMeshIndex = NULL;

	m_pComboMeshList = NULL;
	m_pEnviromentmapsCheck = NULL;
	m_pBtnAddRenderMeshItem = NULL;
	m_pBtnDelRenderMeshItem = NULL;
}
WndPartModify::~WndPartModify(void)
{

}

BOOL WndPartModify::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rect = {20, 24, 20+145 , 24+25 };
	m_Check[0] = new ui::CheckBox;
	m_Check[0]->Create("true",&rect, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rect2 = {20,24+35,20+145,24+35+25};
	m_Check[1] = new ui::CheckBox;
	m_Check[1]->Create("false",&rect2, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rcConfirm = {10, 110, 10 + 80, 110+25};
	m_pBtnConfirm = new ui::Button;		//确定保存
	m_pBtnConfirm->Create("确定/保存",&rcConfirm, FWS_VISIBLE, this);

	RECT rcCancel = {100, 110, 100 + 80, 110+25};
	m_pBtnCancel = new ui::Button;		//取消
	m_pBtnCancel->Create("取消",&rcCancel, FWS_VISIBLE, this);

	m_pCombo = new ui::Combo;
	m_pCombo->Create("alpha方式",&rect, FWS_VISIBLE , this);
	m_pCombo->InsertItem("test");
	m_pCombo->InsertItem("blend");
	m_pCombo->SetEditReadOnly(true);
	m_pBtnSetMeshFile = new ui::Button;
	m_pBtnSetMeshFile->Create("设置MESH文件",&rect, FWS_VISIBLE, this);

	RECT rcAcCheck = {20,20,20+146,20+18};
	m_pAnimcolorCheck = new ui::CheckBox;
	m_pAnimcolorCheck->Create("animcolor",&rcAcCheck, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rcUvCheck = {20,41,20+146,41+18};
	m_pAnimuvCheck = new ui::CheckBox;
	m_pAnimuvCheck->Create("Animuv",&rcUvCheck, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rcVisCheck = {20,62,20+146,62+18};
	m_pAnimvisibilityCheck = new ui::CheckBox;
	m_pAnimvisibilityCheck->Create("Animvisibility",&rcVisCheck, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rcAtCheck = {20,83,20+146,83+18};
	m_pAnimtextureCheck = new ui::CheckBox;
	m_pAnimtextureCheck->Create("Animtexture",&rcAtCheck, FWS_VISIBLE | FWS_FILLBKG , this);

	RECT rcEnviroCheck = {20,104,20+146,104+18};
	m_pEnviromentmapsCheck = new ui::CheckBox;
	m_pEnviromentmapsCheck->Create("Enviromentmaps",&rcEnviroCheck, FWS_VISIBLE | FWS_FILLBKG , this);
	m_pEnviromentmapsCheck->ShowWnd(false);

	m_pBtnSetAnimcolor = new ui::Button;
	m_pBtnSetAnimcolor->Create("设置Animcolor文件",&rect, FWS_VISIBLE, this);

	m_pBtnSetAnimuv = new ui::Button;
	m_pBtnSetAnimuv->Create("设置Animuv文件",&rect, FWS_VISIBLE, this);

	m_pBtnSetAnimvisibility = new ui::Button;
	m_pBtnSetAnimvisibility->Create("设置Animvisibility文件",&rect, FWS_VISIBLE, this);

	m_pBtnSetAnimtexture = new ui::Button;
	m_pBtnSetAnimtexture->Create("设置Animtexture文件",&rect, FWS_VISIBLE, this);

	m_pBtnAddRenderMesh = new ui::Button;
	m_pBtnAddRenderMesh->Create("添加RenderMesh组",&rect, FWS_VISIBLE, this);
	
	m_pBtnDelRenderMesh = new ui::Button;
	m_pBtnDelRenderMesh->Create("删除RenderMesh组",&rect2, FWS_VISIBLE, this);
	
	
	m_pBtnAddLevel = new ui::Button;
	m_pBtnAddLevel->Create("添加Level组",&rcConfirm, FWS_VISIBLE, this);

	m_pBtnDelLevel = new ui::Button;
	m_pBtnDelLevel->Create("删除Level组",&rcCancel, FWS_VISIBLE, this);

	m_pBtnAddMeshIndex = new ui::Button;
	m_pBtnAddMeshIndex->Create("增加MeshIndex",&rect, FWS_VISIBLE, this);
	m_pBtnDelMeshIndex = new ui::Button;
	m_pBtnDelMeshIndex->Create("删除MeshIndex",&rect2, FWS_VISIBLE, this);

	m_pComboMeshList = new ui::Combo;
	m_pComboMeshList->Create("MeshIndex",&rect, FWS_VISIBLE, this);

	RECT rcAddItem = {2, 125, 2 + 90, 125+20};
	m_pBtnAddRenderMeshItem = new ui::Button;
	m_pBtnAddRenderMeshItem->Create("添加RenderItem",&rcAddItem, FWS_VISIBLE, this);
	m_pBtnAddRenderMeshItem->ShowWnd(false);

	RECT rcDelItem = {95, 125, 90 + 90, 125+20};
	m_pBtnDelRenderMeshItem = new ui::Button;
	m_pBtnDelRenderMeshItem->Create("删除RenderItem",&rcDelItem, FWS_VISIBLE, this);
	m_pBtnDelRenderMeshItem->ShowWnd(false);

	return true;
}
void WndPartModify::Destroy()
{
	m_pNode = NULL;
	SAFEDESTROY(m_Check[0]);
	SAFEDESTROY(m_Check[1]);
	SAFEDESTROY(m_pBtnConfirm);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pCombo);
	SAFEDESTROY(m_pBtnSetMeshFile);

	SAFEDESTROY(m_pAnimcolorCheck);		
	SAFEDESTROY(m_pAnimuvCheck);
	SAFEDESTROY(m_pAnimvisibilityCheck);
	SAFEDESTROY(m_pAnimtextureCheck);

	SAFEDESTROY(m_pBtnSetAnimcolor);
	SAFEDESTROY(m_pBtnSetAnimuv);
	SAFEDESTROY(m_pBtnSetAnimvisibility);
	SAFEDESTROY(m_pBtnSetAnimtexture);

	SAFEDESTROY(m_pBtnAddRenderMesh);
	SAFEDESTROY(m_pBtnDelRenderMesh);

	SAFEDESTROY(m_pBtnAddLevel);
	SAFEDESTROY(m_pBtnDelLevel);

	SAFEDESTROY(m_pBtnAddMeshIndex);
	SAFEDESTROY(m_pBtnDelMeshIndex);
	SAFEDESTROY(m_pComboMeshList);

	SAFEDESTROY(m_pEnviromentmapsCheck);
	SAFEDESTROY(m_pBtnAddRenderMeshItem);
	SAFEDESTROY(m_pBtnDelRenderMeshItem);

	SAFEDESTROY(m_pEnviromentmapsCheck);
	SAFEDESTROY(m_pBtnAddRenderMeshItem);
	SAFEDESTROY(m_pBtnDelRenderMeshItem);
	ui::Wnd::Destroy();
}

void WndPartModify::SetCurPartNode(ui::TreeView::Node *pNode)
{
	if (m_pNode == pNode)
		return;
	m_pNode = pNode;

}

void WndPartModify::CaculateToDisplay()
{
	if(!m_pNode)
		return;
	m_Check[0]->ShowWnd(false);
	m_Check[1]->ShowWnd(false);
	m_pBtnConfirm->ShowWnd(false);
	m_pBtnCancel->ShowWnd(false);
	m_pCombo->ShowWnd(false);
	m_pBtnSetMeshFile->ShowWnd(false);

	m_pAnimcolorCheck->ShowWnd(false);
	m_pAnimuvCheck->ShowWnd(false);
	m_pAnimvisibilityCheck->ShowWnd(false);
	m_pAnimtextureCheck->ShowWnd(false);

	m_pBtnSetAnimcolor->ShowWnd(false);
	m_pBtnSetAnimuv->ShowWnd(false);
	m_pBtnSetAnimvisibility->ShowWnd(false);
	m_pBtnSetAnimtexture->ShowWnd(false);

	m_pBtnAddRenderMesh->ShowWnd(false);
	m_pBtnDelRenderMesh->ShowWnd(false);

	m_pBtnAddLevel->ShowWnd(false);
	m_pBtnDelLevel->ShowWnd(false);

	m_pBtnAddMeshIndex->ShowWnd(false);
	m_pBtnDelMeshIndex->ShowWnd(false);
	m_pComboMeshList->ShowWnd(false);

	m_pEnviromentmapsCheck->ShowWnd(false);
	m_pBtnAddRenderMeshItem->ShowWnd(false);
	m_pBtnDelRenderMeshItem->ShowWnd(false);

	ShowWnd(false);
	const char * name = m_pNode->GetName();
	const char * value = m_pNode->GetValue();
	const char * ParentName = NULL;
	const char * GrandFatherName = NULL;
	ui::TreeView::Node *pParentNode = m_pNode->GetParent();
	ui::TreeView::Node *pGrandFatherNode = NULL;
	
	if(pParentNode)
	{
		pGrandFatherNode = pParentNode->GetParent();
		ParentName = pParentNode->GetName();
	}
	if(pGrandFatherNode)
		GrandFatherName = pGrandFatherNode->GetName();
	if(strcmp(name,"billboard") == 0 || strcmp(name,"y_billboard") == 0 || 
	   strcmp(name,"animuv") == 0 || strcmp(name,"animcolor") == 0 ||
	   strcmp(name,"animtexture") == 0 || strcmp(name,"animvisibility") == 0 ||
	   strcmp(name,"effect_by_light") == 0 || strcmp(name,"specular") == 0 ||
	   strcmp(name,"environmentmaps") == 0)
	{
		if(GrandFatherName && strcmp(GrandFatherName,"mesharray") == 0)
		{
			m_Check[0]->ShowWnd(true);
			m_Check[1]->ShowWnd(true);
			if(strcmp(value,"true") == 0)
			{
				m_Check[0]->SetChecked(true);
				m_Check[1]->SetChecked(false);
			}
			else
			{
				m_Check[0]->SetChecked(false);
				m_Check[1]->SetChecked(true);
			}
			m_pBtnConfirm->ShowWnd(true);
			m_pBtnCancel->ShowWnd(true);
		}
		else if(strcmp(name,"animcolor") == 0)
		{
			m_pBtnSetAnimcolor->ShowWnd(true);
		}
		else if(strcmp(name,"animuv") == 0)
		{
			m_pBtnSetAnimuv->ShowWnd(true);
		}
		else if(strcmp(name,"animvisibility") == 0)
		{
			m_pBtnSetAnimvisibility->ShowWnd(true);
		}
		else if(strcmp(name,"animtexture") == 0)
		{
			m_pBtnSetAnimtexture->ShowWnd(true);
		}
		ShowWnd(true);
	}
	else if(strcmp(name,"alphamode") == 0)
	{
		m_pCombo->SetEditText(value);
		m_pCombo->ShowWnd(true);
		m_pBtnConfirm->ShowWnd(true);
		m_pBtnCancel->ShowWnd(true);
		ShowWnd(true);
	}
	else if(strcmp(name,"mesh") == 0)
	{
		m_pBtnSetMeshFile->ShowWnd(true);
		ShowWnd(true);
	}
	else if(strcmp(name,"mesharray") == 0)
	{
		m_pBtnAddRenderMesh->ShowWnd(true);
		m_pBtnDelRenderMesh->ShowWnd(true);
		ShowWnd(true);
	}
	else if (strcmp(name,"levelarray") == 0)
	{
		m_pBtnAddLevel->ShowWnd(true);
		m_pBtnDelLevel->ShowWnd(true);
		m_pAnimcolorCheck->ShowWnd(true);
		m_pAnimuvCheck->ShowWnd(true);
		m_pAnimvisibilityCheck->ShowWnd(true);
		m_pAnimtextureCheck->ShowWnd(true);
		ShowWnd(true);
	}
	else if(GrandFatherName && strcmp(GrandFatherName,"levelarray") == 0)
	{
		m_pBtnAddMeshIndex->ShowWnd(true);
		m_pBtnDelMeshIndex->ShowWnd(true);
		ShowWnd(true);
	}
	else if(strcmp(ParentName,"mesh_index") == 0)
	{
		WndViewer  * pWndViewer	= (WndViewer *)GetParent();
		WndPartTree *pWndPartTree = pWndViewer->GetWndPartTree();
		ui::TreeView *pTreeView = pWndPartTree->GetPartTreeView();
		ui::TreeView::Node *pNode = pTreeView->GetNodeByName(pTreeView->GetRootNode(),"mesharray");
		char strItem[20];
		m_pComboMeshList->Clear();
		for(int i = 0; i < (int)pNode->GetChildCount(); i++)
		{
			sprintf(strItem,"%d",i);
			m_pComboMeshList->InsertItem(strItem);
		}
		m_pComboMeshList->ShowWnd(true);
		m_pComboMeshList->SetEditText(value);
		m_pBtnConfirm->ShowWnd(true);
		m_pBtnCancel->ShowWnd(true);
		ShowWnd(true);
	}
	else if (strcmp(ParentName,"mesharray") == 0)
	{
		m_pAnimcolorCheck->ShowWnd(true);
		m_pAnimuvCheck->ShowWnd(true);
		m_pAnimvisibilityCheck->ShowWnd(true);
		m_pAnimtextureCheck->ShowWnd(true);
		m_pEnviromentmapsCheck->ShowWnd(true);
		
		std::vector<ui::TreeView::Node*> *vecList = m_pNode->GetChildList(); 
		int count = 0;
		for (int i=0; i < (int)m_pNode->GetChildCount(); i++)
		{
			ui::TreeView::Node *PChildNode = (*vecList)[i];
			if (strcmp(PChildNode->GetName(),"animcolor") == 0)
			{	
				m_pAnimcolorCheck->ShowWnd(false);	
				count++;	
			}
			if (strcmp(PChildNode->GetName(),"animuv") == 0)
			{	
				m_pAnimuvCheck->ShowWnd(false);	
				count++;	
			}
			if (strcmp(PChildNode->GetName(),"animtexture") == 0)
			{	
				m_pAnimtextureCheck->ShowWnd(false);	
				count++;	
			}
			if (strcmp(PChildNode->GetName(),"animvisibility") == 0)
			{	
				m_pAnimvisibilityCheck->ShowWnd(false);	
				count++;	
			}
			if (strcmp(PChildNode->GetName(),"environmentmaps") == 0)
			{	
				m_pEnviromentmapsCheck->ShowWnd(false);	
				count++;	
			}
		}
		
		RECT rc[5];
		for (int i = 0;i<5-count;i++)
		{
			rc[i].left = 754;
			rc[i].right = 754+146;
			rc[i].top = 46 + i * 21;
			rc[i].bottom = rc[i].top + 18;
		}
		int j = 0;
		if (m_pAnimcolorCheck->IsVisible())
			m_pAnimcolorCheck->SetWndRect(&rc[j++]);
		if (m_pAnimuvCheck->IsVisible())
			m_pAnimuvCheck->SetWndRect(&rc[j++]);
		if	(m_pAnimvisibilityCheck->IsVisible())
			m_pAnimvisibilityCheck->SetWndRect(&rc[j++]);
		if	(m_pAnimtextureCheck->IsVisible())
			m_pAnimtextureCheck->SetWndRect(&rc[j++]);
		if	(m_pEnviromentmapsCheck->IsVisible())
			m_pEnviromentmapsCheck->SetWndRect(&rc[j++]);
		if (count<5)
		{
			m_pBtnAddRenderMeshItem->ShowWnd(true);
			RECT rectDEl = {829, 150, 829+88, 150+20};
			m_pBtnDelRenderMeshItem->SetWndRect(&rectDEl);
		}
		else
		{
			RECT rectDEl = {772,100,772+115,100+20};
			m_pBtnAddRenderMeshItem->ShowWnd(false);
			m_pBtnDelRenderMeshItem->SetWndRect(&rectDEl);

		}
		m_pBtnDelRenderMeshItem->ShowWnd(true);
		ShowWnd(true);
	}
}
void WndPartModify::Update()
{
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();	
		if(pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
		{
			WndViewer  * pWndViewer	= (WndViewer *)GetParent();
			WndModelModify *pModelModify = pWndViewer->GetWndModelModify();	
			WndPartTree *pWndPartTree = pWndViewer->GetWndPartTree();
			ui::TreeView *pTreeView = pWndPartTree->GetPartTreeView();
			//CheckBox Click
			RECT rect;
			m_Check[0]->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_Check[0]->GetStyle())
			{
				int flag = m_Check[0]->IsChecked();
				m_Check[1]->SetChecked(!flag);
			}
			m_Check[1]->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_Check[1]->GetStyle())
			{
				int flag = m_Check[1]->IsChecked();
				m_Check[0]->SetChecked(!flag);
			}
			//设置MeshArray下的mesh文件
			m_pBtnSetMeshFile->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnSetMeshFile->IsVisible())
			{	
				string filename;
				pModelModify->OpenFileNamePath(&filename,"mesh");
				char temp[128] = "mesh\\";
				if(filename.length())
				{
					strcat(temp,filename.c_str());
					m_pNode->SetValue(temp);
					pWndPartTree->SavePartFile();
					ShowWnd(false);
				}
			}
			//点击取消按钮
			m_pBtnCancel->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnCancel->IsVisible())
			{
				ShowWnd(false);
			}
			//点击确定按钮
			m_pBtnConfirm->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnConfirm->IsVisible())
			{
				const char *name = m_pNode->GetName();
				const char *parentname = m_pNode->GetParent()->GetName();
				if((strcmp(name,"billboard") == 0 || strcmp(name,"y_billboard") == 0 || 
					strcmp(name,"animuv") == 0 || strcmp(name,"animcolor") == 0 ||
					strcmp(name,"animtexture") == 0 || strcmp(name,"animvisibility") == 0 ||
					strcmp(name,"effect_by_light") == 0 || strcmp(name,"specular") == 0 ||
					strcmp(name,"environmentmaps") == 0))
				{
					int setValue = m_Check[0]->IsChecked();
					if(setValue)
						m_pNode->SetValue("true");
					else
						m_pNode->SetValue("false");
				}
				else if(strcmp(name,"alphamode") == 0 && m_pCombo->IsVisible())
				{
					char value[128] = "";
					m_pCombo->GetEditText(value,127);
					m_pNode->SetValue(value);
				}
				else if(strcmp(parentname,"mesh_index") == 0)
				{
					char value[128] = "";
					m_pComboMeshList->GetEditText(value,127);
					m_pNode->SetValue(value);
				}
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			//添加一组RenderMesh
			m_pBtnAddRenderMesh->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnAddRenderMesh->IsVisible())
			{
				ui::TreeView::Node *pNewRenderMesh = new ui::TreeView::Node;
				char NodeName[64] = "";
				sprintf(NodeName,"%d",m_pNode->GetChildCount());
				pNewRenderMesh->SetName(NodeName);
				pTreeView->AppendChildNode(m_pNode,pNewRenderMesh);

				//添加RenderMesh各项节点
				ui::TreeView::Node *pNewNodeBillBoard = new ui::TreeView::Node;
				pNewNodeBillBoard->SetName("billboard");
				pNewNodeBillBoard->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeBillBoard);

				ui::TreeView::Node *pNewNodeY_Billboard = new ui::TreeView::Node;
				pNewNodeY_Billboard->SetName("y_billboard");
				pNewNodeY_Billboard->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeY_Billboard);

				ui::TreeView::Node *pNewNodeAnimuv = new ui::TreeView::Node;
				pNewNodeAnimuv->SetName("animuv");
				pNewNodeAnimuv->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeAnimuv);

				ui::TreeView::Node *pNewNodeAlphamode = new ui::TreeView::Node;
				pNewNodeAlphamode->SetName("alphamode");
				pNewNodeAlphamode->SetValue("test");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeAlphamode);

				ui::TreeView::Node *pNewNodeAnimcolor = new ui::TreeView::Node;
				pNewNodeAnimcolor->SetName("animcolor");
				pNewNodeAnimcolor->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeAnimcolor);

				ui::TreeView::Node *pNewNodeAnimtexture = new ui::TreeView::Node;
				pNewNodeAnimtexture->SetName("animtexture");
				pNewNodeAnimtexture->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeAnimtexture);

				ui::TreeView::Node *pNewNodeAnimvisibility = new ui::TreeView::Node;
				pNewNodeAnimvisibility->SetName("animvisibility");
				pNewNodeAnimvisibility->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeAnimvisibility);

				ui::TreeView::Node *pNewNodeEffect_by_light = new ui::TreeView::Node;
				pNewNodeEffect_by_light->SetName("effect_by_light");
				pNewNodeEffect_by_light->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeEffect_by_light);

				ui::TreeView::Node *pNewNodeSpecular = new ui::TreeView::Node;
				pNewNodeSpecular->SetName("specular");
				pNewNodeSpecular->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeSpecular);

				ui::TreeView::Node *pNewNodeEnvironmentmaps = new ui::TreeView::Node;
				pNewNodeEnvironmentmaps->SetName("environmentmaps");
				pNewNodeEnvironmentmaps->SetValue("false");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeEnvironmentmaps);

				ui::TreeView::Node *pNewNodeMesh = new ui::TreeView::Node;
				pNewNodeMesh->SetName("mesh");
				pNewNodeMesh->SetValue("mesh\\SM_01_h.mesh");
				pTreeView->AppendChildNode(pNewRenderMesh,pNewNodeMesh);

				//保存
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			//删除一组RenderMesh
			m_pBtnDelRenderMesh->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnDelRenderMesh->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);
				//保存
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}

			//添加一组LevelArray
			m_pBtnAddLevel->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnAddLevel->IsVisible())
			{
				ui::TreeView::Node *pNewNodeLevelArray = new ui::TreeView::Node;
				char strName[32] = "";
				sprintf(strName,"level%d",m_pNode->GetChildCount());
				pNewNodeLevelArray->SetName(strName);
				pTreeView->AppendChildNode(m_pNode,pNewNodeLevelArray);

				//添加level组下各项目
				ui::TreeView::Node *pNewNodeMeshIndex = new ui::TreeView::Node;
				pNewNodeMeshIndex->SetName("mesh_index");
				pTreeView->AppendChildNode(pNewNodeLevelArray,pNewNodeMeshIndex);

				ui::TreeView::Node *pNewNodeMeshIndexItem = new ui::TreeView::Node;
				pNewNodeMeshIndexItem->SetName("0");
				pNewNodeMeshIndexItem->SetValue("0");
				pTreeView->AppendChildNode(pNewNodeMeshIndex,pNewNodeMeshIndexItem);

				if(m_pAnimcolorCheck->IsChecked())
				{
					ui::TreeView::Node *pNewNodeAnimcolor = new ui::TreeView::Node;
					pNewNodeAnimcolor->SetName("animcolor");
					pNewNodeAnimcolor->SetValue("0002lv08.ac");
					pTreeView->AppendChildNode(pNewNodeLevelArray,pNewNodeAnimcolor);
				}
				if(m_pAnimuvCheck->IsChecked())
				{
					ui::TreeView::Node *pNewNodeAnimcolor = new ui::TreeView::Node;
					pNewNodeAnimcolor->SetName("animuv");
					pNewNodeAnimcolor->SetValue("flud1.uv");
					pTreeView->AppendChildNode(pNewNodeLevelArray,pNewNodeAnimcolor);
				}
				if(m_pAnimvisibilityCheck->IsChecked())
				{
					ui::TreeView::Node *pNewNodeAnimvisibility = new ui::TreeView::Node;
					pNewNodeAnimvisibility->SetName("animvisibility");
					pNewNodeAnimvisibility->SetValue("fade_01.vis");
					pTreeView->AppendChildNode(pNewNodeLevelArray,pNewNodeAnimvisibility);
				}
				if(m_pAnimtextureCheck->IsChecked())
				{
					ui::TreeView::Node *pNewNodeAnimtexture = new ui::TreeView::Node;
					pNewNodeAnimtexture->SetName("animtexture");
					pNewNodeAnimtexture->SetValue("2x2.at");
					pTreeView->AppendChildNode(pNewNodeLevelArray,pNewNodeAnimtexture);
				}
				//保存
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			//删除一个LevelArray
			m_pBtnDelLevel->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnAddLevel->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			//设置AC文件
			m_pBtnSetAnimcolor->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnSetAnimcolor->IsVisible())
			{
				string filename;
				pModelModify->OpenFileNamePath(&filename,"ac");
				if(filename.length())
				{
					m_pNode->SetValue(filename.c_str());
					pWndPartTree->SavePartFile();
					ShowWnd(false);
				}
			}
			//设置UV文件
			m_pBtnSetAnimuv->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnSetAnimuv->IsVisible())
			{
				string filename;
				pModelModify->OpenFileNamePath(&filename,"uv");
				if(filename.length())
				{
					m_pNode->SetValue(filename.c_str());
					pWndPartTree->SavePartFile();
					ShowWnd(false);
				}
			}
			//设置VIS文件
			m_pBtnSetAnimvisibility->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnSetAnimvisibility->IsVisible())
			{
				string filename;
				pModelModify->OpenFileNamePath(&filename,"vis");
				if(filename.length())
				{
					m_pNode->SetValue(filename.c_str());
					pWndPartTree->SavePartFile();
					ShowWnd(false);
				}
			}
			//设置AT文件
			m_pBtnSetAnimtexture->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnSetAnimtexture->IsVisible())
			{
				string filename;
				pModelModify->OpenFileNamePath(&filename,"at");
				if(filename.length())
				{
					m_pNode->SetValue(filename.c_str());
					pWndPartTree->SavePartFile();
					ShowWnd(false);
				}
			}
			//增加一组MeshIndex
			m_pBtnAddMeshIndex->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnAddMeshIndex->IsVisible())
			{
				ui::TreeView::Node *pNewNodeIndex = new ui::TreeView::Node;
				char strName[64] = "";
				sprintf(strName,"%d",m_pNode->GetChildCount());
				pNewNodeIndex->SetName(strName);
				pNewNodeIndex->SetValue("0");
				pTreeView->AppendChildNode(m_pNode,pNewNodeIndex);
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			//删除一组MeshIndex
			m_pBtnDelMeshIndex->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnDelMeshIndex->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			m_pBtnAddRenderMeshItem->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnAddRenderMeshItem->IsVisible())
			{
				if (m_pAnimcolorCheck->IsChecked())
				{
					ui::TreeView::Node *pNode_AnimColor = new ui::TreeView::Node;
					pNode_AnimColor->SetName("animcolor");
					pNode_AnimColor->SetValue("false");
					pTreeView->AppendChildNode(m_pNode,pNode_AnimColor);
				}
				if (m_pAnimuvCheck->IsChecked())
				{
					ui::TreeView::Node *pNode_AnimUV = new ui::TreeView::Node;
					pNode_AnimUV->SetName("animuv");
					pNode_AnimUV->SetValue("false");
					pTreeView->AppendChildNode(m_pNode,pNode_AnimUV);
				}
				if (m_pAnimtextureCheck->IsChecked())
				{
					ui::TreeView::Node *pNode_Animtexture = new ui::TreeView::Node;
					pNode_Animtexture->SetName("animtexture");
					pNode_Animtexture->SetValue("false");
					pTreeView->AppendChildNode(m_pNode,pNode_Animtexture);
				}
				if (m_pAnimvisibilityCheck->IsChecked())
				{
					ui::TreeView::Node *pNode_Animvisibility = new ui::TreeView::Node;
					pNode_Animvisibility->SetName("animvisibility");
					pNode_Animvisibility->SetValue("false");
					pTreeView->AppendChildNode(m_pNode,pNode_Animvisibility);
				}
				if (m_pEnviromentmapsCheck->IsChecked())
				{
					ui::TreeView::Node *pNewNodeEnviromentmaps = new ui::TreeView::Node;
					pNewNodeEnviromentmaps->SetName("environmentmaps");
					pNewNodeEnviromentmaps->SetValue("false");
					pTreeView->AppendChildNode(m_pNode,pNewNodeEnviromentmaps);
				}
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
			m_pBtnDelRenderMeshItem->GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnDelRenderMeshItem->IsVisible())
			{
				pTreeView->DeleteLastNode(m_pNode);
				pWndPartTree->SavePartFile();
				ShowWnd(false);
			}
		}
	}
}
void WndPartModify::Render()
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