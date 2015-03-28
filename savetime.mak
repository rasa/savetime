# Microsoft Developer Studio Generated NMAKE File, Based on savetime.dsp
!IF "$(CFG)" == ""
CFG=savetime - Win32 Debug
!MESSAGE No configuration specified. Defaulting to savetime - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "savetime - Win32 Release" && "$(CFG)" != "savetime - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "savetime.mak" CFG="savetime - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "savetime - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "savetime - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "savetime - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\savetime.exe"


CLEAN :
	-@erase "$(INTDIR)\savetime.obj"
	-@erase "$(INTDIR)\savetime.res"
	-@erase "$(INTDIR)\set_time.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wslib.obj"
	-@erase "$(OUTDIR)\savetime.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\savetime.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\savetime.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\savetime.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shared.lib setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\savetime.pdb" /machine:I386 /out:"$(OUTDIR)\savetime.exe" 
LINK32_OBJS= \
	"$(INTDIR)\savetime.obj" \
	"$(INTDIR)\set_time.obj" \
	"$(INTDIR)\wslib.obj" \
	"$(INTDIR)\savetime.res"

"$(OUTDIR)\savetime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "savetime - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\savetime.exe" "$(OUTDIR)\savetime.bsc"


CLEAN :
	-@erase "$(INTDIR)\savetime.obj"
	-@erase "$(INTDIR)\savetime.res"
	-@erase "$(INTDIR)\savetime.sbr"
	-@erase "$(INTDIR)\set_time.obj"
	-@erase "$(INTDIR)\set_time.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wslib.obj"
	-@erase "$(INTDIR)\wslib.sbr"
	-@erase "$(OUTDIR)\savetime.bsc"
	-@erase "$(OUTDIR)\savetime.exe"
	-@erase "$(OUTDIR)\savetime.ilk"
	-@erase "$(OUTDIR)\savetime.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\savetime.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\savetime.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\savetime.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\savetime.sbr" \
	"$(INTDIR)\set_time.sbr" \
	"$(INTDIR)\wslib.sbr"

"$(OUTDIR)\savetime.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=sharedd.lib setargv.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\savetime.pdb" /debug /machine:I386 /out:"$(OUTDIR)\savetime.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\savetime.obj" \
	"$(INTDIR)\set_time.obj" \
	"$(INTDIR)\wslib.obj" \
	"$(INTDIR)\savetime.res"

"$(OUTDIR)\savetime.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("savetime.dep")
!INCLUDE "savetime.dep"
!ELSE 
!MESSAGE Warning: cannot find "savetime.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "savetime - Win32 Release" || "$(CFG)" == "savetime - Win32 Debug"
SOURCE=.\savetime.cpp

!IF  "$(CFG)" == "savetime - Win32 Release"


"$(INTDIR)\savetime.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "savetime - Win32 Debug"


"$(INTDIR)\savetime.obj"	"$(INTDIR)\savetime.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\set_time.cpp

!IF  "$(CFG)" == "savetime - Win32 Release"


"$(INTDIR)\set_time.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "savetime - Win32 Debug"


"$(INTDIR)\set_time.obj"	"$(INTDIR)\set_time.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\wslib.cpp

!IF  "$(CFG)" == "savetime - Win32 Release"


"$(INTDIR)\wslib.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "savetime - Win32 Debug"


"$(INTDIR)\wslib.obj"	"$(INTDIR)\wslib.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\savetime.rc

"$(INTDIR)\savetime.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

