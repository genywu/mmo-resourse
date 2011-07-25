

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sat Jun 05 14:11:38 2010
 */
/* Compiler settings for .\RrMMOSimpleClient.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __RrMMOSimpleClient_h_h__
#define __RrMMOSimpleClient_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRrMMOSimpleClient_FWD_DEFINED__
#define __IRrMMOSimpleClient_FWD_DEFINED__
typedef interface IRrMMOSimpleClient IRrMMOSimpleClient;
#endif 	/* __IRrMMOSimpleClient_FWD_DEFINED__ */


#ifndef __CRrMMOSimpleClientDoc_FWD_DEFINED__
#define __CRrMMOSimpleClientDoc_FWD_DEFINED__

#ifdef __cplusplus
typedef class CRrMMOSimpleClientDoc CRrMMOSimpleClientDoc;
#else
typedef struct CRrMMOSimpleClientDoc CRrMMOSimpleClientDoc;
#endif /* __cplusplus */

#endif 	/* __CRrMMOSimpleClientDoc_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __RrMMOSimpleClient_LIBRARY_DEFINED__
#define __RrMMOSimpleClient_LIBRARY_DEFINED__

/* library RrMMOSimpleClient */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_RrMMOSimpleClient;

#ifndef __IRrMMOSimpleClient_DISPINTERFACE_DEFINED__
#define __IRrMMOSimpleClient_DISPINTERFACE_DEFINED__

/* dispinterface IRrMMOSimpleClient */
/* [uuid] */ 


EXTERN_C const IID DIID_IRrMMOSimpleClient;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("16F8DBF5-6632-43DD-9834-F96AD0BDC911")
    IRrMMOSimpleClient : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IRrMMOSimpleClientVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRrMMOSimpleClient * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRrMMOSimpleClient * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRrMMOSimpleClient * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRrMMOSimpleClient * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRrMMOSimpleClient * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRrMMOSimpleClient * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRrMMOSimpleClient * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IRrMMOSimpleClientVtbl;

    interface IRrMMOSimpleClient
    {
        CONST_VTBL struct IRrMMOSimpleClientVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRrMMOSimpleClient_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRrMMOSimpleClient_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRrMMOSimpleClient_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRrMMOSimpleClient_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IRrMMOSimpleClient_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IRrMMOSimpleClient_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IRrMMOSimpleClient_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IRrMMOSimpleClient_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CRrMMOSimpleClientDoc;

#ifdef __cplusplus

class DECLSPEC_UUID("54913426-1B7F-4EFB-A4C9-0A00628AD316")
CRrMMOSimpleClientDoc;
#endif
#endif /* __RrMMOSimpleClient_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


