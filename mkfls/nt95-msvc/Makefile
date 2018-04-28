# Microsoft Visual C++ Makefile
# $Id$
#
# Usage: nmake [DEBUG=1] [STATIC=1] [BINKD9X=1] [PERL=1] [PERLDL=1]
#              [ZLIB=1] [BZLIB2=1] [ZLIBDL=1] [DEBUGCHILD=1] [BW_LIM=1]
#              [MSVC_VER=10] [IPV6=1] [FTS5004=1] [X64=1]
#
# Use MSVC_VER=10 to build with Visual Studio 2008 or 2010
#

#PERLV=510
#PERL_BASE=c:\Perl
#PERL_LIB=$(PERL_BASE)/lib/core/perl$(PERLV).lib

#ZLIB_BASE=..\zlib
#ZLIB_LIB=zlib.lib
#BZLIB2_BASE=..\bzlib2
#BZLIB2_LIB=libbz2.lib

# =============================================================================

CC=cl
LINK=link
BSC=bscmake
RSC=rc
!if "$(OS)" == "Windows_NT"
DELTREE=rmdir /S /Q
!else
DELTREE=deltree /Y
!endif

BINDIR=bin
OUTDIR=$(BINDIR)\msvc
BINKDTYPE=msvc

OBJDIR= $(OUTDIR)\obj
MAKEDIRSONLY= $(OBJDIR) $(OBJDIR)\nt $(OBJDIR)\ntlm
MAKEDIRS= $(BINDIR) $(OUTDIR) $(MAKEDIRSONLY)

!ifdef X64
MACHINETYPE=X64
CDEFS=   $(CDEFS) -DWIN64
!else
MACHINETYPE=I386
!endif

!if "$(MSVC_VER)" == "10"
CFLAGS=  $(CFLAGS) -nologo -M$(RTLTYPE) -W3 -EHsc -c -J \
         -Fp"$(OBJDIR)\$(BINKDNAME).pch" -Fd"$(OBJDIR)\$(BINKDNAME)"
CDEFS=   $(CDEFS) -DWIN32 -DHAVE_THREADS -DHAVE_IO_H -DHAVE_DOS_H \
         -DHAVE_STDARG_H -DVISUALCPP -DHTTPS -DAMIGADOS_4D_OUTBOUND -DNTLM \
         -DHAVE_VSNPRINTF -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE
OUTDIR=$(OUTDIR)10
!else
CFLAGS=  $(CFLAGS) -nologo -M$(RTLTYPE) -W3 -GX -c -YX -J \
         -Fp"$(OBJDIR)\$(BINKDNAME).pch" -Fd"$(OBJDIR)\$(BINKDNAME)"
CDEFS=   $(CDEFS) -DWIN32 -DHAVE_THREADS -DHAVE_IO_H -DHAVE_DOS_H \
         -DHAVE_STDARG_H -DVISUALCPP -DHTTPS -DAMIGADOS_4D_OUTBOUND -DNTLM
!endif

LFLAGS=  /nologo /machine:$(MACHINETYPE) /subsystem:$(SUBSYSTEM) \
         /pdb:"$(OUTDIR)\$(BINKDNAME).pdb" /out:"$(OUTDIR)\$(BINKDEXE)"
LIBS=    $(LIBS) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
         advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib

BSCFLAGS=/nologo

OBJS= \
 "$(OBJDIR)\binkd.obj"    "$(OBJDIR)\readcfg.obj"  "$(OBJDIR)\tools.obj"    \
 "$(OBJDIR)\ftnaddr.obj"  "$(OBJDIR)\ftnq.obj"     "$(OBJDIR)\client.obj"   \
 "$(OBJDIR)\server.obj"   "$(OBJDIR)\protocol.obj" "$(OBJDIR)\bsy.obj"      \
 "$(OBJDIR)\inbound.obj"  "$(OBJDIR)\branch.obj"   "$(OBJDIR)\ftndom.obj"   \
 "$(OBJDIR)\ftnnode.obj"  "$(OBJDIR)\srif.obj"     "$(OBJDIR)\pmatch.obj"   \
 "$(OBJDIR)\readflo.obj"  "$(OBJDIR)\prothlp.obj"  "$(OBJDIR)\iptools.obj"  \
 "$(OBJDIR)\run.obj"      "$(OBJDIR)\binlog.obj"   "$(OBJDIR)\exitproc.obj" \
 "$(OBJDIR)\getw.obj"     "$(OBJDIR)\xalloc.obj"   "$(OBJDIR)\setpttl.obj"  \
 "$(OBJDIR)\https.obj"    "$(OBJDIR)\md5b.obj"     "$(OBJDIR)\crypt.obj"    \
 "$(OBJDIR)\getopt.obj"   "$(OBJDIR)\snprintf.obj" "$(OBJDIR)\rfc2553.obj"  \
                                                                            \
 "$(OBJDIR)\ntlm\des_enc.obj" "$(OBJDIR)\ntlm\helpers.obj"                  \
 "$(OBJDIR)\ntlm\ecb_enc.obj" "$(OBJDIR)\ntlm\md4_dgst.obj"                 \
 "$(OBJDIR)\ntlm\set_key.obj"                                               \
                                                                            \
 "$(OBJDIR)\nt\breaksig.obj" "$(OBJDIR)\nt\dirwin32.obj"                    \
 "$(OBJDIR)\nt\getfree.obj"  "$(OBJDIR)\nt\sem.obj"                         \
 "$(OBJDIR)\nt\TCPErr.obj"   "$(OBJDIR)\nt\tray.obj"                        \
 "$(OBJDIR)\nt\w32tools.obj" "$(OBJDIR)\nt\WSock.obj"

RESOBJS= "$(OBJDIR)\nt\binkdres.res"

# binkd9x +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef BINKD9X
BINKDNAME=binkd9x
OUTDIR=$(OUTDIR)-binkd9x
BINKDTYPE=$(BINKDTYPE), binkd9x
CDEFS=$(CDEFS) -DBINKD9X -D_WINDOWS
SUBSYSTEM=windows
OBJS=$(OBJS) "$(OBJDIR)\nt\win9x.obj"
LIBS=$(LIBS) wsock32.lib
!else
!ifdef X64
BINKDNAME=binkd64
OUTDIR=$(OUTDIR)x64
!else
BINKDNAME=binkd
!endif
!ifdef IPV6
OUTDIR=$(OUTDIR)-binkd-ipv6
CDEFS=$(CDEFS) -D_CONSOLE -DIPV6
SUBSYSTEM=console
OBJS=$(OBJS) "$(OBJDIR)\nt\service.obj"
LIBS=$(LIBS) ws2_32.lib
!else
OUTDIR=$(OUTDIR)-binkd
CDEFS=$(CDEFS) -D_CONSOLE
SUBSYSTEM=console
OBJS=$(OBJS) "$(OBJDIR)\nt\service.obj"
LIBS=$(LIBS) wsock32.lib
!endif
!ifdef FTS5004
CDEFS=$(CDEFS) -DWITH_FTS5004
LIBS=$(LIBS) dnsapi.lib
OBJS=$(OBJS) "$(OBJDIR)\srv_gai.obj"
!endif
!endif
# binkd9x ---------------------------------------------------------------------

# static ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef STATIC
BINKDNAME=$(BINKDNAME)-static
OUTDIR=$(OUTDIR)-static
BINKDTYPE=$(BINKDTYPE), static
CDEFS=$(CDEFS) -DRTLSTATIC
RTLTYPE=T
!else
RTLTYPE=D
!endif
# static ----------------------------------------------------------------------

# debug +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef DEBUG
BINKDNAME=$(BINKDNAME)-debug
OUTDIR=$(OUTDIR)-debug
BINKDTYPE=$(BINKDTYPE), debug
RTLTYPE=$(RTLTYPE)d
CFLAGS=$(CFLAGS) -Gm -ZI -Od
CDEFS=$(CDEFS) -D_DEBUG -DDEBUG
LFLAGS=$(LFLAGS) /incremental:yes /map:"$(OUTDIR)\$(BINKDNAME).map" /debug
SBRS=$(OBJS:.obj=.sbr)
BINKDBSC="$(OUTDIR)\$(BINKDNAME).bsc"
!else
CFLAGS=$(CFLAGS) -Ox
!endif
# debug -----------------------------------------------------------------------

# debugchild, nofork ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef DEBUGCHILD
BINKDNAME=$(BINKDNAME)-debugchild
OUTDIR=$(OUTDIR)-debugchild
BINKDTYPE=$(BINKDTYPE), debugchild
CDEFS=$(CDEFS) -DDEBUGCHILD
!endif
# debugchild, nofork ----------------------------------------------------------

# bandwidth limiting ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef BW_LIM
CDEFS=$(CDEFS) -DBW_LIM=1
!endif
# bandwidth limiting ----------------------------------------------------------

# soft AF force +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef AF_FORCE
CDEFS=$(CDEFS) -DAF_FORCE=1
!endif
# soft AF force ---------------------------------------------------------------

# perl support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# PERLDL implies PERL
!if defined(PERLDL) && !defined(PERL)
PERL=1
!endif

!ifdef PERL

!ifndef PERLDL
BINKDNAME=$(BINKDNAME)-perl
OUTDIR=$(OUTDIR)-perl
BINKDTYPE=$(BINKDTYPE), perl
!else
BINKDNAME=$(BINKDNAME)-perldl
OUTDIR=$(OUTDIR)-perldl
BINKDTYPE=$(BINKDTYPE), perldl
CDEFS=$(CDEFS) -DPERLDL
!endif

!ifdef PERL_BASE
PERL_INCL=$(PERL_BASE)\lib\CORE
!endif

CDEFS=$(CDEFS) -DWITH_PERL -DNDEBUG -DNO_STRICT -DHAVE_DES_FCRYPT \
      -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS -DPERL_MSVCRT_READFIX
OBJS=$(OBJS) "$(OBJDIR)\perlhooks.obj"

!endif
# perl support ----------------------------------------------------------------

# zlib support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef ZLIB
CDEFS=$(CDEFS) -DWITH_ZLIB

!ifdef ZLIB_BASE
C_DIRS=$(C_DIRS) -I "$(ZLIB_BASE)"
!ifndef ZLIBDL
L_DIRS=$(L_DIRS) /LIBPATH:"$(ZLIB_BASE)"
!endif
!endif

!ifndef ZLIBDL
BINKDNAME=$(BINKDNAME)-zlib
OUTDIR=$(OUTDIR)-zlib
BINKDTYPE=$(BINKDTYPE), zlib
!ifndef ZLIB_LIB
ZLIB_LIB=zlib.lib
!endif
LIBS=$(LIBS) $(ZLIB_LIB)
!else
BINKDNAME=$(BINKDNAME)-zlibdl
OUTDIR=$(OUTDIR)-zlibdl
BINKDTYPE=$(BINKDTYPE), zlibdl
!endif

!endif
# zlib support ----------------------------------------------------------------

# bzlib2 support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef BZLIB2
CDEFS=$(CDEFS) -DWITH_BZLIB2

!ifdef BZLIB2_BASE
C_DIRS=$(C_DIRS) -I "$(BZLIB2_BASE)"
!ifndef ZLIBDL
L_DIRS=$(L_DIRS) /LIBPATH:"$(BZLIB2_BASE)"
!endif
!endif

!ifndef ZLIBDL
BINKDNAME=$(BINKDNAME)-bzlib2
OUTDIR=$(OUTDIR)-bzlib2
BINKDTYPE=$(BINKDTYPE), bzlib2
!ifndef BZLIB2_LIB
BZLIB2_LIB=libbz2.lib
!endif
LIBS=$(LIBS) $(BZLIB2_LIB)
!else
BINKDNAME=$(BINKDNAME)-bzlib2dl
OUTDIR=$(OUTDIR)-bzlib2dl
BINKDTYPE=$(BINKDTYPE), bzlib2dl
!endif

!endif
# bzlib2 support --------------------------------------------------------------

# zlib & bzlib2 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!if defined(ZLIB) || defined(BZLIB2)
OBJS=$(OBJS) "$(OBJDIR)\compress.obj"
!ifdef ZLIBDL
CDEFS=$(CDEFS) -DZLIBDL
OBJS=$(OBJS) "$(OBJDIR)\zlibdl.obj"
!endif
!endif
# zlib & bzlib2 ---------------------------------------------------------------

# PERL_BASE & PERL_LIB ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
!ifdef PERL

!if !defined(PERL_INCL) || (!defined(PERL_DL) && !defined(PERL_LIB))

!if !exists("$(BINDIR)")
!if ![mkdir "$(BINDIR)"]
!message Making directory $(BINDIR)...
!endif
!endif
!if !exists("$(OUTDIR)")
!if ![mkdir "$(OUTDIR)"]
!message Making directory $(OUTDIR)...
!else
!error Can't create directory $(OUTDIR)!
!endif
!endif

!if ![perl mkfls\nt95-msvc\perlconf.pl $(OUTDIR)\perlconf.tmp]
!if exists("$(OUTDIR)\perlconf.tmp")
!include "$(OUTDIR)\perlconf.tmp"
!if ![del "$(OUTDIR)\perlconf.tmp"]
!endif
!else
!error Can't include $(OUTDIR)\perlconf.tmp!
!endif
!else
!error Can't execute perlconf.pl!
!endif
!endif

!ifdef PERL_INCL
C_DIRS=$(C_DIRS) -I "$(PERL_INCL)"
!endif
!ifndef PERLDL
!ifdef PERL_INCL
L_DIRS=$(L_DIRS) /LIBPATH:"$(PERL_INCL)"
!endif
!ifdef PERL_LIB
LIBS=$(LIBS) delayimp.lib $(PERL_LIB)
PERLDLL=/delayload:perl$(PERLV).dll
!endif
!endif

!endif
# PERL_BASE & PERL_LIB --------------------------------------------------------


BINKDEXE = $(BINKDNAME).exe

all: printinfo makedirs "$(OUTDIR)\$(BINKDEXE)" $(BINKDBSC)

printinfo:
	@echo on
	@type <<
-----------------------------------------------------------
binkd type : $(BINKDTYPE)
output dir : $(OUTDIR)
binkd exe  : $(BINKDEXE)
!ifdef PERL
perlincl   : $(PERL_INCL)
!ifndef PERLDL
perllib    : $(PERL_LIB)
!endif
!endif
!ifdef ZLIB
!ifdef ZLIB_BASE
zlibincl   : $(ZLIB_BASE)
!endif
!ifndef ZLIBDL
zliblib    : $(ZLIB_LIB)
!endif
!endif
!ifdef BZLIB2
!ifdef BZLIB2_BASE
bzlib2incl : $(BZLIB2_BASE)
!endif
!ifndef ZLIBDL
bzlib2lib  : $(BZLIB2_LIB)
!endif
!endif
-----------------------------------------------------------
<<

makedirs: $(MAKEDIRS)

$(MAKEDIRS):
	@echo Making directory $@...
	@mkdir $@

"$(OUTDIR)\$(BINKDEXE)": $(OBJS) $(RESOBJS)
	@echo Linking $(BINKDEXE)...
	@$(LINK) $(LFLAGS) $(L_DIRS) @<<
$(LIBS) $(OBJS) $(RESOBJS) $(PERLDLL)
<<

!ifdef DEBUG
$(BINKDBSC): $(SBRS)
	@echo Compiling $@...
	@$(BSC) $(BSCFLAGS) /o$(BINKDBSC) @<<
$(SBRS)
<<
!endif

.c{$(OBJDIR)}.obj::
	@echo Compiling:
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\\" -FR"$(OBJDIR)\\" @<<
$<
<<

{nt}.c{$(OBJDIR)\nt}.obj::
	@echo Compiling (nt/*):
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\nt\\" -FR"$(OBJDIR)\\nt\\" @<<
$<
<<

{ntlm}.c{$(OBJDIR)\ntlm}.obj::
	@echo Compiling (ntlm/*):
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\ntlm\\" -FR"$(OBJDIR)\\ntlm\\" @<<
$<
<<

.c{$(OBJDIR)}.sbr::
	@echo Compiling .sbr/.obj:
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\\" -FR"$(OBJDIR)\\" @<<
$<
<<

{nt}.c{$(OBJDIR)\nt}.sbr::
	@echo Compiling .sbr/.obj (nt/*):
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\nt\\" -FR"$(OBJDIR)\\nt\\" @<<
$<
<<

{ntlm}.c{$(OBJDIR)\ntlm}.sbr::
	@echo Compiling .sbr/.obj (ntlm/*):
	@$(CC) $(CFLAGS) $(CDEFS) $(C_DIRS) -Fo"$(OBJDIR)\ntlm\\" -FR"$(OBJDIR)\\ntlm\\" @<<
$<
<<

{nt}.rc{$(OBJDIR)\nt}.res:
	@echo Compiling $<...
	@$(RSC) $(CDEFS) -D"BINKDNAME=$(BINKDNAME)" -D"BINKDEXE=$(BINKDEXE)" -D"_MSC_VER" /fo$@ "$<"

clean:
	-@$(DELTREE) "$(OBJDIR)"
	-@del "$(OUTDIR)\$(BINKDNAME).ilk"

distclean:
	-@$(DELTREE) "$(OUTDIR)"

!if exists("Makefile.dep")
!include "Makefile.dep"
!endif
