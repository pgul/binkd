# $Id$
#
SHELL=bash.exe

ifdef BINKD9X
BINKD9X_SRC=nt/win9x.c
BINKD9X_DEFINES=-DBINKDW9X
BINKD9X_SUBSYSTEM=-Wl,--subsystem,windows
BINKDEXE=binkd9x.exe
else
BINKDEXE=binkd.exe
NT_SERVICE=nt/service.c
endif

CC=gcc
DEFINES=-DHAVE_THREADS -DHAVE_SNPRINTF -DHAVE_UNISTD -DWIN32 -DHAVE_IO_H -DHAVE_WAITPID -DHTTPS -DNTLM
CFLAGS=$(DEFINES) $(BINKD9X_DEFINES) -mno-cygwin -mthreads -Wall -funsigned-char -Wno-char-subscripts -s -O2
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c nt/breaksig.c branch.c nt/sem.c  ftndom.c ftnnode.c nt/getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c nt/TCPErr.c nt/WSock.c $(NT_SERVICE) $(NTLM_SRC) $(BINKD9X_SRC)
OBJS=${SRCS:.c=.o}

all: binkd.exe

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $*.c

binkd.exe: $(OBJS)
	$(CC) $(BINKD9X_SUBSYSTEM) $(CFLAGS) -o $(BINKDEXE) $(OBJS) -lwsock32

install: all clean

clean:
	-del /n *.RES >nul
	-del /n *.obj >nul
	-del /n *.o   >nul
	-del /n nt\*.o   >nul
	-del /n ntlm\*.o >nul
	-del /n *.map >nul
	-del /n *.bak >nul
	-del /n *.b   >nul
	-del /n *.ini >nul
	-del /n *.err >nul
	-del /n core  >nul

depend	Makefile.dep:
	gcc -MM $(CFLAGS) $(SRCS) | tee Makefile.dep

include Makefile.dep
