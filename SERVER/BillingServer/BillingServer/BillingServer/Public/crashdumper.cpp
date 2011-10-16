
//**************************************************************************************
//	日期:	14:11:2003   
//	创建:	tiamo	
//	描述:	异常崩溃记录.from under the hood
//							Improved Error Reporting with DBGHELP 5.1 APIs 
//**************************************************************************************

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <Lmcons.h>
#include <dbghelp.h>
//#include "..\nets\netclient\MyNetClient.h"
#ifdef _CLIENT
#include "crashdumper.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern long g_lVersion;


//using namespace std;

#pragma comment(lib, "dbghelp.lib")

#define gDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))

namespace _Sword3
{
	namespace
	{
		// 输出文件句柄
		HANDLE hFile;
		// 当前进程
		HANDLE hProcess;
	}

	// 定义在 DIA SDK 的CVCONST.H里面
	enum BasicType
	{
		btNoType = 0,
		btVoidType = 1,
		btChar = 2,
		btWChar = 3,
		btInt = 6,
		btUInt = 7,
		btFloat = 8,
		btBCD = 9,
		btBool = 10,
		btLong = 13,
		btULong = 14,
		btCurrency = 25,
		btData = 26,
		btVariant = 27,
		btComplex = 28,
		btBit = 29,
		btBSTR = 30,
		btHresult = 31,
	};

	// c++异常代码
	#define EXCEPTION_MSVCPP 0xe06d7363

	void WriteLog(HANDLE hFile,LPCTSTR szFormat,...)
	{
		if(!szFormat || hFile == INVALID_HANDLE_VALUE)
			return;

		// 数据缓存
		static TCHAR szBuffer[8192];

		va_list list;
		va_start(list,szFormat); 
		int nCount = _vsntprintf(szBuffer,sizeof(szBuffer) - sizeof(TCHAR),szFormat,list);
		va_end(list);

		szBuffer[8191] = 0;

		DWORD dwDummy;
		if(nCount > 0)
			WriteFile(hFile,szBuffer,nCount * sizeof(TCHAR),&dwDummy,NULL);
		else if(nCount == -1)
			WriteFile(hFile,szBuffer,sizeof(szBuffer) - sizeof(TCHAR),&dwDummy,NULL);

	}

	// 获取异常信息
	LPCTSTR GetExceptionMsg(DWORD dwCode)
	{
		LPCTSTR pszRet = TEXT("Unknown");
		switch(dwCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			pszRet = TEXT("The thread attempted to read from or write to a virtual address for which it does not have the appropriate access.");
			break;

		case EXCEPTION_BREAKPOINT:
			pszRet = TEXT("A breakpoint was encountered.");
			break;

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			pszRet = TEXT("The thread attempted to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte boundaries, and so on.");
			break;

		case EXCEPTION_SINGLE_STEP:
			pszRet = TEXT("A trace trap or other single-instruction mechanism signaled that one instruction has been executed.");
			break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			pszRet = TEXT("The thread attempted to divide a floating-point value by a floating-point divisor of zero.");
			break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			pszRet = TEXT("The thread attempted to access an array element that is out of bounds, and the underlying hardware supports bounds checking.");
			break;

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			pszRet = TEXT("One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.");
			break;

		case EXCEPTION_FLT_INEXACT_RESULT:
			pszRet = TEXT("The result of a floating-point operation cannot be represented exactly as a decimal fraction.");
			break;

		case EXCEPTION_FLT_INVALID_OPERATION:
			pszRet = TEXT("This exception represents any floating-point exception not included in this list.");
			break;

		case EXCEPTION_FLT_OVERFLOW:
			pszRet = TEXT("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.");
			break;

		case EXCEPTION_FLT_STACK_CHECK:
			pszRet = TEXT("The stack overflowed or underflowed as the result of a floating-point operation.");
			break;

		case EXCEPTION_FLT_UNDERFLOW:
			pszRet = TEXT("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.");
			break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			pszRet = TEXT("The thread attempted to divide an integer value by an integer divisor of zero.");
			break;

		case EXCEPTION_INT_OVERFLOW:
			pszRet = TEXT("The result of an integer operation caused a carry out of the most significant bit of the result.");
			break;

		case EXCEPTION_PRIV_INSTRUCTION:
			pszRet = TEXT("The thread attempted to execute an instruction whose operation is not allowed in the current machine mode.");
			break;

		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			pszRet = TEXT("The thread attempted to continue execution after a noncontinuable exception occurred.");
			break;

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			pszRet = TEXT("The thread tried to execute an invalid instruction.");
			break;

		case EXCEPTION_IN_PAGE_ERROR:
			pszRet = TEXT("The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.");
			break;

		case EXCEPTION_INVALID_DISPOSITION:
			pszRet = TEXT("An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.");
			break;

		case EXCEPTION_MSVCPP:
			pszRet = TEXT("A c++ exception.");
			break;
		}
		return pszRet;
	}

	// dump 栈内容
	void DumpStackInfo(LPDWORD pStack)
	{
		WriteLog(hFile,TEXT("Stack content:"));
		__try
		{
			// 获取栈顶指针
			LPDWORD pStackTop;
			__asm
			{
				mov eax,fs:[4]
				mov pStackTop,eax
			}

			// 计数
			//int nCount = 0;
			LPDWORD pStackStart = pStack;
			while(pStack + 1 < pStackTop)
			{
				// 行号
				WriteLog(hFile,TEXT("\r\n0x%08x"),pStackStart);

				// 剩余数量
				DWORD count = PtrToUlong(pStackTop - pStackStart);
				if(count > 4)
				{
					count = 4;
				}

				DWORD i;
				for( i =0 ; i < count ; ++i)
				{
					WriteLog(hFile,TEXT("\t0x%08x"),*pStack);
					pStack ++;
				}
				for(; i < 4; ++i)
				{
					WriteLog(hFile,TEXT("\t          "));
				}

				WriteLog(hFile,TEXT("\t|"));
				LPBYTE pText = reinterpret_cast<LPBYTE>(pStackStart);
				for(i = 0; i < count * 4; i ++)
				{
					BYTE c = *pText++;
					if(c < 0x20 || c > 0x7e)
						c = '.';
					WriteLog(hFile,TEXT("%c"),c);
				}

				for( ;i < 16; ++ i)
				{
					WriteLog(hFile,TEXT(" "));
				}

				pStackStart += 4;
				pStack = pStackStart;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			WriteLog(hFile,TEXT("Meet an exception when read stack.\r\n"));
		}
	}

	// dump系统信息
	void DumpSystemInfo()
	{
		WriteLog(hFile,TEXT("\r\n\r\n\r\nSystem Information : \r\n"));
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		// cpu数目
		WriteLog(hFile,TEXT("\tProcessor Numbers : %d\r\n"),sysInfo.dwNumberOfProcessors);

		// cpu类型
		WriteLog(hFile,TEXT("\tProcessor type :"));
		switch(sysInfo.wProcessorArchitecture)
		{
		case PROCESSOR_ARCHITECTURE_INTEL:
			switch(sysInfo.wProcessorLevel)
			{
			case 3:
			case 4:
				if((sysInfo.wProcessorRevision & 0xff00) == 0xff00)
					WriteLog(hFile,TEXT("80%d86-%x"),sysInfo.wProcessorLevel,sysInfo.wProcessorRevision);
				else
					WriteLog(hFile,TEXT("80%d86-%c-%d"),sysInfo.wProcessorLevel,((sysInfo.wProcessorRevision & 0xff00) >> 8) + 'A',
									sysInfo.wProcessorRevision & 0xff);
				break;
			default:
				WriteLog(hFile,TEXT("80%d86 ,Model %d, Stepping %d"),sysInfo.wProcessorLevel,
							(sysInfo.wProcessorRevision & 0xff00) >>8,sysInfo.wProcessorRevision & 0xff);;
				break;
			}
			break;
		case PROCESSOR_ARCHITECTURE_MIPS:
			switch(sysInfo.wProcessorLevel)
			{
			case 4:
				WriteLog(hFile,TEXT("MIPS R4000 %2d"),sysInfo.wProcessorRevision & 0xff);	
				break;
			}
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA:
			WriteLog(hFile,TEXT("Alpha %d Model A+%d,Pass %d"),sysInfo.wProcessorLevel,(sysInfo.wProcessorRevision & 0xff00)>>8,
						sysInfo.wProcessorRevision & 0xff);	
			break;
		case PROCESSOR_ARCHITECTURE_PPC:
			switch(sysInfo.wProcessorLevel)
			{
			case 1:
				WriteLog(hFile,TEXT("PPC 601"));
				break;
			case 3:
				WriteLog(hFile,TEXT("PPC 603"));
				break;
			case 4:
				WriteLog(hFile,TEXT("PPC 604"));
				break;
			case 6:
				WriteLog(hFile,TEXT("PPC 603+"));
				break;
			case 9:
				WriteLog(hFile,TEXT("PPC 604+"));
				break;
			case 20:
				WriteLog(hFile,TEXT("PPC 620"));
				break;
			}
			WriteLog(hFile,TEXT(" %d.%d"),(sysInfo.wProcessorRevision & 0xff00)>>8,sysInfo.wProcessorRevision & 0xff);
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			WriteLog(hFile,TEXT("IA64"));
			break;
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
			WriteLog(hFile,TEXT("IA64 on WIN64"));
			break;
		case PROCESSOR_ARCHITECTURE_AMD64:
			WriteLog(hFile,TEXT("AMD64"));
			break;

		case PROCESSOR_ARCHITECTURE_UNKNOWN:
		default:
			WriteLog(hFile,TEXT(" Unknown"));
			break;
		}
		// 内存状态
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);
		WriteLog(hFile,TEXT("\r\n\r\n\tTotal physical memory : %dMB\r\n"),ms.dwTotalPhys / 1048576);
		WriteLog(hFile,TEXT("\tTotal physical memory in use %d%%100\r\n"),ms.dwMemoryLoad );
		WriteLog(hFile,TEXT("\tAvailable physical memory : %dMB\r\n"),ms.dwAvailPhys / 1048576);
		WriteLog(hFile,TEXT("\tTotalPageFile : %dMB\r\n"),ms.dwTotalPageFile  / 1048576);
		WriteLog(hFile,TEXT("\tAvailable PageFile : %dMB\r\n"),ms.dwAvailPageFile  / 1048576);
		WriteLog(hFile,TEXT("\tTotal Virtual memory: %dMB\r\n"),ms.dwTotalVirtual  / 1048576);
		WriteLog(hFile,TEXT("\tAvailable Virtual memory: %dMB\r\n"),ms.dwAvailVirtual   / 1048576);

#ifdef _CLIENT
		render::Interface *pInterface = render::Interface::GetInstance();
		WriteLog(hFile, "\r\n\tDisplayCard: %s\r\n", pInterface->GetInfo()->AdapterIdentifier.Description);
		WriteLog(hFile, "\tDriver: %s\r\n",pInterface->GetInfo()->AdapterIdentifier.Driver);
		WriteLog(hFile, "\tVersion: %d %d %d %d\r\n",
			HIWORD(pInterface->GetInfo()->AdapterIdentifier.DriverVersion.HighPart),
			LOWORD(pInterface->GetInfo()->AdapterIdentifier.DriverVersion.HighPart),
			HIWORD(pInterface->GetInfo()->AdapterIdentifier.DriverVersion.LowPart),
			LOWORD(pInterface->GetInfo()->AdapterIdentifier.DriverVersion.LowPart));
#endif

		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;

		// 能不能使用ex函数
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)))
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (!GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				return ;
		}

		static TCHAR szName[UNLEN + 1];
		DWORD dwLen = UNLEN + 1;
		GetUserName(szName,&dwLen);

		WriteLog(hFile,TEXT("\r\n\tLogin user name : %s\r\n"),szName);
		WriteLog(hFile,TEXT("\r\n\tOperation System :"));
		switch(osvi.dwPlatformId)
		{
			// nt 系列
		case VER_PLATFORM_WIN32_NT:
			{
				if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
					WriteLog(hFile,TEXT("Microsoft Windows Server 2003 family "));

				if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
					WriteLog(hFile,TEXT("Microsoft Windows XP "));

				if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
					WriteLog(hFile,TEXT ("Microsoft Windows 2000 "));

				if(osvi.dwMajorVersion <= 4)
					WriteLog(hFile,TEXT("Microsoft Windows NT "));

				// Windows NT 4.0 SP6 and later.
				if(bOsVersionInfoEx )
				{
					// workstation type
					if(osvi.wProductType == VER_NT_WORKSTATION)
					{
						if(osvi.dwMajorVersion == 4)
							WriteLog(hFile,TEXT ( "Workstation 4.0 " ));
						else if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
							WriteLog(hFile,TEXT ( "Home Edition " ));
						else
							WriteLog(hFile,TEXT ( "Professional " ));
					}
					// server type.
					else if(osvi.wProductType == VER_NT_SERVER)
					{
						if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
						{
							if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
								WriteLog(hFile,TEXT ( "Datacenter Edition " ));
							else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								WriteLog(hFile,TEXT ( "Enterprise Edition " ));
							else if(osvi.wSuiteMask == VER_SUITE_BLADE)
								WriteLog(hFile,TEXT ( "Web Edition " ));
							else
								WriteLog(hFile,TEXT ( "Standard Edition " ));
						}
						else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
						{
							if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
								WriteLog(hFile,TEXT ( "Datacenter Server " ));
							else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								WriteLog(hFile,TEXT ( "Advanced Server " ));
							else
								WriteLog(hFile,TEXT ( "Server " ));
						}
						else  // Windows NT 4.0 
						{
							if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
								WriteLog(hFile,TEXT ("Server 4.0, Enterprise Edition " ));
							else
								WriteLog(hFile,TEXT ( "Server 4.0 " ));
						}
					}
				}
				else  // Windows NT 4.0 SP5 and earlier
				{
					HKEY hKey;
					char szProductType[80];
					DWORD dwBufLen=80;
					LONG lRet;

					lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
												0,KEY_QUERY_VALUE,&hKey );
					if(lRet != ERROR_SUCCESS)
						return ;

					lRet = RegQueryValueEx(hKey,TEXT("ProductType"),NULL,NULL,(LPBYTE)szProductType,&dwBufLen);
					if((lRet != ERROR_SUCCESS) || (dwBufLen > 80))
						return ;

					RegCloseKey(hKey);

					if(lstrcmpi(TEXT("WINNT"), szProductType) == 0)
						WriteLog(hFile,TEXT( "Workstation " ));
					if(lstrcmpi(TEXT("LANMANNT"), szProductType) == 0)
						WriteLog(hFile,TEXT( "Server " ));
					if(lstrcmpi(TEXT("SERVERNT"), szProductType) == 0)
						WriteLog(hFile,TEXT( "Advanced Server " ));

					WriteLog(hFile,TEXT( "%d.%d "), osvi.dwMajorVersion, osvi.dwMinorVersion);
				}

				// service pack 和 build number.

				if(osvi.dwMajorVersion == 4 && lstrcmpi(osvi.szCSDVersion,TEXT("Service Pack 6")) == 0)
				{
					HKEY hKey;
					LONG lRet;

					//  SP6 SP6a.
					lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
										TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
										0,KEY_QUERY_VALUE,&hKey);

					if(lRet == ERROR_SUCCESS)
						WriteLog(hFile,TEXT( "Service Pack 6a (Build %d)"), osvi.dwBuildNumber & 0xFFFF );
					else // Windows NT 4.0 prior to SP6a
					{
						WriteLog(hFile,TEXT( "%s (Build %d)"),osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
					}

					RegCloseKey(hKey);
				}
				else // Windows NT 3.51 and earlier or Windows 2000 and later
				{
					WriteLog(hFile,TEXT( "%s (Build %d)"),osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
				}
				break;

				// 9x系列
			case VER_PLATFORM_WIN32_WINDOWS:

				if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				{
					WriteLog(hFile,TEXT ("Microsoft Windows 95 "));
					if(osvi.szCSDVersion[1] == TEXT('C') || osvi.szCSDVersion[1] ==TEXT('B'))
						WriteLog(hFile,TEXT("OSR2 " ));
				} 

				if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				{
					WriteLog(hFile,TEXT ("Microsoft Windows 98 "));
					if(osvi.szCSDVersion[1] == TEXT('A'))
						WriteLog(hFile,TEXT("SE " ));
				} 

				if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				{
					WriteLog(hFile,TEXT ("Microsoft Windows Millennium Edition"));
				} 
				break;

			case VER_PLATFORM_WIN32s:
				WriteLog(hFile,TEXT("Microsoft Win32s"));
				break;
			}
		}

		WriteLog(hFile,TEXT("\r\n\r\n\r\n"));
;	}

	// 单个模块
	BOOL DumpModuleInfo(HMODULE hModule,int nCount)
	{
		static TCHAR szName[2048];
		if(GetModuleFileName(hModule,szName,gDIMOF(szName) - 1))
		{
			PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
			if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
				return FALSE;

			PIMAGE_NT_HEADERS pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PBYTE>(hModule)
							+ pDosHeader->e_lfanew);

			// PE文件
			if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
				return FALSE;

			WriteLog(hFile,TEXT("%d Module information:\r\n"),nCount);
			WriteLog(hFile,TEXT("\tModule : %s\r\n"),szName);
			WriteLog(hFile,TEXT("\tBase : 0x%08x\r\n"),pNTHeader->OptionalHeader.ImageBase);
			WriteLog(hFile,TEXT("\tSize : 0x%08x\r\n\r\n"),pNTHeader->OptionalHeader.SizeOfImage);

			return TRUE;
		}
		return FALSE;
	}

	// 模块列表
	void DumpModulesInfoList()
	{
		WriteLog(hFile,TEXT("Modules List: \r\n"));
		int nCount = 1;

		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		DWORD dwPageSize = sysInfo.dwPageSize;
		DWORD dwPages = 4 * (1073741824L / dwPageSize);
		DWORD i = 0;
		MEMORY_BASIC_INFORMATION mbi;
		LPVOID pLastBase = 0;
		while(i < dwPages)
		{
			if(VirtualQuery(ULongToPtr(i * dwPageSize),&mbi,sizeof(mbi)))
			{
				if(mbi.RegionSize >0)
				{
					i += (DWORD)(mbi.RegionSize / dwPageSize);
					if(mbi.State == MEM_COMMIT && mbi.AllocationBase > pLastBase)
					{
						pLastBase = mbi.AllocationBase;
						if(DumpModuleInfo((HMODULE)(mbi.AllocationBase),nCount))
						{
							nCount ++;
						}
					}
				}
				else
					i += 65536 / dwPageSize;
			}
			else
				i += 65536 / dwPageSize;
		}
	}

	// 格式化基本数据
	LPSTR FormatOutputValue(LPSTR pszCurrBuffer,BasicType basicType,DWORD64 length,PVOID pAddress )
	{
		if(length == 1)
			pszCurrBuffer += sprintf(pszCurrBuffer," = 0x%02x",*(PBYTE)pAddress);
		else if(length == 2)
			pszCurrBuffer += sprintf(pszCurrBuffer," = 0x%04x", *(PWORD)pAddress);
		else if(length == 4)
		{
			if(basicType == btFloat)
			{
				pszCurrBuffer += sprintf(pszCurrBuffer," = %f", *(PFLOAT)pAddress);
			}
			else if(basicType == btChar)
			{
				if(!IsBadStringPtr( *(PSTR*)pAddress, 32))
				{
					pszCurrBuffer += sprintf(pszCurrBuffer," = \"%.31s\"",*(PDWORD)pAddress);
				}
				else
					pszCurrBuffer += sprintf( pszCurrBuffer," = 0x%08x",*(PDWORD)pAddress);
			}
			else
				pszCurrBuffer += sprintf(pszCurrBuffer," = 0x%08x", *(PDWORD)pAddress);
		}
		else if(length == 8)
		{
			if(basicType == btFloat )
			{
				pszCurrBuffer += sprintf(pszCurrBuffer," = %lf",*(double *)pAddress);
			}
			else
				pszCurrBuffer += sprintf(pszCurrBuffer, " = 0x%016I64x",*(DWORD64*)pAddress);
		}

		return pszCurrBuffer;
	}

	// 获取基本类型
	BasicType GetBasicType(DWORD typeIndex,DWORD64 modBase)
	{
		BasicType basicType;
		if(SymGetTypeInfo(hProcess,modBase,typeIndex,TI_GET_BASETYPE,&basicType))
		{
			return basicType;
		}

		// 子类的id
		DWORD typeId;
		if(SymGetTypeInfo(hProcess,modBase,typeIndex,TI_GET_TYPEID,&typeId))
		{
			if(SymGetTypeInfo(hProcess,modBase,typeId,TI_GET_BASETYPE,&basicType))
			{
				return basicType;
			}
		}

		return btNoType;
	}

	// dump类型index
	LPSTR DumpTypeIndex(LPSTR pszCurrBuffer,DWORD64 modBase,DWORD dwTypeIndex,
								unsigned nestingLevel,DWORD_PTR offset,BOOL& bHandled)
	{
		bHandled = FALSE;

		// 获取名字
		WCHAR * pwszTypeName;
		if(SymGetTypeInfo(hProcess,modBase,dwTypeIndex,TI_GET_SYMNAME,&pwszTypeName))
		{
			pszCurrBuffer += sprintf(pszCurrBuffer," %ls",pwszTypeName);
			LocalFree(pwszTypeName);
		}

		// 类型有多少个子类型
		DWORD dwChildrenCount = 0;
		SymGetTypeInfo(hProcess,modBase,dwTypeIndex,TI_GET_CHILDRENCOUNT,&dwChildrenCount );

		// 没有
		if(!dwChildrenCount)
			return pszCurrBuffer;

		// XXX一直没有想到一个好用的安全的方式全部的dump出所以的变量
		// 所以这里简单的返回吧
		bHandled = TRUE;
		return pszCurrBuffer;

		// Prepare to get an array of "TypeIds", representing each of the children.
		// SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
		// TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
		struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
		{
			ULONG   MoreChildIds[1024];
			FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
		}children;

		children.Count = dwChildrenCount;
		children.Start= 0;

		// 获取每个类型的TypeId
		if(!SymGetTypeInfo(hProcess,modBase,dwTypeIndex,TI_FINDCHILDREN,&children))
		{
			return pszCurrBuffer;
		}

		pszCurrBuffer += sprintf(pszCurrBuffer,"\r\n");

		// 遍历每个子类型
		for(DWORD i = 0; i < dwChildrenCount; i++)
		{
			// 缩进
			for(unsigned j = 0; j <= nestingLevel + 1; j++)
				pszCurrBuffer += sprintf(pszCurrBuffer,"\t");

			// 递归
			BOOL bHandled2;
			pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,modBase,children.ChildId[i],nestingLevel+1,offset,bHandled2);

			// 如果不是用户定义的
			if(!bHandled2)
			{
				// 偏移量
				DWORD dwMemberOffset;
				SymGetTypeInfo(hProcess,modBase,children.ChildId[i],TI_GET_OFFSET,&dwMemberOffset);

				// TypeId
				DWORD typeId;
				SymGetTypeInfo(hProcess,modBase,children.ChildId[i],TI_GET_TYPEID,&typeId);

				// 大小
				ULONG64 length;
				SymGetTypeInfo(hProcess,modBase,typeId,TI_GET_LENGTH,&length);

				// 地址
				DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

				// 基本类型
				BasicType basicType = GetBasicType(children.ChildId[i], modBase);

				// 格式化
				pszCurrBuffer = FormatOutputValue(pszCurrBuffer,basicType,length,ULongToPtr(dwFinalOffset));

				pszCurrBuffer += sprintf(pszCurrBuffer,"\r\n");
			}
		}

		bHandled = TRUE;

		return pszCurrBuffer;
	}

	// 格式化
	BOOL FormatSymbolValue(PSYMBOL_INFO pSym,STACKFRAME * sf,LPTSTR pszBuffer,size_t cbBuffer)
	{
		LPTSTR pszCurrBuffer = pszBuffer;
		LPTSTR pEnd = pszBuffer + cbBuffer / sizeof(TCHAR);

		int count = 0;
		// 局部变量还是参数
		if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER)
		{
			count = _sntprintf(pszCurrBuffer,cbBuffer,TEXT("Parameter "));
			if(count < 0)
				return FALSE;
			else
				pszCurrBuffer += count;
		}
		else if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
		{
			count = _sntprintf(pszCurrBuffer,cbBuffer,TEXT("Local "));
			if(count < 0)
				return FALSE;
			else
				pszCurrBuffer += count;
		}

		// 如果不是函数
		if(pSym->Tag == 5)   // 5 == SymTagFunction 
			return FALSE;

		// 变量名字
		count = _sntprintf(pszCurrBuffer,(pEnd - pszCurrBuffer) * sizeof(TCHAR),"\'%s\'",pSym->Name);
		if(count < 0)
			return FALSE;
		else
			pszCurrBuffer += count;

		// variable 的指针
		DWORD_PTR pVariable = 0;

		if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE)
		{
			// 指针变量
			// if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
			{                               //  This may change!!!
				pVariable = sf->AddrFrame.Offset;
				pVariable += (DWORD_PTR)pSym->Address;
			}
			// else
			//  return false;
		}
		else if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER)
		{
			// 放入到寄存器里面的变量不获取
			return FALSE;
		}
		else
		{
			// 全局变量
			pVariable = (DWORD_PTR)pSym->Address;
		}

		// 是否是用户定义的变量
		BOOL bHandled = FALSE;
		pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,pSym->ModBase,pSym->TypeIndex,0,pVariable,bHandled);

		if(!bHandled)
		{
			// 是一个基本类型
			BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );

			pszCurrBuffer = FormatOutputValue(pszCurrBuffer,basicType,pSym->Size,(PVOID)pVariable ); 
		}

		return TRUE;
	}

	// 枚举函数
	BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO pSymInfo,ULONG SymbolSize,PVOID UserContext)
	{
		static TCHAR szBuffer[8192];

		__try
		{
			if(FormatSymbolValue(pSymInfo,(STACKFRAME*)UserContext,szBuffer,sizeof(szBuffer)))  
				WriteLog(hFile,TEXT("\t%s\r\n"),szBuffer);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			WriteLog(hFile,TEXT("punting on symbol %s\r\n"),pSymInfo->Name);
		}

		return TRUE;
	}

	// 显示调用栈
	void DumpCallStack(PCONTEXT pContext,BOOL bWriteVariables )  // true if local/params should be output
	{
		WriteLog(hFile,TEXT("\r\n\r\nCall stack:\r\n") );

		DWORD dwMachineType = 0;

		STACKFRAME sf;
		memset(&sf,0,sizeof(sf));

		// 初始化stackframe结构
		sf.AddrPC.Offset = pContext->Eip;
		sf.AddrPC.Mode = AddrModeFlat;
		sf.AddrStack.Offset = pContext->Esp;
		sf.AddrStack.Mode = AddrModeFlat;
		sf.AddrFrame.Offset = pContext->Ebp;
		sf.AddrFrame.Mode = AddrModeFlat;

		dwMachineType = IMAGE_FILE_MACHINE_I386;

		int i = 0;
		for(;;)
		{
			// 获取下一个栈帧
			if(!StackWalk(dwMachineType,hProcess,GetCurrentThread(),&sf,pContext,0,SymFunctionTableAccess,SymGetModuleBase,0))
			{
				break;
			}

			// 检查帧的正确性
			if(0 == sf.AddrFrame.Offset)
			{
				break;
			}

			i ++;

			// 正在调用的函数名字
			BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 1024];
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
			pSymbol->SizeOfStruct = sizeof(symbolBuffer);
			pSymbol->MaxNameLen = 1024;

			// 偏移量
			DWORD64 symDisplacement = 0;

			// 获取符号
			if(SymFromAddr(hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol))
			{
				if(bWriteVariables)
					WriteLog(hFile,TEXT("%4d Function : %hs\r\n\t\t\t\t\t\t\t(+0x%I64x)##[Address : 0x%08x]##[Frame : 0x%08x]"),i,
								pSymbol->Name,symDisplacement,sf.AddrPC.Offset,sf.AddrFrame.Offset);
				else
					WriteLog(hFile,TEXT("%4d Function : %hs"),i,pSymbol->Name);
			}
			// 没有找到符号,显示逻辑地址
			else
			{
				TCHAR szModule[MAX_PATH] = TEXT("");
				TCHAR section[IMAGE_SIZEOF_SHORT_NAME + 1] = TEXT("");
				DWORD offset = 0;

				MEMORY_BASIC_INFORMATION mbi;

				do
				{
					if(!VirtualQuery(ULongToPtr(sf.AddrPC.Offset),&mbi,sizeof(mbi)))
						break;

					HMODULE hMod = (HMODULE)mbi.AllocationBase;

					if(!GetModuleFileName(hMod,szModule,static_cast<DWORD>(_tcslen(szModule))))
						break;

					// dos文件头
					PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

					// nt文件头
					PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);

					// section头
					PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);

					// rva地址
					DWORD rva = (DWORD)sf.AddrPC.Offset - PtrToUlong(hMod);

					// 遍历section table 查找rva地址
					for(WORD i = 0;i < pNtHdr->FileHeader.NumberOfSections;i++, pSection++ )
					{
						DWORD sectionStart = pSection->VirtualAddress;
						DWORD sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

						// rva 落在了section里面
						if((rva >= sectionStart) && (rva <= sectionEnd))
						{
							// 计算偏移
							_tcsncpy(section,reinterpret_cast<LPCTSTR>(pSection->Name),IMAGE_SIZEOF_SHORT_NAME);
							section[IMAGE_SIZEOF_SHORT_NAME] = 0;
							offset = rva - sectionStart;
							break;
						}
					}
				}while(0);

				if(bWriteVariables)
					WriteLog(hFile,TEXT("%4d Function : [Module :%s] ## [Section : %8s offset 0: x%08x] \r\n\t\t\t\t\t\t\t"
									"[Address : 0x%08x]##[Frame : 0x%08x]"),i,section,offset,szModule,sf.AddrPC.Offset,
									sf.AddrFrame.Offset);
				else
					WriteLog(hFile,TEXT("%4d Function : [Module :%s] ## [Section : %8s offset 0: x%08x]"),i,szModule,
									section,offset);
			}

			if(bWriteVariables)
			{
				// 获取源代码的行
				IMAGEHLP_LINE lineInfo = {sizeof(IMAGEHLP_LINE)};
				DWORD dwLineDisplacement;
				if(SymGetLineFromAddr(hProcess,sf.AddrPC.Offset,&dwLineDisplacement,&lineInfo))
				{
					WriteLog(hFile,TEXT("[Source File : %s]"),lineInfo.FileName); 
					WriteLog(hFile,TEXT("[Source Line : %u]"),lineInfo.LineNumber); 
				}
			}
			WriteLog(hFile,TEXT("\r\n"));

			// dump 变量信息
			if ( bWriteVariables )
			{
				// 使用 SymSetContext获取变量信息
				IMAGEHLP_STACK_FRAME imagehlpStackFrame;
				imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
				SymSetContext(hProcess,&imagehlpStackFrame, 0 );

				// 枚举局部变量
				SymEnumSymbols(hProcess,0,0,EnumerateSymbolsCallback,&sf);

				WriteLog(hFile,TEXT("\r\n"));
			}
		}
	}

	// 异常过滤
	int CrashFilter(LPEXCEPTION_POINTERS lpEP,DWORD dwExceptionCode,char* dumpfilename)
	{
		// 防止递归调用
		static BOOL bFirstTime = FALSE;
		if(bFirstTime)
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
		bFirstTime = FALSE;

		// 进程句柄
		hProcess = GetCurrentProcess();

		// 打开log文件
		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		char log_filename[128];
		sprintf(log_filename, "dump[%02d-%02d(%02d.%02d)].log", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute);
		if(dumpfilename)
			strcpy(dumpfilename,log_filename);

		hFile = CreateFile(TEXT(log_filename),GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return EXCEPTION_CONTINUE_SEARCH;

#ifdef _CLIENT
		WriteLog(hFile, "A exception has been occured. [ver:%d]\r\n\r\n\r\n", g_lVersion);
#else
		WriteLog(hFile, "A exception has been occured.\r\n\r\n\r\n");
#endif

		// 写入系统时间
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		WriteLog(hFile,TEXT("Basic Information:\r\n"));
		WriteLog(hFile,TEXT("\tTime : %02d:%02d:%04d - %02d:%02d:%02d:%03d.\r\n"),sysTime.wDay,sysTime.wMonth,sysTime.wYear,
							sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);

#ifdef _CLIENT
		//记录发生错误的场景信息
		if(GetGame() && GetGame()->GetRegion() && GetGame()->GetMainPlayer())
			WriteLog(hFile,TEXT("\t场景 :%s 位置 X:%f,Y:%f\r\n"),GetGame()->GetRegion()->GetName(),GetGame()->GetMainPlayer()->GetPosX(),GetGame()->GetMainPlayer()->GetPosY());
#endif

#ifdef _CLIENT
		//记录发生错误的玩家的帐号
		if(GetGame()->GetCDKEY())
			WriteLog(hFile,TEXT("\t出错CDKEY :%s \r\n"),GetGame()->GetCDKEY());
#endif

		// 异常记录
		PEXCEPTION_RECORD pRecord = lpEP->ExceptionRecord;
		// 异常发生的时候的执行环境
		PCONTEXT pContext = lpEP->ContextRecord;

		// dump异常情况,显示发生异常的模块名字,ip地址,异常代码
		static TCHAR szCrashModuleFileName[2048] = TEXT("Unknown");
		MEMORY_BASIC_INFORMATION mbi;
		if(VirtualQuery(ULongToPtr(pContext->Eip),&mbi,sizeof(mbi)) && 
				GetModuleFileName((HMODULE)mbi.AllocationBase,szCrashModuleFileName,gDIMOF(szCrashModuleFileName) - 1))
		{
		}

		WriteLog(hFile,TEXT("\tModule : \"%s\"\r\n"),szCrashModuleFileName);
		WriteLog(hFile,TEXT("\tAddress : %04x:%08x\r\n"),pContext->SegCs,pContext->Eip);
		WriteLog(hFile,TEXT("\tException code : 0x%08x\r\n"),dwExceptionCode);
		WriteLog(hFile,TEXT("\tException description : %s\r\n\r\n"),GetExceptionMsg(dwExceptionCode));

		// dump 出 cpu的各个寄存器
		WriteLog(hFile,TEXT("CPU register information :\r\n"));
		WriteLog(hFile,TEXT("\t\tEAX : 0x%08x\tEBX : 0x%08x\tECX : 0x%08x\tEDX : 0x%08x\r\n"),
							pContext->Eax,pContext->Ebx,pContext->Ecx,pContext->Edx);
		WriteLog(hFile,TEXT("\t\tESI : 0x%08x\tEDI : 0x%08x\tEBP : 0x%08x\tESP : 0x%08x\r\n"),
							pContext->Esi,pContext->Edi,pContext->Ebp,pContext->Esp);
		WriteLog(hFile,TEXT("\t\tEIP : 0x%08x\tCS  : 0x%08x\tDS  : 0x%08x\tEFlags:0x%08x\r\n"),
							pContext->Eip,pContext->SegCs,pContext->SegDs,pContext->EFlags);
		WriteLog(hFile,TEXT("\t\tFS  : 0x%08x\tGS  : 0x%08x\tES  : 0x%08x\tSS  : 0x%08x\r\n\r\n"),
							pContext->SegFs,pContext->SegGs,pContext->SegEs,pContext->SegSs);

		// dump cs:eip 位置的内存内容
		WriteLog(hFile,TEXT("\tCode at the address cs:eip is :"));
		PBYTE pCode = (PBYTE)ULongToPtr(pContext->Eip);
		int i;
		for(i = 0 ; i < 10; i ++)
		{
			__try
			{
				WriteLog(hFile,TEXT(" %02x"),pCode[i]);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				WriteLog(hFile,TEXT(" ??"));
			}
		}
		WriteLog(hFile,TEXT("\r\n\r\n\r\n"));

		// dump 出 栈的内容
		DumpStackInfo((PDWORD)ULongToPtr(pContext->Esp));

		// dump出系统名称,cpu信息,内存信息
		DumpSystemInfo();

		// dump当前加载的模块信息
//		DumpModulesInfoList();

		SymSetOptions(SYMOPT_DEFERRED_LOADS);
		// Initialize DbgHelp
		if (!SymInitialize(GetCurrentProcess(),0,TRUE))
			return EXCEPTION_CONTINUE_SEARCH;

		CONTEXT trashableContext = *pContext;

		DumpCallStack(&trashableContext,FALSE);

		WriteLog(hFile,TEXT("\r\n\r\n\r\n"));
		WriteLog(hFile,TEXT("Local variables and parameters\r\n"));

		trashableContext = *pContext;
		DumpCallStack(&trashableContext,TRUE);

		WriteLog(hFile,TEXT("\r\n\r\n\r\n"));
		WriteLog(hFile,TEXT("Global variables\r\n"));

//		SymEnumSymbols(GetCurrentProcess(),(DWORD64)GetModuleHandle(szCrashModuleFileName),0,EnumerateSymbolsCallback,0);

		SymCleanup(GetCurrentProcess());

		WriteLog(hFile,TEXT("\r\n"));

		WriteLog(hFile,TEXT("\r\n\r\n#############Crash log end#############"));

		CloseHandle(hFile);

		if(dwExceptionCode == EXCEPTION_MSVCPP)
			return EXCEPTION_CONTINUE_SEARCH;
		else
			return EXCEPTION_EXECUTE_HANDLER;
	}
}