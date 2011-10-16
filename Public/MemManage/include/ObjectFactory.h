


/*

*/

#include "MemManage.h"


#ifndef _OBJECT_FACTORY_H_
#define _OBJECT_FACTORY_H_

namespace XM_Tools
{

	//////////////////////////////////////////////////////////////////////////////////////////
	//! 对象工厂
	//////////////////////////////////////////////////////////////////////////////////////////
	template<typename _TObject, typename _TLock>
	class CObjectFactory
	{
	public:
#ifdef LOG_ALLOC_FREE_INFO
		CObjectFactory(CMemManage<_TLock> &MemManage, const char *pClassName);
#else
		CObjectFactory(CMemManage<_TLock> &MemManage);
#endif
		~CObjectFactory(void);

	public:
#ifdef LOG_ALLOC_FREE_INFO
		//!
		inline _TObject*						CreateObj(const char *pClassName, const char *pInfo);
		//!
		inline _TObject*						CreateObj(void* pConstructParam, const char *pClassName, const char *pInfo);
		//!	
		inline void								ReleaseObj(_TObject *pObj, const char *pClassName, const char *pInfo);

		//!
		inline _TObject*						CreateObjArray(unsigned long uNum, const char *pClassName, const char *pInfo);
		//!
		inline _TObject*						CreateObjArray(unsigned long uNum, void* pConstructParam, const char *pClassName, const char *pInfo);
		//!	
		inline void								ReleaseObjArray(_TObject *pObj, const char *pClassName, const char *pInfo);

#else
		//!
		inline _TObject*						CreateObj(void);
		//!
		inline _TObject*						CreateObj(void* pConstructParam);
		//!	
		inline void								ReleaseObj(_TObject *pObj);

		//!
		inline _TObject*						CreateObjArray(unsigned long uNum);
		//!
		inline _TObject*						CreateObjArray(unsigned long uNum, void* pConstructParam);
		//!	
		inline void								ReleaseObjArray(_TObject *pObj);
#endif

	private:
		//!
		CMemManage<_TLock>						&m_MemManage;
		//!										对象运行时ID
		unsigned long							m_uObjRTID;
	};


	//////////////////////////////////////////////////////////////////////////////////////////
	//!
#ifdef LOG_ALLOC_FREE_INFO
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&	GetFactory(const char *pClassName);
#else
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&	GetFactory(void);
#endif

	template<typename _TLock>
	_TLock&									GetObjInfoManageLock(void)
	{
		static _TLock c_ObjInfoManageLock;
		return c_ObjInfoManageLock;
	}


};

#include "../cpp/ObjectFactory.tcc"
#endif