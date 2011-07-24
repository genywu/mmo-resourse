# Microsoft Developer Studio Project File - Name="Goddess2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Goddess2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Goddess2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Goddess2.mak" CFG="Goddess2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Goddess2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Goddess2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SwordOnline/Sources/MultiServer/Goddess2", GHWAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Goddess2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\..\bin\client\release\	copy release\Goddess2.exe ..\..\..\..\bin\Client\Goddess2.exe	copy release\Goddess2.exe ..\..\..\..\bin\Client\release\Goddess2.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Goddess2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\..\bin\client\debug\	copy debug\Goddess2.exe ..\..\..\..\bin\Client\Goddess2.exe	copy debug\Goddess2.exe ..\..\..\..\bin\Client\debug\Goddess2.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Goddess2 - Win32 Release"
# Name "Goddess2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Goddess2.cpp
# End Source File
# Begin Source File

SOURCE=.\Goddess2.rc
# End Source File
# Begin Source File

SOURCE=.\Goddess2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KSG_RoleProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Goddess2.h
# End Source File
# Begin Source File

SOURCE=.\Goddess2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\src\KSG_RoleProcess.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\giddess2.ico
# End Source File
# Begin Source File

SOURCE=.\res\Goddess2.ico
# End Source File
# Begin Source File

SOURCE=.\res\Goddess2.rc2
# End Source File
# End Group
# Begin Group "Role"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\db.h
# End Source File
# Begin Source File

SOURCE=.\src\DBBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBBackup.h
# End Source File
# Begin Source File

SOURCE=.\src\DBDumpLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBDumpLoad.h
# End Source File
# Begin Source File

SOURCE=.\src\DBTable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DBTable.h
# End Source File
# Begin Source File

SOURCE=.\src\FilterTextLib.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilterTextLib.h
# End Source File
# Begin Source File

SOURCE=.\src\IDBRoleServer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IDBRoleServer.h
# End Source File
# Begin Source File

SOURCE=.\src\RoleNameFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RoleNameFilter.h
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\debug\common.lib
# End Source File
# End Group
# Begin Group "release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\release\common.lib
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\src\libdb41s.lib
# End Source File
# End Target
# End Project
