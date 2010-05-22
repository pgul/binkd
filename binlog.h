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
 * Revision 2.3  2010/05/22 08:11:30  gul
 * Call after_session() hook after removing bsy
 *
 * Revision 2.2  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
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

void BinLogStat (int status, STATE *state, BINKD_CONFIG *config);

#endif
