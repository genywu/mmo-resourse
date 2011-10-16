/**************************************************
*		文件名  ：LogDef.h
*		简要描述：定义了所有日志输出模块

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：2010-11-10
*		修订说明：
****************************************************/

#ifndef _LOGDEF_H_
#define _LOGDEF_H_

    enum eLogModule
    {
        ROOT_MODULE = 0,    //all.log  所有日志都输出到这里
        NET_MODULE,         //net.log  网络相关的日志输出到这里
        PET_MODULE,         //pet.log
        SKILL_MODULE,       //skill.log
        MAIL_MODULE,        //mail.log
    };

#endif