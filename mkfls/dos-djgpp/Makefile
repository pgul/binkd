# $Id$
#

# Usage: make [DEBUG=1] [BINKD9X=1] [PERL=1] [PERLDL=1]
#	      [ZLIB=1] [BZLIB2=1] [ZLIBDL=1] [DEBUGCHILD=1]
#	      [IPV6=1] [FTS5004=1] [BW_LIM=1] [AF_FORCE=1]
#
#

#PERL_BASE=c:/Perl
#PERL_LIB=perl58

#ZLIB_BASE=../zlib
#ZLIB_LIB=z
#BZLIB2_BASE=../bzlib2
#BZLIB2_LIB=bz2

#WATT_ROOT=../watt32
#WATT_LIB=watt

# =============================================================================

# WINDRES?=windres

OUTDIR=bin/dos
BINKDNAME=binkd-dos-djgpp
BINKDTYPE=djgpp

CC=gcc
CFLAGS+= -funsigned-char \
	 -Wall -Wno-char-subscripts -Wno-comment \
	 -I $(WATT_ROOT)\inc

CDEFS+=  -DDOS -DHAVE_UNISTD -DHAVE_INTTYPES_H -DHAVE_INTMAX_T \
	 -DHAVE_IO_H -DHAVE_DOS_H -DHAVE_STDARG_H -DHAVE_NETINET_IN_H -DHAVE_NETDB_H  \
	 -DHTTPS -DNTLM -DHAVE_SNPRINTF -DHAVE_VSNPRINTF -DHAVE_ARPA_INET_H \
	 -DWATT32_NO_OLDIES -DHAVE_SOCKLEN_T




SRCS= binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c \
      bsy.c inbound.c branch.c ftndom.c ftnnode.c srif.c pmatch.c readflo.c   \
      prothlp.c iptools.c rfc2553.c run.c binlog.c exitproc.c getw.c xalloc.c \
      setpttl.c https.c md5b.c crypt.c getopt.c  \
      snprintf.c breaksig.c dos/getfree.c dos/sleep.c \
      ntlm/ecb_enc.c ntlm/md4_dgst.c ntlm/set_key.c ntlm/des_enc.c	      \
      ntlm/helpers.c

LIBS+= -L $(WATT_ROOT)\lib -l$(WATT_LIB)

#RES=  nt/binkdres.rc

OBJDIR= $(OUTDIR)/obj
MAKEDIRSONLY= $(OUTDIR)/obj $(OBJDIR)/ntlm $(OBJDIR)/dos
MAKEDIRS= $(OUTDIR) $(MAKEDIRSONLY)

# debug +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef DEBUG
CFLAGS+= -g
CDEFS+= -DDEBUG -D_DEBUG
BINKDNAME:=$(BINKDNAME)-debug
OUTDIR:=$(OUTDIR)-debug
BINKDTYPE:=$(BINKDTYPE), debug
else
CFLAGS+= -s -O2
endif
# debug (DEBUG) ---------------------------------------------------------------

# debugchild, nofork ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef DEBUGCHILD
CDEFS+= -DDEBUGCHILD
BINKDNAME:=$(BINKDNAME)-debugchild
OUTDIR:=$(OUTDIR)-debugchild
BINKDTYPE:=$(BINKDTYPE), debugchild
endif
# debugchild, nofork ----------------------------------------------------------

# bandwidth limiting ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef BW_LIM
CDEFS+= -DBW_LIM=1
endif
# bandwidth limiting ----------------------------------------------------------

# soft AF force +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef AF_FORCE
CDEFS+= -DAF_FORCE=1
endif
# soft AF force ---------------------------------------------------------------

# perl support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# PERLDL implies PERL
ifdef PERLDL
ifndef PERL
PERL=1
endif
endif

ifdef PERL

ifndef PERL_BASE
PERL_BASE=${shell perl -mConfig -e 'print $$Config::Config{prefix};'}
endif

ifdef PERL_BASE
PERL_INCL=$(PERL_BASE)/lib/CORE
else
PERL_INCL=${shell perl -mConfig -e '$$_ = $$Config::Config{libpth}; s!^\"!!; s!\"$$!!; !s!\\!/!g; print;'}
endif

CDEFS+= -DWITH_PERL -DNDEBUG -DNO_STRICT -DHAVE_DES_FCRYPT -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS -DPERL_MSVCRT_READFIX
SRCS+= perlhooks.c
C_DIRS+= -idirafter "$(PERL_INCL)"

ifdef PERLDL
CDEFS+= -DPERLDL
CFLAGS+= -fno-strict-aliasing
BINKDNAME:=$(BINKDNAME)-perldl
OUTDIR:=$(OUTDIR)-perldl
BINKDTYPE:=$(BINKDTYPE), perldl
else

ifndef PERL_LIB
PERL_LIB=${shell perl -mConfig -e '($$_ = $$Config::Config{libperl}) =~ s!\.lib$$!!i; print;'}
endif

L_DIRS+= -L"$(PERL_INCL)"
LIBS+= -l$(PERL_LIB)
BINKDNAME:=$(BINKDNAME)-perl
OUTDIR:=$(OUTDIR)-perl
BINKDTYPE:=$(BINKDTYPE), perl
endif
endif
# perl support ----------------------------------------------------------------

# zlib support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef ZLIB
CDEFS+= -DWITH_ZLIB
ZLIB_BZLIB2=1

ifdef ZLIB_BASE
C_DIRS+= -I "$(ZLIB_BASE)"
L_DIRS+= -L "$(ZLIB_BASE)"
endif

ifndef ZLIBDL
ifndef ZLIB_LIB
ZLIB_LIB=z
endif
LIBS+= -l"$(ZLIB_LIB)"
BINKDNAME:=$(BINKDNAME)-zlib
OUTDIR:=$(OUTDIR)-zlib
BINKDTYPE:=$(BINKDTYPE), zlib
else
BINKDNAME:=$(BINKDNAME)-zlibdl
OUTDIR:=$(OUTDIR)-zlibdl
BINKDTYPE:=$(BINKDTYPE), zlibdl
endif
endif
# zlib support ----------------------------------------------------------------

# bzlib2 support ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef BZLIB2
CDEFS+= -DWITH_BZLIB2
ZLIB_BZLIB2=1

ifdef BZLIB2_BASE
C_DIRS+= -I "$(BZLIB2_BASE)"
L_DIRS+= -L "$(BZLIB2_BASE)"
endif

ifndef ZLIBDL
ifndef BZLIB2_LIB
BZLIB2_LIB=bz2
endif
LIBS+= -l"$(BZLIB2_LIB)"
BINKDNAME:=$(BINKDNAME)-bzlib2
OUTDIR:=$(OUTDIR)-bzlib2
BINKDTYPE:=$(BINKDTYPE), bzlib2
else
BINKDNAME:=$(BINKDNAME)-bzlib2dl
OUTDIR:=$(OUTDIR)-bzlib2dl
BINKDTYPE:=$(BINKDTYPE), bzlib2dl
endif
endif
# bzlib2 support --------------------------------------------------------------

# zlib & bzlib2 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ifdef ZLIB_BZLIB2
ifdef ZLIBDL
CDEFS+= -DZLIBDL
SRCS+= zlibdl.c
endif
SRCS+= compress.c
endif
# zlib & bzlib2 ---------------------------------------------------------------

BINKDEXE = $(BINKDNAME).exe

OBJS=$(addprefix $(OBJDIR)/,$(patsubst %.c,%.o, $(SRCS)))
RESOBJS=$(addprefix $(OBJDIR)/, $(patsubst %.rc,%.o, $(RES)))

.PHONY: all printinfo install html clean distclean makedirs

all: printinfo makedirs $(OUTDIR)/$(BINKDEXE)

printinfo:
	@echo "-----------------------------------------------------------"
	@echo "binkd type : $(BINKDTYPE)"
	@echo "output dir : $(OUTDIR)"
	@echo "binkd exe  : $(BINKDEXE)"
	@echo "DEPS : $(DEPS)"
ifdef PERL
	@echo "perlincl   : $(PERL_INCL)"
ifndef PERLDL
	@echo "perllib	  : $(PERL_LIB)"
endif
endif
ifdef ZLIB
ifdef ZLIB_BASE
	@echo "zlibincl   : $(ZLIB_BASE)"
endif
ifndef ZLIBDL
	@echo "zliblib	  : $(ZLIB_LIB)"
endif
endif
ifdef BZLIB2
ifdef BZLIB2_BASE
	@echo "bzlib2incl : $(BZLIB2_BASE)"
endif
ifndef ZLIBDL
	@echo "bzlib2lib  : $(BZLIB2_LIB)"
endif
endif
	@echo "-----------------------------------------------------------"

$(OBJDIR)/%.o: %.c
	@echo Compiling $<...
	@$(CC) -c -MMD $(CFLAGS) $(CDEFS) $(C_DIRS) -o $@ $<

$(OBJDIR)/%.o: %.rc
	@echo Compiling $<...
	$(WINDRES) $(CDEFS) -D"BINKDNAME=$(BINKDNAME)" -D"BINKDEXE=$(BINKDEXE)" $< $@

$(OUTDIR)/$(BINKDEXE): $(OBJS) $(RESOBJS)
	@echo Linking $(BINKDEXE)...
	@$(CC) $(CFLAGS) $(LFLAGS) $(L_DIRS) -o $@ $(OBJS) $(RESOBJS) $(LIBS)

makedirs: $(MAKEDIRS)

$(MAKEDIRS):
	@echo Making directory $@...
	@mkdir $@

install: all clean

html: binkd.html

binkd.html: binkd.8
	groff -Thtml -mman binkd.8 >$(OUTDIR)/binkd.html

#
# Cleanup

CLEANEXT:= *.o *.d

clean:
	@echo Cleanup...
	del /f /q $(foreach dir,$(MAKEDIRSONLY),$(foreach ext,$(CLEANEXT),$(dir)\\$(ext)))

distclean:
	@echo Full cleanup...
	rmdir /s /q $(OUTDIR)

#
# Dependencies (idea taken from zlib makefile)

DEPS:=$(foreach dir,$(MAKEDIRSONLY),$(wildcard $(dir)/*.d))

ifneq ($(DEPS),)
include $(DEPS)
endif

# Dependencies for resourses
$(RESOBJS): Config.h confopt.h
