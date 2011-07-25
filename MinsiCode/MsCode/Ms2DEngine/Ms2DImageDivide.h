#pragma once                    //┃//┃
#ifndef __MS2DIMAGEDIVIDE_H__   //┃//┃
#define __MS2DIMAGEDIVIDE_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：Ms2DImageDivide.h       //┃
//功能描述：2D引擎工具,图片分割     //┃
//平台相关：Windows                 //┃
//注    意：本类集成了 WinRAR 压缩工//┃
//          具, 仅限内部测试使用, 不//┃
//          得用于任何商业行为, 本模//┃
//          块作者编写目的仅为技术交//┃
//          流使用, 由于使用本模块引//┃
//          发的任何违法行为, 后果由//┃
//          使用者自行承担.         //┃
//━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━〖MS2DEngine〗━━━━━━┓
                                            //┃
//━━━━━━━━━━━━━━━━━━━━━━┛

class CMs2DImageDivide
{
public:
    CMs2DImageDivide(VOID);
    ~CMs2DImageDivide(VOID);

    // 打开图片文件
    BOOL OpenImage(TCHAR* szImageFilePathName);

    // 关闭所有资源
    BOOL CloseAll(VOID);

    // 设置子图片尺寸
    BOOL SetSubImageSize(NSIZE /*nWidth = INVALID_NID*/, NSIZE /*nHeight = INVALID_NID*/);

    // 进行分割
    BOOL Divide(BOOL);

    // 将子图片打包并保存
    BOOL SaveSubImagePack(TCHAR* /*szPackFilePathName*/);

private:
    CxImage*    m_pMainImage;       // 主图片对象
    CxImage*    m_pSubImage;        // 子图片对象
    DWORD       m_dwSubImageCount;  // 子图片个数
    DWORD       m_dwSubImageWidth;  // 子图片宽度
    DWORD       m_dwSubImageHeight; // 子图片高度
};
//
//void CGameClientView::OnMapConversion()
//{
//    CMsImageSplitManager* m_pCMsImageSplitManager;    // 图片管理器
//    ACHAR    szFileNameOnly[MAX_PATH] = {0};
//
//    m_pCMsImageSplitManager = new CMsImageSplitManager;
//    if (!m_pCMsImageSplitManager)
//    {
//        ASSERT(m_pCMsImageSplitManager);
//    }
//
//    CFileDialog ImageFileDlg(TRUE, NULL, NULL, 0, "位图文件|*.bmp||");
//    ImageFileDlg.DoModal();
//    CString strImageFilePathName = ImageFileDlg.GetPathName();
//    CString strImageFileName = ImageFileDlg.GetFileName();
//    if (0 == strImageFilePathName.GetLength() || 0 == strImageFileName.GetLength())
//    {
//        return;
//    }
//    strcpy_s(szFileNameOnly, MAX_PATH, strImageFileName.GetString());
//
//    m_pCMsImageSplitManager->OpenImageFile(strImageFilePathName.GetString());
//    m_pCMsImageSplitManager->SplitImageFile();
//
//    TCHAR szRarPathName[MAX_PATH] = {0};
//    TCHAR szMapPathName[MAX_PATH] = {0};
//    TCHAR szNewMapPathName[MAX_PATH] = {0};
//    strcpy(szRarPathName, m_szCurrentPath);
//    strcpy(szMapPathName, m_szCurrentPath);
//    strcpy(szNewMapPathName, m_szCurrentPath);
//    strcat(szRarPathName, "/Map/rar");
//    strcat(szMapPathName, "/Map/NewMap/");
//    strcat(szNewMapPathName, "/Map/NewMap.ROmap");
//    ::CreateDirectoryA(szMapPathName, NULL);
//    m_pCMsImageSplitManager->SaveAllSubImage(szMapPathName);
//
//    DWORD dwFileNameLen = strlen(szFileNameOnly);
//    if ('.' == szFileNameOnly[dwFileNameLen-4])
//    {
//        szFileNameOnly[dwFileNameLen-4] = '\0';
//    }
//    else if ('.' == szFileNameOnly[dwFileNameLen-5])
//    {
//        szFileNameOnly[dwFileNameLen-5] = '\0';
//    }
//
//    this->CreateFileFromRes(IDR_WINRAR_EXE, "exe", szRarPathName);
//
//    PROCESS_INFORMATION ProcessInfo;  // 进程信息
//    STARTUPINFOA StartInfo;
//    ZeroMemory(&StartInfo, sizeof(StartInfo));
//    StartInfo.cb = sizeof(StartInfo);
//
//    ::SetCurrentDirectoryA(szMapPathName);
//    TCHAR szCurrentDir[MAX_PATH] = {0};
//    ::CreateProcessA(szRarPathName,
//        "-afzip -s -dw -m1 a -r NewMap.ROmap ./NewMap/*.dat",
//        NULL, NULL, FALSE, 0, NULL, NULL, &StartInfo, &ProcessInfo);
//    ::SetCurrentDirectoryA(szCurrentDir);
//    ::WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
//    //::MoveFile("E:/工作目录/dev/common/tools/Client_Sample/Map/NewMap.ROmap", "E:/工作目录/dev/common/tools/Client_Sample/Map/World.ROmap");
//    ::DeleteFileA(szRarPathName);
//    //::DeleteFileA(szNewMapPathName);
//
//    HANDLE hIniFile = ::CreateFileA(
//        m_szIniPathName,
//        FILE_READ_DATA,
//        0,
//        NULL,
//        OPEN_EXISTING,
//        0,
//        NULL);
//    if (INVALID_HANDLE_VALUE == hIniFile)
//    {
//        AfxMessageBox(_T("打开/Map/World.ini配置文件失败!"));
//        return;
//    }
//    else
//    {
//        ::CloseHandle(hIniFile);
//        hIniFile = INVALID_HANDLE_VALUE;
//        ACHAR szTemp[MAX_PATH] = {0};
//
//        m_pCMsImageSplitManager->GetMainImageSize(m_dwMapSizeWidth, m_dwMapSizeHeight);
//        itoa(m_dwMapSizeWidth, szTemp, 10);
//        ::WritePrivateProfileStringA("MapInfo", "MapWidth", szTemp, m_szIniPathName);
//
//        itoa(m_dwMapSizeHeight, szTemp, 10);
//        ::WritePrivateProfileStringA("MapInfo", "MapHeight", szTemp, m_szIniPathName);
//
//        itoa(m_pCMsImageSplitManager->GetSubImageCount(), szTemp, 10);
//        ::WritePrivateProfileStringA("MapInfo", "ImageCount", szTemp, m_szIniPathName);
//
//        itoa(m_pCMsImageSplitManager->GetSubImageCountByX(), szTemp, 10);
//        ::WritePrivateProfileStringA("MapInfo", "ImageCountByX", szTemp, m_szIniPathName);
//
//        itoa(m_pCMsImageSplitManager->GetSubImageCountByY(), szTemp, 10);
//        ::WritePrivateProfileStringA("MapInfo", "ImageCountByY", szTemp, m_szIniPathName);
//
//        DWORD dwSubMapSizeWidth, dwSubMapSizeHeight;
//        ACHAR szItemName[MAX_PATH] = {0};
//        for (DWORD i = 0; i < m_pCMsImageSplitManager->GetSubImageCount(); i++)
//        {
//            m_pCMsImageSplitManager->GetSubImageSize(i, dwSubMapSizeWidth, dwSubMapSizeHeight);
//
//            sprintf_s(szItemName, MAX_PATH, "X_%d", i);
//            itoa(dwSubMapSizeWidth, szTemp, 10);
//            ::WritePrivateProfileStringA("SizeArray", szItemName, szTemp, m_szIniPathName);
//
//            sprintf_s(szItemName, MAX_PATH, "Y_%d", i);
//            itoa(dwSubMapSizeHeight, szTemp, 10);
//            ::WritePrivateProfileStringA("SizeArray", szItemName, szTemp, m_szIniPathName);
//        }
//    }
//
//    if (m_pCMsImageSplitManager)
//    {
//        delete m_pCMsImageSplitManager;
//        m_pCMsImageSplitManager = NULL;
//    }
//}
//
//// 从资源中提取文件
//BOOL CGameClientView::CreateFileFromRes(INT ID, TCHAR* strResType, TCHAR* szResFilePath)
//{
//    // 布尔类型返回值
//    BOOL bRet = TRUE;
//
//    // 资源句柄
//    HRSRC hRes = NULL;
//
//    TCHAR szResID[MAX_PATH] = {0};
//    // 获取ID的字符串形式
//
//    _stprintf_s(szResID, MAX_PATH, _T("#%d"), ID);
//
//    // 根据ID与类型查找资源
//    hRes = ::FindResource(::GetModuleHandle(NULL), szResID, strResType);
//    ::GetLastError();
//
//    // 装载资源
//    HGLOBAL hLoadRes = ::LoadResource(theApp.m_hInstance, hRes);
//
//    // 锁定资源
//    LPVOID  lpLockRes = ::LockResource(hLoadRes);
//
//    // 创建资源文件
//    HANDLE hResFile = ::CreateFile(
//        szResFilePath,
//        GENERIC_WRITE,
//        0,
//        NULL,
//        CREATE_ALWAYS,
//        0,
//        NULL
//        );
//
//    // 获取资源尺寸
//    DWORD nResSize = ::SizeofResource(NULL, hRes);
//
//    // 资源写入文件
//    bRet = ::WriteFile(hResFile, lpLockRes, nResSize, &nResSize, NULL);
//
//    // 关闭文件句柄
//    bRet = ::CloseHandle(hResFile);
//    hResFile = NULL;
//
//    // 返回成功
//    return TRUE;
//}
//
//void* CGameClientView::LoadZFile(CONST ACHAR* MapFileName, DWORD dwImageIndex, OUT DWORD& dwSize)
//{
//    ACHAR    szName[MAX_PATH];
//    ACHAR    szZipName[MAX_PATH];
//    unzFile zip;
//    unz_file_info file_info;
//    INT     done;
//    BYTE*   ptr;
//
//    _stprintf_s(szName, MAX_PATH, _T("%04d.dat"), dwImageIndex);
//
//    zip = unzOpen(MapFileName);
//    done = unzGoToFirstFile(zip);
//    //done = unzGoToNextFile(zip);
//    for (DWORD i = 0; i < dwImageIndex; i++)
//    {
//        done = unzGoToNextFile(zip);
//        if (done != UNZ_OK)
//        {
//            AfxMessageBox(_T("场景中无此文件"));
//            return NULL;
//        }
//    }
//    unzGetCurrentFileInfo(zip, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
//    dwSize = file_info.uncompressed_size;
//    ACHAR* pTemp = strchr(szZipName, '/');
//    if (!pTemp)
//    {
//        return NULL;
//    }
//    pTemp++;
//    if(!strcmp(szName, pTemp))
//    {
//        if(unzOpenCurrentFilePassword(zip, NULL) != UNZ_OK)
//        {
//            unzClose(zip);
//            return NULL;
//        }
//
//        ptr = new BYTE[dwSize];
//        if(!ptr)
//        {
//            unzCloseCurrentFile(zip);
//            unzClose(zip);
//            return NULL;
//        }
//
//        if(unzReadCurrentFile(zip, ptr, dwSize) < 0)
//        {
//            unzCloseCurrentFile(zip);
//            unzClose(zip);
//            delete[] ptr;
//            ptr = NULL;
//            return NULL;
//        }
//        unzCloseCurrentFile(zip);
//        unzClose(zip);
//        return ptr;
//    }
//    else
//    {
//        AfxMessageBox(_T("场景文件错误"));
//        return NULL;
//    }
//    return NULL;
//}
//
//VOID CGameClientView::FreeZFile(void* pData)
//{
//    if (pData)
//    {
//        delete[] pData;
//    }
//}
//
//void CGameClientView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//    m_bFreeShow = TRUE;
//    SCROLLINFO ScrollInfo;
//    this->GetScrollInfo(SB_HORZ, &ScrollInfo);
//    switch (nSBCode)
//    {
//    case SB_LINELEFT:
//        {
//            ScrollInfo.nPos--;
//        }break;
//    case SB_LINERIGHT:
//        {
//            ScrollInfo.nPos++;
//        }break;
//    case SB_LEFT:
//        {
//            ScrollInfo.nPos-=10;
//        }break;
//    case SB_RIGHT:
//        {
//            ScrollInfo.nPos+=10;
//        }break;
//    case SB_PAGELEFT:
//        {
//            ScrollInfo.nPos -= m_dwMapSizeWidth/100;
//        }break;
//    case SB_PAGERIGHT:
//        {
//            ScrollInfo.nPos += m_dwMapSizeWidth/100;
//            m_dwMapSizeHeight;
//        }break;
//    case SB_THUMBPOSITION:
//        {
//            ScrollInfo.nPos = nPos;
//        }break;
//    default:
//        {
//            ;
//        }
//    }
//    this->InvalidateRect(NULL, TRUE);
//    this->SetScrollInfo(SB_HORZ, &ScrollInfo);
//}
//
//void CGameClientView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//    m_bFreeShow = TRUE;
//    SCROLLINFO ScrollInfo;
//    this->GetScrollInfo(SB_VERT, &ScrollInfo);
//    switch (nSBCode)
//    {
//    case SB_LINEUP:
//        {
//            ScrollInfo.nPos--;
//        }break;
//    case SB_LINEDOWN:
//        {
//            ScrollInfo.nPos++;
//        }break;
//    case SB_TOP:
//        {
//            ScrollInfo.nPos-=10;
//        }break;
//    case SB_BOTTOM:
//        {
//            ScrollInfo.nPos+=10;
//        }break;
//    case SB_PAGEUP:
//        {
//            ScrollInfo.nPos -= m_dwMapSizeWidth/100;
//        }break;
//    case SB_PAGEDOWN:
//        {
//            ScrollInfo.nPos += m_dwMapSizeWidth/100;
//            m_dwMapSizeHeight;
//        }break;
//    case SB_THUMBPOSITION:
//        {
//            ScrollInfo.nPos = nPos;
//        }break;
//    default:
//        {
//            ;
//        }
//    }
//    this->InvalidateRect(NULL, TRUE);
//    this->SetScrollInfo(SB_VERT, &ScrollInfo);
//}
//
//BOOL CGameClientView::InitSubMap(VOID)
//{
//    HANDLE hIniFile = ::CreateFileA(
//        m_szIniPathName,
//        FILE_READ_DATA,
//        0,
//        NULL,
//        OPEN_EXISTING,
//        0,
//        NULL);
//    if (INVALID_HANDLE_VALUE == hIniFile)
//    {
//        AfxMessageBox(_T("打开/Map/World.ini配置文件失败!"));
//        return FALSE;
//    }
//    else
//    {
//        ::CloseHandle(hIniFile);
//        hIniFile = INVALID_HANDLE_VALUE;
//
//        ACHAR szTemp[MAX_PATH] = {0};
//
//        // 获取图片个数
//        if(::GetPrivateProfileStringA("MapInfo", "ImageCount", _T(""), szTemp, MAX_PATH, m_szIniPathName))
//        {
//            m_dwImageCount = atoi(szTemp);
//        }
//        else
//        {
//            AfxMessageBox(_T("读取/Map/World.ini->MapInfo->ImageCount失败!"));
//            return FALSE;
//        }
//
//        // 获取整个场景宽度
//        if(::GetPrivateProfileStringA("MapInfo", "MapWidth", _T(""), szTemp, MAX_PATH, m_szIniPathName))
//        {
//            m_dwMapSizeWidth = atoi(szTemp);
//        }
//        else
//        {
//            AfxMessageBox(_T("读取/Map/World.ini->MapInfo->MapWidth失败!"));
//            return FALSE;
//        }
//
//        // 获取整个场景高度
//        if(::GetPrivateProfileStringA("MapInfo", "MapHeight", _T(""), szTemp, MAX_PATH, m_szIniPathName))
//        {
//            m_dwMapSizeHeight = atoi(szTemp);
//        }
//        else
//        {
//            AfxMessageBox(_T("读取/Map/World.ini->MapInfo->MapHeight失败!"));
//            return FALSE;
//        }
//
//        // 获取横排图片个数
//        if(::GetPrivateProfileStringA("MapInfo", "ImageCountByX", _T(""), szTemp, MAX_PATH, m_szIniPathName))
//        {
//            m_dwSubImageCountByX = atoi(szTemp);
//        }
//        else
//        {
//            AfxMessageBox(_T("读取/Map/World.ini->MapInfo->ImageCountByX失败!"));
//            return FALSE;
//        }
//
//        // 获取纵排图片个数
//        if(::GetPrivateProfileStringA("MapInfo", "ImageCountByX", _T(""), szTemp, MAX_PATH, m_szIniPathName))
//        {
//            m_dwSubImageCountByY = atoi(szTemp);
//        }
//        else
//        {
//            AfxMessageBox(_T("读取/Map/World.ini->MapInfo->ImageCountByX失败!"));
//            return FALSE;
//        }
//
//        m_lpSizeArray = new SIZE[m_dwImageCount];
//        if (!m_lpSizeArray)
//        {
//            AfxMessageBox(_T("内存不足"));
//            return FALSE;
//        }
//        for (DWORD i = 0; i < m_dwImageCount; i++)
//        {
//            ACHAR szItemName[MAX_PATH] = {0};
//            sprintf_s(szItemName, MAX_PATH, "X_%d", i);
//            // 获取纵排图片个数
//            if(::GetPrivateProfileStringA("SizeArray", szItemName, _T(""), szTemp, MAX_PATH, m_szIniPathName))
//            {
//                m_lpSizeArray[i].cx = atoi(szTemp);
//            }
//            else
//            {
//                ::MessageBoxA(m_hWnd, "读取/Map/World.ini->SizeArray失败!", szItemName, MB_OK);
//                return FALSE;
//            }
//
//            sprintf_s(szItemName, MAX_PATH, "Y_%d", i);
//            // 获取纵排图片个数
//            if(::GetPrivateProfileStringA("SizeArray", szItemName, _T(""), szTemp, MAX_PATH, m_szIniPathName))
//            {
//                m_lpSizeArray[i].cy = atoi(szTemp);
//            }
//            else
//            {
//                ::MessageBoxA(m_hWnd, "读取/Map/World.ini->SizeArray失败!", szItemName, MB_OK);
//                return FALSE;
//            }
//        }
//    }
//
//    SCROLLINFO ScrollInfo;
//    this->GetScrollInfo(SB_VERT, &ScrollInfo);
//    ScrollInfo.nMax = m_dwMapSizeHeight;
//    this->SetScrollInfo(SB_VERT, &ScrollInfo);
//
//    this->GetScrollInfo(SB_HORZ, &ScrollInfo);
//    ScrollInfo.nMax = m_dwMapSizeWidth;
//    this->SetScrollInfo(SB_HORZ, &ScrollInfo);
//
//
//    m_pbmpMap = new CBitmapEx[m_dwImageCount];
//    if (!m_pbmpMap)
//    {
//        ASSERT(m_pbmpMap);
//    }
//
//    DWORD dwImageSize;
//    VOID* pImage = NULL;
//    TCHAR szMapPathName[MAX_PATH] = {0};
//    strcpy(szMapPathName, m_szCurrentPath);
//    strcat(szMapPathName, "/Map/World.Romap");
//
//    for (DWORD i = 0; i < m_dwImageCount; i++)
//    {
//        VOID* pImage = this->LoadZFile(szMapPathName, i, dwImageSize);
//        if (pImage)
//        {
//            IPicture* pPicture = m_pbmpMap[i].LoadFromBuffer((BYTE*)pImage, dwImageSize);
//            pPicture->get_Handle((OLE_HANDLE*)&m_pbmpMap[i].m_hObject);
//            this->FreeZFile(pImage);
//            pImage = NULL;
//        }
//    }
//    return TRUE;
//}
//
//
//#pragma once                        //┃//┃
//#ifndef __MSIMAGESPLITMANAGER_H__   //┃//┃
//#define __MSIMAGESPLITMANAGER_H__   //┃//┃
////━━━━━━━━━━━━━━━━━━┛//┃
////文件名称：MsImageSplitManager.h       //┃
////功能描述：图片拆分管理器              //┃
////平台相关：WINDOWS                     //┃
////━━━━━━━━━━━━━━━━━━━━┛
//
//// 子场景图片最小尺寸
//#define SUB_MAP_LEAST_SIZE  400
//
//class CMsImageSplitManager
//{
//public:
//
//    // 构造、析构
//    CMsImageSplitManager(DWORD dwSubMapSize = SUB_MAP_LEAST_SIZE);
//    virtual ~CMsImageSplitManager(VOID);
//
//    BOOL    OpenImageFile(CONST TCHAR* szMapFileName);    // 打开图片
//    //VOID    ZoomOutMainImage(FLOAT fZoom);          // 放大主图片
//    DWORD   SplitImageFile(VOID);                   // 拆分图片
//    VOID    CloseAllImage(VOID);                    // 关闭所有图片
//    DWORD   GetSplitImageSize(VOID){return m_dwSubImageSize;}
//    VOID    GetMainImageSize(DWORD& dwWidth, DWORD& dwHeight);
//    VOID    SaveAllSubImage(TCHAR* szFilePath);
//    DWORD   GetSubImageCount(VOID){return m_dwSubImageCount;}
//    DWORD   GetSubImageCountByX(VOID){return m_dwSubImageCountByX;}
//    DWORD   GetSubImageCountByY(VOID){return m_dwSubImageCountByY;}
//    VOID    GetSubImageSize(DWORD dwIndex, DWORD& dwWidth, DWORD& dwHeight);
//
//    // 拷贝主图片指定区域到目标图片指定位置
//    BOOL    CopyImageInMainImageRect(CxImage* pDesImage, LPRECT lpSrcRc, LONG X, LONG Y, BOOL bCopyAlpha = TRUE);
//
//    // 画主图片
//    BOOL DrawMainImage(HDC hDc, LONG X, LONG Y);
//
//    // 根据索引画子图片
//    BOOL DrawSubImage(HDC hDc, DWORD dwIndex, LONG X, LONG Y);
//
//private:
//
//    CxImage*    m_pMainImage;           // 主图片对象
//    CxImage*    m_pSubImage;            // 子图片对象
//    LPSIZE      m_lpSizeArray;          // 每个子图片尺寸
//    DWORD       m_dwSubImageSize;       // 子图片尺寸
//    DWORD       m_dwSubImageCountByX;   // 每个横排子图片个数
//    DWORD       m_dwSubImageCountByY;   // 每个纵排子图片个数
//    DWORD       m_dwSubImageCount;      // 子图片个数
//};
//
//#include "StdAfx.h"
//#include "xImage/ximage.h"
//#include "MsImageSplitManager.h"
//
//CMsImageSplitManager::CMsImageSplitManager(DWORD dwSubMapSize)
//: m_pSubImage               (NULL)
//, m_dwSubImageSize          (dwSubMapSize)
//, m_dwSubImageCount         (0)
//, m_lpSizeArray             (NULL)
//{
//    if (dwSubMapSize < SUB_MAP_LEAST_SIZE)
//    {
//        dwSubMapSize = SUB_MAP_LEAST_SIZE;
//    }
//    m_pMainImage = new CxImage;
//    if (!m_pMainImage)
//    {
//        ::MessageBox(::GetActiveWindow(), _T("内存不足"), _T("失败"), MB_OK);
//    }
//}
//
//CMsImageSplitManager::~CMsImageSplitManager(VOID)
//{
//
//    this->CloseAllImage();
//    if (m_pMainImage)
//    {
//        m_pMainImage->Destroy();
//        delete m_pMainImage;
//        m_pMainImage = NULL;
//    }
//}
//
//// 打开图片
//BOOL CMsImageSplitManager::OpenImageFile(CONST TCHAR* szMapFileName)
//{
//    BOOL bRet = m_pMainImage->Load(szMapFileName, CXIMAGE_SUPPORT_BMP);
//    if (!bRet)
//    {
//        ::MessageBox(::GetActiveWindow(), szMapFileName, _T("打开场景图片失败"), MB_OK);
//    }
//    return bRet;
//}
//
////// 放大主图片
////VOID CMsImageSplitManager::ZoomOutMainImage(FLOAT fZoom)
////{
////    CxImage* pImage = new CxImage;
////    if (!pImage)
////    {
////        ::MessageBox(::GetActiveWindow(), _T("内存不足"), _T("失败"), MB_OK);
////    }
////
////    DWORD dwDesWidth = (DWORD)(m_pMainImage->GetWidth() * fZoom);
////    DWORD dwDesHeight = (DWORD)(m_pMainImage->GetHeight() * fZoom);
////    pImage->Create(dwDesWidth, dwDesHeight, m_pMainImage->GetBpp(), m_pMainImage->GetType());
////    pImage->Save(_T("{884e200c-217d-11da-b2a4-000e7bbb2b09-884e200b-217d-11da-b2a4-000e7bbb2b09}"), CXIMAGE_FORMAT_BMP);
////    delete pImage;
////    pImage = NULL;
////
////    HANDLE hImage = ::LoadImage(NULL,
////        _T("{884e200c-217d-11da-b2a4-000e7bbb2b09-884e200b-217d-11da-b2a4-000e7bbb2b09}"),
////        IMAGE_BITMAP,
////        LR_DEFAULTSIZE,
////        LR_DEFAULTSIZE,
////        LR_LOADFROMFILE
////        );
////
////    HWND hWnd = ::GetActiveWindow();
////    HDC hMainDc = ::GetDC(hWnd);
////    HDC hDc = ::CreateCompatibleDC(hMainDc);
////    ::SelectObject(hDc, hImage);
////    m_pMainImage->Draw(hDc, 0, 0, dwDesWidth, dwDesHeight);
////
////    //m_pMainImage->Draw()
////
////    delete m_pMainImage;
////    m_pMainImage = NULL;
////    m_pMainImage = new CxImage;
////    if (!m_pMainImage)
////    {
////        ::MessageBox(::GetActiveWindow(), _T("内存不足"), _T("失败"), MB_OK);
////    }
////    m_pMainImage->CreateFromHANDLE(hDc);
////
////    ::DeleteDC(hDc);
////    ::ReleaseDC(hWnd, hMainDc);
////}
//
//// 拆分图片
//DWORD CMsImageSplitManager::SplitImageFile(VOID)
//{
//    //if (fZoom != 1.0f)
//    //{
//    //    this->ZoomOutMainImage(fZoom);
//    //}
//
//    // 获取分割后横排子图片个数
//    m_dwSubImageCountByX = m_pMainImage->GetWidth() / m_dwSubImageSize + 1;
//    
//    // 获取分割后纵排子图片个数
//    m_dwSubImageCountByY = m_pMainImage->GetHeight() / m_dwSubImageSize + 1;
//
//    // 计算子图片总个数
//    m_dwSubImageCount = m_dwSubImageCountByX * m_dwSubImageCountByY;
//
//    // 计算横排最后一个子图片需要显示的宽度
//    DWORD dwLastSubImageWidthByX    = SUB_MAP_LEAST_SIZE - (m_dwSubImageCountByX * SUB_MAP_LEAST_SIZE - m_pMainImage->GetWidth());
//    
//    // 计算纵排最后一个子图片需要显示的高度
//    DWORD dwLastSubImageHeightByY   = SUB_MAP_LEAST_SIZE - (m_dwSubImageCountByY * SUB_MAP_LEAST_SIZE - m_pMainImage->GetHeight());
//
//    // 申请子图片
//    m_pSubImage = new CxImage[m_dwSubImageCount];
//    if (!m_pSubImage)
//    {
//        ::MessageBox(::GetActiveWindow(), _T("申请内存失败"), _T("内存不足"), MB_OK);
//        return -1;
//    }
//
//    // 申请子图片偏移数组
//    m_lpSizeArray = new SIZE[m_dwSubImageCount];
//    if (!m_lpSizeArray)
//    {
//        ::MessageBox(::GetActiveWindow(), _T("申请内存失败"), _T("内存不足"), MB_OK);
//        return -1;
//    }
//
//    // 获取图片颜色深度
//    DWORD dwImageBpp    = m_pMainImage->GetBpp();
//    DWORD dwImageType   = m_pMainImage->GetType();
//
//
//    // 获取
//    for (DWORD dwIndex = 0; dwIndex < m_dwSubImageCount; dwIndex++)
//    {
//        DWORD dwCurrentSubImageWidth = m_dwSubImageSize;
//        DWORD dwCurrentSubImageHeight = m_dwSubImageSize;
//
//        // 如果是最后一个子图片
//        if ((dwIndex+1) == m_dwSubImageCount)
//        {
//            // 创建子图片
//            m_pSubImage[dwIndex].Create(dwLastSubImageWidthByX, dwLastSubImageHeightByY, dwImageBpp, dwImageType);
//            dwCurrentSubImageWidth = dwLastSubImageWidthByX;
//            dwCurrentSubImageHeight = dwLastSubImageHeightByY;
//        }
//
//        // 如果是横排最后一个子图片
//        else if (0 != dwIndex && 0 == (dwIndex+1)%(m_dwSubImageCountByX))
//        {
//            // 创建子图片
//            m_pSubImage[dwIndex].Create(dwLastSubImageWidthByX, m_dwSubImageSize, dwImageBpp, dwImageType);
//            dwCurrentSubImageWidth = dwLastSubImageWidthByX;
//        }
//
//        // 如果是纵排最后一个子图片
//        else if ((dwIndex+1) > ((m_dwSubImageCountByY - 1) * m_dwSubImageCountByX))
//        {
//            // 创建子图片
//            m_pSubImage[dwIndex].Create(m_dwSubImageSize, dwLastSubImageHeightByY, dwImageBpp, dwImageType);
//            dwCurrentSubImageHeight = dwLastSubImageHeightByY;
//        }
//
//        // 其他子图片
//        else
//        {
//            // 创建子图片
//            m_pSubImage[dwIndex].Create(m_dwSubImageSize, m_dwSubImageSize, dwImageBpp, dwImageType);
//        }
//
//
//        m_lpSizeArray[dwIndex].cx = dwCurrentSubImageWidth;
//        m_lpSizeArray[dwIndex].cy = dwCurrentSubImageHeight;
//
//        RECT  rc = {0,0,0,0};
//        rc.left     = (dwIndex%m_dwSubImageCountByX)*m_dwSubImageSize;
//        rc.top      = (dwIndex/m_dwSubImageCountByX)*m_dwSubImageSize;
//        rc.right    = rc.left + dwCurrentSubImageWidth;
//        rc.bottom   = rc.top  + dwCurrentSubImageHeight;
//        this->CopyImageInMainImageRect(&m_pSubImage[dwIndex], &rc, 0, 0, FALSE);
//    }
//    // 返回子图片个数
//    return m_dwSubImageCount;
//}
//
//// 关闭所有图片
//VOID CMsImageSplitManager::CloseAllImage(VOID)
//{
//    // 释放子图片数组
//    if (m_pSubImage)
//    {
//        for (DWORD i = 0; i < m_dwSubImageCount; i++)
//        {
//            m_pSubImage[i].Destroy();
//
//        }
//        delete[] m_pSubImage;
//        m_pSubImage = NULL;
//    }
//
//    // 释放偏移数组
//    if (m_lpSizeArray)
//    {
//        delete[] m_lpSizeArray;
//        m_lpSizeArray = NULL;
//    }
//}
//
//// 画主图片
//BOOL CMsImageSplitManager::DrawMainImage(HDC hDc, LONG X, LONG Y)
//{
//    // 画主图片
//    //FLOAT fDrawCX = fZoom * m_pMainImage->GetWidth();
//    //FLOAT fDrawCY = fZoom * m_pMainImage->GetHeight();
//    INT nRet = m_pMainImage->Draw(hDc, X, Y);
//    if (nRet)
//    {
//        return TRUE;
//    }
//    else
//    {
//        return FALSE;
//    }
//}
//
//// 根据索引画子图片
//BOOL CMsImageSplitManager::DrawSubImage(HDC hDc, DWORD dwIndex, LONG X, LONG Y)
//{
//    if (dwIndex >= m_dwSubImageCount)
//    {
//        ::MessageBox(::GetActiveWindow(), _T("指定的子图片不存在"), _T("提示"), MB_OK);
//        return FALSE;
//    }
//    if (!m_pSubImage)
//    {
//        ::MessageBox(::GetActiveWindow(), _T("调用错误"), _T("提示"), MB_OK);
//        return FALSE;
//    }
//
//    //LONG lDrawCX = /*fZoom * */m_pSubImage[dwIndex].GetWidth();
//    //LONG lDrawCY = /*fZoom * */m_pSubImage[dwIndex].GetHeight();
//
//    
//    LONG lRealDrawX = m_lpSizeArray[dwIndex].cx/* * fZoom*/;
//    LONG lRealDrawY = m_lpSizeArray[dwIndex].cy/* * fZoom*/;
//
//    INT nRet = m_pSubImage[dwIndex].Draw(hDc, X+lRealDrawX, Y+lRealDrawY);
//
//    if (nRet)
//    {
//        return TRUE;
//    }
//    else
//    {
//        return FALSE;
//    }
//}
//
//// 拷贝主图片指定区域到目标图片指定位置
//BOOL CMsImageSplitManager::CopyImageInMainImageRect(CxImage* pDesImage, LPRECT lpSrcRc, LONG DesX, LONG DesY, BOOL bCopyAlpha)
//{
//    if (!pDesImage)
//    {
//        return FALSE;
//    }
//    DWORD dwMainImageWidth = m_pMainImage->GetWidth();        // 主图片宽度
//    DWORD dwMainImageHeight = m_pMainImage->GetHeight();      // 主图片高度
//    DWORD dwSrcBeginX = lpSrcRc->left;                      // 转换主图片起始位置X
//    DWORD dwSrcBeginY = dwMainImageHeight - lpSrcRc->bottom;// 转换主图片起始位置Y
//    DWORD dwCopyWidth = lpSrcRc->right - lpSrcRc->left;     // 复制宽度
//    DWORD dwCopyHeight = lpSrcRc->bottom - lpSrcRc->top;    // 复制高度
//
//    RGBQUAD rgba;
//    for (DWORD x = 0; x < dwCopyWidth; x++)
//    {
//        for (DWORD y = 0; y < dwCopyHeight; y++)
//        {
//            rgba = m_pMainImage->GetPixelColor(dwSrcBeginX+x, dwSrcBeginY+y, bCopyAlpha?true:false);
//            pDesImage->SetPixelColor(DesX+x, DesY+y, rgba, bCopyAlpha?true:false);
//        }
//    }
//    return TRUE;
//}
//
//VOID CMsImageSplitManager::GetMainImageSize(DWORD& dwWidth, DWORD& dwHeight)
//{
//    dwWidth = m_pMainImage->GetWidth();
//    dwHeight = m_pMainImage->GetHeight();
//}
//
//VOID CMsImageSplitManager::SaveAllSubImage(TCHAR* szFilePath)
//{
//    TCHAR szSubImageName[MAX_PATH] = {0};
//    for (DWORD i = 0; i < m_dwSubImageCount; i++)
//    {
//        _stprintf_s(szSubImageName, "%s%04d.dat", szFilePath, i);
//        m_pSubImage[i].Save(szSubImageName, CXIMAGE_FORMAT_BMP);
//    }
//}
//
//VOID CMsImageSplitManager::GetSubImageSize(DWORD dwIndex, DWORD& dwWidth, DWORD& dwHeight)
//{
//    dwWidth = m_lpSizeArray[dwIndex].cx;
//    dwHeight = m_lpSizeArray[dwIndex].cy;
//}

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛

