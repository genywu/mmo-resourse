#pragma once                    //┃//┃
#ifndef __TETRISBACKSCENE_H__   //┃//┃
#define __TETRISBACKSCENE_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：TetrisBackScene.h       //┃
//功能描述：俄罗斯方块场景对象      //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 俄罗斯方块场景横排方块最大个数(游戏设计决定,不要轻易改动)
#define BACKSCENE_MAX_SQUARE_COUNT_X    25

// 俄罗斯方块场景纵排方块最大个数(游戏设计决定,不要轻易改动)
#define BACKSCENE_MAX_SQUARE_COUNT_Y    21

// 场景中元素结构体
typedef struct BACK_SCENE_BYTE 
{
    BYTE m_Square;          // 方块数据
    WORD m_MetafileIndex;   // 方块使用的图元索引数据
    BYTE m_Reserve;         // 内存对其保留
}BSB;

class CT_Scene
{
public: // 构造、析构
    CT_Scene(VOID);
    ~CT_Scene(VOID);

public: // 接口

    // 初始化场景类
    BOOL Init(
        WORD wBackSceneMetafileIndex,
        DWORD dwBackSceneWidth,
        DWORD dwBackSceneHeight
        );

    // 获取场景元素数组首地址
    VOID GetBackSceneSize(
        DWORD& dwBackSceneWidth,
        DWORD& dwBackSceneHeight
        );

    // 重置场景
    BOOL Reset(
        WORD wBackSceneMetafileIndex,
        DWORD dwBackSceneWidth,
        DWORD dwBackSceneHeight
        );

    VOID    Release(VOID);                                          // 释放场景
    WORD    GetBackMetafileIndex(VOID);                             // 设置场景图元索引
    BOOL    SetBackMetafileIndex(WORD wBackSceneMetafileIndex);     // 设置场景图元索引
    BOOL    SetFrontMetafileIndex(WORD wFrontSceneMetafileIndex);   // 设置前景图元索引
    BSB*    GetBackSceneBuff(VOID);                                 // 获取场景元素数组首地址

private:// 私有
    WORD                m_wBackSceneMetafileIndex;  // 场景的图元索引
    BSB*                m_pSceneBYTE;               // 场景元素数组指针
    DWORD               m_dwBackSceneWidth;         // 场景的宽度
    DWORD               m_dwBackSceneHeight;        // 场景的高度
};


//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
