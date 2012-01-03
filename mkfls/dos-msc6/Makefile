#
CC=cl
DEFINES=-DDOS -D__MSC__ -DHAVE_IO_H -DHAVE_DOS_H -DIBMTCPIPDOS -DVOID_SLEEP -DHAVE_ARPA_INET_H -DHAVE_STDARG_H -DPROTOTYPES=1 -DHTTPS -DNTLM
CFLAGS=$(DEFINES) /AL /G2 /W3 /c /nologo
LFLAGS=/AL /F 8000 /nologo

SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c ftndom.c ftnnode.c dos\getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c rfc2553.c run.c binlog.c exitproc.c getw.c dos\tcperr.c dos\dirent.c dos\sleep.c xalloc.c setpttl.c md5b.c crypt.c getopt.c snprintf.c https.c ntlm\des_enc.c ntlm\helpers.c ntlm\ecb_enc.c ntlm\md4_dgst.c ntlm\set_key.c
OBJS=$(SRCS:.c=.obj)
LIBS=/link /bat /inf socketl.lib

all: binkd.exe

.c.obj:
	@set CL=$(CFLAGS)
	$(CC) /Fo$*.obj $*.c

binkd.exe: $(OBJS)
	$(CC) $(LFLAGS) /Febinkd.exe *.obj dos\*.obj ntlm\*.obj $(LIBS)

install: all clean

clean:
	-del dos\*.obj >nul
	-del *.obj >nul

#include Makefile.dep
