# $Id$
#
ifeq ($(OSTYPE),msys)
SHELL=sh
else
SHELL=bash
endif

CC=gcc
DEFINES=-DHAVE_THREADS -DHAVE_SNPRINTF -DHAVE_VSNPRINTF -DHAVE_UNISTD -DWIN32 -DHAVE_IO_H -DHAVE_DOS_H -DHAVE_STDARG_H -DHAVE_WAITPID -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND
CFLAGS=$(DEFINES) -mno-cygwin -mthreads -funsigned-char -Wall -Wno-char-subscripts
LFLAGS=-mno-cygwin -mthreads -Wall
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c branch.c ftndom.c ftnnode.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c getopt.c nt/breaksig.c nt/getfree.c nt/sem.c nt/TCPErr.c nt/WSock.c nt/w32tools.c $(NTLM_SRC)
LIBS=-lwsock32

ifdef BINKD9X
SRCS+= nt/win9x.c
CFLAGS+= -DBINKDW9X
LFLAGS+= -Wl,--subsystem,windows
BINKDEXE=binkd9x-mingw.exe
else
BINKDEXE=binkd-mingw.exe
SRCS+= nt/service.c
endif

ifdef PERL
SRCS+= perlhooks.c
CFLAGS+= -DWITH_PERL
PERLDIR=$(shell perl -MConfig -e '$$_=$$Config{archlib}; s/\\/\\\\/g; print;')
#CFLAGS+= $(shell perl -MExtUtils::Embed -e ccopts 2>/dev/null)
#LIBS+= $(shell perl -MExtUtils::Embed -e ldopts 2>/dev/null)
CFLAGS+= -D__MSVCRT__ -I$(PERLDIR)\\CORE -Wno-comment
LIBS:=$(shell perl -MExtUtils::Embed -e ldopts 2>/dev/null | perl -npe 's/^.* //; s/\\/\\\\/g') $(LIBS)
endif

ifdef DEBUG
CFLAGS+= -g
LFLAGS+= -g
else
CFLAGS+= -s -O2
LFLAGS+= -s -O2
endif

OBJS=${SRCS:.c=.o}

all: $(BINKDEXE)

.c.o:
	@echo Compiling $*.c...
	@$(CC) -c $(CFLAGS) -o $*.o $*.c

$(BINKDEXE): $(OBJS)
	@echo Linking $(BINKDEXE)...
	@$(CC) $(LFLAGS) -o $(BINKDEXE) $(OBJS) $(LIBS)

install: all clean

html: binkd.html

binkd.html: binkd.8
	@echo Making binkd.html...
	@groff -Thtml -mman binkd.8 >binkd.html

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
	@echo Making dependences...
	@gcc -MM $(CFLAGS) $(SRCS) > Makefile.dep

include Makefile.dep
