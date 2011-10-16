


#ifndef _MEM_INFO_CPP_
#define _MEM_INFO_CPP_

#include "../include/MemInfo.h"

namespace XM_Tools
{
	CMemInfo::CMemInfo(void)
	{
		QueryPerformanceFrequency(&m_FrequencyTime);
	}

	CMemInfo::~CMemInfo(void)
	{

	}

	//! 记录一次内存操作的信息
	void CMemInfo::LogMemOptInfo(bool IsAlloc, unsigned long MemSize, LARGE_INTEGER &OptCount)
	{
		InfoItr infoItr = m_InfoGroup.find(MemSize);
		if (m_InfoGroup.end() == infoItr)
		{
			m_InfoGroup.insert(std::make_pair(MemSize, tagOptInfo()));
			infoItr = m_InfoGroup.find(MemSize);
		}

		tagTimeInfo *pTimeInfo = NULL;
		if(IsAlloc)
		{
			++infoItr->second.CurrCount;
			if(infoItr->second.MaxUseCount < infoItr->second.CurrCount)
				infoItr->second.MaxUseCount = infoItr->second.CurrCount;
			pTimeInfo = &(infoItr->second.AllocTimeInfo);
		}
		else
		{
			--infoItr->second.CurrCount;
			pTimeInfo = &(infoItr->second.FreeTimeInfo);
		}

		++pTimeInfo->Degree;
		pTimeInfo->AllTime.QuadPart += OptCount.QuadPart;
		pTimeInfo->MaxTime.QuadPart = ((OptCount.QuadPart > pTimeInfo->MaxTime.QuadPart) ? OptCount.QuadPart : pTimeInfo->MaxTime.QuadPart);
	}

	//! 输出信息
	void CMemInfo::OutInfo(std::string &strOutInfo, bool Detailed)
	{
		for (InfoItr infoItr = m_InfoGroup.begin(); m_InfoGroup.end() != infoItr; ++infoItr)
		{
			if(Detailed || 1000 < infoItr->second.AllocTimeInfo.Degree)
			{
				char szInfo[256] = {0};
				sprintf(szInfo, "Size:%8ubyte, 当前:%6d块(max:%6d), 分配:%10u次, 耗时:%10.3fms(max%6.3f), 释放:%10u次, 耗时:%10.3fms(max%6.3f)\r\n",
					infoItr->first, infoItr->second.CurrCount, infoItr->second.MaxUseCount,
					infoItr->second.AllocTimeInfo.Degree,
					(double)infoItr->second.AllocTimeInfo.AllTime.QuadPart * 1000.0 / (double)m_FrequencyTime.QuadPart,
					(double)infoItr->second.AllocTimeInfo.MaxTime.QuadPart * 1000.0 / (double)m_FrequencyTime.QuadPart,
					infoItr->second.FreeTimeInfo.Degree,
					(double)infoItr->second.FreeTimeInfo.AllTime.QuadPart * 1000.0 / (double)m_FrequencyTime.QuadPart,
					(double)infoItr->second.FreeTimeInfo.MaxTime.QuadPart * 1000.0 / (double)m_FrequencyTime.QuadPart
					);
				strOutInfo += szInfo;
			}
		}
	}
};

#endif