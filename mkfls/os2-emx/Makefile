# $Id$
#
CC=gcc
DEFINES=-DHAVE_FORK -DHAVE_SNPRINTF -DHAVE_VSNPRINTF -DHAVE_GETOPT -DHAVE_UNISTD -DOS2=2 -DHAVE_IO_H -DHAVE_ARPA_INET_H -DHAVE_SYS_TIME_H -DHAVE_SYS_PARAM_H -DHAVE_SYS_IOCTL_H -DHAVE_DOS_H -DHAVE_STDARG_H -DEMX -DEMXSIGNALS -DHAVE_WAITPID -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND
CFLAGS=$(DEFINES) -Wall -funsigned-char -Wno-char-subscripts
LFLAGS=
BASELIBS=-lsocket
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
BASESRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c os2/gettid.c os2/sem.c  ftndom.c ftnnode.c os2/getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c ${NTLM_SRC}

ifdef DEBUG
CFLAGS+=-g
LFLAGS+=-g
else
CFLAGS+=-O2 -s
LFLAGS+=-O2 -s
endif

ifdef PERL
#PERLDIR=`perl -MConfig -e "print \"PERLDIR=$Config{archlib}\""`
PERLDIR=C:/PERL5/LIB/5.00553/os2
CFLAGS+= -DWITH_PERL -DDOSISH -DPERL_IS_AOUT -DEMBED -I$(PERLDIR)/CORE
LFLAGS+= -L$(PERLDIR)/CORE -Zsmall-conv -Zstack 16000
PERLLIBS=$(PERLDIR)/auto/DB_File/DB_File.a \
         $(PERLDIR)/auto/SDBM_File/SDBM_File.a \
         $(PERLDIR)/auto/Fcntl/Fcntl.a \
         $(PERLDIR)/auto/IO/IO.a \
         $(PERLDIR)/auto/POSIX/POSIX.a \
         $(PERLDIR)/auto/Socket/Socket.a \
         $(PERLDIR)/auto/OS2/Process/Process.a \
         $(PERLDIR)/auto/OS2/ExtAttr/ExtAttr.a \
         $(PERLDIR)/auto/OS2/REXX/REXX.a \
         $(PERLDIR)/auto/DynaLoader/DynaLoader.a \
         -llibperl -lm -lbsd -ldb
PERLSRC=perlhooks.c
else
ifdef PERLDL
# Now segfault in perl_parse :(
PERLDIR=C:/PERL5/LIB/5.00553/os2
CFLAGS+= -DWITH_PERL -DPERLDL -DDOSISH -DPERL_IS_AOUT -DEMBED -I$(PERLDIR)/CORE
LFLAGS+= -Zsmall-conv -Zstack 16000
PERLSRC=perlhooks.c
else
CFLAGS+= -Zcrtdll
LFLAGS+= -Zcrtdll
endif
endif

LIBS=$(PERLLIBS) $(BASELIBS)
SRCS=$(BASESRCS) $(PERLSRC)

OBJS=${SRCS:.c=.o}

all: binkd.exe

.c.o:
	@echo Compiling $*.c...
	@$(CC) -c $(CFLAGS) -o $*.o $*.c

binkd.exe: $(OBJS)
	@echo Linking $@...
	@$(CC) -o $@ $(LFLAGS) $(OBJS) $(LIBS)

install: all clean

clean:
	-del /n *.RES *.obj *.o os2\*.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

depend	Makefile.dep:
	@echo Build Makefile.dep...
	@gcc -MM $(CFLAGS) $(SRCS) > Makefile.dep

include Makefile.dep
