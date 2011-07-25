#include "Precompiled.h"
#include "MsImageManager.h"

CMsImageManager::CMsImageManager(VOID)
: m_pSubImage           (NULL)
, m_dwSubImageSize      (INVALID_UID)
, m_dwSubImageCount     (INVALID_UID)
, m_lpPointArray        (NULL)
, m_eImageType          (CXIMAGE_FORMAT_UNKNOWN)
, m_MsLog               (_T("CMsImageManager.log"), FALSE)
{
    ;
}

CMsImageManager::~CMsImageManager(VOID)
{
    this->CloseAllImage();
}

// 打开图片
BOOL CMsImageManager::OpenImageFile(TCHAR* szImageFileName)
{
    BOOL bRet = m_MainImage.Load(szImageFileName, CXIMAGE_SUPPORT_BMP);
    if (!bRet)
    {
        m_MsLog << _T("打开图片失败") << L_NEND;
    }
    return bRet;
}

// 保存图片
BOOL CMsImageManager::SaveImageFile(TCHAR* szImageFileName)
{
    AssertEx(szImageFileName, _T("保存路径不能为空"));
    if (CXIMAGE_FORMAT_UNKNOWN == m_eImageType)
    {
        return FALSE;   // 保存类型不确定
    }
    BOOL bRet = m_MainImage.Save(szImageFileName, m_eImageType);
    if (bRet)
    {
        m_MsLog
            << _T("保存图片[") 
            << szImageFileName 
            << _T("]失败") 
            << L_NEND;
    }
    return bRet;
}

// 设置图片类型
VOID CMsImageManager::SetImageFileType(E_Image_Type eImageType)
{
    m_eImageType = eImageType;
}

// 拆分图片
DWORD CMsImageManager::SplitImageFile(DWORD dwSubImageSize)
{
    _ENTER_FUN_C;

    m_dwSubImageSize = dwSubImageSize;

    if (m_dwSubImageSize < SUB_MAP_LEAST_SIZE)
    {
        return INVALID_UID;  // 子图片尺寸不允许低于 SUB_MAP_LEAST_SIZE 个
    }

    if (0 == m_dwSubImageSize)
    {
        m_dwSubImageSize = 1;
    }

    // 获取分割后横排子图片个数
    m_dwSubImageCountByX = m_MainImage.GetWidth() / m_dwSubImageSize + 1;

    // 获取分割后纵排子图片个数
    m_dwSubImageCountByY = m_MainImage.GetHeight() / m_dwSubImageSize + 1;

    // 计算子图片总个数
    m_dwSubImageCount = m_dwSubImageCountByX * m_dwSubImageCountByY;

    // 计算横排最后一个子图片需要显示的宽度
    DWORD dwLastSubImageWidthByX = SUB_MAP_LEAST_SIZE - 
    (m_dwSubImageCountByX * SUB_MAP_LEAST_SIZE - m_MainImage.GetWidth());

    // 计算纵排最后一个子图片需要显示的高度
    DWORD dwLastSubImageHeightByY = SUB_MAP_LEAST_SIZE - 
    (m_dwSubImageCountByY * SUB_MAP_LEAST_SIZE - m_MainImage.GetHeight());

    // 申请子图片
    m_pSubImage = NEW CxImage[m_dwSubImageCount];
    AssertEx(m_pSubImage, _T("内存不足"));

    // 申请子图片偏移数组
    m_lpPointArray = NEW POINT[m_dwSubImageCount];
    AssertEx(m_lpPointArray, _T("内存不足"));

    // 获取图片颜色深度
    DWORD dwImageBpp    = m_MainImage.GetBpp();
    DWORD dwImageType   = m_MainImage.GetType();

    // 获取
    FOR_x_TO(dwIndex, m_dwSubImageCount)
    {
        DWORD dwCurrentSubImageWidth = m_dwSubImageSize;
        DWORD dwCurrentSubImageHeight = m_dwSubImageSize;

        // 如果是最后一个子图片
        if ((dwIndex+1) == 
            m_dwSubImageCount)
        {
            // 创建子图片
            if (m_pSubImage)
            {
                m_pSubImage[dwIndex].Create(dwLastSubImageWidthByX,
                    dwLastSubImageHeightByY,
                    dwImageBpp,
                    dwImageType
                    );

                dwCurrentSubImageWidth = dwLastSubImageWidthByX;
                dwCurrentSubImageHeight = dwLastSubImageHeightByY;
            }
            else
            {
                AssertEx(m_pSubImage, _T("调用错误"));
            }
        }

        // 如果是横排最后一个子图片
        else if (0 != dwIndex &&
            0 == (dwIndex+1)%(m_dwSubImageCountByX))
        {
            // 创建子图片
            m_pSubImage[dwIndex].Create(dwLastSubImageWidthByX,
                m_dwSubImageSize,
                dwImageBpp,
                dwImageType
                );

            dwCurrentSubImageWidth = dwLastSubImageWidthByX;
        }

        // 如果是纵排最后一个子图片
        else if ((dwIndex+1) > 
            ((m_dwSubImageCountByY - 1) * m_dwSubImageCountByX))
        {
            // 创建子图片
            m_pSubImage[dwIndex].Create(m_dwSubImageSize,
                dwLastSubImageHeightByY,
                dwImageBpp,
                dwImageType
                );

            dwCurrentSubImageHeight = dwLastSubImageHeightByY;
        }

        // 其他子图片
        else
        {
            // 创建子图片
            if (m_pSubImage)
            {
                m_pSubImage[dwIndex].Create(m_dwSubImageSize,
                    m_dwSubImageSize,
                    dwImageBpp,
                    dwImageType
                    );

                RECT  rc = {0,0,0,0};
                rc.left     = (dwIndex%m_dwSubImageCountByX)*m_dwSubImageSize;
                rc.top      = (dwIndex/m_dwSubImageCountByX)*m_dwSubImageSize;
                rc.right    = rc.left + dwCurrentSubImageWidth;
                rc.bottom   = rc.top  + dwCurrentSubImageHeight;
                this->CopyImageInMainImageRect(&m_pSubImage[dwIndex],
                    &rc,
                    0,
                    0,
                    FALSE
                    );
            }
            else
            {
                AssertEx(m_pSubImage, _T("调用错误"));
            }
        }
    }
    // 返回子图片个数
    return m_dwSubImageCount;

    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_UID;
}

// 关闭所有图片
VOID CMsImageManager::CloseAllImage(VOID)
{
    // 释放子图片数组
    SAFE_DELETE_ARRAY(m_pSubImage);

    // 释放偏移数组
    SAFE_DELETE_ARRAY(m_lpPointArray);
}

// 画主图片
BOOL CMsImageManager::DrawMainImage(
    HDC hDc,
    LONG X,
    LONG Y,
    FLOAT fZoom
    )
{
    // 画主图片
    FLOAT fDrawCX = fZoom * m_MainImage.GetWidth();
    FLOAT fDrawCY = fZoom * m_MainImage.GetHeight();
    INT nRet = m_MainImage.Draw(hDc, X, Y, (LONG)fDrawCX, (LONG)fDrawCY);
    if (nRet)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// 根据索引画子图片
BOOL CMsImageManager::DrawSubImage(
    HDC hDc,
    DWORD dwIndex,
    LONG X,
    LONG Y,
    FLOAT fZoom
    )
{
    _ENTER_FUN_C;

    AssertEx(dwIndex < m_dwSubImageCount, _T("指定的子图片不存在"));

    if (m_pSubImage)
    {
        FLOAT fDrawCX = fZoom * m_pSubImage[dwIndex].GetWidth();
        FLOAT fDrawCY = fZoom * m_pSubImage[dwIndex].GetHeight();

        FLOAT fRealDrawX = 
            (dwIndex % m_dwSubImageCountByX) * m_dwSubImageSize * fZoom;

        FLOAT fRealDrawY = 
            (dwIndex / m_dwSubImageCountByX) * m_dwSubImageSize * fZoom;

        INT nRet = m_pSubImage[dwIndex].Draw(
            hDc,
            X+(LONG)fRealDrawX,
            Y+(LONG)fRealDrawY,
            (LONG)fDrawCX,
            (LONG)fDrawCY
            );

        if (nRet)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        AssertEx(m_pSubImage, _T("调用错误"));
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 拷贝主图片指定区域到目标图片指定位置
BOOL CMsImageManager::CopyImageInMainImageRect(
    CxImage* pDesImage,
    LPRECT lpSrcRc,
    LONG DesX,
    LONG DesY,
    BOOL bCopyAlpha
    )
{
    if (!pDesImage)
    {
        return FALSE;
    }
    //DWORD dwMainImageWidth =
    //    m_MainImage.GetWidth();        // 主图片宽度

    DWORD dwMainImageHeight =
        m_MainImage.GetHeight();      // 主图片高度

    DWORD dwSrcBeginX =
        lpSrcRc->left;                      // 转换主图片起始位置X

    DWORD dwSrcBeginY =
        dwMainImageHeight - lpSrcRc->bottom;// 转换主图片起始位置Y

    DWORD dwCopyWidth =
        lpSrcRc->right - lpSrcRc->left;     // 复制宽度

    DWORD dwCopyHeight =
        lpSrcRc->bottom - lpSrcRc->top;    // 复制高度

    RGBQUAD rgba;
    FOR_x_TO(x, dwCopyWidth)
    {
        for (DWORD y = 0; y < dwCopyHeight; y++)
        {
            rgba = m_MainImage.GetPixelColor(
                dwSrcBeginX+x,
                dwSrcBeginY+y,
                bCopyAlpha?true:false
                );

            pDesImage->SetPixelColor(
                DesX+x,
                DesY+y,
                rgba,
                bCopyAlpha?true:false
                );
        }
    }
    return TRUE;
}

// 获取主图片的尺寸
VOID CMsImageManager::GetMainImageSize(
    DWORD& dwWidth,
    DWORD& dwHeight
    )
{
    dwWidth = m_MainImage.GetWidth();
    dwHeight = m_MainImage.GetHeight();
}
