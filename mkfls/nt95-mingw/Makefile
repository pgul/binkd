# $Id$
#
ifeq ($(OSTYPE),msys)
SHELL=sh
else
SHELL=bash
endif

ifdef BINKD9X
BINKD9X_SRC=nt/win9x.c
BINKD9X_DEFINES=-DBINKDW9X
BINKD9X_SUBSYSTEM=-Wl,--subsystem,windows
BINKDEXE=binkd9x-mingw.exe
else
BINKDEXE=binkd-mingw.exe
NT_SERVICE=nt/service.c
endif

CC=gcc
DEFINES=-DHAVE_THREADS -DHAVE_SNPRINTF -DHAVE_VSNPRINTF -DHAVE_UNISTD -DWIN32 -DHAVE_IO_H -DHAVE_DOS_H -DHAVE_STDARG_H -DHAVE_WAITPID -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND
CFLAGS=$(DEFINES) $(BINKD9X_DEFINES) -mno-cygwin -mthreads -Wall -funsigned-char -Wno-char-subscripts
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c branch.c ftndom.c ftnnode.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c getopt.c nt/breaksig.c nt/getfree.c nt/sem.c nt/TCPErr.c nt/WSock.c nt/w32tools.c $(NT_SERVICE) $(NTLM_SRC) $(BINKD9X_SRC)
OBJS=${SRCS:.c=.o}

ifdef DEBUG
CFLAGS+= -g
else
CFLAGS+= -s -O2
endif

all: $(BINKDEXE)

.c.o:
	@echo Compiling $*.c...
	@$(CC) -c $(CFLAGS) -o $*.o $*.c

$(BINKDEXE): $(OBJS)
	@echo Linking $(BINKDEXE)...
	@$(CC) $(BINKD9X_SUBSYSTEM) $(CFLAGS) -o $(BINKDEXE) $(OBJS) -lwsock32

install: all clean

html: binkd.html

binkd.html: binkd.8
	groff -Thtml -mman binkd.8 >binkd.html

clean:
	-del *.RES >nul
	-del *.obj >nul
	-del *.o   >nul
	-del nt\*.o   >nul
	-del ntlm\*.o >nul
	-del *.map >nul
	-del *.bak >nul
	-del *.b   >nul
	-del *.ini >nul
	-del *.err >nul
	-del core  >nul
	-rm -f *.RES *.obj *.o
	-rm -f nt/*.o ntlm/*.o *.map *.bak *.b *.ini *.err core

depend	Makefile.dep:
	gcc -MM $(CFLAGS) $(SRCS) | tee Makefile.dep

include Makefile.dep
