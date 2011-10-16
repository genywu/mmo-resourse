// StickQuest.cpp : 实现文件
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "StickQuest.h"


// CStickQuest 对话框

IMPLEMENT_DYNAMIC(CStickQuest, CDialog)

CStickQuest::CStickQuest(CWnd* pParent /*=NULL*/)
	: CDialog(CStickQuest::IDD, pParent)
    , m_QuestID(0)
{

}

CStickQuest::~CStickQuest()
{
}

void CStickQuest::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_QuestID);
	DDV_MinMaxLong(pDX, m_QuestID, 100000, 999999);
}


BEGIN_MESSAGE_MAP(CStickQuest, CDialog)
END_MESSAGE_MAP()


// CStickQuest 消息处理程序
