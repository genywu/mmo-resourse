/** 
 * @file   PhaseManager.h  
 * @author Expter
 * @brief  管理所有的相位,以及相位阻挡,脚本接口,消息的响应,
 *         提供给外部处理相位功能的类
 *
 * @date   2010/03/18        
 */
#pragma  once

#include "../session/CSession.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../../../../Public/Common/PhaseDef.h"
#include "../../../../Public/ServerPublic/Server/BaseDef/PhaseSeverDef.h"

#define  RNULL ;
#define  PHASEMAN  GetInst(PhaseManager)
#define  PhaseOpen(ret)   if(GetInst(PhaseSetup).PhaseEnable() == 0 )   return ret;

class PhasePro;
class PhaseManager
{
    struct  ApplyList 
    {
        CGUID first;    /// 被邀请者ID
        CSession* second;   /// SessionsID
        ApplyList( CGUID f,CSession* s):first(f),second(s)
        {
        }
    };

    typedef    stdext::hash_map<CGUID,PhasePro*,hash_guid_compare>   PhaseTable;
    typedef    PhaseTable::iterator                                  PIter;
    typedef    stdext::hash_map<CGUID,CGUID,hash_guid_compare>       PhaseInvite;
    typedef    PhaseInvite::iterator                                 PIVter;
    typedef    std::map<CGUID,ApplyList>                             AppMap;
    typedef    AppMap::iterator                                      AppItr;

public:
    /// 构造函数
    PhaseManager();
    ~PhaseManager();

    /// 释放所有资源
    void  Release();

    void  Release(const CGUID& GID);

    /// 
    /// 针对一个队伍的相位区域创建(野外战斗类型)
    /// @param  player  对于创建相位功能的玩家
    /// @param  size1   范围1的大小
    /// @param  size2   范围2的大小
    /// @param  trigger 相位触发类型（主动被动）
    /// @param  scriptfile 相位创建完成需要触发的脚本
    /// 
    void   CreatePhase(CPlayer* player,float size1,
                       float size2,eTrigger trigger,
                       const char* Scriptfile);

    ///
    /// 创建一个竞技的相位区域（这个是功能处理）
    /// @param  first   对于请求创建请求相位功能的玩家ID
    /// @param  second  对于接受邀请创建相位功能的玩家ID
    /// @param  size1   范围1的大小
    /// @param  size2   范围2的大小
    /// @param  trigger 相位触发类型（主动被动）
    /// @param  scriptfile 相位创建完成需要触发的脚本
    /// 
    void   CreatePhase(const CGUID& first,const CGUID& second,
                       float size1,float size2,
                       eTrigger trigger,
                       const char* Scriptfile );

    /// 
    /// 在相位相对区域点x_point , y_point 创建怪物一只怪物
    /// @param   GID        相位的GUID
    /// @param   OrgName    创建怪物的原始名
    /// @param   xy_point   怪物的相对相位中心点的坐标
    /// 
    void  CreatePhaseMonster(const CGUID& GID,const char* OrgName,
                             float x_point , float y_point);

    ///  处理相位消息
    void  OnMessage(CMessage* pMsg);
        
    ///  得到相位的当前剩余的玩家数量
    long  GetPhasePlayerNum(const CGUID& id);

    /// 
    /// 激活当前相位
    /// @param   GID        相位的GUID
    /// @param   eTimerType 相位计数器类型 Add,Sub
    /// @param   lTimer     相位触发时间
    /// @param   lExipires  相位触发周期
    ///   
    long  ActivePhase(const CGUID& GID,eTimerType etype,
                      long  lTimer , long lExipires);

    /// 
    /// 针对相位操作，此函数是根据需求不同的处理条件而剥离出来
    /// 针对玩家各种相位处理类型进行处理
    /// @param   Oper   处理类型
    /// 
    long  OnPhaseOper(const CGUID& GID,CPlayer* player,ePhaseOper Oper);

    /// 相位操作针对怪物
    long  OnPhaseOper(const CGUID& GID,CMonster* Monster,ePhaseMOper Oper);

    /// 相位事件
    long  OnPhaseEvent(const CGUID& GID,CMoveShape*shape,ePhaseEvent Event);

    ///
    /// shape对象是否对玩家过滤
    /// @return  返回1表示需要对玩家过滤，返回0表示不需要过滤
    /// 
    bool  PhasePlayerFilter(CMoveShape* player,CMoveShape* shape);

    /// 
    /// 获得相位的坐标信息值
    /// @param  GID    相位的GUID
    /// @param  pName  表示相位的参数名字
    /// 
    long GetPhasePoint(const CGUID& GID,const char* pName);

    ///
    /// 针对一个玩家提供一个接口表示标记相位结束，可以是主动也可以是被动
    /// @param   GID   相位的GUID
    /// 
    bool  SetPhaseEnd(const CGUID& GID,CPlayer* player);

    /// 判断某点的相位阻挡
    bool  IsPhaseBlock(CMoveShape* shape,long xNew,long yNew);

    ///
    /// 为了完成奖励以及触发，需要对相位结束运行一个脚本，
    /// @function   SetPhaseEndScriptName 设置相位结束运行脚步
    /// @param   GID        相位的GUID
    /// @param   FileName   相位结束运行的脚本
    /// 
    void  SetPhaseEndScriptName(const CGUID& GID,const char* FileName);

    /// 
    /// 提供给脚本一个进行服务器系统主动结束相位的接口
    /// @param   GID     相位ID
    ///  
    void  ClosePhase(const CGUID& GID);

    /// 
    /// 返回相位功能的定时器时间
    /// @param   GID    相位ID
    /// 
    long  GetCostTime(const CGUID& GID);

    /// 
    /// 关闭相位的定时器功能
    /// @param   GID    相位ID
    /// 
    void  CancelPhaseTimer(const CGUID& GID);

    ///  
    /// 返回相位的类型
    /// @param   GID    相位ID
    /// 
    ePhaseType  GetPhaseType(const CGUID& GID);

    ///  
    /// 给相位投递一个消息
    /// @param   GID    相位ID
    /// @param   pMsg   消息内容
    /// 
    void   PostPhaseMsg(const CGUID& GID,const char* pMsg,ulong dwColor, ulong dwBkColor,long ePos);

    /// 
    /// 增加映射表
    /// @param  first  邀请者ID
    /// @param  second 被邀请ID
    ///
    bool   AddPhaseInviteMapTable(const CGUID& first,const CGUID& second);

    /// 邀请相位缓存清空
private:
    void  ClearTimeOut();

private:
    /// 所有的相位列表
    PhaseTable        m_PhaseList;
    /// 邀请者和被邀请者映射表
    AppMap            m_InviteList;
};