


#pragma once


/** 数据缓存模板
*
*		本模板只适用于数据操作较频繁的情况，若数据的添加时间间隔
*	远大于设置的最大缓存时间，则会出现过期数据不能得到即时处理的
*	问题，需要手动调用CheckCache接口
*
*	注意：TData应保证“=”操作符的正确性
*/

template<typename TData>
class DataCache
{
public:
	//! MaxCacheNum=最大缓存数量 MaxCacheSec=最大缓存时间
	DataCache(DWORD MaxCacheNum, DWORD MaxCacheSec);
	~DataCache(VOID);

public:
	//! 用于保存和返回的数据结构
	struct tagDataBlock 
	{
		TData *pData;
		DWORD DataNum;
	};

	//! 添加一条数据到缓存
	//! 若返回值不为空，则说明缓存已满或已超时
	//! 返回的只读数据使用完毕后，应通过Release方法提交回本类处理，否则将造成泄露
	const tagDataBlock* AddData(TData &Data);

	//! 取出已满或者已超时缓存的数据
	//! 返回的只读数据使用完毕后，应通过Release方法提交回本类处理，否则将造成泄露
	const tagDataBlock* CheckCache(VOID);

	//! 提前取出缓存的数据，不考虑任何条件
	//! 返回的只读数据使用完毕后，应通过Release方法提交回本类处理，否则将造成泄露
	const tagDataBlock* GetCache(VOID);

	//! 提交数据操作完成接口
	VOID Release(const tagDataBlock *pDataBlock);
	VOID Release(const TData *pData);

private:
	//! 最大缓存数量
	const DWORD m_MaxCacheNum;
	//! 最大缓存时间
	const DWORD m_MaxCacheSec;

	//! 当前块的创建时间
	DWORD m_CurrBlokCreateTime;
	//! 被外部操作的数据块列表
	list<tagDataBlock> m_OptDataCache;
	//! 当前数据
	tagDataBlock m_CurrData;
};










///////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename TData>
DataCache<TData>::DataCache(DWORD MaxCacheNum, DWORD MaxCacheSec)
:m_MaxCacheNum(MaxCacheNum), m_MaxCacheSec(MaxCacheSec), m_CurrBlokCreateTime(0)
{
}

template<typename TData>
DataCache<TData>::~DataCache(VOID)
{
	//! 若外部操作还没有结束，等待30秒
	if(0 != m_OptDataCache.size())
		Sleep(1000 * 30);

	//! 若外部操作还没有结束，不释放相应内存避免程序崩溃
	if(0 == m_OptDataCache.size())
	{
		list<tagDataBlock>::iterator iteData = m_OptDataCache.begin();
		for (; m_OptDataCache.end() != iteData; ++iteData)
		{
			M_FREE(iteData->pData, m_MaxCacheNum * sizeof(TData));
		}
		m_OptDataCache.clear();
	}
	else
	{
		LogTools::OutErrLog("~DataCache()错误，外部操作没有及时归还使用的数据，造成泄露!!!");
	}
	
	//! 释放掉外部没有用到的内存
	M_FREE(m_CurrData.pData, m_MaxCacheNum * sizeof(TData));
	m_CurrData.DataNum = 0;
	m_CurrBlokCreateTime = 0;
}

template<typename TData>
typename const DataCache<TData>::tagDataBlock* DataCache<TData>::AddData(TData &Data)
{
	const tagDataBlock *pRe = CheckCache();

	m_CurrData.pData[m_CurrData.DataNum] = Data;
	++m_CurrData.DataNum;

	if(1 == m_CurrData.DataNum)
	{
		m_CurrBlokCreateTime = (DWORD)time(NULL);
	}

	return pRe;
}

template<typename TData>
typename const DataCache<TData>::tagDataBlock* DataCache<TData>::CheckCache(VOID)
{
	if(m_MaxCacheSec <= time(NULL) - m_CurrBlokCreateTime
		|| m_MaxCacheNum <= m_CurrData.DataNum
		|| NULL == m_CurrData.pData)
	{
		return GetCache();
	}

	return NULL;
}

template<typename TData>
typename const DataCache<TData>::tagDataBlock* DataCache<TData>::GetCache(VOID)
{
	tagDataBlock *pRe = NULL;
	if(NULL != m_CurrData.pData)
	{
		m_OptDataCache.push_back(m_CurrData);
		pRe = &(*(--m_OptDataCache.end()));
	}

	m_CurrData.DataNum = 0;
	m_CurrBlokCreateTime = 0;
	m_CurrData.pData = (TData*)M_ALLOC(sizeof(TData) * m_MaxCacheNum);

	return pRe;
}

template<typename TData>
VOID DataCache<TData>::Release(const tagDataBlock *pDataBlock)
{
	list<tagDataBlock>::iterator iteData = m_OptDataCache.begin();
	for (; m_OptDataCache.end() != iteData; ++iteData)
	{
		if(&(*iteData) == pDataBlock)
		{
			M_FREE(iteData->pData, m_MaxCacheNum * sizeof(TData));
			m_OptDataCache.erase(iteData);
			return;
		}
	}
}

template<typename TData>
VOID DataCache<TData>::Release(const TData *pData)
{
	list<tagDataBlock>::iterator iteData = m_OptDataCache.begin();
	for (; m_OptDataCache.end() != iteData; ++iteData)
	{
		if(iteData->pData == pData)
		{
			M_FREE(pDataBlock->pData, m_MaxCacheNum * sizeof(TData));
			m_OptDataCache.erase(iteData);
			return;
		}
	}
}

