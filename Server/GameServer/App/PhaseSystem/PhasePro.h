/** 
 * @file   PhaseOper.h  
 * @author Expter
 * @brief  它是一个相位全部功能集合,相位的所有操作,相位阻挡及其脚本触发,
 *         同时管理相位中的玩家和怪物。
 *
 * @date   2010/03/18        
 */

#pragma once 

#include "../../../../Public/Common/PhaseDef.h"
#include "../../../../Public/ServerPublic/Server/BaseDef/PhaseSeverDef.h"


class Script;
class CPlayer;
class GSTeam;
class CServerRegion;
class PhaseList;

class PhasePro : public CGameEvent
{
    struct   PhasePlace
    {
        /// 坐标
        float  x_point;
        float  y_point;
        /// 范围1，2的大小
        float  a_size;
        float  b_size;
        CServerRegion*   pRegion;        
    };

    /// Typedef 
    typedef PhasePro self;
    typedef std::map<ePhaseRest,list<GSTeam*> >  MTEAMTABLE;
    typedef MTEAMTABLE::iterator                 MITER;
    friend  class  PhaseList;

public:
    /// 构造函数
    PhasePro();
    ~PhasePro();

    /// 释放
    void  Release();

    /// 
    /// 相位创建初始化firteam表示第一个队伍指针,secteam为第二个队伍指针
    /// 说明:(当前需求是单独的一个和2个对立的队伍进行相位创建)
    ///       如果为了满足以后的多个队伍触发方式只是修改接口为list<GSteam*>即可.
    /// 
    void  Init(CPlayer* first,GSTeam* firteam ,
               long num,float size1,float size2,
               ePhaseType type,eTrigger triggertype,
               CPlayer* second=NULL,
               GSTeam* secteam=NULL);

    /// 激活
    void  ActiveSelf(eTimerType etype,long lTimer,long lExipires);

    /// 主动处理玩家操作
    long  OnPhaseOper(CPlayer* player,ePhaseOper Oper,ePhaseRest Rest = ePRest);

    /// 响应怪物的操作事件
    long  OnPhaseOper(CMonster* monster,ePhaseMOper Oper);

    /// 相位事件
    long  OnPhaseEvent(CMoveShape* shape,ePhaseEvent Event);
  
    /// 运行脚本文件
    void  RunScript(CPlayer* player,const char *filename);

    /// 脚本运行
    void  RunScript(CPlayer* player,long id,long type);

    /// 脚本运行
    void  RunScript(const char* filename);

    /// 创建怪物
    void  CreateMonster(const char* OrgName,float x_point,float y_point);

    /// 服务器阻挡判断
    bool  IsPhaseBlock(CMoveShape* shape,long xNew,long yNew,long type=0);

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

    /// 给当前相位的所有玩家发送消息，内容为Msg
    void  PostSelfMsg(const char* Msg,ulong dwColor, ulong dwBkColor,long ePos);

    /// Get/Set其他函数
public:
  
    /// 返回相位的ID
    const CGUID& GetID() const         { return m_Id ;      }

    /// 返回相位类型
    ePhaseType   GetPhaseType() const  { return m_type;     }

    /// 返回相位的状态
    ePhaseState  GetPhaseStates()const {  return m_state;   } 
    
    ///  返回相位主动还是被动
    eTrigger     GetPhaseTrigger()const{ return m_triggertype;}

    /// Get/Set玩家的结束运行脚本
    void  SetEndScriptFile(const char* filename){  m_Endfile = filename;}

    const char* GetEndScriptFile() const   {  return m_Endfile.c_str();}

    /// 返回相位花费时间
    long  GetCostTime() const  ;

    /// 返回玩家数量
    long  GetPlayerNum() const ;

    /// 返回队形玩家数量
    long  GetPlayerNum(ePhaseRest e) const;

    /// 返回怪物数量
    long  GetMonsterNum()const ;
   
    /// 返回相位坐标信息
    long  GetPointInfo(const char* pName)const;

    void  SetPointInfo(const char*pName,double Value);

    /// 设置事件的状态
    void  SetPhaseStates(ePhaseState state,bool flag = false);
  
    /// 私有函数
private:
    
    /// 增加一个队伍映射
    void  AddTeadMember(ePhaseRest type,GSTeam* team);

    /// 移除相位玩家
    void  RemovePlayer(CPlayer* player);

    /// 移除相位怪物
    void  RemoveMonster(CPlayer* Monster);

    /// 启动定时器
    void  StarUpTimer();

    /// 拉人操作
    void  RemitPlayer();

    /// 判断是否结束标志
    bool  IsEnd();

private:
    /// 相位类型(野战,竞技)
    ePhaseType      m_type;
    /// 定时器类型(Sub,Add)
    eTimerType      m_timertype;
    /// 定时器类型(主动,被动)
    eTrigger        m_triggertype;
    /// 相位运行状态,运行中,运行结束
    ePhaseState     m_state;    
    /// 相位的触发时间
    long            m_lTime;
    /// 相位的超时时间
    long            m_lExpires;
    /// 相位定时器Timer
    long            m_lTimer;
    /// 开始时间
    long            m_StartTime;
    /// 结束时间
    long            m_EndTime;
    /// 结束执行脚本
    std::string     m_Endfile;
    /// shape列表
    PhaseList*      m_PhaseList;
    /// 相位额ID
    CGUID           m_Id;
    /// 相位的Place信息
    PhasePlace      m_Place;
    /// 相位队伍表( 预留多个队伍的触发方式 )
    MTEAMTABLE      m_ProTable; 
 };