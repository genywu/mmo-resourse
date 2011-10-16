/** 
 * @file   PhaseList.h  
 * @author Expter
 * @brief  记录当前相位的玩家和怪物列表,以及事件响应
 * @date   2010/03/18        
 */
#pragma once 

#include "../../../../Public/Common/PhaseDef.h"
#include "../../../../Public/ServerPublic/Server/BaseDef/PhaseSeverDef.h"

class  PhasePro;
class  PhaseList
{
    typedef std::map<ePhaseRest,std::list<CPlayer*> > MPlayer;
    typedef MPlayer::iterator                    MPIER;
    
public:
     PhaseList(PhasePro* );
     ~PhaseList();

public:

     ///  清空信息
     void Release();

     ///  增加一个Player
     void AddPlayer(ePhaseRest e , CPlayer* player);

     ///  移除一个Player
     void RemovePlayer(ePhaseRest e,CPlayer* player);
     
     ///  增加一个Monster
     void AddMonster(CMonster* Monster);

     ///  移除一个Monster
     void RemoveMonster(CMonster* monster);

     ///  得到Player数目
     long GetPhasePlayerNum( ePhaseRest e ) const;

     ///  得到Monster数目
     long GetPhaseMonsterNum() const;

     ///  投递一个pMsg
     void PostPhaseMsg(const char* pMsg,ulong dwColor, ulong dwBkColor,long ePos);

     ///  每一个玩家触发一个脚本
     long PhaseExitEvent( );

private:

    MPlayer               m_PlayerList;
    std::list<CMonster*>  m_MonsterList;
    PhasePro*             m_PhasePro;
};