#
CC=gcc
DEFINES=-DHAVE_FORK -DAMIGA -DHAVE_SNPRINTF -DHAVE_UNISTD_H -DOS="\"Amiga\"" -DHAVE_WAITPID
CFLAGS=$(DEFINES) -Wall -resident -O
SRCS=binkd.c readcfg.c tools.c ftnaddr.c ftnq.c client.c server.c protocol.c bsy.c inbound.c breaksig.c branch.c amiga/rename.c amiga/getfree.c ftndom.c ftnnode.c srif.c pmatch.c readflo.c prothlp.c iptools.c run.c binlog.c amiga/sem.c exitproc.c getw.c xalloc.c setpttl.c https.c md5b.c
OBJS=binkd.o readcfg.o tools.o ftnaddr.o ftnq.o client.o server.o protocol.o bsy.o inbound.o breaksig.o branch.o rename.o       getfree.o       ftndom.o ftnnode.o srif.o pmatch.o readflo.o prothlp.o iptools.o run.o binlog.o sem.o       exitproc.o getw.o xalloc.o setpttl.o https.o md5b.o
all: binkd
.c.o:
	$(CC) -c $(CFLAGS) $*.c
binkd: $(OBJS)
	$(CC) $(CFLAGS) -o binkd $(OBJS)
install: all clean
clean:
	rm -f *.[bo] *.BAK *.core *.obj *.err *~ core
depend Makefile.dep:   Makefile
	  $(CC) -MM $(CFLAGS) $(SRCS) $(SYS) | \
	      awk '{ if ($$1 != prev) { if (rec != "") print rec; \
		  rec = $$0; prev = $$1; } \
		  else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		  else rec = rec " " $$2 } } \
		  END { print rec }' | tee Makefile.dep
rename.o:
	$(CC) -c $(CFLAGS) amiga/rename.c
getfree.o:
	$(CC) -c $(CFLAGS) amiga/getfree.c
sem.o:
	$(CC) -c $(CFLAGS) amiga/sem.c
include Makefile.dep
