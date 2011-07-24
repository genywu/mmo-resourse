# Microsoft Developer Studio Project File - Name="Heaven" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Heaven - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Heaven.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Heaven.mak" CFG="Heaven - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Heaven - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Heaven - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SwordOnline/Sources/MultiServer/Heaven", VTNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Heaven - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\bin\client\release\	copy release\heaven.dll ..\..\..\..\bin\client\heaven.dll	copy release\heaven.dll ..\..\..\..\bin\client\release\heaven.dll	copy release\heaven.pdb ..\..\..\..\bin\client\release\heaven.pdb	copy release\heaven.map ..\..\..\..\bin\client\release\heaven.map	md ..\..\..\bin\server\release\	copy release\heaven.dll ..\..\..\..\bin\server\heaven.dll	copy release\heaven.dll ..\..\..\..\bin\server\release\heaven.dll	copy release\heaven.pdb ..\..\..\..\bin\server\release\heaven.pdb	copy release\heaven.map ..\..\..\..\bin\server\release\heaven.map
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HEAVEN_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\bin\client\debug\	copy debug\heaven.dll ..\..\..\..\bin\client\heaven.dll	copy debug\heaven.dll ..\..\..\..\bin\client\debug\heaven.dll	md ..\..\..\bin\server\debug\	copy debug\heaven.dll ..\..\..\..\bin\server\heaven.dll	copy debug\heaven.dll ..\..\..\..\bin\server\debug\heaven.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Heaven - Win32 Release"
# Name "Heaven - Win32 Debug"
# Begin Group "DLL Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Heaven.cpp
# End Source File
# Begin Source File

SOURCE=.\IServer.cpp
# End Source File
# End Group
# Begin Group "DLL Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Heaven.def
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "IOCPServer"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ServerStage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ServerStage.h
# End Source File
# End Group
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\HeavenInterface.h
# End Source File
# Begin Source File

SOURCE=.\Interface\IServer.h
# End Source File
# End Group
# Begin Group "Lib"

# PROP Default_Filter ""
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\debug\common.lib

!IF  "$(CFG)" == "Heaven - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Lib\release\common.lib

!IF  "$(CFG)" == "Heaven - Win32 Release"

!ELSEIF  "$(CFG)" == "Heaven - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Interface.txt
# End Source File
# End Target
# End Project
