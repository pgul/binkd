# $Id$

.SILENT

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf .cpp

.c:  .;os2;ntlm

# To make debug version uncomment follow string or use
# "wmake DEBUG=yes" command line
#DEBUG    = yes    

CC        = wcc386 -D_Optlink=
PP        = wpp386 -D_Optlink=

AFLAGS    = /Mx -t -z
ASM       = ml /c /Zm
LFLAGS    = /zq /bm /k1024k /x
!ifeq DEBUG yes
LFLAGS    += -d3 -hw /fm=binkd.map
!endif
CFLAGS    = -DOS2 -DHAVE_THREADS -DHAVE_IO_H -DHAVE_SYS_TIME_H      &
            -DHAVE_SYS_IOCTL_H -DHAVE_SNPRINTF -Dsnprintf=_bprintf  &
            -Dvsnprintf=_vbprintf -DIBMTCPIP -DVOID_SLEEP           &
            -DNTLM -DAMIGADOS_4D_OUTBOUND -DHAVE_DOS_H -DHTTPS      &
            /w4 /bm /zq 
!ifeq DEBUG yes
CFLAGS    += -d3 -hw
!else
CFLAGS    += -oxs
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
            getopt.obj    https.obj      $(NTLM_OBJS)

.c.obj: .autodepend
	-echo Compile  $<
	$(CC) $(CFLAGS) $<

all: binkd.exe .SYMBOLIC

before:	.SYMBOLIC
!ifeq DEBUG yes
	-echo Make binkd debug version from $(%cdrive):$(%cwd)
!else        
	-echo Make binkd release version from $(%cdrive):$(%cwd)
!endif

binkd.exe: before $(OBJS)
	-echo Link     $@
	$(LINK) $(LFLAGS) /fe=$@ $(OBJS)

clean:  .SYMBOLIC
	-del /n *.RES *.obj os2\*.obj *.o *.map *~ *.bak *.b *.ini &
             *.err core >nul 2> nul
