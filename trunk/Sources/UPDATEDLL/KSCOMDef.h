//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSCOMDef.h
//  Creater     :   Li Yu Xiong
//  Date        :   2002-08-14 16:54:06
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KSCOM_DEF_H_
#define _KSCOM_DEF_H_


#ifndef BEGIN_INTERFACE

#ifdef __KS_VC_COMPATIBILITY_GCC__
#ifndef BEGIN_INTERFACE
   #define BEGIN_INTERFACE virtual void __KS_VC_COMPATIBILITY_GCC__a() {} virtual void __KS_VC_COMPATIBILITY_GCC__b() {}
#endif
#else   // __KAV_VC_COMP_GCC__
#ifndef BEGIN_INTERFACE
   #define BEGIN_INTERFACE
#endif
#endif  // __KAV_VC_COMP_GCC__

#endif  // BEGIN_INTERFACE


#ifdef  KSIDL_DEFINE
#include "KSUnknown.h"
#else   // not KSIDL_DEFINE
#include "Unknwn.h"
#define _VC_COMPLIER_   1
#endif  // KSIDL_DEFINE

#ifndef interface
#define interface struct
#endif // interface

#include "assert.h"
#include "KSCOMError.h"


#ifndef USE_KSCOM
#define USE_KSCOM
#endif

//interface IUnknown;
#ifndef CALLTYPE_DEFINED
#define CALLTYPE_DEFINED

#ifdef  _VC_COMPLIER_
#define KSCOMSTDMETHOD		    __stdcall
#define KSCOMFASTCALLMETHOD		__fastcall
#define KSCOMCDECLMETHOD		__cdecl
#elif   _GCC_COMPLIER_
#define KSCOMSTDMETHOD          __attribute__ ((stdcall))
#define KSCOMFASTCALLMETHOD
#define KSCOMCDECLMETHOD
#else   // NOT
#define KSCOMSTDMETHOD
#define KSCOMFASTCALLMETHOD
#define KSCOMCDECLMETHOD
#endif  // NOT

#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE KSCOMSTDMETHOD
#endif

#endif  // CALLTYPE_DEFINED


typedef void * KSCOMHANDLE;

#ifndef ASSERT
#define ASSERT assert
#endif



                                
#ifndef UUID_DEFINED
#define UUID_DEFINED

typedef struct _UUID {          // size is 16
    unsigned int    Data1;
    unsigned short  Data2;
    unsigned short  Data3;
    unsigned char   Data4[8];
} UUID;

#endif // !UUID_DEFINED


inline int KSCOM_EQUAL_UUID(const UUID &cruuid1, const UUID &cruuid2)
{
    typedef int *PINT32;
    return (
        ((((PINT32)&cruuid1)[0]) == (((PINT32)&cruuid2)[0])) &&
        ((((PINT32)&cruuid1)[1]) == (((PINT32)&cruuid2)[1])) &&
        ((((PINT32)&cruuid1)[2]) == (((PINT32)&cruuid2)[2])) &&
        ((((PINT32)&cruuid1)[3]) == (((PINT32)&cruuid2)[3]))
    );
}

//inline int KSCOM_EQUAL_UUID(const UUID &cruuid1, const UUID &cruuid2)
//{
//    return !memcmp((void *)cruuid1, (void *)cruuid2, sizeof(UUID));
//}

// =========================================================================
// *       KSCOM Client Registry and Initialization Function Definition    *
// =========================================================================


typedef KSCOMRESULT (KSCOMClientRegFun)(void);      
typedef KSCOMRESULT (KSCOMClientUnRegFun)(void);
typedef int (KSCOMClientInitFun)(void);
typedef int (KSCOMClientUnInitFun)(void);


typedef struct tagKSCOM_CLIENT_INIT{
    int nRegFlag;                                   // Register Flag
    KSCOMClientRegFun *pfKSCOMClientRegister;       // Register Function
    KSCOMClientUnRegFun *pfKSCOMClientUnRegister;   // UnRegister Function
    int nInitFlag;                                  // Initialize Flag
    KSCOMClientInitFun *pfKSCOMClientInit;          // Initialize Function
    KSCOMClientUnInitFun *pfKSCOMClientUnInit;      // UnInitialize Function
} KSCOM_CLIENT_INIT;


// =========================================================================
// *        Kingsoft COM API Dynamic Load Module Export Function           *
// =========================================================================

#define KSCOM_GET_CLASS_OBJECT_FUNCTION_NAME "KSCOMGetClassObject"

typedef KSCOMRESULT KSCOMSTDMETHOD TKSCOMGetClassObjectFunction(
    const UUID &cruuidCLSID, const UUID &cruuidIID, void ** ppvClassObject
);


#define KS_SET_COM_OPERATOR_FUNCTION_NAME "KSSetCOMOperatorFunction"

typedef int KSCOMSTDMETHOD TKSSetCOMOperatorFunction(
    int nKSCOMOperatorFunctionNumber, void *pfnKSCOMOperatorFunctionTable[]
);


#define KSCOM_COMPONENT_INIT_FUNCTION_NAME "KSCOMComponentInit"

typedef int KSCOMSTDMETHOD TKSCOMComponentInitFunction(void *pvContent);


#define KSCOM_COMPONENT_UNINIT_FUNCTION_NAME "KSCOMComponentUnInit"

typedef int KSCOMSTDMETHOD TKSCOMComponentUnInitFunction(void *pvContent);


// =========================================================================
// *               Kingsoft COM API Export Function Table                  *
// =========================================================================
extern "C"
{
const int nKSCOMExportFunctionNumber = 7;
extern void *KSCOMExportFunctionTable[nKSCOMExportFunctionNumber];
}

// =========================================================================
// *                  Kingsoft COM API Data Struct                         *
// =========================================================================
typedef enum _TKSCOM_REGISTER_CLASS_OBJECT_MODE
{
    REGISTER_COMPONENT_UNKNOWN  = 0,
    REGISTER_COMPONENT_FILENAME = 1,
    REGISTER_COMPONENT_BUFFER   = 2
} TKSCOM_REGISTER_CLASS_OBJECT_MODE;


// =========================================================================
// *               Kingsoft COM API public implement                       *
// =========================================================================
extern "C"
{

KSCOMRESULT KSCOMSTDMETHOD 
KSCoInitialize(void);

KSCOMRESULT KSCOMSTDMETHOD 
KSCoUnInitialize(void);

KSCOMRESULT KSCOMSTDMETHOD 
KSCoCreateInstance
(
    const UUID &cruuidCLSID, 
    IUnknown   *pUnkOuter, 
    const UUID &cruuidIID, 
    void  **ppvObject
);

KSCOMRESULT KSCOMSTDMETHOD 
KSCoGetClassObject
(
    const UUID &cruuidCLSID, 
    const UUID &cruuidIID, 
    void  **ppvClassObject
);

KSCOMRESULT KSCOMSTDMETHOD 
KSCoRegisterClassObject
(
    const UUID &cruuidCLSID, 
    const UUID &cruuidIID, 
    IUnknown *pUnkInterface, 
    KSCOMHANDLE *phRegister
);

KSCOMRESULT KSCOMSTDMETHOD
KSCoUnRegisterClassObject
(
    KSCOMHANDLE hRegister
);


KSCOMRESULT KSCOMSTDMETHOD 
KSCoRegisterComponent
(
    const UUID &cruuidCLSID, 
    const UUID &cruuidIID, 
    TKSCOM_REGISTER_CLASS_OBJECT_MODE RegisterMode,
    int nClassObjectSize,
    void *pvClassObject,
    KSCOMHANDLE *phRegister
);

KSCOMRESULT KSCOMSTDMETHOD 
KSCoUnRegisterComponent
(
    KSCOMHANDLE hRegister
);

KSCOMRESULT KSCOMSTDMETHOD
KSCoFreeUnusedLibraries(void);

// =========================================================================
// Function for Running Object Table Object
// =========================================================================
KSCOMRESULT KSCOMSTDMETHOD
KSGetActiveObject
(
    const UUID &cruuidCLSID,
    IUnknown **ppUnkActiveObject
);

KSCOMRESULT KSCOMSTDMETHOD
KSRegisterActiveObject
(
    IUnknown *pUnkActiveObject,             
    const UUID &cruuidCLSID,
    KSCOMHANDLE *phRegister
);

KSCOMRESULT KSCOMSTDMETHOD
KSUnRegisterActiveObject
(
    KSCOMHANDLE hRegister
);

}   // extern "C"




#endif // _KSCOM_DEF_H_