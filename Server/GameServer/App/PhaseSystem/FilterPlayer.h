/** 
 * @file   PhaseManager.h  
 * @author Expter
 * @brief  是玩家的过滤类，主要是对不同相位标示条件下的消息处理或者操作过滤。
 * @date   2010/03/18        
 */

#pragma  once

#include "../MoveShape.h"
namespace PhaseSystem
{
    class FilterPlayer
    {
    public:
        ///
        /// 因为具体过滤后需要做的操作不放在服务器处理，所以这里只是需要判断2者之间的关系即可。
        /// 
        static  bool Filter(CMoveShape* player , CMoveShape* shape)
        {
            /// 如果相位玩家处于濒死状态
            if ( shape->GetType() == TYPE_MONSTER 
                && shape->GetPhaseID() == player->GetPhaseID() 
                && player->GetState() == CShape::STATE_AGONAL )
                return true;
            return player->GetPhaseID() != shape->GetPhaseID() ;
        }
    };
}
