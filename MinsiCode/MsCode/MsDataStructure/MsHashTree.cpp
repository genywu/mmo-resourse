#include "Precompiled.h"
#include "MsPoolManager.h"
#include "MsHashTree.h"
int g_no_use_MsHashTree_cpp_disable_warning_LNK4221 = 0;
//
//// 树杈节点
//CMsTreeNode::CMsTreeNode(VOID)
//: m_bMustInit               (TRUE)
//, m_lppTreeFurcation        (NULL)
//, m_dwUseCount              (0)
//, m_lpFatherTreeFurcation   (NULL)
//{
//    ;
//}
//
//CMsTreeNode::CMsTreeNode(CMsTreeIndex* lpCMsTreeIndex, DWORD dwTreeFurcation, CMsTreeNode* lpFatherTreeFurcation)
//: m_bMustInit               (TRUE)
//, m_lppTreeFurcation        (NULL)
//, m_lpFatherTreeFurcation   (NULL)
//{
//    //this->Create(lpCMsTreeIndex, dwTreeFurcation, lpFatherTreeFurcation);
//}
//
//CMsTreeNode::~CMsTreeNode(VOID)
//{
//    SAFE_DELETE_ARRAY(m_lppTreeFurcation);
//}
//
//VOID CMsTreeNode::Create(CMsTreeIndex* lpCMsTreeIndex, DWORD dwTreeFurcation, CMsTreeNode* lpFatherTreeFurcation)
//{
//    _ENTER_FUN;
//
//    m_dwTreeFurcationCount  = dwTreeFurcation;
//    m_lpCMsTreeIndex        = lpCMsTreeIndex;
//    m_lpFatherTreeFurcation = lpFatherTreeFurcation;
//    m_lppTreeFurcation = NEW lpCMsTreeNode[m_dwTreeFurcationCount];
//    AssertEx(m_lppTreeFurcation, _T("m_lppTreeFurcation为空,内存不足"));
//    ZeroMemory(m_lppTreeFurcation, sizeof(lpCMsTreeNode) * m_dwTreeFurcationCount);
//    m_bMustInit = FALSE;
//
//    _LEAVE_FUN_DISPOSE_CONTINUE;
//}
//
//VOID CMsTreeNode::SetFather(CMsTreeNode* lpFatherTreeFurcation)
//{
//    m_lpFatherTreeFurcation = lpFatherTreeFurcation;
//}
//
//BOOL CMsTreeNode::SetData(LPVOID lpData)
//{
//    return this->SetData(lpData, sizeof(lpData));
//}
//
//BOOL CMsTreeNode::SetData(LPVOID lpData, DWORD dwDataSize)
//{
//    m_lpData = lpData;
//    return TRUE;
//}
//
///////////////////////////////////////////////
///////////////////////////////////////////////
//
//
//// 树头索引
//CMsTreeIndex::CMsTreeIndex(DWORD dwTreeFurcation, DWORD dwFurcationMaxCount)
//: m_dwTreeFurcation (dwTreeFurcation)
//, m_dwMaxNodeCount  (dwFurcationMaxCount)
//, m_CMsPoolManager  (dwFurcationMaxCount)
//, m_eTreeErrorCode  (ERROR_NO_ERROR)
//{
//    m_lpTreeFurcation = NEW CMsTreeNode[m_dwMaxNodeCount];
//    AssertEx(m_lpTreeFurcation, _T("m_lpTreeFurcation为空,内存不足"));
//    for (DWORD i = 0; i < m_dwMaxNodeCount; i++)
//    {
//        m_lpTreeFurcation[i].Create(this, m_dwTreeFurcation, NULL);
//    }
//    m_CMsPoolManager.Init();
//}
//
//CMsTreeIndex::~CMsTreeIndex(VOID)
//{
//    m_CMsPoolManager.Release();
//    SAFE_DELETE_ARRAY(m_lpTreeFurcation);
//}
//
//// 添加元素
//BOOL CMsTreeIndex::AddData(LPBYTE lpByte, DWORD dwSize, LPVOID lpData)
//{
//    CMsTreeNode* lpTreeFurcation = m_lpTreeFurcation;
//    for (DWORD i = 0; i < dwSize; i++)
//    {
//        if (lpByte[i] > m_dwMaxNodeCount)
//        {
//            m_eTreeErrorCode = ERROR_PARAMETER_OVERFLOW;
//            return FALSE;
//        }
//        CMsTreeNode* lpFurcation = lpTreeFurcation->m_lppTreeFurcation[lpByte[i]];
//        if (lpFurcation && lpFurcation->IsUse())
//        {
//            lpTreeFurcation = lpFurcation;
//            if ((i+1) == dwSize)
//            {
//                m_eTreeErrorCode = ERROR_DATA_IS_EXIST;
//                return FALSE;
//            }
//        }
//        else
//        {
//            CMsTreeNode Furcation;
//            if (m_CMsPoolManager.Append(&Furcation))
//            {
//                lpFurcation = &m_lpTreeFurcation[Furcation.GetIndex()];
//                Furcation.CopyMember(lpFurcation);
//                lpFurcation->SetFather(lpTreeFurcation);
//                lpTreeFurcation = lpFurcation;
//            }
//            else
//            {
//                m_eTreeErrorCode = ERROR_TREE_FULL;
//                return FALSE;
//            }
//        }
//    }
//    CMsTreeNode* lpFurcation = lpTreeFurcation->GetFather();
//    while (lpFurcation)
//    {
//        lpFurcation->AddUse();
//        lpFurcation = lpFurcation->GetFather();
//    }
//    return lpTreeFurcation->SetData(lpData);
//}
//
//// 获取元素
//LPVOID CMsTreeIndex::GetData(LPBYTE lpByte, DWORD dwSize)
//{
//    CMsTreeNode* lpTreeFurcation = m_lpTreeFurcation;
//    for (DWORD i = 0; i < dwSize; i++)
//    {
//        if (lpByte[i] > m_dwMaxNodeCount)
//        {
//            m_eTreeErrorCode = ERROR_PARAMETER_OVERFLOW;
//            return FALSE;
//        }
//        CMsTreeNode* lpFurcation = lpTreeFurcation->m_lppTreeFurcation[lpByte[i]];
//        if (lpFurcation)
//        {
//            lpTreeFurcation = lpFurcation;
//        }
//        else
//        {
//            m_eTreeErrorCode = ERROR_DATA_NOT_EXIST;
//            return NULL;
//        }
//    }
//    return lpTreeFurcation->GetData();
//}
//
//// 删除元素
//BOOL CMsTreeIndex::DelData(LPBYTE lpByte, DWORD dwSize)
//{
//    CMsTreeNode* lpTreeFurcation = m_lpTreeFurcation;
//    for (DWORD i = 0; i < dwSize; i++)
//    {
//        if (lpByte[i] > m_dwMaxNodeCount)
//        {
//            m_eTreeErrorCode = ERROR_PARAMETER_OVERFLOW;
//            return FALSE;
//        }
//        CMsTreeNode* lpFurcation1 = lpTreeFurcation->m_lppTreeFurcation[lpByte[i]];
//        if (lpFurcation1)
//        {
//            lpTreeFurcation = lpFurcation1;
//        }
//        else
//        {
//            m_eTreeErrorCode = ERROR_DATA_NOT_EXIST;
//            return NULL;
//        }
//    }
//
//    CMsTreeNode* lpFurcation2 = lpTreeFurcation->GetFather();
//    while (lpFurcation2)
//    {
//        lpFurcation2->SubUse();
//        if (!lpFurcation2->IsUse())
//        {
//            m_CMsPoolManager.Remove(lpFurcation2);
//        }
//        lpFurcation2 = lpFurcation2->GetFather();
//    }
//    return TRUE;
//}
//
//// 获取错误代码
//CMsTreeIndex::E_ERROR_CODE CMsTreeIndex::GetLastErrorCMsTreeIndex(VOID)
//{
//    return m_eTreeErrorCode;
//}
