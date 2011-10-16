/** 
 *  @file   PhaseServerDef.h
 *  @brief  相位功能:
 *          关于相位功能的服务器的一些枚举和属性定义
 *  @date   2010/03/18
 */

#pragma  once 

enum ePhaseRest
{
    ePRest,              /// 创建相位请求主动 
    ePInVitee            /// 创建相位被邀请者
};

enum  ePhaseWSInfo
{   
    eUpdateGSPhaseID,
};

enum  S2WPhaseOper
{
    eUpdateWSPhaseID,
    eDeleteWSPhaseID,

};
enum  S2WPhaseMsg
{
    eUpdatePlayerPhaseID,
};

struct PhaseFace
{
    PhaseFace(CGUID id,ePhaseRest st):guid(id),Rest(st)
    {

    }

    CGUID       guid;
    ePhaseRest  Rest;
};
