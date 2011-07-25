#pragma once                //┃//┃
#ifndef __MSBASEIMAGE_H__   //┃//┃
#define __MSBASEIMAGE_H__   //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsBaseImage.h       //┃
//功能描述：图片基类            //┃
//平台相关：WINDOWS             //┃
//━━━━━━━━━━━━━━━━┛

class CMsBaseImage
{
public:
    // 构造、析构
    CMsBaseImage(VOID);
    virtual ~CMsBaseImage(VOID);

    // 打开图片
    virtual BOOL Open(TCHAR* szImageFileName) = 0;

    // 保存图片
    virtual BOOL Save(TCHAR* szImageFileName) = 0;

    // 关闭图片
    virtual VOID Close(VOID) = 0;

    // 绘制图片
    virtual BOOL Draw(HDC hDc, INT x, INT y, RECT DesRc) = 0;
private:
};



//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
