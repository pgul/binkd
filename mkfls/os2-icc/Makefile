#
CC=icc
DEFINES=-DHAVE_THREADS -DOS2 -DHAVE_IO_H -DIBMTCPIP -DVOID_SLEEP -DHTTPS -D_ReleaseSem=release_sem
CFLAGS=$(DEFINES) /Gm+ /Q /c /Ss
LFLAGS=$(DEFINES) /Gm+ /Q /B"/noi /pm:vio /st:64000"

SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c os2\gettid.c os2\sem.c  ftndom.c ftnnode.c os2\getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c os2\tcperr.c os2\dirent.c xalloc.c setpttl.c https.c md5b.c crypt.c
OBJS=$(SRCS:.c=.obj)
LIBS=so32dll.LIB tcp32dll.LIB os2386.lib

all: binkd.exe

.c.obj:
#	$(CC) $(CFLAGS) /Fo$@ $<
	$(CC) $(CFLAGS) /Fo$*.obj $*.c

binkd.exe: $(OBJS)
	$(CC) $(LFLAGS) /Febinkd.exe $(OBJS) $(LIBS)

install: all clean

clean:
	-del /n *.RES os2\*.obj *.obj *.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

#include Makefile.dep
