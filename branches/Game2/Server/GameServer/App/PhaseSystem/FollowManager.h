/** 
 * @file   FollowManager.h  
 * @author Expter
 * @brief  玩家跟随队长管理，写此管理类的目的就是为了预留以后队长邀请队员
           跟随队长的一些缓存信息。
 * @date   2010/03/18        
 */

#pragma  once

#include <hash_map>

class   FollowManager : public CGameEvent
{
    typedef  std::map<long,CGUID>   lTIMER;
    typedef  stdext::hash_map<CGUID,CGUID,hash_guid_compare>  INVITETABLE;

public:
    FollowManager();
    ~FollowManager();

    /// 玩家停止跟随状态
    void  StopFollow( CPlayer* player );

    /// 邀请请求
    void  Invite();

    /// 同意邀请判断超时
    bool  IsTimeOut(const CGUID& ID);

    /// 超时响应
    long  OnTimeOut(ulong timerid,ulong curtime,const void* var);

    /// 定时器注销时调用的函数，可用于删除一些附加参数
    void  OnTimerCancel( DWORD timerid,const void *var);

    /// 定时器资源释放
    void  CancelTimer( ) ;

    /// 消息处理
    void  OnMessage(CMessage* pMsg){    }

    /// 处理
    long  Proc(eEventType type,const tagProcVar* pVar){ return 0; }

private:
    lTIMER          m_ltimer;
    INVITETABLE     m_InviteTable;

};