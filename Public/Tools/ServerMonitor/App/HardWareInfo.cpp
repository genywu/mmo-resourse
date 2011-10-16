#include "StdAfx.h"
#include "HardWareInfo.h"

CHardWareInfo CHardWareInfo::cInstance;

pnNtQuerySystemInformation      NtQuerySystemInformation    =0;
LARGE_INTEGER       PreviousCPUIdleTime[MAX_PROCESSOR] = {0 ,0};
LARGE_INTEGER       PreviousCPUTotalTime[MAX_PROCESSOR] = {0 ,0};
extern pnNtQuerySystemInformation      NtQuerySystemInformation;
CHardWareInfo::CHardWareInfo(void)
{
}

CHardWareInfo::~CHardWareInfo(void)
{
}

void CHardWareInfo::GetMemUsage(tagHardwareInfo& tgInfo)
{

	MEMORYSTATUS memory;

	GlobalMemoryStatus(&memory);
	tgInfo.stMS=memory;
	return;
}

void CHardWareInfo::GetCpuUsage(tagHardwareInfo& tgInfo)
{
	SYSTEM_INFO sy;
	::GetSystemInfo(&sy);
	m_nProcessorNum = sy.dwNumberOfProcessors;
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ProcessorInfo[MAX_PROCESSOR];
	LARGE_INTEGER                            CPUIdleTime[MAX_PROCESSOR];
	LARGE_INTEGER                            CPUTotalTime[MAX_PROCESSOR];

	LARGE_INTEGER                            SumIdleTime   = { 0 ,0 };
	LARGE_INTEGER                            SumTotalTime  = { 0, 0 };
	LARGE_INTEGER                            SumKernelTime = { 0, 0 };

	NTSTATUS Status;

	(FARPROC &)NtQuerySystemInformation    =GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),"NtQuerySystemInformation");
	Status = NtQuerySystemInformation(
		SystemProcessorPerformanceInformation,
		ProcessorInfo,
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * MAX_PROCESSOR,
		NULL
		);

	if (!NT_SUCCESS(Status))
	{
		return;
	}

	LARGE_INTEGER DeltaCPUIdleTime;
	LARGE_INTEGER DeltaCPUTotalTime;

	for (int ListIndex = 0; ListIndex < m_nProcessorNum; ListIndex++)
	{
		CPUIdleTime[ListIndex].QuadPart  = ProcessorInfo[ListIndex].IdleTime.QuadPart;
		CPUTotalTime[ListIndex].QuadPart = ProcessorInfo[ListIndex].KernelTime.QuadPart +
			ProcessorInfo[ListIndex].UserTime.QuadPart;// +
		//ProcessorInfo[ListIndex].IdleTime.QuadPart;

		DeltaCPUIdleTime.QuadPart        = CPUIdleTime[ListIndex].QuadPart -
			PreviousCPUIdleTime[ListIndex].QuadPart;
		DeltaCPUTotalTime.QuadPart       = CPUTotalTime[ListIndex].QuadPart -
			PreviousCPUTotalTime[ListIndex].QuadPart;

		SumIdleTime.QuadPart            += DeltaCPUIdleTime.QuadPart;
		SumTotalTime.QuadPart           += DeltaCPUTotalTime.QuadPart;

		PreviousCPUTotalTime[ListIndex].QuadPart = CPUTotalTime[ListIndex].QuadPart;
		PreviousCPUIdleTime[ListIndex].QuadPart  = CPUIdleTime[ListIndex].QuadPart;

		unsigned char Usages = DeltaCPUTotalTime.QuadPart ?
			(BYTE) (100 - ((DeltaCPUIdleTime.QuadPart * 100) / DeltaCPUTotalTime.QuadPart))
			: 0;
		tgInfo.cSubUsage[ListIndex] = Usages;

	}

	tgInfo.cUsage = SumTotalTime.QuadPart ?
		(BYTE) (100 - ((SumIdleTime.QuadPart * 100) / SumTotalTime.QuadPart))
		: 0;
}

void CHardWareInfo::GetHardwareInfo(tagHardwareInfo& tgInfo)
{
	tgInfo.dwTime = timeGetTime();
	//GetMemUsage(tgInfo);
	//GetCpuUsage(tgInfo);
	char szfile[256] = {0};
	char szfilelog[BASE_TEXT_SIZE]="log\\";
	strcat(szfilelog,GetSM()->GetMemUsageFileName());
	//打开文件
	fstream file/*(szfilelog,ios_base::app)*/;
	//计数清空文件

	long lsize = GetSM()->GetSccPrama()->dwSaveHardWareInfoSize;
	if (lsize>0)
	{
		static long i=0;
		i++;
		//清空文件
		if (i>lsize)
		{
			i=0;
			file.open(szfilelog,ios_base::trunc|ios_base::out);
		}
		else
		{
			file.open(szfilelog,ios_base::app);
		}
	}
	if (!file.is_open())
	{
		return;
	}

	//产生时间
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	char strTime[BASE_TEXT_SIZE*2]={0};
	_snprintf(strTime,MAX_PATH,"[%02d-%02d %02d:%02d:%02d]\n",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
	//硬件信息
	file<<strTime<<"=========="<<endl;
	file<<"AvailPageFile:"<<tgInfo.stMS.dwAvailPageFile<<endl
		<<"AvailPhys:"<<tgInfo.stMS.dwAvailPhys<<endl
		<<"AvailVirtual:"<<tgInfo.stMS.dwAvailVirtual<<endl
		<<"MemoryLoad:"<<tgInfo.stMS.dwMemoryLoad<<endl<<endl;

	file<<"CPU Usage："<<(DWORD)tgInfo.cUsage<<endl;
	char szTemp[256] = {0};
	for (int i=0;i < m_nProcessorNum;i++)
	{
		sprintf_s(szTemp,"CPU[%d] ：%d\n",i,(DWORD)tgInfo.cSubUsage[i]);
		file<<szTemp;
	}
	file<<endl;
	file.close();
}

BOOL CHardWareInfo::AddToByteArray(DBWriteSet& dbWriteSet)
{
	dbWriteSet.AddToByteArray((long)m_nProcessorNum);
	dbWriteSet.AddToByteArray((long)GetInstance().m_listInfo.size());
	INFOITER it=m_listInfo.begin();
	for (;it!=m_listInfo.end();it++)
	{
		dbWriteSet.AddToByteArray( &(*it),sizeof(tagHardwareInfo));
	}
	return TRUE;
}
//接配置间隔发送一条硬件信息
void CHardWareInfo::AddOneInfo(DBWriteSet& dbWriteSet)
{
	tagHardwareInfo	tgInfo;
	tgInfo.dwTime = timeGetTime();
	GetMemUsage(tgInfo);
	GetCpuUsage(tgInfo);
	GetHardwareInfo(tgInfo);
	dbWriteSet.AddToByteArray( &tgInfo,sizeof(tagHardwareInfo));
}

int CHardWareInfo::GetProcUsage(int id)
{
	int cpuusage=-1;
	PVOID pProcInfo = NULL;
	DWORD dwInfoSize = 0x20000;
	PPROCESSINFO pProcessInfo;
	DWORD dwWorkingSet;
	long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD );


	static __int64 LastTotalProcessCPUUsage = 0;
	static __int64 LastCurrentProcessCPUUsage = 0;

	int CurrentDelta;
	int TotalDelta;

	__int64 TotalProcessCPUUsage = 0;
	__int64 CurrentProcessCPUUsage = 0;

	/////////////////////////////////

	pProcInfo = (PVOID)(new byte[dwInfoSize]);
	

	NtQuerySystemInformation = (long(__stdcall*)(DWORD,PVOID,DWORD,DWORD))
		GetProcAddress( GetModuleHandle( "ntdll.dll" ),"NtQuerySystemInformation" );

	NtQuerySystemInformation(SystemProcessInformation,pProcInfo,dwInfoSize,0);

	pProcessInfo = (PPROCESSINFO)pProcInfo;

	bool bfind=false;
	do
	{
		TotalProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart + (__int64)pProcessInfo->UserTime.QuadPart;

		if(pProcessInfo->dwProcessID == id)
		{
			bfind=true;
			dwWorkingSet = pProcessInfo->dwWorkingSet; 
			CurrentProcessCPUUsage += (__int64)pProcessInfo->KernelTime.QuadPart + (__int64)pProcessInfo->UserTime.QuadPart;
		}

		/////////
		if(pProcessInfo->dwOffset == 0)
		{
			break;
		}

		pProcessInfo = (PPROCESSINFO)((byte*)pProcessInfo + pProcessInfo->dwOffset);
	}
	while(true);
	if (!bfind)
	{
		SAFE_DELETE_ARRAY(pProcessInfo);
		return -1;
	}
	TotalDelta = TotalProcessCPUUsage - LastTotalProcessCPUUsage;
	CurrentDelta = CurrentProcessCPUUsage - LastCurrentProcessCPUUsage;

	if(TotalDelta != 0)
		cpuusage = 100 * CurrentDelta / TotalDelta;


	LastTotalProcessCPUUsage = TotalProcessCPUUsage;
	LastCurrentProcessCPUUsage = CurrentProcessCPUUsage;

	SAFE_DELETE_ARRAY(pProcessInfo);
	return cpuusage;

}
