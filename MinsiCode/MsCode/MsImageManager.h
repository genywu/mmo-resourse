#pragma once                    //┃    //┃
#ifndef __MSIMAGEMANAGER_H__    //┃    //┃
#define __MSIMAGEMANAGER_H__    //┃    //┃
//━━━━━━━━━━━━━━━━┛    //┃
//文件名称：MsImageManager.h            //┃
//功能描述：图片管理器                  //┃
//平台相关：WINDOWS                     //┃
//━━━━━━━━━━━━━━━━━━━━┛

// 子图片最小尺寸
#define SUB_MAP_LEAST_SIZE  100

class CMsImageManager
{
public:

    // 构造、析构
    CMsImageManager(VOID);
    virtual ~CMsImageManager(VOID);

    BOOL    OpenImageFile(TCHAR* szImageFileName);      // 打开图片
    BOOL    SaveImageFile(TCHAR* szImageFileName);      // 保存图片
    VOID    SetImageFileType(E_Image_Type eImageType);  // 设置图片类型
    DWORD   SplitImageFile(                             // 拆分图片
        DWORD dwSubImageSize = SUB_MAP_LEAST_SIZE
        );
    VOID    CloseAllImage(VOID);                        // 关闭所有图片
    DWORD   GetSplitImageSize(VOID)                     // 获取子图片尺寸
    {
        return m_dwSubImageSize;
    }
    
    BOOL DrawMainImage(         // 画主图片
        HDC hDc,
        LONG X,
        LONG Y,
        FLOAT fZoom = 1.0f
        );

    BOOL DrawSubImage(          // 根据索引画子图片
        HDC hDc,
        DWORD dwIndex,
        LONG X,
        LONG Y,
        FLOAT fZoom = 1.0f
        );

    // 拷贝主图片指定区域到目标图片指定位置
    BOOL CopyImageInMainImageRect(
        CxImage* pDesImage,
        LPRECT lpSrcRc,
        LONG DesX,
        LONG DesY,
        BOOL bCopyAlpha
        );

    // 获取主图片的尺寸
    VOID GetMainImageSize(
        DWORD& dwWidth,
        DWORD& dwHeight);

private:

    CxImage         m_MainImage;            // 主图片对象
    CxImage*        m_pSubImage;            // 子图片对象
    LPPOINT         m_lpPointArray;         // 子图片偏移坐标数组
    DWORD           m_dwSubImageSize;       // 子图片尺寸
    DWORD           m_dwSubImageCountByX;   // 每个横排子图片个数
    DWORD           m_dwSubImageCountByY;   // 每个纵排子图片个数
    DWORD           m_dwSubImageCount;      // 子图片个数
    E_Image_Type    m_eImageType;           // 图片类型

    CMsLog          m_MsLog;
};



//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
