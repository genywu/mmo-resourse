// DlgAddNode.cpp : 实现文件
//

#include "stdafx.h"
#include "UniEditor.h"
#include "DlgAddNode.h"
#include "RegionList.h"
#include "UniEditorDlg.h"

// CDlgAddNode 对话框
#pragma warning (disable : 4244)

IMPLEMENT_DYNAMIC(CDlgAddNode, CDialog)

CDlgAddNode::CDlgAddNode(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddNode::IDD, pParent)
	, m_uRadio(0), m_pWnd(NULL), m_pRegionTree(NULL), 	m_pMonster(NULL)
{
}

CDlgAddNode::~CDlgAddNode()
{
}

void CDlgAddNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTREGION, m_listRegion);
	DDX_Control(pDX, IDC_LISTREGIONADD, m_listRegionAdd);
	DDX_Control(pDX, IDC_LISTMONS, m_listMonster);
	DDX_Control(pDX, IDC_LISTMONSADD, m_listMonsterAdd);
	DDX_Radio(pDX, IDC_RADIOMONSTER, m_uRadio);
}


BEGIN_MESSAGE_MAP(CDlgAddNode, CDialog)
	ON_LBN_DBLCLK(IDC_LISTREGION, &CDlgAddNode::OnLbnDblclkListregion)
	ON_LBN_DBLCLK(IDC_LISTREGIONADD, &CDlgAddNode::OnLbnDblclkListregionadd)
	ON_BN_CLICKED(IDC_BUTADD, &CDlgAddNode::OnBnClickedButadd)
	ON_BN_CLICKED(IDC_BUTDEL, &CDlgAddNode::OnBnClickedButdel)
	ON_LBN_DBLCLK(IDC_LISTMONS, &CDlgAddNode::OnLbnDblclkListmonster)
	ON_LBN_DBLCLK(IDC_LISTMONSADD, &CDlgAddNode::OnLbnDblclkListmonsteradd)
	ON_BN_CLICKED(IDC_BUTADDM, &CDlgAddNode::OnBnClickedButaddM)
	ON_BN_CLICKED(IDC_BUTDELM, &CDlgAddNode::OnBnClickedButdelM)
END_MESSAGE_MAP()


// CDlgAddNode 消息处理程序


//将光标移动到“*”之后
int ReadTo2(ifstream &stream,string name)
{
		string str;
	stream >> str;
	while (str != name)
	{
		//如果已到文件末尾，返回0
		if (stream.eof())
		{

			return 0;
		}
		//读取下一个串
		stream >> str;
	}
	return 1;
}
BOOL CDlgAddNode::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pWnd = (CUniEditorDlg *)GetParent();
	m_pRegionTree = (CRegionTree *)(((CUniEditorDlg *)m_pWnd)->GetDlgItem(IDT_REGION));
	m_RegionList = m_pRegionTree->GetRegionList();
	HTREEITEM hItem;

	hItem = m_pRegionTree->GetRootItem();
	m_hItemFirstRegion = hItem = m_pRegionTree->GetChildItem(hItem);

	while(hItem != NULL)
	{
		CString szRegion = m_pRegionTree->GetItemText(hItem);
		m_listRegion.AddString(szRegion);
		hItem = m_pRegionTree->GetNextSiblingItem(hItem);
	}
	int i=0;
	// 初始化MONSTER列表
	ifstream ifStream;
	string strTemp;
	string str;
	ifStream.open(strFileMonster, ios_base::in);
	while(ReadTo2(ifStream,"*")!=0)
	{
		// 获取Monster名称并加入列表
		ifStream >> strTemp >> strTemp>>strTemp;
		
	//	ifStream>>str;
		if(i>START)
		{
			m_listMonster.AddString(strTemp.c_str());	
		}
		i++;
	}
	ifStream.close();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgAddNode::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();

	// 如果添加列表非空
	HTREEITEM hItem = m_hItemFirstRegion;
	HTREEITEM hItemAdd;
	int iListNodeNum = m_listRegionAdd.GetCount();
	int iMatchNum = 0;
	CString szRegionAdd;
	CString szRegionItem;
	ITRTREE itrTree;
	unsigned uRegionNo;
	MAPTREE	&m_mapTree = m_pRegionTree->GetMapTree();

	while(hItem != NULL && iMatchNum < iListNodeNum)
	{
		szRegionItem = m_pRegionTree->GetItemText(hItem);
		m_listRegionAdd.GetText(iMatchNum, szRegionAdd);

		// 场景名称匹配
		if(szRegionAdd == szRegionItem)
		{
			++iMatchNum;
			uRegionNo = m_pRegionTree->GetItemData(hItem);
			itrTree = m_mapTree.find(uRegionNo);

			// 第一次读取场景文件
			if(uRegionNo > ITEM_REGION_UNEXISTED && itrTree == m_mapTree.end())
			{
				m_pRegionTree->ReadData(hItem);
				uRegionNo = m_pRegionTree->GetItemData(hItem);
			}

			// 场景存在
			if (uRegionNo > ITEM_REGION_UNEXISTED)
			{
				CRegion *pRegion = m_mapTree[uRegionNo];
				m_pRegionTree->SetItemBold(hItem, TRUE);

				switch(m_uRadio)
				{
					// 添加MONSTER
				case RADIO_MONSTER:
					{
						CMonster *pMonster = new CMonster(pRegion->Width(), pRegion->Height());
						int iListMonsterNum = m_listMonsterAdd.GetCount();
						for(int i=0; i<iListMonsterNum; ++i)
						{
							CString szMonsterAdd;
							m_listMonsterAdd.GetText(i, szMonsterAdd);
							CMonsterInfo *pMonInfo = new CMonsterInfo;
							pMonInfo->SetName(szMonsterAdd.GetString());
							pMonster->AddMonsterInfo(pMonInfo);
						}

						hItemAdd = m_pRegionTree->GetChildItem(hItem);
						m_pRegionTree->AddItemMonster(hItemAdd, pRegion, pMonster);

						break;
					}

					// 添加NPC
				case RADIO_NPC:
					{
						break;
					}

					// 添加MONSTERINFO
				case RADIO_MONINFO:
					{
						break;
					}

				default:
					break;
				}
			}
		}
		hItem = m_pRegionTree->GetNextSiblingItem(hItem);
	}

	CDialog::OnOK();
}

void CDlgAddNode::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void CDlgAddNode::OnLbnDblclkListregion()
{
	// TODO: 在此添加控件通知处理程序代码
	int ind = m_listRegion.GetCurSel();
	CString str;

	m_listRegion.GetText(ind, str);
	m_listRegionAdd.AddString(str);
	m_listRegion.DeleteString(ind);
}

void CDlgAddNode::OnLbnDblclkListregionadd()
{
	// TODO: 在此添加控件通知处理程序代码
	int ind = m_listRegionAdd.GetCurSel();
	CString str;

	m_listRegionAdd.GetText(ind, str);
	m_listRegion.AddString(str);
	m_listRegionAdd.DeleteString(ind);
}

void CDlgAddNode::OnBnClickedButadd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	for(int i=0; i<m_listRegion.GetCount(); ++i)
	{
		m_listRegion.GetText(i, str);
		m_listRegionAdd.AddString(str);
	}

	m_listRegion.ResetContent();
}

void CDlgAddNode::OnBnClickedButdel()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	for(int i=0; i<m_listRegionAdd.GetCount(); ++i)
	{
		m_listRegionAdd.GetText(i, str);
		m_listRegion.AddString(str);
	}

	m_listRegionAdd.ResetContent();
}

void CDlgAddNode::OnLbnDblclkListmonster()
{
	// TODO: 在此添加控件通知处理程序代码
	int ind = m_listMonster.GetCurSel();
	CString str;

	m_listMonster.GetText(ind, str);
	m_listMonsterAdd.AddString(str);
	m_listMonster.DeleteString(ind);
}

void CDlgAddNode::OnLbnDblclkListmonsteradd()
{
	// TODO: 在此添加控件通知处理程序代码
	int ind = m_listMonsterAdd.GetCurSel();
	CString str;

	m_listMonsterAdd.GetText(ind, str);
	m_listMonster.AddString(str);
	m_listMonsterAdd.DeleteString(ind);
}

void CDlgAddNode::OnBnClickedButaddM()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	for(int i=0; i<m_listMonster.GetCount(); ++i)
	{
		m_listMonster.GetText(i, str);
		m_listMonsterAdd.AddString(str);
	}

	m_listMonster.ResetContent();
}

void CDlgAddNode::OnBnClickedButdelM()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	for(int i=0; i<m_listMonsterAdd.GetCount(); ++i)
	{
		m_listMonsterAdd.GetText(i, str);
		m_listMonster.AddString(str);
	}

	m_listMonsterAdd.ResetContent();
}
BOOL CDlgAddNode::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{
	case WM_CLOSE:
		MessageBox(_T("关闭DLGADDNODE"));
		return FALSE;

	case WM_MOUSEMOVE:  
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
