#include "Precompiled.h"

CGameTypeNode::CGameTypeNode(CMsGdiBuffImage* lpCMsGdiBuffImage, CBaseNode* lpPreviousNode, CBaseNode* lpNextNode)
: CBaseNode     (lpPreviousNode, lpNextNode)
, m_pIocnImage  (NULL)
, m_pLinkImage  (NULL)
{
    m_pIocnImage = NEW CMsGdiPlusImage;
    HDC hDc = lpCMsGdiBuffImage->GetBuffDc();

    if (m_pIocnImage)
    {
        m_pIocnImage->Init(hDc);
    }
    else
    {
        AssertEx(AL_CATAST, m_pIocnImage, _T("内存不足"));
    }
    m_pLinkImage = NEW CMsGdiPlusImage;
    if (m_pLinkImage)
    {
        m_pLinkImage->Init(hDc);
    }
    else
    {
        AssertEx(AL_CATAST, m_pLinkImage, _T("内存不足"));
    }
}

CGameTypeNode::~CGameTypeNode(VOID)
{
    SAFE_DELETE(m_pIocnImage);
    SAFE_DELETE(m_pLinkImage);
}

// 初始化
BOOL CGameTypeNode::Init(S_GAME_TYPE_INFO* pInfo)
{
    TCHAR szFullPath[MAX_PATH] = {0};
    TCHAR szBuffPath[MAX_PATH] = {0};
    if (pInfo)
    {
        memcpy(&m_GameTypeInfo, pInfo, sizeof(m_GameTypeInfo));
        if (pInfo->m_wIconId < MAX_RES_COUNT)
        {
            _stprintf_s(
                szBuffPath,
                MAX_PATH,
                _T("%%s/%s/%s"),
                g_pCGameStation->GetResConfig()->m_szImageDir,
                g_pCGameStation->GetResConfig()->m_szImageName[pInfo->m_wIconId]
                );
            CreateFullPath(szFullPath, szBuffPath);
            m_pIocnImage->Load(szFullPath);
        }

        _stprintf_s(
            szBuffPath,
            MAX_PATH,
            _T("%%s/%s/05.PNG"),
            g_pCGameStation->GetResConfig()->m_szImageDir
            );
        CreateFullPath(szFullPath, szBuffPath);
        m_pLinkImage->Load(szFullPath);

        return TRUE;
    }
    return FALSE;
}

// 绘制节点
VOID CGameTypeNode::Render(IN OUT INT& nBottom)
{
    USE_PARAM(nBottom);
    CGameTypeNode* pCGameTypeNode = this;
    while ((pCGameTypeNode != TAIL_NODE) && pCGameTypeNode)
    {
        pCGameTypeNode->m_pIocnImage->Draw(0, nBottom);
        pCGameTypeNode->m_pLinkImage->Draw(60, nBottom);
        pCGameTypeNode = (CGameTypeNode*)pCGameTypeNode->GetNextNode();
        nBottom += g_pCGameStation->GetConfig()->dwGameTypeHeight;
        nBottom += g_pCGameStation->GetConfig()->m_dwBetween;
    }

    nBottom = 0;
}

// 打开节点
VOID CGameTypeNode::Open(BOOL bPrint)
{
    return CBaseNode::Open(bPrint);

}

// 关闭节点
VOID CGameTypeNode::Close(BOOL bPrint)
{
    return CBaseNode::Close(bPrint);
}

// 销毁节点
VOID CGameTypeNode::Destroy(VOID)
{

}

// 从链表中断开自己
VOID CGameTypeNode::CutMe(VOID)
{
    return CBaseNode::CutMe();
}