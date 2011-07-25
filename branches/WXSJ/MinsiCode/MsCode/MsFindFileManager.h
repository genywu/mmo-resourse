#pragma once                    //┃      //┃
#ifndef __FINDFILEMANAGER_H__   //┃      //┃
#define __FINDFILEMANAGER_H__   //┃      //┃
//━━━━━━━━━━━━━━━━┛      //┃
//文件名称：FindFileManager.h             //┃
//功能描述：目录遍历文件操作相关封装      //┃
//平台相关：WINDOWS                       //┃
//━━━━━━━━━━━━━━━━━━━━━┛

class CMsFindFileManager
{
public:
    CMsFindFileManager(VOID);
    virtual ~CMsFindFileManager(VOID);

    // 设置搜索根目录
    BOOL            SetRootPath(CONST TCHAR* strPath = NULL);

    // 设置搜索文件类型
    BOOL            SetFindFileType(CONST TCHAR* strFileType);

    // 遍历目录下由指定类型的文件
    BOOL            BrowsePath(VOID);

protected:

    // 检查搜索文件类型是否匹配
    BOOL            CheckFileType(CONST TCHAR* strPath);

    // 路径默认追加指定字符
    BOOL            AppPath(TCHAR* strPath, TCHAR cChr = _T('\\'));

    // 遍历函数每找到一个文件,就调用ProcessFile,并把文件名作为参数传递过去
    // 用户可以覆写该函数,加入自己的处理代码
    virtual BOOL    ProcessFile(CONST TCHAR*) = 0;

    // 遍历函数每进入一个目录,就调用ProcessPath,
    // 并把正在处理的目录名作为参数传递过去,
    // 用户可以覆写该函数,加入自己的处理代码
    virtual BOOL    ProcessPath(CONST TCHAR*) = 0;

protected:
    // 存放当前可执行文件路径
    TCHAR           m_szCurrentDirectory[MAX_PATH];

    // ----------------------------------------
    // 搜索根目录
    TCHAR           m_szRootPath[MAX_PATH];
    // 搜索根目录长度
    INT             m_nRootPathLen;
    // ----------------------------------------


    // ----------------------------------------
    // 搜索文件类型
    TCHAR           m_szFileType[MAX_PATH];
    // 搜索文件类型长度
    INT             m_nFileTypeLen;
    // ----------------------------------------


    // ----------------------------------------
    // 需要查找的具体路径与类型的字符串
    TCHAR           m_szRealFindPathName[MAX_PATH];

    // 需要查找的具体路径与类型的字符串
    INT             m_nRealPathLen;
    // ----------------------------------------

    // 查找文件结果返回结构体
    WIN32_FIND_DATA m_FindFileData;
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
