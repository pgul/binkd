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
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef __BINLOG_H__
#define __BINLOG_H__

void BinLogInit(void);
void BinLogDeInit(void);
#ifdef STATE_DEFINED
void BinLogStat (char *status, STATE *state);
#endif

#endif
