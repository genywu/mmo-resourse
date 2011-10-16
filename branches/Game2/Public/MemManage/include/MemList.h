


/*
本文件定义了内存管理的节点使用的链表数据类型

*/

#include "mmdef.h"

#ifndef _MEM_LIST_H_
#define _MEM_LIST_H_

namespace XM_Tools
{
	/*
	============================================================================
	============================================================================
	链表节点
	============================================================================
	============================================================================
	*/
//!			链表节点对齐值
#define		LIST_NODE_GRID_SIZE		4

#pragma pack(push)
#pragma pack(LIST_NODE_GRID_SIZE)
	
	//! ------------------------------------------------------------------------
	//! 通用模板
	template<typename _TNodeInfo>
	class CListNode
	{
	public:
		inline CListNode*		GetNext(void)				{return m_pNext;}
		inline void				SetNext(CListNode *pNext)	{m_pNext = pNext;}
		inline _TNodeInfo& 		GetInfo(void)				{return m_pInfo;}
		inline void 			SetInfo(_TNodeInfo &Info)	{m_pInfo = Info;}

		//!						内存重用方式取得可用内存（调用前必须保证节点已经在管理LIST中移除）
		inline void*			GetReBuf(void)				{return &m_pNext;}
		//!						普通方式取得可用内存
		inline void*			GetBuf(void)				{return ++(&m_pNext);}

	private:
		//!						节点信息，这个成员必须为对象的第一个成员（m_pNext内存复用）
		_TNodeInfo				m_pInfo;
		//!	
		CListNode<_TNodeInfo>	*m_pNext;
		//! 不允许构造该对象
	private:
		CListNode(void);
		~CListNode(void);
	};

	//! ------------------------------------------------------------------------
	//! 特化模板
	template<>
	class CListNode<void>
	{
	public:
		inline CListNode*		GetNext(void)					{return m_pNext;}
		inline void				SetNext(CListNode<void> *pNext)	{m_pNext = pNext;}

		//!						内存重用方式取得可用内存
		inline void*			GetReBuf(void)					{return &m_pNext;}
		//!						普通方式取得可用内存
		inline void*			GetBuf(void)					{return (&m_pNext) + 1;}

	private:
		CListNode<void>			*m_pNext;

		//! 不允许构造该对象
	private:
		CListNode(void);
		~CListNode(void);
	};

#pragma pack(pop)
	/*
	============================================================================
	============================================================================
	链表
	============================================================================
	============================================================================
	*/
	template<typename _TNodeInfo, typename _TLock>
	class CMemList
	{
	public:
		inline CMemList(void);
		inline ~CMemList(void);

	public:
		//!								压入一个节点
		inline void						PushNode(CListNode<_TNodeInfo> *pNode);
		//!								取出一个节点
		inline CListNode<_TNodeInfo>*	PopNode(void);
		//!								得到节点数
		inline unsigned long			GetNodeNum(void) const;

	private:
		CListNode<_TNodeInfo>			*m_pHead;
		_TLock							m_Lock;
		unsigned long					m_uNum;
	};

	/*
	============================================================================
	============================================================================
	双向链表
	============================================================================
	============================================================================
	*/
	template<typename _TNodeInfo>
	class CListBNode
	{
	public:
		inline CListBNode*		GetPrev(void)				{return m_pPrev;}
		inline void				SetPrev(CListBNode *pPrev)	{m_pPrev = pPrev;}
		inline CListBNode*		GetNext(void)				{return m_pNext;}
		inline void				SetNext(CListBNode *pNext)	{m_pNext = pNext;}
		inline _TNodeInfo& 		GetInfo(void)				{return m_pInfo;}
		inline void 			SetInfo(_TNodeInfo &Info)	{m_pInfo = Info;}

		//!						内存重用方式取得可用内存（调用前必须保证节点已经在管理LIST中移除）
		inline void*			GetReBuf(void)				{return &m_pPrev;}
		//!						普通方式取得可用内存
		inline void*			GetBuf(void)				{return ++(&m_pNext);}

	private:
		//!						节点信息，这个成员必须为对象的第一个成员（m_pPrev，m_pNext内存复用）
		_TNodeInfo				m_pInfo;
		//!
		CListBNode<_TNodeInfo>	*m_pPrev;
		//!	
		CListBNode<_TNodeInfo>	*m_pNext;
		//! 不允许构造该对象
	private:
		CListBNode(void);
		~CListBNode(void);
	};


	/*
	============================================================================
	============================================================================
	有序链表
	============================================================================
	============================================================================
	*/
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	class CMemSerialBList
	{
	public:
		inline CMemSerialBList(void);
		inline ~CMemSerialBList(void);

	public:
		//!									压入一个节点
		inline void							PushNode(CListBNode<_TNodeInfo> *pNode);
		//!									取出一个节点
		inline CListBNode<_TNodeInfo>*		PopNode(void);
		//!									取出一个节点
		inline CListBNode<_TNodeInfo>*		PopBackNode(void);
		//!									取出一个节点
		inline CListBNode<_TNodeInfo>*		PopNode(CListBNode<_TNodeInfo> *pNode);
		//!									找到一个节点
		inline bool							Find(CListBNode<_TNodeInfo> *pNode);
		//!									取出数据最像的一个节点
		inline CListBNode<_TNodeInfo>*		GetLikestNode(_TNodeInfo &NodeInfo);
		//!									得到节点数
		inline unsigned long				GetNodeNum(void) const;

	private:
		//!
		CListBNode<_TNodeInfo>				*m_pHead;
		//!
		CListBNode<_TNodeInfo>				*m_pFoot;
		//!
		_TLock								m_Lock;
		//!
		unsigned long						m_uNum;
	};

};

#include "../cpp/MemList.tcc"

#endif //! _MEM_LIST_H_