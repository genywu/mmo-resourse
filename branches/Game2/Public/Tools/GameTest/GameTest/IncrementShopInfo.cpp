// IncrementShopInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "GameTest.h"
#include "IncrementShopInfo.h"
#include ".\incrementshopinfo.h"
#include "..\App\Goods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CIncrementShopInfo 对话框

IMPLEMENT_DYNAMIC(CIncrementShopInfo, CDialog)
CIncrementShopInfo::CIncrementShopInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CIncrementShopInfo::IDD, pParent)
{
}

CIncrementShopInfo::~CIncrementShopInfo()
{
}

void CIncrementShopInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IncrementShopGoodsLIST, m_IncreShopGoodsList);
}


BEGIN_MESSAGE_MAP(CIncrementShopInfo, CDialog)
END_MESSAGE_MAP()


// CIncrementShopInfo 消息处理程序
void CIncrementShopInfo::SetDisplayIncrementShop()
{
	vector<tagShopGoods>::iterator it = GetShopGoods().begin();
	int i =0;
	char str[16];
	for(;it!=GetShopGoods().end();++it)
	{
		m_IncreShopGoodsList.InsertItem(i,it->strGoodsName.c_str());
		sprintf("%d",str,it->dwNum);
		m_IncreShopGoodsList.SetItemText(i,1,str);
		sprintf(str,"%d",it->dwPrice);
		m_IncreShopGoodsList.SetItemText(i,2,str);
	}
}
BOOL CIncrementShopInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int i =0 ;
	m_IncreShopGoodsList.InsertColumn(i++,"名字",10,80,10);
	m_IncreShopGoodsList.InsertColumn(i++,"数量",10,80,10);
	m_IncreShopGoodsList.InsertColumn(i++,"价格",10,80,10);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
