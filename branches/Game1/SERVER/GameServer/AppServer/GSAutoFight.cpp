#include "StdAfx.h"
#include "GSAutoFight.h"
#include "Player.h"
#include "ServerRegion.h"


CTimerQueue<GSAutoFight*>	GSAutoFight::s_TimerQueue(4, Auto_StableTime_Space);
CTimerQueue<GSAutoFight*>   GSAutoFight::s_CommonTimerQueue(4,Auto_Common_Space);

GSAutoFight::GSAutoFight(void)
:m_pOwner(NULL)
,m_bTimerRunning(FALSE)
,m_dwDurabilityZeroAI(0)
,m_CommonTimerID(0)
,m_bCommonRunning(FALSE)
,m_bProtected(FALSE)
,m_dwTimerAI(0)
,m_dwStableHookReliveAI(0)
{

}

GSAutoFight::~GSAutoFight()
{
    CloseTimer();
    CloseCommonTimer();

    //结束稳定挂机
    tagAutoFightNode AutoFightNode;
    AutoFightNode.bOpen = 0;
    AutoFightNode.lFightTypeIndex = eAHT_STABLE;
    SetOneSetup(eAHT_STABLE,AutoFightNode);

    //结束普通挂机
    tagAutoFightNode AutoFightCommontNode;
    AutoFightCommontNode.bOpen = 0;
    AutoFightCommontNode.lFightTypeIndex = eAHT_COMMON;
    SetOneSetup(eAHT_COMMON,AutoFightCommontNode);

    m_bProtected = FALSE;
    m_bTimerRunning = FALSE;
    m_dwTimerAI = 0;
}

void GSAutoFight::SetOwner(CPlayer* pMainPlayer)
{
    if (pMainPlayer)
    {
        m_pOwner = pMainPlayer;
    }
}

CTimerQueue<GSAutoFight*>& GSAutoFight::GetTimerQueue()
{
    return s_TimerQueue;
}

CTimerQueue<GSAutoFight*>& GSAutoFight::GetCommonTimerQueue()
{
    return s_CommonTimerQueue;
}
//改变辅助挂机设置
LONG GSAutoFight::ChangeSetup(LONG lType, tagAutoFightNode &AutoFightNode, BOOL bSysOpt /* = FALSE */)
{
    LONG lRetValue = eASET_NotErr;
    if (NULL != m_pOwner)
    {
        switch(lType)
        {
        case eAHT_COMMON:
            {
                CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
                if (pRegion != NULL)
                {
                    if (!pRegion->GetCommonHook())
                    {
                        lRetValue = eASET_NotMap;
                        return lRetValue;
                    }

                    //开启
                    if (AutoFightNode.bOpen)
                    {
                        OpenCommonTimer();
                    }
                    else
                    {
                        if (m_bTimerRunning)
                        {
                            m_dwTimerAI = 0;
                            CloseTimer();
                        }
                        if (m_bCommonRunning)
                        {
                            CloseCommonTimer();
                            m_dwDurabilityZeroAI = 0;
                        }
                    }
                }
            }
            break;
        case eAHT_STABLE:
            {
                CServerRegion *pRegion = dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
                if (pRegion != NULL)
                {
                    if (!pRegion->GetStableHook())
                    {
                        lRetValue = eASET_NotMap;
                        return lRetValue;
                    }
                    if (!AutoFightNode.bOpen)
                    {   
                        //关闭，保存稳定时间
                        if (m_bTimerRunning)
                            CloseTimer();
                        //关闭共有定时器
                        if (m_bCommonRunning)
                            CloseCommonTimer();
                        //if (GetStableTime() != 0)
                        //    SetStableTime(GetStableTime() - m_dwTimerAI);
                        m_dwTimerAI = 0;
                        m_dwDurabilityZeroAI = 0;
                    }
                    else
                    {
                        //开启
                        if (GetStableTime() != 0)
                        {
                            if (!m_bTimerRunning)
                                OpenTimer();
                            //开启共有定时器
                            OpenCommonTimer();
                        }
                        else
                        {
                            //没有稳定时间
                            AutoFightNode.bOpen = 0;
                            lRetValue = eASET_NoStableTime;
                            return lRetValue;
                            //AutoFightNode.lFightTypeIndex = eAHT_STABLE;
                            //lRetValue = SetOneSetup(lType,AutoFightNode);
                        }
                    }
                }
            }
            break;
        case eAHT_REPAIR:
            {
                if (AutoFightNode.bOpen)
                {
                    if (m_pOwner->GetSilver() > MAX_REPAIRMONMEY)
                    {
                        break;
                    }
                    else
                    {
                        //没有达到银币底线
                        AutoFightNode.bOpen = 0;
                        lRetValue = eASET_NoMoney;
                        return lRetValue;
                    }
                }
            }
            break;
        default:
            break;
        }
        lRetValue = SetOneSetup(lType,AutoFightNode);
    }
    return lRetValue;
}

//!								定时器超时响应
long GSAutoFight::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
    if (NULL != m_pOwner)
    {
        if (timerid != -1)
        {
            if (timerid == m_CommonTimerID)
            {
                BeginWork();
                return timerid;
            }
        }
        //死亡
        if (m_pOwner->IsDied())
        {
           m_dwTimerAI = 0;
           if (m_bProtected)
           {
               //已经进入了稳定保护状态
               if (m_dwStableHookReliveAI==Auto_DieReliveTime)
               {
                   //原地复活
                   m_pOwner->OnRelive(1);
                   m_dwStableHookReliveAI = 0;
               }
               else
               {
                   DWORD dwFrColor = 4294309170;
                   DWORD dwBkColor = 3435973836;
                   char strContend[256] = "";
                   _snprintf(strContend, 256, CStringReading::LoadString(IDS_GS_SERVERMESSAGE,STR_GS_SERVERMESSAGE_RELIVETIMER),Auto_DieReliveTime-m_dwStableHookReliveAI);

                   //发送  PostPlayerInfo
                   CMessage msg(MSG_S2C_OTHER_ADDPLAYERINFO);
                   msg.Add((LONG)0);
                   msg.Add(dwFrColor);
                   msg.Add(dwBkColor);
                   msg.Add(strContend);
                   msg.SendToPlayer(m_pOwner->GetExID());

                    m_dwStableHookReliveAI++;
               }
           }
           return 0;
        }

        if (!GetStableTime())
        {
            //时间到了
            //if (m_bProtected)
            //{      
                m_bProtected = FALSE;
                //稳定时间用完且没有达到稳定保护状态的时间，客户端自动切换到普通 ，关闭稳定保护状态
                CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeStableTime);
                msg.Add( m_pOwner->GetType() );
                msg.Add( m_pOwner->GetExID() );
                msg.Add( "StableProtectedStatus" );
                msg.Add( (long)m_bProtected );
                msg.SendToPlayer(m_pOwner->GetExID());	// 发送到客户端*/

           /* }
            else
            {
                //稳定时间用完且没有达到稳定保护状态的时间，切换到普通
                CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeStableTime);
                msg.Add(m_pOwner->GetType());
                msg.Add(m_pOwner->GetExID());
                msg.Add("ChangeToCommonMode");
                msg.SendToPlayer(m_pOwner->GetExID());
            }*/
            CloseTimer();
            CloseCommonTimer();
            
            m_dwDurabilityZeroAI = 0;
            m_dwTimerAI = 0;
            m_bTimerRunning = FALSE;

            //结束稳定挂机
             tagAutoFightNode AutoFightNode;
            AutoFightNode.bOpen = 0;
            AutoFightNode.lFightTypeIndex = eAHT_STABLE;
            SetOneSetup(eAHT_STABLE,AutoFightNode);

            SetStableTime(0);
            //设置保存标识
            m_pOwner->SetSavePartFlag(m_pOwner->GetSavePartFlag()|SAVE_DETAIL_INNERHOOK);
        }
        else
        {
            if (m_dwTimerAI == Auto_EnterStableTime)
            {
                m_bProtected = TRUE;
                //发送稳定保护状态
                CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeStableTime);
                msg.Add( m_pOwner->GetType() );
                msg.Add( m_pOwner->GetExID() );
                msg.Add( "StableProtectedStatus" );
                msg.Add( (long)m_bProtected );
                msg.SendToPlayer(m_pOwner->GetExID());	// 发送到客户端*/
            }
            m_dwTimerAI++;
            SetStableTime(GetStableTime() - 1);
            //设置保存标识
            m_pOwner->SetSavePartFlag(m_pOwner->GetSavePartFlag()|SAVE_DETAIL_INNERHOOK);
        }
    }
    return 0;
}


void GSAutoFight::BeginWork()
{
    if (m_pOwner != NULL)
    {
        if (m_pOwner->IsDied())
        {
            return;
        }

        CGoods* pGoods = NULL;
        // ----------------------------
        //	16:武器
        pGoods = m_pOwner -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON );
        if (pGoods != NULL)
        {
            //当前的耐久值
            LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
            if (lCurrentDurability == 0)
            {
                //武器的耐久值为0，先查看能不能进行自动修理(开启了自动修理没有以及银币够不够)
                const tagAutoFightNode* pAutoFightNode;
                pAutoFightNode = GetOneSetup((LONG)eAHT_REPAIR);
                if (pAutoFightNode->bOpen)
                {
                    //银币不足判断
                    if (CanAutoRepair())
                    {
                        return;
                    }
                }
                //没有开启和钱不够，就不能进行自动修理了， 3分钟后自动回城
                if (m_dwDurabilityZeroAI == Auto_ReturnCity)
                {
                    //关闭定时器
                    CloseTimer();
                    CloseCommonTimer();

                    //发送关闭开启给客户端
                    //结束稳定挂机
                    tagAutoFightNode AutoFightNode;
                    AutoFightNode.bOpen = 0;
                    AutoFightNode.lFightTypeIndex = eAHT_STABLE;
                    LONG lReValue = SetOneSetup(eAHT_STABLE,AutoFightNode);

                    CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
                    if (lReValue == eASET_NotErr)
                    {
                        msg.Add( (LONG)lReValue);
                        msg.Add(AutoFightNode.lFightTypeIndex);
                        msg.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
                        msg.SendToPlayer(m_pOwner->GetExID(), false);
                    }

                    CMessage msg1(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
                    AutoFightNode.lFightTypeIndex = eAHT_COMMON;
                    lReValue = SetOneSetup(eAHT_COMMON,AutoFightNode);
                    if (lReValue == eASET_NotErr)
                    {
                        msg1.Add( (LONG)lReValue);
                        msg1.Add(AutoFightNode.lFightTypeIndex);
                        msg1.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
                        msg1.SendToPlayer(m_pOwner->GetExID(), false);
                    }

                    CMessage msg2(MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re);
                    AutoFightNode.lFightTypeIndex = eAHT_REPAIR;
                    lReValue = SetOneSetup(eAHT_REPAIR,AutoFightNode);
                    if (lReValue == eASET_NotErr)
                    {
                        msg2.Add( (LONG)lReValue);
                        msg2.Add(AutoFightNode.lFightTypeIndex);
                        msg2.AddEx(&AutoFightNode, sizeof(tagAutoFightNode));
                        msg2.SendToPlayer(m_pOwner->GetExID(), false);
                    }

                    m_dwTimerAI = 0;
                    //回城
                    m_pOwner->SetState(CShape::STATE_DIED);
                    m_pOwner->OnRelive(0);
                    m_dwDurabilityZeroAI = 0;
                }
                else
                {
                    DWORD dwFrColor = 4294309170;
                    DWORD dwBkColor = 3435973836;
                    char strContend[256] = "";
                    _snprintf(strContend, 256, CStringReading::LoadString(IDS_GS_SERVERMESSAGE,STR_GS_SERVERMESSAGE_RETURNCITYTIMER),Auto_ReturnCity-m_dwDurabilityZeroAI);

                    //发送  PostPlayerInfo
                    CMessage msg(MSG_S2C_OTHER_ADDPLAYERINFO);
                    msg.Add((LONG)0);
                    msg.Add(dwFrColor);
                    msg.Add(dwBkColor);
                    msg.Add(strContend);
                    msg.SendToPlayer(m_pOwner->GetExID());

                    m_dwDurabilityZeroAI+=3;
                }
            }
        }
    }
}

void GSAutoFight::OnTimerCancel(DWORD timerid,const void* var)
{
}

void GSAutoFight::OpenTimer()
{
    if(!m_bTimerRunning)
    {
        GetTimerQueue().Schedule(this, NULL, Auto_StableTime_Space, Auto_StableTime_Space);
        m_bTimerRunning = TRUE;
    }
}

void GSAutoFight::OpenDieReLiveTimer(DWORD timeValue)
{
    if (!m_bTimerRunning)
    {
        GetTimerQueue().Schedule(this, NULL, timeGetTime()+timeValue*60*1000,0);
        m_bTimerRunning = TRUE;
    }
}

void GSAutoFight::OpenCommonTimer()
{
    if (!m_bCommonRunning)
    {
        m_CommonTimerID = GetCommonTimerQueue().Schedule(this, NULL, Auto_Common_Space, Auto_Common_Space);
        m_bCommonRunning = TRUE;
    }
}
void GSAutoFight::CloseTimer()
{
    m_bTimerRunning = FALSE;
    m_bProtected = FALSE;

    GetTimerQueue().Cancel(this);
}

void GSAutoFight::CloseCommonTimer()
{
    m_bCommonRunning = FALSE;
    m_CommonTimerID = 0;
    GetCommonTimerQueue().Cancel(this);
}

//自动修理装备银币判断
bool GSAutoFight::CanAutoRepair()
{
    if (m_pOwner)
    {
        if (m_pOwner->GetSilver() > MAX_REPAIRMONMEY)
        {
            return true;
        }
    }
    return false;
}