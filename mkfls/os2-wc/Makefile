# from "Pavel Gulchouck" <gul@gul.kiev.ua>
# $Id$

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf .cpp

CC         = wcc386 -D_Optlink=
PP         = wpp386 -D_Optlink=

AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
LFLAGS  = /bm /k1024k
CFLAGS  = -DOS2 -DHAVE_THREADS -DHAVE_IO_H -DHAVE_SYS_TIME_H -DHAVE_SNPRINTF -Dsnprintf=_bprintf -Dvsnprintf=_vbprintf -DIBMTCPIP -DVOID_SLEEP -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND /w4 /ox /bm # -DWATCOMC_CLOSEDIR_BUG
LINK    = wcl386
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
NTLM_OBJS=    des_enc.obj    helpers.obj    ecb_enc.obj    md4_dgst.obj    set_key.obj
SRCS=binkd.c   readcfg.c   tools.c   ftnaddr.c   ftnq.c   client.c   server.c   protocol.c   bsy.c   os2\gettid.c os2\tcperr.c inbound.c   os2\sem.c breaksig.c   branch.c   ftndom.c   ftnnode.c   getfree.c   srif.c   pmatch.c   readflo.c   prothlp.c   iptools.c   run.c   binlog.c   exitproc.c   getw.c   xalloc.c   setpttl.c   https.c   md5b.c   crypt.c   $(NTLM_SRC)
OBJS=binkd.obj readcfg.obj tools.obj ftnaddr.obj ftnq.obj client.obj server.obj protocol.obj bsy.obj gettid.obj   tcperr.obj   inbound.obj sem.obj   breaksig.obj branch.obj ftndom.obj ftnnode.obj getfree.obj srif.obj pmatch.obj readflo.obj prothlp.obj iptools.obj run.obj binlog.obj exitproc.obj getw.obj xalloc.obj setpttl.obj https.obj md5b.obj crypt.obj $(NTLM_OBJS)

.c.obj:
	$(CC) $(CFLAGS) $*.c

all: binkd.exe .SYMBOLIC

binkd.obj:    binkd.c         .AUTODEPEND
readcfg.obj:  readcfg.c       .AUTODEPEND
tools.obj:    tools.c         .AUTODEPEND
ftnaddr.obj:  ftnaddr.c       .AUTODEPEND
ftnq.obj:     ftnq.c          .AUTODEPEND
client.obj:   client.c        .AUTODEPEND
server.obj:   server.c        .AUTODEPEND
protocol.obj: protocol.c      .AUTODEPEND
bsy.obj:      bsy.c           .AUTODEPEND
inbound.obj:  inbound.c       .AUTODEPEND
breaksig.obj: breaksig.c      .AUTODEPEND
branch.obj:   branch.c        .AUTODEPEND
gettid.obj:   os2\gettid.c    .AUTODEPEND
sem.obj:      os2\sem.c       .AUTODEPEND
ftndom.obj:   ftndom.c        .AUTODEPEND
ftnnode.obj:  ftnnode.c       .AUTODEPEND
getfree.obj:  os2\getfree.c   .AUTODEPEND
srif.obj:     srif.c          .AUTODEPEND
pmatch.obj:   pmatch.c        .AUTODEPEND
readflo.obj:  readflo.c       .AUTODEPEND
prothlp.obj:  prothlp.c       .AUTODEPEND
iptools.obj:  iptools.c       .AUTODEPEND
run.obj:      run.c           .AUTODEPEND
https.obj:    https.c         .AUTODEPEND
md5.obj:      md5.c           .AUTODEPEND
crypt.obj:    crypt.c         .AUTODEPEND
tcperr.obj:   os2\tcperr.c    .AUTODEPEND
des_enc.obj:  ntlm\des_enc.c  .AUTODEPEND
helpers.obj:  ntlm\helpers.c  .AUTODEPEND
ecb_enc.obj:  ntlm\ecb_enc.c  .AUTODEPEND
md4_dgst.obj: ntlm\md4_dgst.c .AUTODEPEND
set_key.obj:  ntlm\set_key.c  .AUTODEPEND

gettid.obj: os2\gettid.c
	$(CC) $(CFLAGS) os2\gettid.c
tcperr.obj: os2\tcperr.c
	$(CC) $(CFLAGS) os2\tcperr.c
sem.obj: os2\sem.c
	$(CC) $(CFLAGS) os2\sem.c
getfree.obj: os2\getfree.c
	$(CC) $(CFLAGS) os2\getfree.c

des_enc.obj:  ntlm\des_enc.c
	$(CC) $(CFLAGS) ntlm\des_enc.c
helpers.obj:  ntlm\helpers.c
	$(CC) $(CFLAGS) ntlm\helpers.c
ecb_enc.obj:  ntlm\ecb_enc.c
	$(CC) $(CFLAGS) ntlm\ecb_enc.c
md4_dgst.obj: ntlm\md4_dgst.c
	$(CC) $(CFLAGS) ntlm\md4_dgst.c
set_key.obj:  ntlm\set_key.c
	$(CC) $(CFLAGS) ntlm\set_key.c

binkd.exe: $(OBJS)
	$(LINK) $(LFLAGS) /fe=$@ $(OBJS) so32dll.lib tcp32dll.lib

clean:  .SYMBOLIC
	-del /n *.RES *.obj os2\*.obj *.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul
