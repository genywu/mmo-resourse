/** 
 *  @brief  天赋影响的每一个技能
 *  @author expter
 *  @date   2010/08/31
 */           

#pragma  once 

#include <string>
#include <map>
#include <vector>

#include "../../../Public/Common/SkillAttributeDef.h"
#include "../../../Public/Common/GeniusDef.h"
#include "GeniusComDef.h"

#define  SKILLTBL       GetInst(SkillAttribute)
#define  SKILLGNOPER    GetInst(GeniusSysOper)

class  CPlayer;
class  GeniusSysOper;

class  GeniusSysOper
{
    typedef std::map<std::string,long>  AttriMap;
    typedef std::vector<tagSkillEffPro> SkillPro;
    typedef void  (*GeniusFun)(AttriMap& mp,string name,long temp,long value);
    typedef std::map<GeniusOper,GeniusFun>   OperFun;

public:
    GeniusSysOper();
    ~GeniusSysOper();

    long     GetBaseNum(){ return m_BaseNum;}

    void     GeniusPolicy(SkillPro&  Attri,
                          AttriMap&  mp,
                          long id,
                          long lvl);
private:

    OperFun   m_OperTable;
    long      m_BaseNum;
};

/// 天赋影响某个特定技能的最终效果
class  GeniusSys 
{

public:
    GeniusSys(long skillid,long Lvl,CPlayer * player);
    ~GeniusSys();

    /// 更新当前技能的所有的属性
    void   Update(UpdateEvent Event, long id , long Lvl );

    /// 增加
    void   AddSkillPro(std::vector<tagGeniusSkill>& GeniusVec,long Lvl);

    /// 取得技能对应属性值
    long   GetSkillAttri(long lvl , std::string  name); 

    /// 更新编解码
    void   CodeGeniusToDataBlock(DBWriteSet& setWriteDB);

    /// 单个更新接口
    void   CodeDateToClient();

private:

    void   CalcUpdateSkillPro();

    /// 技能发送变化更新属性表
    void   UpdateSkillAttri(long  skillid);          

    /// 分配三种运算属性
    void   Assign(std::vector<tagGeniusSkill>& GeniusVec,
                  std::vector<tagSkillEffPro>& MultiS,
                  std::vector<tagSkillEffPro>& AddSub,
                  std::vector<tagSkillEffPro>& MultiE);

private:

    typedef std::map<std::string,long>   AttriMap;
 
    
    long       m_Skillid;
    long       m_SkillLvl;
    CPlayer*   m_pOwner;

    AttriMap   m_SkillMap;
    AttriMap   m_UpdateAttr;
    
};

void   AddMpPolicy(std::map<string,long>& mp,
                   string name,
                   long temp,
                   long value);

void   SubMpPolicy(std::map<string,long>& mp,
                   string name,
                   long temp,
                   long value);

void   MultiMpPolicy(std::map<string,
                     long>& mp,
                     string name,
                     long temp,
                     long value);
