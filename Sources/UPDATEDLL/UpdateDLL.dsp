# Microsoft Developer Studio Project File - Name="UpdateDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=UpdateDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UpdateDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UpdateDLL.mak" CFG="UpdateDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UpdateDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UpdateDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/KAV20/Platform/UpdateDLL", GBYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./" /I "./src/" /D "_XP_WIN32_MFC_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 KavPublicR.lib ApLib.lib Ws2_32.lib Version.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBCMT.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\UpdateDLL.dll ..\..\..\bin\Client\UpdateDLL.dll	copy release\UpdateDLL.dll ..\..\..\bin\Client\release\UpdateDLL.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "./src" /D "_XP_WIN32_MFC_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 KavPublicD.lib ApLib.lib Ws2_32.lib Version.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\UpdateDLL.dll ..\..\..\bin\Client\UpdateDLL.dll	copy debug\UpdateDLL.dll ..\..\..\bin\Client\debug\UpdateDLL.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "UpdateDLL - Win32 Release"
# Name "UpdateDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\MsgWnd.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.def
# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.rc
# End Source File
# Begin Source File

SOURCE=.\src\UpdateDLLImplement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UpdateExport.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\MsgWnd.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\src\SourceDef.h
# End Source File
# Begin Source File

SOURCE=..\..\Publish\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdateExport.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\UpdateDLL.rc2
# End Source File
# End Group
# Begin Group "Public"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\bufsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bufsocket.h
# End Source File
# Begin Source File

SOURCE=.\src\BusyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BusyThread.h
# End Source File
# Begin Source File

SOURCE=.\src\CheckThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CheckThread.h
# End Source File
# Begin Source File

SOURCE=.\src\CRC32.C
# End Source File
# Begin Source File

SOURCE=.\src\CRC32.h
# End Source File
# Begin Source File

SOURCE=.\src\DataDefine.h
# End Source File
# Begin Source File

SOURCE=.\src\DgmSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DgmSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\downloadfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\downloadfile.h
# End Source File
# Begin Source File

SOURCE=.\src\downnotify.cpp
# End Source File
# Begin Source File

SOURCE=.\src\downnotify.h
# End Source File
# Begin Source File

SOURCE=.\src\ftpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ftpdownload.h
# End Source File
# Begin Source File

SOURCE=.\src\GenKAVMoveProgram.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GenKAVMoveProgram.h
# End Source File
# Begin Source File

SOURCE=.\src\getfilesversion.cpp
# End Source File
# Begin Source File

SOURCE=.\src\getfilesversion.h
# End Source File
# Begin Source File

SOURCE=.\src\getproxysetting.cpp
# End Source File
# Begin Source File

SOURCE=.\src\getproxysetting.h
# End Source File
# Begin Source File

SOURCE=.\GetVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\GetVersion.h
# End Source File
# Begin Source File

SOURCE=.\src\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Global.h
# End Source File
# Begin Source File

SOURCE=.\src\httpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\src\httpdownload.h
# End Source File
# Begin Source File

SOURCE=.\src\InetStruct.cpp
# End Source File
# Begin Source File

SOURCE=.\src\InetStruct.h
# End Source File
# Begin Source File

SOURCE=.\src\IpSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IpSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\KCloseProgramMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KCloseProgramMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\LoadHostList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LoadHostList.h
# End Source File
# Begin Source File

SOURCE=.\src\PingSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PingSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\PingThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PingThread.h
# End Source File
# Begin Source File

SOURCE=.\src\ProcessIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ProcessIndex.h
# End Source File
# Begin Source File

SOURCE=.\src\proxyutility.cpp
# End Source File
# Begin Source File

SOURCE=.\src\proxyutility.h
# End Source File
# Begin Source File

SOURCE=.\src\PublicFun.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PublicFun.h
# End Source File
# Begin Source File

SOURCE=.\src\RawSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RawSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\SaveLog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SaveLog.h
# End Source File
# Begin Source File

SOURCE=.\src\sockspacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sockspacket.h
# End Source File
# Begin Source File

SOURCE=.\src\stmsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stmsocket.h
# End Source File
# Begin Source File

SOURCE=.\src\tcpsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tcpsocket.h
# End Source File
# Begin Source File

SOURCE=.\src\UdpSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UdpSocket.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdateData.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UpdateData.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdatePublic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UpdatePublic.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdateSelf.h
# End Source File
# Begin Source File

SOURCE=.\src\usercheck.cpp
# End Source File
# Begin Source File

SOURCE=.\src\usercheck.h
# End Source File
# Begin Source File

SOURCE=.\src\wndnotify.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wndnotify.h
# End Source File
# End Group
# Begin Group "Lib"

# PROP Default_Filter ""
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Lib\debug\ExpandPackageStaticLib.lib

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Lib\Release\ExpandPackageStaticLib.lib

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
