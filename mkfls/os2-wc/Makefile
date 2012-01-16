# $Id$
#
# Usage: wmake [DEBUG=1] [NOFORK=1] [ZLIB=1] [BZLIB2=1] [ZLIBDL=1]
#

.SILENT

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf .cpp

.c:  .;os2;ntlm

CC        = wcc386 -D_Optlink=
PP        = wpp386 -D_Optlink=

AFLAGS    = /Mx -t -z
ASM       = ml /c /Zm
LFLAGS    = /zq /bm /k1024k /x
CFLAGS    = -DOS2 -DHAVE_THREADS -DHAVE_IO_H -DHAVE_SYS_TIME_H      &
            -DHAVE_SYS_IOCTL_H -DHAVE_SNPRINTF -Dsnprintf=_bprintf  &
            -DHAVE_VSNPRINTF -Dvsnprintf=_vbprintf -DHAVE_DOS_H     &
            -DHAVE_STDARG_H -DIBMTCPIP -DVOID_SLEEP                 &
            -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND                   &
	    -DHAVE_NETINET_IN_H -DHAVE_NETDB_H /w4 /bm /zq

!ifeq DEBUG 1
CFLAGS    += -d3 -hw
LFLAGS    += -d3 -hw /fm=binkd.map
!else
CFLAGS    += -oxs
!endif

!ifeq NOFORK 1
CFLAGS    += -DDEBUGCHILD
!endif

!ifeq ZLIB 1
CFLAGS    += -DWITH_ZLIB
COMPRESS  = 1
!ifeq ZLIBDL 1
!else
LIBS      += z.lib
!endif
!endif

!ifeq BZLIB2 1
CFLAGS    += -DWITH_BZLIB2
COMPRESS  = 1
!ifeq ZLIBDL 1
!else
LIBS      += bz2.lib
!endif
!endif

!ifeq COMPRESS 1
ZOBJS     = compress.obj
!ifeq ZLIBDL 1
ZOBJS     += zlibdl.obj
CFLAGS    += -DZLIBDL
!endif
!endif

LINK      = wcl386
NTLM_OBJS = des_enc.obj   helpers.obj    ecb_enc.obj  md4_dgst.obj  &
            set_key.obj
OBJS      = binkd.obj     readcfg.obj    tools.obj    ftnaddr.obj   &
            ftnq.obj      client.obj     server.obj   protocol.obj  &
            bsy.obj       gettid.obj     tcperr.obj   inbound.obj   &
            sem.obj       breaksig.obj   branch.obj   ftndom.obj    &
            ftnnode.obj   getfree.obj    srif.obj     pmatch.obj    &
            readflo.obj   prothlp.obj    iptools.obj  run.obj       &
            binlog.obj    exitproc.obj   getw.obj     xalloc.obj    &
            setpttl.obj   dirent.obj     md5b.obj     crypt.obj     &
            getopt.obj    https.obj      rfc2553.obj                &
            $(NTLM_OBJS) $(ZOBJS)

.c.obj: .autodepend
	-echo Compile  $<
	$(CC) $(CFLAGS) $<

all: binkd2.exe .SYMBOLIC

before:	.SYMBOLIC
!ifeq DEBUG 1
	-echo Make binkd debug version from $(%cdrive):$(%cwd)
!else        
	-echo Make binkd release version from $(%cdrive):$(%cwd)
!endif

binkd2.exe: before $(OBJS)
	-echo Link     $@
	$(LINK) $(LFLAGS) /fe=$@ $(OBJS) $(LIBS)

clean:  .SYMBOLIC
	-del /n *.RES *.obj os2\*.obj *.o *.map *~ *.bak *.b *.ini &
             *.err core >nul 2> nul
