/*--------------------------------------------------------------------*/
/*       B i n L o g . h                                              */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Binary log interface                                         */
/*                                                                    */
/*       Definition file.                                             */
/*--------------------------------------------------------------------*/
/*
 * $Id$
 *
 * $Log$
 * Revision 2.1  2003/08/16 09:08:33  gul
 * Binlog semaphoring removed
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef __BINLOG_H__
#define __BINLOG_H__

#ifdef STATE_DEFINED
void BinLogStat (char *status, STATE *state);
#endif

#endif
