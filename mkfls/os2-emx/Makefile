# $Id$
#
# Usage: make [DEBUG=1] [NOFORK=1] [PERL=1] [PERLDL=1]
#             [ZLIB=1] [BZLIB2=1] [ZLIBDL=1]
#
# Tested with perl 5.0053
#

CC=gcc
IMPLIB=emximp
DEFINES=-DHAVE_FORK -DHAVE_SNPRINTF -DHAVE_VSNPRINTF -DHAVE_GETOPT -DHAVE_UNISTD -DOS2=2 -DHAVE_IO_H -DHAVE_ARPA_INET_H -DHAVE_SYS_TIME_H -DHAVE_SYS_PARAM_H -DHAVE_SYS_IOCTL_H -DHAVE_DOS_H -DHAVE_STDARG_H -DEMX -DEMXSIGNALS -DHAVE_WAITPID -DHAVE_GETTIMEOFDAY -DHTTPS -DNTLM -DAMIGADOS_4D_OUTBOUND -DHAVE_NETINET_IN_H -DHAVE_NETDB_H -DHAVE_RESOLV_H -Du_int16_t=u_short -Du_int32_t=u_int
CFLAGS=$(DEFINES) -Wall -funsigned-char -Wno-char-subscripts
LFLAGS=-Los2
LIBS=-lsocket -lresolv
NTLM_SRC=ntlm/des_enc.c ntlm/helpers.c ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c os2/gettid.c os2/sem.c  ftndom.c ftnnode.c os2/getfree.c srif.c pmatch.c readflo.c prothlp.c iptools.c rfc2553.c run.c binlog.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c crypt.c srv_gai.c os2/ns_parse.c ${NTLM_SRC}
TARGET=binkd2e.exe

ifdef DEBUG
CFLAGS+=-g -DDEBUG
LFLAGS+=-g
else
CFLAGS+=-O2 -s
LFLAGS+=-O2 -s
endif

ifdef NOFORK
CFLAGS+=-DDEBUGCHILD
endif

ifdef PERLDL
PERLDIR=$(shell perl -MConfig -e "print $$Config{archlib}")
CFLAGS+= -Zmt -DWITH_PERL -DPERLDL -DDOSISH -DPERL_IS_AOUT -DEMBED -I$(PERLDIR)/CORE
LFLAGS+= -Zmt -Zcrtdll -Zstack 16000
SRCS+= perlhooks.c
else
ifdef PERL
PERLDIR=$(shell perl -MConfig -e "print $$Config{archlib}")
CFLAGS+= -DWITH_PERL -DDOSISH -DPERL_IS_AOUT -DEMBED -I$(PERLDIR)/CORE
LFLAGS+= -L$(PERLDIR)/CORE -Zsmall-conv -Zstack 16000
LIBS:=$(PERLDIR)/auto/DB_File/DB_File.a \
         $(PERLDIR)/auto/SDBM_File/SDBM_File.a \
         $(PERLDIR)/auto/Fcntl/Fcntl.a \
         $(PERLDIR)/auto/IO/IO.a \
         $(PERLDIR)/auto/POSIX/POSIX.a \
         $(PERLDIR)/auto/Socket/Socket.a \
         $(PERLDIR)/auto/OS2/Process/Process.a \
         $(PERLDIR)/auto/OS2/ExtAttr/ExtAttr.a \
         $(PERLDIR)/auto/OS2/REXX/REXX.a \
         $(PERLDIR)/auto/DynaLoader/DynaLoader.a \
         -llibperl -lm -lbsd -ldb $(LIBS)
SRCS+= perlhooks.c
else
CFLAGS+= -Zcrtdll
LFLAGS+= -Zcrtdll
endif
endif

ifdef ZLIB
COMPRESS=1
CFLAGS+= -DWITH_ZLIB=1
ifdef ZLIB_DIR
CFLAGS+= -I$(ZLIB_DIR)/include
endif
ifndef ZLIBDL
LIBS:=-lz_s $(LIBS)
ifdef ZLIB_DIR
LFLAGS+= -L$(ZLIB_DIR)/lib
endif
endif
endif

ifdef BZLIB2
COMPRESS=1
CFLAGS+= -DWITH_BZLIB2=1
ifdef BZLIB2_DIR
CFLAGS+= -I$(BZLIB2_DIR)/include
endif
ifndef ZLIBDL
LIBS:=-llibbz2_s $(LIBS)
ifdef BZLIB2_DIR
LFLAGS+= -L$(BZLIB2_DIR)/lib
endif
endif
endif

ifdef COMPRESS
SRCS+= compress.c
endif

ifdef ZLIBDL
SRCS+= zlibdl.c
CFLAGS+= -DZLIBDL=1
endif

.SUFFIXES: .imp .a
OBJS=${SRCS:.c=.o}

all: $(TARGET)

.c.o:
	@echo Compiling $*.c...
	@$(CC) -c $(CFLAGS) -o $*.o $*.c

.imp.a.:
	@echo Creating import library from $*.imp
	@$(IMPLIB) -o $*.a $*.imp

$(TARGET): $(OBJS) os2/resolv.a
	@echo Linking $@...
	@$(CC) -o $@ $(LFLAGS) $(OBJS) $(LIBS)

install: all clean

clean:
	-del /n *.RES *.obj *.o os2\*.o ntlm\*.o *.map *~ *.bak *.b *.ini *.err core >nul 2> nul

depend	Makefile.dep:
	@echo Build Makefile.dep...
	@gcc -MM $(CFLAGS) $(SRCS) > Makefile.dep

include Makefile.dep
