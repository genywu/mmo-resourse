#include "Precompiled.h"
#include "Node.h"

CNode::CNode(VOID)
:m_bIsOpen(false)
,m_dwChildCurNum(0)
{
    m_pChildList.clear();
}

CNode::~CNode(VOID)
{
    Realese();
}

BOOL CNode::Init(RECT PaterRect,DWORD dwNodeId, BOOL bIsRoot)
{
    m_rect.left = PaterRect.left + NODEWIDTHINDENT;
    m_rect.top = PaterRect.bottom;
    m_rect.right = m_rect.left + NODEWIDTH;
    m_rect.bottom = m_rect.top + NODEHIGHT;
    m_dwNodeId = dwNodeId;
    if (bIsRoot)
    {
    }else
    {
       
    }
   
    return TRUE;
}

VOID CNode::Realese()
{
    for(DWORD i = 0; i < m_dwChildCurNum; i++)
    {
        SAFE_DELETE(m_pChildList[i]);
    }
    m_pChildList.clear();
}

VOID CNode::AddNode(DWORD dwNewNodeId)
{
    m_dwChildCurNum++;
    CNode* TempNode = NEW CNode;
    TempNode->Init(m_rect, dwNewNodeId);
    m_pChildList.push_back(TempNode);

    m_rect.bottom += NODEHIGHT;
}

VOID CNode::OpenNode(BOOL bOpen)
{
    m_bIsOpen = bOpen;
    
    if (m_bIsOpen)
    {
        if (!m_dwChildCurNum)
        {
            //AddNode();
            //AddNode();
            //AddNode();
            //AddNode();
            //AddNode();
            //请求服务器打开
        }
    }
}

VOID CNode::Render(HDC hDC)
{
    
    HBRUSH Brush = CreateSolidBrush(RGB(255,0,255));
    RECT re = {m_rect.left, m_rect.top, m_rect.left+NODEWIDTH, m_rect.top + NODEHIGHT};
    FillRect(hDC, &re, Brush);
    DeleteObject(Brush);

    if (m_bIsOpen)
    {
        for (UINT i = 0; i < m_pChildList.size(); i++)
        {
            m_pChildList[i]->Render(hDC);
        }
    }
}