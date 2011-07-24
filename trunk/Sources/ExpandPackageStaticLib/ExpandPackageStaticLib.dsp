# Microsoft Developer Studio Project File - Name="ExpandPackageStaticLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ExpandPackageStaticLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ExpandPackageStaticLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ExpandPackageStaticLib.mak" CFG="ExpandPackageStaticLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ExpandPackageStaticLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ExpandPackageStaticLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SwordOnline/Sources/ExpandPackageStaticLib", XEVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ExpandPackageStaticLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\ExpandPackageStaticLib.lib ..\..\lib\Release\ExpandPackageStaticLib.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ExpandPackageStaticLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\ExpandPackageStaticLib.lib ..\..\lib\debug\ExpandPackageStaticLib.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ExpandPackageStaticLib - Win32 Release"
# Name "ExpandPackageStaticLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ExpandPackage2.0\BlockSource.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\DataPointFromStream.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\DataSource.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\ExpandPackage.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\Interval.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\Package.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\PackageFileHead.cpp
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\ExpandPackage2.0\BlockSource.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\DataPointFromStream.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\DataSource.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\Interval.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\Package.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\PackageFileHead.h
# End Source File
# Begin Source File

SOURCE=..\ExpandPackage2.0\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
