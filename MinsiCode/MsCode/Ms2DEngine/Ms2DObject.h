#pragma once            //┃//┃
#ifndef __MS2DOBJECT_H__//┃//┃
#define __MS2DOBJECT_H__//┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：Ms2DObject.h    //┃
//功能描述：2D引擎:         //┃
//          游戏对象基类    //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

//━━━━━━━━〖MS2DEngine〗━━━━━━━━━┓
                                                //┃
// 2D引擎,名字最大长度                          //┃
#define _2DE_MAX_OBJ_NAME_LEN           10      //┃
                                                //┃
// 2D引擎,对象最大尺寸                          //┃
#define _2DE_MAX_OBJ_SIZE               100.0f  //┃
                                                //┃
// 2D引擎,对象最大移动速度                      //┃
#define _2DE_MAX_OBJ_MOVE_SPEED         100.0f  //┃
                                                //┃
                                                //┃
//━━━━━━━━━━━━━━━━━━━━━━━━┛

class CMs2DObject
{
public:
    CMs2DObject(VOID);
    ~CMs2DObject(VOID);

    // 名字
    CONST TCHAR*    GetName(VOID);
    VOID            SetName(CONST TCHAR* szName, DWORD dwNameLen);
    DWORD           GetNameLen(VOID);

    // 坐标
    VOID GetPos(FPOS& fPosX, FPOS& fPosY);
    VOID SetPos(FPOS fPosX, FPOS fPosY);

    // 尺寸
    VOID GetSize(FSIZE& fWidth, FSIZE& fLength);
    VOID SetSize(FSIZE fWidth, FSIZE fLength);

    // 朝向(旋转角度)
    FLOAT   GetRotary(VOID);
    VOID    SetRotary(FLOAT fRotary);

    // 速度
    FLOAT   GetWalkSpeed(VOID);
    VOID    SetWalkSpeed(FLOAT fWalkSpeed);
    FLOAT   GetRunSpeed(VOID);
    VOID    SetRunSpeed(FLOAT fRunSpeed);

protected:

    // 对象基本属性
    TCHAR   m_szName[_2DE_MAX_OBJ_NAME_LEN];// 名字
    FPOS    m_fPosX;                        // X坐标
    FPOS    m_fPosY;                        // Y坐标
    FSIZE   m_fWidth;                       // 宽度
    FSIZE   m_fLength;                      // 长度
    FLOAT   m_fRotary;                      // 朝向(旋转角度)
    FLOAT   m_fWalkSpeed;                   // 走路速度
    FLOAT   m_fRunSpeed;                    // 跑步速度



    // 逻辑操作属性
    DWORD       m_dwInPoolIndex;            // 在池子中的索引
    FILETIME    m_ftLandTime;               // 登陆时间
};


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
