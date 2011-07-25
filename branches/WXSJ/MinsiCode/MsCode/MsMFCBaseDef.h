#pragma once                //┃//┃
#ifndef __MSMFCBASEDEF_H__  //┃//┃
#define __MSMFCBASEDEF_H__  //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsMFCBaseDef.h      //┃
//功能描述：MFC基本定义         //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━┓
class CMsMFCBaseRun                     //┃
{                                       //┃
public:                                 //┃
//━━━━━━━━━━━━━━━━━━━━┫
    CMsMFCBaseRun(VOID)                 //┃
    {                                   //┃
        ;                               //┃
    }                                   //┃
//━━━━━━━━━━━━━━━━━━━━┫
    ~CMsMFCBaseRun(VOID)                //┃
    {                                   //┃
        ::AfxDaoTerm();                 //┃
    }                                   //┃
};                                      //┃
extern CMsMFCBaseRun g_CMsMFCBaseRun;   //┃
//━━━━━━━━━━━━━━━━━━━━┛
//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
