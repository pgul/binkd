/*
 * $Id$
 *
 * $Log$
 * Revision 2.7  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.6  2003/05/30 17:15:22  gul
 * Asymmetric ND-mode, new protocol option NDA
 *
 * Revision 2.5  2003/05/30 16:03:10  gul
 * Asymmetric NR-mode
 *
 * Revision 2.4  2003/03/05 13:21:51  gul
 * Fix warnings
 *
 * Revision 2.3  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
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
 *
 */
/*

   binkp's frames:

    +---------------------- 0=data block, 1=message(command)
    |                +---- data block size / msg's argument size
    |                |
    7  6543210 76543210
   +-+-------+--------+--- ... ---+
   | |   HI      LO   |           | -- data block / msg's argument
   +-+-------+--------+--- ... ---+

 */

#ifndef _PROTOCO2_H_
#define _PROTOCO2_H_

#define BLK_HDR_SIZE 2

typedef unsigned char t_msg;

#define M_NUL  0			    /* Ignored by binkp (data *
					       optionally Logged) */
#define M_ADR  1
#define M_PWD  2
#define M_FILE 3
#define M_OK   4			    /* The password is ok (data *
					       ignored) */
#define M_EOB  5			    /* End-of-batch (data ignored) */
#define M_GOT  6			    /* File received */
#define M_ERR  7			    /* Misc errors */
#define M_BSY  8			    /* All AKAs are busy */
#define M_GET  9			    /* Get a file from offset */
#define M_SKIP 10			    /* Skip a file */
#define M_MAX  10

typedef struct _BMSG BMSG;
struct _BMSG
{
  t_msg t;			/* msg type */
  int sz;			/* msg size */
  char *s;			/* msg text */
};

/* Protocol's state */
typedef struct _STATE STATE;
struct _STATE
{
  SOCKET s;
  FTN_NODE *to;			/* Dest. address (if an outbound *
				   connection) */
  char expected_pwd[MAXPWDLEN + 1];

  /* binkp buffres */
  char *obuf;
  char *optr;			/* Next byte to send */
  int oleft;			/* Bytes left to send at optr */

  char *ibuf;
  int isize;			/* Current block's size. * -1=expecting block 
				   header */
  int iread;			/* Number of bytes read already */
  int imsg;			/* 0=data block, * 1=message(command) */

  /* binkp queues and data */
  BMSG *msgs;			/* Output msg queue */
  int n_msgs;
  TFILE in, out;		/* Files in transfer */
  TFILE flo;			/* A .?lo in transfer */
  TFILE *sent_fls;		/* Sent files: waiting for GOT */
  int n_sent_fls;		/* The number of... */
  FTNQ *q;			/* Queue */
  FTN_ADDR *fa;			/* Foreign akas */
  int nfa;			/* How many... */
  int nallfa;			/* How many remote akas including busy & n/a */
#ifdef WITH_PERL
  FTN_ADDR *pAddr;              /* Our aka's, NULL = use from config */
  int nAddr;                    /* How many our aka's, 0 = use from config */
#endif
  int maxflvr;
  int skip_all_flag;		/* We'd skip all */
  int r_skipped_flag;		/* Remote skipped smthng */
  int listed_flag;              /* Listed? */
  char *inbound;		/* The current inbound dir */
  char *peer_name;              /* Remote host's name */
  unsigned long our_ip;         /* Local IP */
  int io_error;
  int msgs_in_batch;
  int minor, major;		/* Version of remote binkp */
  KILLLIST *killlist;           /* Files to kill _after_ session */
  int n_killlist;               
  RCVDLIST *rcvdlist;           /* List of files rcvd in the curr.batch */
  int n_rcvdlist;       
        
  /* binkp stats */
  int files_sent;               /* Files sent/rcvd during the session */
  int files_rcvd;
  unsigned long bytes_sent;
  unsigned long bytes_rcvd;
  time_t   start_time;          /* Start of session */
  char sysname[MAXSYSTEMNAME + 1];
  char sysop[MAXSYSOPNAME + 1];
  char location[MAXLOCATIONNAME + 1];

  /* binkp state description */
  int local_EOB, remote_EOB;
  int GET_FILE_balance;		/* Prevent quitting without * waiting for GET 
				   results */
  int off_req_sent;		/* "M_FILE ... -1" sent, waiting * for M_GET */
  int state, state_ext;
  int NR_flag;                  /* State of binkp when in NR mode */

  int ND_flag;                  /* State of binkp when in ND mode */
  int MD_flag;                  /* State of binkp when in MD mode */
  unsigned char *MD_challenge;  /* Received CRAM challenge data   */
  int waiting_for_GOT;          /* File sent, waiting for M_GOT in ND-mode */
  int send_eof;			/* Need to send zero-length data block */
  char *ND_status;              /* ND status file name */
  TFILE in_complete;            /* M_GOT sent, need to rename */
  FTN_ADDR ND_addr;             /* Address for current ND status */
  FTN_ADDR out_addr;            /* Currently precessing remote AKA */
  int crypt_flag;		/* Is session encrypted? */
  unsigned long keys_out[3];	/* Encription keys for outbound */
  unsigned long keys_in[3];	/* Encription keys for inbound */
  EVTQ *evt_queue;		/* Queue of by-files events */
#ifdef WITH_PERL
  int perl_set_lvl;             /* Level of already set Perl vars */
#endif
};
#define STATE_DEFINED 1

/* 
 * Do we have to give up files for this node in this session? We send files if
 *    send-if-pwd not set or
 *    it's a secure session or
 *    it's an outbound session AND they don't skip what we send
 */
#define OK_SEND_FILES(S) \
         ((send_if_pwd == 0 || (S)->state == P_SECURE || (S)->to) \
          && (S)->r_skipped_flag == 0)

/* state.NR_flag: state of binkp when in NR mode */
#define NO_NR	0
#define WANT_NR	1
#define WE_NR	2
#define THEY_NR	4

/* state.ND_flag: state of binkp when in ND mode */
#define NO_ND	0
#define WE_ND	1
#define THEY_ND	2
#define CAN_NDA 4	/* they supports asymmetric ND-mode */

/* state.crypt_flag: state of binkp when in crypt mode */
#define NO_CRYPT	0
#define WE_CRYPT	1
#define THEY_CRYPT	2
#define YES_CRYPT	4

#endif
