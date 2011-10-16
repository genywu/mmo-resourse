/**
 *  @file SkillAttriSetup.h
 *
 *  @brief  关于技能属性的属性配置
 *  @author expter
 */

#pragma  once 


#include "../Common/SkillAttributeDef.h"        

class  SkillAttribute
{
    typedef std::map<long,tagSkillAttr>   SkillTable;
    typedef std::map<long,SkillTable>     SkillAttriTable;
    typedef SkillAttriTable::iterator     SkillIter;
    typedef std::map<long,std::map<string,short> >  MapAtrTable;
    typedef std::map<long,MapAtrTable>              MapTable;

public:
    SkillAttribute();
    ~SkillAttribute();

    bool  Load( );

    void  ConfigEnCode(DBWriteSet&  dbWrite);

    void  ConfigDecode(DBReadSet& dbRead);

    long  GetStrCondif(long skillid,long lvl,std::string str,long Dis=0);

    bool  IsExist(long skillid,long lvl=0);

private:
    bool  LoadEx(const char* filename);

private:
    SkillAttriTable                       m_SkillAttriTable;

    MapTable                              m_MapTable;

};