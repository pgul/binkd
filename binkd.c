/*
 *  binkd.c -- binkd's main
 *
 *  binkd.c is a part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include "Config.h"
#include "sys.h"
#include "iphdr.h"
#include "readcfg.h"
#include "server.h"
#include "client.h"
#include "tools.h"
#include "bsy.h"
#include "protocol.h"
#include "assert.h"
#include "binlog.h"
#include "setpttl.h"
#include "daemonize.h"

#ifdef WIN32
#include "nt/service.h"
#endif

#ifdef USE_SOCKS
#include "socks.h"
#endif

#ifdef HAVE_THREADS
#include "sem.h"
MUTEXSEM hostsem = 0; 
#ifdef OS2
MUTEXSEM fhsem = 0;
#endif
#endif

/*
 * Global variables
 */
int pidcmgr = 0;		       /* pid for clientmgr */
int pidsmgr = 0;		       /* pid for server */
SOCKET inetd_socket = 0;

#ifdef HAVE_FORK
static void chld (int signo)
{
#include "reapchld.inc"
}
#endif

#if defined(UNIX) || defined(AMIGA) || (defined(OS2) && defined(HAVE_FORK))
jmp_buf jb;
static void hup (int signo)
{
  Log (2, "got SIGHUP");
  longjmp (jb, 1);
}
#endif

void usage ()
{
#if defined(WIN32)	
	char *s=NULL;
	switch(checkservice())
	{
	case 1: s="i\0  -i       install WindowsNT service\n"; break;
	case 2: s="u\0  -u       UNinstall WindowsNT service\n"; break;
	}
#endif

  printf ("usage: binkd [-Cc"
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
          "D"
#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "i"
#elif defined(WIN32)
	  "%s"
#endif
	  "pqsv] [-Pnode] config"
#ifdef OS2
	  " [socket]"
#endif
	  "\n"
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
	  "  -D       run as daemon\n"
#endif
	  "  -C       exit(3) on config change\n"
	  "  -c       run client only\n"
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
	  "  -i       run from inetd\n"
#elif defined(WIN32)
	  "%s"
#endif
	  "  -P node  poll a node\n"
	  "  -p       run client only, poll, quit\n"
	  "  -q       be quiet\n"
	  "  -s       run server only\n"
	  "  -v       be verbose / dump version and quit\n"
	  "  -m       disable CRAM-MD5 authorization\n"
	  "\n"
	  "Copyright (c) 1996-2000 Dima Maloff and others.\n"
	  "\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
	  "the Free Software Foundation. See COPYING.\n"
	  "\n"
	  "Report bugs to 2:463/68 or binkd-bugs@happy.kiev.ua.\n"
#if defined(WIN32)	
	  ,s?s:"", s?s+2:""
#endif
	  );
  exit (1);
}

/* Command line flags */
int inetd_flag = 0;		       /* Run from inetd (-i) */
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
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

extern int nNod;

int main (int argc, char *argv[], char *envp[])
{
  char tmp[128];
  int i;
/* Config file name */
  char *config = NULL;
  char **saved_argv;
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
  int  nochdir;
#endif

#ifdef WIN32
  service(argc, argv, envp);
#endif
  /* save argv as setproctitle() under some systems will change it */
  saved_argv = mkargv (argc, argv);

  for (i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      char *s = argv[i] + 1;

      do
      {
	switch (*s)
	  {
	    case '-':
	      /* GNU-style options */
	      if (!strcmp (s + 1, "help"))
		usage ();
	      else
		Log (0, "%s: --%s: unknown command line switch", argv[0], s + 1);
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
	    case 'P':
	      if (argv[i][2] == 0)
	      {
		++i;
		if (argv[i] == 0)
		  Log (0, "%s: -P: missing requred argument", argv[0]);
	      }
	      goto BREAK_WHILE;
	    case 'p':
	      poll_flag = client_flag = 1;
	      break;
	    case 'q':
	      quiet_flag = 1;
	      break;
	    case 's':
	      server_flag = 1;
	      break;
	    case 'm':
	      no_MD5 = 1;
	      break;
	    case 'v':
	      ++verbose_flag;
	      break;
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
	    case 'D':
	      daemon_flag = 1;
	      /* remove this switch from saved_argv */
	      { int j;
	        free(saved_argv[i]);
	        for (j=i; j<argc; j++)
	          saved_argv[j]=saved_argv[j+1];
	      }
	      break;
#endif
	    default:
	      Log (0, "%s: -%c: unknown command line switch", argv[0], *s);
	    case 0:
	      usage ();
	  }
	++s;
      }
      while (*s);
  BREAK_WHILE:;
    }
    else if (!config)
    {
      config = argv[i];
    }
  }

  if (poll_flag && server_flag)
    Log (0, "-p and -s cannot be used together");

  /* No command line options: run both client and server */
  if (!client_flag && !server_flag)
    client_flag = server_flag = 1;

#if defined(UNIX) || defined(OS2) || defined(AMIGA)
  if (inetd_flag)
  {
    inetd_socket = 0;
#ifdef OS2
    if ((inetd_socket = atoi (argv[argc - 1])) == 0)
      Log (0, "%s: bad socket number", argv[argc - 1]);
#endif
#ifdef EMX
    if ((inetd_socket = _impsockhandle (inetd_socket, 0)) == -1)
      Log (0, "_impsockhandle: %s", strerror (errno));
#endif
  }
#endif

  if (verbose_flag >= 3)
    debugcfg = 1;

  /* Init for ftnnode.c */
  nodes_init ();

  if (config)
    readcfg (config);
  else if (verbose_flag)
  {
    printf ("Binkd " MYVER " (" __DATE__ " " __TIME__ "%s)\n", get_os_string ());
    exit (0);
  }
  else if (argc == 1)
    usage ();
  else
  {
    Log (0, "%s: invalid command line: config name must be specified", argv[0]);
    exit (1);
  }

  if (quiet_flag)
  {
    percents = 0;
    conlog = 0;
    printq = 0;
  }
  switch (verbose_flag)
    {
      case 0:
	break;
      case 1:
	percents = printq = 1;
	loglevel = conlog = 4;
	break;
      case 2:
      case 3:
      default:
	percents = printq = 1;
	loglevel = conlog = 6;
	break;
    }

  print_args (tmp, sizeof (tmp), argc - 1, argv + 1);
  Log (4, "BEGIN, " MYNAME "/" MYVER "%s", tmp);

  if (sock_init ())
    Log (0, "sock_init: %s", TCPERR ());

  bsy_init ();
  BinLogInit ();
  rnd ();
  initsetproctitle (argc, argv, envp);

  /* Set up break handler, set up exit list if needed */
  if (!set_break_handlers ())
    Log (0, "cannot install break handlers");

#ifdef HAVE_FORK
  signal (SIGCHLD, chld);
#else
  InitSem (&hostsem);
#ifdef OS2
  InitSem (&fhsem);
#endif
#endif

  for (i = 1; i < argc; ++i)
    if (argv[i][0] == '-' && argv[i][1] == 'P')
    {
      if (argv[i][2] == 0)
	poll_node (argv[++i]);
      else
	poll_node (argv[i] + 2);
    }

#if defined(UNIX) || defined(OS2) || defined(AMIGA)
  if (inetd_flag)
  {
    protocol (inetd_socket, 0);
    soclose (inetd_socket);
    exit (0);
  }
#endif

#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)
  if (daemon_flag)
  {
	 if (!server_flag)
		 Log (0, "Only server can be run in the daemon mode");
	 else
	 {
		 if (saved_argv[0][0] == '/')
			nochdir = 0;
		 else
		 {
			nochdir = 1;
		 	// Log (6, "Run with relative path, will not chdir to /");
		 }

		 if (binkd_daemonize(nochdir) < 0)
		 Log (0, "Cannot daemonize");
	 }
  }
#endif

  if (client_flag && !server_flag)
  {
#if defined(UNIX) || defined(AMIGA) || (defined(OS2) && defined(HAVE_FORK))
    if (setjmp(jb))
      goto binkdrestart;
    else
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

#if defined(UNIX) || defined(AMIGA) || (defined(OS2) && defined(HAVE_FORK))
  if (setjmp (jb))
  {
    extern SOCKET sockfd;

    Log (5, "Closing socket # %i", sockfd);
    soclose (sockfd);
binkdrestart:
    exitfunc();
    print_args (tmp, sizeof (tmp), argc - 1, saved_argv + 1);
    Log (2, "exec %s%s", saved_argv[0], tmp);
    if (execv (saved_argv[0], saved_argv) == -1)
      Log (1, "execv: %s", strerror (errno));
  }
  else
  {
    signal (SIGHUP, hup);
  }
#endif

  if (*pid_file)
  {
    if ( unlink (pid_file) == 0 ) /* successfully unlinked, i.e.
                                     an old pid_file was found */
	Log (1, "unexpected pid_file: %s: unlinked", pid_file);
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
	Log (current_log_level, "unlink_pid_file: %s: %s", pid_file, strerror (errno));
    }
    if ( create_sem_file (pid_file) == 0 ) /* could not create pid_file */
	if (loglevel < 5) /* not logged in create_sem_file() */
	    Log (1, "create_sem_file: %s: %s", pid_file, strerror (errno));
  }

  servmgr (0);

  return 0;
}
