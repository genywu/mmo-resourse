#pragma once                    //┃//┃
#ifndef __MSBASEERRORCODE_H__   //┃//┃
#define __MSBASEERRORCODE_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsBaseErrorCode.h       //┃
//功能描述：基本错误定义            //┃
//平台相关：WINDOWS                 //┃
//━━━━━━━━━━━━━━━━━━┛

enum E_MS_ERROR
{
    // 操作失败,未知错误
    MS_ERROR_UNKNOW             = 0xFFFFFFFF,

    // 操作成功,没有错误
    MS_ERROR_SUCCEED            = 0x00000000,

    MS_ERROR_EXCEPTION,                 // 操作失败,发生异常
    MS_ERROR_ACCESS,                    // 操作失败,权限错误
    MS_ERROR_SUB_CALL_ACCESS,           // 操作失败,调用权限错误

    MS_ERROR_BUFFER_NOT_ENOUGH_FREE,    // 操作失败,缓冲区中没有足够的空余空间
    MS_ERROR_BUFFER_NOT_ENOUGH_DATA,    // 操作失败,缓冲区中没有足够的数据可用

    MS_ERROR_INVALID_PARAMETER,         // 操作失败,参数无效
    MS_ERROR_NO_INIT,                   // 操作失败,对象没有进行初始化

    MS_ERROR_MAX_COUNT                  // 最大错误个数
};

extern LPTCHAR  szMsErrorCodeName[MS_ERROR_MAX_COUNT+2];
extern LPTCHAR  szMsErrorCodeInfo[MS_ERROR_MAX_COUNT+2];
TCHAR* GetErrorCodeName(E_MS_ERROR eMsError);
TCHAR* GetErrorCodeInfo(E_MS_ERROR eMsError);

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
