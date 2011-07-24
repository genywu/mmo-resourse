////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   UserCheck.h.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-17  10:37:18
//  Comment     :   KAVuser-net check declaration header file
//  
////////////////////////////////////////////////////////////////////////////////

#ifndef _USERCHECK_H
#define _USERCHECK_H    1


#define defCHECK_CURRENT_VERSION            2
#define defCHECK_CURRENT_USERFLAG           "KAV2000 User Info"
#define defCHECK_CURRENT_SERVERFLAG         "KAV2000 Server Response"


#pragma pack(push, 1)

typedef struct tagUSER_INFO {
    char    szFlag[30];             /* KAV2000 User Info + 0x0 */
    int     nStructVersion;         /* 2 */
    int     nWaitForResult;         /* 1 -> Server will return result 0 -> no result */
    char    szDateTime[20];         /* 10/24/2000 22:39:08 + 0x0 */
    
    // valid when StructVersion > 1
    char    szSerialNumber[28];     // xxxxxx-xxxxxx-xxxxxx-xxxxxx + 0x0
    
} USER_INFO;


typedef struct tagSERVER_INFO{
    char    szFlag[24];             /* KAV2000 Server Response + 0x0 */
    
    int     nStructVersion;         /* 1 */
    int     nNumber;                /* number */
    
} SERVER_INFO;

#pragma pack(pop)


void GetUserInfo(USER_INFO& pUserInfo, BOOL bWaitForResult);


#endif  // #ifndef _USERCHECK_H
