#include "Precompiled.h"
#include "Ms2DImageDivide.h"
#pragma message("━━━━━━━━━━━━未完成━━━━━━━━━━━━━")

#include "MsString.h"
CMs2DImageDivide::CMs2DImageDivide(VOID)
{
    m_pMainImage = NEW CxImage;

}

CMs2DImageDivide::~CMs2DImageDivide(VOID)
{
    SAFE_DELETE_ARRAY(m_pSubImage);
    SAFE_DELETE(m_pMainImage);
}

// 打开图片文件
BOOL CMs2DImageDivide::OpenImage(TCHAR* szImageFilePathName)
{
    return m_pMainImage->Load(szImageFilePathName);
}

// 关闭所有资源
BOOL CMs2DImageDivide::CloseAll(VOID)
{
    BOOL bRet = TRUE;
    if (m_pSubImage)
    {
        FOR_x_TO(i, m_dwSubImageCount)
        {
            bRet = m_pSubImage[i].Destroy();
            if (!bRet)
            {
                return bRet;
            }
        }
    }
    if (m_pMainImage)
    {
        bRet = m_pMainImage->Destroy();
    }
    return bRet;
}

// 设置子图片尺寸
BOOL CMs2DImageDivide::SetSubImageSize(NSIZE /*nWidth*/, NSIZE /*nHeight*/)
{
    return TRUE;

}

// 进行分割
BOOL CMs2DImageDivide::Divide(BOOL)
{
    return TRUE;
}

// 将子图片打包并保存
BOOL CMs2DImageDivide::SaveSubImagePack(TCHAR* /*szPackFilePathName*/)
{
    return TRUE;
}