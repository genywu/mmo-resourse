#include "Precompiled.h"

CDistricNode::CDistricNode(VOID)
: CBaseNode       (RGB(0,0,255))
{
    m_eType = NODE_DISTRIC;
    if (g_pCGameStation)
    {
        m_dwWidth = g_pCGameStation->GetConfig()->dwDistrictWidth; 
        m_dwHight = g_pCGameStation->GetConfig()->dwDistrictHight;
        m_dwIndent = g_pCGameStation->GetConfig()->dwDistrictIndent;
    }
    else
    {
        AssertEx(g_pCGameStation, _T("咋回事？"));
    }
}

CDistricNode::~CDistricNode(VOID)
{
    ;
}

// 初始化
BOOL CDistricNode::Init(DWORD dwRoomNodeCount)
{
    m_dwChildCount = dwRoomNodeCount;
    if (m_dwChildCount)
    {
        m_lppChildList = NEW lpCBaseNode[m_dwChildCount];
        if (m_lppChildList)
        {
            FOR_x_TO(i, m_dwChildCount)
            {
                m_lppChildList[i] = NEW CRoomNode;
                AssertEx(m_lppChildList[i], _T("内存不足"));
            }

            if (g_pCGameStation)
            {
                //DWORD dwDeskCountInEveryRoom = g_pCGameStation->GetConfig()->dwDeskCountInEveryRoom;
                FOR_x_TO(i, m_dwChildCount)
                {
                    m_lppChildList[i]->Init(0);
                }
                return TRUE;
            }
            else
            {
                AssertEx(g_pCGameStation, _T("又咋回事"));
            }
        }
        else
        {
            AssertEx(m_lppChildList, _T("内存不足"));
        }
    }
    return FALSE;
}

// 渲染
VOID CDistricNode::Render(INT& nBottom)
{
    return CBaseNode::Render(nBottom);
}