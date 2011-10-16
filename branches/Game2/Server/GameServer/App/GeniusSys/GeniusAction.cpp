#include "StdAfx.h"
#include "GeniusAction.h"
#include "GeniusSys.h"
#include "../Player.h"
#include "../../../../Public/Setup/GeniusSetup.h"
#include "../../../../Public/Setup/SkillAttriSetup.h"

GeniusAction::GeniusAction(CPlayer* player)
{
    m_pOwner = player;
    m_GeniusList.clear();
}

GeniusAction::~GeniusAction()
{
    std::map<long,GeniusSys*>::iterator  itr = m_GeniusList.begin();
    for ( ; itr != m_GeniusList.end() ; itr ++ )
    {
        if( itr->second )
            delete itr->second;
    }                    
    m_GeniusList.clear();
}

void  GeniusAction::Update(UpdateEvent Event,long id, long Lvl )
{
    /// 更新技能
    if (  Event == Skill_Update )
    {
        GeniusSys* pSys = NULL;
        std::map<long,GeniusSys*>::iterator  itr = m_GeniusList.find( id );
        if ( itr != m_GeniusList.end() )
        {
            itr->second->Update(Event,id,Lvl);
            pSys = itr->second;
        }
        else
        {
             GeniusSys* tmpGenius = new GeniusSys( id , Lvl , m_pOwner );
             m_GeniusList [ id ] = tmpGenius;
             tmpGenius->Update( Event , id , Lvl );
             pSys = tmpGenius;
        }                     
        if ( pSys )
            pSys->CodeDateToClient();
    }
    /// 天赋更新
    else
    {
         long lvl = m_pOwner->GetGeniusLvl( id );
         if( lvl <= 0 )
             return ;

         std::map<long,long> skillmp;skillmp.clear();
         typedef  std::map<long,long>::iterator  MItr;

         GetInst(GeniusSetup).GetSkillList( id, lvl, skillmp );

         for( MItr itr= skillmp.begin(); itr != skillmp.end(); itr ++ )
         {
             long lvl = m_pOwner->GetSkillLv( itr->first );
             if(  lvl > 0 )
             {
                 Update(Skill_Update,itr->first,lvl);
             }
         }
    }
}

void  GeniusAction::Remove( RemoveEvent Event,long id ,long Lvl )
{
    if (  Event == Skill_Update )
    {
        std::map<long,GeniusSys*>::iterator  itr = m_GeniusList.find( id );
        if ( itr != m_GeniusList.end() )
        {
            delete itr->second;
            m_GeniusList.erase( itr );
        }
    }
    else
    {
        std::map<long,long> skillmp;skillmp.clear();
        typedef  std::map<long,long>::iterator  MItr;

        GetInst(GeniusSetup).GetSkillList( id, Lvl, skillmp );
     
        for( MItr itr= skillmp.begin(); itr != skillmp.end(); itr ++ )
        {
            long lvl = m_pOwner->GetSkillLv( itr->first );
            if(  lvl > 0 )
            {
                Update(Skill_Update,itr->first,lvl);
            }
        }
    }

}

void  GeniusAction::Update()
{
    std::map<long,long> mp = m_pOwner->GetGenius();
    std::map<long,long> skillmp;skillmp.clear();

    typedef  std::map<long,long>::iterator  MItr;

    for( MItr itr= mp.begin(); itr != mp.end(); itr ++ )
    {
       GetInst(GeniusSetup).GetSkillList( itr->first, itr->second , skillmp );
    }

    for( MItr itr= skillmp.begin(); itr != skillmp.end(); itr ++ )
    {
        long lvl = m_pOwner->GetSkillLv( itr->first );
        if(  lvl > 0 )
        {
            Update(Skill_Update,itr->first,lvl);
        }
    }
}

void GeniusAction::AddGenius(long id , long lvl)
{
     if( GetInst(GeniusSetup).FindGenius(id) )
     {
         m_pOwner->SetGeniusLvl(id,lvl);

         Update( Genius_Update , id , lvl );         
     }
}

void  GeniusAction::AddStates(CMoveShape* pDest,tagGeniusBuff& buf)
{
    CMoveShape * pAnnex = pDest;
    if ( buf.effobj == Obj_ESelf )
        pDest = m_pOwner;

    /// 利用位运算减少冗余代码的判断
    long  nNum = pDest->GetType();
    /// nNum &( nnNum ^(nNum-1) ) -1 
    long  temp = nNum & 0x4 ? 0x02 :( (nNum & 0x8) ? 0x04 : 0x08 );

    if ( buf.objType & temp )
    {
        for ( long k = 0 ; k < buf.AnnexNum ; k++ )
            m_pOwner->AddNewState( pDest,buf.AnnexId,buf.Level,11);
    }
}

void  GeniusAction::AddGeniusStates(long skillid,CMoveShape* pDest,long Gid)
{
    /// 找到影响技能id的天赋配置表
    std::vector<tagGeniusBuff>   VecGeniusBuf;VecGeniusBuf.clear();
    if( Gid == 0 )
    {
        std::map<long,vector<long> >  mp = GetInst(GeniusSetup).GetGeniusList( skillid );
        /// 遍历每一个天赋，查看玩家是否学习了，然后如果学习了提取影响属性配置
        for ( std::map<long,vector<long> >::iterator it = mp.begin(); it != mp.end() ; it ++ )
        {
            long  geniusid = it->first;
            /// 查看天赋属性
            long  lev = m_pOwner->GetGeniusLvl( geniusid );
            if ( !m_pOwner ||  lev <=0 )
            {
                continue;
            } 
            tagGeniusSkill*  GeniusSkill = GetInst(GeniusSetup).GetGeniusVec(geniusid,skillid,lev);
            if( GeniusSkill == NULL )
                continue;
            std::vector<tagGeniusBuff>& tmpBuf = GeniusSkill->GeniusBuff;
            VecGeniusBuf.insert(VecGeniusBuf.begin(),tmpBuf.begin(),tmpBuf.end());
        }
    }
    else
    {
        /// 查看天赋属性
        long  lev = m_pOwner->GetGeniusLvl( Gid );
        tagGeniusSkill*  GeniusSkill = GetInst(GeniusSetup).GetGeniusVec(Gid,skillid,lev);
        if( GeniusSkill == NULL )
            return;
        std::vector<tagGeniusBuff>& tmpBuf = GeniusSkill->GeniusBuff;
        VecGeniusBuf.insert(VecGeniusBuf.begin(),tmpBuf.begin(),tmpBuf.end());
    }
    
    if ( VecGeniusBuf.empty() )
        return ;

    for ( size_t j = 0 ; j < VecGeniusBuf.size() ; j++ )
    {
        AddStates( pDest , VecGeniusBuf[j] );
    }

}

long  GeniusAction::GetSkillAttri(long skillid,long level,std::string name,long xPos,long yPos)
{
    //long  lvl = m_pOwner->GetSkillLv( skillid );
    if( level <= 0 /*|| level != lvl */) return 0;
     
    /// 飞行时间单独处理
    if (name=="FutureTime")
    {
        if( !m_pOwner ) return 0;

        long Dis = m_pOwner->RealDistance( xPos,yPos );
        return  ( GetSkillAttri(skillid,level,"ReadyTime") +  Dis* GetSkillAttri(skillid,level,"GridFlyTime") ) ;
    }

    std::map<long,GeniusSys*>::iterator  itr =  m_GeniusList.find( skillid );
    if ( itr != m_GeniusList.end() )
    {
       return itr->second->GetSkillAttri(level,name);
    }
    return GetInst(SkillAttribute).GetStrCondif(skillid,level,name);
}

void  GeniusAction::AddSkill(long id,long level)
{
    std::map<long,GeniusSys*>::iterator  itr = m_GeniusList.find( id );
    if ( itr != m_GeniusList.end() )
    {
        itr->second->Update(Skill_Update,id,level);
        itr->second->CodeDateToClient();
    }
    else
    {
        /// 找到影响技能id的天赋配置表
        std::map<long,vector<long> >  mp = GetInst(GeniusSetup).GetGeniusList( id );
        std::vector<tagGeniusSkill>   GeniusVec;GeniusVec.clear();
        /// 遍历每一个天赋，查看玩家是否学习了，然后如果学习了提取影响属性配置
        for ( std::map<long,vector<long> >::iterator it = mp.begin(); it != mp.end() ; it ++ )
        {
            long  geniusid = it->first;
            /// 查看天赋属性
            long  lev = m_pOwner->GetGeniusLvl( geniusid );
            if ( !m_pOwner ||  lev <=0 )
            {
                continue;
            }
            GeniusVec.push_back( *GetInst(GeniusSetup).GetGeniusVec(geniusid,id,lev) );
        }

        if ( GeniusVec.empty() )
            return ;

        GeniusSys* tmpGenius = new GeniusSys( id , level , m_pOwner );
        m_GeniusList [ id ] = tmpGenius;
        tmpGenius->AddSkillPro(GeniusVec,level);
        tmpGenius->CodeDateToClient();
    } 
}

void  GeniusAction::CodeGeniusToDataBlock(DBWriteSet& setWriteDB, BOOL bAddRestoreTime)
{
    std::map<long,GeniusSys*>::iterator  itr = m_GeniusList.begin();

    setWriteDB.AddToByteArray( (long) m_GeniusList.size() );

    for ( ; itr != m_GeniusList.end() ; itr++ )
    {
        itr->second->CodeGeniusToDataBlock(setWriteDB);
    }
}