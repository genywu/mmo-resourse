#include "Precompiled.h"
#include "MsNode.h"

CMsNode::CMsNode(VOID)
: m_lpRootNode      (NULL)
, m_lpParentNode    (NULL)
, m_lppSubNode      (NULL)
, m_lpData          (NULL)
, m_bInit           (FALSE)
, m_dwBearLevel     (INVALID_UID)
{
    _ENTER_FUN_C;

    _LEAVE_FUN_DISPOSE_END_C;
}

BOOL CMsNode::Init(
    CMsNode* lpRootNode,
    CMsNode* lpParentNode,
    CMsNode** lppSubNode,
    LPVOID lpData
    )
{
    m_lpRootNode        = lpRootNode;
    m_lpParentNode      = lpParentNode;
    m_lpData            = lpData;
    m_dwBearLevel       = 0;

    memcpy(
        m_lppSubNode,
        lppSubNode,
        sizeof(lpCMsNode) * m_dwSubNodeCount
        );

    m_bInit = TRUE;

    return TRUE;
}

LPVOID CMsNode::ReSet(
    CMsNode* lpParentNode,
    CMsNode** lppSubNode,
    LPVOID lpData
    )
{
    if (!m_bInit)
    {
        return NULL;
    }

    m_dwBearLevel = 0;

    LPVOID lpOldData = m_lpData;

    m_lpParentNode  = lpParentNode;
    m_lpData        = lpData;
    if (lppSubNode)
    {
        memcpy(
            m_lppSubNode,
            lppSubNode,
            sizeof(lpCMsNode) * m_dwSubNodeCount
            );
    }
    else
    {
        ZeroMemory(m_lppSubNode, sizeof(CMsNode**) * m_dwSubNodeCount);
    }

    return lpOldData;
}

// 获取自己的数据
LPVOID CMsNode::GetData(VOID)
{
    return m_lpData;
}

// 获取子级的数据
LPVOID CMsNode::GetSubNodeData(DWORD dwIndex)
{
    // 如果当前节点无子级
    if (!m_lppSubNode)
    {
        return NULL;
    }

    // 如果访问的子节点不存在
    if (dwIndex >= m_dwSubNodeCount)
    {
        return NULL;
    }

    return m_lppSubNode[dwIndex]->GetData();
}

// 获取上级的数据
LPVOID CMsNode::GetParentData(VOID)
{
    // 如果当前节点无上级
    if (!m_lpParentNode)
    {
        return NULL;
    }

    return m_lpParentNode->GetData();
}

// 获取子级的地址
lpCMsNode CMsNode::GetSubNode(DWORD dwIndex)
{
    // 如果当前节点无子级
    if (!m_lppSubNode)
    {
        return NULL;
    }

    // 如果访问的子节点不存在
    if (dwIndex >= m_dwSubNodeCount)
    {
        return NULL;
    }

    return m_lppSubNode[dwIndex];
}

// 获取上级的地址
lpCMsNode CMsNode::GetParentNode(VOID)
{
    return m_lpParentNode;
}

// 设置子级的地址
lpCMsNode CMsNode::SetSubNode(DWORD dwIndex, lpCMsNode lpTreeNode)
{
    // 如果当前节点无子级
    if (!m_lppSubNode)
    {
        return NULL;
    }

    // 如果访问的子节点不存在
    if (dwIndex >= m_dwSubNodeCount)
    {
        return NULL;
    }

    lpCMsNode pCMsTreeNode_Temp = m_lppSubNode[dwIndex];
    m_lppSubNode[dwIndex] = lpTreeNode;
    return pCMsTreeNode_Temp;
}

// 设置上级的地址
lpCMsNode CMsNode::SetParentNode(lpCMsNode lpTreeNode)
{
    lpCMsNode pCMsTreeNode_Temp = m_lpParentNode;
    m_lpParentNode = lpTreeNode;
    return pCMsTreeNode_Temp;
}

// 获取根级的地址
lpCMsNode CMsNode::GetRootNode(VOID)
{
    return m_lpRootNode;
}

// 设置根级的地址
lpCMsNode CMsNode::SetRootNode(lpCMsNode lpTreeNode)
{
    lpCMsNode pCMsTreeNode_Temp = m_lpRootNode;
    m_lpRootNode = lpTreeNode;
    return pCMsTreeNode_Temp;
}

// 增加权重级别
DWORD CMsNode::AddBearLevel(VOID)
{
    return m_dwBearLevel++;
}


// 减少权重级别
DWORD CMsNode::SubBearLevel(VOID)
{
    return m_dwBearLevel--;
}

// 获取权重级别
DWORD CMsNode::GetBearLevel(VOID)
{
    return m_dwBearLevel;
}
