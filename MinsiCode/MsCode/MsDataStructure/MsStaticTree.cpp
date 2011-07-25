#include "Precompiled.h"
#include "MsNode.h"
#include "MsPoolManager.h"
#include "MsStaticTree.h"

// 构造
CMsStaticTree::CMsStaticTree(DWORD dwMaxSaveCount, DWORD dwTreeDepth)
: m_dwMaxNodeCount      (dwMaxSaveCount*dwTreeDepth)
, m_CMsPoolManager      (dwMaxSaveCount*dwTreeDepth)
, m_dwMaxSaveCount      (dwMaxSaveCount*dwTreeDepth)
, m_dwTreeDepth         (dwTreeDepth)
, m_lpCMsNode           (NULL)
{
    AssertEx(AL_NORMAL, m_dwMaxSaveCount, _T("树中节点个数不能为零"));
    AssertEx(AL_NORMAL, m_dwTreeDepth, _T("树层不能为零"));

    BOOL bRet = this->Init();
    AssertEx(AL_NORMAL, bRet, _T("初始化静态树失败"));
}

// 析构
CMsStaticTree::~CMsStaticTree(VOID)
{
    ;
}

// 初始化
BOOL CMsStaticTree::Init(VOID)
{
    m_lpCMsNode = NEW CMsTreeNode[m_dwMaxNodeCount];
    AssertEx(AL_CATAST, m_lpCMsNode, _T("内存不足"));
    return TRUE;
}

// 释放
BOOL CMsStaticTree::Release(VOID)
{
    SAFE_DELETE_ARRAY(m_lpCMsNode);
    return TRUE;
}

// 生长一个节点出来
lpCMsTreeNode CMsStaticTree::Develop(DWORD dwTreeDepth, lpCMsTreeNode lpParentNode, DWORD dwSubNodeIndex)
{
    lpCMsTreeNode lpNewNode = NULL;
    // 如果是根节点上生长
    if (0 == dwTreeDepth)
    {
        // 如果此节点已经存在
        if (m_lpCMsNode->m_lpSubNode[dwSubNodeIndex])
        {
            return m_lpCMsNode->m_lpSubNode[dwSubNodeIndex];
        }
        else
        {
            // 获取下个可用索引
            DWORD dwIndex = m_CMsPoolManager.GetNextIndex();

            // 记录新节点
            lpNewNode = &m_lpCMsNode[dwIndex];

            // 将上级节点的对应分支与新节点连接
            m_lpCMsNode->m_lpSubNode[dwSubNodeIndex] = lpNewNode;

            // 将池子索引存入新节点
            lpNewNode->m_dwPoolIndex = dwIndex;

            // 将根节点权重增加
            lpNewNode->AddBearLevel();
        }
    }

    // 如果不是根节点
    else
    {
        // 如果此节点已经存在
        if (lpParentNode->m_lpSubNode[dwSubNodeIndex])
        {
            return lpParentNode->m_lpSubNode[dwSubNodeIndex];
        }
        else
        {
            // 获取下个可用索引
            DWORD dwIndex = m_CMsPoolManager.GetNextIndex();

            // 记录新节点
            lpNewNode = &m_lpCMsNode[dwIndex];

            // 将上级节点的对应分支与新节点连接
            m_lpCMsNode->m_lpSubNode[dwSubNodeIndex] = lpNewNode;

            // 设置上级节点
            lpNewNode->SetParentNode(lpParentNode);

            // 将池子索引存入新节点
            lpNewNode->m_dwPoolIndex = dwIndex;

            // 循环将上级节点权重增加
            lpCMsNode lpNode = lpNewNode;
            do 
            {
                lpNode->AddBearLevel();
                lpNode = lpNode->GetParentNode();
            } while (lpNode);
        }
    }
    return lpNewNode;
}

// 剪掉一个节点
BOOL CMsStaticTree::Pruning(lpCMsTreeNode lpParentNode, DWORD dwSubNodeIndex)
{
    lpCMsNode lpSubNodeForDel = lpParentNode->GetSubNode(dwSubNodeIndex);

    if (!lpSubNodeForDel)
    {
        return FALSE;
    }

    if (lpSubNodeForDel->GetBearLevel() > 0)
    {
        return FALSE;
    }

    m_CMsPoolManager.FreeIndexByPool(lpParentNode->m_dwPoolIndex);
    lpSubNodeForDel->ReSet(NULL, NULL, NULL);
    lpParentNode->SetSubNode(dwSubNodeIndex, NULL);
    return TRUE;
}

// 获取使用个数
DWORD CMsStaticTree::GetUseCount(VOID)
{
    return m_CMsPoolManager.GetCurrentCount();
}

// 通过索引直接获取节点
lpCMsTreeNode CMsStaticTree::GetNodeByIndex(DWORD dwIndex)
{
    DWORD dwPoolIndex = m_CMsPoolManager.LocateIndexToPoolIndex(dwIndex);
    return &m_lpCMsNode[dwPoolIndex];
}