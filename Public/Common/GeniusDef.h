/** 
 *  @brief  关于天赋的配置信息
 *  @author expter
 */

#pragma  once 

struct  tagGeniusPro
{
    tagGeniusPro()
    {
        memset( this , 0 , sizeof( tagGeniusPro) );
    }
    long  id;               /// 天赋Id
    long  skillNum;         /// 天赋影响技能数量
    long  maxLvl;           /// 最高等级
    char  desid[16];        /// 天赋描述ID
};

/// 附加对象
enum  EffObj
{
    Obj_ESelf=1,
    Obj_EDest,
};

/// 对象类型
enum ObjType
{
    Obj_Invalid=-1,
    Obj_Pet             =0x0002,
    Obj_Monster         =0x0004,
    Obj_Player          =0x0008,    
    Obj_Monster_Player  =0x000C,
    Obj_Monster_Pet     =0x0006,
    Obj_Player_Pet      =0x000A,
    Obj_AllObj          =0x000F,
};

/// 计算公式
enum GeniusOper
{
    No_Type=0,
    Add_Type=1,
    Sub_Type,
    Multi_Type_S,          /// 先乘法 在加减
    Multi_Type_E,          /// 先加减在乘法
};

/// 技能属性值
struct tagSkillEffPro 
{
     tagSkillEffPro()
     {
         memset( this , 0 , sizeof(tagSkillEffPro) );
     }
     GeniusOper eff;        
     long       value; 
     char       type[16];
};

/// 天赋附加技能buff
struct tagGeniusBuff 
{
    tagGeniusBuff()
    {
        memset( this , 0 ,sizeof(tagGeniusBuff) );
    }
    long        AnnexNum;
    long        Level;
    long        AnnexId;
    ObjType     objType;
    EffObj      effobj;
};

/// 一个天赋等级下面的某个技能配置
struct  tagGeniusSkill 
{
    long        id;
    long        AnnexSum;
    std::vector<tagGeniusBuff>   GeniusBuff;
    std::vector<tagSkillEffPro>  SkillPro;

    tagGeniusSkill():id(0),AnnexSum(0)
    {
        GeniusBuff.clear();
        SkillPro.clear();
    }
};

/// 一个天赋等级对应的技能map
struct  tagGeniusLvl 
{
    tagGeniusLvl()
    {
        GeniusSkill.clear();
    }
    std::map<long,tagGeniusSkill>   GeniusSkill;
};

/// 一个天赋全部配置
struct  tagGenius 
{
    tagGenius()
    {
       GeniusConfig.clear();
    }
    tagGeniusPro                    GeniusPro;  
    /// 全部等级对应map
    std::map<long,tagGeniusLvl>     GeniusConfig;
};