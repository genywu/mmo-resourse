#pragma once                //┃//┃
#ifndef __MSSTATICTREE_H__  //┃//┃
#define __MSSTATICTREE_H__  //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsStaticTree.h      //┃
//功能描述：静态树              //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 树每个节点分支最大个数
#define MAX_EMBRANCHMENT_COUNT 256

class CMsTreeNode
    : public CMsNode
{
public:
    CMsTreeNode(VOID)
        : m_dwPoolIndex (INVALID_UID)
    {
        CMsNode::m_lppSubNode = (CMsNode**)m_lpSubNode;
        ZeroMemoryArray(m_lpSubNode);
    }

    CMsTreeNode(
        lpCMsNode lpRootNode,
        lpCMsNode lpParentNode,
        lpCMsNode* lppSubNode,
        LPVOID lpData
        )
        : m_dwPoolIndex (INVALID_UID)
    {
        _ENTER_FUN_C;

        CMsNode::m_bInit = FALSE;
        CMsNode::m_lppSubNode = (CMsNode**)m_lpSubNode;
        CMsNode::m_dwSubNodeCount = MAX_EMBRANCHMENT_COUNT;
        BOOL bRet = this->Init(
            lpRootNode,
            lpParentNode,
            lppSubNode,
            lpData
            );
        AssertEx(AL_SHOW_ONLY, bRet, _T("初始化节点失败"));
        ZeroMemoryArray(m_lpSubNode);

        _LEAVE_FUN_DISPOSE_END_C;
    }

    virtual VOID Release(VOID)
    {
        CMsNode::m_bInit = FALSE;
        m_dwPoolIndex = INVALID_UID;
        ZeroMemoryArray(m_lpSubNode);
    }

    virtual LPVOID ReSet(
        CMsNode*,
        CMsNode**,
        LPVOID)
    {
        m_dwPoolIndex = INVALID_UID;
        return CMsNode::ReSet(NULL, NULL, NULL);
    }

    CMsTreeNode*    m_lpSubNode[MAX_EMBRANCHMENT_COUNT];    // 下一级节点数组
    DWORD           m_dwPoolIndex;  // 池子中的索引
    //DWORD           m_dwTierIndex;  // 层数组中索引
};
typedef class CMsTreeNode* lpCMsTreeNode;
typedef class CMsTreeNode** lppCMsTreeNode;


class CMsStaticTree
{
public:
    CMsStaticTree(DWORD dwMaxSaveCount, DWORD dwTreeDepth); // 构造
    ~CMsStaticTree(VOID);   // 析构

    VOID operator = (CMsStaticTree){AssertEx(AL_NORMAL, FALSE, _T("禁用赋值运算符"));}
    BOOL Init(VOID);        // 初始化
    BOOL Release(VOID);     // 释放

    // 生长一个节点出来
    lpCMsTreeNode Develop(
        DWORD dwTreeDepth,
        lpCMsTreeNode lpParentNode,
        DWORD dwSubNodeIndex
        );

    // 剪掉一个节点
    BOOL Pruning(lpCMsTreeNode lpParentNode, DWORD dwSubNodeIndex);

    // 获取使用个数
    DWORD GetUseCount(VOID);

    // 通过索引直接获取节点
    /* 如果用此函数遍历节点, 请保证在遍历期间不会调用任何其他此类的成员函数 */
    lpCMsTreeNode GetNodeByIndex(DWORD dwIndex);
private:

    lpCMsTreeNode   m_lpCMsNode;        // 节点数组
    CONST DWORD     m_dwMaxSaveCount;   // 最大储存数量
    CONST DWORD     m_dwMaxNodeCount;   // 最大节点个数
    CONST DWORD     m_dwTreeDepth;      // 树的深度数量
    CMsPoolManager  m_CMsPoolManager;   // 静态树节点索引池
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
