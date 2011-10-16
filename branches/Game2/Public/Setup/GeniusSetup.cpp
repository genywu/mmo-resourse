#include "StdAfx.h"
#include "GeniusSetup.h"


GeniusSetup::GeniusSetup()
{
    m_GeniusConfig.clear();
    m_BaseNum = 1000;
}

GeniusSetup::~GeniusSetup()
{
    m_GeniusConfig.clear();
    m_filelist.clear();
}

bool   GeniusSetup::Load(const char* filename)
{
    if ( !LoadList( filename) )
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_31", filename));
        return false;
    }

    LoadSkillPro();

    for ( size_t i = 0 ; i < m_filelist.size() ; i++ )
    {
        TiXmlDocument T_dox(m_filelist[i].c_str());
        TiXmlDocument* m_Tdoc = & T_dox; 
        //打开文件	
        if (!T_dox.LoadFile())									
        {
            return  false; 
        }
        TiXmlNode *pNode =m_Tdoc->FirstChild("Genius");
        TiXmlElement* te=pNode->ToElement();//获取node 的指针
        if (te==NULL)
        {
            return  false;
        }
        TiXmlElement* p=te->FirstChildElement();
        for (; p!=NULL; p=p->NextSiblingElement())
        {
            if (!strcmp(p->Value(),"GeniusId"))
            {
                tagGenius   tempGenius;
                tempGenius.GeniusPro.id = atol( p->Attribute("id"));
                tempGenius.GeniusPro.skillNum = atol( p->Attribute("SkillNum"));
                tempGenius.GeniusPro.maxLvl = atol( p->Attribute("MaxLev"));
                strcpy(tempGenius.GeniusPro.desid,p->Attribute("DesId"));

                /// 读取等级下配置
                TiXmlElement* Level = p->FirstChildElement("Level");
                for ( ; Level != NULL ; Level = Level->NextSiblingElement() )
                {   
                    long lev =  atol( Level->Attribute("id") );

                    tagGeniusLvl   tmpGenLvl;
                    /// 读取当前等级下技能配置
                    TiXmlElement* SkillParam = Level->FirstChildElement("SkillParam");
                    for ( ; SkillParam != NULL ; SkillParam = SkillParam->NextSiblingElement() )
                    {
                        long Skillid =  atol( SkillParam->Attribute("id") );

                        tagGeniusSkill  tmpGeniusSkill;
                        tmpGeniusSkill.id = Skillid;  

                        /// 读取buffer
                        TiXmlElement* AnnexBuff = SkillParam->FirstChildElement("AnnexBuff");
                        if ( AnnexBuff )
                        {
                            long AnnexSum =  atol( AnnexBuff->Attribute("id") );
                            tmpGeniusSkill.AnnexSum = AnnexSum;

                            /// 读取配置
                            TiXmlElement* Annex = AnnexBuff->FirstChildElement("Annex");
                            for ( ; Annex!=NULL ; Annex=Annex->NextSiblingElement() )
                            {
                                tagGeniusBuff GeniusBuff;
                                GeniusBuff.effobj = ForMatEffObj( Annex->Attribute("EffObj"));
                                GeniusBuff.AnnexId =  atol( Annex->Attribute("AnnexId") );
                                GeniusBuff.Level =  atol( Annex->Attribute("AnnexLvl") );
                                GeniusBuff.AnnexNum=atol( Annex->Attribute("AnnexNum"));
                                GeniusBuff.objType = ForMatObjType( Annex->Attribute("ObjType"));
                                tmpGeniusSkill.GeniusBuff.push_back( GeniusBuff );
                            }
                        }

                        /// 读取Skill
                        TiXmlElement* skill = SkillParam->FirstChildElement("skill");
                        for ( ; skill != NULL ; skill = skill->NextSiblingElement("skill") )
                        {
                            tagSkillEffPro  tmpSkillPro;
                            tmpSkillPro.eff = ForMatOper( skill->Attribute("Oper"));
                            strcpy(tmpSkillPro.type,skill->Attribute("Type"));
                            tmpSkillPro.value =  atol( skill->Attribute("Value") );
                            tmpGeniusSkill.SkillPro.push_back( tmpSkillPro );
                        }

                        tmpGenLvl.GeniusSkill[ Skillid  ] =   tmpGeniusSkill;
                    } 

                    tempGenius.GeniusConfig[ lev ] =  tmpGenLvl;
                }

                m_GeniusConfig[ tempGenius.GeniusPro.id  ] = tempGenius;
            }
        }
    }
    return true;
}

bool  GeniusSetup::LoadSkillPro()
{
    m_UpdateSkillProTable.clear();

    CRFile* prfile = rfOpen(m_UpdateSkillPro.c_str());
    if(prfile == NULL)
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_31", m_UpdateSkillPro));
        return false;
    }
    stringstream stream;
    prfile->ReadToStream(stream);
    rfClose(prfile);

    string  SkillPro;
    while(ReadTo(stream, "#"))
    {
          stream >> SkillPro ;

          m_UpdateSkillProTable.insert( SkillPro );
    }

    return true;
}

bool  GeniusSetup::LoadList(const char* filename)
{
    m_filelist.clear();
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 
    //打开文件	
    if (!T_dox.LoadFile())									
    {
        return  false; 
    }
    TiXmlNode *pNode =m_Tdoc->FirstChild("GeniusList");
    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        return  false;
    }
    m_BaseNum = atol( te->Attribute("BaseNum") );
    m_UpdateSkillPro = te->Attribute("UpdateSkillPro");
    TiXmlElement* p=te->FirstChildElement();
    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"file"))
        {
            m_filelist.push_back( p->Attribute("id") );
        }
    }
    return true;
}

EffObj  GeniusSetup::ForMatEffObj(string str)
{
    if ( str == "Obj_Self")
    {
        return  Obj_ESelf;
    }
    return   Obj_EDest;
}

ObjType   GeniusSetup::ForMatObjType(string str)
{
    if ( str == "Obj_Monster")
    {
        return  Obj_Monster;
    }
    else if ( str == "Obj_Player")
    {
        return  Obj_Player;
    }
    else if ( str == "Obj_Pet")
    {
        return  Obj_Pet;
    }
    else if ( str == "Obj_Monster_Player")
    {
        return  Obj_Monster_Player;
    }
    else if ( str == "Obj_Monster_Pet")
    {
        return  Obj_Monster_Pet;
    }
    else if ( str == "Obj_Player_Pet")
    {
        return  Obj_Player_Pet;
    }
    else if ( str == "Obj_ALlObj")
    {
        return Obj_AllObj;
    }                     
    return   Obj_Invalid;
}

GeniusOper GeniusSetup::ForMatOper(string str)
{
    if ( str == "Add")
    {
        return  Add_Type;
    }
    else if ( str == "Sub")
    {
        return  Sub_Type;
    }
    else if ( str == "Multi_S")
    {
        return  Multi_Type_S;
    }
    else if ( str == "Multi_E")
    {
        return  Multi_Type_E;
    }
    return   No_Type;
}

void   GeniusSetup::GeniusConfigEnCode(DBWriteSet& dbWrite)
{
     dbWrite.AddToByteArray( (long)m_UpdateSkillProTable.size());

     for ( std::set<string>::iterator proitr = m_UpdateSkillProTable.begin() ; proitr != m_UpdateSkillProTable.end() ; proitr ++ )
     {
         dbWrite.AddToByteArray( (*proitr).c_str() );
     }

     dbWrite.AddToByteArray( m_BaseNum );
     dbWrite.AddToByteArray( (long) m_GeniusConfig.size() );

     std::map<long,tagGenius>::iterator  ConfigItr = m_GeniusConfig.begin();
     for ( ; ConfigItr != m_GeniusConfig.end() ; ConfigItr ++ )
     {
         dbWrite.AddToByteArray( ConfigItr->first );
         dbWrite.AddToByteArray( &ConfigItr->second.GeniusPro , sizeof( tagGeniusPro ) );
         
         dbWrite.AddToByteArray( (long)ConfigItr->second.GeniusConfig.size() );
         std::map<long,tagGeniusLvl>::iterator  LvlItr = ConfigItr->second.GeniusConfig.begin();
         for ( ; LvlItr != ConfigItr->second.GeniusConfig.end() ; LvlItr ++ )
         {
             /// 等级
             dbWrite.AddToByteArray( LvlItr->first );

             dbWrite.AddToByteArray( (long)LvlItr->second.GeniusSkill.size() );
             std::map<long,tagGeniusSkill>::iterator  skillItr = LvlItr->second.GeniusSkill.begin(); 
             for ( ; skillItr != LvlItr->second.GeniusSkill.end(); skillItr ++ )
             {
                   /// 技能id
                   dbWrite.AddToByteArray( skillItr->first );
                   
                   /// 具体技能配置影响属性
                   dbWrite.AddToByteArray( skillItr->second.AnnexSum );
                   dbWrite.AddToByteArray( (long)skillItr->second.GeniusBuff.size() );

                   for ( size_t j = 0 ; j < skillItr->second.GeniusBuff.size(); j++ )
                   {
                       dbWrite.AddToByteArray( &skillItr->second.GeniusBuff[j] , sizeof(tagGeniusBuff) );
                   }

                   size_t pSize = skillItr->second.SkillPro.size();
                   dbWrite.AddToByteArray((long)pSize);
                   for ( size_t j = 0 ; j < pSize ; j++ )
                   {
                       dbWrite.AddToByteArray( &skillItr->second.SkillPro[j] , sizeof(tagSkillEffPro) );
                   }
             }
         }
     }
}

void   GeniusSetup::GeniusConfigDecode(DBReadSet& dbRead)
{
      m_UpdateSkillProTable.clear();
      m_GeniusConfig.clear();

      long  proNum = dbRead.GetLongFromByteArray() ;

      for ( int i = 0 ; i <proNum ; i++ )
      {
          char pStr[128];
          dbRead.GetStringFromByteArray(pStr,128);

          m_UpdateSkillProTable.insert( pStr );
      }
      m_BaseNum  = dbRead.GetLongFromByteArray();
      long  size = dbRead.GetLongFromByteArray();

      for (  int i = 0 ; i < size ; i++ )
      {
          tagGenius tempGenius;

          /// 得到天赋Id
          long  id = dbRead.GetLongFromByteArray();
          dbRead.GetBufferFromByteArray( &tempGenius.GeniusPro , sizeof(tagGeniusPro) );

          long  LvlNum =  dbRead.GetLongFromByteArray();
          for ( int j = 0 ; j < LvlNum ; j++ )
          {
              tagGeniusLvl Genlvl;
              long  lvl = dbRead.GetLongFromByteArray();
              long  SkillNum = dbRead.GetLongFromByteArray();

              for ( int k = 0 ; k < SkillNum ; k++ )
              {
                   tagGeniusSkill  SkillParam;
                   SkillParam.id = dbRead.GetLongFromByteArray();
                   SkillParam.AnnexSum = dbRead.GetLongFromByteArray();
                   
                   int  Num = dbRead.GetLongFromByteArray();
                   for ( int m = 0 ; m < Num ; m++ )
                   {
                       tagGeniusBuff buff;
                       dbRead.GetBufferFromByteArray( &buff , sizeof(tagGeniusBuff) );
                       SkillParam.GeniusBuff.push_back( buff );
                   }

                   Num = dbRead.GetLongFromByteArray();
                   for ( int m = 0 ; m < Num ; m++ )
                   {
                       tagSkillEffPro Pro;
                       dbRead.GetBufferFromByteArray( &Pro , sizeof(tagSkillEffPro) );
                       SkillParam.SkillPro.push_back( Pro );
                   }
                   Genlvl.GeniusSkill[ SkillParam.id ] = SkillParam;

                   m_SkillTable[ SkillParam.id ][ id ].push_back( lvl );
                  
              }
              tempGenius.GeniusConfig[ lvl ] = Genlvl;
          }
         m_GeniusConfig[ id ] =  tempGenius ;
      }
}

std::map<long,vector<long> >  GeniusSetup::GetGeniusList(long skillid)
{
    return m_SkillTable[ skillid ];
}

tagGeniusSkill*   GeniusSetup::GetGeniusVec(long id,long skill,long lvl)
{
    std::map<long,tagGenius>::iterator itr = m_GeniusConfig.find( id );
    if ( itr != m_GeniusConfig.end() )
    {
        std::map<long,tagGeniusLvl>::iterator it =  itr->second.GeniusConfig.find( lvl );
        if ( it != itr->second.GeniusConfig.end() )
        {
            std::map<long,tagGeniusSkill>::iterator skillitr = it->second.GeniusSkill.find(skill);

            if ( skillitr != it->second.GeniusSkill.end() )
            {
                return &(skillitr->second);
            }
        }
    }
    return NULL;
}

void GeniusSetup::GetSkillList(long geniusid,long lvl,std::map<long,long>&  mp)
{
    std::map<long,tagGenius>::iterator itr = m_GeniusConfig.find( geniusid );
    if ( itr != m_GeniusConfig.end() )
    {
        std::map<long,tagGeniusLvl>::iterator it =  itr->second.GeniusConfig.find( lvl );
        if ( it != itr->second.GeniusConfig.end() )
        {
            std::map<long,tagGeniusSkill>::iterator skillitr = it->second.GeniusSkill.begin() ;

            for( ; skillitr != it->second.GeniusSkill.end(); skillitr ++ )
            {
                mp[ skillitr->first ] = 1;
            }
        }
    }
    return ;
}

bool  GeniusSetup::FindGenius(long Geniusid)
{
    std::map<long,tagGenius>::iterator itr = m_GeniusConfig.find( Geniusid );
    if ( itr != m_GeniusConfig.end() )
        return  true;
    return false;
}

bool  GeniusSetup::IsSkillUpdatePro(std::string pro)
{
    std::set<string>::iterator  Itr = m_UpdateSkillProTable.find( pro );

    if (  Itr != m_UpdateSkillProTable.end() )
    {
        return true;
    }
    return false;
}