#pragma once            //┃//┃
#ifndef __MSSTRING_H__  //┃//┃
#define __MSSTRING_H__  //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsString.h      //┃
//功能描述：字符串操作封装  //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

class CMsString
{
public:

    // 字符类型
    enum E_CHAR_TYPE
    {
        CY_A_CHAR,  // 多字符
        CY_W_CHAR,  // 宽字符
    };

    // 构造
    CMsString(DWORD dwMaxBuffLen, BOOL bChangeCharacterSet = FALSE);  

    // 多字符构造
    CMsString(LPCHAR szText, DWORD dwMaxBuffLen, BOOL bChangeCharacterSet = FALSE);           

    // 宽字符构造
    CMsString(LPWCHAR szText, DWORD dwMaxBuffLen, BOOL bChangeCharacterSet = FALSE);          

    // 析构
    ~CMsString(VOID);

    // 获取多字符串
    operator LPCHAR();

    // 获取宽字符串
    operator LPWCHAR();

    // 获取字符串长度(字符个数)
    size_t GetTextLen(VOID);   

private:
    LPCHAR      m_strTextA;             // 多字符串
    LPWCHAR     m_strTextW;             // 宽字符串
    DWORD       m_dwMaxBuffLen;         // 字符串缓冲最大尺寸
    BOOL        m_bChangeCharacterSet;  // 是否允许转换字符集
    size_t      m_stTextLen;            // 字符串长度
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
