# Microsoft Developer Studio Project File - Name="map2difPlus" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=map2difPlus - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "map2difPlus.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "map2difPlus.mak" CFG="map2difPlus - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "map2difPlus - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "map2difPlus - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "map2difPlus - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../tools/out.VC6.RELEASE"
# PROP Intermediate_Dir "../tools/out.VC6.RELEASE/map2difPlus"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /GR /GX /O2 /I "../lib/xiph/include" /I "../lib/directx8" /I "../lib/vorbis/include" /I "../lib/zlib" /I "../lib/lungif" /I "../lib/lpng" /I "../lib/ljpeg" /I "../lib/openal/win32" /I "../engine" /I "../tools" /D "UNICODE" /YX /FD /c /Tp
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 engine.lib WINMM.LIB WSOCK32.LIB ljpeg.lib lpng.lib lungif.lib zlib.lib vfw32.lib ogg_static.lib theora_static.lib vorbis_static.lib Imm32.lib User32.lib Gdi32.lib Advapi32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc" /out:"../tools/map2dif_plus.exe" /libpath:"../lib/xiph/win32" /libpath:"../lib/out.VC6.RELEASE"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "map2difPlus - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../tools/out.VC6.DEBUG"
# PROP Intermediate_Dir "../tools/out.VC6.DEBUG/map2difPlus"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /Gm /GR /GX /ZI /Od /I "../lib/xiph/include" /I "../lib/SDL/include" /I "../lib/directx8" /I "../lib/vorbis/include" /I "../lib/zlib" /I "../lib/lungif" /I "../lib/lpng" /I "../lib/ljpeg" /I "../lib/openal/win32" /I "../engine" /I "../tools" /D "UNICODE" /D "TORQUE_DEBUG" /YX /FD /GZ /c /Tp
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 engine_DEBUG.lib WINMM.LIB WSOCK32.LIB ljpeg_DEBUG.lib lpng_DEBUG.lib lungif_DEBUG.lib zlib_DEBUG.lib vfw32.lib ogg_static.lib theora_static.lib vorbis_static.lib Imm32.lib User32.lib Gdi32.lib Advapi32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBC" /nodefaultlib:"LIBCD" /out:"../tools/map2dif_plus_DEBUG.exe" /pdbtype:sept /libpath:"../lib/xiph/win32" /libpath:"../lib/SDL/win32" /libpath:"../lib/out.VC6.DEBUG"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "map2difPlus - Win32 Release"
# Name "map2difPlus - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc"
# Begin Source File

SOURCE="..\tools\map2difPlus\bspNode.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\convert.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\createLightmaps.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\csgBrush.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editFloorPlanRes.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editGeometry.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editInteriorRes.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\entityTypes.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\exportGeometry.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\lmapPacker.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\main.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\morianUtil.cc"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\navGraph.cc"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\tools\map2difPlus\bspNode.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\convert.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\createLightmaps.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\csgBrush.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editFloorPlanRes.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editGeometry.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\editInteriorRes.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\entityTypes.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\lmapPacker.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\morianBasics.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\morianGame.h"
# End Source File
# Begin Source File

SOURCE="..\tools\map2difPlus\morianUtil.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
