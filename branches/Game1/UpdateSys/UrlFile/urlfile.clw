; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSelectDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "urlfile.h"

ClassCount=6
Class1=CUrlfileApp
Class2=CUrlFileDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_URLFILE_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_SETUP_DLG
Class4=CSetupDlg
Class5=CSetDlg
Resource4=IDD_SELECT
Class6=CSelectDlg
Resource5=IDD_ABOUTBOX
Resource6=IDD_LOGODLG

[CLS:CUrlfileApp]
Type=0
HeaderFile=urlfile.h
ImplementationFile=urlfile.cpp
Filter=N

[CLS:CAboutDlg]
Type=0
HeaderFile=urlfileDlg.h
ImplementationFile=urlfileDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CSetupDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_URLFILE_DIALOG]
Type=1
Class=CUrlFileDlg
ControlCount=15
Control1=IDC_URL,edit,1342244992
Control2=IDOK,button,1342242817
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PROGRESS1,msctls_progress32,1350565889
Control5=IDC_FILELIST,SysListView32,1350631425
Control6=IDC_ANIMATE1,SysAnimate32,1342242818
Control7=IDC_TOTELPROGRESS,msctls_progress32,1350565889
Control8=IDC_STATUS,static,1342308352
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_TIMELEFT,static,1342308352
Control14=IDC_TRANSFER_RATE,static,1342308352
Control15=IDC_STATIC,static,1342308352

[CLS:CUrlFileDlg]
Type=0
HeaderFile=urlfiledlg.h
ImplementationFile=urlfiledlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CUrlFileDlg

[DLG:IDD_SETUP_DLG]
Type=1
Class=CSetDlg
ControlCount=5
Control1=IDOK,button,1342242816
Control2=IDC_INPUT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_NAME,edit,1350631552
Control5=IDC_STATIC,static,1342308352

[CLS:CSetupDlg]
Type=0
HeaderFile=SetupDlg.h
ImplementationFile=SetupDlg.cpp
BaseClass=CDialog
Filter=D

[CLS:CSetDlg]
Type=0
HeaderFile=SetDlg.h
ImplementationFile=SetDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSetDlg

[DLG:IDD_SELECT]
Type=1
Class=CSelectDlg
ControlCount=6
Control1=IDOK,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDC_SERVERLIST,SysListView32,1350631437
Control4=IDC_TEST,button,1342242816
Control5=IDC_ADDSERVER,button,1342242816
Control6=IDC_DELSERVER,button,1342242816

[CLS:CSelectDlg]
Type=0
HeaderFile=SelectDlg.h
ImplementationFile=SelectDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSelectDlg

[DLG:IDD_LOGODLG]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342177294

