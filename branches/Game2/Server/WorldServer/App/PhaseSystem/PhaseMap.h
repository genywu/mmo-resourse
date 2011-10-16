/** 
 *  @file   PhaseMap.h
 *  @Author Expter
 *  @brief  相位功能:
 *          关于相位功能的WS保存相位存在情况下的离线玩家的一些数据
 *  @date   2010/03/18
 */
#pragma  once
                
class  CGUID;
class  CPlayer;
class  CMessage;

#include "../../../../Public/ServerPublic/Server/BaseDef/PhaseSeverDef.h"

class  PhaseMap
{
    typedef  stdext::hash_map<CGUID,PhaseFace,hash_guid_compare>  GGMAP;
    typedef  GGMAP::iterator                                      GIter;
                
public:

    void            AddPhaseTable(const CGUID& PlayerID,const PhaseFace& PhaseId);

    void            ClearPlayerID(const CGUID& PlayerID) ;

    const CGUID&    GetPhaseID(const CGUID& PlayerID);

    const ePhaseRest GetPhaseRest(const CGUID& PlayerID);

    void            OnMessage( CMessage* pMsg );
                    
private:
    GGMAP       m_GuidTable;

};