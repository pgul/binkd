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
/*
 * $Id$
 *
 * $Log$
 * Revision 2.5  2003/07/18 10:30:34  stas
 * New functions: IsNT(), Is9x(); small code cleanup
 *
 * Revision 2.4  2003/07/16 15:50:44  stas
 * Fix: restore "Minimise to tray"
 *
 * Revision 2.3  2003/07/16 15:08:49  stas
 * Fix NT services to use getopt(). Improve logging for service
 *
 * Revision 2.2  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.1  2003/02/13 19:44:45  gul
 * Change \r\n -> \n
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */

/* service_main types of call */
#define service_main_services       0 /* Check: we can operate with services */
#define service_main_testinstalled  1 /* Check: installed or not */
#define service_main_install        2 /* Install service */
#define service_main_uninstall      3 /* Uninstall service */
#define service_main_start          4 /* Start service */
#define service_main_installstart   5 /* Install and start service */
#define service_main_testrunning    6 /* Check: service is running */
#define service_main_stop           7 /* Stop service */

/* service_main return values */
#define service_main_ret_ok         0
#define service_test_notinstall     1 /* Service not installed (OpenService() failed or other) */
#define service_test_fail           2 /* Can't check service: QueryServiceStatus() fail */
#define service_test_notrunning     3 /* Service not running */
#define service_main_ret_failinstall 1 /* Service install fail */
#define service_main_ret_failstart  1 /* Service start failed */
#define service_main_ret_failstop   1 /* Service stop failed */
#define service_main_ret_faildelete 1 /* Service delete failed */
#define service_main_ret_fail       2 /* Can't check service: QueryServiceStatus() fail */
#define service_main_ret_not        2 /* Can't operate: OpenSCManagerA() dont exist */
#define service_main_ret_failcontrol 3 /* OpenSCManager failed */

extern char *service_name;


/* checkservice() return values */
#define CHKSRV_CANT_INSTALL  -1
#define CHKSRV_NOT_INSTALLED  1
#define CHKSRV_INSTALLED      2

/* Check service status
 * Return:
 * -1 : can't install service
 *  1 : Service not installed
 *  2 : Service installed
 */
int checkservice(void);

int service(int argc, char **argv, char **envp);

/* Try connect to NT service controller
 * Return 1 if program running standalone or system error
 */
int tell_start_ntservice(void);

/* Minimise to tray
 */
void do_tray_flag(void);
