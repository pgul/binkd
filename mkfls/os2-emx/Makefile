#
CC=gcc
DEFINES=-DHAVE_FORK -DHAVE_SNPRINTF -DHAVE_UNISTD -DOS2 -DHAVE_IO_H -DHAVE_ARPA_INET_H -DEMX -DEMXSIGNALS -DHAVE_WAITPID -DHTTPS
CFLAGS=$(DEFINES) -Wall -Zcrtdll -funsigned-char -Wno-char-subscripts
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c os2/gettid.c os2/sem.c  ftndom.c ftnnode.c os2/getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c
OBJS=${SRCS:.c=.o}

all: binkd.exe

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $*.c

binkd.exe: $(OBJS)
	$(CC) -o binkd.exe -Zcrtdll $(OBJS) -lsocket

install: all clean

clean:
	-del /n *.RES *.obj *.o os2\\*.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

depend	Makefile.dep:
	gcc -MM $(CFLAGS) $(SRCS) | tee Makefile.dep

include Makefile.dep
