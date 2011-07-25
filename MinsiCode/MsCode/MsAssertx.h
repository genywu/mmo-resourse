#pragma once              //┃  //┃
#ifndef __MSASSERTX_H__   //┃  //┃
#define __MSASSERTX_H__   //┃  //┃
//━━━━━━━━━━━━━┛  //┃
//文件名称：MsAssertx.h         //┃
//功能描述：基本断言定义        //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//断言日志文本最大长度                                  //┃
#define ASSERTX_STR_MAX_LEN 1256                        //┃
                                                        //┃
enum                                                    //┃
{                                                       //┃
    COMMAND_NORMAL_MSGBOX = 0,                          //┃
    COMMAND_IGNORE_MSGBOX,                              //┃
    COMMAND_NORMAL_LOG,                                 //┃
    COMMAND_IGNORE_LOG,                                 //┃
                                                        //┃
};                                                      //┃
														//┃
enum E_ASSERT_LEVEL                                     //┃
{                                                       //┃
	ASSERT_LEVEL_NORMAL = 0,							//┃
	ASSERT_LEVEL_CATASTROPHIC,                          //┃
	ASSERT_LEVEL_SHOW_ONLY,                             //┃
	ASSERT_LEVEL_IGNORE,								//┃
};                                                      //┃
#define AL_NORMAL		ASSERT_LEVEL_NORMAL				//┃
#define AL_CATAST		ASSERT_LEVEL_CATASTROPHIC		//┃
#define AL_SHOW_ONLY	ASSERT_LEVEL_SHOW_ONLY			//┃
#define AL_IGNORE		ASSERT_LEVEL_IGNORE				//┃
// 0:存储日志 1:忽略日志                                //┃
extern INT      g_Command_Assert_Log;                   //┃
                                                        //┃
// 0:使用MessageBox提示 1:忽略MessageBox提示            //┃
extern INT      g_Command_MessageBox;                   //┃
                                                        //┃
// 全局日志文件句柄                                     //┃
extern HANDLE   g_hAssertLogFile;                       //┃
                                                        //┃
// 全局是否已经初始化断言句柄                           //┃
extern BOOL     g_bAssertInit;                          //┃
                                                        //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

BOOL InitAssertx(TCHAR* szAssertFileName);
BOOL ReleaseAssertx(VOID* /*Parameter*/);

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//断言对话框T                                           	//┃
inline VOID MESSAGEBOX(										//┃
	CONST E_ASSERT_LEVEL eLevel,							//┃
	CONST TCHAR* msg										//┃
	)														//┃
{                                                       	//┃
	if (COMMAND_NORMAL_MSGBOX == g_Command_MessageBox)  	//┃
	{                                                   	//┃
		switch (eLevel)										//┃
		{													//┃
		// 标准断言											//┃
		case ASSERT_LEVEL_NORMAL:							//┃
			{												//┃
#ifdef _DEBUG                                           	//┃
				INT nRet = ::MessageBox(                	//┃
					NULL,                               	//┃
					msg,                                	//┃
					_T("【 是 】中断,")                 	//┃
					_T("【 否 】忽略.")                 	//┃
					_T("【取消】结束,"),                	//┃
					MB_YESNOCANCEL);                    	//┃
#else                                                   	//┃
				INT nRet = ::MessageBox(                	//┃
					NULL,                               	//┃
					msg,                                	//┃
					_T("【确定】忽略.")                 	//┃
					_T("【取消】结束,"),                	//┃
					MB_OKCANCEL);                       	//┃
#endif                                                  	//┃
				switch (nRet)                           	//┃
				{                                       	//┃
				case IDOK:                              	//┃
				case IDNO:                              	//┃
					{                                   	//┃
						;                               	//┃
					}break;                             	//┃
				case IDYES:                             	//┃
					{                                   	//┃
						_CrtDbgBreak();                 	//┃
					}break;                             	//┃
				default:                                	//┃
				case IDCANCEL:                          	//┃
					{										//┃
						static TCHAR MsgBuf[KB_SIZE] = {0}; //┃
						::FormatMessage(                    //┃
							FORMAT_MESSAGE_FROM_SYSTEM,     //┃
							NULL,                           //┃
							::GetLastError(),               //┃
							MAKELANGID(                     //┃
							LANG_NEUTRAL,                   //┃
							SUBLANG_CUSTOM_DEFAULT          //┃
							),                              //┃
							MsgBuf,                         //┃
							KB_SIZE,                        //┃
							NULL);                          //┃
						SIZE_T sMsgLen = _tcslen(MsgBuf);   //┃
						if (sMsgLen)                        //┃
						{                                   //┃
#ifdef UNICODE												//┃
							MsgBuf[sMsgLen-1] = _T('\0');   //┃
							MsgBuf[sMsgLen-2] = _T('\0');   //┃
#else														//┃
							MsgBuf[sMsgLen-1] = _T('\0');   //┃
#endif														//┃
						}                                   //┃
						::MessageBox(						//┃
							NULL,							//┃
							MsgBuf,							//┃
							_T("结束程序"),					//┃
							MB_OK);							//┃
						exit(INVALID_NID);					//┃
					}break;                                 //┃
				}											//┃
			}break;											//┃
		// 灾难性											//┃
		case ASSERT_LEVEL_CATASTROPHIC:						//┃
			{												//┃
				INT nRet = ::MessageBox(                    //┃
					NULL,                                   //┃
					msg,                                    //┃
					_T("灾难性断言")                        //┃
#ifdef _DEBUG												//┃
					_T("程序即将结束.")                     //┃
					_T("是否中断查看"),                     //┃
					MB_YESNO);								//┃
#else														//┃
					_T("程序即将结束."),                    //┃
					MB_OK);									//┃
#endif														//┃
				switch (nRet)                               //┃
				{                                           //┃
				case IDOK:                                  //┃
				case IDNO:                                  //┃
					{                                       //┃
						;                                   //┃
					}break;                                 //┃
				case IDYES:                                 //┃
					{                                       //┃
						_CrtDbgBreak();                     //┃
					}break;									//┃
				}											//┃
				static TCHAR MsgBuf[KB_SIZE] = {0};			//┃
				::FormatMessage(							//┃
					FORMAT_MESSAGE_FROM_SYSTEM,				//┃
					NULL,									//┃
					::GetLastError(),						//┃
					MAKELANGID(								//┃
					LANG_NEUTRAL,							//┃
					SUBLANG_CUSTOM_DEFAULT					//┃
					),										//┃
					MsgBuf,									//┃
					KB_SIZE,								//┃
					NULL);									//┃
				SIZE_T sMsgLen = _tcslen(MsgBuf);			//┃
				if (sMsgLen)								//┃
				{											//┃
#ifdef UNICODE												//┃
					MsgBuf[sMsgLen-1] = _T('\0');			//┃
					MsgBuf[sMsgLen-2] = _T('\0');			//┃
#else														//┃
					MsgBuf[sMsgLen-1] = _T('\0');			//┃
#endif														//┃
				}											//┃
				::MessageBox(								//┃
					NULL,									//┃
					MsgBuf,									//┃
					_T("结束程序"),							//┃
					MB_OK);									//┃
				exit(INVALID_NID);							//┃
		}break;												//┃
		// 仅提示											//┃
		case ASSERT_LEVEL_SHOW_ONLY:						//┃
			{												//┃
				;											//┃
				INT nRet = ::MessageBox(                    //┃
					NULL,                                   //┃
					msg,                                    //┃
#ifdef _DEBUG												//┃
					_T("提示断言")							//┃
					_T("是否中断查看")						//┃
					_T("【取消】忽略所有"),                 //┃
					MB_YESNOCANCEL);						//┃
#else														//┃
					_T("提示断言")							//┃
					_T("【取消】忽略所有"),                 //┃
					MB_OKCANCEL);							//┃
#endif														//┃
				switch (nRet)                               //┃
				{                                           //┃
				case IDOK:                                  //┃
				case IDNO:                                  //┃
					{                                       //┃
						;                                   //┃
					}break;                                 //┃
				case IDYES:                                 //┃
					{                                       //┃
						_CrtDbgBreak();                     //┃
					}break;									//┃
				case IDCANCEL:								//┃
					{										//┃
						g_Command_MessageBox =				//┃
							COMMAND_IGNORE_MSGBOX;			//┃
					}break;									//┃
				}											//┃
			}break;											//┃
		// 忽略提示											//┃
		case ASSERT_LEVEL_IGNORE:							//┃
			{												//┃
				;											//┃
			}break;											//┃
	}														//┃
}                                                   		//┃
}                                                       	//┃
//断言对话框A                                           	//┃
inline VOID MESSAGEBOXA(CONST ACHAR* msg)               	//┃
{                                                       	//┃
	if (COMMAND_NORMAL_MSGBOX == g_Command_MessageBox)  	//┃
	{                                                   	//┃
#ifdef _DEBUG                                           	//┃
		INT nRet = ::MessageBoxA(                       	//┃
			NULL,                                       	//┃
			msg,                                        	//┃
			"【 是 】中断,"                             	//┃
			"【 否 】忽略."                             	//┃
			"【取消】结束,",                            	//┃
			MB_YESNOCANCEL);                            	//┃
#else                                                   	//┃
		INT nRet = ::MessageBoxA(                       	//┃
			NULL,                                       	//┃
			msg,                                        	//┃
			"【确定】忽略."                             	//┃
			"【取消】结束,",                            	//┃
			MB_OKCANCEL);                               	//┃
#endif                                                  	//┃
		switch (nRet)                                   	//┃
		{                                               	//┃
		case IDOK:                                      	//┃
		case IDNO:                                      	//┃
			{                                           	//┃
				;                                       	//┃
			}break;                                     	//┃
		case IDYES:                                     	//┃
			{                                           	//┃
				_CrtDbgBreak();                         	//┃
			}break;                                     	//┃
		default:                                        	//┃
		case IDCANCEL:                                  	//┃
			{                                           	//┃
				static ACHAR MsgBuf[KB_SIZE] = {0};     	//┃
				::FormatMessageA(                       	//┃
					FORMAT_MESSAGE_FROM_SYSTEM,         	//┃
					NULL,                               	//┃
					::GetLastError(),                   	//┃
					MAKELANGID(                         	//┃
					LANG_NEUTRAL,                       	//┃
					SUBLANG_CUSTOM_DEFAULT              	//┃
					),                                  	//┃
					MsgBuf,                             	//┃
					KB_SIZE,                            	//┃
					NULL);                              	//┃
				SIZE_T sMsgLen = strlen(MsgBuf);        	//┃
				if (sMsgLen)                            	//┃
				{                                       	//┃
					MsgBuf[sMsgLen-1] = '\0';           	//┃
				}                                       	//┃
				if (IDYES == ::MessageBoxA(             	//┃
					NULL,                               	//┃
					MsgBuf,                             	//┃
					"结束程序？",                       	//┃
					MB_YESNO))                          	//┃
				{                                       	//┃
					exit(INVALID_NID);                  	//┃
				}                                       	//┃
			}break;                                     	//┃
		}                                               	//┃
	}                                                   	//┃
}                                                       	//┃
//断言对话框W                                           	//┃
inline VOID MESSAGEBOXW(CONST WCHAR* msg)               	//┃
{                                                       	//┃
	if (COMMAND_NORMAL_MSGBOX == g_Command_MessageBox)  	//┃
	{                                                   	//┃
#ifdef _DEBUG                                           	//┃
		INT nRet = ::MessageBoxW(                       	//┃
			NULL,                                       	//┃
			msg,                                        	//┃
			L"【 是 】中断,"                            	//┃
			L"【 否 】忽略."                            	//┃
			L"【取消】结束,",                           	//┃
			MB_YESNOCANCEL);                            	//┃
#else                                                   	//┃
		INT nRet = ::MessageBoxW(                       	//┃
			NULL,                                       	//┃
			msg,                                        	//┃
			L"【确定】忽略."                            	//┃
			L"【取消】结束,",                           	//┃
			MB_OKCANCEL);                               	//┃
#endif                                                  	//┃
		switch (nRet)                                   	//┃
		{                                               	//┃
		case IDOK:                                      	//┃
		case IDNO:                                      	//┃
			{                                           	//┃
				;                                       	//┃
			}break;                                     	//┃
		case IDYES:                                     	//┃
			{                                           	//┃
				_CrtDbgBreak();                         	//┃
			}break;                                     	//┃
		default:                                        	//┃
		case IDCANCEL:                                  	//┃
			{                                           	//┃
				static WCHAR MsgBuf[KB_SIZE] = {0};     	//┃
				::FormatMessageW(                       	//┃
					FORMAT_MESSAGE_FROM_SYSTEM,         	//┃
					NULL,                               	//┃
					::GetLastError(),                   	//┃
					MAKELANGID(                         	//┃
					LANG_NEUTRAL,                       	//┃
					SUBLANG_CUSTOM_DEFAULT              	//┃
					),                                  	//┃
					MsgBuf,                             	//┃
					KB_SIZE,                            	//┃
					NULL);                              	//┃
				SIZE_T sMsgLen = wcslen(MsgBuf);        	//┃
				if (sMsgLen)                            	//┃
				{                                       	//┃
					MsgBuf[sMsgLen-2] = L'\0';          	//┃
					MsgBuf[sMsgLen-1] = L'\0';          	//┃
				}                                       	//┃
				if (IDYES == ::MessageBoxW(             	//┃
					NULL,                               	//┃
					MsgBuf,                             	//┃
					L"结束程序？",                      	//┃
					MB_YESNO))                          	//┃
				{                                       	//┃
					exit(INVALID_NID);                  	//┃
				}                                       	//┃
			}break;                                     	//┃
		}                                               	//┃
	}                                                   	//┃
}                                                       	//┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
inline VOID LOG_(CONST TCHAR* szTemp)                                               //┃
{                                                                                   //┃
    // 静态断言显示锁                                                               //┃
    static MsLock lock;                                                             //┃
                                                                                    //┃
    // 日志缓冲区                                                                   //┃
    TCHAR szLog_t[ASSERTX_STR_MAX_LEN] = {0};                                       //┃
                                                                                    //┃
    // 成功写入日志的字节数                                                         //┃
    DWORD dwLogSize = 0;                                                            //┃
                                                                                    //┃
    // 检测断言是否已经初始化                                                       //┃
    if (!g_bAssertInit)                                                             //┃
    {                                                                               //┃
        MessageBox(NULL, _T("断言未初始化,将被忽略"), _T("断言"), MB_OK);           //┃
        g_bAssertInit = TRUE;                                                       //┃
        g_Command_Assert_Log = COMMAND_IGNORE_LOG;                                  //┃
    }                                                                               //┃
                                                                                    //┃
    if(COMMAND_NORMAL_LOG == g_Command_Assert_Log)                                  //┃
    {                                                                               //┃
        BOOL bRet = ::CreateDirectory(_T("Log"), NULL);                             //┃
        if (!bRet)                                                                  //┃
        {                                                                           //┃
            DWORD dwErrorCode = ::GetLastError();                                   //┃
            if (ERROR_ALREADY_EXISTS != dwErrorCode)                                //┃
            {                                                                       //┃
                _stprintf_s(                                                        //┃
                    szLog_t,                                                        //┃
                    ASSERTX_STR_MAX_LEN,                                            //┃
                    _T("断言日志目录创建失败！！！\n%d"),                           //┃
                    dwErrorCode                                                     //┃
                    );                                                              //┃
            }                                                                       //┃
        }                                                                           //┃
        {                                                                           //┃
            AUTO_LOCK(lock);                                                        //┃
                                                                                    //┃
            dwLogSize =                                                             //┃
                (DWORD)(_tcsnlen(szTemp, ASSERTX_STR_MAX_LEN)*sizeof(TCHAR));       //┃
                                                                                    //┃
            if (!::WriteFile(                                                       //┃
                g_hAssertLogFile,                                                   //┃
                szTemp,                                                             //┃
                dwLogSize,                                                          //┃
                &dwLogSize,                                                         //┃
                NULL                                                                //┃
                ))                                                                  //┃
            {                                                                       //┃
                _stprintf_s(                                                        //┃
                    szLog_t,                                                        //┃
                    ASSERTX_STR_MAX_LEN,                                            //┃
                    _T("断言日志写入失败！！！\n%d"),                               //┃
                    ::GetLastError()                                                //┃
                    );                                                              //┃
            }                                                                       //┃
        }                                                                           //┃
    }                                                                               //┃
}                                                                                   //┃
                                                                                    //┃
inline VOID ASSERT_(                                                                //┃
	CONST E_ASSERT_LEVEL eLevel,                                                    //┃
	CONST BOOL bThrow,                                                              //┃
    CONST TCHAR* file,                                                              //┃
    CONST DWORD  line,                                                              //┃
    CONST TCHAR* func,                                                              //┃
    CONST TCHAR* expr,                                                              //┃
    CONST TCHAR* str                                                                //┃
        )                                                                           //┃
{                                                                                   //┃
    // 检测断言是否已经初始化                                                       //┃
    if (!g_bAssertInit)                                                             //┃
    {                                                                               //┃
        MessageBox(NULL, _T("断言未初始化,将被忽略"), _T("断言"), MB_OK);           //┃
        g_bAssertInit = TRUE;                                                       //┃
        g_Command_Assert_Log = COMMAND_IGNORE_LOG;                                  //┃
    }                                                                               //┃
                                                                                    //┃
    if ((COMMAND_IGNORE_LOG == g_Command_Assert_Log) &&                             //┃
        (COMMAND_IGNORE_MSGBOX == g_Command_MessageBox))                            //┃
    {                                                                               //┃
        return;                                                                     //┃
    }                                                                               //┃
    TCHAR szTemp[ASSERTX_STR_MAX_LEN] = {0};                                        //┃
    UPDATA_LOCALTIME;                                                               //┃
                                                                                    //┃
    _stprintf_s(                                                                    //┃
        szTemp,                                                                     //┃
        ASSERTX_STR_MAX_LEN,                                                        //┃
        _T("文件: [%s]\n")                                                          //┃
        _T("行  : [%d]\n")                                                          //┃
        _T("函数: [%s]\n")                                                          //┃
        _T("原因: [%s]\n")                                                          //┃
        _T("异常: [%s]\n")                                                          //┃
        _T("时间: [%d年%d月%d日-星期%d")                                            //┃
        _T("[%d时%d分%d秒%d毫秒]\n"),                                               //┃
        file, line, func, expr, str,                                                //┃
        LOCALTIME_YEAR, LOCALTIME_MONTH, LOCALTIME_DAY, LOCALTIME_WEEK,             //┃
        LOCALTIME_HOUR, LOCALTIME_MINUTE, LOCALTIME_SECOND, LOCALTIME_MILLISECOND   //┃
        );                                                                          //┃
                                                                                    //┃
    LOG_(szTemp);                                                                   //┃
    MESSAGEBOX(eLevel, szTemp);                                                     //┃
    if (bThrow)                                                                     //┃
    {                                                                               //┃
        throw(expr);                                                                //┃
    }                                                                               //┃
}                                                                                   //┃
                                                                                    //┃
inline VOID ASSERTA_(                                                               //┃
	CONST E_ASSERT_LEVEL eLevel,                                                    //┃
	CONST BOOL bThrow,                                                              //┃
    CONST ACHAR* file,                                                              //┃
    CONST DWORD  line,                                                              //┃
    CONST ACHAR* func,                                                              //┃
    CONST ACHAR* expr,                                                              //┃
    CONST ACHAR* str                                                                //┃
    )                                                                               //┃
{                                                                                   //┃
    // 检测断言是否已经初始化                                                       //┃
    if (!g_bAssertInit)                                                             //┃
    {                                                                               //┃
        MessageBox(NULL, _T("断言未初始化,将被忽略"), _T("断言"), MB_OK);           //┃
        g_bAssertInit = TRUE;                                                       //┃
        g_Command_Assert_Log = COMMAND_IGNORE_LOG;                                  //┃
    }                                                                               //┃
                                                                                    //┃
    if ((COMMAND_IGNORE_LOG == g_Command_Assert_Log) &&                             //┃
        (COMMAND_IGNORE_MSGBOX == g_Command_MessageBox))                            //┃
    {                                                                               //┃
        return;                                                                     //┃
    }                                                                               //┃
    ACHAR szTempA[ASSERTX_STR_MAX_LEN] = {0};                                       //┃
#ifdef UNICODE                                                                      //┃
    TCHAR szTempT[ASSERTX_STR_MAX_LEN] = {0};                                       //┃
#endif                                                                              //┃
    UPDATA_LOCALTIME;                                                               //┃
                                                                                    //┃
    _snprintf_s(                                                                    //┃
        szTempA,                                                                    //┃
        sizeof(szTempA),                                                            //┃
        "文件: [%s]\n"                                                              //┃
        "行  : [%d]\n"                                                              //┃
        "函数: [%s]\n"                                                              //┃
        "原因: [%s]\n"                                                              //┃
        "异常: [%s]\n"                                                              //┃
        "时间: [%d年%d月%d日-星期%d"                                                //┃
        "[%d时%d分%d秒%d毫秒]\n",                                                   //┃
        file, line, func, expr, str,                                                //┃
        LOCALTIME_YEAR, LOCALTIME_MONTH, LOCALTIME_DAY, LOCALTIME_WEEK,             //┃
        LOCALTIME_HOUR, LOCALTIME_MINUTE, LOCALTIME_SECOND, LOCALTIME_MILLISECOND   //┃
        );                                                                          //┃
                                                                                    //┃
#ifdef UNICODE                                                                      //┃
    // 宽字符转换成多字符文件名字符串                                               //┃
    ACharToWChar(szTempA, szTempT, MAX_PATH);                                       //┃
    LOG_(szTempT);                                                                  //┃
    MESSAGEBOX(eLevel, szTempT);                                                    //┃
#else                                                                               //┃
    LOG_(szTempA);                                                                  //┃
    MESSAGEBOX(eLevel, szTempA);                                                    //┃
#endif                                                                              //┃
    if (bThrow)                                                                     //┃
    {                                                                               //┃
        throw(expr);                                                                //┃
    }                                                                               //┃
}                                                                                   //┃
                                                                                    //┃
inline VOID ASSERTEXFORMATT(                                                        //┃
	CONST E_ASSERT_LEVEL eLevel,                                                    //┃
	CONST BOOL bThrow,                                                              //┃
    CONST TCHAR* file,                                                              //┃
    CONST DWORD  line,                                                              //┃
    CONST TCHAR* func,                                                              //┃
    CONST TCHAR* expr,                                                              //┃
    CONST TCHAR* str,                                                               //┃
    ...                                                                             //┃
    )                                                                               //┃
{                                                                                   //┃
    va_list ap;                                                                     //┃
    TCHAR szStrBuff[MAX_PATH] = {0};                                                //┃
                                                                                    //┃
    va_start(ap, str);                                                              //┃
    _vsntprintf_s(szStrBuff, ASSERTX_STR_MAX_LEN, str, ap);                         //┃
    va_end(ap);                                                                     //┃
                                                                                    //┃
    // 检测断言是否已经初始化                                                       //┃
    if (!g_bAssertInit)                                                             //┃
    {                                                                               //┃
        MessageBox(NULL, _T("断言未初始化,将被忽略"), _T("断言"), MB_OK);           //┃
        g_bAssertInit = TRUE;                                                       //┃
        g_Command_Assert_Log = COMMAND_IGNORE_LOG;                                  //┃
    }                                                                               //┃
                                                                                    //┃
    if ((COMMAND_IGNORE_LOG == g_Command_Assert_Log) &&                             //┃
        (COMMAND_IGNORE_MSGBOX == g_Command_MessageBox))                            //┃
    {                                                                               //┃
        return;                                                                     //┃
    }                                                                               //┃
    TCHAR szTemp[ASSERTX_STR_MAX_LEN] = {0};                                        //┃
    UPDATA_LOCALTIME;                                                               //┃
                                                                                    //┃
    _stprintf_s(                                                                    //┃
        szTemp,                                                                     //┃
        ASSERTX_STR_MAX_LEN,                                                        //┃
        _T("文件: [%s]\n")                                                          //┃
        _T("行  : [%d]\n")                                                          //┃
        _T("函数: [%s]\n")                                                          //┃
        _T("原因: [%s]\n")                                                          //┃
        _T("异常: \n")                                                              //┃
        _T("━━━━━━━━━━━━━━━━━━━━━━\n")                        //┃
        _T("%s\n")                                                                  //┃
        _T("━━━━━━━━━━━━━━━━━━━━━━\n")                        //┃
        _T("时间: [%d年%d月%d日-星期%d[%d时%d分%d秒%d毫秒]\n"),                     //┃
        file, line, func, expr, szStrBuff,                                          //┃
        LOCALTIME_YEAR, LOCALTIME_MONTH, LOCALTIME_DAY, LOCALTIME_WEEK,             //┃
        LOCALTIME_HOUR, LOCALTIME_MINUTE, LOCALTIME_SECOND, LOCALTIME_MILLISECOND   //┃
        );                                                                          //┃
                                                                                    //┃
    LOG_(szTemp);                                                                   //┃
    MESSAGEBOX(eLevel, szTemp);                                                     //┃
    if (bThrow)                                                                     //┃
    {                                                                               //┃
        throw(expr);                                                                //┃
    }                                                                               //┃
}                                                                                   //┃
                                                                                    //┃
inline VOID ASSERTEXFORMATA(                                                        //┃
	CONST E_ASSERT_LEVEL eLevel,                                                    //┃
    CONST BOOL bThrow,                                                              //┃
    CONST ACHAR* file,                                                              //┃
    CONST DWORD  line,                                                              //┃
    CONST ACHAR* func,                                                              //┃
    CONST ACHAR* expr,                                                              //┃
    CONST ACHAR* str,                                                               //┃
    ...                                                                             //┃
    )                                                                               //┃
{                                                                                   //┃
    va_list ap;                                                                     //┃
    ACHAR szStrBuff[MAX_PATH] = {0};                                                //┃
                                                                                    //┃
    va_start(ap, str);                                                              //┃
    _vsnprintf_s(szStrBuff, ASSERTX_STR_MAX_LEN, str, ap);                          //┃
    va_end(ap);                                                                     //┃
                                                                                    //┃
    // 检测断言是否已经初始化                                                       //┃
    if (!g_bAssertInit)                                                             //┃
    {                                                                               //┃
        MessageBox(NULL, _T("断言未初始化,将被忽略"), _T("断言"), MB_OK);           //┃
        g_bAssertInit = TRUE;                                                       //┃
        g_Command_Assert_Log = COMMAND_IGNORE_LOG;                                  //┃
    }                                                                               //┃
                                                                                    //┃
    if ((COMMAND_IGNORE_LOG == g_Command_Assert_Log) &&                             //┃
        (COMMAND_IGNORE_MSGBOX == g_Command_MessageBox))                            //┃
    {                                                                               //┃
        return;                                                                     //┃
    }                                                                               //┃
    ACHAR szTempA[ASSERTX_STR_MAX_LEN] = {0};                                       //┃
#ifdef UNICODE                                                                      //┃
    TCHAR szTempT[ASSERTX_STR_MAX_LEN] = {0};                                       //┃
#endif                                                                              //┃
    UPDATA_LOCALTIME;                                                               //┃
                                                                                    //┃
    _snprintf_s(                                                                    //┃
        szTempA,                                                                    //┃
        sizeof(szTempA),                                                            //┃
        "文件: [%s]\n"                                                              //┃
        "行  : [%d]\n"                                                              //┃
        "函数: [%s]\n"                                                              //┃
        "原因: [%s]\n"                                                              //┃
        "异常: \n"                                                                  //┃
        "━━━━━━━━━━━━━━━━━━━━━━\n"                            //┃
        "%s\n"                                                                      //┃
        "━━━━━━━━━━━━━━━━━━━━━━\n"                            //┃
        "时间: [%d年%d月%d日-星期%d[%d时%d分%d秒%d毫秒]\n",                         //┃
        file, line, func, expr, szStrBuff,                                          //┃
        LOCALTIME_YEAR, LOCALTIME_MONTH, LOCALTIME_DAY, LOCALTIME_WEEK,             //┃
        LOCALTIME_HOUR, LOCALTIME_MINUTE, LOCALTIME_SECOND, LOCALTIME_MILLISECOND   //┃
        );                                                                          //┃
#ifdef UNICODE                                                                      //┃
    // 宽字符转换成多字符文件名字符串                                               //┃
    ACharToWChar(szTempA, szTempT, MAX_PATH);                                       //┃
    LOG_(szTempT);                                                                  //┃
    MESSAGEBOX(eLevel, szTempT);                                                    //┃
#else                                                                               //┃
    LOG_(szTempA);                                                                  //┃
    MESSAGEBOX(szTempA);                                                            //┃
#endif                                                                              //┃
    if (bThrow)                                                                     //┃
    {                                                                               //┃
        throw(expr);                                                                //┃
    }                                                                               //┃
}                                                                                   //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

#if defined(USE_MS_ASSERTX_FULL)
    #define Assert(level,expr)          ((VOID)((expr)  ? \
        0                                               :\
        (ASSERT_(level, TRUE, _T(__FILE__), __LINE__, _T(__FUNCTION__), _T(#expr), _T("")))))
    #define AssertEx(level,expr,str)    ((VOID)((expr)  ? \
        0                                               :\
        (ASSERT_(level, FALSE, _T(__FILE__), __LINE__, _T(__FUNCTION__), _T(#expr), str))))
    #define AssertA(level,expr)         ((VOID)((expr)  ? \
        0                                               :\
        (ASSERTA_(level, TRUE, __FILE__, __LINE__, __FUNCTION__, #expr), _T(""))))
    #define AssertExA(level,expr,str)   ((VOID)((expr)  ? \
        0                                               :\
        (ASSERTA_(level, FALSE, __FILE__, __LINE__, __FUNCTION__, #expr, str))))
    #define fAssert(level,expr,str,...) ((VOID)((expr)  ? \
        0                                               :\
        (ASSERTEXFORMATT(   \
		level,				\
        TRUE,               \
        _T(__FILE__),       \
        __LINE__,           \
        _T(__FUNCTION__),   \
        _T(#expr),          \
        str,                \
        __VA_ARGS__))))
    #define fAssertEx(level,expr,str,...) ((VOID)((expr)? \
        0                                               : \
        (ASSERTEXFORMATT(   \
		level,				\
		FALSE,              \
        _T(__FILE__),       \
        __LINE__,           \
        _T(__FUNCTION__),   \
        _T(#expr),          \
        str,                \
        __VA_ARGS__))))
    #define fAssertA(level,expr,str,...)((VOID)((expr)  ? \
        0                                               : \
        (ASSERTEXFORMATA(   \
		level,				\
		TRUE,               \
        __FILE__,           \
        __LINE__,           \
        __FUNCTION__,       \
        #expr,              \
        str,                \
        __VA_ARGS__))))
    #define fAssertExA(level,expr,str,...)((VOID)((expr)? \
        0                                               : \
        (ASSERTEXFORMATA(   \
		level,				\
		FALSE,              \
        __FILE__,           \
        __LINE__,           \
        __FUNCTION__,       \
        #expr,              \
        str,                \
        __VA_ARGS__))))
#elif defined(USE_MS_ASSERTX_IMPART_ONLY)
    #define Assert(level,expr,hwnd)
    #define AssertEx(level,expr,str)
    #define AssertA(level,expr,hwnd)
    #define AssertExA(level,expr,str)
#else
    #define fAssert(level,expr,hwnd)
    #define fAssertEx(level,expr,str)
    #define fAssertA(level,expr,hwnd)
    #define fAssertExA(level,expr,str)
#endif

//断言注释解释：
//#define 仅为了使代码高亮，并无其他任何含义

//1、错误调用:(不该被触发的断言,说明代码存在错误调用的情况.
//   请更正代码对此函数的调用!)
#define NO_PURPOSE_0001 AssertEx(p, "p,错误调用");

//2、错误传参:(对函数传入的参数做错误检查,触发了断言,
//   请检查函数调用处是否在不该调用的地方调用了该函数或参数有异常!)
#define NO_PURPOSE_0002 AssertEx(p, "p,错误传参");

//3、函数失败:(说明函数执行返回失败或错误,请检查调用函数的具体内容或调用是否正确!)
#define NO_PURPOSE_0003 AssertEx(Result == "错误", "pfun失败,函数失败");

//4、内存不足:(说明断言前做了一个申请内存操作,并返回失败的结果!)
#define NO_PURPOSE_0004 AssertEx(p, "p,内存不足");

//5、数据溢出:(说明访问越界,一般检测偏移是否超过最大限度或小于最小限度!
//   可能存在错误调用或逻辑错误!)
#define NO_PURPOSE_0005 AssertEx(p>&(p[max]), "p,数据溢出");

//6、代码异常:(说明此处在设计目的中并不希望看到,在代码实现中也要求避免,
//   但还是出现了!应该检查整体逻辑是否错误和代码编写疏忽!)
#define NO_PURPOSE_0006 AssertEx(AL_NORMAL, FALSE, "uNumber,代码异常");

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
