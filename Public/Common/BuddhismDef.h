
/** 
 * @brief  关于佛之学系统的数据定义
 * @author expter
 */


#pragma  once 

const  long  lMaxDisttractValue = 100;  ///> 最大值

#define  CAMP(value,Min,Max) \
    {\
        if( value > Max )\
            value = Max;\
        if( value < Min )\
            value = Min;\
    }

struct  tagBuddhism
{
    long  lDistract;    ///> 当前杂念值
    long  lDryNum;      ///> 燥的数量
    long  lDryEndTime;  ///> 燥的结束时间
    long  lLastTime;    ///> 上次使用技能时间
    long  lBudhismTimer;///> 杂念值降低定时器对象id
    long  lDryTimer;    ///> 燥清空定时器

    tagBuddhism()
    {
        memset( this , 0 , sizeof(tagBuddhism) );
    }

    long UpdateDistract( long Value)
    {
        lDistract +=Value;
        
        CAMP( lDistract , 0 , lMaxDisttractValue );

        return lDistract;
    }

    long  UpdateDryNum( long value, long Max )
    {
        lDryNum  += value;

        CAMP( lDryNum , 0 , Max );

        return lDryNum;
    }

    void  ClearDryNum()     { lDryNum = 0 ; }
    long  RetDryNum() const { return lDryNum;  }
    long  RetDistract()const{ return lDistract;}
    bool  IsFull()    const { return lDistract>= lMaxDisttractValue;}
    bool  IsEmpty()   const { return lDistract <= 0 ;}

    long  RegisterTimer(long Time)
    {
    }

    long  UpdateEndTime( long Time)
    {

        return 0;
    }
};
