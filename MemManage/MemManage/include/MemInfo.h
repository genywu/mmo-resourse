





/*
本文件定义了内存管理的信息记录

*/

#ifndef _MEM_INFO_H_
#define _MEM_INFO_H_

#include "mmdef.h"

namespace XM_Tools
{

	//!
	class CMemInfo
	{
	public:
		CMemInfo(void);
		~CMemInfo(void);

	public:
		//!				得到当前市时钟周期
		static void		GetTimeFrequencyCount(LARGE_INTEGER *pOutCount){QueryPerformanceCounter(pOutCount);}

		//!				记录一次内存操作的信息
		void			LogMemOptInfo(bool IsAlloc, unsigned long MemSize, LARGE_INTEGER &OptCount);
		//!				输出信息
		void			OutInfo(std::string &strOutInfo, bool Detailed);

	private:

		//!		时间信息
		struct tagTimeInfo 
		{
			//! 次数
			unsigned long	Degree;
			//! 总耗时
			LARGE_INTEGER	AllTime;
			//! 最大耗时
			LARGE_INTEGER	MaxTime;

			tagTimeInfo(void){memset(this, 0, sizeof(tagTimeInfo));}
		};

		//!		操作信息
		struct	tagOptInfo 
		{
			//! 当前数量
			long			CurrCount;
			//! 最大使用数量
			long			MaxUseCount;
			//! 分配时间信息
			tagTimeInfo		AllocTimeInfo;
			//! 释放时间信息
			tagTimeInfo		FreeTimeInfo;

			tagOptInfo(void):CurrCount(0),MaxUseCount(0){}
		};


		typedef stdext::hash_map<unsigned long, tagOptInfo>			InfoGroup;
		typedef InfoGroup::iterator							InfoItr;


	private:
		//!					
		InfoGroup			m_InfoGroup;
		//!					始终频率
		LARGE_INTEGER		m_FrequencyTime;
	};
};

#endif