/*
 *  binkd.c -- binkd's main
 *
 *  binkd.c is a part of binkd project
 *
 *  Copyright (C) 1996-2003  Dima Maloff 5047/13 and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 2.66  2003/10/05 09:37:42  stas
 * Optimize binkd/nt start: use hack to determine if we're running as a service without waiting for the service control manager to fail
 *
 * Revision 2.65  2003/09/24 07:32:16  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.64  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.63  2003/09/12 07:37:57  val
 * compression support via zlib (preliminary)
 *
 * Revision 2.62  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.61  2003/08/28 01:28:52  hbrew
 * Fix typo
 *
 * Revision 2.60  2003/08/26 22:18:47  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.59  2003/08/26 21:01:09  gul
 * Fix compilation under unix
 *
 * Revision 2.58  2003/08/26 18:18:51  gul
 * Process "-?" commandline switch as "-h"
 *
 * Revision 2.57  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.56  2003/08/26 14:36:47  gul
 * Perl hooks in os2-emx
 *
 * Revision 2.55  2003/08/24 16:55:07  hbrew
 * Fix memory allocation for polls
 *
 * Revision 2.54  2003/08/21 15:40:34  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.53  2003/08/17 10:38:55  gul
 * Return semaphoring for log and binlog
 *
 * Revision 2.52  2003/08/16 09:08:32  gul
 * Binlog semaphoring removed
 *
 * Revision 2.51  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.50  2003/08/04 12:17:49  gul
 * Remove extra error message
 *
 * Revision 2.49  2003/07/19 06:59:34  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
 *
 * Revision 2.48  2003/07/17 04:32:16  hbrew
 * Fix "No prototype found for 'isdigit'" warning.
 *
 * Revision 2.47  2003/07/17 02:41:47  hbrew
 * Compability with nt/service.c & nt/win9x.c.
 * Usage "--service" options as win9x "run-as-service" flag.
 *
 * Revision 2.46  2003/07/16 15:50:44  stas
 * Fix: restore "Minimise to tray"
 *
 * Revision 2.45  2003/07/16 15:42:53  stas
 * Fix: restore -T option
 *
 * Revision 2.44  2003/07/16 15:08:49  stas
 * Fix NT services to use getopt(). Improve logging for service
 *
 * Revision 2.43  2003/07/07 10:39:24  gul
 * getopt() usage fix
 *
 * Revision 2.42  2003/07/07 10:13:49  gul
 * Use getopt() for commandline parse
 *
 * Revision 2.41  2003/07/07 08:33:25  val
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.40  2003/07/06 06:49:43  gul
 * Code cleanup
 *
 * Revision 2.39  2003/06/30 22:42:27  hbrew
 * Print only binkd name (without path) in error messages
 *
 * Revision 2.38  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.37  2003/06/13 03:10:07  hbrew
 * Correct standalone/service logging for binkd9x.
 *
 * Revision 2.36  2003/06/12 12:04:10  gul
 * Code cleanup
 *
 * Revision 2.35  2003/06/11 07:44:21  gul
 * Cosmetics
 *
 * Revision 2.34  2003/06/10 19:15:25  gul
 * Also one commandline parser fix
 *
 * Revision 2.32  2003/06/10 07:28:25  gul
 * Fix patch about commandline parsing
 *
 * Revision 2.31  2003/06/09 13:27:28  stas
 * New command line parser
 *
 * Revision 2.30  2003/05/10 00:30:36  hbrew
 * binkd9x: -u option now support '--all' service name (uninstall all services).
 * Unneeded spaces cleanup.
 *
 * Revision 2.29  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.28  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.27  2003/03/31 21:48:59  gul
 * Avoid infinite recursion
 *
 * Revision 2.26  2003/03/31 19:53:08  gul
 * Close socket before exit
 *
 * Revision 2.25  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.24  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.23  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.22  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.21  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.20  2003/03/05 13:21:50  gul
 * Fix warnings
 *
 * Revision 2.19  2003/03/04 07:27:11  gul
 * Put OS to log on start
 *
 * Revision 2.18  2003/03/03 09:43:07  gul
 * Cosmetics changes
 *
 * Revision 2.17  2003/03/01 17:42:32  gul
 * Added tzset() call to main() start
 *
 * Revision 2.16  2003/03/01 17:33:25  gul
 * Clean daemonize code
 *
 * Revision 2.15  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.14  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.13  2003/02/22 20:19:54  gul
 * Update copyrightes, 2002->2003
 *
 * Revision 2.12  2003/02/22 14:30:18  gul
 * Make nNod var static
 *
 * Revision 2.11  2003/02/21 16:47:57  gul
 * Remove <socks.h> file and USE_SOCKS macro
 *
 * Revision 2.10  2002/12/10 22:05:29  gul
 * binkd/win32 bugfix
 *
 * Revision 2.9  2002/11/13 07:49:27  gul
 * Fix typo
 *
 * Revision 2.8  2002/11/12 16:55:58  gul
 * Run as service under win9x
 *
 * Revision 2.7  2002/07/22 19:48:30  gul
 * Update usage
 *
 * Revision 2.6  2001/11/07 13:46:20  gul
 * bugfix (added saved_argc)
 *
 * Revision 2.5  2001/10/29 19:31:44  gul
 * Added -h option (print usage)
 *
 * Revision 2.4  2001/10/28 14:52:58  gul
 * Cyrillic filenames fix
 *
 * Revision 2.3  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.2  2001/05/23 15:44:09  gul
 * help-message fixed (-C option)
 *
 * Revision 2.1  2001/01/16 03:53:23  gul
 * Added -D switch (run as daemon)
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 * nsoveiko@doe.carleton.ca 1998/12/14
 * creation of pid_file is now logged and warnings produced
 *
 * Revision 1.15  1998/05/08  03:35:47  mff
 * Added -P switch, brushed up error msgs
 *
 * Revision 1.14  1997/11/03  06:10:39  mff
 * +nodes_init()
 *
 * Revision 1.13  1997/10/23  04:20:53  mff
 * pidfiles fixed, ...
 *
 * Revision 1.12  1997/06/16  05:52:07  mff
 * Added -C, copyright note
 *
 * Revision 1.10  1997/05/17  08:44:42  mff
 * Changed cmd line processing a bit
 *
 * Revision 1.9  1997/03/09  07:16:31  mff
 * Added command line parsing, support for inetd
 *
 * Revision 1.8  1997/02/07  06:42:59  mff
 * Under UNIXs SIGHUP forces binkd to restart
 *
 * Revision 1.7  1997/02/01  05:55:24  mff
 * Changed SIGCHLD support
 *
 * Revision 1.5  1996/12/14  07:00:32  mff
 * Now we use branch() and set_break_handlers()
 *
 * Revision 1.3  1996/12/07  12:26:37  mff
 * SOCKS support by msh
 *
 * Revision 1.1.1 1996/12/02  18:26:00  ufm
 *    Port to NT
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_FORK
#include <signal.h>
#include <sys/wait.h>
#endif

#include "readcfg.h"
#include "common.h"

#include "server.h"
#include "client.h"
#include "tools.h"
#include "bsy.h"
#include "protocol.h"
#include "setpttl.h"
#include "sem.h"

#ifdef HAVE_GETOPT
#include <unistd.h>
#else
#include "getopt.h"
#endif

#ifdef WITH_PERL
#include "perlhooks.h"
#endif

#if defined(ZLIBDL)
#include "zlibdl.h"
#endif

#ifdef UNIX
#include "unix/daemonize.h"
#endif

#ifdef WIN32
#include <windows.h>
#include "nt/service.h"
#ifdef BINKDW9X
#include "nt/win9x.h"
#endif
#endif

#ifdef HAVE_THREADS
MUTEXSEM hostsem = 0;
MUTEXSEM resolvsem = 0;
MUTEXSEM lsem = 0;
MUTEXSEM blsem = 0;
MUTEXSEM varsem = 0;
MUTEXSEM config_sem = 0;
EVENTSEM eothread = 0;
EVENTSEM exitcmgr = 0;
#ifdef OS2
MUTEXSEM fhsem = 0;
#endif
#endif

/*
 * Global variables
 */
int pidcmgr = 0;		       /* pid for clientmgr */
int pidCmgr = 0;		       /* real pid for clientmgr (not 0) */
int pidsmgr = 0;		       /* pid for server */
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
static SOCKET inetd_socket = 0;
#endif

char *configpath = NULL;               /* Config file name */

#ifdef HAVE_FORK

int mypid;

static void chld (int signo)
{
#include "reapchld.inc"
}

static void hup (int signo)
{
  Log (2, "got SIGHUP");
  /* !!! ToDo: safe reload of configurations (don't mess with regular checks!) */
}
#endif

void usage (void)
{
#if defined(BINKDW9X)
  AllocTempConsole();
#endif

  printf ("usage: binkd [-Cc"
#if defined(BINKD_DAEMONIZE)
          "D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "i"
#elif defined(WIN32)
	  "iu"
#if !defined(BINKDW9X)
          "T"
#endif
#endif
	  "pqrsvmh] [-P node]"
#if defined(WIN32)
	  " [-S name]"
#if defined(BINKDW9X)
          " [-t cmd]"
#endif
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
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "  -i       run from inetd\n"
#elif defined(BINKDW9X)
	  "  -i       install Win9x service\n"
	  "  -u       UNinstall Win9x service\n"
	  "  -t cmd   (start|stop|restart|status) service(s)\n"
	  "  -S name  set Win9x service name, all - use all services\n"
#elif defined(WIN32)
	  "  -T       minimize to Tray\n"
	  "  -i       install WindowsNT service\n"
	  "  -u       UNinstall WindowsNT service\n"
	  "  -S name  set WindowsNT service name\n"
#endif
	  "  -P node  poll a node\n"
	  "  -p       run client only, poll, quit\n"
	  "  -q       be quiet\n"
	  "  -r       disable crypt traffic\n"
	  "  -s       run server only\n"
	  "  -v       be verbose / dump version and quit\n"
	  "  -m       disable CRAM-MD5 authorization\n"
	  "  -h       print this help\n"
	  "\n"
	  "Copyright (c) 1996-2003 Dima Maloff and others.\n"
	  );

  puts ("\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
	  "the Free Software Foundation. See COPYING.\n"
	  "\n"
	  "Report bugs to 2:463/68 or binkd-bugs@happy.kiev.ua.");
  exit (1);
}

/* Command line flags */
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
int inetd_flag = 0;		       /* Run from inetd (-i) */
#endif
#ifdef BINKD_DAEMONIZE
int daemon_flag = 0;		       /* Run as daemon (-D) */
#endif
int server_flag = 0;		       /* Run servermgr (-s) */
int client_flag = 0;		       /* Run clientmgr (-c) */
int poll_flag = 0;		       /* Run clientmgr, make all jobs, quit
				        * (-p) */
int quiet_flag = 0;		       /* Be quiet (-q) */
int verbose_flag = 0;		       /* Be verbose / print version (-v) */
int checkcfg_flag = 0;		       /* exit(3) on config change (-C) */
int no_MD5 = 0;			       /* disable MD5 flag (-m) */
int no_crypt = 0;		       /* disable CRYPT (-r) */

static TYPE_LIST(maskchain) psPolls;   /* Create polls (-P) */

#ifdef WIN32
enum serviceflags service_flag = w32_noservice;  /* install, uninstall, start, stop, restart wnt/w9x service */
char *service_name = NULL;
#ifdef BINKDW9X
extern const char *Win9xStartService;  /* 'Run as win9x service' option */
#else
extern char *srvname;
#endif
#ifndef BINKDW9X
int tray_flag = 0;                     /* minimize to tray */
#endif
#endif

const char *optstring = "CchmP:pqrsv-:?"
#ifdef BINKD_DAEMONIZE
			"D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
			"i"
#endif
#if defined(WIN32)
#if !defined (BINKDW9X)
			"T"
#endif
#if defined(BINKDW9X)
			"t:"
#endif
			"iuS:"
#endif
			;

/* Parse and check command line parameters. Return config file name or NULL
 * On error prints usage information and exit (see usage() function)
 */
char *parseargs (int argc, char *argv[])
{
  char *cfgfile=NULL;
  int i, curind, rerun = 0;

  /* rerun will be removed when reconfig-on-fly feature will be added */
  if (getenv("BINKD_RERUN")) rerun = 1;

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
#if defined (BINKDW9X)
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
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	    case 'i':
	      inetd_flag = 1;
	      break;
#endif
#if defined(WIN32)
#if !defined (BINKDW9X)
	    case 'T':
	      tray_flag = 1;
	      break;
#endif

	    case 't': /* service control/query */
	      if (rerun) break;
              if ((service_flag != w32_noservice)) {
                Log (0, "%s: '-t' command line switch can't mixed with '-i', '-u' and other '-t'", extract_filename(argv[0]));
              }
	      if (!strcmp (optarg, "status"))
                service_flag = w32_queryservice;
	      else if (!strcmp (optarg, "start"))
                service_flag = w32_startservice;
              else if (!strcmp (optarg, "stop"))
                service_flag = w32_stopservice;
	      else if (!strcmp (optarg, "restart"))
                service_flag = w32_restartservice;
	      else
                Log (0, "%s: '-t': invalid argument '%s'", extract_filename(argv[0]), optarg);
              break;
	    case 'S':
	      if (service_name)
	        Log(0, "%s: '-S %s': service name specified before, can't overwrite!", extract_filename(argv[0]), optarg);
	      service_name = strdup (optarg);
	      break;
	    case 'i': /* install service */
	      if (rerun) break;
              if ( service_flag==w32_installservice
                || service_flag==w32_uninstallservice
                || service_flag==w32_queryservice
                || service_flag==w32_startservice
                || service_flag==w32_restartservice
                || service_flag==w32_stopservice
                 )
                Log (0, "%s: '-i' command line switch can't mixed with '-u', '-t' and other '-i'", extract_filename(argv[0]));
              if (!service_flag)
                service_flag = w32_installservice;
	      break;

	    case 'u': /* uninstall service */
	      if (rerun) break;
              if ( service_flag==w32_installservice
                || service_flag==w32_uninstallservice
                || service_flag==w32_queryservice
                || service_flag==w32_startservice
                || service_flag==w32_restartservice
                || service_flag==w32_stopservice
                 )
                Log (0, "%s: '-u' command line switch can't mixed with '-i', '-t' and other '-u'", extract_filename(argv[0]));
              if (!service_flag)
                service_flag = w32_uninstallservice;
	      break;
#endif

	    case 'P': /* create poll to node */
	      if (rerun) break;
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

#ifdef BINKD_DAEMONIZE
	    case 'D': /* run as unix daemon */
	      if (!rerun) daemon_flag = 1;
	      break;
#endif

	    default:  /* unknown parameter/option */
	      if (optopt != '?')
	      /* getopt() already print error message
	      Log (0, "%s: %s: unknown command line switch", extract_filename(argv[0]), argv[curind]);
	      */ exit(1);

	    case 'h': /* display command line help */
	      usage();

	  }
	curind = optind;
  }
  if (optind<argc)
    cfgfile = argv[optind++];
#ifdef OS2
  if (optind<argc)
  { if ((inetd_socket = atoi(argv[argc-1])) == 0 && !isdigit(argv[argc-1][0]))
      Log (0, "%s: bad socket number", argv[optind]);
#ifdef EMX
    if ((inetd_socket = _impsockhandle (inetd_socket, 0)) == -1)
      Log (0, "_impsockhandle: %s", strerror (errno));
#endif
  }
#endif
  if (optind<argc)
    Log (1, "Extra parameters ignored");

  return cfgfile;
}

#if defined(WIN32) && !defined(BINKDW9X)
int binkd_main (int argc, char *argv[], char *envp[]);
int main (int argc, char *argv[], char *envp[])
{ int res=-1;

  if( argc!=1 || (tell_start_ntservice()) )  /* See nt/service.c */
    res=binkd_main(argc,argv,envp); /* Running not as NT service */

  return res;
}
#endif

#if defined(WIN32)
int binkd_main (int argc, char *argv[], char *envp[])
#else
int main (int argc, char *argv[], char *envp[])
#endif
{
  char tmp[128];
#if defined(BINKD_DAEMONIZE)
  int  nochdir;
#endif
#if defined(HAVE_FORK)
  char **saved_argv;

  mypid = getpid();
  /* save argv as setproctitle() under some systems will change it */
  saved_argv = mkargv (argc, argv);

  configpath = parseargs(argc, saved_argv);
#else
  configpath = parseargs(argc, argv);
#endif

#ifdef WIN32
  if (service_flag==w32_installservice && !configpath)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
#ifdef BINKDW9X
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

#if defined(WIN32) && !defined(BINKDW9X)
  if (service_flag!=w32_noservice)
    if (service(argc, argv, envp) && service_flag!=w32_run_as_service) {
      Log(0, "Windows NT service error");
    }
  if (tray_flag)
     do_tray_flag();
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
  InitEventSem (&exitcmgr);
#ifdef OS2
  InitSem (&fhsem);
#endif

  /* Init for ftnnode.c */
  nodes_init ();

  if (configpath)
    readcfg (configpath);
  else if (verbose_flag)
  {
#if defined(WIN32) && defined(BINKDW9X)
    AllocTempConsole();
#endif

    printf ("Binkd " MYVER " (" __DATE__ " " __TIME__ "%s)\n", get_os_string ());
    exit (0);
  }
  else if (argc > 1)
    Log (0, "%s: invalid command line: config name must be specified", extract_filename(argv[0]));
  else
    usage ();

  print_args (tmp, sizeof (tmp), argv + 1);
#ifdef WIN32
#ifndef BINKDW9X
  if (isService()) {
    service_flag = w32_run_as_service;
    if (!service_name)
      service_name = strdup((const char *)srvname);
  }
#endif
  if (service_flag==w32_run_as_service)
    Log (4, "BEGIN service '%s', " MYNAME "/" MYVER "%s%s", service_name, get_os_string(), tmp);
  else
    Log (4, "BEGIN standalone, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#else
  Log (4, "BEGIN, " MYNAME "/" MYVER "%s%s", get_os_string(), tmp);
#endif
  if (sock_init ())
    Log (0, "sock_init: %s", TCPERR ());

  bsy_init ();
  rnd ();
  initsetproctitle (argc, argv, envp);
#ifdef WIN32
  SetFileApisToOEM();
#endif

  /* Set up break handler, set up exit list if needed */
  if (!set_break_handlers ())
    Log (0, "cannot install break handlers");

#if defined(WITH_ZLIB) && defined(ZLIBDL)
  if (!zlib_init("zlib.dll")) Log (3, "cannot load zlib.dll, GZ compression disabled");
#endif
#if defined(WITH_BZLIB2) && defined(ZLIBDL)
  if (!bzlib2_init("bzlib2.dll")) Log (3, "cannot load bzlib2.dll, BZ2 compression disabled");
#endif

#ifdef WITH_PERL
  if (current_config->perl_script[0]) {
    int ok = perl_init(current_config->perl_script, current_config);
    if (current_config->perl_strict && !ok)
      Log (0, "error parsing Perl script %s", current_config->perl_script);
  }
#endif

  if (!getenv("BINKD_RERUN")) putenv("BINKD_RERUN=1");

#ifdef HAVE_FORK
  signal (SIGCHLD, chld);
#endif

  { /* Create polls and release polls list */
    struct maskchain *psP;
    for (psP = psPolls.first; psP; psP = psP->next)
      poll_node (psP->mask, current_config);
    simplelist_free(&psPolls.linkpoint, destroy_maskchain);
  }

#if defined(UNIX) || defined(OS2) || defined(AMIGA)
  if (inetd_flag)
  {
    protocol (inetd_socket, 0, NULL, current_config);
    soclose (inetd_socket);
    exit (0);
  }
#endif

#ifdef BINKD_DAEMONIZE
  if (daemon_flag)
  {
	 if (!server_flag)
		 Log (0, "Only server can be run in the daemon mode");
	 else
	 {
		 if (argv[0][0] == '/')
			nochdir = 0;
		 else
		 {
			nochdir = 1;
		 	/* Log (6, "Run with relative path, will not chdir to /"); */
		 }

		 if (binkd_daemonize(nochdir) < 0)
		 Log (0, "Cannot daemonize");
	 }
  }
#endif

  if (client_flag && !server_flag)
  {
#if defined(HAVE_FORK)
    signal (SIGHUP, hup);
#endif
    clientmgr (0);
    exit (0);
  }

  pidsmgr = (int) getpid ();
  if (client_flag && (pidcmgr = branch (clientmgr, 0, 0)) < 0)
  {
    Log (0, "cannot branch out");
  }

#if defined(HAVE_FORK)
  signal (SIGHUP, hup);
#endif

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
