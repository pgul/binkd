/*--------------------------------------------------------------------*/
/*       B i n L o g . c                                              */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Binary log interface                                         */
/*                                                                    */
/*       Definition file.                                             */
/*--------------------------------------------------------------------*/

#ifndef __BINLOG_H__
#define __BINLOG_H__

void BinLogInit(void);
void BinLogDeInit(void);
#ifdef STATE_DEFINED
void BinLogStat (char *status, STATE *state);
#endif

#endif
