# $Id$
#
CC=gcc
DEFINES=-DHAVE_FORK -DHAVE_SNPRINTF -DHAVE_UNISTD -DOS2 -DHAVE_IO_H -DHAVE_ARPA_INET_H -DHAVE_SYS_TIME_H -DHAVE_SYS_PARAM_H -DHAVE_SYS_IOCTL_H -DHAVE_DOS_H -DEMX -DEMXSIGNALS -DHAVE_WAITPID -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND
CFLAGS=$(DEFINES) -Wall -Zcrtdll -funsigned-char -Wno-char-subscripts
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c os2/gettid.c os2/sem.c  ftndom.c ftnnode.c os2/getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c ${NTLM_SRC}
OBJS=${SRCS:.c=.o}
LFLAGS=-Zcrtdll
TARGET=binkd2e.exe

ifdef DEBUG
CFLAGS+=-DDEBUG -g
LFLAGS+=-g
else
CFLAGS+=-O2
LFLAGS+=-s
endif

all: $(TARGET)

.c.o:
	@echo Compiling $*.c
	@$(CC) -c $(CFLAGS) -o $*.o $*.c

$(TARGET): $(OBJS)
	@echo Linking $@
	@$(CC) $(LFLAGS) -o $@ $(OBJS) -lsocket

install: all clean

clean:
	-del /n *.RES *.obj *.o os2\*.o ntlm\*.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

depend	Makefile.dep:
	@echo Making deps...
	@gcc -MM $(CFLAGS) $(SRCS) >Makefile.dep

include Makefile.dep
