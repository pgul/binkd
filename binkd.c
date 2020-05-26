/*
 *  binkd.c -- binkd's main
 *
 *  binkd.c is a part of binkd project
 *
 *  Copyright (C) 1996-2015  Dima Maloff 5047/13 and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#ifdef HAVE_FORK
#include <signal.h>
#include <sys/wait.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "server.h"
#include "client.h"
#include "tools.h"
#include "bsy.h"
#include "protocol.h"
#include "setpttl.h"
#include "sem.h"
#include "ftnnode.h"
#include "ftnaddr.h"
#include "rfc2553.h"
#include "srv_gai.h"

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
#include "getopt.h"
#endif

#ifdef WITH_PERL
#include "perlhooks.h"
#endif

#ifdef ZLIBDL
#include "compress.h"
#endif

#ifdef UNIX
#include "unix/daemonize.h"
#endif

#ifdef WIN32
#include "nt/service.h"
#include "nt/w32tools.h"
#ifdef BINKD9X
#include "nt/win9x.h"
#endif
#endif

#include "confopt.h"

#ifdef HAVE_THREADS
MUTEXSEM hostsem;
MUTEXSEM resolvsem;
MUTEXSEM lsem;
MUTEXSEM blsem;
MUTEXSEM varsem;
MUTEXSEM config_sem;
EVENTSEM eothread;
EVENTSEM wakecmgr;
#ifdef OS2
MUTEXSEM fhsem;
#endif
#endif

/*
 * Global variables
 */
int pidcmgr = 0;		       /* pid for clientmgr */
int pidCmgr = 0;		       /* real pid for clientmgr (not 0) */
int pidsmgr = 0;		       /* pid for server */
#ifdef WITH_PTHREADS
pthread_t servmgr_thread;
int tidsmgr;
#endif
static SOCKET inetd_socket_in = -1, inetd_socket_out = -1;
static char *remote_addr, *remote_node;

char *configpath = NULL;               /* Config file name */
char **saved_envp;

#ifdef HAVE_FORK

int mypid, got_sighup, got_sigchld;

void chld (int *childcount)
{
  int status;
  int pid;
  extern int pidcmgr;

  got_sigchld = 0;
#ifdef HAVE_WAITPID
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
#else
  if ((pid = (int) wait (&status)) > 0)
#endif
  {
    if (pidcmgr && pid == pidcmgr) {
      if (WIFSIGNALED(status))
        Log (0, "client manager (pid=%u) exited by signal %u", pid, WTERMSIG(status));
      else
        Log (0, "client manager (pid=%u) exited, retcode %u", pid, WEXITSTATUS(status));
      exit(4);
    }
    if (childcount)
      childcount[0]--;
    if (WIFSIGNALED(status))
      Log (2, "process %u exited by signal %u", pid, WTERMSIG(status));
    else
      Log (4, "rc(%u)=%u", pid, WEXITSTATUS(status));
  }
}

void sighandler(int signo)
{
  int old_errno = errno;

  switch (signo) {
    case SIGHUP:  got_sighup++;
#ifndef HAVE_THREADS
                  if (pidcmgr) kill(pidcmgr, SIGHUP);
#endif
                  break;
    case SIGCHLD: got_sigchld++;
                  break;
  }

#ifdef SYS5SIGNALS
  signal(signo, sighandler);
#endif
#ifdef EMXSIGNALS
  signal(signo, SIG_ACK);
#endif

  errno = old_errno;
}
#endif

#if defined(BLOCK_SIG)
void switchsignal(int how)
{
  sigset_t sigset;
  int old_errno = errno;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGCHLD);
  sigaddset(&sigset, SIGHUP);
  sigprocmask(how, &sigset, NULL);
  errno = old_errno;
}
#endif

void usage (void)
{
#if defined(BINKD9X)
  AllocTempConsole();
#endif

  printf ("usage: binkd [-Cc"
#if defined(BINKD_DAEMONIZE)
	  "D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "i"
#elif defined(WIN32) && !defined(BINKD9X)
	  "T"
#endif
	  "pqrsvmh] [-P node]"
#if defined(WIN32)
	  " [-S name] [-t cmd]"
#endif
	  " config"
#ifdef OS2
	  " [socket]"
#endif
	  "\n"
#ifdef BINKD_DAEMONIZE
	  "  -D       run as daemon\n"
#endif
	  "  -C       reload on config change\n"
	  "  -c       run client only\n"
	  "  -i       run server on stdin/stdout pipe (by inetd or other)\n"
	  "  -f node  run server protected session with this node\n"
	  "  -a ip    assume remote address when running with '-i' switch\n"
#if defined(BINKD9X)
	  "  -t cmd   (start|stop|restart|status|install|uninstall) service(s)\n"
	  "  -S name  set Win9x service name, all - use all services\n"
#elif defined(WIN32)
	  "  -T       minimize to Tray\n"
	  "  -t cmd   (start|stop|restart|status|install|uninstall) service\n"
	  "  -S name  set WindowsNT service name\n"
#endif
	  );
  printf ("  -P node  poll a node\n"
	  "  -p       run client only, poll, quit\n"
	  "  -q       be quiet\n"
	  "  -r       disable crypt traffic\n"
	  "  -s       run server only\n"
	  "  -v       be verbose / dump version and quit\n"
	  "  -vv      dump version with compilation flags and quit\n"
	  "  -d       dump parsed config and exit\n"
	  "  -m       disable CRAM-MD5 authorization\n"
	  "  -n       don't run binkd-client and binkd-server (check config, make polls)\n"
	  "  -h       print this help\n"
	  "\n"
	  "Copyright (c) 1996-2009 Dima Maloff and others.\n"
	  );

  puts ("\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
	  "the Free Software Foundation. See COPYING.\n"
	  "\n"
	  "Report bugs to 2:463/68 or binkd-bugs@happy.kiev.ua.");
  exit (1);
}

/* Environment variables in POSIX compliant */
#ifndef environ
extern char **environ;
#endif

/* Command line flags */
int inetd_flag = 0;		       /* Run from inetd (-i) */
#ifdef BINKD_DAEMONIZE
int daemon_flag = 0;		       /* Run as daemon (-D) */
#endif
int server_flag = 0;		       /* Run servermgr (-s) */
int client_flag = 0;		       /* Run clientmgr (-c) */
int poll_flag = 0;		       /* Run clientmgr, make all jobs, quit
				        * (-p) */
int quiet_flag = 0;		       /* Be quiet (-q) */
int verbose_flag = 0;		       /* Be verbose / print version (-v) */
int dumpcfg_flag = 0;		       /* Dump parsed config */
int checkcfg_flag = 0;		       /* exit(3) on config change (-C) */
int no_MD5 = 0;			       /* disable MD5 flag (-m) */
int no_crypt = 0;		       /* disable CRYPT (-r) */
int no_flag = 0;                       /* don't run client and server */

static TYPE_LIST(maskchain) psPolls;   /* Create polls (-P) */

#ifdef WIN32
enum serviceflags service_flag = w32_noservice;  /* install, uninstall, start, stop, restart wnt/w9x service */
char *service_name = NULL;
#ifdef BINKD9X
extern const char *Win9xStartService;  /* 'Run as win9x service' option */
#endif
#ifndef BINKD9X
int tray_flag = 0;                     /* minimize to tray */
#endif
#endif

const char *optstring = "CchmP:pqrsvd-:?n"
#ifdef BINKD_DAEMONIZE
			"D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
			"ia:f:"
#endif
#if defined(WIN32)
#if !defined (BINKD9X)
			"T"
#endif
			"t:iuS:"
#endif
			;

/* Parse and check command line parameters. Return config file name or NULL
 * On error prints usage information and exit (see usage() function)
 */
char *parseargs (int argc, char *argv[])
{
  char *cfgfile=NULL;
  int i, curind;

  curind = 1;
  while ((i = getopt(argc, argv, optstring)) != -1)
  {
	switch (i)
	  {
	    case '-':
	      /* GNU-style options */
	      if (!strcmp (argv[curind], "--help"))
		usage ();
	      else
#if defined (BINKD9X)
	      if (!strcmp (argv[curind], Win9xStartService))
	        service_flag = w32_run_as_service;
	      else
#endif
		Log (0, "%s: %s: unknown command line switch", extract_filename(argv[0]), argv[curind]);
	      break;
	    case 'C':
	      checkcfg_flag = 1;
	      break;
	    case 'c':
	      client_flag = 1;
	      break;
	    case 'i':
	      inetd_flag = 1;
	      break;
	    case 'a': /* remote IP address */
	      remote_addr = strdup(optarg);
	      break;
	    case 'f': /* remote FTN address */
	      remote_node = strdup(optarg);
	      break;
#if defined(WIN32)
#if !defined (BINKD9X)
	    case 'T':
	      tray_flag = 1;
	      break;
#endif

	    case 't': /* service control/query */
	      if (isService()) break;
	      if ((service_flag != w32_noservice)) {
	        Log (0, "%s: multiple '-t' switch", extract_filename(argv[0]));
	      }
	      if (!strcmp (optarg, "status"))
	        service_flag = w32_queryservice;
	      else if (!strcmp (optarg, "start"))
	        service_flag = w32_startservice;
	      else if (!strcmp (optarg, "stop"))
	        service_flag = w32_stopservice;
	      else if (!strcmp (optarg, "restart"))
	        service_flag = w32_restartservice;
	      else if (!strcmp (optarg, "install"))
	        service_flag = w32_installservice;
	      else if (!strcmp (optarg, "uninstall"))
	        service_flag = w32_uninstallservice;
	      else
	        Log (0, "%s: '-t': invalid argument '%s'", extract_filename(argv[0]), optarg);
	      break;
	    case 'S':
	      if (service_name)
	        Log(0, "%s: '-S %s': service name specified before, can't overwrite!", extract_filename(argv[0]), optarg);
	      service_name = strdup (optarg);
	      break;
#endif

	    case 'P': /* create poll to node */
	      {
	        struct maskchain new_entry;

	        new_entry.mask = xstrdup(optarg);
	        simplelist_add(&psPolls.linkpoint, &new_entry, sizeof(new_entry));
	      }
	      break;

	    case 'p': /* run clients and exit */
	      poll_flag = client_flag = 1;
	      break;

	    case 'q': /* quiet */
	      quiet_flag = 1;
	      break;

	    case 's': /* run server only */
	      server_flag = 1;
	      break;

	    case 'm': /* MD5 off */
	      no_MD5 = 1;
	      /* fallthrough */

	    case 'r': /* CRYPT off */
	      no_crypt = 1;
	      break;

	    case 'v': /* verbose */
	      ++verbose_flag;
	      break;

	    case 'd': /* dump cfg */
	      ++dumpcfg_flag;
	      break;

#ifdef BINKD_DAEMONIZE
	    case 'D': /* run as unix daemon */
	      daemon_flag = 1;
	      break;
#endif

	    default:  /* unknown parameter/option */
	      if (optopt != '?')
	      /* getopt() already print error message
	      Log (0, "%s: %s: unknown command line switch", extract_filename(argv[0]), argv[curind]);
	      */ exit(1);

	    case 'n':
	      no_flag = 1;
	      break;

	    case 'h': /* display command line help */
	      usage();

	  }
	curind = optind;
  }
  if (optind<argc)
    cfgfile = argv[optind++];
#ifdef OS2
  if (optind<argc)
  { if ((inetd_socket_in = atoi(argv[argc-1])) == 0 && !isdigit(argv[argc-1][0]))
      Log (0, "%s: bad socket number", argv[optind]);
#ifdef EMX
    if ((inetd_socket_in = _impsockhandle (inetd_socket_in, 0)) == -1)
      Log (0, "_impsockhandle: %s", strerror (errno));
#endif
    inetd_socket_out = inetd_socket_in;
    argc++;
  }
#endif
  if (optind<argc)
    Log (1, "Extra parameters ignored");

  return cfgfile;
}

#if defined(WIN32) && !defined(BINKD9X)
int binkd_main (int argc, char *argv[]);
int main (int argc, char *argv[])
{ int res=-1;

  if( argc!=1 || (tell_start_ntservice()) )  /* See nt/service.c */
    res=binkd_main(argc,argv); /* Running not as NT service */

  return res;
}
#endif

#if defined(WIN32)
int binkd_main (int argc, char *argv[])
#else
int main (int argc, char *argv[])
#endif
{
  char tmp[128];
#if defined(HAVE_FORK)
  char **saved_argv;

  mypid = getpid();
  /* save argv as setproctitle() under some systems will change it */
  saved_argv = mkargv (argc, argv);

  configpath = parseargs(argc, saved_argv);
#else
  configpath = parseargs(argc, argv);
#endif

  saved_envp = mkargv (-1, environ);

#ifdef WIN32
  if (service_flag==w32_installservice && !configpath)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
  w32Init();
#ifdef BINKD9X
  {
    int win9x_rc;

    win9x_rc = win9x_process(argc, argv);
    if (win9x_rc != -1)
      return win9x_rc;
  }
#endif
#endif

  tzset();

  if (poll_flag && server_flag)
    Log (0, "-p and -s cannot be used together");

#if defined(WIN32) && !defined(BINKD9X)
  if (service_flag!=w32_noservice)
    if (service(argc, argv, environ) && service_flag!=w32_run_as_service) {
      Log(0, "Windows NT service error");
    }
  if (tray_flag)
     do_tray_flag();
  else
  {
    atexit(UnloadBinkdIcon);
    LoadBinkdIcon();
  }
#endif

  /* No command line options: run both client and server */
  if (!client_flag && !server_flag)
    client_flag = server_flag = 1;

  InitSem (&hostsem);
  InitSem (&resolvsem);
  InitSem (&lsem);
  InitSem (&blsem);
  InitSem (&varsem);
  InitSem (&config_sem);
  InitEventSem (&eothread);
  InitEventSem (&wakecmgr);
#ifdef OS2
  InitSem (&fhsem);
#endif

  /* Init for ftnnode.c */
  nodes_init ();

  /* Needed for getaddrinfo() in find_port() */
  if (sock_init ())
    Log (0, "sock_init: %s", TCPERR ());

  if (configpath)
  {
    current_config = readcfg (configpath);
    if (!current_config)
      Log (0, "error in configuration, aborting");
    if (dumpcfg_flag)
    {
      debug_readcfg ();
      exit(0);
    }
    InitLog(current_config->loglevel, current_config->conlog,
            current_config->logpath, current_config->nolog.first);
  }
  else if (verbose_flag)
  {
#if defined(WIN32) && defined(BINKD9X)
    AllocTempConsole();
#endif

    printf ("Binkd " MYVER " (" __DATE__ " " __TIME__ "%s)\n", get_os_string ());
    if (verbose_flag>1)
    {
      printf ("Compilation flags: " _DBNKD ".\n");
      printf ("Facilities:"
#ifndef srv_getaddrinfo
              " fts5004"
#endif
#ifndef HAVE_GETADDRINFO
              " rfc2553emu"
#else
              " ipv6"
#endif
              "\n");
    }
    exit (0);
  }
  else if (argc > 1)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
  else
    usage ();

  print_args (tmp, sizeof (tmp), argv + 1);
#ifdef WIN32
  if (service_flag==w32_run_as_service)
    Log (4, "BEGIN service '%s', " MYNAME "/" MYVER "%s%s", service_name, get_os_string(), tmp);
  else
    Log (4, "BEGIN standalone, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#else
  Log (4, "BEGIN, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#endif

  bsy_init ();
  rnd ();
  initsetproctitle (argc, argv, environ);
#ifdef WIN32
  SetFileApisToOEM();
#endif

  /* Set up break handler, set up exit list if needed */
  if (!set_break_handlers ())
    Log (0, "cannot install break handlers");

#if defined(SIGPIPE)
  signal(SIGPIPE, SIG_IGN);
#endif

#if defined(WITH_ZLIB) && defined(ZLIBDL)
  if (current_config->zlib_dll[0]) {
    if (!zlib_init(current_config->zlib_dll))
      Log (2, "cannot load %s, GZ compression disabled", current_config->zlib_dll);
    else
      Log (6, "%s loaded successfully", current_config->zlib_dll);
  } else
    Log (current_config->zrules.first ? 3 : 5, "zlib-dll not defined, GZ compression disabled");
#endif
#if defined(WITH_BZLIB2) && defined(ZLIBDL)
  if (current_config->bzlib2_dll[0]) {
    if (!bzlib2_init(current_config->bzlib2_dll))
      Log (2, "cannot load %s, BZ2 compression disabled", current_config->bzlib2_dll);
    else
      Log (6, "%s loaded successfully", current_config->bzlib2_dll);
  } else
    Log (current_config->zrules.first
#ifdef WITH_ZLIB
         && !zlib_loaded
#endif
         ? 3 : 5, "bzlib2-dll not defined, BZ2 compression disabled");
#endif

#ifdef WITH_PERL
  if (current_config->perl_script[0]) {
    if (!perl_init(current_config->perl_script, current_config, 1)) {
      if (current_config->perl_strict)
        Log (0, "error parsing Perl script %s", current_config->perl_script);
    } else {
      perl_on_start(current_config);
      perl_config_loaded(current_config);
    }
  }
#endif

#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
  signal (SIGCHLD, sighandler);
#endif

  { /* Create polls and release polls list */
    struct maskchain *psP;
    for (psP = psPolls.first; psP; psP = psP->next)
      poll_node (psP->mask, current_config);
    simplelist_free(&psPolls.linkpoint, destroy_maskchain);
  }

  if (no_flag)
    Log (0, "Exit on option '-n'");

  if (inetd_flag)
  {
    FTN_ADDR ftn_addr, *pftn_addr;
    int tempfd;

    pftn_addr = NULL;
    if (remote_node)
    {
      if (parse_ftnaddress (remote_node, &ftn_addr, current_config->pDomains.first))
      {
        char szFTNAddr[FTN_ADDR_SZ + 1];

        exp_ftnaddress (&ftn_addr, current_config->pAddr, current_config->nAddr, current_config->pDomains.first);
        pftn_addr = &ftn_addr;
        ftnaddress_to_str (szFTNAddr, pftn_addr);
        Log (3, "Session with %s", szFTNAddr);
      }
      else
        Log (1, "`%s' cannot be parsed as a Fido-style address", remote_node);
    }
    if (!remote_addr)
    {
      char *p = getenv("SSH_CONNECTION");

      if (p)
      {
	remote_addr = strdup(p);
	p = strchr(remote_addr, ' ');
	if (p) *p = '\0';
      }
    }
    /* not using stdin/stdout itself to avoid possible collisions */
    if (inetd_socket_in == -1)
      inetd_socket_in = dup(fileno(stdin));
    if (inetd_socket_out == -1)
      inetd_socket_out = dup(fileno(stdout));
#ifdef UNIX
    tempfd = open("/dev/null", O_RDWR);
#else
    tempfd = open("nul", O_RDWR);
#endif
    if (tempfd != -1)
    {
      dup2(tempfd, fileno(stdin));
      dup2(tempfd, fileno(stdout));
      close(tempfd);
    }
    protocol (inetd_socket_in, inetd_socket_out, NULL, pftn_addr, NULL, NULL, remote_addr, current_config);
    soclose (inetd_socket_out);
    exit (0);
  }

#ifdef BINKD_DAEMONIZE
  if (daemon_flag)
  {
    if (binkd_daemonize(1) < 0)
      Log (0, "Cannot daemonize");
#ifndef HAVE_THREADS
    else
      mypid = getpid();
#endif
  }
#endif

#if defined(HAVE_FORK)
  signal (SIGHUP, sighandler);
#endif

  if (client_flag && !server_flag)
  {
    clientmgr (0);
    exit (0);
  }

  pidsmgr = (int) getpid ();
#ifdef WITH_PTHREADS
  servmgr_thread = pthread_self();
  tidsmgr = PID();
#endif
  if (client_flag && (pidcmgr = branch (clientmgr, 0, 0)) < 0)
  {
    Log (0, "cannot branch out");
  }

  if (*current_config->pid_file)
  {
    if ( unlink (current_config->pid_file) == 0 ) /* successfully unlinked, i.e.
	                                            an old pid_file was found */
	Log (1, "unexpected pid_file: %s: unlinked", current_config->pid_file);
    else
    {
	int current_log_level = 1;
	switch ( errno )
	{
	   case ENOENT :	/* file not found or null pathname */
		current_log_level = 8; /* it's ok */
		break;
	   default :
		break;
	}
	Log (current_log_level, "unlink_pid_file: %s: %s", current_config->pid_file, strerror (errno));
    }
    create_sem_file (current_config->pid_file, 1);
  }

  servmgr ();

  return 0;
}
