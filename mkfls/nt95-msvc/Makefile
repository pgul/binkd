# Microsoft Developer Studio Generated NMAKE File, Based on binkd.dsp
!IF "$(CFG)" == ""
CFG=binkd - Win32 Release
!MESSAGE No configuration specified. Defaulting to binkd - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "binkd - Win32 Debug" && "$(CFG)" != "binkd - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE CFG="binkd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "binkd - Win32 Debug"   
!MESSAGE "binkd - Win32 Release" (default)
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "binkd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\binkd.exe" "$(OUTDIR)\binkd.bsc"


CLEAN :
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(INTDIR)\https.obj"
	-@erase "$(INTDIR)\BINKD.OBJ"
	-@erase "$(INTDIR)\BINKD.SBR"
	-@erase "$(INTDIR)\binlog.obj"
	-@erase "$(INTDIR)\binlog.sbr"
	-@erase "$(INTDIR)\branch.obj"
	-@erase "$(INTDIR)\branch.sbr"
	-@erase "$(INTDIR)\breaksig.obj"
	-@erase "$(INTDIR)\breaksig.sbr"
	-@erase "$(INTDIR)\BSY.OBJ"
	-@erase "$(INTDIR)\BSY.SBR"
	-@erase "$(INTDIR)\CLIENT.OBJ"
	-@erase "$(INTDIR)\CLIENT.SBR"
	-@erase "$(INTDIR)\dirwin32.obj"
	-@erase "$(INTDIR)\dirwin32.sbr"
	-@erase "$(INTDIR)\exitproc.obj"
	-@erase "$(INTDIR)\exitproc.sbr"
	-@erase "$(INTDIR)\FTNADDR.OBJ"
	-@erase "$(INTDIR)\FTNADDR.SBR"
	-@erase "$(INTDIR)\ftndom.obj"
	-@erase "$(INTDIR)\ftndom.sbr"
	-@erase "$(INTDIR)\ftnnode.obj"
	-@erase "$(INTDIR)\ftnnode.sbr"
	-@erase "$(INTDIR)\FTNQ.OBJ"
	-@erase "$(INTDIR)\FTNQ.SBR"
	-@erase "$(INTDIR)\getfree.obj"
	-@erase "$(INTDIR)\getfree.sbr"
	-@erase "$(INTDIR)\getw.obj"
	-@erase "$(INTDIR)\getw.sbr"
	-@erase "$(INTDIR)\INBOUND.OBJ"
	-@erase "$(INTDIR)\INBOUND.SBR"
	-@erase "$(INTDIR)\iptools.obj"
	-@erase "$(INTDIR)\iptools.sbr"
	-@erase "$(INTDIR)\pmatch.obj"
	-@erase "$(INTDIR)\pmatch.sbr"
	-@erase "$(INTDIR)\prothlp.obj"
	-@erase "$(INTDIR)\prothlp.sbr"
	-@erase "$(INTDIR)\PROTOCOL.OBJ"
	-@erase "$(INTDIR)\PROTOCOL.SBR"
	-@erase "$(INTDIR)\READCFG.OBJ"
	-@erase "$(INTDIR)\READCFG.SBR"
	-@erase "$(INTDIR)\readflo.obj"
	-@erase "$(INTDIR)\readflo.sbr"
	-@erase "$(INTDIR)\run.obj"
	-@erase "$(INTDIR)\run.sbr"
	-@erase "$(INTDIR)\sem.obj"
	-@erase "$(INTDIR)\sem.sbr"
	-@erase "$(INTDIR)\SERVER.OBJ"
	-@erase "$(INTDIR)\SERVER.SBR"
	-@erase "$(INTDIR)\setpttl.obj"
	-@erase "$(INTDIR)\setpttl.sbr"
	-@erase "$(INTDIR)\srif.obj"
	-@erase "$(INTDIR)\srif.sbr"
	-@erase "$(INTDIR)\TCPErr.obj"
	-@erase "$(INTDIR)\TCPErr.sbr"
	-@erase "$(INTDIR)\TOOLS.OBJ"
	-@erase "$(INTDIR)\TOOLS.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WSock.obj"
	-@erase "$(INTDIR)\WSock.sbr"
	-@erase "$(INTDIR)\xalloc.obj"
	-@erase "$(INTDIR)\xalloc.sbr"
	-@erase "$(OUTDIR)\binkd.bsc"
	-@erase "$(OUTDIR)\binkd.exe"
	-@erase "$(OUTDIR)\binkd.ilk"
	-@erase "$(OUTDIR)\binkd.map"
	-@erase "$(OUTDIR)\binkd.pdb"
	-@erase "$(INTDIR)\md5b.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "HTTPS" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "HAVE_THREADS" /D "HAVE_IO_H" /D "VISUALCPP" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\binkd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\binkd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BINKD.SBR" \
	"$(INTDIR)\binlog.sbr" \
	"$(INTDIR)\branch.sbr" \
	"$(INTDIR)\breaksig.sbr" \
	"$(INTDIR)\BSY.SBR" \
	"$(INTDIR)\CLIENT.SBR" \
	"$(INTDIR)\dirwin32.sbr" \
	"$(INTDIR)\exitproc.sbr" \
	"$(INTDIR)\FTNADDR.SBR" \
	"$(INTDIR)\ftndom.sbr" \
	"$(INTDIR)\ftnnode.sbr" \
	"$(INTDIR)\FTNQ.SBR" \
	"$(INTDIR)\getfree.sbr" \
	"$(INTDIR)\getw.sbr" \
	"$(INTDIR)\INBOUND.SBR" \
	"$(INTDIR)\iptools.sbr" \
	"$(INTDIR)\pmatch.sbr" \
	"$(INTDIR)\prothlp.sbr" \
	"$(INTDIR)\PROTOCOL.SBR" \
	"$(INTDIR)\READCFG.SBR" \
	"$(INTDIR)\readflo.sbr" \
	"$(INTDIR)\run.sbr" \
	"$(INTDIR)\sem.sbr" \
	"$(INTDIR)\SERVER.SBR" \
	"$(INTDIR)\setpttl.sbr" \
	"$(INTDIR)\srif.sbr" \
	"$(INTDIR)\TCPErr.sbr" \
	"$(INTDIR)\TOOLS.SBR" \
	"$(INTDIR)\WSock.sbr" \
	"$(INTDIR)\xalloc.sbr"

"$(OUTDIR)\binkd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\binkd.pdb" /map:"$(INTDIR)\binkd.map" /debug /machine:I386 /out:"$(OUTDIR)\binkd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\service.obj" \
	"$(INTDIR)\https.obj" \
	"$(INTDIR)\BINKD.OBJ" \
	"$(INTDIR)\binlog.obj" \
	"$(INTDIR)\branch.obj" \
	"$(INTDIR)\breaksig.obj" \
	"$(INTDIR)\BSY.OBJ" \
	"$(INTDIR)\CLIENT.OBJ" \
	"$(INTDIR)\dirwin32.obj" \
	"$(INTDIR)\exitproc.obj" \
	"$(INTDIR)\FTNADDR.OBJ" \
	"$(INTDIR)\ftndom.obj" \
	"$(INTDIR)\ftnnode.obj" \
	"$(INTDIR)\FTNQ.OBJ" \
	"$(INTDIR)\getfree.obj" \
	"$(INTDIR)\getw.obj" \
	"$(INTDIR)\INBOUND.OBJ" \
	"$(INTDIR)\iptools.obj" \
	"$(INTDIR)\pmatch.obj" \
	"$(INTDIR)\prothlp.obj" \
	"$(INTDIR)\PROTOCOL.OBJ" \
	"$(INTDIR)\READCFG.OBJ" \
	"$(INTDIR)\readflo.obj" \
	"$(INTDIR)\run.obj" \
	"$(INTDIR)\sem.obj" \
	"$(INTDIR)\SERVER.OBJ" \
	"$(INTDIR)\setpttl.obj" \
	"$(INTDIR)\srif.obj" \
	"$(INTDIR)\TCPErr.obj" \
	"$(INTDIR)\TOOLS.OBJ" \
	"$(INTDIR)\WSock.obj" \
	"$(INTDIR)\md5b.obj" \
	"$(INTDIR)\xalloc.obj"

"$(OUTDIR)\binkd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "binkd - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\binkd.exe"


CLEAN :
	-@erase "$(INTDIR)\service.obj"
	-@erase "$(INTDIR)\https.obj"
	-@erase "$(INTDIR)\BINKD.OBJ"
	-@erase "$(INTDIR)\binlog.obj"
	-@erase "$(INTDIR)\branch.obj"
	-@erase "$(INTDIR)\breaksig.obj"
	-@erase "$(INTDIR)\BSY.OBJ"
	-@erase "$(INTDIR)\CLIENT.OBJ"
	-@erase "$(INTDIR)\dirwin32.obj"
	-@erase "$(INTDIR)\exitproc.obj"
	-@erase "$(INTDIR)\FTNADDR.OBJ"
	-@erase "$(INTDIR)\ftndom.obj"
	-@erase "$(INTDIR)\ftnnode.obj"
	-@erase "$(INTDIR)\FTNQ.OBJ"
	-@erase "$(INTDIR)\getfree.obj"
	-@erase "$(INTDIR)\getw.obj"
	-@erase "$(INTDIR)\INBOUND.OBJ"
	-@erase "$(INTDIR)\iptools.obj"
	-@erase "$(INTDIR)\pmatch.obj"
	-@erase "$(INTDIR)\prothlp.obj"
	-@erase "$(INTDIR)\PROTOCOL.OBJ"
	-@erase "$(INTDIR)\READCFG.OBJ"
	-@erase "$(INTDIR)\readflo.obj"
	-@erase "$(INTDIR)\run.obj"
	-@erase "$(INTDIR)\sem.obj"
	-@erase "$(INTDIR)\SERVER.OBJ"
	-@erase "$(INTDIR)\setpttl.obj"
	-@erase "$(INTDIR)\srif.obj"
	-@erase "$(INTDIR)\TCPErr.obj"
	-@erase "$(INTDIR)\TOOLS.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WSock.obj"
	-@erase "$(INTDIR)\xalloc.obj"
	-@erase "$(OUTDIR)\binkd.exe"
	-@erase "$(OUTDIR)\binkd.ilk"
	-@erase "$(INTDIR)\md5b.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /Ox /D "HTTPS" /D "WIN32" /D "_CONSOLE" /D "HAVE_THREADS" /D "HAVE_IO_H" /D "VISUALCPP" /Fp"$(INTDIR)\binkd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\binkd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /pdb:"$(OUTDIR)\binkd.pdb" /machine:I386 /out:"$(OUTDIR)\binkd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\service.obj" \
	"$(INTDIR)\https.obj" \
	"$(INTDIR)\BINKD.OBJ" \
	"$(INTDIR)\binlog.obj" \
	"$(INTDIR)\branch.obj" \
	"$(INTDIR)\breaksig.obj" \
	"$(INTDIR)\BSY.OBJ" \
	"$(INTDIR)\CLIENT.OBJ" \
	"$(INTDIR)\dirwin32.obj" \
	"$(INTDIR)\exitproc.obj" \
	"$(INTDIR)\FTNADDR.OBJ" \
	"$(INTDIR)\ftndom.obj" \
	"$(INTDIR)\ftnnode.obj" \
	"$(INTDIR)\FTNQ.OBJ" \
	"$(INTDIR)\getfree.obj" \
	"$(INTDIR)\getw.obj" \
	"$(INTDIR)\INBOUND.OBJ" \
	"$(INTDIR)\iptools.obj" \
	"$(INTDIR)\pmatch.obj" \
	"$(INTDIR)\prothlp.obj" \
	"$(INTDIR)\PROTOCOL.OBJ" \
	"$(INTDIR)\READCFG.OBJ" \
	"$(INTDIR)\readflo.obj" \
	"$(INTDIR)\run.obj" \
	"$(INTDIR)\sem.obj" \
	"$(INTDIR)\SERVER.OBJ" \
	"$(INTDIR)\setpttl.obj" \
	"$(INTDIR)\srif.obj" \
	"$(INTDIR)\TCPErr.obj" \
	"$(INTDIR)\TOOLS.OBJ" \
	"$(INTDIR)\WSock.obj" \
	"$(INTDIR)\md5b.obj" \
	"$(INTDIR)\xalloc.obj"

"$(OUTDIR)\binkd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Makefile.dep")
!INCLUDE "Makefile.dep"
!ELSE 
!MESSAGE Warning: cannot find "Makefile.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "binkd - Win32 Debug" || "$(CFG)" == "binkd - Win32 Release"
SOURCE=.\BINKD.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\BINKD.OBJ"	"$(INTDIR)\BINKD.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\BINKD.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\binlog.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\binlog.obj"	"$(INTDIR)\binlog.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\binlog.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\branch.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\branch.obj"	"$(INTDIR)\branch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\branch.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\breaksig.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\breaksig.obj"	"$(INTDIR)\breaksig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\breaksig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\BSY.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\BSY.OBJ"	"$(INTDIR)\BSY.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\BSY.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\CLIENT.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\CLIENT.OBJ"	"$(INTDIR)\CLIENT.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\CLIENT.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\dirwin32.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\dirwin32.obj"	"$(INTDIR)\dirwin32.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\dirwin32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\exitproc.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\exitproc.obj"	"$(INTDIR)\exitproc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\exitproc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\FTNADDR.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\FTNADDR.OBJ"	"$(INTDIR)\FTNADDR.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\FTNADDR.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ftndom.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\ftndom.obj"	"$(INTDIR)\ftndom.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\ftndom.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ftnnode.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\ftnnode.obj"	"$(INTDIR)\ftnnode.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\ftnnode.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\FTNQ.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\FTNQ.OBJ"	"$(INTDIR)\FTNQ.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\FTNQ.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\getfree.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\getfree.obj"	"$(INTDIR)\getfree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\getfree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\getw.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\getw.obj"	"$(INTDIR)\getw.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\getw.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\INBOUND.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\INBOUND.OBJ"	"$(INTDIR)\INBOUND.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\INBOUND.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\iptools.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\iptools.obj"	"$(INTDIR)\iptools.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\iptools.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pmatch.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\pmatch.obj"	"$(INTDIR)\pmatch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\pmatch.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\prothlp.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\prothlp.obj"	"$(INTDIR)\prothlp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\prothlp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PROTOCOL.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\PROTOCOL.OBJ"	"$(INTDIR)\PROTOCOL.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\PROTOCOL.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\READCFG.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\READCFG.OBJ"	"$(INTDIR)\READCFG.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\READCFG.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\readflo.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\readflo.obj"	"$(INTDIR)\readflo.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\readflo.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\run.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\run.obj"	"$(INTDIR)\run.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\run.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\sem.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\sem.obj"	"$(INTDIR)\sem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\sem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\SERVER.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\SERVER.OBJ"	"$(INTDIR)\SERVER.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\SERVER.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\setpttl.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\setpttl.obj"	"$(INTDIR)\setpttl.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\setpttl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\srif.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\srif.obj"	"$(INTDIR)\srif.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\srif.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\TCPErr.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\TCPErr.obj"	"$(INTDIR)\TCPErr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\TCPErr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\TOOLS.C

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\TOOLS.OBJ"	"$(INTDIR)\TOOLS.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\TOOLS.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nt\WSock.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\WSock.obj"	"$(INTDIR)\WSock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\WSock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\xalloc.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\xalloc.obj"	"$(INTDIR)\xalloc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\xalloc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\https.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\https.obj"	"$(INTDIR)\https.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\https.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\md5b.c

!IF  "$(CFG)" == "binkd - Win32 Debug"


"$(INTDIR)\md5b.obj"	"$(INTDIR)\md5b.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "binkd - Win32 Release"


"$(INTDIR)\md5b.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


SOURCE=.\nt\service.c

!IF  "$(CFG)" == "binkd - Win32 Debug"

"$(INTDIR)\service.obj"	"$(INTDIR)\service.SBR" : $(SOURCE) "$(INTDIR)"

!ELSEIF  "$(CFG)" == "binkd - Win32 Release"

"$(INTDIR)\service.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

!ENDIF 

!ENDIF 

