# Microsoft Visual C++ Makefile
# $Id$
#
# Usage: nmake [DEBUG=1] [DLLRTL=1] [BINKD9X=1] [PERL=1] [PERLDL=1]
#              [ZLIB=1] [BZLIB2=1] [ZLIBDL=1] [DEBUGCHILD=1]
#
#

# define path to perl sources if using PERL=1
!ifdef PERL
PERL_BASE=c:\programs\Perl   # set perl package base path
PERL_VER=56                  # set perl version (56 for 5.6, 58 for 5.8)
!endif

# define path to zlib sources if using ZLIB=1 without ZLIBDL
!if defined(ZLIB) && !defined(ZLIBDL)
ZLIB_SRC=..\zlib
!endif
# define path to bzlib2 sources if using BZLIB2=1 without ZLIBDL
!if defined(BZLIB2) && !defined(ZLIBDL)
BZLIB2_SRC=..\bzlib2
!endif

!ifdef DLLRTL
RTLTYPE=D
RTLSUFFIX=-dll
RTL_DEFINES=-D"RTLDLL"
!else
RTLTYPE=T
RTLSUFFIX=
RTL_DEFINES=
!endif

!ifdef BINKD9X
BINKDEXE=  binkd9x$(RTLSUFFIX)
BINKD9X_DEFINES= -D"_WINDOWS" -D"BINKDW9X"
BINKD9X_SUBSYSTEM= windows
OUTDIRSUFFIX= -w9x
!else
BINKDEXE=  binkd$(RTLSUFFIX)
BINKD9X_DEFINES= -D"_CONSOLE"
BINKD9X_SUBSYSTEM= console
OUTDIRSUFFIX=
!endif

!ifndef DEBUG
CFLAGS=    -nologo -M$(RTLTYPE)  -W3 -GX -Ox -c
CDEBUG=
OUTDIR=    .\Release$(OUTDIRSUFFIX)$(RTLSUFFIX)
BINKDSBR=
LDEBUG=
!else
CFLAGS=    -nologo -M$(RTLTYPE)d -W3 -Gm -GX -ZI -Od -c
CDEBUG=    -D"_DEBUG" -D"DEBUG"
OUTDIR=    .\Debug$(OUTDIRSUFFIX)$(RTLSUFFIX)
BINKDSBR=  -FR"$(OUTDIR)\\"
LDEBUG=    /incremental:yes /map:"$(OUTDIR)\$(BINKDEXE).map" /debug
!endif

!ifdef DEBUGCHILD
DEBUGCHILD_CDEFS= -D"DEBUGCHILD"
!endif

!ifdef PERL
PERL_COPT= -I"$(PERL_BASE)\lib\core"
!ifndef PERLDL
PERL_CDEFS= -D"WITH_PERL" -DNDEBUG -DNO_STRICT -DHAVE_DES_FCRYPT -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS -DPERL_MSVCRT_READFIX
PERL_LLIBS= $(PERL_BASE)\lib\core\perl$(PERL_VER).lib
!else
PERL_CDEFS= -D"WITH_PERL=$(PERL_VER)" -D"PERLDL" -DNDEBUG -DNO_STRICT -DHAVE_DES_FCRYPT -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS -DPERL_MSVCRT_READFIX
!endif
PERL_OBJS= $(OUTDIR)\perlhooks.obj
!endif

!if defined(ZLIB) || defined(BZLIB2)
ZLIB_OBJS = $(OUTDIR)\compress.obj
!endif
!if defined(ZLIB)
ZLIB_CDEFS= $(ZLIB_CDEFS) -DWITH_ZLIB
!endif
!if defined(BZLIB2)
ZLIB_CDEFS= $(ZLIB_CDEFS) -DWITH_BZLIB2
!endif
!ifdef ZLIBDL
ZLIB_CDEFS= $(ZLIB_CDEFS) -DZLIBDL
ZLIBDL_OBJS = $(OUTDIR)\zlibdl.obj
!else
! if defined(ZLIB)
ZLIB_COPT= -I"$(ZLIB_SRC)"
ZLIB_LLIBS= $(ZLIB_LLIBS) $(ZLIB_SRC)\zlib.lib
! endif
! if defined(BZLIB2)
ZLIB_COPT= -I"$(BZLIB2_SRC)"
ZLIB_LLIBS= $(ZLIB_LLIBS) $(BZLIB2_SRC)\libbz2.lib
! endif
!endif

CC=        cl
CDEFS=     $(CDEBUG) $(DEBUGCHILD_CDEFS) $(BINKD9X_DEFINES) $(RTL_DEFINES) $(PERL_CDEFS) $(ZLIB_CDEFS) -D"HTTPS" -D"WIN32" -D"HAVE_THREADS" -D"HAVE_IO_H" -D"HAVE_DOS_H" -D"HAVE_STDARG_H" -D"HAVE_SNPRINTF" -D"HAVE_VSNPRINTF" -D"VISUALCPP" -D"AMIGADOS_4D_OUTBOUND"
COPT=      $(BINKDSBR) $(PERL_COPT) $(ZLIB_COPT) -Fo"$(OUTDIR)\\" -Fp"$(OUTDIR)\$(BINKDEXE).pch" -YX -Fd"$(OUTDIR)\\"
LINK=      link
LFLAGS=    /nologo /machine:I386
LOPT=      $(LDEBUG) /subsystem:$(BINKD9X_SUBSYSTEM) /pdb:"$(OUTDIR)\$(BINKDEXE).pdb" /out:"$(OUTDIR)\$(BINKDEXE).exe" $(PERL_LOPT)
LLIBS=     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib $(ZLIB_LLIBS) $(PERL_LLIBS)
BSC=       bscmake
BINKDBSC=  "$(OUTDIR)\$(BINKDEXE).bsc"
BSCFLAGS=  /nologo /o$(BINKDBSC)

!ifdef BINKD9X
BINKD9X_OBJ="$(OUTDIR)\win9x.obj"
NTSERVICE_OBJ=
!else
BINKD9X_OBJ=
NTSERVICE_OBJ="$(OUTDIR)\service.obj"
!endif

NTLM_OBJS= \
 "$(OUTDIR)\des_enc.obj"  "$(OUTDIR)\helpers.obj"  "$(OUTDIR)\ecb_enc.obj" \
 "$(OUTDIR)\md4_dgst.obj" "$(OUTDIR)\set_key.obj"
NT_OBJS= \
 "$(OUTDIR)\breaksig.obj" "$(OUTDIR)\sem.obj"      "$(OUTDIR)\getfree.obj" \
 "$(OUTDIR)\TCPErr.obj"   "$(OUTDIR)\WSock.obj"    "$(OUTDIR)\dirwin32.obj" \
 "$(OUTDIR)\w32tools.obj"  "$(OUTDIR)\tray.obj"
OBJS= \
 "$(OUTDIR)\binkd.obj"    "$(OUTDIR)\readcfg.obj"  "$(OUTDIR)\tools.obj"   \
 "$(OUTDIR)\ftnaddr.obj"  "$(OUTDIR)\ftnq.obj"     "$(OUTDIR)\client.obj"  \
 "$(OUTDIR)\server.obj"   "$(OUTDIR)\protocol.obj" "$(OUTDIR)\bsy.obj"     \
 "$(OUTDIR)\inbound.obj"  "$(OUTDIR)\branch.obj"   "$(OUTDIR)\ftndom.obj"  \
 "$(OUTDIR)\ftnnode.obj"  "$(OUTDIR)\srif.obj"     "$(OUTDIR)\pmatch.obj"  \
 "$(OUTDIR)\readflo.obj"  "$(OUTDIR)\prothlp.obj"  "$(OUTDIR)\iptools.obj" \
 "$(OUTDIR)\run.obj"      "$(OUTDIR)\binlog.obj"   "$(OUTDIR)\exitproc.obj"\
 "$(OUTDIR)\getw.obj"     "$(OUTDIR)\xalloc.obj"   "$(OUTDIR)\setpttl.obj" \
 "$(OUTDIR)\https.obj"    "$(OUTDIR)\md5b.obj"     "$(OUTDIR)\crypt.obj"   \
 "$(OUTDIR)\getopt.obj"                                                    \
 $(NT_OBJS) $(NTSERVICE_OBJ) $(BINKD9X_OBJ) $(NTLM_OBJS) $(PERL_OBJS) $(ZLIB_OBJS) $(ZLIBDL_OBJS)

!ifdef DEBUG
SBRS= $(OBJS:.obj=.sbr)
BSCTARGET= $(BINKDBSC)
!endif

all:      "$(OUTDIR)" "$(OUTDIR)\$(BINKDEXE).exe" $(BSCTARGET)

"$(OUTDIR)\$(BINKDEXE).exe": "$(OUTDIR)" $(OBJS)
	$(LINK) $(LFLAGS) $(LOPT) @<<
$(LLIBS) $(OBJS)
<<

$(BINKDBSC): "$(OUTDIR)" $(SBRS)
	$(BSC) $(BSCFLAGS) @<<
$(SBRS)
<<

.c{$(OUTDIR)}.obj::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<
{nt}.c{$(OUTDIR)}.obj::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<
{ntlm}.c{$(OUTDIR)}.obj::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<

.c{$(OUTDIR)}.sbr::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<
{nt}.c{$(OUTDIR)}.sbr::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<
{ntlm}.c{$(OUTDIR)}.sbr::
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) @<<
$<
<<

"$(OUTDIR)":
	mkdir "$(OUTDIR)"

clean:
	-@del "$(OUTDIR)\*.obj"
	-@del "$(OUTDIR)\$(BINKDEXE).pch"
	-@del "$(OUTDIR)\*.sbr"
	-@del "$(OUTDIR)\*.idb"
	-@del "$(OUTDIR)\$(BINKDEXE).ilk"

distclean: clean
	-@del "$(OUTDIR)\$(BINKDEXE).exe"
	-@del "$(OUTDIR)\$(BINKDEXE).bsc"
	-@del "$(OUTDIR)\$(BINKDEXE).map"
	-@del "$(OUTDIR)\*.pdb"

!if exists("Makefile.dep")
!include "Makefile.dep"
!endif
