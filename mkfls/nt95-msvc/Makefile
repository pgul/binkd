# Microsoft Visual C++ Makefile
# $Id$
#
# Usage: nmake [DEBUG=1] [BINKD9X=1]
#

!ifdef BINKD9X
BINKDEXE=  binkd9x
BINKD9X_DEFINES= -D "_WINDOWS" -D "BINKDW9X"
BINKD9X_SUBSYSTEM= windows
OUTDIRSUFFIX= -w9x
!else
BINKDEXE=  binkd
BINKD9X_DEFINES= -D"_CONSOLE"
BINKD9X_SUBSYSTEM= console
OUTDIRSUFFIX=
!endif

!ifndef DEBUG
CFLAGS=    -nologo -MT  -W3 -GX -Ox -c
CDEBUG=
OUTDIR=    .\Release$(OUTDIRSUFFIX)
BINKDSBR=
LDEBUG=
!else
CFLAGS=    -nologo -MTd -W3 -Gm -GX -ZI -Od -c
CDEBUG=    -D"_DEBUG"
OUTDIR=    .\Debug$(OUTDIRSUFFIX)
BINKDSBR=  -FR"$(OUTDIR)\\"
LDEBUG=    /incremental:yes /map:"$(OUTDIR)\$(BINKDEXE).map" /debug
!endif

CC=        cl
CDEFS=     $(CDEBUG) $(BINKD9X_DEFINES) -D"HTTPS" -D"WIN32" -D"HAVE_THREADS" -D"HAVE_IO_H" -D"HAVE_DOS_H" -D"VISUALCPP" -D"AMIGADOS_4D_OUTBOUND"
COPT=      $(BINKDSBR) -Fo"$(OUTDIR)\\" -Fp"$(OUTDIR)\$(BINKDEXE).pch" -YX -Fd"$(OUTDIR)\\"
LINK=      link
LFLAGS=    /nologo /machine:I386
LOPT=      $(LDEBUG) /subsystem:$(BINKD9X_SUBSYSTEM) /pdb:"$(OUTDIR)\$(BINKDEXE).pdb" /out:"$(OUTDIR)\$(BINKDEXE).exe"
LLIBS=     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib
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
 "$(OUTDIR)\TCPErr.obj"   "$(OUTDIR)\WSock.obj"    "$(OUTDIR)\dirwin32.obj"
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
 $(NT_OBJS) $(NTSERVICE_OBJ) $(BINKD9X_OBJ) $(NTLM_OBJS)

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
