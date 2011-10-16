#pragma once
#define   MAX_PROCESSOR   32
typedef LONG NTSTATUS;
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,             // obsolete...delete
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS;

typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnERPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;

}THREADINFO, *PTHREADINFO;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;

typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;

	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];

	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	ULONG dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;

	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];

} PROCESSINFO, *PPROCESSINFO;

typedef NTSTATUS (_stdcall *pnNtQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS,OUT PVOID,IN ULONG,OUT PULONG);
extern pnNtQuerySystemInformation      NtQuerySystemInformation;
//NEW
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER DpcTime;          // DEVL only
	LARGE_INTEGER InterruptTime;    // DEVL only
	ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)



class CHardWareInfo
{
private:
	CHardWareInfo(void);
	~CHardWareInfo(void);
public:
	void GetMemUsage(tagHardwareInfo& tgInfo);
	void GetCpuUsage(tagHardwareInfo& tgInfo);
	void GetHardwareInfo(tagHardwareInfo& tgInfo);
	void ClearInfoList()					{m_listInfo.clear();}
	static CHardWareInfo& GetInstance()		{return cInstance;}
	BOOL	AddToByteArray(DBWriteSet& dbWriteSet);
	void	AddOneInfo(DBWriteSet& dbWriteSet);
	int		GetProcUsage(int id);//获取指定进程的CPU使用率
	typedef list<tagHardwareInfo>::iterator INFOITER;
private:
		
	list<tagHardwareInfo>m_listInfo;
	int m_nProcessorNum;
private:
	static CHardWareInfo	cInstance;
};
