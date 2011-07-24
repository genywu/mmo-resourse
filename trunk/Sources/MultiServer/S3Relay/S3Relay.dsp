# Microsoft Developer Studio Project File - Name="S3Relay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=S3Relay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "S3Relay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S3Relay.mak" CFG="S3Relay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S3Relay - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "S3Relay - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SwordOnline/Sources/MultiServer/S3Relay", BSSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "S3Relay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib lualibdll.lib /nologo /subsystem:windows /map /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\..\bin\client\release\	copy release\S3Relay.exe ..\..\..\..\bin\Client\S3Relay.exe	copy release\S3Relay.exe ..\..\..\..\bin\Client\release\S3Relay.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WORKMODE_SINGLETHREAD" /D "_TESTING" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib lualibdll.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\..\bin\client\debug\	copy debug\S3Relay.exe ..\..\..\..\bin\Client\S3Relay.exe	copy debug\S3Relay.exe ..\..\..\..\bin\Client\debug\S3Relay.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "S3Relay - Win32 Release"
# Name "S3Relay - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChannelMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatServer.cpp
# End Source File
# Begin Source File

SOURCE=.\DBConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\DBTable.cpp
# End Source File
# Begin Source File

SOURCE=.\DealRelay.cpp
# End Source File
# Begin Source File

SOURCE=.\DoScript.cpp
# End Source File
# Begin Source File

SOURCE=.\FriendMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GatewayCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\GatewayClient.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\HeavenLib.cpp
# End Source File
# Begin Source File

SOURCE=.\HostConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\HostServer.cpp
# End Source File
# Begin Source File

SOURCE=.\KThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Lock.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\NetCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\NetClient.cpp
# End Source File
# Begin Source File

SOURCE=.\NetConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\NetServer.cpp
# End Source File
# Begin Source File

SOURCE=.\NetSockDupEx.cpp
# End Source File
# Begin Source File

SOURCE=.\RainbowLib.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayClient.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayServer.cpp
# End Source File
# Begin Source File

SOURCE=.\RootCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\RootClient.cpp
# End Source File
# Begin Source File

SOURCE=.\S3Relay.cpp
# End Source File
# Begin Source File

SOURCE=.\S3Relay.rc
# End Source File
# Begin Source File

SOURCE=.\SockThread.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TongConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\TongServer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChannelMgr.h
# End Source File
# Begin Source File

SOURCE=.\ChatConnect.h
# End Source File
# Begin Source File

SOURCE=.\ChatServer.h
# End Source File
# Begin Source File

SOURCE=.\DBConnect.h
# End Source File
# Begin Source File

SOURCE=.\DBTable.h
# End Source File
# Begin Source File

SOURCE=.\DealRelay.h
# End Source File
# Begin Source File

SOURCE=.\DoScript.h
# End Source File
# Begin Source File

SOURCE=.\FriendMgr.h
# End Source File
# Begin Source File

SOURCE=.\GatewayCenter.h
# End Source File
# Begin Source File

SOURCE=.\GatewayClient.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\HeavenLib.h
# End Source File
# Begin Source File

SOURCE=.\HostConnect.h
# End Source File
# Begin Source File

SOURCE=.\HostServer.h
# End Source File
# Begin Source File

SOURCE=.\KThread.h
# End Source File
# Begin Source File

SOURCE=.\Lock.h
# End Source File
# Begin Source File

SOURCE=.\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\Memory.h
# End Source File
# Begin Source File

SOURCE=.\NetCenter.h
# End Source File
# Begin Source File

SOURCE=.\NetClient.h
# End Source File
# Begin Source File

SOURCE=.\NetConnect.h
# End Source File
# Begin Source File

SOURCE=.\NetServer.h
# End Source File
# Begin Source File

SOURCE=.\NetSockDupEx.h
# End Source File
# Begin Source File

SOURCE=.\RainbowLib.h
# End Source File
# Begin Source File

SOURCE=.\RelayCenter.h
# End Source File
# Begin Source File

SOURCE=.\RelayClient.h
# End Source File
# Begin Source File

SOURCE=.\RelayConnect.h
# End Source File
# Begin Source File

SOURCE=.\RelayServer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RootCenter.h
# End Source File
# Begin Source File

SOURCE=.\RootClient.h
# End Source File
# Begin Source File

SOURCE=.\S3Relay.h
# End Source File
# Begin Source File

SOURCE=.\SockThread.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TongConnect.h
# End Source File
# Begin Source File

SOURCE=.\TongServer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\S3Relay.ico
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Library"

# PROP Default_Filter ""
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\debug\engine.lib

!IF  "$(CFG)" == "S3Relay - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\release\engine.lib

!IF  "$(CFG)" == "S3Relay - Win32 Release"

!ELSEIF  "$(CFG)" == "S3Relay - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\libdb41s.lib
# End Source File
# End Group
# Begin Group "°ï»á"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KTongControl.cpp
# End Source File
# Begin Source File

SOURCE=.\KTongControl.h
# End Source File
# Begin Source File

SOURCE=.\KTongSet.cpp
# End Source File
# Begin Source File

SOURCE=.\KTongSet.h
# End Source File
# Begin Source File

SOURCE=.\TONGDB.CPP
# End Source File
# Begin Source File

SOURCE=.\TONGDB.H
# End Source File
# End Group
# Begin Group "ProtocolHeaderFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Headers\KRelayProtocol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Headers\KTongProtocol.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
