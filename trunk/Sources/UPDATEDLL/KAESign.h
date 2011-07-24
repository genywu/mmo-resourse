/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Jan 04 16:04:46 2003
 */
/* Compiler settings for ..\..\PUBLISH\KAESIGN.IDL:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __KAESIGN_h__
#define __KAESIGN_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IKAESign_FWD_DEFINED__
#define __IKAESign_FWD_DEFINED__
typedef interface IKAESign IKAESign;
#endif 	/* __IKAESign_FWD_DEFINED__ */


#ifndef __KAESign_FWD_DEFINED__
#define __KAESign_FWD_DEFINED__

#ifdef __cplusplus
typedef class KAESign KAESign;
#else
typedef struct KAESign KAESign;
#endif /* __cplusplus */

#endif 	/* __KAESign_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_KAESIGN_0000 */
/* [local] */ 

const char g_cszKAECopyRight[] = "Kingsoft AntiVirus Database.   Copyright (c) 1999, 2001 Kingsoft AntiVirus.\x1A";

typedef struct  tagKAE_KAVSIGNDATA_ITEM
    {
    unsigned int uPos;
    unsigned int uLen;
    }	KAE_KAVSIGNDATA_ITEM;

#define defMAXSIGNDATAITEM           0x20
#define defMZSCANBUFDATAITEM         0
#define defMZSCANBUFVIRNAMEITEM      1
#define defMACROSCRIPTDATAITEM       2
#define defMACROSCRIPTVIRNAMEITEM    3
#define defVBSSCRIPTDATAITEM         4
#define defVBSSCRIPTVIRNAMEITEM      5
#define defPESCRIPTDATAITEM          6
#define defPESCRIPTVIRNAMEITEM       7
#define defMZCLEANDATAITEM           8
#define defMZCLEANMEMIMAGE           9
#define defDRWEBVIRUSBASEITEM        10
#define defCOMSCANBUF2DATAITEM          11
#define defCOMSCANPOLYBUF2DATAITEM      12
#define defMZSCANBUF2DATAITEM           13
#define defMZSCANPOLYBUF2DATAITEM       14
#define defPESCANBUF2DATAITEM           15
#define defPESCANPOLYBUF2DATAITEM       16
#define defMACROSCANBUF2DATAITEM        17
#define defMACROSCANPOLYBUF2DATAITEM    18
#define defTEXTSCANBUF2DATAITEM       19
#define defTEXTSCANPOLYBUF2DATAITEM   20

#define defDAILYSCANPOLYBUF2DATAITEM    21
#define defNUMOFSIGNDATA                22

#define defKVSIGN_PACK_NORAMAL              0
#define defKVSIGN_PACK_APLIB                1
#define defKVSIGN_PACK_NRV2B                2
#define defKVSIGN_PACK_NRV2D                3
#define defKVSIGN_ENCRYPT_NORMAL            0
#define defKVSIGN_ENCRYPT_RAND              1
typedef struct  tagKAE_KVSIGN_HEADER
    {
    char szCopyRight[ 80 ];
    unsigned int uVersion;
    unsigned int uDateStamp;
    unsigned int uProcessVirusNum;
    unsigned short wPackMethod;
    unsigned short wEncryptMethod;
    unsigned int uCheckSum;
    unsigned int uDataAreaEntry;
    unsigned int uDataAreaLen;
    unsigned int uSignDataItemCount;
    KAE_KAVSIGNDATA_ITEM SignDataItem[ 32 ];
    }	KAE_KVSIGN_HEADER;



extern RPC_IF_HANDLE __MIDL_itf_KAESIGN_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_KAESIGN_0000_v0_0_s_ifspec;

#ifndef __IKAESign_INTERFACE_DEFINED__
#define __IKAESign_INTERFACE_DEFINED__

/* interface IKAESign */
/* [helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_IKAESign;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("65CD0BBB-6FF0-4ad8-892D-C32E640CA6AD")
    IKAESign : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitSignBuffer( 
            /* [in] */ int nBufSize,
            /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSignBuffer( 
            /* [in] */ int nBufSize,
            /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnInitSignBuffer( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitSignFileName( 
            /* [in] */ int nFileNameSize,
            /* [size_is][in] */ const char __RPC_FAR *pcszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSignFileName( 
            /* [in] */ int nFileNameSize,
            /* [size_is][in] */ const char __RPC_FAR *pcszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnInitSignFileName( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSignVersion( 
            /* [out] */ int __RPC_FAR *pnSignVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVirusCount( 
            /* [out] */ int __RPC_FAR *pnVirusCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKAESignVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IKAESign __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IKAESign __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IKAESign __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitSignBuffer )( 
            IKAESign __RPC_FAR * This,
            /* [in] */ int nBufSize,
            /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddSignBuffer )( 
            IKAESign __RPC_FAR * This,
            /* [in] */ int nBufSize,
            /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnInitSignBuffer )( 
            IKAESign __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitSignFileName )( 
            IKAESign __RPC_FAR * This,
            /* [in] */ int nFileNameSize,
            /* [size_is][in] */ const char __RPC_FAR *pcszFileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddSignFileName )( 
            IKAESign __RPC_FAR * This,
            /* [in] */ int nFileNameSize,
            /* [size_is][in] */ const char __RPC_FAR *pcszFileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnInitSignFileName )( 
            IKAESign __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSignVersion )( 
            IKAESign __RPC_FAR * This,
            /* [out] */ int __RPC_FAR *pnSignVersion);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVirusCount )( 
            IKAESign __RPC_FAR * This,
            /* [out] */ int __RPC_FAR *pnVirusCount);
        
        END_INTERFACE
    } IKAESignVtbl;

    interface IKAESign
    {
        CONST_VTBL struct IKAESignVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKAESign_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IKAESign_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IKAESign_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IKAESign_InitSignBuffer(This,nBufSize,pcbyBuf)	\
    (This)->lpVtbl -> InitSignBuffer(This,nBufSize,pcbyBuf)

#define IKAESign_AddSignBuffer(This,nBufSize,pcbyBuf)	\
    (This)->lpVtbl -> AddSignBuffer(This,nBufSize,pcbyBuf)

#define IKAESign_UnInitSignBuffer(This)	\
    (This)->lpVtbl -> UnInitSignBuffer(This)

#define IKAESign_InitSignFileName(This,nFileNameSize,pcszFileName)	\
    (This)->lpVtbl -> InitSignFileName(This,nFileNameSize,pcszFileName)

#define IKAESign_AddSignFileName(This,nFileNameSize,pcszFileName)	\
    (This)->lpVtbl -> AddSignFileName(This,nFileNameSize,pcszFileName)

#define IKAESign_UnInitSignFileName(This)	\
    (This)->lpVtbl -> UnInitSignFileName(This)

#define IKAESign_GetSignVersion(This,pnSignVersion)	\
    (This)->lpVtbl -> GetSignVersion(This,pnSignVersion)

#define IKAESign_GetVirusCount(This,pnVirusCount)	\
    (This)->lpVtbl -> GetVirusCount(This,pnVirusCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IKAESign_InitSignBuffer_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [in] */ int nBufSize,
    /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf);


void __RPC_STUB IKAESign_InitSignBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_AddSignBuffer_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [in] */ int nBufSize,
    /* [size_is][in] */ const byte __RPC_FAR *pcbyBuf);


void __RPC_STUB IKAESign_AddSignBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_UnInitSignBuffer_Proxy( 
    IKAESign __RPC_FAR * This);


void __RPC_STUB IKAESign_UnInitSignBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_InitSignFileName_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [in] */ int nFileNameSize,
    /* [size_is][in] */ const char __RPC_FAR *pcszFileName);


void __RPC_STUB IKAESign_InitSignFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_AddSignFileName_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [in] */ int nFileNameSize,
    /* [size_is][in] */ const char __RPC_FAR *pcszFileName);


void __RPC_STUB IKAESign_AddSignFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_UnInitSignFileName_Proxy( 
    IKAESign __RPC_FAR * This);


void __RPC_STUB IKAESign_UnInitSignFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_GetSignVersion_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [out] */ int __RPC_FAR *pnSignVersion);


void __RPC_STUB IKAESign_GetSignVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IKAESign_GetVirusCount_Proxy( 
    IKAESign __RPC_FAR * This,
    /* [out] */ int __RPC_FAR *pnVirusCount);


void __RPC_STUB IKAESign_GetVirusCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IKAESign_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_KAESIGN_0007 */
/* [local] */ 

EXTERN_C const CLSID CLSID_KAESign;


extern RPC_IF_HANDLE __MIDL_itf_KAESIGN_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_KAESIGN_0007_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
