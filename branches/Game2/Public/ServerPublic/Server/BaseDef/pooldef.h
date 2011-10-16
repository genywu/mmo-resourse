/*
*	file:		pooldef.h
*	Brief:		内存池相关的一些定义
*	detail:		
*				本文件还实现了一个基本的Win32锁类型LockOfWin32，可以直接放入MultiMemPool模板。
*				如需其他实现，请按照LockOfWin32编写				
*	Authtor:	张科智
*	Datae:		2007
*/

#pragma warning(disable:4311;disable:4291)

#pragma once

//! 测试用宏
#define MEM_POOL_DEBUG
//! 记录日志宏
#define MEM_POOL_LOG

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MEM_POOL_LOG

	#define FILE_LINE_TEMP2(x, y)	"所在文件["x"]，所在行数["#y"]"
	#define FILE_LINE_TEMP(x, y)	FILE_LINE_TEMP2(x, y)
	#define _FILE_AND_LINE_			FILE_LINE_TEMP(__FILE__, __LINE__)

		//! 用于日志记录的一个宏（将传入的类名与代码所在的文件名、行号连接成一个字符串）
	#define LINK_OBJ_INFO(ClassName) (#ClassName ## _FILE_AND_LINE_ )


#else

	#define _FILE_AND_LINE_			NULL

		//! 用于日志记录的一个宏（将传入的类名与代码所在的文件名、行号连接成一个字符串）
	#define LINK_OBJ_INFO(ClassName) NULL

#endif


//! 
inline void MyAssert(bool bExpression)	
{								
#ifdef MEM_POOL_DEBUG		
	if(!bExpression) throw;			
#endif						
}

//! 填充内存的值
#ifdef MEM_POOL_DEBUG
#define FILL_VALUE (0xcd)
#else
#define FILL_VALUE (0)
#endif

//! 分配内存时填充分配的内存
#define FILL_MEM


//! 定义最大的小内存块尺寸，超过这个尺寸的内存申请直接调用系统API分配
//! 默认尺寸: 64 * 8 = 512byte
//! 尺寸 = MAX_BUF_SZIE * 8
#ifndef MAX_TILE_BUF_SZIE
#define MAX_TILE_BUF_SZIE 64
#endif

//! 请求内存尺度的最小区别
const long MIN_BUF_SIZE_DIFFERENCE = 8;


//! 默认大块缓冲尺寸
//! 这个尺寸只相对于可向外分配的空间，不是指实际一个块的大小
const long USABLE_BLOCK_SIZE = MAX_TILE_BUF_SZIE * MIN_BUF_SIZE_DIFFERENCE * 128;

/*
内存释放错误枚举
*/
enum eFreeError
{
	eFE_NotErr,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
链表节点
*/
template<typename NodeData>
class ListNode
{
	typedef		ListNode<NodeData>			MyType;
	typedef		ListNode<NodeData>*			LPMyType;

public:
	ListNode(void):m_pNext(NULL){}
	LPVOID operator new(size_t, LPVOID pBuf){return pBuf;}

public:
	LPMyType		GetNext(void)			{return m_pNext;};
	void			SetNext(LPMyType pNext)	{m_pNext = pNext;};
	LPVOID			GetTile(void)			{return (&m_pNext) + 1;}

	NodeData*		GetNodeData(void)		{return &m_NodeData;}
	void			Rlease(void)			{memset(&m_NodeData, 0, sizeof(NodeData)); m_pNext = NULL;}

	static	long	GetAddedSize(void)		{return sizeof(NodeData) + sizeof(LPMyType);}
private:
	NodeData	m_NodeData;
	LPMyType	m_pNext;
};

template<>
class ListNode<void>
{
	typedef		ListNode<void>			MyType;
	typedef		ListNode<void>*			LPMyType;

public:

	ListNode(void):m_pNext(NULL){}
	LPVOID operator new(size_t, LPVOID pBuf){return pBuf;}

public:

	LPMyType		GetNext(void)			{return m_pNext;};
	void			SetNext(LPMyType pNext)	{m_pNext = pNext;};
	LPVOID			GetTile(void)			{return (&m_pNext) + 1;}

	void			Rlease(void)			{m_pNext = NULL;}

	static	long	GetAddedSize(void)		{return sizeof(LPMyType);}

private:

	LPMyType	m_pNext;

};


/*
链表
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename NodeData>
class MyList
{
public:

	typedef ListNode<NodeData>	NodeType;

public:
	MyList(void)
		: m_pHead(NULL)
		, m_NodeNum(0)
	{}


	//! 
	void PushNode(NodeType *pNode)
	{
		pNode->SetNext(m_pHead);
		m_pHead = pNode;
		++m_NodeNum;
	}

	//! 
	NodeType* PopNode(void)
	{
		if(NULL == m_pHead) return NULL;
		NodeType *pReNode = m_pHead;
		m_pHead = m_pHead->GetNext();
		--m_NodeNum;
		MyAssert(0 <= m_NodeNum);
		return pReNode;
	}

	void Release(void)
	{
		m_pHead		= NULL;
		m_NodeNum	= 0;
	}

	//! 
	long Size(void)
	{
		return m_NodeNum;
	}

#ifdef MEM_POOL_DEBUG
	//!
	NodeType* GetHead(void)
	{
		return m_pHead;
	}
#endif


private:
	NodeType					*m_pHead;
	long						m_NodeNum;
};



/////////////////////////////////////////////////////////////////////////////
//! 一个WIN32的锁
class LockOfWin32
{
public:
	LockOfWin32(void)		{InitializeCriticalSection(&m_CriticalSection);}
	~LockOfWin32(void)		{DeleteCriticalSection(&m_CriticalSection);}
public:
	void	Lock(void)		{EnterCriticalSection(&m_CriticalSection);}
	void	UnLock(void)	{LeaveCriticalSection(&m_CriticalSection);}
private:
	CRITICAL_SECTION		m_CriticalSection;
};


/////////////////////////////////////////////////////////////////////////////
//! 一个无效的锁，用于不需要同步的对象
class InvalidLock
{
public:
	InvalidLock(void)		{}
	~InvalidLock(void)		{}
public:
	void	Lock(void)		{}
	void	UnLock(void)	{}
};