#include "StdAfx.h"
#include "SkillAttriSetup.h"

SkillAttribute::SkillAttribute()
{
    m_SkillAttriTable.clear();
}

SkillAttribute::~SkillAttribute()
{
    m_SkillAttriTable.clear();
}

bool  SkillAttribute::Load()
{
    m_SkillAttriTable.clear();

   if(LoadEx("Data/SkillBaseAttribute.ini") && LoadEx("Data/SkillStatesAttribute.ini") )
       return true;

   return false;
}

bool  SkillAttribute::LoadEx(const char* filename)
{
    CRFile* prfile = rfOpen(filename);
    if(prfile == NULL)
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_31", filename));
        return false;
    }
    stringstream stream;
    prfile->ReadToStream(stream);
    rfClose(prfile);

    while(ReadTo(stream, "#"))
    {
        tagSkillAttr stSkillAttr;
        long         Skillid;
        long         Level;

        stream >> Skillid
            >> Level
            >> stSkillAttr.TempId           ///< 读取当前等级关联tempid
            >> stSkillAttr.MinAtkDistance   ///< 最小距离
            >> stSkillAttr.MaxAtkDistance   ///< 最大距离
            >> stSkillAttr.BuffPower        ///< 攻击强度 
            >> stSkillAttr.IntonateTime     ///< 吟唱时间
            >> stSkillAttr.CoolDown         ///< 冷却时间
            >> stSkillAttr.ActTime          ///< 释放动作时间
            >> stSkillAttr.HitTime          ///< 释放和命中时间
            >> stSkillAttr.FlyDelay         ///< 飞行延迟时间
            >> stSkillAttr.LoopCount        ///< 循环时间
            >> stSkillAttr.HurtFactor       ///< 伤害系数
            >> stSkillAttr.HealFactor       ///< 治疗系数
            >> stSkillAttr.BaseHurt         ///< 固定伤害
            >> stSkillAttr.BaseHeal         ///< 固定治疗
            >> stSkillAttr.EHit             ///< 附加攻击
            >> stSkillAttr.EHeal            ///< 附加治疗
            >> stSkillAttr.EHurtValue       ///< 附加伤害值
            >> stSkillAttr.EHealValue       ///< 附加治疗值
            >> stSkillAttr.SEnergy          ///< 内力
            >> stSkillAttr.SMp              ///< 法力
            >> stSkillAttr.SHp              ///< 气血
            >> stSkillAttr.SStrength        ///< 力量
            >> stSkillAttr.SDexterity       ///< 身法
            >> stSkillAttr.SCon             ///< 根骨
            >> stSkillAttr.SIntellect       ///< 意志
            >> stSkillAttr.SSpiritualism    ///< 灵性
            >> stSkillAttr.TaoismLvl        ///< 有效道法等级
            >> stSkillAttr.OutEnergyPower   ///< 外功强度
            >> stSkillAttr.OutEnergyHurt    ///< 外功伤害
            >> stSkillAttr.EEnergyHit       ///< 内功伤害加成
            >> stSkillAttr.EEnergyHeal      ///< 内功治疗加成
            >> stSkillAttr.OutEnergyHitLvl  ///< 外功命中等级
            >> stSkillAttr.EnergyHitLvl     ///< 内功命中等级
            >> stSkillAttr.OutEnergyCriRatio///< 外功致命等级
            >> stSkillAttr.EnergyCriRatio   ///< 内功致命等级
            >> stSkillAttr.OutEnergyCri     ///< 外功致命伤害
            >> stSkillAttr.EnergyCri        ///< 内功致命伤害
            >> stSkillAttr.OutEnergySpeed   ///< 外功加速等级
            >> stSkillAttr.EnergySpeed      ///< 内功加速等级
            >> stSkillAttr.IgnoreTarget     ///< 熟练等级
            >> stSkillAttr.ArmorDef         ///< 护甲值
            >> stSkillAttr.DefLvl           ///< 防御等级
            >> stSkillAttr.ParryLvl         ///< 招架等级
            >> stSkillAttr.ToughnessLvl     ///< 韧性等级
            >> stSkillAttr.BrokenArmorLvl   ///< 破甲等级
            >> stSkillAttr.MissRatio        ///< 失误
            >> stSkillAttr.TaoismHurt       ///< 道法伤害
            >> stSkillAttr.TaoismHitLvl     ///< 道法命中等级
            >> stSkillAttr.TaoismHitRatio   ///< 道法致命等级
            >> stSkillAttr.TaoismSpeed      ///< 道法加速等级
            >> stSkillAttr.TaoismCri        ///< 道法致命伤害
            >> stSkillAttr.TaosimRiftLvl    ///< 道法穿透等级
            >> stSkillAttr.TaosimDefRatio   ///< 道法防御等级
            >> stSkillAttr.ERation          ///< 几率变量修正
            >> stSkillAttr.EArmorHp         ///< 护盾生命修正
            >> stSkillAttr.AttackSpeed      ///< 攻击速度
            >> stSkillAttr.MoveSpeed        ///< 移动速度
            >> stSkillAttr.ReadyTime        ///< 准备时间
            >> stSkillAttr.GridFlyTime      ///< 每格飞行时间 
            >> stSkillAttr.RangeTimes       ///< 作用范围目标次数
            >> stSkillAttr.RangeCount       ///< 作用范围目标个数
            >> stSkillAttr.BufId1           ///< 读取附加状 态
            >> stSkillAttr.BufLvl1
            >> stSkillAttr.BufNum1
            >> stSkillAttr.BufId2
            >> stSkillAttr.BufLvl2
            >> stSkillAttr.BufNum2
            >> stSkillAttr.BufId3
            >> stSkillAttr.BufLvl3
            >> stSkillAttr.BufNum3
            >> stSkillAttr.BufId4
            >> stSkillAttr.BufLvl4
            >> stSkillAttr.BufNum4
            >> stSkillAttr.BufId5
            >> stSkillAttr.BufLvl5
            >> stSkillAttr.BufNum5
            >> stSkillAttr.BufId6
            >> stSkillAttr.BufLvl6
            >> stSkillAttr.BufNum6;


        SkillIter itr = m_SkillAttriTable.find(Skillid);
        if( itr != m_SkillAttriTable.end() )
        {
            SkillTable::iterator litr =  itr->second.find( Level );
            if ( litr != itr->second.end() )
            {
                Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_32", filename,Skillid));
                return false;
            }
        }

        m_SkillAttriTable[ Skillid ][ Level ] = stSkillAttr;
    }
    return true;
}

void  SkillAttribute::ConfigEnCode(DBWriteSet& dbWrite)
{
     dbWrite.AddToByteArray( (long) m_SkillAttriTable.size() );

     for ( SkillIter Iter( m_SkillAttriTable.begin()) ; Iter != m_SkillAttriTable.end() ; Iter ++ )
     {
         /// skill id
         dbWrite.AddToByteArray( Iter->first );
         dbWrite.AddToByteArray( (long)Iter->second.size() );
         for ( SkillTable::iterator it = Iter->second.begin() ; it != Iter->second.end() ; it++)
         {                
             /// add level
             dbWrite.AddToByteArray( it->first );
             dbWrite.AddToByteArray( &it->second, sizeof(tagSkillAttr) );
         }
     }
}

void  SkillAttribute::ConfigDecode(DBReadSet& dbRead)
{
     m_MapTable.clear();

     long  SkillNum = dbRead.GetLongFromByteArray();

     for ( int i = 0 ; i < SkillNum ; i++ )
     {
         long Skillid = dbRead.GetLongFromByteArray();
         long LevelNum= dbRead.GetLongFromByteArray();
         for ( int j = 0 ; j < LevelNum ; j++ )
         {
             long lvl = dbRead.GetLongFromByteArray();
             tagSkillAttr  tmpSkillAttr;
             dbRead.GetBufferFromByteArray(&tmpSkillAttr,sizeof(tagSkillAttr) );

             m_SkillAttriTable[ Skillid ][ lvl ] = tmpSkillAttr;
             std::map<string,short>  mp;
             tmpSkillAttr.AddMapTable( mp );

             m_MapTable[ Skillid ][ lvl ] = mp;
         }
     }
}

long SkillAttribute::GetStrCondif(long skillid,long lvl,std::string str,long Dis)
{
    if ( str == "SkillLvl" )
    {
        return lvl;
    }

    MapTable::iterator IdItr = m_MapTable.find( skillid );
    if ( IdItr != m_MapTable.end() )
    {
        MapAtrTable::iterator LvlItr = IdItr->second.find( lvl );
        if ( LvlItr != IdItr->second.end() )
        {
            if ( str == "FutureTime" )
            {
                 return  ( LvlItr->second["ReadyTime"] +  Dis   * LvlItr->second["GridFlyTime"] );
            }
            return LvlItr->second[str];
        }
    }

    return 0;
}

bool  SkillAttribute::IsExist(long skillid,long lvl)
{
    MapTable::iterator IdItr = m_MapTable.find( skillid );
    if ( IdItr != m_MapTable.end() )
    {
        MapAtrTable::iterator LvlItr = IdItr->second.find( lvl );
        if ( LvlItr != IdItr->second.end() )
        {
            return true;
        }
    }
    
    Log4c::Trace(ROOT_MODULE,"Skllid %d is not Exist!", skillid );

    return false;            
}