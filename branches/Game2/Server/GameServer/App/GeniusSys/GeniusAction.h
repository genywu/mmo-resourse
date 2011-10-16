/** 
 *  @file  GeniusAction.h
 *  @brief 技能的天赋系统
 *
 *  @author expter
 */


#pragma  once 

#include "GeniusComDef.h"
#include "../../../Public/Common/GeniusDef.h"

class  GeniusSys;
class  CPlayer;
class  GeniusAction
{
public:
    GeniusAction(CPlayer * player);
    ~GeniusAction();
    
    void   Update( UpdateEvent Event,long id, long Lvl );

    void   Remove( RemoveEvent Event,long id ,long Lvl );

    long   GetSkillAttri(long  skillid,long level,std::string  name,long xPos=0,long yPos=0);

    /// 天赋
    void   Update();

    /// 天赋增加
    void   AddGenius(long id , long lvl);

    /// 增加技能
    void   AddSkill(long ID, long level);

    /// 使用技能的时候附加状态
    void   AddGeniusStates(long skillid,CMoveShape* pDest,long Gid);

    /// 天赋属性编解码
    void   CodeGeniusToDataBlock(DBWriteSet& setWriteDB, BOOL bAddRestoreTime = TRUE);

private:

    void   Add( UpdateEvent Event , long id );

    void   AddStates(CMoveShape* pDest,tagGeniusBuff& buf);

private:                                     
    std::map<long,GeniusSys*>  m_GeniusList;
    CPlayer*                    m_pOwner;                 
};