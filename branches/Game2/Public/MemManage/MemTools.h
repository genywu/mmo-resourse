


/**
*	内存管理器：
*		通过定义USE_IN_SYNC_SYSTEM宏，确定内存管理系统用于同步还是异步
*		通过定义SHRINK_SCALE宏的值，可以减少预分配内存的大小，这个值定义应在0~31之间，预分配的内存将减少到2的SHRINK_SCALE次方倍
*		通过定义USE_C_LIB_ALLOC宏，可以将内存管理工作交给C库malloc\free函数，模块只保留调试功能
*
*	内存管理工具分为四个部分：信息管理、基本内存管理、被内存池管理的对象、对象工厂模板
*		其中，后面三个分配部分的分配和释放都必须各自成对调用，不能混淆。
*/


#include "include/Tools.h"

using namespace XM_Tools;

#ifndef _MEM_TOOLS_H_
#define _MEM_TOOLS_H_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 信息管理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!					设置输出信息参数（Param_OutFun必须为：void OutInfoFun(const char *pFileName, const char *pText)格式）
#define				SET_OUT_INFO_SETUP(Param_OutFun, Param_FileName)				__SET_OUT_INFO_SETUP__((Param_OutFun), (Param_FileName))
//!					输出运行时信息
#define				OUT_RUNNING_INFO(Param_Detailed)								__OUT_RUNNING_INFO__(Param_Detailed)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 基本内存管理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!					异步分配内存
//!					（Param_Size 类型：unsigned long）
#define				M_ALLOC(Param_Size)												__ALLOC__(Param_Size)
//!					异步释放内存
//!					（Param_Pointer 类型：void*，Param_Size 类型：unsigned long）
#define				M_FREE(Param_Pointer, Param_Size)								__FREE__((Param_Pointer), (Param_Size))

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 被内存池管理的对象
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define				MP_NEW															__NEW__
#define				MP_DELETE(Param_Pointer)										__SAFE_DELETE__(Param_Pointer)
#define				MP_DELETE_ARRAY(Param_Pointer)									__SAFE_DELETE_ARRAY__(Param_Pointer)

//!					对象要被内存池管理，需从此类开始继承
typedef 			CMemObject														BaseMemObj;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 对象工厂模板
//! 有继承关系对象，必须带有虚析构函数否则将造成析构不完全
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!					创建对象（无参构造函数）
//!					（参数类型：class/struct）
#define				OBJ_CREATE(_TClassName)											__OBJ_ALLOC__(_TClassName)
//!					创建对象（LPVOID类型参数构造函数）
//!					（_TClassName：class/struct，Param_ConstructParam：LPVOID类型参数指针）
#define				OBJ_CREATE_PVOID(_TClassName, Param_ConstructParam)				__OBJ_ALLOC_PVOID__(_TClassName, (Param_ConstructParam))
//!					释放对象
#define				OBJ_RELEASE(_TClassName, Param_Pointer)							__OBJ_FREE__(_TClassName, (Param_Pointer))


#endif //! _MEM_TOOLS_H_