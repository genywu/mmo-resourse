



#ifndef _OBJECT_FACTORY_CPP_
#define _OBJECT_FACTORY_CPP_

#include "../include/ObjectFactory.h"
#include "../include/ObjInfoManage.h"

namespace XM_Tools
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//! 对象工厂
	//////////////////////////////////////////////////////////////////////////////////////////
	template<typename _TObject, typename _TLock>
#ifdef LOG_ALLOC_FREE_INFO
	CObjectFactory<_TObject, _TLock>::CObjectFactory(CMemManage<_TLock> &MemManage, const char *pClassName)
#else
	CObjectFactory<_TObject, _TLock>::CObjectFactory(CMemManage<_TLock> &MemManage)
#endif
		:m_MemManage(MemManage)
	{
		//! 获取ID
		m_uObjRTID = GetObjInfoManage().GetNewRTID();
		//! 创建注册名称
#ifdef LOG_ALLOC_FREE_INFO
		MM_ASSERY(500 > strlen(pClassName));
		char szTempInfo[2048] = {0};
		sprintf(szTempInfo, "%s(%dbyte)", pClassName, sizeof(_TObject) + sizeof(long) * 2);
		bool bRegResult = GetObjInfoManage().RegisterObj(szTempInfo, m_uObjRTID);
#endif
		MM_ASSERY(bRegResult);
	}

	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>::~CObjectFactory(void)
	{

	}

#ifdef LOG_ALLOC_FREE_INFO

	struct tagObjPointerInfo
	{
		unsigned short	ID;
		unsigned short	Num;
		unsigned long	Size;
	};

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(const char *pClassName, const char *pInfo)
	{
		long *pNeedData = (long*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject) + sizeof(long) * 2, (std::string(pClassName) + " Factory" + "<" + pInfo + ">").c_str());
		pNeedData[0] = sizeof(_TObject) + sizeof(long) * 2;
		pNeedData[1] = m_uObjRTID;
		_TObject *pObj = (_TObject*)&pNeedData[2];
		if(NULL != pObj)
		{
			new(pObj)_TObject();
			GetObjInfoManageLock<_TLock>().Lock();
			{
				GetObjInfoManage().AddObj(m_uObjRTID);
			}
			GetObjInfoManageLock<_TLock>().UnLock();
		}
		return pObj;
	}


	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(void* pConstructParam, const char *pClassName, const char *pInfo)
	{
		long *pNeedData = (long*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject) + sizeof(long) * 2, (std::string(pClassName) + "Factory" + "<" + pInfo + ">").c_str());
		pNeedData[0] = sizeof(_TObject) + sizeof(long) * 2;
		pNeedData[1] = m_uObjRTID;
		_TObject *pObj = (_TObject*)&pNeedData[2];
		if(NULL != pObj)
		{
			new(pObj)_TObject(pConstructParam);
			GetObjInfoManageLock<_TLock>().Lock();
			{
				GetObjInfoManage().AddObj(m_uObjRTID);
			}
			GetObjInfoManageLock<_TLock>().UnLock();
		}
		return pObj;
	}

	//!	
	template<typename _TObject, typename _TLock>
	void CObjectFactory<_TObject, _TLock>::ReleaseObj(_TObject *pObj, const char *pClassName, const char *pInfo)
	{
		if(NULL == pObj)
			return;

		pObj = dynamic_cast<_TObject*>(pObj);
		long *pNeedData = (long*)pObj;
		pObj->~_TObject();
		GetObjInfoManageLock<_TLock>().Lock();
		{
			GetObjInfoManage().DelObj(pNeedData[-1]);
		}
		GetObjInfoManageLock<_TLock>().UnLock();
		GetMemManage<_TLock>().AsyncFree(pNeedData - 2, pNeedData[-2], (std::string(pClassName) + "Factory" + "<" + pInfo + ">").c_str());
	}

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObjArray(unsigned long uNum, const char *pClassName, const char *pInfo)
	{

	}

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObjArray(unsigned long uNum, void* pConstructParam, const char *pClassName, const char *pInfo)
	{

	}

	//!	
	template<typename _TObject, typename _TLock>
	void CObjectFactory<_TObject, _TLock>::ReleaseObjArray(_TObject *pObj, const char *pClassName, const char *pInfo)
	{

	}

#else
	
	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(void)
	{
		unsigned long AllocSize = sizeof(unsigned long) + sizeof(_TObject);
		unsigned long *pMem = (unsigned long*)GetMemManage<_TLock>().AsyncAlloc(AllocSize);
		if(NULL == pMem)
			return NULL;
		
		pMem[0] = AllocSize;
		_TObject *pObj = (_TObject*)(pMem + 1);
		new(pObj)_TObject();
		return pObj;
	}


	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(void* pConstructParam)
	{
		unsigned long AllocSize = sizeof(unsigned long) + sizeof(_TObject);
		unsigned long *pMem = (unsigned long*)GetMemManage<_TLock>().AsyncAlloc(AllocSize);
		if(NULL == pMem)
			return NULL;

		pMem[0] = AllocSize;
		_TObject *pObj = (_TObject*)(pMem + 1);
		new(pObj)_TObject(pConstructParam);
		return pObj;
	}

	//!	
	template<typename _TObject, typename _TLock>
	void CObjectFactory<_TObject, _TLock>::ReleaseObj(_TObject *pObj)
	{
		//MM_ASSERY(NULL != pObj);
		if(NULL == pObj)
			return;
		pObj->~_TObject();
		unsigned long *pMem = (unsigned long*)pObj;
		--pMem;
		GetMemManage<_TLock>().AsyncFree(pMem, pMem[0]);
	}

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObjArray(unsigned long uNum)
	{

	}

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObjArray(unsigned long uNum, void* pConstructParam)
	{

	}

	//!	
	template<typename _TObject, typename _TLock>
	void CObjectFactory<_TObject, _TLock>::ReleaseObjArray(_TObject *pObj)
	{

	}

#endif

	


	




	//////////////////////////////////////////////////////////////////////////////////////////
	//! 
	//////////////////////////////////////////////////////////////////////////////////////////
	//!
#ifdef LOG_ALLOC_FREE_INFO
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&	GetFactory(const char *pClassName)
	{
		static CObjectFactory<_TObject, _TLock> c_ObjectFactory(GetMemManage<_TLock>(), pClassName);
		return c_ObjectFactory;
	}
#else
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&	GetFactory(void)
	{
		static CObjectFactory<_TObject, _TLock> c_ObjectFactory(GetMemManage<_TLock>());
		return c_ObjectFactory;
	}
#endif
};
#endif //! _OBJECT_FACTORY_CPP_