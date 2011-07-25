#include "Precompiled.h"

CGameTree::CGameTree(CMsGdiBuffImage* lpCMsGdiBuffImage)
: m_lpCGameTypeRoot     (NULL)
, m_dwGameTypeNodeCount (0)
, m_lpCMsGdiBuffImage   (lpCMsGdiBuffImage)
, m_nLastBottom         (0)
, m_lpLastNode          (NULL)
, m_pCGameTreeCaption   (NULL)
, m_pCGameTreeScrollbar (NULL)
, m_bScrollFocus        (FALSE)
{
    m_pCGameTreeCaption = NEW CGameTreeCaption(m_lpCMsGdiBuffImage);
    AssertEx(AL_CATAST, m_pCGameTreeCaption, _T("内存不足"));
    m_pCGameTreeCaption->SetFont(RR_GAME_CAPTION_TEXT_SIZE, FW_HEAVY, FALSE, FALSE, _T("黑体"));

    DWORD dwScrollbarX = 0;
    dwScrollbarX = g_pCGameStation->GetConfig()->dwDeskWidth;
    dwScrollbarX += g_pCGameStation->GetConfig()->dwRoomWidth;
    dwScrollbarX += g_pCGameStation->GetConfig()->dwDistrictWidth;
    dwScrollbarX += g_pCGameStation->GetConfig()->dwGameWidth;
    dwScrollbarX += g_pCGameStation->GetConfig()->dwGameTypeWidth;

    DWORD dwScrollbarY = 0;
    //dwScrollbarY = g_pCGameStation->GetConfig()->dwDeskHight;
    //dwScrollbarY += g_pCGameStation->GetConfig()->dwRoomHight;
    //dwScrollbarY += g_pCGameStation->GetConfig()->dwDistrictHight;
    //dwScrollbarY += g_pCGameStation->GetConfig()->dwGameHight;
    //dwScrollbarY += g_pCGameStation->GetConfig()->dwGameTypeHight;

    m_pCGameTreeScrollbar = NEW CGameTreeScrollbar(m_lpCMsGdiBuffImage, dwScrollbarX, dwScrollbarY, SCROLL_BAR_WIDTH, SCROLL_BAR_HEIGHT, LOGNITUDINAL);
    AssertEx(AL_CATAST, m_pCGameTreeScrollbar, _T("内存不足"));

    m_CaptionRc.left = 0;
    m_CaptionRc.right = 500;
    m_CaptionRc.top = 0;
    m_CaptionRc.bottom = 200;
}

CGameTree::~CGameTree(VOID)
{
    this->Destroy();
}

// 销毁节点
VOID CGameTree::Destroy(VOID)
{
    AUTO_LOCK(m_Lock);
    lpCGameTypeNode lpGameTypeNode = NULL;
    FOR_x_TO(i, m_dwGameTypeNodeCount)
    {
        lpGameTypeNode = m_lpCGameTypeRoot;
        m_lpCGameTypeRoot = (lpCGameTypeNode)m_lpCGameTypeRoot->GetNextNode();
        lpGameTypeNode->CutMe();
        lpGameTypeNode->Destroy();
        SAFE_DELETE(lpGameTypeNode);
    }
    m_dwGameTypeNodeCount = 0;
    SAFE_DELETE(m_pCGameTreeCaption);
    SAFE_DELETE(m_pCGameTreeScrollbar);
}

// 重置节点
VOID CGameTree::Reset(VOID)
{
    AUTO_LOCK(m_Lock);
    lpCGameTypeNode lpGameTypeNode = NULL;
    FOR_x_TO(i, m_dwGameTypeNodeCount)
    {
        lpGameTypeNode = m_lpCGameTypeRoot;
        m_lpCGameTypeRoot = (lpCGameTypeNode)m_lpCGameTypeRoot->GetNextNode();
        lpGameTypeNode->CutMe();
        lpGameTypeNode->Destroy();
        SAFE_DELETE(lpGameTypeNode);
    }
    m_dwGameTypeNodeCount = 0;
}

// 绘制节点
VOID CGameTree::Render(VOID)
{
    AUTO_LOCK(m_Lock);
    if (m_lpCGameTypeRoot)
    {
        // 绘制节点
        m_lpCGameTypeRoot->Render(m_nLastBottom);

        // 绘制滚动条
        m_pCGameTreeScrollbar->DrawScrollbar();

        // 绘制标题
        m_pCGameTreeCaption->PrintText(&m_CaptionRc, _T("冗睿游戏平台"), (INT)_tcslen(_T("冗睿游戏平台")), RGB(255,0,0), DT_CENTER);
    }
}

// 增加游戏类型
CGameTypeNode* CGameTree::AddGameTypeNode(VOID)
{
    lpCGameTypeNode lpNode = NULL;

    // 如果是第一个节点、创建自己为头尾节点
    if (0 == m_dwGameTypeNodeCount)
    {
        m_lpCGameTypeRoot = NEW CGameTypeNode(m_lpCMsGdiBuffImage, HEAD_NODE, TAIL_NODE);
        AssertEx(AL_CATAST, m_lpCGameTypeRoot, _T("内存不足"));
        m_dwGameTypeNodeCount++;
        m_lpLastNode = m_lpCGameTypeRoot;
        lpNode = m_lpCGameTypeRoot;
    }

    // 如果之前有节点、挂在之前节点的后面
    else
    {
        m_lpLastNode = NEW CGameTypeNode(m_lpCMsGdiBuffImage, m_lpLastNode, TAIL_NODE);
        AssertEx(AL_CATAST, m_lpLastNode, _T("内存不足"));
        m_dwGameTypeNodeCount++;
        lpNode = m_lpLastNode;
    }
    DWORD dwHight = g_pCGameStation->GetConfig()->dwGameTypeHeight + g_pCGameStation->GetConfig()->m_dwBetween;
    m_pCGameTreeScrollbar->AddMaxMoveScrollY(dwHight);
    return lpNode;
}

// 鼠标是否穿过滚动条
BOOL CGameTree::IsOnScrollbar(POINT& Pos)
{
    USE_PARAM(Pos);
    return TRUE;
}

// 记录鼠标位置
VOID CGameTree::SaveMousePos(POINT& Pos)
{
    m_OldMousePos = Pos;
}

// 记录鼠标位置
VOID CGameTree::OperationPosOffset(POINT& Pos)
{
    Pos.x -= m_OldMousePos.x;
    Pos.y -= m_OldMousePos.y;
}

// 移动滚动条
VOID CGameTree::MoveScroll(POINT& Pos)
{
    if (m_pCGameTreeScrollbar)
    {
        m_pCGameTreeScrollbar->MoveScrollX(Pos.x);
        m_pCGameTreeScrollbar->MoveScrollY(Pos.y);
    }
}

// 设置滚动条焦点
VOID CGameTree::GetScrollFocus(VOID)
{
    m_bScrollFocus = TRUE;
}

// 释放滚动条焦点
VOID CGameTree::ReleaseScrollFocus(VOID)
{
    m_bScrollFocus = FALSE;
}

// 是否滚动条焦点
BOOL CGameTree::IsScrollFocus(VOID)
{
    return m_bScrollFocus;
}