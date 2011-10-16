#include "StdAfx.h"
#include "GeniusSys.h"
#include "../Player.h"
#include "../../../../Public/Setup/GeniusSetup.h"
#include "../../../../Public/Setup/SkillAttriSetup.h"

#define  OperMulti( Mp,name,temp,value)\
        Mp[name] =temp*value/GetInst(GeniusSetup).GetGeniusBase()
#define  OperAdd(Mp,name,temp,value)\
        Mp[name] =temp+value
#define  OperSub(Mp,name,temp,value)\
        Mp[name] =temp-value


GeniusSysOper::GeniusSysOper()
{
    m_OperTable[Add_Type] = &AddMpPolicy;
    m_OperTable[Sub_Type] = &SubMpPolicy;
    m_OperTable[Multi_Type_S] = &MultiMpPolicy;
    m_OperTable[Multi_Type_E] = &MultiMpPolicy;

    m_BaseNum = GetInst(GeniusSetup).GetGeniusBase();
    if( m_BaseNum <= 0 )
        m_BaseNum = BASEGENIUSNUM;
}


GeniusSysOper::~GeniusSysOper()
{
    m_OperTable.clear();
}

void GeniusSysOper::GeniusPolicy(SkillPro&  Attri,
                            AttriMap&  Mp,
                            long id,
                            long lvl)
{
    for( size_t i = 0 ; i < Attri.size() ; i++ )
    {
        OperFun::iterator itr = m_OperTable.find( Attri[i].eff );
        if ( itr != m_OperTable.end() )
        {
            long    temp;
            string  name = Attri[i].type;
            AttriMap::iterator MItr = Mp.find( name );
            if(  MItr != Mp.end() )
            {
                temp = MItr->second;
            }
            else
            {
                temp = GetInst(SkillAttribute).GetStrCondif(id,lvl,name);
            }
            if( *itr->second != NULL)
                (*itr->second)(Mp,name,temp,Attri[i].value);
        }
    }
}

GeniusSys::GeniusSys(long skillid,long Lvl,CPlayer * player)
{
    m_Skillid = skillid;
    m_SkillLvl= Lvl;
    m_SkillMap.clear();
    m_pOwner = player;
}

GeniusSys::~GeniusSys()
{
    m_SkillMap.clear();
}

void  GeniusSys::Update(UpdateEvent Event,long id , long Lvl )
{
    if ( Event == Skill_Update )
    {
        m_SkillMap.clear();

        std::map<long,vector<long> >  mp = GetInst(GeniusSetup).GetGeniusList( id );
        
        /// 找到影响的天赋配置
        std::vector<tagGeniusSkill>   GeniusVec;
        for ( std::map<long,vector<long> >::iterator it = mp.begin(); it != mp.end() ; it ++ )
        {
           long  geniusid = it->first;
           vector<long> vec = it->second;
           /// 查看天赋属性
           long  lev = m_pOwner->GetGeniusLvl( geniusid);
           if ( !m_pOwner ||  lev <= 0)
           {
               continue;
           }
           GeniusVec.push_back( *GetInst(GeniusSetup).GetGeniusVec(geniusid,id,lev) );
        }

        if ( GeniusVec.empty() )
            return ;

        std::vector<tagSkillEffPro>  MulitS;
        std::vector<tagSkillEffPro>  AddSub;
        std::vector<tagSkillEffPro>  MulitE;

        Assign(GeniusVec,MulitS,AddSub,MulitE);
     
        SKILLGNOPER.GeniusPolicy(MulitS,m_SkillMap,m_Skillid,Lvl);
        SKILLGNOPER.GeniusPolicy(AddSub,m_SkillMap,m_Skillid,Lvl);
        SKILLGNOPER.GeniusPolicy(MulitE,m_SkillMap,m_Skillid,Lvl);
    }
}

long  GeniusSys::GetSkillAttri(long lvl , std::string  name)
{
    AttriMap::iterator itr = m_SkillMap.find(name);
    if ( itr != m_SkillMap.end() )
    {
        return itr->second;
    }

    return  GetInst(SkillAttribute).GetStrCondif(m_Skillid,lvl,name);
}

void  GeniusSys::AddSkillPro(std::vector<tagGeniusSkill>& GeniusVec,long Lvl)
{
    m_SkillMap.clear();

    std::vector<tagSkillEffPro>  MulitS;
    std::vector<tagSkillEffPro>  AddSub;
    std::vector<tagSkillEffPro>  MulitE;

    Assign(GeniusVec,MulitS,AddSub,MulitE);

    SKILLGNOPER.GeniusPolicy(MulitS,m_SkillMap,m_Skillid,Lvl);
    SKILLGNOPER.GeniusPolicy(AddSub,m_SkillMap,m_Skillid,Lvl);
    SKILLGNOPER.GeniusPolicy(MulitE,m_SkillMap,m_Skillid,Lvl);
}

void   GeniusSys::CalcUpdateSkillPro()
{
    m_UpdateAttr.clear();

    AttriMap::iterator itr =  m_SkillMap.begin();
    for ( ; itr != m_SkillMap.end() ; itr ++)
    {
        if( GetInst(GeniusSetup).IsSkillUpdatePro( itr->first ) )
            m_UpdateAttr[ itr->first ] = itr->second ;
    }
}

void   GeniusSys::CodeGeniusToDataBlock(DBWriteSet& setWriteDB)
{
    CalcUpdateSkillPro();

    setWriteDB.AddToByteArray( m_Skillid );
    setWriteDB.AddToByteArray( m_SkillLvl);
    setWriteDB.AddToByteArray( (long)m_UpdateAttr.size() );

    for (AttriMap::iterator itr =  m_UpdateAttr.begin() ; itr != m_UpdateAttr.end() ; itr ++ )
    {
        setWriteDB.AddToByteArray( itr->first.c_str() );
        setWriteDB.AddToByteArray( itr->second );
    }
}

void   GeniusSys::CodeDateToClient()
{
     CMessage  msg(MSG_S2C_GENIUS_SKILLUPDATE);

     msg.Add( m_Skillid );
     msg.Add( m_SkillLvl);

     msg.Add( (long)m_UpdateAttr.size() );

     for (AttriMap::iterator itr =  m_SkillMap.begin() ; itr != m_SkillMap.end() ; itr ++ )
     {
         msg.Add( itr->first.c_str() );
         msg.Add( itr->second );
     }

     msg.SendToPlayer( m_pOwner->GetExID() );
}

void   GeniusSys::Assign(std::vector<tagGeniusSkill>& GeniusVec,
                         std::vector<tagSkillEffPro>& MultiS,
                         std::vector<tagSkillEffPro>& AddSub,
                         std::vector<tagSkillEffPro>& MultiE)
{
    /// 找到计算结果
    for ( size_t i = 0 ; i < GeniusVec.size() ; i++ )
    {
        for( size_t j = 0 ; j < GeniusVec[i].SkillPro.size() ; j++ )
        {
            if ( GeniusVec[i].SkillPro[j].eff == Multi_Type_S )
            {
                MultiS.push_back(  GeniusVec[i].SkillPro[j] );
            }
            else  if( GeniusVec[i].SkillPro[j].eff == Add_Type || 
                      GeniusVec[i].SkillPro[j].eff == Sub_Type) 
            {
                AddSub.push_back(  GeniusVec[i].SkillPro[j] );
            }
            else  if( GeniusVec[i].SkillPro[j].eff == Multi_Type_E)
            {
                MultiE.push_back(  GeniusVec[i].SkillPro[j] );
            }
        }
    }
}

void   AddMpPolicy(std::map<string,long>& mp,
                   string name,
                   long temp,
                   long value)
{
    mp[name] =temp+value;
}

void   SubMpPolicy(std::map<string,long>& mp,
                   string name,
                   long temp,
                   long value)
{
    mp[name] =temp-value;
}

void   MultiMpPolicy(std::map<string,
                     long>& mp,
                     string name,
                     long temp,
                     long value)
{
    mp[name] =temp*value/ SKILLGNOPER.GetBaseNum();
}