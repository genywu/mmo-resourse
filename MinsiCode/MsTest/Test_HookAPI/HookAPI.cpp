#include "Precompiled.h"

HOOKAPI ha_MessageBoxA;
HOOKAPI ha_MessageBoxW;

INT WINAPI MyMessageBoxA(
    __in_opt HWND hWnd,
    __in_opt LPCSTR lpText,
    __in_opt LPCSTR lpCaption,
    __in UINT uType
    )
{
    INT(WINAPI*pRealMessageBoxA)(HWND,LPCSTR,LPCSTR,UINT);
    pRealMessageBoxA = (INT(WINAPI*)(HWND,LPCSTR,LPCSTR,UINT))ha_MessageBoxA.m_pOldProc;
    return pRealMessageBoxA(hWnd, "不允许调用MessageBox", "拦截", MB_OK);
}

INT WINAPI MyMessageBoxW(
     __in_opt HWND hWnd,
    __in_opt LPCWSTR lpText,
    __in_opt LPCWSTR lpCaption,
    __in UINT uType
    )
{
    INT(WINAPI*pRealMessageBoxW)(HWND,LPCWSTR,LPCWSTR,UINT);
    pRealMessageBoxW = (INT(WINAPI*)(HWND,LPCWSTR,LPCWSTR,UINT))ha_MessageBoxW.m_pOldProc;
    return pRealMessageBoxW(hWnd, _T("不允许调用MessageBox"), _T("拦截"), MB_OK);
}

INT __fastcall MainCall(INT,LPTCHAR*)
{
    _ENTER_FUN_C;

    MSLOG.SetSaveLog(FALSE);
    MSLOG.SetEnableLog(FALSE);

    HMODULE hModule = ::GetModuleHandle(NULL);

    ha_MessageBoxA.m_strFunc    = _T("MessageBoxA");
    ha_MessageBoxA.m_pNewProc   = (ULONG_PTR)MyMessageBoxA;
    CMsWinMain::HookAPIByName(hModule, _T("User32.dll"), &ha_MessageBoxA);

    ha_MessageBoxW.m_strFunc    = _T("MessageBoxW");
    ha_MessageBoxW.m_pNewProc   = (ULONG_PTR)MyMessageBoxW;
    CMsWinMain::HookAPIByName(hModule, _T("User32.dll"), &ha_MessageBoxW);




























    ::MessageBoxA(NULL, "测试对话框", "提示", MB_OK);
    ::MessageBoxW(NULL, _T("测试对话框"), _T("提示"), MB_OK);

    _tsystem(_T("pause"));
    return 0;
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}

















/*
(2)   在ActiveKey.h文件中加入HOOKAPI结构，此结构用来存储被挡截API函数名称、原API函数地址和替代函数地址。   
    
  　　　typedef   struct   tag_HOOKAPI     
  　　　{     
  　　　LPCSTR   szFunc;//被HOOK的API函数名称。   
  　　　PROC   pNewProc;//替代函数地址。   
  　　　PROC   pOldProc;//原API函数地址。   
  　　　}HOOKAPI,   *LPHOOKAPI;       
    
  　　(3)   打开ActiveKey.cpp文件，首先加入一个函数，用于定位输入库在输入数据段中的IAT地址。代码如下：   
    
  　　　extern   "C"   __declspec(dllexport)PIMAGE_IMPORT_DESCRIPTOR     
  　　　LocationIAT(HMODULE   hModule,   LPCSTR   szImportMod)     
  　　　//其中，hModule为进程模块句柄；szImportMod为输入库名称。   
  　　　{     
  　　　//检查是否为DOS程序，如是返回NULL，因DOS程序没有IAT。   
  　　　PIMAGE_DOS_HEADER   pDOSHeader   =   (PIMAGE_DOS_HEADER)   hModule;     
  　　　if(pDOSHeader->e_magic   !=   IMAGE_DOS_SIGNATURE)   return   NULL;     
  　　　　//检查是否为NT标志，否则返回NULL。   
  　　　　PIMAGE_NT_HEADERS   pNTHeader   =   (PIMAGE_NT_HEADERS)((DWORD)pDOSHeader+   (DWORD)(pDOSHeader->e_lfanew));     
  　　　　if(pNTHeader->Signature   !=   IMAGE_NT_SIGNATURE)   return   NULL;     
  　　　　//没有IAT表则返回NULL。   
  　　　　if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress   ==   0)   return   NULL;     
  　　　　//定位第一个IAT位置。     
  　　　　PIMAGE_IMPORT_DESCRIPTOR   pImportDesc   =   (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pDOSHeader   +   (DWORD)(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));     
  　　　　//根据输入库名称循环检查所有的IAT，如匹配则返回该IAT地址，否则检测下一个IAT。   
  　　　　while   (pImportDesc->Name)     
  　　　　{     
  　　　　　//获取该IAT描述的输入库名称。   
  　　　PSTR   szCurrMod   =   (PSTR)((DWORD)pDOSHeader   +   (DWORD)(pImportDesc->Name));     
  　　　if   (stricmp(szCurrMod,   szImportMod)   ==   0)   break;     
  　　　pImportDesc++;     
  　　　　}     
  　　　　if(pImportDesc->Name   ==   NULL)   return   NULL;     
  　　　return   pImportDesc;     
  　　　}     
    
  　　再加入一个函数，用来定位被挡截API函数的IAT项并修改其内容为替代函数地址。代码如下：   
    
  　　　extern   "C"   __declspec(dllexport)     
  　　　HookAPIByName(   HMODULE   hModule,   LPCSTR   szImportMod,   LPHOOKAPI   pHookApi)     
  　　　//其中，hModule为进程模块句柄；szImportMod为输入库名称；pHookAPI为HOOKAPI结构指针。   
  　　　{     
  　　　　//定位szImportMod输入库在输入数据段中的IAT地址。   
  　　　　PIMAGE_IMPORT_DESCRIPTOR   pImportDesc   =   LocationIAT(hModule,   szImportMod);     
  　　if   (pImportDesc   ==   NULL)   return   FALSE;     
  　　　　//第一个Thunk地址。   
  　　　　PIMAGE_THUNK_DATA   pOrigThunk   =   (PIMAGE_THUNK_DATA)((DWORD)hModule   +   (DWORD)(pImportDesc->OriginalFirstThunk));     
  　　   //第一个IAT项的Thunk地址。   
  　　　　PIMAGE_THUNK_DATA   pRealThunk   =   (PIMAGE_THUNK_DATA)((DWORD)hModule   +   (DWORD)(pImportDesc->FirstThunk));     
  　　　　//循环查找被截API函数的IAT项，并使用替代函数地址修改其值。   
  　　　while(pOrigThunk->u1.Function)     
  {     
  　//检测此Thunk是否为IAT项。   
  if((pOrigThunk->u1.Ordinal   &   IMAGE_ORDINAL_FLAG)   !=   IMAGE_ORDINAL_FLAG)     
  {   
  　   //获取此IAT项所描述的函数名称。   
  　PIMAGE_IMPORT_BY_NAME   pByName   =(PIMAGE_IMPORT_BY_NAME)((DWORD)hModule+(DWORD)(pOrigThunk->u1.AddressOfData));     
  　if(pByName->Name[0]   ==   '\0')   return   FALSE;     
  　　//检测是否为挡截函数。   
  if(strcmpi(pHookApi->szFunc,   (char*)pByName->Name)   ==   0)     
  　   {     
  　　　　　　　MEMORY_BASIC_INFORMATION   mbi_thunk;   
  　　　　　　　//查询修改页的信息。   
  　　　　　　　VirtualQuery(pRealThunk,   &mbi_thunk,   sizeof(MEMORY_BASIC_INFORMATION));     
  //改变修改页保护属性为PAGE_READWRITE。   
  　　　　　　　VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize,   PAGE_READWRITE,   &mbi_thunk.Protect);     
  //保存原来的API函数地址。   
  　　　   　　if(pHookApi->pOldProc   ==   NULL)     
  pHookApi->pOldProc   =   (PROC)pRealThunk->u1.Function;     
  　   //修改API函数IAT项内容为替代函数地址。   
  pRealThunk->u1.Function   =   (PDWORD)pHookApi->pNewProc;     
  //恢复修改页保护属性。   
  DWORD   dwOldProtect;     
  　　　　　　　VirtualProtect(mbi_thunk.BaseAddress,   mbi_thunk.RegionSize,   mbi_thunk.Protect,   &dwOldProtect);     
  　　　　　   }     
  }     
  　   pOrigThunk++;     
  　   pRealThunk++;     
  }     
  　　SetLastError(ERROR_SUCCESS);   //设置错误为ERROR_SUCCESS，表示成功。   
  　　return   TRUE;     
  　　　}       
    
  　　(4)   定义替代函数，此实例中只给MessageBoxA和recv两个API进行挡截。代码如下：   
    
  　　　static   int   WINAPI   MessageBoxA1   (HWND   hWnd   ,   LPCTSTR   lpText,   LPCTSTR   lpCaption,   UINT   uType)   
  　　　{   
  HANDLE   hFile;   
    
  if((hFile   =CreateFile("c:\\MessageBoxA1.log",   GENERIC_WRITE,   0,   NULL,   OPEN_ALWAYS,   FILE_ATTRIBUTE_NORMAL,   NULL))   <0)   
  {   
  WriteLog("open   file   %s   failed",   fname);   
  return   -1;   
  }   
  _llseek((HFILE)hFile,   0,   SEEK_END);   
  char   temp[2048];   
  wsprintf(temp,   "\r\n(LPCTSTR   lpText%s,   LPCTSTR   lpCaption%s)   ",     lpText,lpCaption);   
  DWORD   dw;   
  WriteFile(hFile,   temp,   strlen(temp),   &dw,   NULL);   
    
  _lclose((HFILE)hFile);   
  　　　　//过滤掉原MessageBoxA的正文和标题内容，只显示如下内容。   
  return   MessageBox(hWnd,   "Hook   API   OK!",   "Hook   API",   uType);     
  　　　}     
  　　　static   int   WINAPI   recv1(SOCKET   s,   char   FAR   *buf,   int   len,   int   flags   )   
  　　　{   
  　　　HANDLE   hFile;   
    
  if((hFile   =CreateFile("c:\\recv1.log",   GENERIC_WRITE,   0,   NULL,   OPEN_ALWAYS,   FILE_ATTRIBUTE_NORMAL,   NULL))   <0)   
  {   
  WriteLog("open   file   %s   failed",   fname);   
  return   -1;   
  }   
  _llseek((HFILE)hFile,   0,   SEEK_END);   
  char   temp[2048];   
  wsprintf(temp,   "\r\n(len=%d)   ",     len);   
  DWORD   dw;   
  WriteFile(hFile,   temp,   strlen(temp),   &dw,   NULL);   
    
    
  for(int   i   =0;   i<len;   i++)   
  {   
    
          wsprintf(temp,   "(%02x)",   buf[i]&0x00FF);   
  WriteFile(hFile,   temp,   strlen(temp),   &dw,   NULL);   
  }   
  _lclose((HFILE)hFile);   
    
    
    
    
  　　　return   recv(s,buf,len,flags);   
  　　　}       
    
  (5)   在KeyboardProc函数中加入激活挡截API代码，在if(   wParam   ==   0X79   )语句中后面加入如下else   if语句：   
    
  　　　......   
  　　　//当激活F11键时，启动挡截API函数功能。   
  　　　else   if(   wParam   ==   0x7A   )   
  　　　{     
  　　　　HOOKAPI   api[2];   
  api[0].szFunc   ="MessageBoxA";//设置被挡截函数的名称。   
  api[0].pNewProc   =   (PROC)MessageBoxA1;//设置替代函数的地址。   
  api[1].szFunc   ="recv";//设置被挡截函数的名称。   
  api[1].pNewProc   =   (PROC)recv1;   //设置替代函数的地址。   
  //设置挡截User32.dll库中的MessageBoxA函数。   
  HookAPIByName(GetModuleHandle(NULL),"User32.dll",&api[0]);   
  //设置挡截Wsock32.dll库中的recv函数。   
  HookAPIByName(GetModuleHandle(NULL),"Ws2_32.dll",&api[1]);   
  　　　}   
  　　　......       
  我在recv1()里写了个截获数据并保存到c:\recv1.log的代码，但运行后没有截获到目标程序的数据。   
  而同样的代码却在MessageBoxA1()中运行正常，我发现目标程序被钩住后，一调用MessageBoxA就被截获，并纪录在c:\recv1.log   
    
  请大虾指点，为何截获不到recv()？   

*/