/*
 *  srif.c -- Create flags or run external programs on mail events
 *
 *  srif.c is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
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
 * Revision 2.1  2001/10/27 07:53:46  gul
 * Unlink req-file after run freq-processor
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.4  1997/06/16  05:40:33  mff
 * Binkd will not complain about missing .rsp files when running
 * programms with "exec" statement without using of *S macro.
 *
 * Revision 1.3  1997/03/28  06:12:48  mff
 * Changes to support SRIF: + evt_run(), etc.
 *
 * Revision 1.2  1997/02/09  04:17:30  mff
 * Removed error msgs
 *
 * Revision 1.1  1997/01/08  03:57:54  mff
 * Initial revision
 *
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "Config.h"
#include "ftnaddr.h"
#include "ftnq.h"
#include "tools.h"
#include "srif.h"
#include "run.h"
#include "tools.h"

EVT_FLAG *evt_flags = 0;

/*
 * Tests if filename matches any of EVT_FLAG's patterns.
 */
int evt_test (char *filename0)
{
  EVT_FLAG *curr;
  char filename[MAXPATHLEN + 1];
  int rc=0;

  strnzcpy (filename, filename0, MAXPATHLEN);
  strlower (filename);
  for (curr = evt_flags; curr; curr = curr->next)
  {
    if (pmatch (curr->pattern, filename))
    {
      if (curr->path)
      {
	if (curr->imm)
	{
	  Log (4, "got %s, creating %s", curr->pattern, curr->path);
	  create_empty_sem_file (curr->path);
	}
	else
	{
	  ++curr->flag;
	  break;
	}
      }
      else if ((curr->command) && (curr->imm))
      {
	Log (4, "got %s, starting %s", curr->pattern, curr->command);
	rc=1;
      }
    }
  }
  return rc;
}

/*
 * Sets flags for all matched with evt_test events
 */
void evt_set (void)
{
  EVT_FLAG *curr;

  for (curr = evt_flags; curr; curr = curr->next)
  {
    if (curr->flag > 0)
    {
      Log (4, "got %s (%i), creating %s",
	   curr->pattern, curr->flag, curr->path);
      curr->flag = 0;
      create_empty_sem_file (curr->path);
    }
  }
}

/* Makes SRIF request and SRIF response names from FTN address of a node */
static int mksrifpaths (FTN_ADDR *fa, char *srf, char *rsp)
{
  ftnaddress_to_filename (srf, fa);
  if (*srf)
  {
    strnzcpy (rsp, srf, MAXPATHLEN);
    strnzcat (srf, ".srf", MAXPATHLEN);
    strnzcat (rsp, ".rsp", MAXPATHLEN);
    return 1;
  }
  else
    return 0;
}

static int srif_fill (char *path, FTN_ADDR *fa, int nfa,
		       char *req, char *rsp,
		       int prot, int listed, char *peer_name)
{
  FILE *out;
  int i;

  if ((out = fopen (path, "w")) != 0)
  {
    fprintf (out, "Sysop sysop\n");
    for (i = 0; i < nfa; ++i)
    {
      char adr[FTN_ADDR_SZ + 1];

      ftnaddress_to_str (adr, fa + i);
      fprintf (out, "AKA %s\n", adr);
    }
    fprintf (out, "Baud 115200\n");
    fprintf (out, "Time -1\n");
    fprintf (out, "RequestList %s\n", req);
    fprintf (out, "ResponseList %s\n", rsp);
    fprintf (out, "RemoteStatus %s\n", prot ? "PROTECTED" : "UNPROTECTED");
    fprintf (out, "SystemStatus %s\n", listed ? "LISTED" : "UNLISTED");
    fprintf (out, "CallerID %s\n", peer_name);
    fprintf (out, "SessionType OTHER\n");
    fclose (out);
    return 1;
  }
  return 0;
}

static FTNQ *parse_response (FTNQ *q, char *rsp, FTN_ADDR *fa)
{
  FILE *in = fopen (rsp, "r");

  if (in)
  {
    char buf[MAXPATHLEN + 1];
    int i;

    while (!feof (in))
    {
      if (!fgets (buf, MAXPATHLEN, in))
	break;
      for (i = 0; i < sizeof (buf) - 1 && !isspace (buf[i]); ++i);
      buf[i] = 0;
      switch (*buf)
	{
	  case '=':
	    q = q_add_file (q, buf + 1, fa, 'h', 'd', 0);
	    break;
	  case '+':
	    q = q_add_file (q, buf + 1, fa, 'h', 0, 0);
	    break;
	  case '-':
	    q = q_add_file (q, buf + 1, fa, 'h', 'a', 0);
	    break;
	  default:
	    break;
	}
    }
    fclose (in);
  }
  else
    Log (1, "parse_response: %s: %s", rsp, strerror (errno));
  return q;
}

static char valid_filename_chars[]=".\\-_:/@";
static void run_args(char *cmd, char *filename0, FTN_ADDR *fa, 
                 int nfa, int prot, int listed, char *peer_name, STATE *st)
{
  char *sp, *w;
  char *fn=filename0;
  char *pn=peer_name?peer_name:"";
  char ipaddr[16];
  char aka[4];
  char adr[FTN_ADDR_SZ + 2];
  int i;
  unsigned int l;
  unsigned int sw=1024;
  int use_fn=0;

  for(i=0;fn[i];i++)
  if((!isalnum(fn[i])) && (!strchr(valid_filename_chars, fn[i])))
  {
    fn="-";
    Log(4, "Invalid filename (%s) received!", filename0);
    break;
  }
  for(i=0;pn[i];i++)
  if((!isalnum(pn[i])) && (!strchr(valid_filename_chars, pn[i])))
  {
    i=0;
    break;
  }
  if(!i)
  {
    struct sockaddr_in sin;
    i=sizeof(struct sockaddr_in);
    if((!st) || (getpeername (st->s, (struct sockaddr *) &sin, &i) == -1))
      strcpy(ipaddr, "-");
    else  /* We don't like inet_ntoa ;-) */
    {
      sin.sin_addr.s_addr=htonl(sin.sin_addr.s_addr);
      sprintf(ipaddr, "%d.%d.%d.%d", 
                (int)(sin.sin_addr.s_addr>>24),
                (int)((sin.sin_addr.s_addr>>16)&0xFF),
                (int)((sin.sin_addr.s_addr>>8)&0xFF),
                (int)(sin.sin_addr.s_addr&0xFF));
    }
    pn=ipaddr;
  }

  if(sw<=strlen(cmd)) sw=strlen(cmd)+1;
  w=(char*)xalloc(sw);
  strcpy(w, cmd);

  strcpy(aka, "*A0");

  for(l=0;(l<strlen(w)) && ((sp=strchr(w+l, '*'))!=NULL); l++)
  {
    l=sp-w;
    switch(toupper(sp[1]))
    {
      case 'N': /* short filename for win32 */
#ifdef WIN32
        {
          char ts[MAXPATHLEN+1];
          i=GetShortPathName(filename0, ts, sizeof(ts));
          if((i<1) || (i>MAXPATHLEN))
          {
            Log(2, "GetShortPathName() fails %d", GetLastError());
            use_fn=1;
            strcpy(ts, fn);
          }
          w = ed (w, "*N", ts, &sw);
          break;
        }
#endif
        use_fn=1;
        w = ed (w, "*N", fn, &sw);
        break;
      case 'F': /* filename */
        use_fn=1;
        w = ed (w, "*F", fn, &sw);
        break;
      case 'A': /* AKA */
        i=isdigit(sp[2])?(sp[2]-'0'):0;
        if(i<nfa)
          ftnaddress_to_str (adr, fa+i);
        else
          strcpy(adr, "-");
        aka[2]=sp[2];
        if(!isdigit(sp[2]))  /* If you will remove it, *A *A1 may not work */
        {
          i=strlen(adr);
          adr[i++]=sp[2];
          adr[i]=0;
        }
        w = ed (w, aka, adr, &sw);
        break;
      case 'P': /* protected ? */
        w = ed (w, "*P", prot?"1":"0", &sw);
        break;
      case 'L': /* listed ? */
        w = ed (w, "*L", listed?"1":"0", &sw);
        break;
      case 'H': /* hostname or IP */
        w = ed (w, "*H", pn, &sw);
        break;
    }
  }

  if((fn!=filename0) && (use_fn))
    Log(1, "Security problem. Execution aborted...");
  else
    run(w);
  free(w);
}

/*
 * Runs external programs using S.R.I.F. interface
 * if the name matches one of our "exec"'s
 */
FTNQ *evt_run (FTNQ *q, char *filename0,
	        FTN_ADDR *fa, int nfa,
	        int prot, int listed, char *peer_name, STATE *st)
{
  EVT_FLAG *curr;
  char filename[MAXPATHLEN + 1];

  strnzcpy (filename, filename0, MAXPATHLEN);
  strlower (filename);
  for (curr = evt_flags; curr; curr = curr->next)
  {
    if (curr->command && pmatch (curr->pattern, filename) && 
        ((!st && !curr->imm) || (curr->imm && st)) )
    {
      char srf[MAXPATHLEN + 1], rsp[MAXPATHLEN + 1];

      if (strstr (curr->command, "*S") || strstr (curr->command, "*s"))
      {
	if (mksrifpaths (fa, srf, rsp))
	{
	  char *w = ed (curr->command, "*S", srf, NULL);

	  if (srif_fill (srf, fa, nfa, filename0, rsp, prot, listed, peer_name))
	  {
            run_args (w, filename0, fa, nfa, prot, listed, peer_name, st);
	  }
	  else
	    Log (1, "srif_fill: error");
	  free (w);
	  q = parse_response (q, rsp, fa);
	  delete (srf);
	  delete (rsp);
          delete (filename0);
	}
	else
	  Log (1, "mksrifpaths: error");
      }
      else
        run_args (curr->command, filename0, fa, nfa, prot, listed, peer_name, st);
    }
  }
  return q;
}
