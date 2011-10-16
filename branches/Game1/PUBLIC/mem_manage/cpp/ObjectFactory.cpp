



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
	CObjectFactory<_TObject, _TLock>::CObjectFactory(CMemManage<_TLock> &MemManage)
		:m_MemManage(MemManage)
	{
		//! 获取ID
		m_uObjRTID = GetObjInfoManage().GetNewRTID();
		//! 创建注册名称
#ifdef LOG_ALLOC_FREE_INFO
		std::string strRegName = std::string(typeid(_TObject).name()) + "<" + typeid(_TLock).name() + ">";
		bool bRegResult = GetObjInfoManage().RegisterObj(strRegName, m_uObjRTID);
#else
		bool bRegResult = GetObjInfoManage().RegisterObj(m_uObjRTID);
#endif
		MM_ASSERY(bRegResult);
	}

	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>::~CObjectFactory(void)
	{

	}

	//!
	template<typename _TObject, typename _TLock>
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(void)
	{
#ifdef _LOG_ALLOC_FILE_INFO_
		_TObject *pObj = (_TObject*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject), (std::string(typeid(_TObject).name()) + "<" + typeid(_TLock).name() + "> Factory").c_str());
#else
		_TObject *pObj = (_TObject*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject));
#endif
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
	_TObject* CObjectFactory<_TObject, _TLock>::CreateObj(void* pConstructParam)
	{
#ifdef _LOG_ALLOC_FILE_INFO_
		_TObject *pObj = (_TObject*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject), (std::string(typeid(_TObject).name()) + "<" + typeid(_TLock).name() + "> Factory").c_str());
#else
		_TObject *pObj = (_TObject*)GetMemManage<_TLock>().AsyncAlloc(sizeof(_TObject));
#endif
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
	void CObjectFactory<_TObject, _TLock>::ReleaseObj(_TObject *pObj)
	{
		MM_ASSERY(NULL != pObj);
		pObj->~_TObject();
		GetMemManage<_TLock>().AsyncFree(pObj, sizeof(_TObject));
		GetObjInfoManageLock<_TLock>().Lock();
		{
			GetObjInfoManage().DelObj(m_uObjRTID);
		}
		GetObjInfoManageLock<_TLock>().UnLock();
	}


	




	//////////////////////////////////////////////////////////////////////////////////////////
	//! 
	//////////////////////////////////////////////////////////////////////////////////////////
	//!
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&	GetFactory(void)
	{
		static CObjectFactory<_TObject, _TLock> c_ObjectFactory(GetMemManage<_TLock>());
		return c_ObjectFactory;
	}
};
#endif //! _OBJECT_FACTORY_CPP_