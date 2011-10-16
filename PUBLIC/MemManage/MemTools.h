


/*
	内存管理器：
		通过定义USE_IN_SYNC_SYSTEM宏，确定内存管理系统用于同步还是异步
		通过定义SHRINK_SCALE宏的值，可以减少预分配内存的大小，这个值最好定义为2的次方数，在默认情况下，预分配内存大小约为1.1GB
*/


#include "include/Tools.h"

using namespace XM_Tools;

#ifndef _MEM_TOOLS_H_
#define _MEM_TOOLS_H_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 内存管理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!					设置输出信息参数（Param_OutFun必须为：void OutInfoFun(const char *pFileName, const char *pText)格式）
#define				SET_OUT_INFO_SETUP(Param_OutFun, Param_FileName)				__SET_OUT_INFO_SETUP__((Param_OutFun), (Param_FileName))
//!					输出运行时信息
#define				OUT_RUNNING_INFO()												__OUT_RUNNING_INFO__


	#ifndef USE_IN_SYNC_SYSTEM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 内存管理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!					异步分配内存
//!					（Param_Size 类型：unsigned long）
#define				M_ALLOC(Param_Size)												__ASYNC_ALLOC__(Param_Size)
//!					异步释放内存
//!					（Param_Pointer 类型：void*，Param_Size 类型：unsigned long）
#define				M_FREE(Param_Pointer, Param_Size)								__ASYNC_FREE__((Param_Pointer), (Param_Size));

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 对象工厂接口
//!		用法：
//!			class CTestClase;
//!				无参构造	OBJECT_FACTORY(CTestClase).CreateObj(void);
//!				参数构造	OBJECT_FACTORY(CTestClase).CreateObj(void* pConstructParam);
//!				释放		OBJECT_FACTORY(CTestClase).ReleaseObj(CTestClase *pCTestClase);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//!					得到对象工厂
//!					（参数类型：class/struct）
#define				OBJECT_FACTORY(_TClassName)										__ASYNC_OBJECT_FACTORY__(_TClassName)

	#else //! USE_IN_SYNC_SYSTEM

	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 内存管理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!					同步分配内存
//!					（Param_Size 类型：unsigned long）
#define				M_ALLOC(Param_Size)												__SYNC_ALLOC__(Param_Size)
//!					同步释放内存
//!					（Param_Pointer 类型：void*，Param_Size 类型：unsigned long）
#define				M_FREE(Param_Pointer, Param_Size)								__SYNC_FREE__((Param_Pointer), (Param_Size));

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 对象工厂接口
//!		用法：
//!			class CTestClase;
//!			同步:
//!				无参构造	SYNC_OBJECT_FACTORY(CTestClase).CreateObj(void);
//!				参数构造	SYNC_OBJECT_FACTORY(CTestClase).CreateObj(void* pConstructParam);
//!				释放		SYNC_OBJECT_FACTORY(CTestClase).ReleaseObj(CTestClase *pCTestClase);
//!			异步:
//!				无参构造	ASYNC_OBJECT_FACTORY(CTestClase).CreateObj(void);
//!				参数构造	ASYNC_OBJECT_FACTORY(CTestClase).CreateObj(void* pConstructParam);
//!				释放		ASYNC_OBJECT_FACTORY(CTestClase).ReleaseObj(CTestClase *pCTestClase);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//!					得到对象工厂
//!					（参数类型：class/struct）
#define				OBJECT_FACTORY(_TClassName)										__SYNC_OBJECT_FACTORY__(_TClassName)



	#endif //! USE_IN_SYNC_SYSTEM

#endif //! _MEM_TOOLS_H_