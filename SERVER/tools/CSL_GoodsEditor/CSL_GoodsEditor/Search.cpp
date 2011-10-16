/// 用于搜索物品
/// Add by 肖杨 2010.5.21

// Search.cpp : 实现文件
//

#include "stdafx.h"
#include "CSL_GoodsEditor.h"
#include "Search.h"
#include "MainFrm.h"
#include "GoodAttrPage.h"
#include "ContainerPage.h"

extern vector<tagGoodAttr> vecGoodAttr; 

// Search 对话框
IMPLEMENT_DYNAMIC(Search, CDialog)

Search::Search(CWnd* pParent):CDialog(Search::IDD, pParent)
{
	m_preSerResult = -1;
}

Search::~Search()
{
	m_preSerResult = -1;
}

void Search::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCHOPTION, m_choseType);
	DDX_Control(pDX, IDC_SEARCHCONTENT, m_SerContent);
}


BEGIN_MESSAGE_MAP(Search, CDialog)
	ON_CBN_SELCHANGE(IDC_SEARCHOPTION, &Search::OnCbnSelchangeSearchoption)
	ON_EN_KILLFOCUS(IDC_SEARCHCONTENT, &Search::OnEnKillfocusSearchcontent)
	ON_BN_CLICKED(IDCANCEL, &Search::OnBnClickedCancel)
	ON_BN_CLICKED(ID_NEXT, &Search::OnBnClickedNext)
END_MESSAGE_MAP()


// Search 消息处理程序

void Search::OnCbnSelchangeSearchoption()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_choseType.GetCurSel();
	//switch(index)
	//{
	//case ST_ID:
	//	break;
	//case ST_ORINAME:
	//	break;
	//case ST_SHOWNAME:
	//    break;
	//default:
	//    break;
	//}
}

//void Search::OnEnChangeSearchcontent()
//{
//	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
//	// 发送该通知，除非重写 CDialog::OnInitDialog()
//	// 函数并调用 CRichEditCtrl().SetEventMask()，
//	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//	// TODO:  在此添加控件通知处理程序代码
//}

void Search::OnEnKillfocusSearchcontent()
{
	// TODO: 在此添加控件通知处理程序代码
}

//void Search::OnBnClickedOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	//OnOK();
//}

void Search::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_preSerResult = -1;
	OnCancel();
}

// 检测字符窜是否是数字
bool Search::CheckStrIsNum(const char* str,DWORD len)
{
	if(str==NULL) return false;
	char* _tstr = new char[len+1];
	strcpy_s(_tstr,len+1,str);
	_tstr[len] = '\0';
	DWORD count = 0;
	while (count<len)
	{
		if (_tstr[count]<=57&&_tstr[count]>=48) 
			count++;
		else
		{	
			delete _tstr;
			return false;
		}
	}
	delete _tstr;
	return true;
}

void Search::OnBnClickedNext()
{
	// TODO: 在此添加控件通知处理程序代码
	char szContent[128] = "";
	LONG val = 0;
	int nSelected=0;
	if(m_SerContent.GetWindowText(szContent,128)==0) return;
	vector<tagGoodAttr>::iterator iter = vecGoodAttr.begin();
	switch(m_choseType.GetCurSel())
	{
		//按物品ID值搜索
	case 0:
		if (!CheckStrIsNum(szContent,(DWORD)strlen(szContent)))	//检测编辑框中的内容是否是数字
		{
			MessageBox("按物品ID搜索时请输入数字！","提示",MB_OK);
			return;
		}
		val = atol(szContent);
		for(;iter!=vecGoodAttr.end();iter++,nSelected++)
		{			
			if(iter->dwIndex==val)
				break;
		}
		break;
		//按物品原始名搜索
	case 1:
		for(;iter!=vecGoodAttr.end();iter++,nSelected++)
		{			
			if(strcmp(iter->strOrigName.c_str(),szContent)==0
				&&nSelected>m_preSerResult)
			break;
		}
		break;
		//按物品显示名搜索
	case 2:
		for(;iter!=vecGoodAttr.end();iter++,nSelected++)
		{			
			if(strcmp(iter->strName.c_str(),szContent)==0
				&&nSelected>m_preSerResult)
				break;
		}
	    break;
	default:
		MessageBox("没有此种搜索方式！","错误",MB_OK);
		OnCancel();
		return;
	}

	if (iter==vecGoodAttr.end()&&m_preSerResult==-1)
	{
		MessageBox("没有找到您搜索的物品！","提示",MB_OK);
		return;
	}

	if (m_preSerResult!=nSelected&&iter!=vecGoodAttr.end())
		m_preSerResult = nSelected;
	else
	{
		m_preSerResult = -1;
		MessageBox("搜不到新东西啦！","提示",MB_OK);
		return;
	}

	/// 根据选择的搜索方式进行搜索
	CSplitterWnd& pSplitter = ((CMainFrame*)GetParent())->GetSplitterWnd();
	CContainerPage *pContainer=(CContainerPage*)pSplitter.GetPane(0,1);
	CAllGoodsTree *pGoodsTree=(CAllGoodsTree*)pSplitter.GetPane(0,0);

	pContainer->m_wndGoodAttrPage.SetEditFlag(false);
	pContainer->m_wndGoodAttrPage.ClearCtrls();
	pContainer->m_wndGoodAttrPage.FillCtrls(m_preSerResult);		
	pContainer->m_wndAllGoodsPage.ShowWindow(SW_HIDE);
	pContainer->m_wndGoodAttrPage.ShowWindow(SW_NORMAL);		
	pContainer->m_wndGoodAttrPage.SetEditFlag(true);
}
