/*
 *  protocol.c -- binkp implementation
 *
 *  protocol.c is a part of binkd project
 *
 *  Copyright (C) 1996-2004  Dima Maloff, 5047/13 and others
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
 * Revision 2.67.2.18  2004/11/08 12:05:33  gul
 * 0.9.8 release
 *
 * Revision 2.67.2.17  2004/10/21 17:02:03  gul
 * Rename trunc() -> trunc_file() due to conflicts under OS/2 EMX and SuSE
 *
 * Revision 2.67.2.16  2004/10/18 15:18:16  gul
 * Minor bugfix (patch from Victor Levenets <aq@takas.lt>)
 *
 * Revision 2.67.2.15  2004/10/01 10:05:09  gul
 * Fixed memory leak
 * (Reported by Victor Levenets <aq@takas.lt>)
 *
 * Revision 2.67.2.14  2003/09/15 21:28:31  gul
 * Fix remote IP check logic
 *
 * Revision 2.67.2.13  2003/08/29 13:37:37  gul
 * Do not save zero-length .dt files
 *
 * Revision 2.67.2.12  2003/08/27 06:46:37  gul
 * Migrate from stable branch:
 * remove partial if received part more then total size,
 * flush buffer after receive data frame,
 * drop session if extra bytes received.
 *
 * Revision 2.67.2.11  2003/08/24 18:58:33  gul
 * Bugfix in timeout check on win32
 *
 * Revision 2.67.2.10  2003/08/24 01:34:11  hbrew
 * Update for previous patch
 *
 * Revision 2.67.2.9  2003/08/24 00:35:46  hbrew
 * Cosmetic change for previous patch
 *
 * Revision 2.67.2.8  2003/08/24 00:29:31  hbrew
 * win9x-select-workaround fix, thanks to Pavel Gulchouck)
 *
 * Revision 2.67.2.7  2003/08/17 08:11:06  gul
 * Migrate patch from current:
 * Drop remote AKA with another password on outgoing sessions
 *
 * Revision 2.67.2.5  2003/08/13 11:38:43  hbrew
 * Fix warning.
 *
 * Revision 2.67.2.4  2003/08/11 08:41:55  gul
 * workaround winsock bug (patch by Alexander Reznikov)
 *
 * Revision 2.67.2.3  2003/06/24 07:11:47  gul
 * Migrate try/hold behavior from current branch
 *
 * Revision 2.67.2.2  2003/06/21 19:36:13  gul
 * Fixed remote ip check
 *
 * Revision 2.67.2.1  2003/06/06 16:27:44  gul
 * Workaround winsock bug - giveup CPU when sending file
 *
 * Revision 2.67  2003/06/04 20:59:43  gul
 * bugfix: do not force NR-mode if remote uses binkp/1.0
 *
 * Revision 2.66  2003/06/02 17:56:03  gul
 * Workaround old binkd bug in asymmetric NR-mode
 *
 * Revision 2.65  2003/06/02 17:29:28  gul
 * Bugfix in asymmetric ND-mode
 *
 * Revision 2.64  2003/05/30 17:15:22  gul
 * Asymmetric ND-mode, new protocol option NDA
 *
 * Revision 2.63  2003/05/30 16:03:10  gul
 * Asymmetric NR-mode
 *
 * Revision 2.62  2003/05/22 06:39:41  gul
 * Send CRYPT option only in crypt-mode sessions on answer
 *
 * Revision 2.61  2003/05/17 15:37:48  gul
 * Improve logging
 *
 * Revision 2.60  2003/05/17 15:33:51  gul
 * Improve logging
 *
 * Revision 2.59  2003/05/15 06:08:46  gul
 * Crypt bug with asymmetric secure settings
 *
 * Revision 2.58  2003/05/04 10:29:54  gul
 * Say "OPT ND" on answer only if this option specified in config for this node
 *
 * Revision 2.57  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.56  2003/05/03 20:36:45  gul
 * Print diagnostic message to log on failed session
 *
 * Revision 2.55  2003/05/03 11:04:58  gul
 * Fix typo
 *
 * Revision 2.54  2003/05/03 10:56:00  gul
 * Bugfix in protocol logic (when file already exists)
 *
 * Revision 2.53  2003/05/03 08:41:16  gul
 * bugfix in protocol, when file already exists
 *
 * Revision 2.52  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.51  2003/04/30 13:38:17  gul
 * Avoid warnings
 *
 * Revision 2.50  2003/04/28 07:30:16  gul
 * Bugfix: Log() changes TCPERRNO
 *
 * Revision 2.49  2003/04/04 13:54:28  gul
 * Bugfix in localtime detection
 *
 * Revision 2.48  2003/04/02 13:12:57  gul
 * Try to use workaround for buggy windows time functions (timezone)
 *
 * Revision 2.47  2003/03/31 20:28:24  gul
 * safe_localtime() and safe_gmtime() functions
 *
 * Revision 2.46  2003/03/31 14:25:36  gul
 * Segfault under FreeBSD
 *
 * Revision 2.45  2003/03/25 14:08:30  gul
 * Do not save empty partial files
 *
 * Revision 2.44  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.43  2003/03/10 17:32:37  gul
 * Use socklen_t
 *
 * Revision 2.42  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.41  2003/03/05 13:21:51  gul
 * Fix warnings
 *
 * Revision 2.40  2003/03/04 13:46:27  gul
 * Small bugfix in binkp protocol logic
 *
 * Revision 2.39  2003/03/04 13:10:39  gul
 * Do not report errors when threads exits by exitfunc
 *
 * Revision 2.38  2003/03/04 09:50:58  gul
 * Cosmetics changes
 *
 * Revision 2.37  2003/03/03 20:16:32  gul
 * Fixed bug in previous patch
 *
 * Revision 2.36  2003/03/02 18:08:56  gul
 * Do not scan outbound twice: on prescan (for TRF report) and at complete_login
 *
 * Revision 2.35  2003/03/02 17:51:37  gul
 * Close received file before send M_GOT
 *
 * Revision 2.34  2003/03/02 14:30:02  gul
 * Drop unsecure AKA with bad source IP address, no drop session
 *
 * Revision 2.33  2003/03/01 20:49:21  gul
 * Fix spelling
 *
 * Revision 2.32  2003/03/01 18:29:52  gul
 * Change size_t to off_t for file sizes and offsets
 *
 * Revision 2.31  2003/03/01 18:16:04  gul
 * Use HAVE_SYS_TIME_H macro
 *
 * Revision 2.30  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.29  2003/03/01 15:00:16  gul
 * Join skipmask and overwrite into common maskchain
 *
 * Revision 2.28  2003/02/28 19:52:24  gul
 * Small optimize
 *
 * Revision 2.27  2003/02/23 16:47:07  gul
 * change restrictIP logic
 *
 * Revision 2.26  2003/02/23 16:31:21  gul
 * Add "-sip" option in node string.
 * Change "-ip" check logic.
 *
 * Revision 2.25  2003/02/22 20:19:54  gul
 * Update copyrightes, 2002->2003
 *
 * Revision 2.24  2003/02/22 15:53:46  gul
 * Bugfix with locking array of nodes in multithread version
 *
 * Revision 2.23  2003/02/22 12:56:00  gul
 * Do not give unsecure mail to secure link when send-if-pwd
 *
 * Revision 2.22  2003/02/22 12:12:34  gul
 * Cleanup sources
 *
 * Revision 2.21  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.20  2003/02/13 19:31:56  gul
 * Ignore non-MD5 challanges
 *
 * Revision 2.19  2003/01/29 20:53:34  gul
 * Assume default domain for remote 4D address
 *
 * Revision 2.18  2003/01/28 16:14:05  gul
 * Bugfix: binkd did not remove lo-files with empty lines
 *
 * Revision 2.17  2003/01/16 13:35:26  gul
 * Fixed crash on bad incoming packets
 *
 * Revision 2.16  2002/11/22 14:40:42  gul
 * Check free space on inbox if defined
 *
 * Revision 2.15  2002/10/22 20:29:46  gul
 * Do not send zero-length data packet as EOF
 * Prevent incorrect "remote already has ..." message
 *
 * Revision 2.14  2002/05/11 10:28:11  gul
 * fix spelling
 *
 * Revision 2.13  2002/05/11 10:27:22  gul
 * Do not send empty (60-bytes) pkt-files
 *
 * Revision 2.12  2002/02/22 08:57:23  gul
 * Pring warning if remote says "OK non-secure" and we have password
 * for the session
 *
 * Revision 2.11  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
 *
 * Revision 2.10  2001/07/28 17:26:26  gul
 * Avoid compiler warnings
 *
 * Revision 2.9  2001/07/28 09:56:39  gul
 * NR-mode bugfix
 *
 * Revision 2.8  2001/07/28 09:10:04  gul
 * minor fix in log messages
 *
 * Revision 2.7  2001/07/28 08:53:07  gul
 * set ND-mode bugfix
 *
 * Revision 2.6  2001/05/23 16:48:03  gul
 * msvc warnings fixed
 *
 * Revision 2.5  2001/04/13 20:16:10  gul
 * "OPT CRYPT" was send only in NR-mode
 *
 * Revision 2.4  2001/02/20 12:01:50  gul
 * rename encrypt to encrypt_buf to avoid conflict with unistd.h
 *
 * Revision 2.3  2001/02/16 09:13:25  gul
 * Disable crypt with plaintext password
 *
 * Revision 2.2  2001/02/15 16:05:59  gul
 * crypt bugfix
 *
 * Revision 2.1  2001/02/15 11:03:18  gul
 * Added crypt traffic possibility
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.35  1997/11/04  23:37:11  mff
 * send_block() changed to send as many msgs as possible with one send()
 *
 * Revision 1.34  1997/10/23  03:56:21  mff
 * authentication fixes, new binary log, more?
 *
 * 1997/07/11  11:56:55  maxb
 * Added more informative percents (I think mff won't kill me for this :)
 *
 * Revision 1.33  1997/06/27  01:05:09  mff
 * Now we close link after M_ERR or M_BSY
 *
 * Revision 1.32  1997/06/16  05:45:21  mff
 * Binkd could not correctly ACK receiving of files with some names.
 * Binary log in T-Mail format. Binkd didn't strip disk labels.
 * Session would never end if remote skips files being sent
 * from our outbound filebox. New keyword: `send-if-pwd'.
 *
 * Revision 1.31  1997/05/17  08:42:49  mff
 * Binkd could not ACK some filenames
 *
 * Revision 1.30  1997/05/08  05:30:29  mff
 * End-of-session is now logged verbosely
 *
 * Revision 1.29  1997/03/28  06:47:39  mff
 * SRIF support, prothlp.*, etc?
 *
 * Revision 1.28  1997/03/15  05:06:44  mff
 * Binkp/1.1 finished(?): NR mode, multiple batches. *.bsy touching.
 *
 * Revision 1.26  1997/02/13  07:08:39  mff
 * Support for fileboxes
 *
 * Revision 1.24  1997/01/09  05:31:29  mff
 * minfree and minfree_nonsecure
 *
 * Revision 1.23  1997/01/08  03:59:54  mff
 * Support for mail events flags
 *
 * Revision 1.22  1996/12/29  09:45:05  mff
 * Fixed NONBIO bug
 *
 * Revision 1.18  1996/12/07  12:14:04  mff
 * max_servers limits the number of server processes/threads running
 *
 * Revision 1.12  1996/12/03  11:27:33  mff
 * NT port by ufm
 */

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "Config.h"
#include "sys.h"
#include "iphdr.h"
#include "common.h"
#include "iptools.h"
#include "tools.h"
#include "readcfg.h"
#include "ftnq.h"
#include "bsy.h"
#include "inbound.h"
#include "srif.h"
#include "readflo.h"
#include "protocol.h"
#include "prothlp.h"
#include "protoco2.h"
#include "assert.h"
#include "binlog.h"
#include "setpttl.h"
#include "sem.h"
#include "md5b.h"
#include "crypt.h"

static char *scommand[] = {"NUL", "ADR", "PWD", "FILE", "OK", "EOB",
                           "GOT", "ERR", "BSY", "GET", "SKIP"};

/*
 * Fills <<state>> with initial values, allocates buffers, etc.
 */
static int init_protocol (STATE *state, SOCKET socket, FTN_NODE *to)
{
  memset (state, 0, sizeof (STATE));

  state->major = 1;
  state->minor = 0;
  state->msgs_in_batch = 0;
  state->off_req_sent = 0;
  state->waiting_for_GOT = 0;
  state->send_eof = 0;
  state->inbound = inbound_nonsecure;
  state->io_error = 0;
  state->ibuf = xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->isize = -1;
  state->obuf = xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->optr = 0;
  state->oleft = 0;
  state->bytes_sent = state->bytes_rcvd = 0;
  state->files_sent = state->files_rcvd = 0;
  state->to = to;
  state->NR_flag = (to && (to->NR_flag == NR_ON || to->ND_flag == ND_ON)) ? WANT_NR : NO_NR;
  state->ND_flag = (to && to->ND_flag == ND_ON) ? THEY_ND : NO_ND;
  state->MD_flag = 0;
  state->MD_challenge = NULL;
  state->crypt_flag = no_crypt ? NO_CRYPT : WE_CRYPT;
  strcpy (state->expected_pwd, "-");
  state->skip_all_flag = state->r_skipped_flag = 0;
  state->maxflvr = 'h';
  state->listed_flag = 0;
  state->fa = NULL;
  state->nfa = state->nallfa = 0;
  setsockopts (state->s = socket);
  TF_ZERO (&state->in);
  TF_ZERO (&state->out);
  TF_ZERO (&state->flo);
  TF_ZERO (&state->in_complete);
  state->ND_addr.z = -1;
  state->start_time = safe_time();
  state->evt_queue = NULL;
  Log (6, "binkp init done, socket # is %i", state->s);
  return 1;
}

/* 
 * Close file currently receiving,
 * remove .hr and .dt if it's partial pkt or zero-length
 */
static int close_partial (STATE *state)
{
  off_t s;

  if (state->in.f)
  {
    if (ispkt (state->in.netname))
    {
      Log (2, "%s: partial .pkt", state->in.netname);
      s = 0;
    }
    else
    {
      if ((s = ftell (state->in.f)) == 0)
      Log (4, "%s: empty partial", state->in.netname);
    }
    fclose (state->in.f);
    if (s == 0)
      inb_reject (state->in.netname, state->in.size, state->in.time,
                  state->fa, state->nallfa, state->inbound);
  }
  TF_ZERO (&state->in);
  return 0;
}

/*
 * Clears protocol buffers and queues, closes files, etc.
 */
static int deinit_protocol (STATE *state)
{
  int i;

  close_partial(state);
  if (state->out.f)
    fclose (state->out.f);
  if (state->flo.f)
    fclose (state->flo.f);
  if (state->killlist)
    free_killlist (&state->killlist, &state->n_killlist);
  if (state->rcvdlist)
    free_rcvdlist (&state->rcvdlist, &state->n_rcvdlist);
  if (state->ibuf)
    free (state->ibuf);
  if (state->obuf)
    free (state->obuf);
  if (state->msgs)
    free (state->msgs);
  if (state->sent_fls)
    free (state->sent_fls);
  if (state->q)
    q_free (state->q);
  for (i = 0; i < state->nfa; ++i)
    bsy_remove (state->fa + i, F_BSY);
  if (state->fa)
    free (state->fa);
  if (state->MD_challenge)
	free (state->MD_challenge);
  rel_grow_handles(-state->nfa);
  Log (6, "binkp deinit done...");
  return 0;
}

/* Process rcvdlist */
FTNQ *process_rcvdlist (STATE *state, FTNQ *q)
{
  int i;

  Log (6, "processing rcvd list");
  for (i = 0; i < state->n_rcvdlist; ++i)
  {
    q = evt_run(&(state->evt_queue), q, state->rcvdlist[i].name, state->fa,
		state->nfa, state->state == P_SECURE, state->listed_flag,
		state->peer_name, NULL);
  }
  return q;
}

/* Fills s[0] and s[1] with binkp frame header using value of u */
static void mkhdr (char *s, unsigned u)
{
  s[0] = (char) (u >> 8);
  s[1] = (char) u;
}

/*
 * Puts a message to the output msg. queue. These msgs will be send
 * right after the current data block.
 */
static void msg_send2 (STATE *state, t_msg m, char *s1, char *s2)
{
  if (!s1)
    s1 = "";
  if (!s2)
    s2 = "";

  state->msgs = xrealloc (state->msgs, sizeof (BMSG) * (state->n_msgs + 1));
  state->msgs[state->n_msgs].t = m;
  /* We will check for sz correctness (sz <= 0x7fff) later, when sending
   * messages from the queue. */
  state->msgs[state->n_msgs].sz = strlen (s1) + strlen (s2) + 1;
  state->msgs[state->n_msgs].s =
    xalloc (state->msgs[state->n_msgs].sz + 3);

  mkhdr (state->msgs[state->n_msgs].s,
	 (unsigned) (state->msgs[state->n_msgs].sz | 0x8000));
  state->msgs[state->n_msgs].s[2] = m;

  strcpy (state->msgs[state->n_msgs].s + 3, s1);
  strcat (state->msgs[state->n_msgs].s + 3, s2);
  state->msgs[state->n_msgs].sz += 2;
  if (state->crypt_flag == YES_CRYPT)
    encrypt_buf(state->msgs[state->n_msgs].s, state->msgs[state->n_msgs].sz,
            state->keys_out);

  ++state->n_msgs;
  ++state->msgs_in_batch;

  Log (5, "send message %s %s%s", scommand[m], s1, s2);
}

/*
 * Sends a message using format string
 */
static void msg_sendf (STATE *state, t_msg m, char *s,...)
{
  char msg_text[max (MAXPATHLEN, MAX_NETNAME) + 80];
  va_list ap;

  va_start (ap, s);
#ifdef HAVE_SNPRINTF
  vsnprintf (msg_text, sizeof (msg_text), s, ap);
#else
  vsprintf (msg_text, s, ap);
#endif
  va_end (ap);
  msg_send2 (state, m, msg_text, 0);
}

static void current_file_was_sent (STATE *state)
{
  fclose (state->out.f);
  state->sent_fls = xrealloc (state->sent_fls,
			      ++(state->n_sent_fls) * sizeof (TFILE));
  memcpy (state->sent_fls + state->n_sent_fls - 1,
	  &state->out,
	  sizeof (TFILE));
  TF_ZERO (&state->out);
  if (state->ND_flag & WE_ND)
  {
    state->waiting_for_GOT = 1;
    Log(4, "Waiting for M_GOT");
  }
}

/*
 * Sends next msg from the msg queue or next data block
 */
static int send_block (STATE *state)
{
  int i, n, save_errno;
  const char *save_err;

  /* Have something to send in buffers */
  if (state->optr && state->oleft)
  {
    Log (7, "sending %li byte(s)", (long) (state->oleft));
    n = send (state->s, state->optr, state->oleft, 0);
    save_errno = TCPERRNO;
    save_err = TCPERR ();
    Log (7, "send() done, rc=%i", n);
    if (n == state->oleft)
    {
      state->optr = 0;
      state->oleft = 0;
      Log (7, "data sent");
    }
    else if (n == SOCKET_ERROR)
    {
      if (save_errno != TCPERR_WOULDBLOCK && save_errno != TCPERR_AGAIN)
      {
	state->io_error = 1;
	if (!binkd_exit)
	{
	  Log (1, "send: %s", save_err);
	  if (state->to)
	    bad_try (&state->to->fa, save_err);
	}
	return 0;
      }
      Log (7, "data transfer would block");
      return 2;
    }
    else
    {
      state->optr += n;
      state->oleft -= n;
      Log (7, "partially sent, %li byte(s) left", state->oleft);
    }
  }
  else
  {
    /* There is no data partially sent */

    if (state->msgs)
    {
      /* There are unsent msgs */
      state->optr = state->obuf;
      state->oleft = 0;
      for (i = 0; i < state->n_msgs; ++i)
      {
	if (state->msgs[i].s)
	{
	  /* Check for possible internal error */
	  if (state->msgs[i].sz - 2 > MAX_BLKSIZE)
	  {
	    Log (1, "size of msg we want to send is too big (%i)",
		 state->msgs[i].sz - 2);
	    return 0;
	  }

	  /* Is there some space for the new msg? */
	  if (state->oleft + state->msgs[i].sz > MAX_BLKSIZE)
	    break;

	  Log (7, "put next msg to obuf, %li", (long) state->msgs[i].sz);
	  memcpy (state->optr, state->msgs[i].s, state->msgs[i].sz);
	  state->oleft += state->msgs[i].sz;
	  state->optr += state->msgs[i].sz;
	  free (state->msgs[i].s);
	  state->msgs[i].s = 0;
	}
      }

      /* Optr should be non-NULL if there are some data to send */
      if (state->oleft == 0)
	state->optr = 0;
      else
	state->optr = state->obuf;

      /* If the message queue is empty, free it */
      if (i >= state->n_msgs)
      {
	free (state->msgs);
	state->msgs = 0;
	state->n_msgs = 0;
      }
      return 1;
    }
    else if ((state->out.f && !state->off_req_sent && !state->waiting_for_GOT) ||
	     state->send_eof)
    {
      /* There is a file in transfer and we don't wait for an answer for * *
       * "FILE ... -1" */
      unsigned sz;

      if (state->out.f)
      {
	sz = min ((off_t) oblksize, state->out.size - ftell (state->out.f));
	if (percents && state->out.size > 0)
	{
	  printf ("%-20.20s %3.0f%%\r", state->out.netname,
		  100.0 * ftell (state->out.f) / (float) state->out.size);
	  fflush (stdout);
	}
      }
      else
      {
	state->send_eof = 0;
	sz = 0;
      }
      Log (7, "next block to send: %u byte(s)", sz);
      mkhdr (state->obuf, sz);
      if (sz != 0)
      {
	Log (7, "freading %u byte(s)", sz);
	if ((n = fread (state->obuf + BLK_HDR_SIZE, 1, sz, state->out.f)) < (int) sz)
	{
	  Log (1, "error reading %s: expected %u, read %i",
	       state->out.path, sz, n);
	  return 0;
	}
      }
      if (state->out.f && (sz == 0 || state->out.size == ftell(state->out.f)))
	/* The current file have been sent */
	current_file_was_sent (state);
      state->optr = state->obuf;
      state->oleft = sz + BLK_HDR_SIZE;
      if (state->crypt_flag == YES_CRYPT)
        encrypt_buf(state->optr, state->oleft, state->keys_out);
    }
  }
  return 1;
}

/*
 * Extends parse_args() by sending error message to the remote
 */
static int parse_msg_args (int ac, char **av, char *s, char *ID, STATE *state)
{
  int rc = parse_args (ac, av, s, ID);

  if (!rc)
    msg_send2 (state, M_ERR, ID, ": cannot parse args");
  return rc;
}

/*
 * Truncates the file if action == 't'
 * Removes the file if action == 'd'
 * Removes on successful session if action == 's'
 * Otherwise lefts the file unchanged
 */
static int perform_action (STATE *state, char *path, char action)
{
  if (action == 'd')
  {
    delete (path);
  }
  else if (action == 't')
  {
    trunc_file (path);
  }
  else if (action == 's')
  {
    add_to_killlist (&state->killlist, &state->n_killlist, path, 's');
  }
  return 1;
}

/*
 * Marks the file in flopath as sent. (Empty .?lo will be removed)
 * If file == 0 just tryes to unlink flopath.
 * If flopath == 0 performs action on file.
 */
static int remove_from_spool (STATE *state, char *flopath,
			       char *file, char action)
{
  char buf[MAXPATHLEN + 1], *w = 0;
  FILE *flo = 0;
  off_t offset = 0, curr_offset;
  int i;
  int seek_flag = 0;		       /* Seek _state->flo.f_ to */

  /* _offset_ after processing */
  int empty_flo_flag = 1;

  if (file)
    Log (5, "removing from spool: %s", file);
  else if (flopath)
    Log (5, "removing flo: %s", flopath);
  else
    Log (1, "internal error in remove_from_spool!");

  if (flopath && *flopath)	       /* A file attached via .?lo */
  {
    if (state->flo.f && !strcmp (state->flo.path, flopath))
    {
      flo = state->flo.f;
      offset = ftell (flo);
      fseek (flo, 0, SEEK_SET);
      seek_flag = 1;
    }
    else
    {
      if ((flo = fopen (flopath, "r+b")) == 0)
      {
	Log (5, "remove_from_spool: %s: %s", flopath, strerror (errno));
	return 0;
      }
    }

    while (!feof (flo))
    {
      curr_offset = ftell (flo);
      if (!fgets (buf, MAXPATHLEN, flo))
	break;
      for (i = strlen (buf) - 1; i >= 0 && isspace (buf[i]); --i)
	buf[i] = 0;
      if (buf[0] == '\0') continue;

      if (file && (!strcmp (file, buf) ||
		 ((*buf == '^' || *buf == '#') && !strcmp (file, buf + 1))))
      {
	clearerr (flo);
	if (fseek (flo, curr_offset, SEEK_SET) == EOF)
	  Log (1, "remove_from_spool: fseek(%s): %s", flopath,
	       strerror (errno));
	else if (putc ('~', flo) == EOF)
	  Log (1, "remove_from_spool: fputc(%s): %s", flopath,
	       strerror (errno));
	fflush (flo);
	/* The line was marked, now skip it */
	fgets (buf, MAXPATHLEN, flo);
	/* We've found the file in flo, so try to translate it's name before
	 * the action */
	if (w == 0 && (w = trans_flo_line (file)) != 0)
	{
	  Log (5, "%s mapped to %s", file, w);
	}
      }
      else if (*buf && *buf != '~')
      {
	empty_flo_flag = 0;
      }
    }
    if (seek_flag)
    {
      fseek (flo, offset, SEEK_SET);
      fflush (flo);
    }
    else
    {
      fclose (flo);
      if (empty_flo_flag)
	delete (flopath);
    }
  }
  if (w)
  {
    perform_action (state, w, action);
    free (w);
  }
  else if (file)
    perform_action (state, file, action);
  return 1;
}

/*
 * Removes n-th element from the sent files queue,
 * free()'s the queue if it's empty
 */
static void remove_from_sent_files_queue (STATE *state, int n)
{
  state->sent_fls[n].netname[0] = 0;

  for (n = 0; n < state->n_sent_fls; ++n)
    if (state->sent_fls[n].netname[0])
      return;

  if (n >= state->n_sent_fls)
  {
    free (state->sent_fls);
    state->sent_fls = 0;
    state->n_sent_fls = 0;
  }
}

static void do_prescan(STATE *state)
{
  char s[64];
  unsigned long netsize, filessize;

  if (OK_SEND_FILES (state) && prescan)
  {
    state->q = q_scan_addrs (0, state->fa, state->nfa, state->to ? 1 : 0);
    q_get_sizes (state->q, &netsize, &filessize);
    sprintf(s, "%lu %lu", netsize, filessize);
    msg_send2 (state, M_NUL, "TRF ", s);
  }
}

/*
 * These functions down to recv_block() handle binkp msgs
 *
 * They should return 0 on fatal error, otherwise 1
 */

/*
 * Parses if needed and logs down the M_NUL message data
 */
static int NUL (STATE *state, char *buf, int sz)
{
  char *s, *a, *b;

  Log (3, "%s", s = strquote (buf, SQ_CNTRL));
  if (!memcmp (s, "VER ", 4) &&
      (a = strstr (s, PRTCLNAME "/")) != 0 &&
      (b = strstr (a, ".")) != 0)
  {
    state->major = atoi (a + 6);
    state->minor = atoi (b + 1);
    Log (6, "remote uses " PRTCLNAME " v.%i.%i", state->major, state->minor);
  }
  else if (!memcmp (s, "OPT ", 4))
  {
    char *w;
    int i;

    for (i = 1; (w = getwordx (s + 4, i, 0)) != 0; ++i)
    {
      if (!strcmp (w, "NR"))
      {
	state->NR_flag |= WE_NR;      /* They want NR mode - turn it on */
	Log(2, "Remote requests NR mode");
      }
      if (!strcmp (w, "ND"))
      {
	state->ND_flag |= WE_ND;      /* They want ND mode - turn it on */
	Log(2, "Remote requests ND mode");
      }
      if (!strcmp (w, "NDA"))
      {
	state->ND_flag |= CAN_NDA;     /* They supports asymmetric ND */
	Log(2, "Remote supports asymmetric ND mode");
      }
      if (!strcmp (w, "CRYPT"))
      {
	state->crypt_flag |= THEY_CRYPT;  /* They want crypt mode */
	Log(2, "Remote requests CRYPT mode");
      }
      if (!strncmp(w, "CRAM-MD5-", 9) && !no_MD5 &&
          state->to && (state->to->MD_flag>=0))
      {
	Log(2, "Remote requests MD mode");
	if(state->MD_challenge) free(state->MD_challenge);
	state->MD_challenge=MD_getChallenge(w, NULL);
      }
      free (w);
    }
  }
  else if (!memcmp (s, "SYS ", 4))
    strnzcpy (state->sysname, s + 4, sizeof (state->sysname));
  else if (!memcmp (s, "ZYZ ", 4))
    strnzcpy (state->sysop, s + 4, sizeof (state->sysop));
  else if (!memcmp (s, "LOC ", 4))
    strnzcpy (state->location, s + 4, sizeof (state->location));
  free (s);
  return 1;
}

/*
 * Handles M_ERR msg from the remote
 */
static int RError (STATE *state, char *buf, int sz)
{
  char *s;

  Log (1, "rerror: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s);
  free (s);
  return 0;
}

static int BSY (STATE *state, char *buf, int sz)
{
  char *s;

  Log (1, "got M_BSY: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s);
  free (s);
  return 0;
}

static int ADR (STATE *state, char *s, int sz)
{
  int i, j, main_AKA_ok = 0, ip_verified = 0;
  char *w;
  FTN_ADDR fa;
  FTN_NODE n, *pn;
  char szFTNAddr[FTN_ADDR_SZ + 1];
  int secure_NR, unsecure_NR;
  int secure_ND, unsecure_ND;

  s[sz] = 0;
  secure_NR = unsecure_NR = NO_NR;
  secure_ND = unsecure_ND = NO_ND;

  /* set expected password on outgoing session
   * for drop remote AKAs with another passwords */
  if (state->to)
    memcpy (state->expected_pwd, state->to->pwd, sizeof (state->expected_pwd));

  for (i = 1; (w = getwordx (s, i, 0)) != 0; ++i)
  {
    if (!parse_ftnaddress (w, &fa) || !is4D (&fa))
    {
      char *q = strquote (s, SQ_CNTRL);

      msg_send2 (state, M_ERR, "Bad address", 0);
      Log (1, "remote passed bad address: `%s'", q);
      free (w);
      free (q);
      return 0;
    }

    free (w);

    if (!fa.domain[0])
      strcpy (fa.domain, get_def_domain()->name);

    ftnaddress_to_str (szFTNAddr, &fa);
    pn = get_node(&fa, &n);

    if (state->to == 0 && pn && n.restrictIP)
    { int i, ipok = 0, rc;
      struct hostent *hp;
      struct in_addr defaddr;
      char **cp;
      char host[MAXHOSTNAMELEN + 1];       /* current host/port */
      unsigned short port;
      struct sockaddr_in sin;
      socklen_t si;

      si = sizeof (struct sockaddr_in);
      if (getpeername (state->s, (struct sockaddr *) &sin, &si) == -1)
      { Log (1, "Can't getpeername(): %s", TCPERR());
        ipok = 2;
      }

      for (i = 1; n.hosts &&
           (rc = get_host_and_port(i, host, &port, n.hosts, &n.fa)) != -1; ++i)
      {
	if (rc == 0)
	{
	  Log (1, "%s: %i: error parsing host list", n.hosts, i);
	  continue;
	}
	if (strcmp(host, "-") == 0)
	  continue;
	if (!isdigit (host[0]) ||
	    (defaddr.s_addr = inet_addr (host)) == INADDR_NONE)
	{
	  /* If not a raw ip address, try nameserver */
	  Log (5, "resolving `%s'...", host);
	  lockresolvsem();
	  if ((hp = gethostbyname (host)) == NULL)
	  {
	    releaseresolvsem();
	    Log (1, "%s: unknown host", host);
	    continue;
	  }
	  for (cp = hp->h_addr_list; cp && *cp; cp++)
	    if (((struct in_addr *) * cp)->s_addr == sin.sin_addr.s_addr)
	    {
	      ipok = 1;
	      break;
	    } else if (ipok == 0)
	      ipok = -1; /* resolved and not match */
	  releaseresolvsem();
	}
	else
	{
	  if (defaddr.s_addr == sin.sin_addr.s_addr)
	    ipok = 1;
	  else if (ipok == 0)
	    ipok = -1;  /* resolved and not match */
	}
	if (ipok == 1)
	  break;
      }
      if (ipok == 1)
      { /* matched */
	ip_verified = 2;
      } else if (ipok<0 || n.restrictIP == 2)
      { /* not matched or unresolvable with strict check */
	if (n.pwd && strcmp(n.pwd, "-") && state->to == NULL)
	{
	  if (ipok == 0)
	    Log (1, "addr: %s (unresolvable)", szFTNAddr);
	  else
	    Log (1, "addr: %s (not from allowed remote address)", szFTNAddr);
	  msg_send2 (state, M_ERR, "Bad source IP", 0);
	  return 0;
	} else
	{ /* drop unsecure AKA with bad IP-addr */
	  if (ip_verified == 0)
	    ip_verified = -1;
	  if (ipok == 0)
	    Log(2, "Addr %s dropped - unresolvable", szFTNAddr);
	  else
	    Log(2, "Addr %s dropped - not from allowed IP", szFTNAddr);
	  continue;
	}
      }
    }
    else if (pn)
    { /* no check ip -> reset restrict */
      ip_verified = 1;
    }

    if (state->expected_pwd[0] && pn)
    {
      state->listed_flag = 1;
      if (!strcmp (state->expected_pwd, "-"))
      {
	memcpy (state->expected_pwd, n.pwd, sizeof (state->expected_pwd));
	state->MD_flag=n.MD_flag;
      }
      else if (n.pwd && strcmp(n.pwd, "-") &&
               strcmp(state->expected_pwd, n.pwd))
      {
	if (state->to)
	  Log (2, "inconsistent pwd settings for this node, aka %s dropped", szFTNAddr);
	else
	{ /* drop incoming session with M_ERR "Bad password" */
	  Log (1, "inconsistent pwd settings for this node");
	  state->expected_pwd[0] = 0;
	}
	continue;
      }
    }

    if (bsy_add (&fa, F_BSY))
    {
      Log (2, "addr: %s", szFTNAddr);
      if (state->nfa == 0)
	setproctitle ("%c %s [%s]",
		      state->to ? 'o' : 'i',
		      szFTNAddr,
		      state->peer_name);
      state->fa = xrealloc (state->fa, sizeof (FTN_ADDR) * ++state->nallfa);
      ++state->nfa;
      rel_grow_handles(1);
      for (j = state->nallfa - 1; j >= state->nfa; j--)
	memcpy (state->fa + j, state->fa + j - 1, sizeof (FTN_ADDR));
      memcpy (state->fa + state->nfa - 1, &fa, sizeof (FTN_ADDR));
      if (state->to &&
	  !ftnaddress_cmp (state->fa + state->nfa - 1, &state->to->fa))
      {
	main_AKA_ok = 1;
      }
    }
    else
    {
      Log (2, "addr: %s (n/a or busy)", szFTNAddr);
      state->fa = xrealloc (state->fa, sizeof (FTN_ADDR) * ++state->nallfa);
      memcpy (state->fa + state->nallfa - 1, &fa, sizeof (FTN_ADDR));
    }

    if (!state->to && pn)
    { if (n.ND_flag)
      {
	if (n.pwd && strcmp(n.pwd, "-"))
	{
	  secure_ND = THEY_ND;
	  secure_NR = WANT_NR;
	}
	else
	{
	  unsecure_ND = THEY_ND;
	  unsecure_NR = WANT_NR;
	}
      }
      else if (n.NR_flag)
      {
	if (n.pwd && strcmp(n.pwd, "-"))
	  secure_NR = WANT_NR;
	else
	  unsecure_NR = WANT_NR;
      }
    }
  }
  if (state->nfa == 0)
  {
    Log (1, "no AKAs in common domains or all AKAs are busy");
    msg_send2 (state, M_BSY, "No AKAs in common domains or all AKAs are busy", 0);
    return 0;
  }
  if (state->to != 0 && main_AKA_ok == 0)
  {
    ftnaddress_to_str (szFTNAddr, &state->to->fa);
    Log (1, "called %s, but remote has no such AKA", szFTNAddr);
    bad_try (&state->to->fa, "Remote has no needed AKA");
    return 0;
  }
  if (ip_verified < 0)
  { /* strict IP check and no address resolved */
    Log (1, "Source IP check failed");
    msg_send2 (state, M_ERR, "Bad source IP", 0);
    return 0;
  }
  else if (ip_verified == 2)
    Log (4, "Source IP matched");
  else
    Log (5, "Source IP not checked");

  if (!state->to)
  {
    if (state->expected_pwd[0] && strcmp(state->expected_pwd, "-"))
    {
      state->ND_flag |= secure_ND;
      state->NR_flag |= secure_NR;
    }
    else
    {
      state->ND_flag |= unsecure_ND;
      state->NR_flag |= unsecure_NR;
    }
  }

  if (state->to)
  {
    do_prescan (state);
    if(state->MD_challenge)
    {
      char *tp=MD_buildDigest(state->to->pwd, state->MD_challenge);
      if(!tp) 
      {
        Log(2, "Unable to build MD5 digest");
        bad_try (&state->to->fa, "Unable to build MD5 digest");
        return 0;
      }
      msg_send2 (state, M_PWD, tp, 0);
      state->MD_flag=1;
      free(tp);
    }
    else if ((state->to->MD_flag == 1) && !no_MD5) /* We do not want to talk without MD5 */
    {
      Log(2, "CRAM-MD5 is not supported by remote");
      bad_try (&state->to->fa, "CRAM-MD5 is not supported by remote");
      return 0;
    }
    else
      msg_send2 (state, M_PWD, state->to->pwd, 0);
  }
  return 1;
}

static char *select_inbound (FTN_ADDR *fa, int secure_flag)
{
  FTN_NODE *node;
  char *p;

  locknodesem();
  node = get_node_info(fa);
  p = ((node && node->ibox) ? node->ibox :
	  (secure_flag == P_SECURE ? inbound : inbound_nonsecure));
  releasenodesem();
  return p;
}

static void complete_login (STATE *state)
{
  state->inbound = select_inbound (state->fa, state->state);
  if (OK_SEND_FILES (state) && state->q == NULL)
    state->q = q_scan_addrs (0, state->fa, state->nfa, state->to ? 1 : 0);
  state->msgs_in_batch = 0;	       /* Forget about login msgs */
  if (state->state == P_SECURE)
    Log (2, "pwd protected session (%s)",
         (state->MD_flag == 1) ? "MD5" : "plain text");
  if (state->ND_flag & WE_ND)
  { state->NR_flag |= WE_NR;
    Log (3, "we are in ND mode");
  }
  if (state->ND_flag & THEY_ND)
    Log (3, "remote is in ND mode");
  else if (state->NR_flag == WE_NR)
    Log (3, "we are in NR mode");
  if (state->state != P_SECURE)
    state->crypt_flag = NO_CRYPT;
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT) && !state->MD_flag)
  { state->crypt_flag = NO_CRYPT;
    Log (3, "Crypt allowed only with MD5 authorization");
  }
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT))
  { char *p;
    state->crypt_flag = YES_CRYPT;
    Log (3, "session in CRYPT mode");
    if (state->to)
    { init_keys(state->keys_out, state->to->pwd);
      init_keys(state->keys_in,  "-");
      for (p=state->to->pwd; *p; p++)
        update_keys(state->keys_in, (int)*p);
    } else
    { init_keys(state->keys_in, state->expected_pwd);
      init_keys(state->keys_out,  "-");
      for (p=state->expected_pwd; *p; p++)
        update_keys(state->keys_out, (int)*p);
    }
  }
  if (state->crypt_flag!=YES_CRYPT) state->crypt_flag=NO_CRYPT;
}

static int PWD (STATE *state, char *pwd, int sz)
{
  int bad_pwd=STRNICMP(pwd, "CRAM-", 5);
  int no_password=!strcmp (state->expected_pwd, "-");

  if (state->to)
  { Log (1, "unexpected password from the remote on outgoing call: `%s'", pwd);
    return 1;
  }
  if ((no_password)&&(bad_pwd))
  {
    do_prescan (state);
    state->state = P_NONSECURE;
    if (strcmp (pwd, "-"))
      Log (1, "unexpected password from the remote: `%s'", pwd);
  }
  else 
  {
    if((state->MD_flag == 1) || ((!bad_pwd) && (state->MD_challenge)))
    {
      char *sp;
      if((bad_pwd)&&(state->MD_flag))
      {
        msg_send2(state, M_ERR, "You must support MD5", 0);
        Log(1, "Caller does not support MD5");
        return 0;
      }
      if((sp=MD_buildDigest(state->expected_pwd, state->MD_challenge))!=NULL)
      {
        if((bad_pwd=STRICMP(sp, pwd))==0) state->MD_flag=1;
	free(sp);
        sp=NULL;
      }
      else {
        Log(2, "Unable to build Digest");
        bad_pwd=1;
      }
    }
    else bad_pwd=(state->expected_pwd[0] == 0 || strcmp (state->expected_pwd, pwd));

    if ((bad_pwd)&&(!no_password)) /* I don't check password if we do not need one */
    {
      msg_send2 (state, M_ERR, "Bad password", 0);
      Log (1, "`%s': incorrect password", pwd);
      return 0;
    }
    else
    {
      if(no_password) 
      {
        state->state = P_NONSECURE;
        do_prescan (state);
        if(bad_pwd) 
          Log (1, "unexpected password digest from the remote");
      }
      else
      {
	state->state = P_SECURE;
        do_prescan (state);
      }
    }
  }

  if (state->state != P_SECURE)
    state->crypt_flag = NO_CRYPT;
  else if (state->crypt_flag == (THEY_CRYPT | WE_CRYPT) && !state->MD_flag)
  { state->crypt_flag = NO_CRYPT;
    Log (4, "Crypt allowed only with MD5 authorization");
  }

  if ((state->ND_flag & WE_ND) && (state->ND_flag & CAN_NDA) == 0)
    state->ND_flag |= THEY_ND;
  if ((state->ND_flag & WE_ND) == 0 && (state->ND_flag & CAN_NDA) == 0)
    state->ND_flag &= ~THEY_ND;

  if ((state->NR_flag & WANT_NR) &&
      !(state->ND_flag & CAN_NDA) && !(state->ND_flag & WE_ND))
  { /* workaround bug of old binkd */
    /* force symmetric NR-mode with it */
#if 1
    if (state->major * 100 + state->minor > 100)
      state->NR_flag |= WE_NR;
    else
#endif
      state->NR_flag &= ~WANT_NR;
  }

  if ((state->NR_flag & WANT_NR) ||
      (state->crypt_flag == (WE_CRYPT | THEY_CRYPT)) ||
      (state->ND_flag & (WE_ND|THEY_ND)))
    msg_sendf (state, M_NUL, "OPT%s%s%s%s",
               (state->NR_flag & WANT_NR) ? " NR" : "",
               (state->ND_flag & THEY_ND) ? " ND" : "",
               (!(state->ND_flag & WE_ND)) != (!(state->ND_flag & THEY_ND)) ? " NDA" : "",
               (state->crypt_flag == (WE_CRYPT | THEY_CRYPT)) ? " CRYPT" : "");
  msg_send2 (state, M_OK, state->state==P_SECURE ? "secure" : "non-secure", 0);
  complete_login (state);
  return 1;
}

static int OK (STATE *state, char *buf, int sz)
{
  if (!state->to) return 0;
  state->state = !strcmp (state->to->pwd, "-") ? P_NONSECURE : P_SECURE;
  if (state->state == P_SECURE && strcmp(buf, "non-secure") == 0)
  {
    state->crypt_flag=NO_CRYPT; /* some development binkd versions send OPT CRYPT with unsecure session */
    Log (1, "Warning: remote set UNSECURE session");
  }
  if (state->ND_flag == WE_ND || state->ND_flag == THEY_ND)
    state->ND_flag = 0; /* remote does not support asymmetric ND-mode */
  complete_login (state);
  return 1;
}

/*
 * Handles M_FILE msg from the remote
 * M_FILE args: "%s %li %li %li", filename, size, time, offset
 */
static int start_file_recv (STATE *state, char *args, int sz)
{
  const int argc = 4;
  char *argv[4];
  off_t offset;

  if (parse_msg_args (argc, argv, args, "M_FILE", state))
  {
    /* They request us for offset (M_FILE "name size time -1") */
    int off_req = 0;

    if (state->in.f &&		       /* Already receiving smthing */
	strcmp (argv[0], state->in.netname))	/* ...a file with another *
						 * name! */
    {
      Log (1, "receiving of %s interrupted", state->in.netname);
      close_partial (state);
    }
    if ((state->ND_flag & THEY_ND) && state->in_complete.netname[0])
    { /* rename complete received file to its true form */
      char realname[MAXPATHLEN + 1];
      char szAddr[FTN_ADDR_SZ + 1];

      if (inb_done (state->in_complete.netname, state->in_complete.size,
	            state->in_complete.time, state->fa, state->nallfa,
		    state->inbound, realname) == 0)
        return 0; /* error, drop session */
      if (*realname)
      {
        /* Set flags */
        if(evt_test(&(state->evt_queue), realname))
          state->q = evt_run(&(state->evt_queue), state->q, realname, state->fa,
	         state->nfa, state->state == P_SECURE, state->listed_flag,
                 state->peer_name, state);
        /* We will run external programs using this list */
        add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
      }
      ftnaddress_to_str (szAddr, state->fa);
      state->bytes_rcvd += state->in_complete.size;
      ++state->files_rcvd;
      Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in_complete.netname,
	   (long) state->in_complete.size,
	   (double) (state->in_complete.size) /
	   (safe_time() == state->in_complete.start ?
	                1 : (safe_time() - state->in_complete.start)), szAddr);
      TF_ZERO (&state->in_complete);
    }
    if (state->in.f == 0)
    {
      state->in.start = safe_time();
      strnzcpy (state->in.netname, argv[0], MAX_NETNAME);
      state->in.size = atol (argv[1]);
      state->in.time = atol (argv[2]);
    }
    offset = (off_t) atol (argv[3]);
    if (!strcmp (argv[3], "-1"))
    {
      off_req = 1;
      Log (6, "got offset request for %s", state->in.netname);
      if ((state->NR_flag & THEY_NR) == 0)
      {
	state->NR_flag |= THEY_NR;
	if ((state->ND_flag & THEY_ND) == 0)
	  Log (3, "remote is in NR mode");
      }
    }

    if (state->in.f == 0)
    {
      char realname[MAXPATHLEN + 1], *mask;

      if ((mask = mask_test (state->in.netname, skipmask)) != NULL)
      {
	Log (1, "skipping %s (destructive, %li byte(s), skipmask %s)",
	     state->in.netname, (long) state->in.size, mask);
	msg_sendf (state, M_GOT, "%s %li %li",
		   state->in.netname,
		   (long) state->in.size,
		   (long) state->in.time);
	return 1;
      }
      if (inb_test (state->in.netname, state->in.size,
		    state->in.time, state->inbound, realname))
      {
	Log (2, "already have %s (%s, %li byte(s))",
	     state->in.netname, realname, (long) state->in.size);
	msg_sendf (state, M_GOT, "%s %li %li",
		   state->in.netname,
		   (long) state->in.size,
		   (long) state->in.time);
	return 1;
      }
      else if (!state->skip_all_flag)
      {
	if ((state->in.f = inb_fopen (state->in.netname, state->in.size,
				      state->in.time, state->fa, state->nallfa,
				      state->inbound, state->state)) == 0)
	{
	  state->skip_all_flag = 1;
	}
      }

#if defined(DOS) && defined(__MSC__)
      if (!state->skip_all_flag &&
          (state->n_rcvdlist+1ul) * sizeof(RCVDLIST) > 64535ul)
      {
        Log (1, "ReceivedList has reached max size 64K");
        state->skip_all_flag = 1;
      }
#endif

      if (state->skip_all_flag)
      {
	Log (2, "skipping %s (non-destructive)", state->in.netname);
	msg_sendf (state, M_SKIP, "%s %li %li",
		   state->in.netname,
		   (long) state->in.size,
		   (long) state->in.time);
	if (state->in.f)
	  fclose (state->in.f);
	TF_ZERO (&state->in);
	return 1;
      }
    }

    if (off_req || offset != (off_t) ftell (state->in.f))
    {
      Log (2, "have %li byte(s) of %s",
	   (long) ftell (state->in.f), state->in.netname);
      msg_sendf (state, M_GET, "%s %li %li %li", state->in.netname,
		 (long) state->in.size, (long) state->in.time,
		 (long) ftell (state->in.f));
      ++state->GET_FILE_balance;
      fclose (state->in.f);
      TF_ZERO (&state->in);
      return 1;
    }
    else if (offset != 0 || (state->NR_flag & THEY_NR))
    {
      --state->GET_FILE_balance;
    }

    Log (3, "receiving %s (%li byte(s), off %li)",
	 state->in.netname, (long) (state->in.size), (long) offset);

    if (fseek (state->in.f, offset, SEEK_SET) == -1)
    {
      Log (1, "fseek: %s", strerror (errno));
      return 0;
    }
    else
      return 1;
  }
  else
    return 0;
}

static int ND_set_status(char *status, FTN_ADDR *fa, STATE *state)
{
  char buf[MAXPATHLEN+1];
  FILE *f;
  int  rc;

  if ((state->NR_flag & WE_NR) == 0)
    return 1; /* ignoring status if no NR mode */ 
  if (fa->z==-1)
  { Log(8, "ND_set_status: unknown address for '%s'", status);
    return 0;
  }
  Log(4, "Set link status with %u:%u/%u.%u to '%s'",
      fa->z, fa->net, fa->node, fa->p, status ? status : "");
  ftnaddress_to_filename (buf, fa);
  if (*buf=='\0') return 0;
  strnzcat(buf, ".stc", sizeof(buf));
  if (!status || !*status)
    return (unlink(buf) && errno != ENOENT) ? 0 : 1;
  else
  {
    f=fopen(buf, "w");
    if (f==NULL)
    { Log(1, "Can't write to %s: %s", buf, strerror(errno));
      return 0;
    }
    rc=1;
    if (fputs(status, f)==EOF)
      rc=0;
    if (fclose(f)==EOF)
      rc=0;
    return rc;
  }
}

/*
 * M_GET message from the remote: Resend file from offset
 * M_GET args: "%s %li %li %li", filename, size, time, offset
 */
static int GET (STATE *state, char *args, int sz)
{
  const int argc = 4;
  char *argv[4];
  int i, rc = 0;
  off_t offset;

  if (parse_msg_args (argc, argv, args, "M_GET", state))
  {
    /* Check if the file was already sent */
    for (i = 0; i < state->n_sent_fls; ++i)
    {
      if (!tfile_cmp (state->sent_fls + i, argv[0], atol (argv[1]), atol (argv[2])))
      {
	if (state->out.f)
	{
	  TFILE tfile_buf;
	  fclose (state->out.f);
	  state->out.f = NULL;
	  memcpy (&tfile_buf, &state->out, sizeof (TFILE));
	  memcpy (&state->out, state->sent_fls + i, sizeof (TFILE));
	  memcpy (state->sent_fls + i, &tfile_buf, sizeof (TFILE));
	}
	else
	{
	  memcpy (&state->out, state->sent_fls + i, sizeof (TFILE));
	  remove_from_sent_files_queue (state, i);
	}
	if ((state->out.f = fopen (state->out.path, "rb")) == 0)
	{
	  Log (1, "GET: %s: %s", state->out.path, strerror (errno));
	  TF_ZERO (&state->out);
	}
	break;
      }
    }

    if ((state->out.f || state->off_req_sent) &&
         !tfile_cmp (&state->out, argv[0], atol (argv[1]), atol (argv[2])))
    {
      if (!state->out.f)
      { /* response for status */
	rc = 1;
	/* to satisfy remote GET_FILE_balance */
	msg_sendf (state, M_FILE, "%s %li %li %li", state->out.netname,
	   (long) state->out.size, (long) state->out.time, atol(argv[3]));
	if (atol(argv[3])==(long)state->out.size && (state->ND_flag & WE_ND))
	{
	  state->send_eof = 1;
	  state->waiting_for_GOT = 1;
	  Log(4, "Waiting for M_GOT");
	  state->off_req_sent = 0;
	  return rc;
	}
	else
	  /* request from offset 0 - file already renamed */
	  ND_set_status("", &state->out_addr, state);
	TF_ZERO(&state->out);
      }
      else if ((offset = atol (argv[3])) > state->out.size ||
               fseek (state->out.f, offset, SEEK_SET) == -1)
      {
	Log (1, "GET: error seeking %s to %li: %s",
	     argv[0], offset, strerror (errno));
	/* touch the file and drop session */
	fclose(state->out.f);
	state->out.f=NULL;
	touch(state->out.path, time(NULL));
	rc = 0;
      }
      else
      {
	Log (2, "sending %s from %li", argv[0], offset);
	msg_sendf (state, M_FILE, "%s %li %li %li", state->out.netname,
		   (long) state->out.size, (long) state->out.time, offset);
	rc = 1;
      }
    }
    else
      Log (1, "unexpected M_GET for %s", argv[0]);
    ND_set_status("", &state->ND_addr, state);
    state->ND_addr.z=-1;
    if (state->ND_flag & WE_ND)
    {
      state->waiting_for_GOT = 0;
      Log(9, "Don't waiting for M_GOT");
    }
    state->off_req_sent = 0;

    return rc;
  }
  else
    return 0;
}

/*
 * M_SKIP: Remote asks us to skip a file. Only a file currently in
 * transfer will be skipped!
 *
 * M_SKIP args: "%s %li %li", filename, size, time
 */
static int SKIP (STATE *state, char *args, int sz)
{
  const int argc = 3;
  char *argv[3];
  int n;

  if (parse_msg_args (argc, argv, args, "M_SKIP", state))
  {
    for (n = 0; n < state->n_sent_fls; ++n)
    {
      if (!tfile_cmp (state->sent_fls + n, argv[0], atol (argv[1]), atol (argv[2])))
      {
	state->r_skipped_flag = 1;
	Log (2, "%s skipped by remote", state->sent_fls[n].netname);
	remove_from_sent_files_queue (state, n);
      }
    }
    if (state->out.f && !tfile_cmp (&state->out, argv[0],
				    atol (argv[1]), atol (argv[2])))
    {
      state->r_skipped_flag = 1;
      fclose (state->out.f);
      Log (2, "%s skipped by remote", state->out.netname);
      TF_ZERO (&state->out);
    }
    ND_set_status("", &state->ND_addr, state);
    state->ND_addr.z=-1;
    if ((state->ND_flag & WE_ND) || (state->NR_flag & WE_NR))
    {
      state->waiting_for_GOT = state->off_req_sent = 0;
      Log(9, "Don't waiting for M_GOT");
    }
    return 1;
  }
  else
    return 0;
}

/*
 * M_GOT args: "%s %li %li", filename, size, time
 */
static int GOT (STATE *state, char *args, int sz)
{
  const int argc = 3;
  char *argv[3];
  int n, rc=1;
  char *status = NULL;

  if (parse_msg_args (argc, argv, args, "M_GOT", state))
  {
    if (state->ND_flag & WE_ND)
      status = strdup(args);
    else
      ND_set_status("", &state->ND_addr, state);
    if (!tfile_cmp (&state->out, argv[0], atol (argv[1]), atol (argv[2])))
    {
      Log (2, "remote already has %s", state->out.netname);
      if (state->out.f)
      {
	fclose (state->out.f);
	state->out.f = NULL;
      }
      memcpy(&state->ND_addr, &state->out_addr, sizeof(state->out_addr));
      if (state->ND_flag & WE_ND)
        Log (7, "Set ND_addr to %u:%u/%u.%u",
             state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
      if (status)
      {
	if (state->off_req_sent)
	  rc = ND_set_status("", &state->ND_addr, state);
	else
	  rc = ND_set_status(status, &state->ND_addr, state);
      }
      state->waiting_for_GOT = state->off_req_sent = 0;
      Log(9, "Don't waiting for M_GOT");
      remove_from_spool (state, state->out.flo,
			 state->out.path, state->out.action);
      TF_ZERO (&state->out);
    }
    else
    {
      for (n = 0; n < state->n_sent_fls; ++n)
      {
	if (!tfile_cmp (state->sent_fls + n, argv[0], atol (argv[1]), atol (argv[2])))
	{
	  char szAddr[FTN_ADDR_SZ + 1];

	  ftnaddress_to_str (szAddr, &state->out_addr);
	  state->bytes_sent += state->sent_fls[n].size;
	  ++state->files_sent;
          memcpy(&state->ND_addr, &state->out_addr, sizeof(state->out_addr));
          if (state->ND_flag & WE_ND)
             Log (7, "Set ND_addr to %u:%u/%u.%u",
                  state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
	  Log (2, "sent: %s (%li, %.2f CPS, %s)", state->sent_fls[n].path,
	       (long) state->sent_fls[n].size,
	       (double) (state->sent_fls[n].size) /
	       (safe_time() == state->sent_fls[n].start ?
		1 : (safe_time() - state->sent_fls[n].start)), szAddr);
	  if (status)
	  {
	    if (state->off_req_sent)
	      rc = ND_set_status("", &state->ND_addr, state);
	    else
	      rc = ND_set_status(status, &state->ND_addr, state);
	  }
	  state->waiting_for_GOT = 0;
	  Log(9, "Don't waiting for M_GOT");
	  remove_from_spool (state, state->sent_fls[n].flo,
			state->sent_fls[n].path, state->sent_fls[n].action);
	  remove_from_sent_files_queue (state, n);
	  break;		       /* we have ACK for _ONE_ file */
	}
      }
    }
    if (status) free(status);
    return rc;
  }
  else
    return 0;
}

static int EOB (STATE *state, char *buf, int sz)
{
  state->remote_EOB = 1;
  if (state->in.f)
  {
    fclose (state->in.f);
    Log (1, "receiving of %s interrupted", state->in.netname);
    TF_ZERO (&state->in);
  }
  if ((state->ND_flag & THEY_ND) && state->in_complete.netname[0])
  { /* rename complete received file to its true form */
    char realname[MAXPATHLEN + 1];
    char szAddr[FTN_ADDR_SZ + 1];

    if (inb_done (state->in_complete.netname, state->in_complete.size,
                  state->in_complete.time, state->fa, state->nallfa,
	          state->inbound, realname) == 0)
      return 0;
    if (*realname)
    {
      /* Set flags */
      if (evt_test(&(state->evt_queue), realname))
        state->q = evt_run(&(state->evt_queue), state->q, realname, state->fa,
	       state->nfa, state->state == P_SECURE, state->listed_flag,
               state->peer_name, state);
      /* We will run external programs using this list */
      add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
    }
    ftnaddress_to_str (szAddr, state->fa);
    state->bytes_rcvd += state->in_complete.size;
    ++state->files_rcvd;
    Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in_complete.netname,
         (long) state->in_complete.size,
         (double) (state->in_complete.size) /
         (safe_time() == state->in_complete.start ?
                      1 : (safe_time() - state->in_complete.start)), szAddr);
    TF_ZERO (&state->in_complete);
  }
  return 1;
}

typedef int command (STATE *state, char *buf, int sz);
command *commands[] =
{
  NUL, ADR, PWD, start_file_recv, OK, EOB, GOT, RError, BSY, GET, SKIP
};

/* Recvs next block, processes msgs or writes down the data from the remote */
static int recv_block (STATE *state)
{
  int no;

  int sz = state->isize == -1 ? BLK_HDR_SIZE : state->isize;

  if (sz == 0)
    no = 0;
  else if ((no = recv (state->s, state->ibuf + state->iread,
		       sz - state->iread, 0)) == SOCKET_ERROR)
  {
    if (TCPERRNO == TCPERR_WOULDBLOCK || TCPERRNO == TCPERR_AGAIN)
    {
      return 1;
    }
    else
    {
      const char *save_err = TCPERR();
      state->io_error = 1;
      if (!binkd_exit)
      {
	Log (1, "recv: %s", save_err);
	if (state->to)
	  bad_try (&state->to->fa, save_err);
      }
      return 0;
    }
  }
  if (state->crypt_flag == YES_CRYPT)
    decrypt_buf(state->ibuf + state->iread, no, state->keys_in);
  state->iread += no;
  /* assert (state->iread <= sz); */
  if (state->iread == sz)
  {
    if (state->isize == -1)	       /* reading block header */
    {
      state->imsg = state->ibuf[0] >> 7;
      state->isize = ((((unsigned char *) state->ibuf)[0] & ~0x80) << 8) +
	((unsigned char *) state->ibuf)[1];
      Log (7, "recvd hdr: %i (%s)", state->isize, state->imsg ? "msg" : "data");
      if (state->isize == 0)
	goto DoNotEvenTryToRecvZeroLengthBlock;
    }
    else
    {
  DoNotEvenTryToRecvZeroLengthBlock:
      Log (7, "got block: %i (%s)", state->isize, state->imsg ? "msg" : "data");
      if (state->imsg)
      {
	int rc = 1;

	++state->msgs_in_batch;

	if (state->isize == 0)
	  Log (1, "zero length command from remote (must be at least 1)");
	else if ((unsigned) (state->ibuf[0]) > M_MAX)
	  Log (1, "unknown msg type from remote: %u", state->ibuf[0]);
	else
	{
	  state->ibuf[state->isize] = 0;
          Log (5, "rcvd msg %s %s", scommand[(unsigned char)(state->ibuf[0])], state->ibuf+1);
	  rc = commands[(unsigned) (state->ibuf[0])]
	    (state, state->ibuf + 1, state->isize - 1);
	}

	if (rc == 0)
	{
	  state->iread = 0;
	  return 0;
	}
      }
      else if (state->in.f)
      {
	if (state->isize != 0 &&
	    (fwrite (state->ibuf, state->isize, 1, state->in.f) < 1 ||
	     fflush (state->in.f)))
	{
	  Log (1, "write error: %s", strerror(errno));
	  return 0;
	}
	if (percents && state->in.size > 0)
	{
	  printf ("%-20.20s %3.0f%%\r", state->in.netname,
		  100.0 * ftell (state->in.f) / (float) state->in.size);
	  fflush (stdout);
	}
	if ((off_t) ftell (state->in.f) == state->in.size)
	{
	  char szAddr[FTN_ADDR_SZ + 1];
	  char realname[MAXPATHLEN + 1];

	  if (fclose (state->in.f))
	  {
	    Log (1, "Cannot fclose(%s): %s!",
	         state->in.netname, strerror (errno));
	    state->in.f = NULL;
	    return 0;
	  }
	  state->in.f = NULL;
	  if (state->ND_flag & THEY_ND)
	  {
	    Log (5, "File %s complete received, waiting for renaming",
	         state->in.netname);
	    memcpy(&state->in_complete, &state->in, sizeof(state->in_complete));
	  }
	  else
	  {
	    if (inb_done (state->in.netname, state->in.size,
		          state->in.time, state->fa, state->nallfa,
		          state->inbound, realname) == 0)
              return 0;
	    if (*realname)
	    {
	      /* Set flags */
              if (evt_test(&(state->evt_queue), realname))
                state->q = evt_run(&(state->evt_queue), state->q, realname,
		       state->fa, state->nfa, state->state == P_SECURE,
		       state->listed_flag, state->peer_name, state);
	      /* We will run external programs using this list */
	      add_to_rcvdlist (&state->rcvdlist, &state->n_rcvdlist, realname);
	    }
	    ftnaddress_to_str (szAddr, state->fa);
	    state->bytes_rcvd += state->in.size;
	    ++state->files_rcvd;
	    Log (2, "rcvd: %s (%li, %.2f CPS, %s)", state->in.netname,
	         (long) state->in.size,
	         (double) (state->in.size) /
	         (safe_time() == state->in.start ?
		  1 : (safe_time() - state->in.start)), szAddr);
	  }
	  msg_sendf (state, M_GOT, "%s %li %li",
		     state->in.netname,
		     (long) state->in.size,
		     (long) state->in.time);
	  TF_ZERO (&state->in);
	}
	else if ((off_t) ftell (state->in.f) > state->in.size)
	{
	  Log (1, "rcvd %li extra bytes!", (long) ftell (state->in.f) - state->in.size);
	  return 0;
	}
      }
      else if (state->isize > 0)
      {
	Log (7, "ignoring data block (%li byte(s))", (long) state->isize);
      }
      state->isize = -1;
    }
    state->iread = 0;
  }
  if (no == 0 && sz > 0)
  {
    state->io_error = 1;
    if (!binkd_exit)
    { 
      const char *s_err = "connection closed by foreign host";
      Log (1, "recv: %s", s_err);
      if (state->to)
	bad_try (&state->to->fa, s_err);
    }
    return 0;
  }
  else
    return 1;
}

static void banner (STATE *state)
{
  char szFTNAddr[FTN_ADDR_SZ + 1];
  char *szAkas;
  int i, tz;
  char szLocalTime[60];
  time_t t, gt;
  struct tm tm;
  char *dayweek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  if ((!no_MD5) && (!state->to) &&
      ((state->MD_challenge=MD_getChallenge(NULL, state))!=NULL))
  {  /* Answering side MUST send CRAM message as a very first M_NUL */
    char s[MD_CHALLENGE_LEN*2+15]; /* max. length of opt string */
    strcpy(s, "OPT ");
    MD_toString(s+4, state->MD_challenge[0], state->MD_challenge+1);
    state->MD_flag=1;
    msg_send2 (state, M_NUL, s, "");
  }
  else
    state->MD_flag=0;

  msg_send2 (state, M_NUL, "SYS ", sysname);
  msg_send2 (state, M_NUL, "ZYZ ", sysop);
  msg_send2 (state, M_NUL, "LOC ", location);
  msg_send2 (state, M_NUL, "NDL ", nodeinfo);

  tzset();
  t = safe_time();
  safe_gmtime (&t, &tm);
  tm.tm_isdst = 0;
  gt = mktime(&tm);
  safe_localtime (&t, &tm);
  tm.tm_isdst = 0;
  tz = (int)(((long)mktime(&tm)-(long)gt)/60);
  safe_localtime (&t, &tm);

#if 0
  sprintf (szLocalTime, "%s, %2d %s %d %02d:%02d:%02d %c%02d%02d (%s)",
           dayweek[tm->tm_wday], tm->tm_mday, month[tm->tm_mon],
           tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec,
           (tz>=0) ? '+' : '-', abs(tz)/60, abs(tz)%60,
           tzname[tm->tm_isdst>0 ? 1 : 0]);
#else
  sprintf (szLocalTime, "%s, %2d %s %d %02d:%02d:%02d %c%02d%02d",
           dayweek[tm.tm_wday], tm.tm_mday, month[tm.tm_mon],
           tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
           (tz>=0) ? '+' : '-', abs(tz)/60, abs(tz)%60);
#endif

  msg_send2 (state, M_NUL, "TIME ", szLocalTime);

  msg_sendf (state, M_NUL,
    "VER " MYNAME "/" MYVER "%s " PRTCLNAME "/" PRTCLVER, get_os_string ());

  szAkas = xalloc (nAddr * (FTN_ADDR_SZ + 1));
  *szAkas = 0;
  for (i = 0; i < nAddr; ++i)
  {
    ftnaddress_to_str (szFTNAddr, pAddr + i);
    strcat (szAkas, " ");
    strcat (szAkas, szFTNAddr);
  }
  msg_send2 (state, M_ADR, szAkas, 0);

  if (state->to)
    msg_sendf (state, M_NUL, "OPT NDA%s%s%s",
               (state->NR_flag & WANT_NR) ? " NR" : "",
               (state->ND_flag & THEY_ND) ? " ND" : "",
               (state->crypt_flag & WE_CRYPT) ? " CRYPT" : "");
  free (szAkas);
}

static int start_file_transfer (STATE *state, FTNQ *file)
{
  struct stat sb;
  FILE *f = NULL;
  int action = -1;

  if (state->out.f)
    fclose (state->out.f);
  TF_ZERO (&state->out);	       /* No file in transfer */

  if (state->flo.f == 0)	       /* There is no open .?lo */
  {
    state->maxflvr = MAXFLVR (state->maxflvr, file->flvr);
    /* Try to open the suggested file */
    if (file->type == 's')
    {
      sb.st_size  = file->size;
      sb.st_ctime = sb.st_mtime = file->time;
      f = NULL;
    }
    else
    {
      if ((f = fopen (file->path, (file->type == 'l') ? "r+b" : "rb")) == 0 ||
	  fstat (fileno (f), &sb) == -1)
      {
        Log (1, "%s: cannot open: %s", file->path, strerror (errno));
        return 0;
      }
      /* We've opened a .?lo */
      if (file->type == 'l')
      {
        state->flo.action = file->action;
        strcpy (state->flo.path, file->path);
        state->flo.f = f;
      }
    }
    memcpy(&state->out_addr, &file->fa, sizeof(state->out_addr));
    if ((state->ND_flag & WE_ND) == 0)
      memcpy(&state->ND_addr, &file->fa, sizeof(state->out_addr));
    Log (8, "cur remote addr is %u:%u/%u.%u",
         file->fa.z, file->fa.net, file->fa.node, file->fa.p);
  }
  if (state->flo.f != 0)
  {
    strcpy (state->out.flo, state->flo.path);
    while (1)
    {
      char *w;

      if (!read_flo_line (state->out.path, &action, state->flo.f))
      {
	fclose (state->flo.f);
	state->flo.f = 0;
	/* .?lo closed, remove_from_spool() will now unlink it */
	remove_from_spool (state, state->flo.path, 0, 0);
	TF_ZERO (&state->flo);
	return 0;
      }

      if ((w = trans_flo_line (state->out.path)) != 0)
	Log (5, "%s mapped to %s", state->out.path, w);

      if ((f = fopen (w ? w : state->out.path, "rb")) == 0 ||
	  fstat (fileno (f), &sb) == -1 ||
	  (sb.st_mode & S_IFDIR) != 0)
      {
	Log (1, "start_file_transfer: %s: %s",
	     w ? w : state->out.path, strerror (errno));
        if (f) fclose(f);
	if (w) free (w);
	remove_from_spool (state, state->out.flo,
			   state->out.path, state->out.action);
      }
      else
      {
	if (w) free (w);
	break;
      }
    }
  }

  if (action == -1)
  {
    strcpy (state->out.path, file->path);
    state->out.flo[0] = 0;
    state->out.action = file->action;
    state->out.type = file->type;
  }
  else
  {
    state->out.action = action;
    state->out.type = 0;
  }
  state->out.f = f;
  state->out.size = sb.st_size;
  state->out.time = sb.st_mtime;
  state->waiting_for_GOT = 0;
  Log(9, "Dont waiting for M_GOT");
  state->out.start = safe_time();
  netname (state->out.netname, &state->out);
  if (ispkt(state->out.netname) && state->out.size <= 60)
  {
    Log (3, "skip empty pkt %s, %li bytes", state->out.path, state->out.size);
    if (state->out.f) fclose(state->out.f);
    remove_from_spool (state, state->out.flo,
			 state->out.path, state->out.action);
    TF_ZERO (&state->out);
    return 0;
  }
  Log (3, "sending %s as %s (%li)",
       state->out.path, state->out.netname, (long) state->out.size);

  if (state->NR_flag & WE_NR)
  {
    msg_sendf (state, M_FILE, "%s %li %li -1",
	       state->out.netname, (long) state->out.size,
	       (long) state->out.time);
    state->off_req_sent = 1;
  }
  else
    msg_sendf (state, M_FILE, "%s %li %li 0",
	       state->out.netname, (long) state->out.size,
	       (long) state->out.time);

  return 1;
}

void log_end_of_session (char *status, STATE *state)
{
  char szFTNAddr[FTN_ADDR_SZ + 1];

  BinLogStat (status, state);

  if (state->to)
    ftnaddress_to_str (szFTNAddr, &state->to->fa);
  else if (state->fa)
    ftnaddress_to_str (szFTNAddr, state->fa);
  else
    strcpy (szFTNAddr, "?");

  Log (2, "done (%s%s, %s, S/R: %i/%i (%lu/%lu bytes))",
       state->to ? "to " : (state->fa ? "from " : ""), szFTNAddr,
       status,
       state->files_sent, state->files_rcvd,
       state->bytes_sent, state->bytes_rcvd);
}

void protocol (SOCKET socket, FTN_NODE *to, char *current_addr)
{
  STATE state;
  struct timeval tv;
  fd_set r, w;
  int no, rd;
#ifdef WIN32
  unsigned long t_out = 0;
  unsigned long u_nettimeout = nettimeout*1000000l;
#endif
  struct sockaddr_in peer_name;
  socklen_t peer_name_len = sizeof (peer_name);
  char host[MAXHOSTNAMELEN + 1];
  const char *save_err = NULL;

  if (!init_protocol (&state, socket, to))
    return;

  if (getpeername (socket, (struct sockaddr *) & peer_name, &peer_name_len) == -1)
  {
    Log (1, "getpeername: %s", TCPERR ());
    memset(&peer_name, 0, sizeof (peer_name));
  }

  if (to && current_addr)
    state.peer_name = current_addr;
  else
  {
    get_hostname(&peer_name, host, sizeof(host));
    state.peer_name = host;
  }
  setproctitle ("%c [%s]", to ? 'o' : 'i', state.peer_name);
  lockhostsem();
  Log (2, "session with %s (%s)",
       state.peer_name,
       inet_ntoa (peer_name.sin_addr));
  releasehostsem();

  if (getsockname (socket, (struct sockaddr *) & peer_name, &peer_name_len) == -1)
  {
    Log (1, "getsockname: %s", TCPERR ());
    memset(&peer_name, 0, sizeof (peer_name));
  }
  else 
    state.our_ip=peer_name.sin_addr.s_addr;

  banner (&state);
  if (n_servers > max_servers && !to)
  {
    Log (1, "too many servers");
    msg_send2 (&state, M_BSY, "Too many servers", 0);
  }
  else
  {
    while (1)
    {
      /* If the queue is not empty and there is no file in tranafer */
      if (!state.local_EOB && state.q && state.out.f == 0 &&
          !state.waiting_for_GOT && !state.off_req_sent && state.state!=P_NULL)
      {
	FTNQ *q;

	while (1)
	{			       /* Next .pkt, .flo or a file */
	  q = 0;
	  if (state.flo.f ||
	      (q = select_next_file (state.q, state.fa, state.nfa)) != 0)
	  {
	    if (q && (q->type=='s') && (state.NR_flag & WE_NR) == 0)
	    { /* TODO: wait for send queue and switch to NR mode */
	      Log(1, "WARNING: status present and no NR mode!");
	      continue;
	    }
	    if (start_file_transfer (&state, q))
	      break;
	  }
	  else
	  {
	    q_free (state.q);
	    state.q = 0;
	    break;
	  }
	}
      }

      /* No more files to send in this batch, so send EOB */
      if (!state.q && !state.local_EOB && state.state != P_NULL && state.sent_fls == 0)
      {
	state.local_EOB = 1;
	msg_send2 (&state, M_EOB, 0, 0);
      }

      FD_ZERO (&r);
      FD_ZERO (&w);
      FD_SET (socket, &r);
      if (state.msgs ||
          (state.out.f && !state.off_req_sent && !state.waiting_for_GOT) ||
          state.oleft || state.send_eof)
	FD_SET (socket, &w);

      if (state.remote_EOB && state.sent_fls == 0 && state.local_EOB &&
	  state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
      {
	/* End of the current batch */
	if (state.rcvdlist)
	{
	  state.q = process_rcvdlist (&state, state.q);
	  q_to_killlist (&state.killlist, &state.n_killlist, state.q);
	  free_rcvdlist (&state.rcvdlist, &state.n_rcvdlist);
	}
	Log (6, "there were %i msgs in this batch", state.msgs_in_batch);
	if (state.msgs_in_batch <= 2 || (state.major * 100 + state.minor <= 100))
	{
          ND_set_status("", &state.ND_addr, &state);
          state.ND_addr.z=-1;
	  break;
	}
	else
	{
	  /* Start the next batch */
	  state.msgs_in_batch = 0;
	  state.remote_EOB = state.local_EOB = 0;
	  if (OK_SEND_FILES (&state))
	    state.q = q_scan_boxes (state.q, state.fa, state.nfa);
	  continue;
	}
      }

#if defined(WIN32) /* workaround winsock bug */
    if (t_out < u_nettimeout)
    {
#endif
      tv.tv_sec = nettimeout;	       /* Set up timeout for select() */
      tv.tv_usec = 0;
      Log (8, "tv.tv_sec=%li, tv.tv_usec=%li", (long) tv.tv_sec, (long) tv.tv_usec);
      no = select (socket + 1, &r, &w, 0, &tv);
      if (no < 0)
        save_err = TCPERR ();
      Log (8, "selected %i (r=%i, w=%i)", no, FD_ISSET (socket, &r), FD_ISSET (socket, &w));
#if defined(WIN32) /* workaround winsock bug */
    }
    else
    {
      Log (8, "win9x winsock workaround: timeout detected (nettimeout=%u sec, t_out=%lu sec)", nettimeout, t_out/1000000);
      no = 0;
    }
#endif
      bsy_touch ();		       /* touch *.bsy's */
      if (no == 0)
      {
	state.io_error = 1;
	Log (1, "timeout!");
	if (to)
	  bad_try (&to->fa, "Timeout!");
	break;
      }
      else if (no < 0)
      {
	state.io_error = 1;
	if (!binkd_exit)
	{
	  Log (1, "select: %s (args: %i %i)", save_err, socket, tv.tv_sec);
	  if (to)
	    bad_try (&to->fa, save_err);
	}
	break;
      }
      rd = FD_ISSET (socket, &r);
      if (rd)       /* Have something to read */
      {
	if (!recv_block (&state))
	  break;
      }
      if (FD_ISSET (socket, &w))       /* Clear to send */
      {
	no = send_block (&state);
#if defined(WIN32) /* workaround winsock bug - give up CPU */
        if (!rd && no == 2)
        {
	  FD_ZERO (&r);
	  FD_SET (socket, &r);
	  tv.tv_sec = 0;
	  tv.tv_usec = w9x_workaround_sleep; /* see iphdr.h */
	  if (!select (socket + 1, &r, 0, 0, &tv))
	  {
	    t_out += w9x_workaround_sleep;
	  } else { t_out = 0; }
        }
        else { t_out = 0; }
#endif
        if (!no)
          break;
      }
#if defined(WIN32) /* workaround winsock bug - give up CPU */
      else { t_out = 0; }
#endif
    }
  }

  /* Flush input queue */
  while (!state.io_error)
  {
    if ((no = recv(socket, state.ibuf, BLK_HDR_SIZE + MAX_BLKSIZE, 0)) == 0)
      break;
    if (no < 0)
    {
      if (TCPERRNO != TCPERR_WOULDBLOCK && TCPERRNO != TCPERR_AGAIN)
	state.io_error = 1;
      break;
    }
    else
      Log (9, "Purged %d bytes from input queue", no);
  }

  /* Still have something to send */
  while (!state.io_error &&
        (state.msgs || (state.optr && state.oleft)) && send_block (&state));

  if (state.local_EOB && state.remote_EOB && state.sent_fls == 0 &&
      state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
  {
    /* Successful session */
    log_end_of_session ("OK", &state);
    process_killlist (state.killlist, state.n_killlist, 's');
    if (to)
      good_try (&to->fa, "CONNECT/BND");
  }
  else
  {
    /* Unsuccessful session */
    log_end_of_session ("failed", &state);
    process_killlist (state.killlist, state.n_killlist, 0);
    if (to)
    {
      /* We called and there were still files in transfer -- restore poll */
      if (tolower (state.maxflvr) != 'h')
      {
	Log (4, "restoring poll with `%c' flavour", state.maxflvr);
	create_poll (&state.to->fa, state.maxflvr);
      }
    }
  }

  if (to && state.r_skipped_flag && hold_skipped > 0)
  {
    Log (2, "holding skipped mail for %li sec", (long) hold_skipped);
    hold_node (&to->fa, safe_time() + hold_skipped);
  }

  Log (4, "session closed, quitting...");
  deinit_protocol (&state);
  evt_set (state.evt_queue);
  state.evt_queue = NULL;
}
