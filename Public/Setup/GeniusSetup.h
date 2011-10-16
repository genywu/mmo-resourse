/** 
 *  @file   GeniusSetup.h
 *
 *  @brief  关于天赋的配置
 *  @author Expter
 */

#pragma  once 

#include "../Common/GeniusDef.h"

class  GeniusSetup
{

public:
    /// 构造函数
    GeniusSetup();
    ~GeniusSetup();

    /// 载入配置文件
    bool  Load(const char* filename="Data/GeniusSys/GeniusList.xml");

    /// 载入天赋列表
    bool  LoadList(const char* filename);

    /// 编码
    void  GeniusConfigEnCode(DBWriteSet&  dbWrite);

    /// 解码
    void  GeniusConfigDecode(DBReadSet& dbRead);

    /// 得到当前技能依赖的天赋列表
    std::map<long,vector<long> >  GetGeniusList(long skillid);

    /// 得到技能表
    void  GetSkillList(long geniusid,long lvl,std::map<long,long>&  mp);

    /// 
    /// 得到天赋id的某个等级的全部属性配置表
    /// id  天赋id
    /// skill 技能id
    /// lvl 天赋等级
    ///
    tagGeniusSkill*   GetGeniusVec(long id,long skill,long lvl);

    /// 是否存在此天赋
    bool  FindGenius(long Geniusid);

    /// 得到天赋乘法基数
    long  GetGeniusBase()const {    return m_BaseNum;   }

    /// 是否是更新属性
    bool  IsSkillUpdatePro( std::string pro) ;

private:

    /// 载入更新列表
    bool    LoadSkillPro();

    /// 格式化字符串
    ObjType ForMatObjType(string str);

    EffObj  ForMatEffObj( string str);

    GeniusOper ForMatOper( string str);

private:
    /// 运算基数
    long                          m_BaseNum;
    /// 更新属性
    std::string                   m_UpdateSkillPro;
    /// 更新属性
    std::set<string>              m_UpdateSkillProTable;
    /// 成员数据
    std::map<long,tagGenius>      m_GeniusConfig;    
    /// 技能对应 影响的天赋表
    std::map<long,std::map<long,vector<long> > >  m_SkillTable;
    /// 所有的文件列表
    std::vector<std::string>      m_filelist;
};
