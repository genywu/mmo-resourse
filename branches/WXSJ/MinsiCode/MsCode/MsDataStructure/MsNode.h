#pragma once        //┃//┃
#ifndef __MSNODE_H__//┃//┃
#define __MSNODE_H__//┃//┃
//━━━━━━━━━━┛//┃
//文件名称：MsNode.h    //┃
//功能描述：节点类      //┃
//平台相关：Windows     //┃
//━━━━━━━━━━━━┛

class CMsNode
{
public:
    CMsNode(VOID);

    BOOL Init(
        CMsNode* lpRootNode,
        CMsNode* lpParentNode,
        CMsNode** lppSubNode,
        LPVOID lpData
        );

    virtual LPVOID ReSet(
        CMsNode* lpParentNode,
        CMsNode** lppSubNode,
        LPVOID lpData
        );

    virtual VOID Release(VOID) = 0;

    LPVOID  GetData(VOID);                      // 获取自己的数据
    LPVOID  GetSubNodeData(DWORD dwIndex);      // 获取下级的数据
    LPVOID  GetParentData(VOID);                // 获取上级的数据

    CMsNode* GetRootNode(VOID);                 // 获取根级的地址
    CMsNode* SetRootNode(CMsNode* lpTreeNode);  // 设置根级的地址

    CMsNode* GetParentNode(VOID);               // 获取上级的地址
    CMsNode* SetParentNode(CMsNode* lpTreeNode);// 设置上级的地址

    CMsNode* GetSubNode(DWORD dwIndex);         // 获取下级的地址
    CMsNode* SetSubNode(                        // 设置下级的地址
        DWORD dwIndex,
        CMsNode* lpTreeNode
        );

    DWORD AddBearLevel(VOID);       // 增加权重级别
    DWORD SubBearLevel(VOID);       // 减少权重级别
    DWORD GetBearLevel(VOID);       // 获取权重级别

protected:
    CMsNode*    m_lpRootNode;       // 根节点指针
    CMsNode*    m_lpParentNode;     // 父节点指针
    CMsNode**   m_lppSubNode;       // 下一级节点数组
    DWORD       m_dwSubNodeCount;   // 下一级节点个数
    LPVOID      m_lpData;           // 数据
    DWORD       m_dwBearLevel;      // 权重级别
    BOOL        m_bInit;            // 是否已经初始化
};
typedef class CMsNode* lpCMsNode;

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
