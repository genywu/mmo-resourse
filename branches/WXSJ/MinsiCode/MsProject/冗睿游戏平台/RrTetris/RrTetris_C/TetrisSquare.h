#pragma once                //┃//┃
#ifndef __TETRISSQUARE_H__  //┃//┃
#define __TETRISSQUARE_H__  //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：TetrisSquare.h      //┃
//功能描述：俄罗斯方块形状对象  //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 默认的形状文件名
#define DEFAULT_SHAPE_FILE_NAME _T("%s/RrTetris/Shape.sap")

// 最大的图元个数
#define MAX_METAFILE_COUNT      10

// 图元最大宽高
#define MAX_METAFILE_WIDTH      16
#define MAX_METAFILE_HEIGHT     16

// 一个方块有几种变形状态
#define SHAPE_DISTORTION_STATE  4

// 方块形状
class CT_Square
{
public:
    CT_Square(VOID);
    ~CT_Square(VOID);

    BOOL InitMetafile(WORD wMetafileIndex);     // 初始化图元数据成员
    BOOL InitShape(WORD wShapeIndex);           // 初始化形状数据成员

    VOID GetShapePos(NPOS& uX, NPOS& uY);       // 获取形状在场景中的坐标
    VOID SetShapePos(NPOS X, NPOS Y);           // 设置形状在场景中的坐标

    VOID GetShapeShadowPos(NPOS& uX, NPOS& uY); // 获取形状阴影在场景中的坐标X
    VOID SetShapeShadowPos(NPOS X, NPOS Y);     // 设置形状阴影在场景中的坐标

    BOOL CreateShape(DWORD dwBackSceneWidth);   // 创建方块
    VOID Clear(VOID);                           // 清除形状

    CT_ShapeFile* GetShapeFile(VOID);           // 获取形状文件

    // 形状变形
    BOOL Distortion(
        DWORD dwBackSceneWidth,
        DWORD dwBackSceneHeight,
        BOOL bOrder = TRUE
        );

    BYTE GetVoidCountByLeft(VOID)       {return m_VoidCountByLeft;}
    BYTE GetVoidCountByRight(VOID)      {return m_VoidCountByRight;}
    BYTE GetVoidCountByUp(VOID)         {return m_VoidCountByUp;}
    BYTE GetVoidCountByDown(VOID)       {return m_VoidCountByDown;}

    // 获取形状索引
    WORD GetCurrentShapeIndex(VOID)     {return m_wCurrentShapeIndex;}
    WORD GetNext_ShapeIndex(VOID)       {return m_wNext_ShapeIndex;}
    WORD GetNext2_ShapeIndex(VOID)      {return m_wNext2_ShapeIndex;}
    WORD GetNext3_ShapeIndex(VOID)      {return m_wNext3_ShapeIndex;}

    // 获取形状图元索引
    WORD GetCurrentMetafileIndex(VOID)  {return m_wCurrentMetafileIndex;}
    INT GetNext_MetafileIndex(VOID)     {return m_wNext_MetafileIndex;}
    INT GetNext2_MetafileIndex(VOID)    {return m_wNext2_MetafileIndex;}
    INT GetNext3_MetafileIndex(VOID)    {return m_wNext3_MetafileIndex;}

    // 获取形状数据指针
    LPBYTE GetCurrentShapeBuff(VOID)    {return m_pCurrentShape;}
    LPBYTE GetNext_ShapeBuff(VOID)      {return m_pNext_Shape;}
    LPBYTE GetNext2_ShapeBuff(VOID)     {return m_pNext2_Shape;}
    LPBYTE GetNext3_ShapeBuff(VOID)     {return m_pNext3_Shape;}

private:

    CT_ShapeFile*       m_pShapeFile;       // 形状文件
    CMsGdiPlusImage*    m_pBitmapMetafile;  // 形状所使用的图元图片

    WORD    m_wCurrentMetafileIndex;    // 当前图元在图片中的索引位置
    WORD    m_wNext_MetafileIndex;      // 下一个图元在图片中的索引位置
    WORD    m_wNext2_MetafileIndex;     // 下下一个图元在图片中的索引位置
    WORD    m_wNext3_MetafileIndex;     // 下下下一个图元在图片中的索引位置

    LPBYTE  m_pCurrentShape;            // 当前方块形状数组指针
    LPBYTE  m_pNext_Shape;              // 下一个方块形状数组指针
    LPBYTE  m_pNext2_Shape;             // 下下一个方块形状数组指针
    LPBYTE  m_pNext3_Shape;             // 下下下一个方块形状数组指针

    WORD    m_wCurrentShapeIndex;       // 方块的形状索引
    WORD    m_wNext_ShapeIndex;         // 下一个方块的形状索引
    WORD    m_wNext2_ShapeIndex;        // 下下一个方块的形状索引
    WORD    m_wNext3_ShapeIndex;        // 下下下一个方块的形状索引

    UPOS    m_nMetafilePosX;            // 图元在图片中的横坐标
    UPOS    m_nMetafilePosY;            // 图元在图片中的纵坐标
    NPOS    m_SquarePosX;               // 形状在场景中的横坐标
    NPOS    m_SquarePosY;               // 形状在场景中的纵坐标

    BYTE    m_VoidCountByLeft;          // 当前形状中左边空白个数
    BYTE    m_VoidCountByRight;         // 当前形状中右边空白个数
    BYTE    m_VoidCountByUp;            // 当前形状中上边空白个数
    BYTE    m_VoidCountByDown;          // 当前形状中下边空白个数

    DWORD   m_dwShapeCount;             // 形状总个数

    NPOS    m_SquareShadowPosX;         // 形状阴影在场景中的横坐标
    NPOS    m_SquareShadowPosY;         // 形状阴影在场景中的纵坐标
};



//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
