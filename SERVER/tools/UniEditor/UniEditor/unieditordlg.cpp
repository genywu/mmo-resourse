// UniEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditor.h"
#include "UniEditorDlg.h"
#include <atlimage.h>
#include "Info.h"
#include "Monster.h"
//#include "NPC.h"
#include "vld.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



const int	cMaxTime = 4;
BOOL bJpgShow=TRUE;
BOOL bMove=FALSE;
CPoint g_point;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int vcurrpos;//当前纵向滚动条滑块位置
int hcurrpos;//当前横向滚动条滑块位置
struct tagRegion
{
	DWORD dwID;
};
vector<tagRegion>vecRegion;
BOOL bFlag;
BOOL IsLoad(DWORD dwcurrID)
{
	vector<tagRegion>::iterator iter=vecRegion.begin();
	for(;iter!=vecRegion.end();iter++)
	{
		if(iter->dwID==dwcurrID)
			return TRUE;
	}
	return FALSE;
}
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDelItem();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_SCROLLBAR1, m_VScroll);
	//DDX_Control(pDX, IDC_SCROLLBAR3, m_HScroll);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(ID__DEL, &CAboutDlg::OnDelItem)
END_MESSAGE_MAP()

BOOL CUniEditorDlg::m_bUpdateMonster = FALSE;			// 编辑更新标记
BOOL CUniEditorDlg::m_bUpdateMonInfo = FALSE;			// 编辑更新标记
//BOOL CUniEditorDlg::m_bUpdateNPC = FALSE;				// 编辑更新标记
BOOL CUniEditorDlg::m_bUpdateCollection=FALSE;	
BOOL CUniEditorDlg::m_bUpdateRoads=FALSE;
BOOL CUniEditorDlg::m_bUpdateCollInfo=FALSE;
BOOL CUniEditorDlg::m_bUpdateSignPos=FALSE;
BOOL CUniEditorDlg::m_bSelChange = TRUE;				// 选择结点变化
bool CUniEditorDlg::m_bCtrl=false;
CTextLog *CUniEditorDlg::m_pTextLog = NULL;				// 操作日志
CString	CUniEditorDlg::m_szModify = _T("");				// 修改内容


// CUniEditorDlg 对话框
CUniEditorDlg::CUniEditorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUniEditorDlg::IDD, pParent)
, m_bCreateNew(false)
, m_bJpg(true)
, m_bLButton(false)
, m_bRButton(false)
, m_bSpace(false)
, m_bSignPos(false)
, m_bMonster(false)
, m_bCollection(false)
, m_bRegion(false)
//, m_rcMoveRect(rcMap)
, m_bRoads(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ptreeRegion = new CRegionTree;
	m_uRegionCurr = 0;
	m_rcMonsterDensity.SetRectEmpty();

	m_pCell = NULL;
	m_lRegionHeight = 0;
	m_lRegionWidth = 0;

	m_uLBStyle = LBS_NOTDOWN;
	m_bRBDown = FALSE;
	m_bMouseDown = FALSE;

	m_bModify = false;
	//m_pvecMonster	= new VECMONSTER;		// MONSTER向量指针
	//m_pvecMoninfo	= new VECMONINFO;		// MONSTER具体信息向量指针
	//m_pvecNPC		= new VECNPC;			// NPC向量指针
}

CUniEditorDlg::~CUniEditorDlg()
{
	//if(m_pvecNPC != NULL)
	//{
	//	delete m_pvecNPC;
	//}

	//if(m_pvecMoninfo != NULL)
	//{
	//	delete m_pvecMoninfo;
	//}

	//if(m_pvecMonster != NULL)
	//{
	//	delete m_pvecMonster;
	//}

	//if(m_pCell !=NULL)
	//{
	//	delete = NULL;
	//}

	if(m_ptreeRegion != NULL)
	{
		delete m_ptreeRegion;
	}

	if(m_pTextLog != NULL)
	{
		delete m_pTextLog;
	}
	m_dcMemory.DeleteDC();
	m_Bitmap.DeleteObject();
	m_BitJpg.DeleteObject();

}

void CUniEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
	DDX_Control(pDX, IDC_NUMLIST, m_listNum);
	DDX_Control(pDX, IDC_LISTCOLL, m_listColl);
	DDX_Control(pDX, IDC_LIST1, m_ScriptList);
}

BEGIN_MESSAGE_MAP(CUniEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID__ADD, &CUniEditorDlg::OnAddItem)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CUniEditorDlg::OnBnClickedBtnSave)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VKADD, &CUniEditorDlg::OnVkadd)
	ON_COMMAND(ID_ZOOMOUT, &CUniEditorDlg::OnZoomout)
	ON_COMMAND(ID_ZOOMIN, &CUniEditorDlg::OnZoomin)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_REFRESH, &CUniEditorDlg::OnBnClickedRefresh)
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CUniEditorDlg::OnHdnItemclickNumlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_NUMLIST, &CUniEditorDlg::OnLvnColumnclickNumlist)
	ON_NOTIFY(NM_CLICK, IDC_NUMLIST, &CUniEditorDlg::OnNMClickNumlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTCOLL, &CUniEditorDlg::OnLvnColumnclickListcoll)
	ON_NOTIFY(NM_CLICK, IDC_LISTCOLL, &CUniEditorDlg::OnNMClickListcoll)
END_MESSAGE_MAP()
bool CUniEditorDlg::ShowJpgGif(CDC* pDC,CString strPath, int x, int y)
 {   
	
	 return true;
 }
// 绘制新场景
BOOL CUniEditorDlg::DrawRegion(void)
{
	m_nTimes=1;
	m_lRegionWidth = m_mapTree[m_uRegionCurr]->Width();
	m_lRegionHeight = m_mapTree[m_uRegionCurr]->Height();
	m_rcMap.SetRect(0, 0, m_lRegionWidth, m_lRegionHeight);
	CRect rect;
	GetClientRect(&rect);
	m_uRegionPrev = m_uRegionCurr;
	m_dcMemory.SelectStockObject(BLACK_BRUSH);
	m_dcMemory.SelectStockObject(BLACK_PEN);
	m_dcMemory.Rectangle(&rcMap);
	m_pCell = m_mapTree[m_uRegionCurr]->GetCell();

	
	
	for(long v=0, c=0; v<m_lRegionHeight; ++v)
	{
		for(long h=0; h<m_lRegionWidth; ++h, ++c)
		{
			// 出入口
			if(m_pCell[c].lSwitch > 0)
			{
				m_dcMemory.SetPixel(h, v, RGB(255,0,0));
			}

			else 
			{
				long lLine(h);
				long i(0);
				while(h<(m_lRegionWidth-1) && (m_pCell[c+1].bBlock == m_pCell[c].bBlock))
				{
					i = m_pCell[c].bBlock;
					++c;
					++h;
				}
				if(lLine != h)
				{
					CPen pen(PS_SOLID, 1, m_clDraw[i]);
					m_dcMemory.SelectObject(&pen);
					m_dcMemory.MoveTo(lLine, v);
					m_dcMemory.LineTo(h, v);
				}
				/*if(0==m_pCell[c].bBlock)
					m_dcMemory.SetPixel(h, v,RGB(255,255,255));
				else
					m_dcMemory.SetPixel(h, v, RGB(0,0,0));*/
				
			}
		}
	}

	return TRUE;
}
BOOL CUniEditorDlg::DrawRegion(float fx)
{
	m_lRegionWidth = m_mapTree[m_uRegionCurr]->Width();
	m_lRegionHeight = m_mapTree[m_uRegionCurr]->Height();
	m_rcMap.SetRect(0, 0, m_lRegionWidth, m_lRegionHeight);
	CRect rect;
	GetClientRect(&rect);
	m_uRegionPrev = m_uRegionCurr;
	m_dcMemory.SelectStockObject(BLACK_BRUSH);
	m_dcMemory.SelectStockObject(BLACK_PEN);

	RECT rcRealMap = rcMap;
	rcRealMap.right *= fx;
	rcRealMap.bottom *= fx;
	m_dcMemory.Rectangle(0,0,rcMap.right*cMaxTime,rcMap.bottom*cMaxTime);
	m_dcMemory.Rectangle(&rcRealMap);
	m_pCell = m_mapTree[m_uRegionCurr]->GetCell();

	for(long v=0, c=0; v<m_lRegionHeight; ++v)
	{
		for(long h=0; h<m_lRegionWidth; ++h, ++c)
		{
			// 出入口
			if(m_pCell[c].lSwitch > 0)
				{
					m_dcMemory.SetPixel(fx*h, fx*v, RGB(255,0,0));
					m_dcMemory.SetPixel(fx*h+1, fx*v+1, RGB(255,0,0));
				}
				
				long lLine=h;
				long i=0;
				while(h<(m_lRegionWidth-1) && (m_pCell[c+1].bBlock == m_pCell[c].bBlock))
				{
					
					++c;
					++h;
				}
				i = m_pCell[c].bBlock;
			if(lLine != h)
			{
				CPen pen(PS_SOLID, fx*1+1, m_clDraw[i]);
				m_dcMemory.SelectObject(&pen);
				
				for(int ftimes=fx;ftimes<=fx;ftimes++)
				{
					m_dcMemory.MoveTo(lLine*ftimes, v*ftimes);
					m_dcMemory.LineTo(fx*h, v*ftimes);
				}
				
			}
		}
	}

	return TRUE;
}
// 绘制地图区域
BOOL CUniEditorDlg::DrawMap(void)
{
	// 场景变换

	GetDlgItem(IDC_REFRESH)->EnableWindow(TRUE);
	if(m_uRegionCurr > 1)
	{
		CClientDC dc(this);
		if(m_uRegionCurr != m_uRegionPrev)
		{
			bFlag=TRUE;
			DrawRegion(m_nTimes);
		}


		
		m_bf.SourceConstantAlpha = 128;


		CDC dcMemory;
		dcMemory.CreateCompatibleDC(NULL);
		CBitmap bmp;
		
		if(!bmp.CreateCompatibleBitmap(&dc, rcMap.right, rcMap.bottom))
		{
			PostMessage(0);
		}

		dcMemory.SelectObject(bmp);
		if(bFlag&&!IsLoad(m_uRegionCurr))
		{
			DrawJpg(m_uRegionCurr);
			bFlag=FALSE;
		}
		
	
		 dcMemory.BitBlt(rcMap.left, rcMap.top,
			rcMap.right, rcMap.bottom,
			&m_dcMemory, hcurrpos, vcurrpos, SRCCOPY);
		//当没选中路标结点时画绘制怪物和采集物区域
		if(!m_bRegion)
		{
			if(m_bMonster)
			{
				DrawMonster(dcMemory);	
			}
			if(m_bCollection)
			DrawCollection(dcMemory);
			if(m_bSignPos)
			DrawSignPos(dcMemory);
			if (m_bRoads)
			DrawRoads(dcMemory);
			
		}
		else
		{
			DrawMonster(dcMemory);	
			DrawCollection(dcMemory);
			DrawSignPos(dcMemory);
			DrawRoads(dcMemory);
			
		}
		DrawMonsterVector(dcMemory);
		DrawCollectionVector(dcMemory);
		switch(m_uLayer)
		{
		case ITEM_LAYER_REGION:
		case ITEM_LAYER_TYPE:
			switch(m_uOffset)
			{
			case ITEM_OFFSET_MONSTER:
				break;

			case ITEM_OFFSET_NPC:
				break;
			case ITEM_OFFSET_SIGNPOS:
				break;
			case ITEM_OFFSET_ROADS:
				break;
			}
			if(!m_rcMonsterDensity.IsRectEmpty()&&!m_bRButton)
			{
				CPen penMonsterDensity(PS_DOT, 1, RGB(0,255,0));
				dcMemory.SelectObject(&penMonsterDensity);
				CRect rect(m_rcMonsterDensity);
			//	rect.left -= 1;
			//	rect.top -= 1;
				rect.left=(rect.left)*m_nTimes-hcurrpos;
				rect.top=(rect.top)*m_nTimes-vcurrpos;
				rect.right=(rect.right)*m_nTimes-hcurrpos;
				rect.bottom=(rect.bottom)*m_nTimes-vcurrpos;
				if(!m_bSpace&&m_bShift)
				dcMemory.Rectangle(rect);
				
				// Add drawn rect of a new batch monster/NPC
				if(m_bCreateNew)	
				{
					m_bCreateNew = false;
					m_bShift=false;
					m_ptreeRegion->DlgADD(&m_rcMonsterDensity);
					HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
					
					hItem=m_ptreeRegion->GetParentItem(hItem);
					
					m_ptreeRegion->SelectItem(hItem);
				}
			}
			break;

		case ITEM_LAYER_EDIT:
			{
				
				switch(m_uParentOffset)
				{
				case ITEM_OFFSET_MONSTER:
					if(m_bShift)
						{
							HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
							hItem=m_ptreeRegion->GetParentItem(hItem);
							m_ptreeRegion->SelectItem(hItem);
						}
					DrawMonster(dcMemory, m_uOffset);
				
					break;

				case ITEM_OFFSET_NPC:
					if(m_bShift)
					{
						HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
						hItem=m_ptreeRegion->GetParentItem(hItem);
						m_ptreeRegion->SelectItem(hItem);
					}
					DrawCollection(dcMemory, m_uOffset);
					break;
				case ITEM_OFFSET_SIGNPOS:
					if(m_bShift)
					{
						HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
						hItem=m_ptreeRegion->GetParentItem(hItem);
						m_ptreeRegion->SelectItem(hItem);
					}
					DrawSignPos(dcMemory,m_uOffset);
				case ITEM_OFFSET_ROADS:
					if(m_bShift)
					{
						HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
						//hItem=m_ptreeRegion->GetParentItem(hItem);
						m_ptreeRegion->SelectItem(hItem);
						if (m_bLButton)
						{
							m_bCreateNew = false;
							m_bShift=false;
							CRect rect(m_ptFrom.x,m_ptFrom.y,m_ptFrom.x+3,m_ptFrom.y+3);
							m_ptreeRegion->DlgADD(&rect);
							m_bLButton=false;
							
						}
					}
					/*if (m_ptreeRegion->m_ncurrRoads>0)
					{
						DrawRoads(dcMemory,m_ptreeRegion->m_ncurrRoads,m_ptreeRegion->m_ncurrRoads,-1);
					}*/
					
					
				}
			}
			break;

		case ITEM_LAYER_MONSTER:
			{
				HTREEITEM hItem,hItemroad;
				CString str,str1,str2;
				if(m_bShift)
				{
					hItem=m_ptreeRegion->GetSelectedItem();
					hItem=m_ptreeRegion->GetParentItem(hItem);
					m_ptreeRegion->SelectItem(hItem);
					str=m_ptreeRegion->GetItemText(hItem);
				}
				//最小结点
				hItemroad=m_ptreeRegion->GetSelectedItem();
				str2=m_ptreeRegion->GetItemText(hItemroad);
				//父结点
				hItemroad=m_ptreeRegion->GetParentItem(hItemroad);
				str1=m_ptreeRegion->GetItemText(hItemroad);
				hItemroad=m_ptreeRegion->GetParentItem(hItemroad);
				str=m_ptreeRegion->GetItemText(hItemroad);
				
		
				if("路径"==str)
				{
					
					DrawRoads(dcMemory,-1,atoi(str1),m_uOffset);

				}
				
			}
			break;

		default:
			break;
		}

		// 绘制地图场景
		dc.BitBlt(rcMap.left+ptMap.cx, rcMap.top+ptMap.cy, 
			rcMap.right+ptMap.cx, rcMap.bottom+ptMap.cy,
			&dcMemory, 0, 0, MERGECOPY);
		DeleteDC(dc);
		DeleteDC(dcMemory);
		return TRUE;
	}
	else
	{
		CRect rect(rcMap);
		InvalidateRect(rect+ptMap);
		return FALSE;
	}
	
}
void CUniEditorDlg::DrawJpg(unsigned int unNo)
{
	CImage image;
	char strVal[100];
	sprintf(strVal, _T("mapJPEG\\%u.jpg"), unNo);
	string str(strVal);
	fstream file(str.c_str());
	if(!file.is_open())
	{
		char strVal[59];
		sprintf_s(strVal,"不能打开编号为:%d的JPEG文件",unNo);
		MessageBox(strVal);
		return;	
	}
	file.close();
	
	image.Load(str.c_str());
	SetStretchBltMode(m_dcMemory,COLORONCOLOR);
//	image.AlphaBlend(m_dcMemory,0,0,180);
	image.StretchBlt(m_dcMemory,0,0,m_lRegionWidth*m_nTimes,m_lRegionHeight*m_nTimes);
	
}

// 绘制Monster信息
BOOL CUniEditorDlg::DrawMonster(CDC &dc, unsigned uNo)
{
	// 读取Monster信息
	
	CRect rect;
	int nBlock;
	CPen penActive,penInactive,penActive1,penInactive1;
	penActive.CreatePen(PS_SOLID,3,m_clDraw[CL_ACTIVE_MONSTER]);
	penInactive.CreatePen(PS_SOLID,3,m_clDraw[CL_INACTIVE_MONSTER]);
	penActive1.CreatePen(PS_SOLID,3,RGB(0,0,0));
	penInactive1.CreatePen(PS_SOLID,3,m_clDraw[REGION_BLOCK]);
	unsigned unNo;
	char strVal[10];
	for(unsigned u=0; u<m_vecMonster.size(); ++u)
	{
		rect = m_vecMonster[u]->GetCoordinate();
		
		unNo=m_vecMonster[u]->GetNo();
		nBlock=GetBlockCellNumber(m_vecMonster[u]->GetCoordinate());
		if(0==nBlock)
		{
			//penActive.CreatePen(PS_SOLID,3,m_clDraw[CL_ACTIVE_MONSTER]);
			//penInactive.CreatePen(PS_SOLID,3,m_clDraw[CL_ACTIVE_MONSTER]);
			//CPen penActive(PS_SOLID, 3, m_clDraw[CL_ACTIVE_MONSTER]);
			//CPen penInactive(PS_SOLID, 3, m_clDraw[CL_INACTIVE_MONSTER]);
			dc.SelectObject(&penInactive);
			dc.SelectStockObject(NULL_BRUSH);
		}
		else
		{
			
			//penActive.CreatePen(PS_SOLID,3,RGB(255,0,0));
			//penInactive.CreatePen(PS_SOLID,3,RGB(255,0,0));
			
			dc.SelectObject(&penInactive1);
			dc.SelectStockObject(NULL_BRUSH);
		}
		sprintf_s(strVal,"%d",unNo);
		string str(strVal);
		int n=str.size();
		// 怪物产生区域不是一个点
		if(rect.left != rect.right)
		{
			rect.left = rect.left *m_nTimes-hcurrpos;
			rect.top =rect.top * m_nTimes-vcurrpos;
			rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
			rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
			dc.Rectangle(rect);
			dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
		}

		// 怪物产生区域是一个点
		else
		{
			rect += CRect(3, 3, 2, 2);
			rect.left = rect.left *m_nTimes-hcurrpos;
			rect.top =rect.top * m_nTimes-vcurrpos;
			rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
			rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
			dc.Rectangle(rect);
			dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
		}
	}

	if(uNo != -1)
	{
		dc.SelectObject(&penActive);
		if (uNo < m_vecMonster.size())
		{
			rect = m_vecMonster[uNo]->GetCoordinate();
			
			// 怪物产生区域不是一个点
			if(rect.left != rect.right)
			{
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
				dc.Rectangle(rect);
				/*string str(strVal);
				dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/
			
			}
			
			// 怪物产生区域是一个点
			else
			{
				rect += CRect(3, 3, 2, 2);
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
				
				dc.Rectangle(rect);
				/*string str(strVal);
				dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/
			}
		}
		
	}
	
	return TRUE;
}
void CUniEditorDlg::DrawMonsterFirst(vector<long>vecNo)
{
	vector<long>::iterator it = m_vecMonsterNo.begin();
	if (m_vecMonsterNo.size()>0)
	{
		it=m_vecMonsterNo.begin();
	}
	else
	{	
		return;
	}
	CPen penActive;
	penActive.CreatePen(PS_SOLID,3,RGB(0,0,0));
	CClientDC dc(this);
	dc.SelectObject(&penActive);

	CRect rect;
	for (;it!=m_vecMonsterNo.end();it++)
	{

		dc.SelectObject(&penActive);

		VECMONSTERITR itm=m_vecMonster.begin();
		for (;itm!=m_vecMonster.end();itm++)
		{
			if ( (*itm)->GetNo() == (*it) )
			{
				rect = (*itm)->GetCoordinate();

				// 怪物产生区域不是一个点

				if(rect.left != rect.right)
				{
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					CPoint pt(rect.left,rect.top);
					CPoint pt1(rect.right,rect.bottom);
					CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*char strVal[10]="";
					itoa((*it),strVal,10);
					string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/

				}

				// 怪物产生区域是一个点
				else
				{
					rect += CRect(3, 3, 2, 2);
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					CPoint pt(rect.left,rect.top);
					CPoint pt1(rect.right,rect.bottom);
					CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
				}	
				break;
			}
		}//for	
	}//for
}
void CUniEditorDlg::DrawMonsterVector(CDC &dc)
{
	vector<long>::iterator it = m_vecMonsterNo.begin();
	if (m_vecMonsterNo.size()>0)
	{
		it=m_vecMonsterNo.begin();
	}
	else
	{	
		return;
	}
	CPen penActive;
	penActive.CreatePen(PS_SOLID,3,RGB(0,0,0));
	dc.SelectStockObject(NULL_BRUSH);
	//CClientDC dc(this);
	dc.SelectObject(&penActive);
	
	CRect rect;
	for (;it!=m_vecMonsterNo.end();it++)
	{

		dc.SelectObject(&penActive);
		
		VECMONSTERITR itm=m_vecMonster.begin();
		for (;itm!=m_vecMonster.end();itm++)
		{
			if ( (*itm)->GetNo() == (*it) )
			{
				rect = (*itm)->GetCoordinate();
				
				// 怪物产生区域不是一个点
				
				if(rect.left != rect.right)
				{
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					//CPoint pt(rect.left,rect.top);
					//CPoint pt1(rect.right,rect.bottom);
					//CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					//if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*char strVal[10]="";
					itoa((*it),strVal,10);
					string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/

				}

				// 怪物产生区域是一个点
				else
				{
					rect += CRect(3, 3, 2, 2);
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					/*rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;*/
					//CPoint pt(rect.left,rect.top);
					//CPoint pt1(rect.right,rect.bottom);
					//CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					//if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
				}
				break;
			}
		}	
	}
}

void CUniEditorDlg::DrawCollectionFirst(vector<long>vecNo)
{
	vector<long>::iterator it = m_vecCollectionNo.begin();
	if (m_vecCollectionNo.size()>0)
	{
		it=m_vecCollectionNo.begin();
	}
	else
		return;
	CPen penActive;
	penActive.CreatePen(PS_SOLID,3,RGB(0,0,0));
	CClientDC dc(this);
	dc.SelectObject(&penActive);

	CRect rect;
	for (;it!=m_vecCollectionNo.end();it++)
	{

		dc.SelectObject(&penActive);

		VECCOLLECTIONITR itm=m_vecCollection.begin();
		for (;itm!=m_vecCollection.end();itm++)
		{
			if ( (*itm)->GetNo() == (*it) )
			{
				rect = (*itm)->GetCoordinate();
				if(rect.left != rect.right)
				{
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					CPoint pt(rect.left,rect.top);
					CPoint pt1(rect.right,rect.bottom);
					CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/

				}

				// 怪物产生区域是一个点
				else
				{
					rect += CRect(3, 3, 2, 2);
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					CPoint pt(rect.left,rect.top);
					CPoint pt1(rect.right,rect.bottom);
					CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/
				}
				break;
			}
		}	
	}
}

void CUniEditorDlg::DrawCollectionVector(CDC& dc)
{
	vector<long>::iterator it = m_vecCollectionNo.begin();
	if (m_vecCollectionNo.size()>0)
	{
		it=m_vecCollectionNo.begin();
	}
	else
		return;
	CPen penActive;
	penActive.CreatePen(PS_SOLID,3,RGB(0,0,0));
	dc.SelectStockObject(NULL_BRUSH);
	//CClientDC dc(this);
	dc.SelectObject(&penActive);

	CRect rect;
	for (;it!=m_vecCollectionNo.end();it++)
	{

		dc.SelectObject(&penActive);

		VECCOLLECTIONITR itm=m_vecCollection.begin();
		for (;itm!=m_vecCollection.end();itm++)
		{
			if ( (*itm)->GetNo() == (*it) )
			{
				rect = (*itm)->GetCoordinate();
				if(rect.left != rect.right)
				{
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					//CPoint pt(rect.left,rect.top);
					//CPoint pt1(rect.right,rect.bottom);
					//CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					//if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/

				}

				// 怪物产生区域是一个点
				else
				{
					rect += CRect(3, 3, 2, 2);
					rect.left = rect.left *m_nTimes-hcurrpos;
					rect.top =rect.top * m_nTimes-vcurrpos;
					rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
					rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
					rect.left +=ptMap.cx;
					rect.right +=ptMap.cx;
					rect.top +=ptMap.cy;
					rect.bottom +=ptMap.cy;
					//CPoint pt(rect.left,rect.top);
					//CPoint pt1(rect.right,rect.bottom);
					////CRect rect1(ptMap.cx,ptMap.cy,ptMap.cx+rcMap.bottom,ptMap.cy+rcMap.bottom);
					//if (rect1.PtInRect(pt1) && rect1.PtInRect(pt))
					{
						dc.Rectangle(rect);
					}
					/*string str(strVal);
					dc.DrawText(str.c_str(),3,&rect,DT_BOTTOM);*/
				}
			}
			break;
		}	
	}
}
// 绘制采集物信息
BOOL CUniEditorDlg::DrawCollection(CDC &dc, unsigned uNo)
{
	// 读取采集物信息
	CRect rect;
	int nBlock;
	CPen penActive(PS_SOLID, 3, m_clDraw[CL_ACTIVE_NPC]);
	CPen penInactive(PS_SOLID, 3, m_clDraw[CL_INACTIVE_NPC]);
	CPen penActive1(PS_SOLID, 3, RGB(0,0,0));
	CPen penInactive1(PS_SOLID, 3, m_clDraw[REGION_BLOCK]);
	dc.SelectObject(&penInactive);
	dc.SelectStockObject(NULL_BRUSH);

	unsigned unNo;
	for(unsigned u=0; u<m_vecCollection.size(); ++u)
	{
		rect = m_vecCollection[u]->GetCoordinate();
		unNo=m_vecCollection[u]->GetNo();
		nBlock=GetBlockCellNumber(m_vecCollection[u]->GetCoordinate());
		if(0==nBlock)
		{
			//penActive.CreatePen(PS_SOLID,3,m_clDraw[CL_ACTIVE_MONSTER]);
			//penInactive.CreatePen(PS_SOLID,3,m_clDraw[CL_ACTIVE_MONSTER]);
			//CPen penActive(PS_SOLID, 3, m_clDraw[CL_ACTIVE_MONSTER]);
			//CPen penInactive(PS_SOLID, 3, m_clDraw[CL_INACTIVE_MONSTER]);
			dc.SelectObject(&penInactive);
			dc.SelectStockObject(NULL_BRUSH);
		}
		else
		{
			
			//penActive.CreatePen(PS_SOLID,3,RGB(255,0,0));
			//penInactive.CreatePen(PS_SOLID,3,RGB(255,0,0));
			
			dc.SelectObject(&penInactive1);
			dc.SelectStockObject(NULL_BRUSH);
		}
		char strVal[10];
		sprintf_s(strVal,"%d",unNo);
		
		string str(strVal);
		int n=str.size();
		// 采集物产生区域不是一个点
		if(rect.left != rect.right)
		{
			rect.left = rect.left *m_nTimes-hcurrpos;
			rect.top =rect.top * m_nTimes-vcurrpos;
			rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
			rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
			dc.Rectangle(rect);
			dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
		}

		// 采集物产生区域是一个点
		else
		{
			rect += CRect(3, 3, 2, 2);
			rect.left = rect.left *m_nTimes-hcurrpos;
			rect.top =rect.top * m_nTimes-vcurrpos;
			rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
			rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
			dc.Rectangle(rect);
			dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
		}
	}

	if(uNo != -1)
	{
		dc.SelectObject(&penActive);
		if (uNo < m_vecCollection.size())
		{
			rect = m_vecCollection[uNo]->GetCoordinate();

			//采集物产生区域不是一个点
			if(rect.left != rect.right)
			{
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
				dc.Rectangle(rect);
			}

			// 采集物产生区域是一个点
			else
			{
				rect += CRect(3, 3, 2, 2);
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
				dc.Rectangle(rect);
			}
		}
		
	}
	return TRUE;
}
//绘制路标点信息
BOOL CUniEditorDlg::DrawSignPos(CDC &dc,unsigned uNo)
{
	CRect rect;
	CPen penActive(PS_SOLID, 4, m_clDraw[CL_MONSTER_DENSITY]);
	CPen penInactive(PS_SOLID, 4, m_clDraw[CL_MONSTER_DENSITY]);

	CBrush brush(m_clDraw[CL_MONSTER_DENSITY]);
	CPen penActive1(PS_SOLID, 4, RGB(0,0,0));
	CPen penInactive1(PS_SOLID, 4,  m_clDraw[REGION_BLOCK]);

	
	int nBlock;
	unsigned unNo;

	int xpos,ypos,n;
	for(unsigned u=0; u<m_vecSignPos.size(); ++u)
	{
		char strVal[10];
		unNo=m_vecSignPos[u]->GetNo();
		
		sprintf_s(strVal,"%d",unNo);
		string str(strVal);
		
		n=str.size();
		unNo=m_vecSignPos[u]->GetNo();
		xpos=m_vecSignPos[u]->GetxPos();
		ypos=m_vecSignPos[u]->GetyPos();
		rect.left=xpos;
		rect.top=ypos;
		rect.right=xpos+4;
		rect.bottom=ypos+4;
		nBlock=GetBlockCellNumber(rect);
		if(0==nBlock)
		{
			dc.SelectObject(&penInactive);
			dc.SelectStockObject(NULL_BRUSH);
		}
		else
		{
			dc.SelectObject(&penInactive1);
			dc.SelectStockObject(NULL_BRUSH);
		}
		rect.left = rect.left *m_nTimes-hcurrpos;
		rect.top =rect.top * m_nTimes-vcurrpos;
		rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
		rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
		
		dc.Rectangle(rect);
		dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
	}

	if(uNo != -1)
	{
		CPen pen(PS_SOLID, 4, m_clDraw[CL_ACTIVE_MONSTER]);
		dc.SelectObject(&pen);
		//dc.SelectStockObject(NULL_PEN);
		CBrush brush(m_clDraw[CL_ACTIVE_MONSTER]);
		dc.SelectObject(&brush);
		if (uNo < m_vecSignPos.size())
		{
			unNo=m_vecSignPos[uNo]->GetNo();
			xpos=m_vecSignPos[uNo]->GetxPos();
			ypos=m_vecSignPos[uNo]->GetyPos();
			rect.left=xpos;
			rect.top=ypos;
			rect.right=xpos+4;
			rect.bottom=ypos+4;
			rect.left = rect.left *m_nTimes-hcurrpos;
			rect.top =rect.top * m_nTimes-vcurrpos;
			rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
			rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
			dc.Rectangle(rect);
		}
		
	}
	return TRUE;
}
BOOL CUniEditorDlg::DrawRoads(CDC &dc,unsigned uNo2,unsigned uNo1,unsigned uNo)
{
	CRect rect;
	CPen penActive(PS_SOLID, 4, m_clDraw[CL_MONSTER_DENSITY]);
	CPen penInactive(PS_SOLID, 4, m_clDraw[CL_MONSTER_DENSITY]);

	CBrush brush(m_clDraw[CL_MONSTER_DENSITY]);
	CPen penActive1(PS_SOLID, 4, RGB(0,0,0));
	CPen penInactive1(PS_SOLID, 4,  m_clDraw[REGION_BLOCK]);
	unsigned unNo;
	int nBlock;
	int xpos,ypos,n;
	if (-1==uNo1)
	{
		MAPROADSITR itroads=m_mapRoads.begin();
		for(;itroads!=m_mapRoads.end();itroads++)
		{
			
			VECROADPOINT vecroads=(*itroads->second).GetRoadsPoints();
			VECROADPOINTITR it=vecroads.begin();
			for (;it!=vecroads.end();it++)
			{
				char strVal[10];
				unNo=it->npointNo;
				sprintf_s(strVal,"%d",unNo);
				string str(strVal);
				n=str.size();
				xpos=it->nx;
				ypos=it->ny;
				rect.left=xpos;
				rect.top=ypos;
				rect.right=xpos+4;
				rect.bottom=ypos+4;
				nBlock=GetBlockCellNumber(rect);
				if(0==nBlock)
				{
					dc.SelectObject(&penInactive);
					dc.SelectStockObject(NULL_BRUSH);
				}
				else
				{
					dc.SelectObject(&penInactive1);
					dc.SelectStockObject(NULL_BRUSH);
				}
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;

				dc.Rectangle(rect);
				dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
			}
			
		}
	}
	else
	{
		
		MAPROADSITR itr;
		itr=m_mapRoads.find(uNo1);
		if (itr != m_mapRoads.end())
		{
			VECROADPOINT vecroads=(*itr->second).GetRoadsPoints();
			VECROADPOINTITR it=vecroads.begin();
			for (;it!=vecroads.end();it++)
			{
				char strVal[10];
				unNo=it->npointNo;
				sprintf_s(strVal,"%d",unNo);
				string str(strVal);
				n=str.size();
				xpos=it->nx;
				ypos=it->ny;
				rect.left=xpos;
				rect.top=ypos;
				rect.right=xpos+4;
				rect.bottom=ypos+4;
				nBlock=GetBlockCellNumber(rect);
				if(0==nBlock)
				{
					dc.SelectObject(&penInactive);
					dc.SelectStockObject(NULL_BRUSH);
				}
				else
				{
					dc.SelectObject(&penInactive1);
					dc.SelectStockObject(NULL_BRUSH);
				}
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;

				dc.Rectangle(rect);
				dc.DrawText(str.c_str(),n,&rect,DT_BOTTOM);
			}
		}	
	}
	if(uNo != -1 )
	{
		CPen pen(PS_SOLID, 4, m_clDraw[CL_ACTIVE_MONSTER]);
		dc.SelectObject(&pen);
		//dc.SelectStockObject(NULL_PEN);
		CBrush brush(m_clDraw[CL_ACTIVE_MONSTER]);
		dc.SelectObject(&brush);
		MAPROADSITR it;
		it=m_mapRoads.find(uNo1);
		if (it != m_mapRoads.end())
		{
			VECROADPOINT vecpoint=(*it->second).GetRoadsPoints();
			if (uNo < vecpoint.size())
			{
				CRoads::tagRoadsPoint roadspoint=vecpoint[uNo];
				rect.left = roadspoint.nx;
				rect.top =roadspoint.ny;
				rect.right=rect.left+4;
				rect.bottom=rect.top+4;
				rect.left = rect.left *m_nTimes-hcurrpos;
				rect.top =rect.top * m_nTimes-vcurrpos;
				rect.right=rect.left*m_nTimes+m_nTimes*(rect.right-rect.left)-hcurrpos;
				rect.bottom=rect.top*m_nTimes+m_nTimes*(rect.bottom-rect.top)-vcurrpos;
				dc.Rectangle(rect);
			}
		}
		
	}

	return TRUE;
}
/*
// 获取point处所有Monster编号信息
unsigned *CUniEditorDlg::GetMonster(const CPoint point)
{
	unsigned count(0);
	unsigned *puArrayNo(NULL);
	if(!m_vecMonster.empty())
	{
		CRect rect;
		unsigned size = m_vecMonster.size();
		puArrayNo = new unsigned[size+1];
		unsigned i(0);

		for(; i<m_vecMonster.size(); i++)
		{
			rect = m_vecMonster[i]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				puArrayNo[count++] = i;
			}
		}

		puArrayNo[count] = MAX_NUM;
	}

	if (0 == count)
	{
		return NULL;
	} 
	else
	{
		return puArrayNo;
	}
}

// 获取point处所有NPC编号信息
unsigned *CUniEditorDlg::GetNPC(const CPoint point)
{
	unsigned count(0);
	unsigned *puArrayNo(NULL);
	if(!m_vecNPC.empty())
	{
		CRect rect;
		unsigned size = m_vecNPC.size();
		puArrayNo = new unsigned[size+1];
		unsigned i(0);

		for(; i<m_vecNPC.size(); i++)
		{
			rect = m_vecNPC[i]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				puArrayNo[count++] = i;
			}
		}

		puArrayNo[count] = MAX_NUM;
	}

	if (0 == count)
	{
		return NULL;
	} 
	else
	{
		return puArrayNo;
	}
}
*/

// 获取point处最上层Monster编号信息
unsigned CUniEditorDlg::GetMonster(const CPoint point)
{
	CRect rect;

	if(!m_vecMonster.empty())
	{

		// 检测当前结点矩形
		if (ITEM_LAYER_EDIT == m_uLayer && ITEM_OFFSET_MONSTER == m_uParentOffset)
		{
			rect = m_vecMonster[m_uOffset]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				return m_uOffset;
			}
		}

		// 检测所有矩形
		unsigned size = m_vecMonster.size();
		unsigned i;
		for(i=0; i<m_vecMonster.size(); i++)
		{
			rect = m_vecMonster[i]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				break;
			}
		}

		if(i < size)
		{
			return i;
		}
	}

	return MAX_NUM;
}

// 获取point处最上层collection编号信息
unsigned CUniEditorDlg::GetCollection(const CPoint point)
{
	CRect rect;

	if(!m_vecCollection.empty())
	{
		// 检测当前结点矩形
		if (ITEM_LAYER_EDIT == m_uLayer && ITEM_OFFSET_NPC == m_uParentOffset)
		{
			rect = m_vecCollection[m_uOffset]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				return m_uOffset;
			}
		}

		// 否则检测所有矩形
		unsigned size = m_vecCollection.size();
		unsigned i;
		for(i=0; i<m_vecCollection.size(); i++)
		{
			rect = m_vecCollection[i]->GetCoordinate();
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				break;
			}
		}

		if(i < size)
		{
			return i;
		}
	}

	return MAX_NUM;
}
//获取最上层路标点信息
unsigned CUniEditorDlg::GetSignPos(const CPoint point)
{
	CRect rect;

	if(!m_vecSignPos.empty())
	{
		// 检测当前结点矩形
		if (ITEM_LAYER_EDIT == m_uLayer && ITEM_OFFSET_SIGNPOS == m_uParentOffset)
		{
			rect.left = m_vecSignPos[m_uOffset]->GetxPos();
			rect.top =m_vecSignPos[m_uOffset]->GetyPos();
			rect.right=rect.left+3;
			rect.bottom=rect.top+3;
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				return m_uOffset;
			}
		}

		// 否则检测所有矩形
		unsigned size = m_vecSignPos.size();
		unsigned i;
		for(i=0; i<m_vecSignPos.size(); i++)
		{
			rect.left = m_vecSignPos[i]->GetxPos();
			rect.top =m_vecSignPos[i]->GetyPos();
			rect.right=rect.left+3;
			rect.bottom=rect.top+3;
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				break;
			}
		}

		if(i < size)
		{
			return i;
		}
	}

	return MAX_NUM;
}
unsigned CUniEditorDlg::GetRoadsPoint(const CPoint point)
{
	CRect rect;

	if(!m_vecRoadsPoint.empty())
	{
		// 检测当前结点矩形
	/*	if (ITEM_LAYER_EDIT == m_uLayer && ITEM_OFFSET_ROADS == m_uParentOffset)
		{
			rect.left = m_vecRoadsPoint[m_uOffset].nx;
			rect.top =m_vecRoadsPoint[m_uOffset].ny;
			rect.right=rect.left+3;
			rect.bottom=rect.top+3;
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				return m_uOffset;
			}
		}*/

		// 否则检测所有矩形
		unsigned size = m_vecRoadsPoint.size();
		unsigned i;
		for(i=0; i<m_vecRoadsPoint.size(); i++)
		{
			rect.left = m_vecRoadsPoint[i].nx;
			rect.top =m_vecRoadsPoint[i].ny;
			rect.right=rect.left+3;
			rect.bottom=rect.top+3;
			if(rect.left == rect.right)
			{
				rect += CRect(3, 3, 2, 2);
			}

			else
			{
				rect.right += 1;
				rect.bottom += 1;
			}

			if(rect.PtInRect(point))
			{
				break;
			}
		}

		if(i < size)
		{
			return i;
		}
	}

	return MAX_NUM;
}
// 计算区域内阻挡格子数
long CUniEditorDlg::GetBlockCellNumber(const CRect &rect)
{
	long lBlockedCellNum(0);
	unsigned uRight = (rect.right < m_lRegionWidth) ? rect.right : m_lRegionWidth;
	unsigned uBottom = (rect.bottom < m_lRegionHeight) ? rect.bottom : m_lRegionHeight;
	
	for(long v=rect.top-1, c=v*m_lRegionWidth; v<uBottom; ++v, c+=m_lRegionWidth)
	{
		
		for(long h=rect.left-1; h<uRight; ++h)
		{
			if(m_pCell[c+h].bBlock!=0)
			{
				++lBlockedCellNum;
			}
		}
	}

	return lBlockedCellNum;
}
// 计算区域内无阻挡格子数
long CUniEditorDlg::GetUnBlockCellNumber(const CRect &rect)
{
	long lBlockedCellNum(0);
	unsigned uRight = (rect.right < m_lRegionWidth) ? rect.right : m_lRegionWidth;
	unsigned uBottom = (rect.bottom < m_lRegionHeight) ? rect.bottom : m_lRegionHeight;
	
	for(long v=rect.top-1, c=v*m_lRegionWidth; v<uBottom; ++v, c+=m_lRegionWidth)
	{
		
		for(long h=rect.left-1; h<uRight; ++h)
		{
			if(0==m_pCell[c+h].bBlock)
			{
				++lBlockedCellNum;
			}
		}
	}

	return lBlockedCellNum;
}

// 计算框选区域内怪物密度
CString CUniEditorDlg::MonsterDensity()
{
	m_rcMonsterDensity.NormalizeRect();
	long lUnblockedCellNum(GetUnBlockCellNumber(m_rcMonsterDensity));
	UINT nMonsterQuantity(0);
	unsigned long nUnblockCell(GetUnBlockCellNumber(m_rcMonsterDensity)), nUnblockCellI(0);

	for(VECMONSTERITR itr=m_vecMonster.begin(); itr!=m_vecMonster.end(); ++itr)
	{
		CRect rcMonster((*itr)->GetCoordinate());
		CRect rcInterSect;
		rcMonster.NormalizeRect();
		rcInterSect.IntersectRect(m_rcMonsterDensity, rcMonster);
		if (m_rcMonsterDensity.PtInRect(rcMonster.TopLeft()))
		{
			rcInterSect = rcMonster;
		}

		//if (0 == lUnblockedCellNum)
		//{
		//	MessageBox(_T("\r\n区域内无阻挡格子数目为 0！"));
		//}

		if (rcInterSect.right != 0)
		{
			nUnblockCellI = GetBlockCellNumber(rcInterSect);
			unsigned long nUnblockCellM = GetBlockCellNumber(rcMonster);
			float rate = (0 == nUnblockCellM) ? 1 : (nUnblockCellI * 1.0 / nUnblockCellM);
			nMonsterQuantity += (*itr)->GetQuantity() * rate;
		}
	}

	if (0 == nMonsterQuantity)
	{
		return NULL;
	}

	CString str;
	str.Format(_T("当前选中区域(%3u, %3u; %3u, %3u).\r\n 宽度: %3d, 高度: %3d, 非阻挡数: %lu.\r\n 怪物总数目: %u; 怪物总密度: %.1f.\r\n"),
		m_rcMonsterDensity.left, m_rcMonsterDensity.top, m_rcMonsterDensity.right, m_rcMonsterDensity.bottom,
		m_rcMonsterDensity.Width()+1, m_rcMonsterDensity.Height()+1,
		nUnblockCell, nMonsterQuantity, nUnblockCell*1.0 / nMonsterQuantity
		);
	return str;
}


CString CUniEditorDlg::GetEditInfo(const unsigned uNo, const CPoint point, const unsigned uType)
{

	// 计算非阻挡格子数
	CRect rect;
	CString strEditInfo(_T(""));

	// Collection
	if(ITEM_OFFSET_NPC == uType)
	{
		VECCOLLECTIONINFO &vecCollInfo = m_vecCollection[uNo]->GetCollectionInfo();
		int i = 0;
		CString szMonNo;
		szMonNo.Format(_T(" 采集物:%u , "), uNo);
		strEditInfo += szMonNo;
		for(VECCOLLINFOITR itr=vecCollInfo.begin(); (itr!=vecCollInfo.end()) && i<3; ++i, ++itr)
		{
			if(i > 0)
			{
				strEditInfo += _T("          ");
			}

			strEditInfo.Append((*itr)->GetName().c_str());
			strEditInfo += _T("\r\n");
		}

		if(i>=3)
		{
			strEditInfo += _T("          ......\r\n");
		}

		rect = m_vecCollection[uNo]->GetCoordinate();
	}

	// Monster
	else if(ITEM_OFFSET_MONSTER == uType)
	{
		VECMONINFO &vecMoninfo = m_vecMonster[uNo]->GetMonsterInfo();
		int i = 0;
		CString szMonNo;
		szMonNo.Format(_T(" 怪物%03u: "), uNo);
		strEditInfo += szMonNo;
		for(VECMONINFOITR itr=vecMoninfo.begin(); (itr!=vecMoninfo.end()) && i<3; ++i, ++itr)
		{
			if(i > 0)
			{
				strEditInfo += _T("          ");
			}

			strEditInfo.Append((*itr)->GetName().c_str());
			strEditInfo += _T("\r\n");
		}

		if(i>=3)
		{
			strEditInfo += _T("          ......\r\n");
		}

		rect = m_vecMonster[uNo]->GetCoordinate();
	}
	else if(ITEM_OFFSET_SIGNPOS == uType)
	{
		
		strEditInfo +=_T(" 路标点");
		
		rect.left = m_vecSignPos[uNo]->GetxPos();
		rect.right=rect.left+4;
		rect.top=m_vecSignPos[uNo]->GetyPos();
		rect.bottom=rect.top+4;
		char strVal[20];
		sprintf(strVal,"%d:,x:%d,y:%d",m_vecSignPos[uNo]->GetNo(),rect.left,rect.top);
		string str(strVal);
		strEditInfo+=str.c_str();
		
		strEditInfo += _T("\r\n");
	}
	else if(ITEM_OFFSET_ROADS == uType)
	{

		strEditInfo +=_T(" 路径点");

		rect.left = m_vecRoadsPoint[uNo].nx;
		rect.right=rect.left+4;
		rect.top=m_vecRoadsPoint[uNo].ny;
		rect.bottom=rect.top+4;
		char strVal[20];
		sprintf(strVal,"%d:,x:%d,y:%d",m_vecRoadsPoint[uNo].npointNo,rect.left,rect.top);
		string str(strVal);
		strEditInfo+=str.c_str();

		strEditInfo += _T("\r\n");
	}
	// 计算区域内非阻挡格子数
	rect.NormalizeRect();
	long lblockedCellNum(GetBlockCellNumber(rect));
	
	CString szUnblockedCellNum(_T(""));

	// 计算MONSTR密度
	szUnblockedCellNum.Format(_T(" 宽度: %d, 高度: %d. 阻挡数: %lu.\r\n"), rect.Width()+1, rect.Height()+1, lblockedCellNum);
	long lUnblockedCellNum(GetUnBlockCellNumber(rect));
	// 怪物数量不能为0
	if(ITEM_OFFSET_MONSTER == uType)
	{
		unsigned uNum = m_vecMonster[uNo]->GetQuantity();
		float rate = (float)lUnblockedCellNum/uNum;
		CString szDensity(_T(""));
		szDensity.Format(_T(" 怪物数目: %u; 怪物密度: %.1f.\r\n"), uNum, rate);
		szUnblockedCellNum += szDensity;
	}
	
	return strEditInfo + szUnblockedCellNum;
}
void CUniEditorDlg::HitCurrItem(int n,string str1,string str2,string str3)
{
	if( 1==n )
		m_bRegion=true;
	else
		m_bRegion=false;
	if("怪物"==str1||"怪物"==str2||"怪物"==str3)
	{
		//在list中显示数目
		m_listNum.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_MONSTER);
		m_listColl.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_NPC);
		m_bMonster=true;
	}
	else
		m_bMonster=false;
	if("采集物"==str1||"采集物"==str2||"采集物"==str3)
	{
		//在list中显示数目
		m_listNum.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_MONSTER);
		m_listColl.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_NPC);
		if("采集物"==str1)
			m_uOffset-=1;
		m_bCollection=true;
	}
	else
		m_bCollection=false;
	if("路标点"==str1||"路标点"==str2)
	{
		//在list中显示数目
		m_listNum.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_SIGNPOS);
		if("路标点"==str1)
			m_uOffset-=2;
		m_bSignPos=true;
	}
	else
		m_bSignPos=false;
	if("路径"==str1||"路径"==str2)
	{
		//在list中显示数目
		m_listNum.SetData(m_mapTree[m_uRegionCurr],ITEM_OFFSET_ROADS);
		if("路径"==str1)
			m_uOffset-=3;
		m_bRoads=true;
	}
	else
		m_bRoads=false;
}	

// 选择节点变化
void CUniEditorDlg::ItemChanged(DWORD_PTR uNo)
{
	// TODO: 在此添加控件通知处理程序代码
	m_vecCollectionNo.clear();
	m_vecMonsterNo.clear();
	HTREEITEM hItem = m_ptreeRegion->GetItem();
	HTREEITEM hParentItem = m_ptreeRegion->GetParentItem(hItem);
	
	// 更新编辑列表
	m_pTextLog->AddLog(UpdateLog());

	m_uLayer = m_ptreeRegion->GetLayer(hItem);
	m_uOffset = m_ptreeRegion->GetOffset(hItem);
	CString str1=m_ptreeRegion->GetItemText(hItem);
	HTREEITEM hItempos=m_ptreeRegion->GetParentItem(hItem);
	CString strpos=m_ptreeRegion->GetItemText(hItempos);
	HTREEITEM hItemEdit=m_ptreeRegion->GetParentItem(hItempos);
	CString strEdit=m_ptreeRegion->GetItemText(hItemEdit);
	//判断显示哪种区域
	if (IsDlgButtonChecked(IDC_RADIOSORT))
	{
		HitCurrItem(m_uLayer,str1.GetString(),strpos.GetString(),strEdit.GetString());
	}
	else
	{
		m_bRegion=true;
		m_bMonster = true;
		m_bCollection = true;
		m_bRoads = true;
		m_bSignPos = true;
	}
	m_uRegionPrev = m_uRegionCurr;
	m_uRegionCurr = uNo;
	
	m_uParentOffset = m_ptreeRegion->GetOffset(hParentItem);
	CString str4=m_ptreeRegion->GetItemText(hParentItem);
	HTREEITEM hCollItem=m_ptreeRegion->GetParentItem(hItem);
	hCollItem=m_ptreeRegion->GetParentItem(hCollItem);
	CString str=m_ptreeRegion->GetItemText(hCollItem);
	m_bSelChange = TRUE;
	

	if(uNo > ITEM_REGION_UNEXISTED)
	{
		m_mapTree = m_ptreeRegion->GetMapTree();
		m_vecMonster = m_mapTree[m_uRegionCurr]->GetMonster();
		m_vecCollection=m_mapTree[m_uRegionCurr]->GetCollection();
		m_vecSignPos=m_mapTree[m_uRegionCurr]->GetSignPos();
		m_mapRoads=m_mapTree[m_uRegionCurr]->GetRoads();
	//	m_vecCollInfo=m_mapTree[m_uRegionCurr]->GetCollection();
		CString strUpdateFile(_T(""));

		switch(m_uLayer)
		{
		case ITEM_LAYER_ROOT:
		case ITEM_LAYER_REGION:
		case ITEM_LAYER_TYPE:
			m_dlgMonster.ShowWindow(SW_HIDE);
			m_dlgMoninfo.ShowWindow(SW_HIDE);
			m_dlgCollection.ShowWindow(SW_HIDE);
			m_dlgSignPos.ShowWindow(SW_HIDE);
			m_dlgRoadsPoint.ShowWindow(SW_HIDE);
			break;

		case ITEM_LAYER_EDIT:
			{
				switch(m_uParentOffset)
				{
				case ITEM_OFFSET_MONSTER:
					m_dlgMonster.ShowWindow(SW_SHOW);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					m_dlgCollInfo.ShowWindow(SW_HIDE);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					m_dlgSignPos.ShowWindow(SW_HIDE);
					m_dlgRoadsPoint.ShowWindow(SW_HIDE);
					if (m_uOffset<m_vecMonster.size())
					{
						m_pInfo = m_vecMonster[m_uOffset];
						m_dlgMonster.SetDlg(m_mapTree[m_uRegionCurr], (CMonster *)m_pInfo, TRUE);
					}
				
					break;

				case ITEM_OFFSET_NPC:
					m_dlgCollection.ShowWindow(SW_SHOW);
					m_dlgMonster.ShowWindow(SW_HIDE);
					m_dlgCollInfo.ShowWindow(SW_HIDE);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					m_dlgSignPos.ShowWindow(SW_HIDE);
					m_dlgRoadsPoint.ShowWindow(SW_HIDE);
					if (m_uOffset<m_vecCollection.size())
					{
						m_pInfo = m_vecCollection[m_uOffset];
						m_dlgCollection.SetDlg(m_mapTree[m_uRegionCurr], (CCollection*)m_pInfo,TRUE);
					}
					break;
				case ITEM_OFFSET_SIGNPOS:
					m_dlgSignPos.ShowWindow(SW_SHOW);
					m_dlgCollection.ShowWindow(SW_HIDE);
					m_dlgMonster.ShowWindow(SW_HIDE);
					m_dlgCollInfo.ShowWindow(SW_HIDE);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					m_dlgRoadsPoint.ShowWindow(SW_HIDE);
					m_pInfo = m_vecSignPos[m_uOffset];
					if (m_uOffset<m_vecSignPos.size())
					{
						m_dlgSignPos.SetDlg(m_mapTree[m_uRegionCurr], (CSignPos *)m_pInfo, TRUE);
					}
					
					break;
				case ITEM_OFFSET_ROADS:
					{
						
						m_dlgRoadsPoint.ShowWindow(SW_SHOW);
						m_dlgCollection.ShowWindow(SW_HIDE);
						m_dlgMonster.ShowWindow(SW_HIDE);
						m_dlgCollInfo.ShowWindow(SW_HIDE);
						m_dlgMoninfo.ShowWindow(SW_HIDE);
						m_dlgSignPos.ShowWindow(SW_HIDE);
						HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
						CString str1=m_ptreeRegion->GetItemText(hItem);
						MAPROADSITR it=m_mapRoads.find(atoi(str1));
						if (it!=m_mapRoads.end())
						{
							m_dlgRoadsPoint.SetDlgNo(m_mapRoads,it->first);
						}
						
					}
					break;
				default:
					break;
				}
			}
			break;

		case ITEM_LAYER_MONSTER:
			
				if("采集物"==str)
				{
					m_dlgCollInfo.ShowWindow(SW_SHOW);
					m_dlgMonster.ShowWindow(SW_HIDE);
					m_dlgCollection.ShowWindow(SW_HIDE);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					m_dlgSignPos.ShowWindow(SW_HIDE);
					m_dlgRoadsPoint.ShowWindow(SW_HIDE);
					if (m_uParentOffset<=m_vecCollection.size() &&
						m_uOffset<=m_vecCollInfo.size())
					{
						m_vecCollInfo=m_vecCollection[m_uParentOffset]->GetCollectionInfo();
						m_dlgCollInfo.SetDlg(m_mapTree[m_uRegionCurr], m_vecCollInfo[m_uOffset]);
					}
				}
				else if("怪物"==str)
				{
					m_dlgMoninfo.ShowWindow(SW_SHOW);
					m_dlgMonster.ShowWindow(SW_HIDE);
					m_dlgCollection.ShowWindow(SW_HIDE);
					m_dlgCollInfo.ShowWindow(SW_HIDE);
					m_dlgSignPos.ShowWindow(SW_HIDE);
					m_dlgRoadsPoint.ShowWindow(SW_HIDE);
					if (m_uParentOffset<=m_vecMonster.size() &&
						m_uOffset<=m_vecMoninfo.size())
					{
						m_vecMoninfo = m_vecMonster[m_uParentOffset]->GetMonsterInfo();
						m_dlgMoninfo.SetDlg(m_mapTree[m_uRegionCurr], m_vecMoninfo[m_uOffset]);
					}
				}
				else if("路径"==str)
				{
					m_dlgRoadsPoint.ShowWindow(SW_SHOW);
					m_dlgMonster.ShowWindow(SW_HIDE);
					m_dlgCollection.ShowWindow(SW_HIDE);
					m_dlgCollInfo.ShowWindow(SW_HIDE);
					m_dlgSignPos.ShowWindow(SW_HIDE);
					m_dlgMoninfo.ShowWindow(SW_HIDE);
					MAPROADSITR it;
					HTREEITEM hItem = m_ptreeRegion->GetSelectedItem();
					HTREEITEM hItemp = m_ptreeRegion->GetParentItem(hItem);
					CString strpt = m_ptreeRegion->GetItemText(hItemp);
					it=m_mapRoads.find(atoi(strpt));
					if (it!=m_mapRoads.end())
					{
						m_ptreeRegion->m_ncurrRoads=m_uParentOffset+1;
						VECROADPOINT vecpoint=(*it->second).GetRoadsPoints();
						m_vecRoadsPoint=vecpoint;
						m_dlgRoadsPoint.SetDlg(m_mapTree[m_uRegionCurr],it->first, &(*it->second).GetRoadsPoints()[m_uOffset],TRUE);
					}	
				}
				break;
			default:
				break;

		}
	}

	else
	{
		m_dlgMonster.ShowWindow(SW_HIDE);
		m_dlgMoninfo.ShowWindow(SW_HIDE);
		m_dlgCollection.ShowWindow(SW_HIDE);
		m_dlgCollInfo.ShowWindow(SW_HIDE);
		m_dlgSignPos.ShowWindow(SW_HIDE);
		m_dlgRoadsPoint.ShowWindow(SW_HIDE);
	}

	m_bSelChange = FALSE;
}

// 备份文件
bool CUniEditorDlg::Backup(const CString &szFileName)
{
	SYSTEMTIME sysTime(GetLog()->GetStartTime());
	CString strFolder, strFileBak, strFileOri;
	CreateDirectory(_T("Backup"), NULL);
	strFolder.Format(_T("Backup\\%4u_%02u_%02u@%02u.%02u.%02u"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	CreateDirectory(strFolder, NULL);

	// 备份文件
	strFileOri.Format(_T("regions\\%s"),szFileName);
	strFileBak.Format(_T("%s\\%s"), strFolder, szFileName);
	CopyFile(strFileOri, strFileBak, TRUE);
	return true;
}
//去掉名字,保留原始名
string GetNameString1(string str)
{
	char strVal[100];
	int i=0,j=0;
	int nstrSize=str.size();
	for(;60!=str[i]&&i<nstrSize;i++)
	{	
	}
	if(i!=str.size())
	{
		
		i+=1;
		for(;62!=str[i];i++,j++)	
		{
			strVal[j]=str[i];
			if(62==str[i+1])
			{
				strVal[j+1]='\0';
				break;
			}
		}
		return strVal;
	}
	
	else
		return str;
}
// 保存文件
bool CUniEditorDlg::SaveFile(const CString &szFileName)
{
	Backup(szFileName);
	string str,strTemp;
	int i = szFileName.Find(_T('.'));
	unsigned uRegionNo;
	sscanf(szFileName.Left(i), _T("%u"), &uRegionNo);

	CString strFileSave;
	strFileSave.Format(_T("regions\\%s"), szFileName);
	ofstream ofs;
	ofs.open(strFileSave.GetString());
	if(ofs.is_open())
	{
		// 保存MONSTER文件
		if(-1 != szFileName.Find(_T("monster")))
		{
			VECMONSTERITR itr;
			ofs << strMonsterHead << strLine;
			for(itr = m_mapTree[uRegionNo]->GetMonster().begin(); itr != m_mapTree[uRegionNo]->GetMonster().end(); ++itr)
			{
				ofs << **itr;
			}
			ofs << strEnd << endl << endl;

			char strID[16];
			for(itr = m_mapTree[uRegionNo]->GetMonster().begin(); itr != m_mapTree[uRegionNo]->GetMonster().end(); ++itr)
			{
				sprintf(strID, _T("id\t%u\n"), (*itr)->GetNo());
				ofs << strID << strMonsterInfoHead << strLine;
				VECMONINFOITR iteMonsterInfo;
				for(iteMonsterInfo = (*itr)->GetMonsterInfo().begin(); iteMonsterInfo != (*itr)->GetMonsterInfo().end(); ++iteMonsterInfo)
				{
					strTemp=(*iteMonsterInfo)->GetName();
					str=GetNameString1(strTemp);
					(*iteMonsterInfo)->SetName(str);
					ofs << strSharp;
					ofs << **iteMonsterInfo;
					(*iteMonsterInfo)->SetName(strTemp);
				}
				ofs << strEnd << endl << endl;
			}
		}

		// 保存Collection文件
		else if(-1 != szFileName.Find(_T("collection")))
		{
			VECCOLLECTIONITR itr;
			ofs << strMonsterHead << strLine;
			for(itr = m_mapTree[uRegionNo]->GetCollection().begin(); itr != m_mapTree[uRegionNo]->GetCollection().end(); ++itr)
			{
				ofs << **itr;
			}
			ofs << strEnd << endl << endl;

			char strID[16];
			for(itr = m_mapTree[uRegionNo]->GetCollection().begin(); itr != m_mapTree[uRegionNo]->GetCollection().end(); ++itr)
			{
				sprintf(strID, _T("id\t%u\n"), (*itr)->GetNo());
				ofs << strID << strCollInfoHead << strLine;
				VECCOLLINFOITR iteCollInfo;
				for(iteCollInfo = (*itr)->GetCollectionInfo().begin(); iteCollInfo != (*itr)->GetCollectionInfo().end(); ++iteCollInfo)
				{
					strTemp=(*iteCollInfo)->GetName();
					str=GetNameString1(strTemp);
					(*iteCollInfo)->SetName(str);
					ofs << strSharp;
					ofs << **iteCollInfo;
					(*iteCollInfo)->SetName(strTemp);
				}
				ofs << strEnd << endl << endl;
			}
		}
		else if(-1 != szFileName.Find(_T("xml")))
		{
			//保存路标点文件
			VECSIGNPOSITR itr;
			ofs<<strXMLHead<<"\t";
			char strVal[100];
			sprintf(strVal,_T(" <Region id=\"%d\">"),uRegionNo);
			ofs<<strVal;
			for(itr=m_mapTree[uRegionNo]->GetSignPos().begin();itr!=m_mapTree[uRegionNo]->GetSignPos().end();++itr)
			{
				memset(strVal,0,100);
				sprintf(strVal,_T("<SignPos x=\"%d\" y=\"%d\" /> "),(*itr)->GetxPos(),(*itr)->GetyPos());
				ofs<<"\t";
				ofs<<strVal;
				ofs<<"\n";
			}
			ofs<<strXMLTail;
		}
		else if (-1 != szFileName.Find(_T("roads")))
		{
			MAPROADSITR itroads=m_mapTree[uRegionNo]->GetRoads().begin();
			
			for (;itroads!=m_mapTree[uRegionNo]->GetRoads().end();itroads++)
			{
				CRoads *pRoads=(*itroads).second;
				VECROADPOINT vecroad=pRoads->GetRoadsPoints();
				VECROADPOINTITR itr=vecroad.begin();
				ofs<<"*"<<"\t";
				ofs<<(*itroads).first<<"\t"<<"\n";
				for(;itr!=vecroad.end();itr++)
				{
					ofs<<"<"
						<<"\t"
						<<itr->nx<<"\t"
						<<itr->ny
						<<"\n";
					
				}
				ofs<<"<end>"<<"\n";
			}
			
			
		}
		ofs.close();

		m_listLog.AddLog(LOG_SAVE, szFileName);
		return true;
	}

	// 无法打开
	else
	{
		ofs.close();
		m_listLog.AddLog(LOG_ERROR, szFileName + _T("为只读，请去除只读属性后再次保存！"));

		MessageBox(szFileName + _T("为只读，请去除只读属性后再次保存！"));
		return false;
	}
}

// 更新文件列表
bool CUniEditorDlg::UpdateFileList(int nflag)
{
	char str[32];
	static bool sbShowBtn = false;

	if(0==nflag)
	{
		sprintf(str, _T("%u.monster"), m_uRegionPrev);
	}

	else if(1==nflag)
	{
		sprintf(str, _T("%u.collection"), m_uRegionPrev);
	}
	else if(2==nflag)
	{
		sprintf(str, _T("%u.xml"), m_uRegionPrev);
	}
	else if(3==nflag)
	{
		sprintf(str, _T("%u.roads"), m_uRegionPrev);
	}
	if (!m_bMouseDown)
	{
		m_listLog.AddLog(LOG_MOD, UpdateLog());
	}

	m_setUpdate.insert(str);
	m_bModify = true;
	if (!sbShowBtn)
	{
		GetDlgItem(IDC_BTN_SAVE)->ShowWindow(SW_SHOW);
	}

	return true;
}

// 更新日志
CString CUniEditorDlg::UpdateLog()
{
	CString szModifyNode;
	CString szOriNode, szCurNode;
	CRegion *pRegion = m_mapTree[m_uRegionCurr];

	if(m_bUpdateMonster || m_bUpdateMonInfo || m_bUpdateCollection || m_bUpdateCollInfo||m_bUpdateSignPos||m_bUpdateRoads)
	{
		if(m_bUpdateMonster&&!m_bCtrl)
		{
			m_dlgMonster.GetModifyNode(szOriNode);
			m_vecMonster[m_uOffset]->Monster2String(szCurNode);
			szModifyNode.Format(_T("修改结点 <Monster>\t\t在场景 %s\n\t%-u %s\t%s"), 
				pRegion->GetRegionName(), m_uOffset, szOriNode, szCurNode);
			m_bUpdateMonster = FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_MONSTER);

			CTextLog::WriteLog(szModifyNode);

		}

		else if(m_bUpdateCollection&&!m_bCtrl)
		{
			m_dlgCollection.GetModifyNode(szOriNode);
			m_vecCollection[m_uOffset]->Collection2String(szCurNode);
			szModifyNode.Format(_T("修改结点 <采集物>\t\t\t在场景 %s\n\t%s\t\t%s"), 
				pRegion->GetRegionName(), szOriNode, szCurNode);
			m_bUpdateCollection = FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_NPC);
			CTextLog::WriteLog(szModifyNode);
		}
		//改变了采集物详细信息
		else if(m_bUpdateCollInfo&&!m_bCtrl)
		{
			HTREEITEM hItem = m_ptreeRegion->GetParentItem(m_ptreeRegion->GetItem());
			unsigned	uNoModifyColl(m_ptreeRegion->GetOffset(hItem));
			CCollection	*pCollection = m_vecCollection[uNoModifyColl];
			CString		szNodeCollInfo, szCollInfo;
			pCollection->Collection2String(szCollInfo);
			m_dlgCollInfo.GetModifyNode(szOriNode);
//			m_vecCollInfo[m_uOffset]->CollInfo2String(szCurNode);
			szModifyNode.Format(_T("修改结点 <CollInfo>\t在场景 %s\n\t%-u%s\t%-u%s\t%s"), 
				pRegion->GetRegionName(), uNoModifyColl, szCollInfo, m_uOffset, szOriNode, szCurNode);
			m_bUpdateCollInfo = FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_NPC);
			CTextLog::WriteLog(szModifyNode);
		}
		else if(m_bUpdateMonInfo&&!m_bCtrl)
		{
			HTREEITEM hItem = m_ptreeRegion->GetParentItem(m_ptreeRegion->GetItem());
			
			unsigned	uNoModifyMonster(m_ptreeRegion->GetOffset(hItem));
			CMonster	*pMonster = m_vecMonster[uNoModifyMonster];
			CString		szNodeMonster, szMonster;
			pMonster->Monster2String(szMonster);
			m_dlgMoninfo.GetModifyNode(szOriNode);
			m_vecMoninfo[m_uOffset]->MonsterInfo2String(szCurNode);
			szModifyNode.Format(_T("修改结点 <MonsterInfo>\t在场景 %s\n\t%-u%s\t%-u%s\t%s"), 
				pRegion->GetRegionName(), uNoModifyMonster, szMonster, m_uOffset, szOriNode, szCurNode);
			m_bUpdateMonInfo = FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_MONSTER);
			CTextLog::WriteLog(szModifyNode);
		}
		else if(m_bUpdateSignPos&&!m_bCtrl)
		{
			szModifyNode.Format(_T("修改结点 <SignPos>\t在场景 %s,路标点%d,x:%d,y:%d"), 
				pRegion->GetRegionName(), m_vecSignPos[m_uOffset]->GetNo(),
				m_vecSignPos[m_uOffset]->GetxPos(),
				m_vecSignPos[m_uOffset]->GetyPos());
			m_bUpdateSignPos=FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_SIGNPOS);
			CTextLog::WriteLog(szModifyNode);
		}
		else if (m_bUpdateRoads&&!m_bCtrl)
		{
			szModifyNode.Format(_T("修改结点 <RoadsPoint>\t在场景 %s"), 
				pRegion->GetRegionName());
			m_bUpdateRoads=FALSE;
			m_listNum.SetData(pRegion,ITEM_OFFSET_ROADS);
			CTextLog::WriteLog(szModifyNode);
		}
	}

	return szModifyNode;
}

// CUniEditorDlg 消息处理程序

BOOL CUniEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu(_T(""));
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时,框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rectscroll(215,710,930,900);
	m_HScroll.Create(SBS_HORZ | SBS_TOPALIGN | WS_CHILD, rectscroll, this, 100);
	m_VScroll.Create(SBS_VERT  | SBS_TOPALIGN | WS_CHILD, CRect(915,10,900,710), this, 100);

	m_hAccelTable=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU2));
	m_pTextLog = new CTextLog;
	SetWindowPos(&CWnd::wndTop, rcMainDlg.left, rcMainDlg.top, rcMainDlg.right, rcMainDlg.bottom, SWP_NOMOVE);
	m_nTimes=1;
	vcurrpos=0;
	hcurrpos=0;
	m_ptFrom.x=0;
	m_ptFrom.y=0;
	m_pt.x=0;
	m_pt.y=0;

	GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
	m_bf.AlphaFormat = 0;
	m_bShift=false;
	// 生成树
	CRect rect(rcTreeRegion);

	m_ptreeRegion->Create(WS_BORDER | WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS, 
		rect+ptTreeRegion, this, IDT_REGION);
	
	//CRect rectLog(rcLog);
	//rectLog.OffsetRect(ptLog);
	//m_listLog.SetWindowPos(&CWnd::wndTop, rectLog.left, rectLog.top, rectLog.right, rectLog.bottom, SWP_SHOWWINDOW);
	m_listLog.InitialList();
	m_listNum.InitialList();
	m_listColl.InitialList();
	// 初始化场景列表失败
	if(!m_ptreeRegion->InitialRegionTree())
	{
		PostQuitMessage(0);
		return FALSE;
	}
	m_dlgMonster.Create(IDD_MONSTER, this);
	m_dlgMoninfo.Create(IDD_MONSTERINFO, this);
	m_dlgCollection.Create(IDD_COLLECTION,this);
	m_dlgCollInfo.Create(IDD_COLLECTIONINFO,this);
	m_dlgSignPos.Create(IDD_SIGNPOS,this);
	m_dlgRoadsPoint.Create(IDD_ROADPOINT,this);
	rect = rcDlg;
	rect.OffsetRect(ptDlg);
	m_dlgMonster.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgMonster.ShowWindow(SW_HIDE);

	m_dlgMoninfo.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgMoninfo.ShowWindow(SW_HIDE);

	m_dlgCollection.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgCollection.ShowWindow(SW_HIDE);

	m_dlgCollInfo.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgCollInfo.ShowWindow(SW_HIDE);
	
	m_dlgSignPos.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgSignPos.ShowWindow(SW_HIDE);

	m_dlgRoadsPoint.SetWindowPos(&CWnd::wndTop, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	m_dlgRoadsPoint.ShowWindow(SW_HIDE);

	CheckDlgButton(IDC_RADIOSORT,true);
	// create tooltip
	m_toolTip.Create(this);
	m_toolTip.Activate(FALSE);
	m_toolTip.AddTool(this);
	m_toolTip.SetMaxTipWidth(300);

	//rect = rcMap;
	//rect.OffsetRect(ptMap);
	//m_toolTip.SetMargin(rect);

	// 创建兼容DC
	if(!m_dcMemory.CreateCompatibleDC(NULL)&&!m_dcJpg.CreateCompatibleDC(NULL)&&
		!m_dcContent.CreateCompatibleDC(NULL))
	{
		PostQuitMessage(0);
		return FALSE;
	}

	// 创建兼容Bitmap
	if(!m_Bitmap.CreateCompatibleBitmap(GetDC(), rcMap.right * cMaxTime, rcMap.bottom * cMaxTime))
	{
		PostQuitMessage(0);
		return FALSE;
	}
	if(!m_BitJpg.CreateCompatibleBitmap(GetDC(), rcMap.right * cMaxTime, rcMap.bottom * cMaxTime))
	{
		PostQuitMessage(0);
		return FALSE;
	}
	if (!m_dcMemory.SelectObject(m_Bitmap)&&!m_dcJpg.SelectObject(m_BitJpg))
	{
		PostQuitMessage(0);
		return FALSE;
	}


	// 	enum { CL_NOBLOCK, CL_FLY, CL_NOFLY, CL_SWITCH, CL_ACTIVE_MONSTER, CL_INACTIVE_MONSTER, CL_ACTIVE_NPC, CL_INACTIVE_NPC };
	ifstream ifs;
	ifs.open(_T("Setup\\ColorSet.ini"), ios_base::in);
	
	// 从文件读取颜色配置
	int i = 0;
	int nrgb = 0;
	if(ifs.is_open())
	{
		string str;
		int r, g, b;
		CUniEditorApp::ReadTo(_T("NUM"), ifs);
		
		ifs>>nrgb;
		while(TRUE == CUniEditorApp::ReadTo(_T("("), ifs))
		{
			ifs >> r >> g >> b;
			m_clDraw[i] = RGB(r, g, b);
			++i;
		}
	}
	
	// 采用默认颜色配置
	if(i != nrgb)
	{
		m_clDraw[CL_NOBLOCK]			= RGB(250, 250, 200);	// 0：可行走
		m_clDraw[CL_FLY]				= RGB(30, 30, 10);		// 1：可飞行
		m_clDraw[CL_NOFLY]				= RGB(20, 20, 5);		// 2：不可飞行
		m_clDraw[CL_SWITCH]				= RGB(255, 255, 255);	// 3：出入口
		m_clDraw[CL_ACTIVE_MONSTER]		= RGB(255, 0, 255);		// 可编辑MONSTER
		m_clDraw[CL_INACTIVE_MONSTER]	= RGB(150, 0, 0);		// 不可编辑MONSTER
		m_clDraw[CL_ACTIVE_NPC]			= RGB(255, 0, 255);		// 可编辑NPC
		m_clDraw[CL_INACTIVE_NPC]		= RGB(0, 150, 0);		// 不可编辑NPC
		m_clDraw[CL_MONSTER_DENSITY]	= RGB(0, 0, 255);		// 检测Monster密度
		m_clDraw[REGION_BLOCK]			= RGB(128,128,128);		//区域内有阻挡点
	}

	ifs.close();
	m_ScriptList.SetExtendedStyle(
		LVS_EX_FLATSB
		|LVS_EX_FULLROWSELECT
		|LVS_EX_HEADERDRAGDROP
		|LVS_EX_ONECLICKACTIVATE
		|LVS_EX_GRIDLINES
		);
	m_ScriptList.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 56);
	return TRUE;  // 除非将焦点设置到控件,否则返回 TRUE
}

void CUniEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮,则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序,
//  这将由框架自动完成。

void CUniEditorDlg::OnPaint()
{
	static int i = 0;
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}
	
	else	// 重绘地图场景
	{
		
		// 当前选择根结点, 无须绘制地图场景
		if(m_uRegionCurr < ITEM_REGION_UNEXISTED)
		{
			CRect rect(rcMap);
			InvalidateRect(rect+ptMap);
		}

		else
		{
			//CRect rect(rcLog);
			//InvalidateRect(rect+ptLog);
			//rect = rcDlg;
			//InvalidateRect(rect+ptDlg);
			DrawMap();
		}
		
		CDialog::OnPaint();
	}
}

void CUniEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bShift)
		m_bCreateNew=false;
	CRect rect(m_rcMap);
	CPoint pt1(point);
	point -= ptMap;
	
	CPoint pt(m_ptFrom);
	CPoint ptMove;
	//if(m_bLButton)
	//{	
	//	ptMove = m_ptFrom - point;
	//	m_ptFrom = point;
	//}
	point.x=(point.x+hcurrpos)/m_nTimes;
	point.y=(point.y+vcurrpos)/m_nTimes;
	//快捷移动
	if(m_bLButton&&m_bSpace)
		{
			HCURSOR   hCur   =   ::LoadCursor(NULL,IDC_SIZEALL);   
			::SetCursor(hCur);
			
			//OnHScroll(SB_THUMBTRACK, ::hcurrpos+ptMove.x, &m_HScroll);
			//OnVScroll(SB_THUMBTRACK, ::vcurrpos+ptMove.y, &m_VScroll);
			if(pt.x>point.x&&pt.y>point.y)
			{
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINERIGHT,1);
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEDOWN,1);
				
			}
			else if(pt.x<point.x&&pt.y<point.y)
			{
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINELEFT,1);
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEUP,1);
				
			}
			else if(pt.x<point.x&&pt.y>point.y)	
			{
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINELEFT,1);
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEDOWN,1);
			}
			else if(pt.x>point.x&&pt.y<point.y)
			{
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINERIGHT,1);
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEUP,1);
			}	
			else if(pt.x==point.x&&pt.y>point.y)
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEDOWN,1);
			else if(pt.x==point.x&&pt.y<point.y)
				::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_LINEUP,1);
			else if(pt.x>point.x&&pt.y==point.y)
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINERIGHT,1);
			else if(pt.x<point.x&&pt.y==point.y)
				::SendMessage(this->GetSafeHwnd(),WM_HSCROLL,SB_LINELEFT,1);
			
		
		}
	if(rect.PtInRect(point) &&  m_uRegionCurr>1)
	{
	
		//point.Offset(2,2);
		m_toolTip.Activate(TRUE);

		CString str;
		
		
		// 判断阻挡
		if(pt1.x<900&&pt1.y<700)
		{
			if(0 == m_pCell[(point.y) * m_lRegionWidth + point.x ].bBlock)
			{
				str.Format(_T("(%d, %d): 无阻挡.\r\n"), point);
			}

			else
			{
				str.Format(_T("(%d, %d): 有阻挡.\r\n"), point);
			}
		}

		point.Offset(1,1);
		unsigned uMonsterNo = 0, uNPCNo = 0,uSignPosNo=0,uRoadsPoint=0;
		uNPCNo = GetCollection(point);
		uMonsterNo = GetMonster(point);
		uSignPosNo = GetSignPos(point);
		uRoadsPoint =GetRoadsPoint(point);
		if(MAX_NUM != uNPCNo)
		{
			str += GetEditInfo(uNPCNo, point, ITEM_OFFSET_NPC);
		}

		if(MAX_NUM != uMonsterNo)
		{
			str += GetEditInfo(uMonsterNo, point, ITEM_OFFSET_MONSTER);
		}
		if(MAX_NUM != uSignPosNo)
		{
			str += GetEditInfo(uSignPosNo, point, ITEM_OFFSET_SIGNPOS);
		}
		if(MAX_NUM != uRoadsPoint)
		{
			str += GetEditInfo(uRoadsPoint, point, ITEM_OFFSET_ROADS);
		}
		if (m_rcMonsterDensity.PtInRect(point))
		{
			str += MonsterDensity();
		}

		m_toolTip.UpdateTipText(str,this);
		m_toolTip.Update();
		m_listLog.UpdateWindow();
		PostMessage(WM_PAINT);

		switch(m_uLayer)
		{
		case ITEM_LAYER_REGION:
		case ITEM_LAYER_TYPE:
			switch(m_uOffset)
			{
			case ITEM_OFFSET_MONSTER:
				break;

			case ITEM_OFFSET_NPC:
				break;
			case ITEM_OFFSET_SIGNPOS:
				break;
			}

			if(LBS_DOWN_SELECT_RECT == m_uLBStyle)
			{
				CPoint ptTL(m_rcMonsterDensity.TopLeft());
				m_rcMonsterDensity.SetRect(ptTL, point);
			}
			break;

		case ITEM_LAYER_EDIT:
			{
				CRect rcEdit;
				if(ITEM_OFFSET_MONSTER == m_uParentOffset)
				{
					rcEdit = m_vecMonster[m_uOffset]->GetCoordinate();
				}

				else if(ITEM_OFFSET_NPC == m_uParentOffset)
				{
					rcEdit = m_vecCollection[m_uOffset]->GetCoordinate();
				}
				else if(ITEM_OFFSET_SIGNPOS==m_uParentOffset)
				{
					rcEdit.left = m_vecSignPos[m_uOffset]->GetxPos();
					rcEdit.top =m_vecSignPos[m_uOffset]->GetyPos();
					rcEdit.right=rcEdit.left+3;
					rcEdit.bottom=rcEdit.top+3;
				}
			
				
				// 鼠标左键按下, 拖动鼠标变换大小
				if(m_uLBStyle > 0)
				{
					CPoint pt1(point - m_ptOrigin);
	
					switch(m_uLBStyle)
					{
					case LBS_DOWN_LEFT_TOP:
						rcEdit.left += pt1.x;
						rcEdit.top += pt1.y;
						break;

					case LBS_DOWN_RIGHT_BOTTOM:
						rcEdit.right += pt1.x;
						rcEdit.bottom += pt1.y;
						break;

					default:
						break;
					}

					m_ptOrigin = point;
					//m_ptOrigin.x=point.x*m_nTimes;
				//	m_ptOrigin.y=point.y*m_nTimes;
				
					if(ITEM_OFFSET_MONSTER == m_uParentOffset)
					{
						if(m_vecMonster[m_uOffset]->SetCoor(rcEdit, m_lRegionWidth, m_lRegionHeight))
						{
							m_dlgMonster.SetDlg(m_mapTree[m_uRegionCurr], m_vecMonster[m_uOffset]);
						}
					}

					else if((ITEM_OFFSET_NPC == m_uParentOffset))
					{
						if(m_vecCollection[m_uOffset]->SetCoor(rcEdit, m_lRegionWidth, m_lRegionHeight))
						{
							m_dlgCollection.SetDlg(m_mapTree[m_uRegionCurr], m_vecCollection[m_uOffset]);
						}
					}
					else if(ITEM_OFFSET_SIGNPOS== m_uParentOffset)
					{

					}
				}
				
				// 鼠标右键按下, 移动鼠标变换位置
				else if(m_bRBDown)
				{
					if(ITEM_OFFSET_MONSTER == m_uParentOffset)
					{
						m_vecMonster[m_uOffset]->OffsetCoor(point - m_ptOrigin, m_lRegionWidth, m_lRegionHeight);
						m_dlgMonster.SetDlg(m_mapTree[m_uRegionCurr], m_vecMonster[m_uOffset]);
					}

					else if(ITEM_OFFSET_NPC == m_uParentOffset)
					{
						m_vecCollection[m_uOffset]->OffsetCoor(point - m_ptOrigin, m_lRegionWidth, m_lRegionHeight);
						m_dlgCollection.SetDlg(m_mapTree[m_uRegionCurr], m_vecCollection[m_uOffset]);
					}
					else if(ITEM_OFFSET_SIGNPOS== m_uParentOffset)
					{
						int x,y;
						x=m_vecSignPos[m_uOffset]->GetxPos();
						y=m_vecSignPos[m_uOffset]->GetyPos();
						m_vecSignPos[m_uOffset]->SetxPos(x+point.x-m_ptOrigin.x);
						m_vecSignPos[m_uOffset]->SetyPos(y+point.y-m_ptOrigin.y);
						m_dlgSignPos.SetDlg(m_mapTree[m_uRegionCurr],m_vecSignPos[m_uOffset]);
					}
					m_ptOrigin = point;
				}
			}
			break;

		case ITEM_LAYER_MONSTER:
			{
				CRect rcEdit;
				HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
				hItem=m_ptreeRegion->GetParentItem(hItem);
				string str=m_ptreeRegion->GetItemText(hItem);
				HTREEITEM hItem1=m_ptreeRegion->GetParentItem(hItem);
				CString str1=m_ptreeRegion->GetItemText(hItem1);
				if("路径"==str1)
				{
					VECROADPOINT vecroad=m_mapRoads[atoi(str.c_str())]->GetRoadsPoints();

					rcEdit.left = vecroad[m_uOffset].nx;
					rcEdit.top =vecroad[m_uOffset].ny;
					rcEdit.right=rcEdit.left+3;
					rcEdit.bottom=rcEdit.top+3;
					// 鼠标右键按下, 移动鼠标变换位置
					if(m_bRBDown)
					{
						
						int x,y;
						x=vecroad[m_uOffset].nx;
						y=vecroad[m_uOffset].ny; 
						
						m_mapRoads[atoi(str.c_str())]->GetRoadsPoints()[m_uOffset].nx=x+point.x-m_ptOrigin.x;
						m_mapRoads[atoi(str.c_str())]->GetRoadsPoints()[m_uOffset].ny=y+point.y-m_ptOrigin.y;
						int& nx = m_mapRoads[atoi(str.c_str())]->GetRoadsPoints()[m_uOffset].nx;
						int& ny = m_mapRoads[atoi(str.c_str())]->GetRoadsPoints()[m_uOffset].ny;
						MAPROADSITR it;
						it=m_mapRoads.find(atoi(str.c_str()));
						if (it == m_mapRoads.end())
						{
							break;
						}
						VECROADPOINT vecpoint=(*it->second).GetRoadsPoints();	
						
						m_dlgRoadsPoint.SetDlg(m_mapTree[m_uRegionCurr], it->first,&(*it->second).GetRoadsPoints()[m_uOffset],TRUE);
					//	char strVal[32];
					//	sprintf_s(strVal,"<%d,%d>",nx,ny);
					//	m_ptreeRegion->SetItemText(m_ptreeRegion->GetSelectedItem(),strVal);
						m_ptOrigin=point;
						
					}

				}
			}
			break;

		default:
			break;
		}
	}

	else
	{
		m_toolTip.Activate(FALSE);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CUniEditorDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// The layer that can be EDITED
	m_bRButton=true;
	
	//m_bAlt=true;
	if(m_bCtrl)
	{
		if(m_nTimes<4)
		{
			m_HScroll.ShowScrollBar();
			m_VScroll.ShowScrollBar();
			m_HScroll.SetScrollRange(0,m_lRegionWidth*m_nTimes+300);
			m_VScroll.SetScrollRange(0,m_lRegionHeight*m_nTimes+300);
			m_nTimes=m_nTimes*2;
			DrawRegion(m_nTimes);
			if(bJpgShow)
			DrawJpg(m_uRegionCurr);
		}
	}
	CRect rect(m_rcMap);
	point -= ptMap;
	//g_point=point;
	point.x=(point.x+hcurrpos)/m_nTimes;
	point.y=(point.y+vcurrpos)/m_nTimes;
	point.Offset(1, 1);

	if(rect.PtInRect(point) && m_uRegionCurr>1 /*&&ITEM_LAYER_EDIT == m_uLayer*/ )
	{
		m_bRBDown = TRUE;
		m_bMouseDown = TRUE;
		m_ptOrigin = point;
	}

	CDialog::OnRButtonDown(nFlags, point);
}

void CUniEditorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bRButton=false;
	//m_bAlt=false;
	if (m_bModify)
	{
		m_bModify = false;
		m_listLog.AddLog(LOG_MOD, UpdateLog());
	}
	HTREEITEM hItem=m_ptreeRegion->GetSelectedItem();
	hItem=m_ptreeRegion->GetParentItem(hItem);
	hItem=m_ptreeRegion->GetParentItem(hItem);
	CString str=m_ptreeRegion->GetItemText(hItem);
	if(str!="路径")
	{

		
		if(m_bRBDown && ITEM_OFFSET_MONSTER == m_uParentOffset && ITEM_LAYER_EDIT == m_uLayer)
		{
			point -= ptMap;
			point.x=(point.x+hcurrpos)/m_nTimes;
			point.y=(point.y+vcurrpos)/m_nTimes;
			point.Offset(1, 1);

			m_vecMonster[m_uOffset]->OffsetCoor(point - m_ptOrigin, m_lRegionWidth, m_lRegionHeight);
			m_dlgMonster.SetDlg(m_mapTree[m_uOffset], m_vecMonster[m_uOffset]);
			PostMessage(WM_PAINT);
		}

		else if(m_bRBDown && ITEM_OFFSET_NPC == m_uParentOffset && ITEM_LAYER_EDIT == m_uLayer)
		{
			point -= ptMap;
			point.x=(point.x+hcurrpos)/m_nTimes;
			point.y=(point.y+vcurrpos)/m_nTimes;
			point.Offset(1, 1);

			m_vecCollection[m_uOffset]->OffsetCoor(point - m_ptOrigin, m_lRegionWidth, m_lRegionHeight);
			m_dlgCollection.SetDlg(m_mapTree[m_uOffset], m_vecCollection[m_uOffset]);
			PostMessage(WM_PAINT);
		}
	}
	m_bRBDown = FALSE;
	m_bMouseDown = FALSE;

	CDialog::OnRButtonUp(nFlags, point);
}
void CUniEditorDlg::SearchTreeNode(CPoint point,UINT ntpye)
{
	VECMONSTERITR iter;
	iter=m_vecMonster.begin();
	CRect clkrect;
	unsigned i=0;
	for(;iter!=m_vecMonster.end();iter++)
	{
		
		//检测点击位置
		clkrect=(*iter)->GetCoordinate();
		//在矩形中
		if(clkrect.PtInRect(point))
		{
			i=(*iter)->GetNo();
		}
	}
	unsigned ucurrNo=i;
	char strVal[10];
	itoa(i,strVal,10);
	string str1(strVal);
	HTREEITEM hItem,nextItem;
	hItem=m_ptreeRegion->GetSelectedItem();

	nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
	CString str=m_ptreeRegion->GetItemText(nextItem);			
	HTREEITEM monsterItem;
	monsterItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_CHILD);
	//查找树结点
	while(monsterItem!=NULL)
	{
		if(m_ptreeRegion->GetItemText(monsterItem).Compare(str1.c_str())==0)
			{
				m_ptreeRegion->SetFocus();
				m_ptreeRegion->SelectItem(monsterItem);
				
				break;
			}
		monsterItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_NEXT);
	}
			
}
void CUniEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// The layer that can be EDITED
	m_bLButton=true;
	
	//if(m_bRButton)
	//{
		m_ptFrom.x=point.x;
		m_ptFrom.y=point.y;
	//}
	if(m_bCtrl)
	{
		int n=m_nTimes/2;
		if(n<1)
			return;
		m_nTimes/=2;
		DrawRegion(m_nTimes);
		if(bJpgShow)
		DrawJpg(m_uRegionCurr);
	}
	CRect rect(m_rcMap);
	
	point -= ptMap;
	
	point.x=(point.x+hcurrpos)/m_nTimes;
	point.y=(point.y+vcurrpos)/m_nTimes;
	point.Offset(1, 1);
	g_point=point;
	m_ptFrom=point;
	CPoint pt;
	pt.x=point.x-1;
	pt.y=point.y-1;
	if(rect.PtInRect(point) && m_uRegionCurr>1)
	{
		switch(m_uLayer)
		{
		case ITEM_LAYER_REGION:
		case ITEM_LAYER_TYPE:
			m_uLBStyle = LBS_DOWN_SELECT_RECT;
			
			m_rcMonsterDensity.SetRect(pt, pt);
			break;

		case ITEM_LAYER_EDIT:
			{
				CRect rcEdit;
				// 拖拽MONSTER
				if(ITEM_OFFSET_MONSTER == m_uParentOffset)
				{
					rcEdit = m_vecMonster[m_uOffset]->GetCoordinate();
				}

				else if(ITEM_OFFSET_NPC==m_uParentOffset)
				{
					rcEdit = m_vecCollection[m_uOffset]->GetCoordinate();
				}
				else if(ITEM_OFFSET_SIGNPOS==m_uParentOffset)
				{
					
				}
				if((point.x>rcEdit.left-5) && (point.x<rcEdit.left+5) || (point.y>rcEdit.top-5) && (point.y<rcEdit.top+5))
				{
					// 设置鼠标样式
					m_uLBStyle = LBS_DOWN_LEFT_TOP;
				}

				else if((point.x>rcEdit.right-5) && (point.x<rcEdit.right+5) || (point.y>rcEdit.bottom-5) && (point.y<rcEdit.bottom+5))
				{
					// 设置鼠标样式
					m_uLBStyle = LBS_DOWN_RIGHT_BOTTOM;
				}

				// 可以变换大小
				if(m_uLBStyle > 0)
				{
					m_ptOrigin = point;
				}
				break;
			}

		default:
			break;
		}
	}

	else
	{
		m_uLBStyle = LBS_NOTDOWN;
	}

	m_bMouseDown = TRUE;
	CDialog::OnLButtonDown(nFlags, point);
}

void CUniEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 
	m_bLButton=false;
	if (m_bModify)
	{
		m_bModify = false;
		m_listLog.AddLog(LOG_MOD, UpdateLog());
	}

	if(LBS_DOWN_SELECT_RECT == m_uLBStyle)
	{
	}

	else if(m_uLBStyle > LBS_DOWN_SELECT_RECT)
	{
		CRect rect;
		if(ITEM_OFFSET_MONSTER == m_uParentOffset)
		{
			rect = m_vecMonster[m_uOffset]->GetCoordinate();
		}

		else if(ITEM_OFFSET_NPC == m_uParentOffset)
		{
			rect = m_vecCollection[m_uOffset]->GetCoordinate();
		}

		point -= ptMap;
		point.x=(point.x+hcurrpos)/m_nTimes;
		point.y=(point.y+vcurrpos)/m_nTimes;
		point.Offset(1, 1);

		CPoint pt(point - m_ptOrigin);

		switch(m_uLBStyle)
		{
		case LBS_DOWN_SELECT_RECT:
			break;

		case LBS_DOWN_LEFT_TOP:
			rect.left += pt.x;
			rect.top += pt.y;
			break;

		case LBS_DOWN_RIGHT_BOTTOM:
			rect.right += pt.x;
			rect.bottom += pt.y;
			break;

		default:
			break;
		}

		if(ITEM_OFFSET_MONSTER == m_uParentOffset)
		{
			if(m_vecMonster[m_uOffset]->SetCoor(rect, m_lRegionWidth, m_lRegionHeight))
			{
				PostMessage(WM_PAINT);
				m_dlgMonster.SetDlg(m_mapTree[m_uRegionCurr], m_vecMonster[m_uOffset]);
			}
		}

		else if(ITEM_OFFSET_NPC == m_uParentOffset)
		{
			if(m_vecCollection[m_uOffset]->SetCoor(rect, m_lRegionWidth, m_lRegionHeight))
			{
				PostMessage(WM_PAINT);
				m_dlgCollection.SetDlg(m_mapTree[m_uRegionCurr], m_vecCollection[m_uOffset]);
			}
		}
	}

	m_uLBStyle = LBS_NOTDOWN;
	m_bMouseDown = FALSE;
	if(m_bShift)
	m_bCreateNew = true;

	CDialog::OnLButtonUp(nFlags, point);
}

void CUniEditorDlg::OnAddItem()
{
	// TODO: 在此添加命令处理程序代码
}

void CAboutDlg::OnDelItem()
{
	// TODO: 在此添加命令处理程序代码
}

BOOL CUniEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	static BOOL bCtrl = FALSE;
	switch(pMsg->message)
	{
	case WM_CLOSE: 
		if(!(m_setUpdate.empty()))
		{
			CDlgFileList dlgFileList;
			dlgFileList.DoModal();
		}
		return TRUE;

	case WM_KEYUP:
		if(pMsg->wParam == VK_SPACE)
			{
				m_bSpace=FALSE;
				return TRUE;
			}
		if(pMsg->wParam == VK_CONTROL)
		{
			m_bCtrl=false;
			bCtrl = FALSE;
			return TRUE;
		}
		if(pMsg->wParam == VK_SHIFT)
		{
			
			m_bShift=false;
			return TRUE;
		}
		
	case WM_KEYDOWN:
		{
			if(pMsg->wParam == VK_ESCAPE)
			{
				return TRUE;
			}
			if(pMsg->wParam == VK_SHIFT)
			{
				
				m_bShift=true;
				return TRUE;
			}
			case VK_ADD:
			if(pMsg->wParam == VK_CONTROL)
			{
				m_bCtrl=true;
				bCtrl = TRUE;
				return TRUE;
			}
			if(pMsg->wParam == VK_SPACE)
			{
				m_bSpace=TRUE;
				return TRUE;
			}
			// Ctrl + E修改颜色
			if(bCtrl && (pMsg->wParam == _T('e') || pMsg->wParam == _T('E')))
			{
				CColorDialog dlgColor;
				//dlgColor.GetColor();
				dlgColor.DoModal();
				return TRUE;
			}

			// Ctrl + S保存
			else if(!m_setUpdate.empty() && bCtrl && (pMsg->wParam == _T('s') || pMsg->wParam == _T('S')))
			{
				for(SETUPDATEITR itr=m_setUpdate.begin(); itr!=m_setUpdate.end(); )
				{
					if(SaveFile(itr->c_str()))
					{
						itr = m_setUpdate.erase(itr);
					}

					else
					{
						++itr;
					}
				}
				
				if (0 == m_setUpdate.size())
				{
					GetDlgItem(IDC_BTN_SAVE)->ShowWindow(SW_HIDE);
				}
				return TRUE;
			}
		}	
	}
	
	if(m_toolTip.m_hWnd != NULL)
	{
		m_toolTip.RelayEvent(pMsg);
	}
	return((m_hAccelTable!=NULL)&&::TranslateAccelerator(m_hWnd,m_hAccelTable,pMsg));
}

LRESULT CUniEditorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	switch(message)
	{
	case WM_CLOSE: 
		if(!(m_setUpdate.empty()))
		{
			CDlgFileList dlgFileList;
			INT_PTR nResponse = dlgFileList.DoModal();
			if (nResponse == IDCANCEL)
			{
				if (IDOK == MessageBox(_T("存在未保存的被编辑文件，是否退出程序？"), _T("退出"), MB_OKCANCEL))
				{
					PostQuitMessage(0);
				}

				else
				{
					return TRUE;
				}
			}
		}
		break;

	default:
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CUniEditorDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	for(SETUPDATEITR itr=m_setUpdate.begin(); itr!=m_setUpdate.end();)
	{
		if(SaveFile(itr->c_str()))
		{			
			itr = m_setUpdate.erase(itr);
		}

		else
		{
			 ++itr;
		}
	}

	if (0 == m_setUpdate.size())
	{
		GetDlgItem(IDC_BTN_SAVE)->ShowWindow(SW_HIDE);
	}
}
string CUniEditorDlg::SearchRoadsPoint(CPoint point)
{
	MAPROADSITR itr=m_mapRoads.begin();
	CRect rect;
	for (;itr!=m_mapRoads.end();itr++)
	{
		VECROADPOINT vecroads=(*itr->second).GetRoadsPoints();
		VECROADPOINTITR itpoint=vecroads.begin();
		for (;itpoint!=vecroads.end();itpoint++)
		{	
			rect.left=(*itpoint).nx-2;
			rect.top=(*itpoint).ny-2;
			rect.right=rect.left+8;
			rect.bottom=rect.top+8;
			if(rect.PtInRect(point))
			{
				char strVal[32];
				sprintf_s(strVal,"<%d,%d>",(*itpoint).nx,(*itpoint).ny);
				/*int i=(*itpoint).npointNo;
				itoa(i,strVal,10);
				string str(strVal);*/
				return strVal;
			}
		}
	}
	return "";
}
void CUniEditorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect(m_rcMap);
	point -=ptMap;
	point.x=(point.x+hcurrpos)/m_nTimes;
	point.y=(point.y+vcurrpos)/m_nTimes;
	point.Offset(1, 1);
	

	if(rect.PtInRect(point) && m_uRegionCurr>1)
	{
		switch(m_uLayer)
		{
		case ITEM_LAYER_REGION:
			{
				//检测选中矩形并定位树节点
				VECMONSTERITR iter;
				VECCOLLECTIONITR itercoll;
				CRect clkrect;
				unsigned i=0;
				for(iter=m_vecMonster.begin();iter!=m_vecMonster.end();iter++)
				{	
					//检测点击位置
					clkrect=(*iter)->GetCoordinate();
					if(clkrect.PtInRect(point))
					{
						i=(*iter)->GetNo();
						
					}
					
				}
				
				unsigned ucurrNo=i;
				char strVal[10];
				itoa(i,strVal,10);
				string str1(strVal);
				HTREEITEM hItem,nextItem,parentItem;
				hItem=m_ptreeRegion->GetSelectedItem();
				parentItem=m_ptreeRegion->GetRootItem();
				CString strRoot=m_ptreeRegion->GetItemText(parentItem);
				nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
				CString str=m_ptreeRegion->GetItemText(nextItem);			
				HTREEITEM monsterItem;
				monsterItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_CHILD);
				//查找树结点
				while(monsterItem!=NULL)
				{
					if(m_ptreeRegion->GetItemText(monsterItem).Compare(str1.c_str())==0)
						{
							m_ptreeRegion->SetFocus();
							m_ptreeRegion->SelectItem(monsterItem);
							
							break;
						}
					monsterItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_NEXT);
				}	


				for(itercoll=m_vecCollection.begin();itercoll!=m_vecCollection.end();itercoll++)
				{	
					//检测点击位置
					clkrect=(*itercoll)->GetCoordinate();
					if(clkrect.PtInRect(point))
					{
						i=(*itercoll)->GetNo();
						
					}
					
				}
				ucurrNo=i;

				itoa(i,strVal,10);
				str1=strVal;
	
				hItem=m_ptreeRegion->GetSelectedItem();
				parentItem=m_ptreeRegion->GetRootItem();
				strRoot=m_ptreeRegion->GetItemText(parentItem);
				nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
				nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
				str=m_ptreeRegion->GetItemText(nextItem);			
		
				monsterItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_CHILD);
				//查找树结点
				while(monsterItem!=NULL)
				{
					if(m_ptreeRegion->GetItemText(monsterItem).Compare(str1.c_str())==0)
						{
							m_ptreeRegion->SetFocus();
							m_ptreeRegion->SelectItem(monsterItem);
							
							break;
						}
					monsterItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_NEXT);
				}	
			}
			break;
		case ITEM_LAYER_TYPE:
			{
				VECMONSTERITR iter;
				VECCOLLECTIONITR itercoll;
				VECSIGNPOSITR itersignpos;
				iter=m_vecMonster.begin();
				itercoll=m_vecCollection.begin();
				itersignpos=m_vecSignPos.begin();
				CRect clkrect;
				unsigned i=0,j=0,m=0;
				//判断是否点中monster的区域
				if(m_bMonster)
				{
					for(;iter!=m_vecMonster.end();iter++)
					{
						
						//检测点击位置
						clkrect=(*iter)->GetCoordinate();
						//在矩形中
						if(clkrect.PtInRect(point))
						{
							i=(*iter)->GetNo();
						}
					}
				}
				//判断是否点中采集物的区域
				if(m_bCollection)
				{
					for(;itercoll!=m_vecCollection.end();itercoll++)
					{
						
						//检测点击位置
						clkrect=(*itercoll)->GetCoordinate();
						//在矩形中
						if(clkrect.PtInRect(point))
						{
							j=(*itercoll)->GetNo();
						}
					}
				}
				//判断是否点中路标点区域
				if(m_bSignPos)
				{
					for(;itersignpos!=m_vecSignPos.end();itersignpos++)
					{
						
						//检测点击位置
						clkrect.left=(*itersignpos)->GetxPos()-2;
						clkrect.top=(*itersignpos)->GetyPos()-2;
						clkrect.right=clkrect.left+8;
						clkrect.bottom=clkrect.top+8;
						//在矩形中
						
						if(clkrect.PtInRect(point))
						{
							m=(*itersignpos)->GetNo();
							break;
						}
					}
				}
				unsigned ucurrNo2=m;
				unsigned ucurrNo1=j;
				unsigned ucurrNo=i;
				char strVal[10];
				char strVal1[10];
				char strVal2[10];
				itoa(i,strVal,10);
				itoa(j,strVal1,10);
				itoa(m,strVal2,10);
				string str1(strVal);
				string str2(strVal1);
				string str3(strVal2);
				
				HTREEITEM hItem,nextItem;
				hItem=m_ptreeRegion->GetSelectedItem();
				CString strmonster=m_ptreeRegion->GetItemText(hItem);
				if("怪物"==strmonster)
				{
					nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
					CString str=m_ptreeRegion->GetItemText(nextItem);			
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str1.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(nextItem);
								
								break;
							}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
				else if("采集物"==strmonster)
				{
					nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
					CString str=m_ptreeRegion->GetItemText(nextItem);			
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str2.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(nextItem);
								
								break;
							}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
				else if("路标点"==strmonster)
				{
					nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
					CString str=m_ptreeRegion->GetItemText(nextItem);			
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str3.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(nextItem);
								
								break;
							}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
				else if("路径"==strmonster)
				{
					string str4=SearchRoadsPoint(point);
					nextItem=m_ptreeRegion->GetNextItem(hItem,TVGN_CHILD);
					CString str=m_ptreeRegion->GetItemText(nextItem);
					HTREEITEM itemChild = m_ptreeRegion->GetNextItem(nextItem,TVGN_CHILD);
					str=m_ptreeRegion->GetItemText(itemChild);
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str4.c_str())==0)
						{
							str=m_ptreeRegion->GetItemText(nextItem);
							m_ptreeRegion->SetFocus();
							m_ptreeRegion->SelectItem(nextItem);
							
							break;
						}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
			m_uLBStyle = LBS_DOWN_SELECT_RECT;
			CPoint pt;
			pt.x=point.x-1;
			pt.y=point.y-1;
			m_rcMonsterDensity.SetRect(pt, pt);
			}
			break;
		case ITEM_LAYER_EDIT:
			{
				HTREEITEM allItem=m_ptreeRegion->GetSelectedItem();
				allItem=m_ptreeRegion->GetParentItem(allItem);
				CString str=m_ptreeRegion->GetItemText(allItem);
				CRect clkrect(rcMap);
				int i=0;
				unsigned ucurrNo=0;
				char strVal[10];
				if("怪物"==str)
				{
					VECMONSTERITR iter;
					iter=m_vecMonster.begin();
					unsigned i=0,sum=0;
					
					for(;iter!=m_vecMonster.end();iter++)
					{
						clkrect=(*iter)->GetCoordinate();
						//检测点击位置
						if(clkrect.PtInRect(point))
						{
							//选中最小的区域
							if((*iter)->GetCoordinate().left>rcMap.left||
								(*iter)->GetCoordinate().top>rcMap.top||
								(*iter)->GetCoordinate().bottom<rcMap.bottom||
								(*iter)->GetCoordinate().right <rcMap.right)
							{
								clkrect=(*iter)->GetCoordinate();
					
								i=(*iter)->GetNo();
							}
							
						}
						
					}
					
					ucurrNo=i;
					itoa(i,strVal,10);
					string str1(strVal);
					HTREEITEM hItem,parentItem,monsterItem;
					//当前选择节点
					hItem=m_ptreeRegion->GetSelectedItem();
					//当前选择节点的父节点
					parentItem=m_ptreeRegion->GetParentItem(hItem);
					if(0==ucurrNo)
						{
							m_ptreeRegion->SelectItem(parentItem);
							return;
						}
					//从父节点的第一个节点开始循环
					monsterItem=m_ptreeRegion->GetNextItem(parentItem,TVGN_CHILD);
					while(monsterItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(monsterItem).Compare(str1.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(monsterItem);
								
								break;
							}
						monsterItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_NEXT);
					}
				}
				//快捷选择采集物区域
				if("采集物"==str)
				{
					VECCOLLECTIONITR itercoll;
					itercoll=m_vecCollection.begin();
					for(;itercoll!=m_vecCollection.end();itercoll++)
					{
						clkrect=(*itercoll)->GetCoordinate();
						//检测点击位置
						if(clkrect.PtInRect(point))
						{
							//选中最小的区域
							if((*itercoll)->GetCoordinate().left>rcMap.left||
								(*itercoll)->GetCoordinate().top>rcMap.top||
								(*itercoll)->GetCoordinate().bottom<rcMap.bottom||
								(*itercoll)->GetCoordinate().right <rcMap.right)
							{
								clkrect=(*itercoll)->GetCoordinate();
					
								i=(*itercoll)->GetNo();
							}
							
						}
						
					}
					ucurrNo=i;
					itoa(i,strVal,10);
					string str1(strVal);
					HTREEITEM hItemcoll,collItem,parentItem;
					hItemcoll=m_ptreeRegion->GetSelectedItem();
					//当前选择节点的父节点
					parentItem=m_ptreeRegion->GetParentItem(hItemcoll);
					if(0==ucurrNo)
						{
							m_ptreeRegion->SelectItem(parentItem);
							return;
						}
					//从父节点的第一个节点开始循环
					collItem=m_ptreeRegion->GetNextItem(parentItem,TVGN_CHILD);
					while(collItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(collItem).Compare(str1.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(collItem);
								
								break;
							}
						collItem=m_ptreeRegion->GetNextItem(collItem,TVGN_NEXT);
					}

				}
				if("路标点"==str)
				{
					VECSIGNPOSITR itersignpos;
					itersignpos=m_vecSignPos.begin();
					for(;itersignpos!=m_vecSignPos.end();itersignpos++)
					{
						clkrect.left=(*itersignpos)->GetxPos()-2;
						clkrect.top=(*itersignpos)->GetyPos()-2;
						clkrect.right=clkrect.left+8;
						clkrect.bottom=clkrect.top+8;
						if(clkrect.PtInRect(point))
						{
							i=(*itersignpos)->GetNo();
						}
					}
					ucurrNo=i;
					itoa(i,strVal,10);
					string str1(strVal);
					HTREEITEM hItemcoll,collItem,parentItem;
					hItemcoll=m_ptreeRegion->GetSelectedItem();
					//当前选择节点的父节点
					parentItem=m_ptreeRegion->GetParentItem(hItemcoll);
					if(0==ucurrNo)
						{
							m_ptreeRegion->SelectItem(parentItem);
							return;
						}
					//从父节点的第一个节点开始循环
					collItem=m_ptreeRegion->GetNextItem(parentItem,TVGN_CHILD);
					while(collItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(collItem).Compare(str1.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(collItem);
								
								break;
							}
						collItem=m_ptreeRegion->GetNextItem(collItem,TVGN_NEXT);
					}

				}
				if ("路径"==str)
				{
					string str1=SearchRoadsPoint(point);
					HTREEITEM hItemRoads,collItem,parentItem;
					hItemRoads=m_ptreeRegion->GetSelectedItem();
					CString str=m_ptreeRegion->GetItemText(hItemRoads);
					//当前选择节点的父节点
					parentItem=m_ptreeRegion->GetParentItem(hItemRoads);
					if(0==atoi(str1.c_str()))
					{
						m_ptreeRegion->SelectItem(parentItem);
						return;
					}
					//从父节点的第一个节点开始循环
					collItem=m_ptreeRegion->GetNextItem(hItemRoads,TVGN_CHILD);
					while(collItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(collItem).Compare(str1.c_str())==0)
						{
							m_ptreeRegion->SetFocus();
							m_ptreeRegion->SelectItem(collItem);

							break;
						}
						collItem=m_ptreeRegion->GetNextItem(collItem,TVGN_NEXT);
					}
				}
				CRect rcEdit;
				
			}
			break;
		case 4:
			{
				VECMONSTERITR iter;
				iter=m_vecMonster.begin();
				CRect clkrect(rcMap);
				unsigned i=0,sum=0;
				for(;iter!=m_vecMonster.end();iter++)
				{
					clkrect=(*iter)->GetCoordinate();
					//检测点击位置
					if(clkrect.PtInRect(point))
					{
						//选中最小的区域
						if((*iter)->GetCoordinate().left>rcMap.left||
							(*iter)->GetCoordinate().top>rcMap.top||
							(*iter)->GetCoordinate().bottom<rcMap.bottom||
							(*iter)->GetCoordinate().right <rcMap.right)
						{
							clkrect=(*iter)->GetCoordinate();
				
							i=(*iter)->GetNo();
						}
						
					}
					
				}
				unsigned ucurrNo=i;
				char strVal[10];
				itoa(i,strVal,10);
				string str1(strVal);
				HTREEITEM hItem,parentItem,monsterItem,nextItem;
				//当前选择节点
				hItem=m_ptreeRegion->GetSelectedItem();
				parentItem=m_ptreeRegion->GetParentItem(hItem);
				monsterItem=m_ptreeRegion->GetParentItem(parentItem);
				CString strmonster=m_ptreeRegion->GetItemText(monsterItem);
				if("怪物"==strmonster)
				{
					nextItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_CHILD);		
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str1.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(nextItem);
								
								break;
							}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
				else if("采集物"==strmonster)
				{
					VECCOLLECTIONITR itercoll=m_vecCollection.begin();
					for(;itercoll!=m_vecCollection.end();itercoll++)
					{
						clkrect=(*itercoll)->GetCoordinate();
						if(clkrect.PtInRect(point))
						{
							clkrect=(*itercoll)->GetCoordinate();
				
							i=(*itercoll)->GetNo();
						}
					}
					itoa(i,strVal,10);
					string str2(strVal);
					nextItem=m_ptreeRegion->GetNextItem(monsterItem,TVGN_CHILD);		
					while(nextItem!=NULL)
					{
						if(m_ptreeRegion->GetItemText(nextItem).Compare(str2.c_str())==0)
							{
								m_ptreeRegion->SetFocus();
								m_ptreeRegion->SelectItem(nextItem);
								
								break;
							}
						nextItem=m_ptreeRegion->GetNextItem(nextItem,TVGN_NEXT);
					}
				}
				else if("路径"==strmonster)
				{
					string str=SearchRoadsPoint(point);
					nextItem=m_ptreeRegion->GetSelectedItem();
					nextItem=m_ptreeRegion->GetParentItem(nextItem);
					CString str1=m_ptreeRegion->GetItemText(nextItem);
					HTREEITEM hItemChild=m_ptreeRegion->GetNextItem(nextItem,TVGN_CHILD);
					while(hItemChild!=NULL)
					{
						if(m_ptreeRegion->GetItemText(hItemChild).Compare(str.c_str())==0)
						{	
							m_ptreeRegion->SetFocus();
							m_ptreeRegion->SelectItem(hItemChild);

							break;
						}
						hItemChild=m_ptreeRegion->GetNextItem(hItemChild,TVGN_NEXT);
					}
				}
			
			}
			break;
			default:
				break;
		}
	}

	else
	{
	}


	CDialog::OnLButtonDblClk(nFlags, point);
}

void CUniEditorDlg::OnVkadd()
{
	// TODO: 在此添加命令处理程序代码
	//m_ptreeRegion->DlgADD();
	
}
//缩小
void CUniEditorDlg::OnZoomout()
{
	// TODO: 在此添加命令处理程序代码
	
		int n=m_nTimes/2;
		if(n<1)
			return;
		m_nTimes/=2;
		DrawRegion(m_nTimes);
		if(m_bJpg)
		DrawJpg(m_uRegionCurr);
}
//放大
void CUniEditorDlg::OnZoomin()
{
	// TODO: 在此添加命令处理程序代码
	if(m_nTimes<4)
	{
		m_HScroll.ShowScrollBar();
		m_VScroll.ShowScrollBar();
		m_HScroll.SetScrollRange(0,m_lRegionWidth*m_nTimes+300);
		m_VScroll.SetScrollRange(0,m_lRegionHeight*m_nTimes+300);
		m_nTimes=m_nTimes*2;
		DrawRegion(m_nTimes);
		if(m_bJpg)
		DrawJpg(m_uRegionCurr);
	}
}
//横向滚动条
void CUniEditorDlg::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CBitmap bmp;
	CDC dcMemory;
	CClientDC dc(this);
	switch(SBCode)
	{
	case SB_PAGEUP:
		hcurrpos -= 1;
		break;
	case SB_PAGEDOWN:
		hcurrpos += 1;
		break;
	case SB_LINELEFT:
		if(hcurrpos>0)
		hcurrpos -= m_nTimes;
		break;
	case SB_LINERIGHT:
		if(hcurrpos<1324)
		hcurrpos += m_nTimes;
		break;
	case SB_THUMBPOSITION:
		hcurrpos = nPos;
		break;
	case SB_THUMBTRACK:
		hcurrpos = nPos;
		break;
	default:;	
	}
	m_HScroll.SetScrollPos(hcurrpos);
	
	dcMemory.CreateCompatibleDC(NULL);
	if(!bmp.CreateCompatibleBitmap(&dc, rcMap.right, rcMap.bottom))
	{
		PostMessage(0);
	}

	dcMemory.SelectObject(bmp);
	//重画内存DC中的RGN位图

	dcMemory.BitBlt(rcMap.left, rcMap.top, rcMap.right, rcMap.bottom, &m_dcMemory, hcurrpos, vcurrpos, SRCCOPY);
	if(!m_bRegion)
	{
		if(m_bMonster)
		{
			DrawMonster(dcMemory);	
		}
		if(m_bCollection)
			DrawCollection(dcMemory);
		if(m_bSignPos)
			DrawSignPos(dcMemory);
		if (m_bRoads)
			DrawRoads(dcMemory);

	}
	else
	{
		DrawMonster(dcMemory);	
		DrawCollection(dcMemory);
		DrawSignPos(dcMemory);
		DrawRoads(dcMemory);
		
	}
	DrawMonsterVector(dcMemory);
	DrawCollectionVector(dcMemory);
	dc.BitBlt(rcMap.left+ptMap.cx, rcMap.top+ptMap.cy, 
		rcMap.right+ptMap.cx, rcMap.bottom+ptMap.cy,
		&dcMemory, 0, 0, SRCCOPY);
	
	
	CDialog::OnHScroll(SBCode, nPos, pScrollBar);
}
//纵向滚动条
void CUniEditorDlg::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CBitmap bmp;
	CDC dcMemory;
	CClientDC dc(this);
	
	switch(SBCode)
	{
	case SB_PAGEUP:
		vcurrpos -= 3;
		break;
	case SB_PAGEDOWN:
		vcurrpos += 3;
		break;
	case SB_LINELEFT:
		if(vcurrpos>0)
		vcurrpos -= m_nTimes;
		break;
	case SB_LINERIGHT:
		if(vcurrpos<1324)
		vcurrpos += m_nTimes;
		break;
	case SB_THUMBPOSITION:
		vcurrpos = nPos;
		break;
	case SB_THUMBTRACK:
		vcurrpos = nPos;
		break;
	default:;
		
	}
	m_VScroll.SetScrollPos(vcurrpos);

	dcMemory.CreateCompatibleDC(NULL);
	if(!bmp.CreateCompatibleBitmap(&dc, rcMap.right, rcMap.bottom))
	{
		PostMessage(0);
	}
	
	dcMemory.SelectObject(bmp);
	//重画内存DC中的RGN位图
	
	dcMemory.BitBlt(rcMap.left, rcMap.top, rcMap.right, rcMap.bottom, &m_dcMemory, hcurrpos, vcurrpos, SRCCOPY);
	if(!m_bRegion)
	{
		if(m_bMonster)
		{
			DrawMonster(dcMemory);	
		}
		if(m_bCollection)
			DrawCollection(dcMemory);
		if(m_bSignPos)
			DrawSignPos(dcMemory);
		if (m_bRoads)
			DrawRoads(dcMemory);

	}
	else
	{
		DrawMonster(dcMemory);	
		DrawCollection(dcMemory);
		DrawSignPos(dcMemory);
		DrawRoads(dcMemory);
		
	}
	DrawMonsterVector(dcMemory);
	DrawCollectionVector(dcMemory);
	dc.BitBlt(rcMap.left+ptMap.cx, rcMap.top+ptMap.cy, 
		rcMap.right+ptMap.cx, rcMap.bottom+ptMap.cy,
		&dcMemory, 0, 0, SRCCOPY);
	
	
}

void CUniEditorDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if(bJpgShow)
	{
		bJpgShow=FALSE;
		m_bJpg=false;
		SetDlgItemTextA(IDC_REFRESH,"开启JPG(&D)");
		DrawRegion(m_nTimes);
	}
	else
	{
		bJpgShow=TRUE;
		m_bJpg=true;
		SetDlgItemTextA(IDC_REFRESH,"关闭JPG(&D)");
		DrawJpg(m_uRegionCurr);
	}
	
}

BOOL CUniEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*	if(m_nTimes<4)
		{
			m_HScroll.ShowScrollBar();
			m_VScroll.ShowScrollBar();
			m_HScroll.SetScrollRange(0,m_lRegionWidth*m_nTimes+300);
			m_VScroll.SetScrollRange(0,m_lRegionHeight*m_nTimes+300);
			m_nTimes=m_nTimes*2;
			DrawRegion(m_nTimes);
			if(bJpgShow)
			DrawJpg(m_uRegionCurr);
		}*/
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CUniEditorDlg::OnHdnItemclickNumlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
struct tagInfo
{
	string name;
	int nValue;
};
bool bsort=false;
void CUniEditorDlg::OnLvnColumnclickNumlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//m_listNum.SortItems(Compositor,(DWORD)reinterpret_cast<DWORD>(this));
	if (!bsort)
		bsort=true;
	else
		bsort=false;
	long lsize=(long) m_listNum.GetItemCount();
	vector<tagInfo>vecInfo;
	vector<tagInfo>::iterator itstr;
	for (int i=0;i<lsize;i++)
	{
		tagInfo Info;
		Info.nValue=m_listNum.GetItemData(i);
		string str=m_listNum.GetItemText(i,0);
		Info.name=str;
		vecInfo.push_back(Info);
	}
	tagInfo InfoTemp;
	for (int i=0;i<lsize;i++)
	{
		for (int j=0;j<i;j++)
		{
			if (bsort)
			{
				if (vecInfo[i].nValue>vecInfo[j].nValue)
				{
					InfoTemp=vecInfo[i];
					vecInfo[i]=vecInfo[j];
					vecInfo[j]=InfoTemp;
				}
			}
			else
			{
				if (vecInfo[i].nValue<vecInfo[j].nValue)
				{
					InfoTemp=vecInfo[i];
					vecInfo[i]=vecInfo[j];
					vecInfo[j]=InfoTemp;
				}
			}
			
		}
	}
	m_listNum.DeleteAllItems();
	itstr=vecInfo.begin();
	char strVal[10];
	for (int i=0;itstr!=vecInfo.end();itstr++,i++)
	{
		m_listNum.InsertItem(i,"");
		m_listNum.SetItemText(i,0,itstr->name.c_str());
		_itoa(itstr->nValue,strVal,10);
		m_listNum.SetItemText(i,1,strVal);
		m_listNum.SetItemData(i,itstr->nValue);
	}
	*pResult = 0;
}
int CALLBACK CUniEditorDlg::Compositor(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl=reinterpret_cast<CListCtrl*>(lParamSort);	
	return 1;
}
void CUniEditorDlg::OnNMClickNumlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_listNum.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	else
	{
		
		CClientDC dc(this);
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(NULL);
		CBitmap bmp;

		if(!bmp.CreateCompatibleBitmap(&dc, rcMap.right, rcMap.bottom))
		{
			PostMessage(0);
		}

		dcMemory.SelectObject(bmp);
		if(bFlag&&!IsLoad(m_uRegionCurr))
		{
			DrawJpg(m_uRegionCurr);
			bFlag=FALSE;
		}


		dcMemory.BitBlt(rcMap.left, rcMap.top,
			rcMap.right, rcMap.bottom,
			&m_dcMemory, hcurrpos, vcurrpos, SRCCOPY);
		string str;
		//获取行
		//int nItem = m_listNum.GetNextSelectedItem(pos);
		//获取指定行和列的数据
		//str=m_listNum.GetItemText(nItem,0);
		int i = m_listNum.GetSelectedCount();
		m_vecMonsterNo.clear();
		m_vecCollectionNo.clear();
		DrawMap();
		vector<long>vecData;
		for (int j = 0;j<i;j++)
		{
			int nItem = m_listNum.GetNextSelectedItem(pos);
			str=m_listNum.GetItemText(nItem,0);
			

			m_listNum.GetVecData(ITEM_OFFSET_MONSTER,str,m_vecMonsterNo);
			//DrawMonsterVector(vecData);
			if (j<1)
			{
				vector<long>vecDisScript=vecData;
				CheckScript(ITEM_OFFSET_MONSTER,&vecDisScript);
			}
			else
				m_ScriptList.DeleteAllItems();

		}


		DrawMonsterFirst(m_vecMonsterNo);
		
	}
}

void CUniEditorDlg::CheckScript(int nType,vector<long>*vecData)
{
	m_ScriptList.DeleteAllItems();
	vector<long>vecRecord;
	vector<long>vecContent;
	vecRecord.clear();

	switch(nType)
	{
	case ITEM_OFFSET_MONSTER:
		{
			vector<long>::iterator it=(*vecData).begin();
			for ( ;it!=(*vecData).end();it++)
			{	
				CMonster* pMonster=NULL;
				VECMONSTERITR itmonster=m_vecMonster.begin();
				for(;itmonster!=m_vecMonster.end();itmonster++)
				{
					if ((*itmonster)->GetNo()==*it)
					{
						pMonster=*itmonster;
						if (pMonster)
						{
							VECMONSTERINFO vecmonsterinfo=pMonster->GetMonsterInfo();
							for (int j=0;j<vecmonsterinfo.size();j++)
							{
								string str=(*vecmonsterinfo[j]).GetScript();
								if ( str== "0")
								{
									vecRecord.push_back(*it);
								}
								else
								{
									vecContent.push_back(*it);
								}
							}
						}
					}	
				}	
			}
			if (vecContent.size()>vecRecord.size())
			{
				for (int m=0;m<vecRecord.size();m++)
				{
					char strVal[128];
					itoa(vecRecord[m],strVal,10);
					m_ScriptList.InsertItem(m,"");
					m_ScriptList.SetItemText(m,0,strVal);
				}
			}
			else
			{
				for (int m=0;m<vecContent.size();m++)
				{
					char strVal[128];
					itoa(vecContent[m],strVal,10);
					m_ScriptList.InsertItem(m,"");
					m_ScriptList.SetItemText(m,0,strVal);
				}
			}
		}
		break;
	case ITEM_OFFSET_NPC:
		{
			
		}
		break;
	}
}
void CUniEditorDlg::OnLvnColumnclickListcoll(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CUniEditorDlg::OnNMClickListcoll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = m_listColl.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	else
	{
		
		CClientDC dc(this);
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(NULL);
		CBitmap bmp;

		if(!bmp.CreateCompatibleBitmap(&dc, rcMap.right, rcMap.bottom))
		{
			PostMessage(0);
		}

		dcMemory.SelectObject(bmp);
		if(bFlag&&!IsLoad(m_uRegionCurr))
		{
			DrawJpg(m_uRegionCurr);
			bFlag=FALSE;
		}


		dcMemory.BitBlt(rcMap.left, rcMap.top,
			rcMap.right, rcMap.bottom,
			&m_dcMemory, hcurrpos, vcurrpos, SRCCOPY);
		string str;
	

		m_vecMonsterNo.clear();
		m_vecCollectionNo.clear();
		DrawMap();
		int i = m_listColl.GetSelectedCount();
		vector<long>vecData;
		for (int j = 0;j<i;j++)
		{
			int nItem = m_listColl.GetNextSelectedItem(pos);
			str=m_listColl.GetItemText(nItem,0);
			m_listColl.GetVecData(ITEM_OFFSET_NPC,str,m_vecCollectionNo);
		}
		DrawCollectionFirst(m_vecCollectionNo);	
	}

}
