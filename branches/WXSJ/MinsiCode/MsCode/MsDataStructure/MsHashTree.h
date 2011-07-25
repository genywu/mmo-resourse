#pragma once            //┃//┃
#ifndef __MSHASHTREE_H__//┃//┃
#define __MSHASHTREE_H__//┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsHashTree.h    //┃
//功能描述：哈希树相关      //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛
//
//// 树杈节点
//class CMsTreeNode
//    : public CPoolMember
//{
//public:
//    friend class CMsTreeIndex;
//    CMsTreeNode(CMsTreeIndex* lpCMsTreeIndex, DWORD dwTreeFurcation, CMsTreeNode* lpFatherTreeFurcation);
//    CMsTreeNode(VOID);
//    ~CMsTreeNode(VOID);
//    VOID         Create(CMsTreeIndex* lpCMsTreeIndex, DWORD dwTreeFurcation, CMsTreeNode* lpFatherTreeFurcation, DWORD dwBeginFlag);
//    VOID         SetFather(CMsTreeNode* lpFatherTreeFurcation);
//    CMsTreeNode* GetFather(VOID){return m_lpFatherTreeFurcation;}
//    VOID         AddUse(VOID){m_dwUseCount++;}
//    VOID         SubUse(VOID){m_dwUseCount--;}
//    BOOL         IsUse(VOID){return m_dwUseCount?TRUE:FALSE;}
//    BOOL         SetData(LPVOID lpData);
//protected:
//private:
//    virtual BOOL SetData(LPVOID lpData, DWORD dwDataSize);
//    BOOL            m_bMustInit;            // 必须进行初始化
//    DWORD           m_dwBeginFlag;          // 起始标记值
//    CMsTreeIndex*   m_lpCMsTreeIndex;       // 树头指针
//    CMsTreeNode**   m_lppTreeFurcation;     // 子树杈指针数组
//    CMsTreeNode*    m_lpFatherTreeFurcation;// 父树杈指针
//    DWORD           m_dwTreeFurcationCount; // 拥有子树杈个数
//    DWORD           m_dwUseCount;           // 本节点权重(被使用多少次)
//};typedef CMsTreeNode* lpCMsTreeNode;
//
//// 树头索引
//class CMsTreeIndex
//{
//public:
//    enum E_ERROR_CODE
//    {
//        ERROR_UNKNOW = -1,          // 未知错误
//        ERROR_NO_ERROR = 0,         // 无错误
//        ERROR_PARAMETER_OVERFLOW,   // 参数溢出
//        ERROR_TREE_FULL,            // 树已满
//        ERROR_DATA_IS_EXIST,        // 数据已存在
//        ERROR_DATA_NOT_EXIST,       // 数据不存在
//    };
//    CMsTreeIndex(DWORD dwTreeFurcation, DWORD dwMaxNodeCount, DWORD dwBeginFlag);
//    ~CMsTreeIndex(VOID);
//    BOOL    AddData(LPBYTE lpByte, DWORD dwSize, LPVOID lpData);// 添加元素
//    LPVOID  GetData(LPBYTE lpByte, DWORD dwSize);               // 获取元素
//    BOOL    DelData(LPBYTE lpByte, DWORD dwSize);               // 删除元素
//    E_ERROR_CODE    GetLastErrorCMsTreeIndex(VOID);             // 获取错误代码
//private:
//    DWORD           m_dwTreeFurcation;  // 每个节点的分杈个数
//    DWORD           m_dwMaxNodeCount;   // 整棵树最多节点个数
//    CMsTreeNode*    m_lpTreeFurcation;  // 树杈节点数组
//    DWORD           m_dwBeginFlag;      // 起始标记值
//    CMsPoolManager  m_CMsNodePool;      // 节点池子
//    CMsPoolManager  m_CMsDataPool;      // 数据池子
//    CMsPoolManager  m_CMsFurcationPool; // 树杈池子
//    E_ERROR_CODE    m_eTreeErrorCode;   // 树错误代码
//};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
