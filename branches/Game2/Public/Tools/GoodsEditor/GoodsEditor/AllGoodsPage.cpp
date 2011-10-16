// AllGoodsPage.cpp : 实现文件
//

#include "stdafx.h"
#include "GoodsEditor.h"
#include "AllGoodsPage.h"
#include ".\allgoodspage.h"
#include "ContainerPage.h"
#include "AllGoodsTree.h"


// CAllGoodsPage
extern int GoodID;
extern int MaxID;
extern vector<tagGoodAttr> vecGoodAttr; 
extern const char* strGoodType[];

IMPLEMENT_DYNCREATE(CAllGoodsPage, CFormView)

CAllGoodsPage::CAllGoodsPage()
	: CFormView(CAllGoodsPage::IDD)
{
	m_bInit=false;
	m_bSortFlag=false;
}

CAllGoodsPage::~CAllGoodsPage()
{
}

void CAllGoodsPage::InitMyCtrls()
{	
	m_cAllGoodsList.InsertColumn(0,"ID",LVCFMT_LEFT,50);
	m_cAllGoodsList.InsertColumn(1,"原始名",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(2,"名称",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(3,"存DB",LVCFMT_LEFT,50);
	m_cAllGoodsList.InsertColumn(4,"界面ID",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(5,"落地ID",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(6,"换装ID",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(7,"金币价格",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(8,"银币价格",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(9,"物品类别",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(10,"拾取声音",LVCFMT_LEFT,60);
	m_cAllGoodsList.InsertColumn(11,"描述",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(12,"挥动/弱伤声音ID",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(13,"被击中声音ID",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(14,"攻击时混音",LVCFMT_LEFT,100);
	m_cAllGoodsList.InsertColumn(15,"等级限制", LVCFMT_LEFT,100);
	m_cAllGoodsList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	
    
	
	int i=0;
	vector<tagGoodAttr>::iterator iter;	
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,i++)
	{			
		int nCurItem=m_cAllGoodsList.InsertItem(0xffff,"");
		char strID[500],strUIID[500],strGround[500],strCosplay[500],
			strPrice[500],strSilverPrice[500],strSound[500],strSound1[500],strSound2[500];
		sprintf_s(strID,"%d",iter->dwIndex);
		
		m_cAllGoodsList.SetItem(i,0,LVIF_TEXT,strID,NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,1,LVIF_TEXT,iter->strOrigName.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,2,LVIF_TEXT,iter->strName.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,3,LVIF_TEXT,iter->bSave?"是":"否",NULL,0,0,0);		
		
		sprintf_s(strUIID,"%d",iter->dwIconId);
        m_cAllGoodsList.SetItem(i,4,LVIF_TEXT,strUIID,NULL,0,0,0);
		
		sprintf_s(strGround,"%d",iter->dwGroundId);
		m_cAllGoodsList.SetItem(i,5,LVIF_TEXT,strGround,NULL,0,0,0);
		
		sprintf_s(strCosplay,"%d",iter->dwEquipID);
		m_cAllGoodsList.SetItem(i,6,LVIF_TEXT,strCosplay,NULL,0,0,0);
		
		sprintf_s(strPrice,"%d",iter->dwValue);
		m_cAllGoodsList.SetItem(i,7,LVIF_TEXT,strPrice,NULL,0,0,0);

		//银币价格
		sprintf_s(strSilverPrice,"%d",iter->dwSilverValue);
		m_cAllGoodsList.SetItem(i,8,LVIF_TEXT,strSilverPrice,NULL,0,0,0);

		//类别
		m_cAllGoodsList.SetItem(i,9,LVIF_TEXT,strGoodType[iter->dwType],NULL,0,0,0);		
		
		//拾取声音	
		sprintf_s(strSound,"%d",iter->dwSound);		
		m_cAllGoodsList.SetItem(i,10,LVIF_TEXT,strSound,NULL,0,0,0);
		
		//描述
		m_cAllGoodsList.SetItem(i,11,LVIF_TEXT,iter->strContent.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItemData(nCurItem,i);

		
		//挥动/弱伤声音
		sprintf_s(strSound1,"%d",iter->dwSoundID1);		
		m_cAllGoodsList.SetItem(i,12,LVIF_TEXT,strSound1,NULL,0,0,0);

		//被击中声音ID
		sprintf_s(strSound2,"%d",iter->dwSoundID2);		
		m_cAllGoodsList.SetItem(i,13,LVIF_TEXT,strSound2,NULL,0,0,0);				
		
		//攻击时候是否混音
		m_cAllGoodsList.SetItem(i,14,LVIF_TEXT,iter->bSoundSwitch?"是":"否",NULL,0,0,0);

		// 等级限制 默认为0级
		sprintf_s(strSound1,"%d",0);		
		m_cAllGoodsList.SetItem(i,15,LVIF_TEXT,strSound1,NULL,0,0,0);
		vector<tagAddAttr>& vecAddAttr = iter->vecAddAttr;
		vector<tagAddAttr>::iterator itAddAttr = vecAddAttr.begin();
		for (; itAddAttr!=vecAddAttr.end(); ++itAddAttr)
		{
			if (itAddAttr->wType == 7)
			{
				sprintf_s(strSound1,"%d",itAddAttr->lValue1);		
				m_cAllGoodsList.SetItem(i,15,LVIF_TEXT,strSound1,NULL,0,0,0);
				break;
			}
		}
	}
	m_bInit=true;
}

void CAllGoodsPage::DoDataExchange(CDataExchange* pDX)
{
	if(!m_bInit)
	{

		CFormView::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_ALLGOODSLIST, m_cAllGoodsList);
		InitMyCtrls();
		
	}
}

BEGIN_MESSAGE_MAP(CAllGoodsPage, CFormView)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALLGOODSLIST, OnLvnItemchangedAllgoodslist)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickAllgoodslist)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLGOODSLIST, OnNMDblclkAllgoodslist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_ALLGOODSLIST, OnLvnKeydownAllgoodslist)
	ON_NOTIFY(NM_RETURN, IDC_ALLGOODSLIST, OnNMReturnAllgoodslist)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ALLGOODSLIST, OnLvnColumnclickAllgoodslist)
END_MESSAGE_MAP()


// CAllGoodsPage 诊断

#ifdef _DEBUG
void CAllGoodsPage::AssertValid() const
{
	CFormView::AssertValid();
}

void CAllGoodsPage::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CAllGoodsPage 消息处理程序

void CAllGoodsPage::OnLvnItemchangedAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CAllGoodsPage::OnHdnItemdblclickAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;	
}

void CAllGoodsPage::OnNMDblclkAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;	
    POSITION pos=m_cAllGoodsList.GetFirstSelectedItemPosition();	
	if(pos)
	{
		int nSelected=m_cAllGoodsList.GetNextSelectedItem(pos);
		int nVecIndex=0;
		char strVal[500];
		m_cAllGoodsList.GetItemText(nSelected,1,strVal,500);
		CContainerPage *pContainer=(CContainerPage*)GetParent();
		CSplitterWnd *pSplitter=(CSplitterWnd*)pContainer->GetParent();
		CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)pSplitter->GetPane(0,0);
		pContainer->m_wndGoodAttrPage.SetEditFlag(false);
		pContainer->m_wndGoodAttrPage.ClearCtrls();
		vector<tagGoodAttr>::iterator iter;
		for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,nVecIndex++)
		{
			if(iter->strOrigName==strVal)
				break;
		}
		pContainer->m_wndGoodAttrPage.FillCtrls(nVecIndex);		
		pContainer->m_wndAllGoodsPage.ShowWindow(SW_HIDE);
		pContainer->m_wndGoodAttrPage.ShowWindow(SW_NORMAL);	
		pContainer->m_wndGoodAttrPage.SetEditFlag(true);
		pGoodsTree->m_cGoodsTree.Expand(pGoodsTree->m_cGoodsTree.GetRootItem(),TVE_EXPAND);
		HTREEITEM firstItem;
		firstItem=pGoodsTree->m_cGoodsTree.GetNextItem(pGoodsTree->m_cGoodsTree.GetRootItem(),TVGN_CHILD);
		while(firstItem!=NULL)
		{
			if(pGoodsTree->m_cGoodsTree.GetItemText(firstItem).Compare(CString(strVal))==0)
			{
				pGoodsTree->m_cGoodsTree.SelectItem(firstItem);

				break;
			}
			firstItem=pGoodsTree->m_cGoodsTree.GetNextItem(firstItem,TVGN_NEXT);
		}
	}
}

void CAllGoodsPage::OnLvnKeydownAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
   	*pResult = 0;
	POSITION pos=m_cAllGoodsList.GetFirstSelectedItemPosition();
	if(pos)
	{
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			int nRet=MessageBox("是否要删除此项记录?",NULL,MB_OKCANCEL);
			if(nRet==2)
				return;
			int nSelected=m_cAllGoodsList.GetNextSelectedItem(pos);
			char strVal[500];
			m_cAllGoodsList.GetItemText(nSelected,1,strVal,500);
			vector<tagGoodAttr>::iterator iter=vecGoodAttr.begin();
			for(;iter!=vecGoodAttr.end();iter++)
			{
				if(strcmp(iter->strOrigName.c_str(),strVal)==0)
				{
					vecGoodAttr.erase(iter);
					m_cAllGoodsList.DeleteItem(nSelected);
					break;
				}
			}			
			CContainerPage *pContainer=(CContainerPage*)GetParent();
			CSplitterWnd *pSplitter=(CSplitterWnd*)pContainer->GetParent();
			CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)pSplitter->GetPane(0,0);
			
			HTREEITEM firstItem;
			firstItem=pGoodsTree->m_cGoodsTree.GetNextItem(pGoodsTree->m_cGoodsTree.GetRootItem(),TVGN_CHILD);
			while(firstItem!=NULL)
			{
				if(pGoodsTree->m_cGoodsTree.GetItemText(firstItem).Compare(CString(strVal))==0)
				{
					pGoodsTree->m_cGoodsTree.DeleteItem(firstItem);

					break;
				}
				firstItem=pGoodsTree->m_cGoodsTree.GetNextItem(firstItem,TVGN_NEXT);
			}
			GoodID--;
			MaxID=0;
			for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++)
			{
				if(MaxID < (int)(iter->dwIndex))
					MaxID=iter->dwIndex;
			}
		}
	}    	
}

void CAllGoodsPage::OnNMReturnAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CAllGoodsPage::OnSize(UINT nType, int cx, int cy)
{
	CContainerPage *pContainer=(CContainerPage*)GetParent();
	if(m_bInit)
	{
	//	pContainer->MoveWindow(0,0,cx,cy);
		m_cAllGoodsList.MoveWindow(0,0,cx,cy);
	}
	CFormView::OnSize(nType, cx, cy);	
	
}

void CAllGoodsPage::OnLvnColumnclickAllgoodslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	m_nSortedCol=pNMLV->iSubItem;
	m_bSortFlag=!m_bSortFlag;
	m_cAllGoodsList.SortItems(MyCompareProc,(LPARAM)this);
	long max = m_cAllGoodsList.GetItemCount();
	for (int i=0; i<max; i++)
	{
		m_cAllGoodsList.SetItemData(i,i);
	}
}

void CAllGoodsPage::ChangeItemText(DWORD dwIndex,int subitem,char* text)
{
	int nCnt=m_cAllGoodsList.GetItemCount();
	char strVal1[32];
	char strVal2[32];
	sprintf_s(strVal1,"%d",dwIndex);
	for(int i=0;i<nCnt;i++)
	{
		m_cAllGoodsList.GetItemText(i,0,strVal2,30);
		if(strcmp(strVal1,strVal2)==0)
		{
			m_cAllGoodsList.SetItem(i,subitem,LVIF_TEXT,text,NULL,0,0,0);
			break;
		}
	}
}

int CALLBACK CAllGoodsPage::MyCompareProc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	CAllGoodsPage *pGoodsPage=(CAllGoodsPage*)lParamSort;
	int nSortedCol=pGoodsPage->GetSortedCol();
	CString strVal1=pGoodsPage->m_cAllGoodsList.GetItemText((int)lParam1,nSortedCol);
	CString strVal2=pGoodsPage->m_cAllGoodsList.GetItemText((int)lParam2,nSortedCol);
	int nVal1=atoi(strVal1);
	int nVal2=atoi(strVal2);
	switch(nSortedCol) 
	{	
	case 1:
	case 2:
	case 3:
	case 10:
	case 13:
		{
			if(pGoodsPage->GetSortFlag())
				return strcmp(strVal2,strVal1);
			else
				return strcmp(strVal1,strVal2);
		}	
	default:
		{
			if(pGoodsPage->GetSortFlag())
				return nVal2-nVal1;
			else
				return nVal1-nVal2;
		}
		break;
	}
	return 0;
	
}

void CAllGoodsPage::UpDataAllGoodsList()
{
	int i=0;
	vector<tagGoodAttr>::iterator iter;	
	for(iter=vecGoodAttr.begin();iter!=vecGoodAttr.end();iter++,i++)
	{			
		int nCurItem=m_cAllGoodsList.InsertItem(0xffff,"");
		char strID[500],strUIID[500],strGround[500],strCosplay[500],
			strPrice[500],strSilverPrice[500],strSound[500],strSound1[500],strSound2[500];
		sprintf_s(strID,"%d",iter->dwIndex);

		m_cAllGoodsList.SetItem(i,0,LVIF_TEXT,strID,NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,1,LVIF_TEXT,iter->strOrigName.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,2,LVIF_TEXT,iter->strName.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItem(i,3,LVIF_TEXT,iter->bSave?"是":"否",NULL,0,0,0);		

		sprintf_s(strUIID,"%d",iter->dwIconId);
		m_cAllGoodsList.SetItem(i,4,LVIF_TEXT,strUIID,NULL,0,0,0);

		sprintf_s(strGround,"%d",iter->dwGroundId);
		m_cAllGoodsList.SetItem(i,5,LVIF_TEXT,strGround,NULL,0,0,0);

		sprintf_s(strCosplay,"%d",iter->dwEquipID);
		m_cAllGoodsList.SetItem(i,6,LVIF_TEXT,strCosplay,NULL,0,0,0);

		sprintf_s(strPrice,"%d",iter->dwValue);
		m_cAllGoodsList.SetItem(i,7,LVIF_TEXT,strPrice,NULL,0,0,0);

		sprintf_s(strSilverPrice,"%d",iter->dwSilverValue);
		m_cAllGoodsList.SetItem(i,8,LVIF_TEXT,strSilverPrice,NULL,0,0,0);

		m_cAllGoodsList.SetItem(i,9,LVIF_TEXT,strGoodType[iter->dwType],NULL,0,0,0);			

		//拾取声音	
		sprintf_s(strSound,"%d",iter->dwSound);		
		m_cAllGoodsList.SetItem(i,10,LVIF_TEXT,strSound,NULL,0,0,0);

		//描述
		m_cAllGoodsList.SetItem(i,11,LVIF_TEXT,iter->strContent.c_str(),NULL,0,0,0);
		m_cAllGoodsList.SetItemData(nCurItem,i);


		//挥动/弱伤声音
		sprintf_s(strSound1,"%d",iter->dwSoundID1);		
		m_cAllGoodsList.SetItem(i,12,LVIF_TEXT,strSound1,NULL,0,0,0);

		//被击中声音ID
		sprintf_s(strSound2,"%d",iter->dwSoundID2);		
		m_cAllGoodsList.SetItem(i,13,LVIF_TEXT,strSound2,NULL,0,0,0);				

		//攻击时候是否混音
		m_cAllGoodsList.SetItem(i,14,LVIF_TEXT,iter->bSoundSwitch?"是":"否",NULL,0,0,0);

		// 等级限制 默认为0级
		sprintf_s(strSound1,"%d",0);		
		m_cAllGoodsList.SetItem(i,15,LVIF_TEXT,strSound1,NULL,0,0,0);
		vector<tagAddAttr>& vecAddAttr = iter->vecAddAttr;
		vector<tagAddAttr>::iterator itAddAttr = vecAddAttr.begin();
		for (; itAddAttr!=vecAddAttr.end(); ++itAddAttr)
		{
			if (itAddAttr->wType == 7)
			{
				sprintf_s(strSound1,"%d",itAddAttr->lValue1);		
				m_cAllGoodsList.SetItem(i,15,LVIF_TEXT,strSound1,NULL,0,0,0);
				break;
			}
		}
	}
}