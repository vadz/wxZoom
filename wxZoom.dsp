# Microsoft Developer Studio Project File - Name="wxZoomDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxZoomDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxZoom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxZoom.mak" CFG="wxZoomDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxZoomDll - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxZoomDll - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxZoomDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDll"
# PROP BASE Intermediate_Dir "ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDll"
# PROP Intermediate_Dir "ReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS"  /D WXUSINGDLL /MD /YX /FD /c
# ADD CPP /nologo /W4 /O1 /I "$(wx)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS"  /D WXUSINGDLL /MD /D _WIN32 /D __WIN32__ /D WINVER=0x400 /D __WINDOWS__ /D __WXMSW__ /D __WIN95__ /D __WIN32__ /D _MT /D wxUSE_GUI=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "$(wx)\include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "$(wx)\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib $(wx)\ReleaseDll\wxmsw221.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib $(wx)\ReleaseDll\wxmsw221.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "wxZoomDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDll"
# PROP BASE Intermediate_Dir "DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDll"
# PROP Intermediate_Dir "DebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"  /D WXUSINGDLL /MDd /D "__WXDEBUG__" /D "WXDEBUG=1" /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "$(wx)\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"  /D WXUSINGDLL /MDd /D "__WXDEBUG__" /D "WXDEBUG=1" /D _WIN32 /D __WIN32__ /D WINVER=0x400 /D __WINDOWS__ /D __WXMSW__ /D __WIN95__ /D __WIN32__ /D _MT /D wxUSE_GUI=1 /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "$(wx)\include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "$(wx)\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib $(wx)\DebugDll\wxmsw221d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib $(wx)\DebugDll\wxmsw221d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "wxZoomDll - Win32 Release"
# Name "wxZoomDll - Win32 Debug"
# Begin Source File

SOURCE=.\src\wxZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\wxZoom.rc
# End Source File
# End Target
# End Project
    
