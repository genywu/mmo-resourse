#include "stdafx.h"
#include "pool.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
ObjectPool::ObjectPool(void)
{

}

ObjectPool::~ObjectPool(void)
{ 
	Release(); 
}



bool		ObjectPool::SetPolicy(long size, long objCount, long blockCount)
{
	if (size <= 0 || objCount <= 0 || blockCount <= 0)
	{
		return false;
	}
	map<long, Blocks*>::iterator ite = m_SizeTable.find(size);

	if (ite != m_SizeTable.end())
	{
		return false;
	}

	//else
	Blocks* pBlocks = NULL;
	pBlocks = new Blocks(size, objCount, blockCount);
	if (NULL == pBlocks)
	{
		return NULL;
	}
	m_SizeTable[size] = pBlocks;
	return true;
}



void*		ObjectPool::AllotObj(long size)
{
	map<long, Blocks*>::iterator ite = m_SizeTable.find(size);

	if (ite != m_SizeTable.end())
	{
		return (*ite).second->alloc_obj();
	}

	//else
	Blocks* pBlocks = NULL;
	pBlocks = new Blocks(size);
	if (NULL == pBlocks)
	{
		return NULL;
	}
	m_SizeTable[size] = pBlocks;
	return pBlocks->alloc_obj();

};



bool		ObjectPool::FreeObj(void* p, long size)
{
	if(0 == size)
	{
		for (map<long, Blocks*>::iterator ite = m_SizeTable.begin(); ite != m_SizeTable.end(); ++ite)
		{
			if ((*ite).second->free_obj(p))
			{
				return true;
			}
		}
	}
	else
	{
		map<long, Blocks*>::iterator ite = m_SizeTable.find(size);
		if (ite != m_SizeTable.end())
		{
			return (*ite).second->free_obj(p);
		}
	}

	return false;
}



void		ObjectPool::Release(void)
{
	for (map<long, Blocks*>::iterator ite = m_SizeTable.begin(); ite != m_SizeTable.end(); ++ite)
	{
		if (!(*ite).second->free_buf())
		{
#ifdef NEW_DEBUG
			//这里如果抛出异常，说明有内存泄露
			throw;
#endif
		}
	}
}




long		ObjectPool::GetBufSize(void)
{
	long re = 0;
	for (map<long, Blocks*>::iterator ite = m_SizeTable.begin(); ite != m_SizeTable.end(); ++ite)
	{
		re += ite->second->get_buf_size();
	}
	return re;
}




long		ObjectPool::GetUesedSize(void)
{
	long re = 0;
	for (map<long, Blocks*>::iterator ite = m_SizeTable.begin(); ite != m_SizeTable.end(); ++ite)
	{
		re += ite->second->get_uesed_size();
	}
	return re;
}




long		ObjectPool::GetFreeSize(void)
{
	long re = 0;
	for (map<long, Blocks*>::iterator ite = m_SizeTable.begin(); ite != m_SizeTable.end(); ++ite)
	{
		re += ite->second->get_free_size();
	}
	return re;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ObjectPool::Blocks::Blocks(long objSize, long blockObjNum, long g_blockCount)
:m_objSize(objSize),m_num(blockObjNum)
{
	for (long i = 0; i < g_blockCount; ++i)
	{
		long bufSize = m_objSize * m_num;
		char* p = (char*)malloc(bufSize);
		if (NULL == p)
		{
			return;
		}
		for(long j = 0; j < m_num; ++j)
		{
			m_unUseList.push_back(p + m_objSize * j);
		}
		m_bufs.push_back(p);
	}
}



ObjectPool::Blocks::~Blocks(void)
{
	vector<char*>::iterator ite = m_bufs.begin();
	for ( ; ite != m_bufs.end(); ++ite)
	{
		free(*ite);
	}

	m_bufs.clear();
}




void*	ObjectPool::Blocks::alloc_obj(void)
{
	if (0 == m_unUseList.size())
	{
		//扩展buf
		long bufSize = m_objSize * m_num;
		char* p = (char*)malloc(bufSize);
		if (NULL == p)
		{
			return NULL;
		}
		for(long j = 0; j < m_num; ++j)
		{
			m_unUseList.push_back(p + m_objSize * j);
		}
		m_bufs.push_back(p);
	}
	void* p = *(m_unUseList.begin());
	m_unUseList.pop_front();
	m_useSet.insert(p);
	return p;
}




bool	ObjectPool::Blocks::free_obj(void *p)
{
	set<void*>::iterator ite = m_useSet.find(p);
	if (ite != m_useSet.end())
	{
		m_useSet.erase(ite);
		m_unUseList.push_front(p);
		return true;
	}
	return false;
}




bool	ObjectPool::Blocks::free_buf(void)
{
	if (0 == (long)m_useSet.size())
	{
		vector<char*>::iterator ite = m_bufs.begin();
		for ( ; ite != m_bufs.end(); ++ite)
		{
			free(*ite);
		}
		m_bufs.clear();
		m_unUseList.clear();
		return true;
	}
	return false;
}

long	ObjectPool::Blocks::get_buf_size(void)
{
	return (long)m_bufs.size() * m_objSize * m_num;
}

long	ObjectPool::Blocks::get_uesed_size(void)
{
	return (long)m_useSet.size() * m_objSize;
}

long	ObjectPool::Blocks::get_free_size(void)
{
	return (long)m_unUseList.size() * m_objSize;
}