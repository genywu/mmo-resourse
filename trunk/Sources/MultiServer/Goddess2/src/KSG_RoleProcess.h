//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_RoleProcess.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-15 7:45:32
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KSG_ROLEPROCESS_H
#define _KSG_ROLEPROCESS_H  1

#define WM_ADD_CLIENT		WM_USER + 0x200
#define WM_DEL_CLIENT		WM_USER + 0x300

int KSG_RoleProcess_Init(int nPort, HWND hMainWnd);
int KSG_RoleProcess_UnInit();

int KSG_RoleProcess_Start();



#endif  // _KSG_ROLEPROCESS_H