#
CC=cl
DEFINES=-DDOS -D__MSC__ -DHAVE_IO_H -DHAVE_DOS_H -DIBMTCPIPDOS -DVOID_SLEEP -DHAVE_ARPA_INET_H -DHTTPS -DNTLM -DPROTOTYPES=1
CFLAGS=$(DEFINES) /AL /G2 /W3 /c /nologo
#LFLAGS=/AL /F AF00
#LFLAGS=/AL /F 9840 /nologo
LFLAGS=/AL /F 9700 /nologo

SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c ftndom.c ftnnode.c dos\getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c dos\tcperr.c dos\dirent.c dos\sleep.c xalloc.c setpttl.c https.c md5b.c crypt.c
OBJS=$(SRCS:.c=.obj)
LIBS=/link /bat /inf socketl.lib

all: binkd.exe

.c.obj:
#	$(CC) $(CFLAGS) /Fo$@ $<
#	$(CC) $(CFLAGS) /Fo$*.obj $*.c
	@set CL=$(CFLAGS)
	@$(CC) /Fo$*.obj $*.c

binkd.exe: $(OBJS)
#	$(CC) $(LFLAGS) /Febinkd.exe $(OBJS) $(LIBS)
	$(CC) $(LFLAGS) /Febinkd.exe *.obj dos\*.obj $(LIBS)

install: all clean

clean:
	-del /n *.RES dos\*.obj *.obj *.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

#include Makefile.dep
