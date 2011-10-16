/** 
 *  @brief  关于技能附加属性的具体栏位定义 
 *  @author expter
 */


#pragma once 


#define  SKILLPRODISPLAY(v,s)     #v#s
    
#define  SKILLPRO_DISPLAY(v,s)    v##s

#define  BIND_SKILLPRO(mp,name,value) \
    mp[name]=value;

#define  BIND_SKILLPRO_EXED(mp,num,value) \
    mp[ SKILLPRODISPLAY(value,num) ] = SKILLPRO_DISPLAY(value,num) 

#define  BIND_SKILLPRO_STATE_EXED(mp,num)\
    BIND_SKILLPRO_EXED(mp,num,BufId);\
    BIND_SKILLPRO_EXED(mp,num,BufLvl);\
    BIND_SKILLPRO_EXED(mp,num,BufNum)  


struct tagSkillAttr 
{
    short    TempId;           ///< 当前等级关联template id
    short    MinAtkDistance;   ///< 最小攻击距离
    short    MaxAtkDistance;   ///< 最大攻击距离
    short    BuffPower;        ///< 攻击强度 
    short    IntonateTime;     ///< 吟唱时间  c
    short    CoolDown;         ///< 冷却时间  c
    short    ActTime;          ///< 释放动作时间  c
    short    HitTime;          ///< 释放和命中时间 c
    short    FlyDelay;         ///< 飞行延迟时间 c
    short    LoopCount;        ///< 循环时间 c
    short    HurtFactor;       ///< 伤害系数
    short    HealFactor;       ///< 治疗系数
    short    BaseHurt;         ///< 固定伤害
    short    BaseHeal;         ///< 固定治疗
    short    EHit;             ///< 附加攻击
    short    EHeal;            ///< 附加治疗
    short    EHurtValue;       ///< 附加伤害值
    short    EHealValue;       ///< 附加治疗值
    short    SEnergy;          ///< 内力
    short    SMp;              ///< 法力
    short    SHp;              ///< 气血
    short    SStrength;        ///< 力量
    short    SDexterity;       ///< 身法
    short    SCon ;            ///< 根骨
    short    SIntellect;       ///< 意志
    short    SSpiritualism;    ///< 灵性
    short    TaoismLvl;        ///< 有效道法等级
    short    OutEnergyPower;   ///< 外功强度
    short    OutEnergyHurt;    ///< 外功伤害
    short    EEnergyHit ;      ///< 内功伤害加成
    short    EEnergyHeal ;     ///< 内功治疗加成
    short    OutEnergyHitLvl;  ///< 外功命中等级
    short    EnergyHitLvl;     ///< 内功命中等级
    short    OutEnergyCriRatio;///< 外功致命等级
    short    EnergyCriRatio ;  ///< 内功致命等级
    short    OutEnergyCri ;    ///< 外功致命伤害
    short    EnergyCri ;       ///< 内功致命伤害
    short    OutEnergySpeed ;  ///< 外功加速等级
    short    EnergySpeed ;     ///< 内功加速等级
    short    IgnoreTarget;     ///< 熟练等级
    short    ArmorDef;         ///< 护甲值
    short    DefLvl;           ///< 防御等级
    short    ParryLvl;         ///< 招架等级
    short    ToughnessLvl;     ///< 韧性等级
    short    BrokenArmorLvl;   ///< 破甲等级
    short    MissRatio;        ///< 失误
    short    TaoismHurt;       ///< 道法伤害
    short    TaoismHitLvl;     ///< 道法命中等级
    short    TaoismHitRatio;   ///< 道法致命等级
    short    TaoismSpeed;      ///< 道法加速等级
    short    TaoismCri;        ///< 道法致命伤害
    short    TaosimRiftLvl;    ///< 道法穿透等级
    short    TaosimDefRatio;   ///< 道法防御等级
    short    ERation;          ///< 几率变量修正
    short    EArmorHp;         ///< 护盾生命修正
    short    AttackSpeed;      ///< 攻击速度   c maybe
    short    MoveSpeed;        ///< 移动速度   c maybe
    short    ReadyTime;        ///< 准备时间   c maybe
    short    GridFlyTime;      ///< 每格飞行时间 c maybe
    short    RangeTimes;       ///< 作用范围目标次数 c maybe
    short    RangeCount;       ///< 作用范围目标个数 c maybe
    short    BufId1;           ///< 存在状态Buff1的技能id
    short    BufLvl1;          ///< 存在状态的buff1的附加等级
    short    BufNum1;          ///< 存在状态buff1的附加层数
    short    BufId2;           ///< 存在状态Buff2的技能id
    short    BufLvl2;          ///< 存在状态的buff2的附加等级
    short    BufNum2;          ///< 存在状态buff2的附加层数
    short    BufId3;           ///< 存在状态Buff3的技能id
    short    BufLvl3;          ///< 存在状态的buff3的附加等级
    short    BufNum3;          ///< 存在状态buff3的附加层数
    short    BufId4;           ///< 存在状态Buff4的技能id
    short    BufLvl4;          ///< 存在状态的buff4的附加等级
    short    BufNum4;          ///< 存在状态buff4的附加层数
    short    BufId5;           ///< 存在状态Buff5的技能id
    short    BufLvl5;          ///< 存在状态的buff5的附加等级
    short    BufNum5;          ///< 存在状态buff5的附加层数
    short    BufId6;           ///< 存在状态Buff6的技能id
    short    BufLvl6;          ///< 存在状态的buff6的附加等级
    short    BufNum6;          ///< 存在状态buff6的附加层数


    tagSkillAttr()
    {
        memset( this , 0 , sizeof(tagSkillAttr) );
    }

    void   AddMapTable( std::map<string,short>&  mp)
    {
        BIND_SKILLPRO(mp,"TemplateId",TempId);
        BIND_SKILLPRO(mp,"MinAtkDistance",MinAtkDistance);
        BIND_SKILLPRO(mp,"MaxAtkDistance",MaxAtkDistance);
        BIND_SKILLPRO(mp,"BuffPower",BuffPower);
        BIND_SKILLPRO(mp,"IntonateTime",IntonateTime);
        BIND_SKILLPRO(mp,"CoolDown",CoolDown);
        BIND_SKILLPRO(mp,"ActTime",ActTime);
        BIND_SKILLPRO(mp,"HitTime",HitTime);
        BIND_SKILLPRO(mp,"FlyDelay",FlyDelay);
        BIND_SKILLPRO(mp,"LoopCount",LoopCount);     
        BIND_SKILLPRO(mp,"HurtFactor",HurtFactor);      
        BIND_SKILLPRO(mp,"HealFactor",HealFactor);     
        BIND_SKILLPRO(mp,"BaseHurt",BaseHurt);        
        BIND_SKILLPRO(mp,"BaseHeal",BaseHeal);       
        BIND_SKILLPRO(mp,"EHit",EHit);            
        BIND_SKILLPRO(mp,"EHeal",EHeal);           
        BIND_SKILLPRO(mp,"EHurtValue",EHurtValue);      
        BIND_SKILLPRO(mp,"EHealValue",EHealValue);      
        BIND_SKILLPRO(mp,"SEnergy",SEnergy);          
        BIND_SKILLPRO(mp,"SMp",SMp);            
        BIND_SKILLPRO(mp,"SHp",SHp);             
        BIND_SKILLPRO(mp,"SStrength",SStrength);        
        BIND_SKILLPRO(mp,"SDexterity",SDexterity);      
        BIND_SKILLPRO(mp,"SCon",SCon);            
        BIND_SKILLPRO(mp,"SIntellect",SIntellect);      
        BIND_SKILLPRO(mp,"SSpiritualism",SSpiritualism);    
        BIND_SKILLPRO(mp,"TaoismLvl",TaoismLvl);      
        BIND_SKILLPRO(mp,"OutEnergyPower",OutEnergyPower); 
        BIND_SKILLPRO(mp,"OutEnergyHurt",OutEnergyHurt);  
        BIND_SKILLPRO(mp,"EEnergyHit",EEnergyHit);    
        BIND_SKILLPRO(mp,"EEnergyHeal",EEnergyHeal);   
        BIND_SKILLPRO(mp,"OutEnergyHitLvl",OutEnergyHitLvl);    
        BIND_SKILLPRO(mp,"EnergyHitLvl",EnergyHitLvl);       
        BIND_SKILLPRO(mp,"OutEnergyCriRatio",OutEnergyCriRatio);
        BIND_SKILLPRO(mp,"EnergyCriRatio",EnergyCriRatio); 
        BIND_SKILLPRO(mp,"OutEnergyCri",OutEnergyCri);   
        BIND_SKILLPRO(mp,"EnergyCri",EnergyCri);       
        BIND_SKILLPRO(mp,"OutEnergySpeed",OutEnergySpeed);  
        BIND_SKILLPRO(mp,"EnergySpeed",EnergySpeed);    
        BIND_SKILLPRO(mp,"IgnoreTarget",IgnoreTarget);    
        BIND_SKILLPRO(mp,"ArmorDef",ArmorDef);        
        BIND_SKILLPRO(mp,"DefLvl",DefLvl);         
        BIND_SKILLPRO(mp,"ParryLvl",ParryLvl);       
        BIND_SKILLPRO(mp,"ToughnessLvl",ToughnessLvl);    
        BIND_SKILLPRO(mp,"BrokenArmorLvl",BrokenArmorLvl);  
        BIND_SKILLPRO(mp,"MissRatio",MissRatio);      
        BIND_SKILLPRO(mp,"TaoismHurt",TaoismHurt);      
        BIND_SKILLPRO(mp,"TaoismHitLvl",TaoismHitLvl);    
        BIND_SKILLPRO(mp,"TaoismHitRatio",TaoismHitRatio);  
        BIND_SKILLPRO(mp,"TaoismSpeed",TaoismSpeed);    
        BIND_SKILLPRO(mp,"TaoismCri",TaoismCri);       
        BIND_SKILLPRO(mp,"TaosimRiftLvl",TaosimRiftLvl);  
        BIND_SKILLPRO(mp,"TaosimDefRatio",TaosimDefRatio);  
        BIND_SKILLPRO(mp,"ERation",ERation);         
        BIND_SKILLPRO(mp,"EArmorHp",EArmorHp);      
        BIND_SKILLPRO(mp,"AttackSpeed",AttackSpeed);
        BIND_SKILLPRO(mp,"MoveSpeed",MoveSpeed);
        BIND_SKILLPRO(mp,"ReadyTime",ReadyTime);
        BIND_SKILLPRO(mp,"GridFlyTime",GridFlyTime);
        BIND_SKILLPRO(mp,"RangeTimes",RangeTimes);
        BIND_SKILLPRO(mp,"RangeCount",RangeCount);


        BIND_SKILLPRO_STATE_EXED(mp,1);
        BIND_SKILLPRO_STATE_EXED(mp,2);
        BIND_SKILLPRO_STATE_EXED(mp,3);
        BIND_SKILLPRO_STATE_EXED(mp,4);
        BIND_SKILLPRO_STATE_EXED(mp,5);
        BIND_SKILLPRO_STATE_EXED(mp,6);
 
    }
};
