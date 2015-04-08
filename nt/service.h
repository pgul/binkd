/*
 *  service.h -- Windows NT services support for binkd definition file
 *
 *  service.h is a part of binkd project
 *
 *  Copyright (C) 2000 Dima Afanasiev, da@4u.net (Fido 2:5020/463)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include "../sem.h"


extern char *service_name;
extern MUTEXSEM exitsem;

/* checkservice() return values */
#define CHKSRV_CANT_INSTALL  -1
#define CHKSRV_ERROR          0
#define CHKSRV_NOT_INSTALLED  1
#define CHKSRV_INSTALLED      2

/* Check service status
 * Return:
 * -1 : can't install service
 *  1 : Service not installed
 *  2 : Service installed
 */
int checkservice(void);


/* main service code
 */
int service(int argc, char **argv, char **envp);

/* Try connect to NT service controller
 * Return 1 if program running standalone or system error
 */
int tell_start_ntservice(void);

/* Minimise to tray
 */
void do_tray_flag(void);


/*  wrapper for win32API function SetServiceStatus() */
BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint);

/* Service-specific cleanup procedure. Not an thread-safe!
 * Must be set in first call of atexit().
 */
void atServiceExit(void);

/* Start service-specific cleanup procedure.
 * Must be set in last call of atexit().
 * (Report to SCM about service stop pending)
 */
void atServiceExitBegins(void);
