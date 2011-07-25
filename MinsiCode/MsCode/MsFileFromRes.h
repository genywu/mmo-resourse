#pragma once              //┃            //┃
#ifndef __FILEFROMRES_H__ //┃            //┃
#define __FILEFROMRES_H__ //┃            //┃
//━━━━━━━━━━━━━┛            //┃
//文件名称：FileFromRes.h                 //┃
//功能描述：文件从资源中释放相关操作封装  //┃
//平台相关：WINDOWS                       //┃
//━━━━━━━━━━━━━━━━━━━━━┛

class CMsFileFromRes
{
public:
    CMsFileFromRes(HINSTANCE hInstance);
    virtual ~CMsFileFromRes(VOID);

    // 公有接口
public:

    // 创建文件从资源
    BOOL    CreateFileFromRes(INT ID, TCHAR* strResType, TCHAR* szFilePath);

    // 加载资源
    LPVOID  LoadFileFromRes(INT ID, TCHAR* strResType, DWORD* lpdwFileSize);

    // 保护属性
protected:

    // 当前应用程序实例句柄
    HINSTANCE   m_hInstance;

    // 资源类型字符串
    TCHAR       m_strResType[MAX_PATH];

    // 资源文件句柄
    HANDLE      m_hResFile;
};



//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
