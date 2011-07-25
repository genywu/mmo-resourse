#include "Precompiled.h"

CBaseNode::CBaseNode(CBaseNode* lpPreviousNode, CBaseNode* lpNextNode)
: m_lpNextNode      (lpNextNode)
, m_lpPreviousNode  (lpPreviousNode)
, m_bOpen           (FALSE)
, m_dwWidth         (0)
, m_dwHight         (0)
, m_dwIndent        (0)
, m_dwBetween       (0)
, m_eNodeType       (INVALID_UID)
, m_dwNodeId        (INVALID_UID)
{
    if (m_lpNextNode && m_lpPreviousNode)
    {
        if (m_lpPreviousNode != HEAD_NODE)
        {
            lpPreviousNode->m_lpNextNode = this;
        }
        if (m_lpNextNode != TAIL_NODE)
        {
            lpNextNode->m_lpPreviousNode = this;
        }
    }
}

CBaseNode::~CBaseNode(VOID)
{
    this->CutMe();
}

// 打开节点
VOID CBaseNode::Open(BOOL bPrint)
{
    m_bOpen = TRUE;
    if (bPrint)
    {
        ;
    }
}

// 关闭节点
VOID CBaseNode::Close(BOOL bPrint)
{
    m_bOpen = FALSE;
    if (bPrint)
    {
        ;
    }
}

// 销毁节点
VOID CBaseNode::Destroy(VOID)
{
    this->Close(TRUE);
}

// 从链表中断开自己
VOID CBaseNode::CutMe(VOID)
{
    if (m_lpNextNode && m_lpPreviousNode)
    {
        if (m_lpPreviousNode == HEAD_NODE)
        {
            if (m_lpNextNode != TAIL_NODE)
            {
                m_lpNextNode->m_lpPreviousNode = HEAD_NODE;
            }
            m_lpPreviousNode = NULL;
            m_lpNextNode = NULL;
        }
        else if (m_lpNextNode == TAIL_NODE)
        {
            if (m_lpPreviousNode != HEAD_NODE)
            {
                m_lpPreviousNode->m_lpNextNode = TAIL_NODE;
            }
            m_lpPreviousNode = NULL;
            m_lpNextNode = NULL;
        }
        else
        {
            m_lpNextNode->m_lpPreviousNode = m_lpPreviousNode;
            m_lpPreviousNode->m_lpNextNode = m_lpNextNode;
            m_lpPreviousNode = NULL;
            m_lpNextNode = NULL;
        }
    }
}

// 节点位置
LPRECT CBaseNode::GetRect(VOID)
{
    return &m_Rect;
}

BOOL CBaseNode::IsOpen(VOID)
{
    return m_bOpen;
}

// 获得节点Id
DWORD CBaseNode::GetNodeId(VOID)
{
    return m_dwNodeId;
}

// 获取下一个节点
CBaseNode* CBaseNode::GetNextNode(VOID)
{
    return m_lpNextNode;
}

// 获取前一个节点
CBaseNode* CBaseNode::GetPreviousNode(VOID)
{
    return m_lpPreviousNode;
}