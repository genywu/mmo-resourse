///////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSCOMError.h
//  Creater     :   
//  Date        :   2000-6-20 19:11:40
//  Comment     :   error code definitions for the AntiVirus functions 
//
//////////////////////////////////////////////////////////////////////////////////////


#ifndef KSCOMERROR_H
#define KSCOMERROR_H

//  usable Error Value Area
//#include <winerror.h>

// =========================================================================
// *                      Kingsoft COM API Error Code                      *
// =========================================================================

typedef long KSCOMRESULT;           // typedef HRESULT KSCOMRESULT;

#ifndef SUCCEEDED
//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
#define SUCCEEDED(Status) ((KSCOMRESULT)(Status) >= 0)

#endif // !SUCCEEDED

#ifndef FAILED
//
// and the inverse
//
#define FAILED(Status) ((KSCOMRESULT)(Status) < 0)

#endif // !FAILED

//
// Error Codes get from WinError.h(Win32)
//
#define KSCOM_E_UNEXPECTED       ((KSCOMRESULT)0x8000FFFF)
#define KSCOM_E_NOTIMPL          ((KSCOMRESULT)0x80004001)
#define KSCOM_E_OUTOFMEMORY      ((KSCOMRESULT)0x8007000E)
#define KSCOM_E_INVALIDARG       ((KSCOMRESULT)0x80070057)
#define KSCOM_E_NOINTERFACE      ((KSCOMRESULT)0x80004002)
#define KSCOM_E_POINTER          ((KSCOMRESULT)0x80004003)
#define KSCOM_E_HANDLE	         ((KSCOMRESULT)0x80070006)
#define KSCOM_E_ABORT	         ((KSCOMRESULT)0x80004004)
#define KSCOM_E_FAIL             ((KSCOMRESULT)0x80004005)
#define KSCOM_E_ACCESSDENIED     ((KSCOMRESULT)0x80070005)
#define KSCOM_E_STG_FILENOTFOUND ((KSCOMRESULT)0x80030002)

#define KSCOM_CLASS_E_NOAGGREGATION         ((KSCOMRESULT)0x80040110L)
#define KSCOM_CLASS_E_CLASSNOTAVAILABLE     ((KSCOMRESULT)0x80040111L)


//
// Success codes get from WinError.h(Win32)
//
#define KSCOM_S_OK                  ((KSCOMRESULT)0x00000000L)
#define KSCOM_S_FALSE               ((KSCOMRESULT)0x00000001L)

#define KSCOM_S_SKIP                ((KSCOMRESULT)0x01960001L)
#define KSCOM_S_CLEAN_OK            ((KSCOMRESULT)0x01960002L)
#define KSCOM_S_CLEAN_DELETE        ((KSCOMRESULT)0x01960003L)
#define KSCOM_S_HAS_VIRUS           ((KSCOMRESULT)0x01960004L)


//  81 90 ~ 99
#define KSCOM_E_USER_BREAK              ((KSCOMRESULT)0x81960001L)
#define KSCOM_E_FILE_NOT_FOUND          ((KSCOMRESULT)0x81960002L)
#define KSCOM_E_FILE_OPEN               ((KSCOMRESULT)0x81960003L)
#define KSCOM_E_GET_FILE_TYPE           ((KSCOMRESULT)0x81960004L)
#define KSCOM_E_GET_VIRUS_NAME          ((KSCOMRESULT)0x81960005L)
#define KSCOM_E_FILE_ENCRYPT            ((KSCOMRESULT)0x81960006L)
#define KSCOM_E_EMPTY                   ((KSCOMRESULT)0x81960007L)
#define KSCOM_E_NEW_INIT_SCAN_HANDLE    ((KSCOMRESULT)0x81960008L)
#define KSCOM_E_FILE_READ_ERROR         ((KSCOMRESULT)0x81960009L)
#define KSCOM_E_FILE_WRITE_ERROR        ((KSCOMRESULT)0x8196000aL)
#define KSCOM_E_FILE_CRACKED            ((KSCOMRESULT)0x8196000bL)
#define KSCOM_E_OUTOFBUFFER             ((KSCOMRESULT)0x8196000cL)

#endif  //  KSCOMERROR_H


