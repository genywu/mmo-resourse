


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
		CObjectFactory(CMemManage<_TLock> &MemManage);
		~CObjectFactory(void);

	public:
		//!
		_TObject*								CreateObj(void);
		//!
		_TObject*								CreateObj(void* pConstructParam);
		//!	
		void									ReleaseObj(_TObject *pObj);

	private:
		//!
		CMemManage<_TLock>						&m_MemManage;
		//!										对象运行时ID
		unsigned long							m_uObjRTID;
	};


	//////////////////////////////////////////////////////////////////////////////////////////
	//!
	template<typename _TObject, typename _TLock>
	CObjectFactory<_TObject, _TLock>&		GetFactory(void);

	template<typename _TLock>
	_TLock&									GetObjInfoManageLock(void)
	{
		static _TLock c_ObjInfoManageLock;
		return c_ObjInfoManageLock;
	}


};

#include "../cpp/ObjectFactory.cpp"
#endif