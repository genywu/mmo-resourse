
#ifndef _MEM_LIST_CPP_
#define _MEM_LIST_CPP_


#include "../include/MemList.h"

namespace XM_Tools
{
	/*
	============================================================================
	============================================================================
	链表
	============================================================================
	============================================================================
	*/
	template<typename _TNodeInfo, typename _TLock>
	CMemList<_TNodeInfo, _TLock>::CMemList(void)
		:m_pHead(NULL)
		,m_uNum(0)
	{

	}

	template<typename _TNodeInfo, typename _TLock>
	CMemList<_TNodeInfo, _TLock>::~CMemList(void)
	{
		m_pHead = NULL;
		m_uNum = 0;
	}

	//! 压入一个节点
	template<typename _TNodeInfo, typename _TLock>
	void CMemList<_TNodeInfo, _TLock>::PushNode(CListNode<_TNodeInfo> *pNode)
	{
		m_Lock.Lock();
		{
			pNode->SetNext(m_pHead);
			m_pHead = pNode;
			++m_uNum;
		}
		m_Lock.UnLock();
	}

	//! 取出一个节点
	template<typename _TNodeInfo, typename _TLock>
	CListNode<_TNodeInfo>* CMemList<_TNodeInfo, _TLock>::PopNode(void)
	{
		CListNode<_TNodeInfo>* pRe = NULL;
		m_Lock.Lock();
		{
			pRe = m_pHead;
			if(NULL != m_pHead)
			{
				m_pHead = m_pHead->GetNext();
				--m_uNum;
			}
		}
		m_Lock.UnLock();
		return pRe;
	}

	//! 得到节点数
	template<typename _TNodeInfo, typename _TLock>
	unsigned long CMemList<_TNodeInfo, _TLock>::GetNodeNum(void) const
	{
		return m_uNum;
	}




	/*
	============================================================================
	============================================================================
	双向链表
	============================================================================
	============================================================================
	*/


	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::CMemSerialBList(void)
		:m_pHead(NULL)
		,m_pFoot(NULL)
		,m_uNum(0)
	{
		m_pHead = NULL;
		m_uNum = 0;
	}

	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::~CMemSerialBList(void)
	{

	}


	//! 压入一个节点
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	void CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::PushNode(CListBNode<_TNodeInfo> *pNode)
	{
		m_Lock.Lock();
		{
			if(NULL == m_pHead)
			{
				pNode->SetNext(NULL);
				pNode->SetPrev(NULL);
				m_pHead = pNode;
				m_pFoot = pNode;
			}
			else
			{
				CListBNode<_TNodeInfo>* pCurr = m_pHead;
				do 
				{
					//! 如果当前节点大于新节点
					if(pNode->GetInfo() < pCurr->GetInfo())
					{
						//! 插入到当前节点之前
						pNode->SetNext(pCurr);
						pNode->SetPrev(pCurr->GetPrev());
						pCurr->SetPrev(pNode);			
						if(NULL == pNode->GetPrev())
							m_pHead = pNode;
						else
							pNode->GetPrev()->SetNext(pNode);
						break;
					}
					//! 如果后面没有节点
					else if(NULL == pCurr->GetNext())
					{
						//! 插入到当前节点之后
						pNode->SetPrev(pCurr);
						pNode->SetNext(pCurr->GetNext());
						pCurr->SetNext(pNode);
						if(NULL == pNode->GetNext())
							m_pFoot = pNode;
						else
							pNode->GetNext()->SetPrev(pNode);
						break;
					}
					else
					{
						pCurr = pCurr->GetNext();
					}
				} while(true);
			}

			++m_uNum;
		}
		m_Lock.UnLock();
	}


	//! 取出一个节点
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	CListBNode<_TNodeInfo>* CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::PopNode(void)
	{
		CListBNode<_TNodeInfo>* pRe = NULL;
		m_Lock.Lock();
		{
			pRe = m_pHead;
			if(NULL != m_pHead)
			{
				m_pHead = m_pHead->GetNext();
				if(NULL == m_pHead)
					m_pFoot = NULL;
				--m_uNum;
			}
		}
		m_Lock.UnLock();
		return pRe;
	}

	//! 取出一个节点
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	CListBNode<_TNodeInfo>* CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::PopNode(CListBNode<_TNodeInfo> *pNode)
	{
		CListBNode<_TNodeInfo>* pRe = NULL;
		m_Lock.Lock();
		{
			CListBNode<_TNodeInfo>* pCurr = m_pHead;
			while (NULL != pCurr)
			{
				if(pCurr == pNode)
				{
					pRe = pCurr;
					if(NULL != pCurr->GetPrev())
						pCurr->GetPrev()->SetNext(pCurr->GetNext());
					else
						m_pHead = pCurr->GetNext();
					if(NULL != pCurr->GetNext())
						pCurr->GetNext()->SetPrev(pCurr->GetPrev());
					else
						m_pFoot = pCurr->GetPrev();

					--m_uNum;
					break;
				}

				pCurr = pCurr->GetNext();
			}	
		}
		m_Lock.UnLock();
		return pRe;
	}

	//! 找到一个节点
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	bool CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::Find(CListBNode<_TNodeInfo> *pNode)
	{
		bool bRe = false;
		m_Lock.Lock();
		{
			CListBNode<_TNodeInfo>* pCurr = m_pHead;
			while (NULL != pCurr)
			{
				if(pCurr == pNode)
				{
					bRe = true;
					break;	
				}
				pCurr = pCurr->GetNext();
			}
		}
		m_Lock.UnLock();
		return bRe;
	}

	//! 取出最像的一个节点
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	CListBNode<_TNodeInfo>* CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::GetLikestNode(_TNodeInfo &NodeInfo)
	{
		CListBNode<_TNodeInfo> *pRe = NULL;
		m_Lock.Lock();
		{
			CListBNode<_TNodeInfo>* pCurr = m_pHead;
			while (NULL != pCurr)
			{
				if(_TLikeCompare()(pCurr->GetInfo(), NodeInfo))
				{
					pRe = pCurr;
					if(NULL != pCurr->GetPrev())
						pCurr->GetPrev()->SetNext(pCurr->GetNext());
					else
						m_pHead = pCurr->GetNext();
					if(NULL != pCurr->GetNext())
						pCurr->GetNext()->SetPrev(pCurr->GetPrev());
					else
						m_pFoot = pCurr->GetPrev();

					--m_uNum;
					break;
				}

				pCurr = pCurr->GetNext();
			}
		}
		m_Lock.UnLock();

		return pRe;
	}

	//! 得到节点数
	template<typename _TNodeInfo, typename _TLock, typename _TLikeCompare>
	unsigned long CMemSerialBList<_TNodeInfo, _TLock, _TLikeCompare>::GetNodeNum(void) const
	{
		return m_uNum;
	}

};


#endif //! _MEM_LIST_CPP_