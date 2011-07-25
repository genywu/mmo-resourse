#pragma once                    //┃//┃
#ifndef __TETRISSHAPEFILE_H__   //┃//┃
#define __TETRISSHAPEFILE_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：TetrisShapeFile.h       //┃
//功能描述：俄罗斯方块形状文件      //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 形状文件头标记宏定义
#define SHAPE_FILE_FILG     0x45504153 // SAPE

// 形状文件头版本号
#define SHAPE_FILE_VERSION  0x00000001

// 形状文件中每个形状所占字节数
#define SHAPE_FILE_ONE_SHAPE_SIZE   16

// 宏定义方块宽高
#define SHAPE_MAX_WIDTH     4
#define SHAPE_MAX_HEIGHT    4

// 宏定义存在方块
#define HAVE_SQUARE TRUE

// 宏定义不存在方块
#define NO_SQUARE   FALSE

class CT_ShapeFile
{
public:
    typedef struct SHAPE_FILE_HEAD 
    {
        DWORD   m_FileFlag;     // 形状文件头的标记
        DWORD   m_Version;      // 形状文件头的版本号
        DWORD   m_ShapeCount;   // 形状文件头的形状个数
        DWORD   m_Hold;         // 形状文件头的保留(4字节)
    }SFH;

    CT_ShapeFile(TCHAR* strShapeFileName);
    ~CT_ShapeFile(VOID);

    DWORD   GetShapeCount(VOID);                        // 获取形状个数
    LPBYTE  GetShapeByIndex(WORD wIndex);               // 获取形状数组
    BYTE    GetShapeUsedSquareByLeft(WORD wIndex);      // 获取第一个有方块的位置从左
    BYTE    GetShapeUsedSquareByRight(WORD wIndex);     // 获取第一个有方块的位置从右
    BYTE    GetShapeUsedSquareByUp(WORD wIndex);        // 获取第一个有方块的位置从上
    BYTE    GetShapeUsedSquareByDown(WORD wIndex);      // 获取第一个有方块的位置从下
private:

    LPBYTE  m_pData;        // 形状文件内容缓冲区
    DWORD   m_dwFileSize;   // 形状文件的尺寸
    HANDLE  m_hShapeFile;   // 形状文件句柄
    SFH     m_ShapeFileHead;// 形状文件头
};


//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
