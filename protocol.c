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
 * Revision 2.233  2014/01/14 08:20:10  gul
 * Possible segfault on some systems in rare case
 *
 * Revision 2.232  2014/01/12 13:25:30  gul
 * unix (linux) pthread version
 *
 * Revision 2.231  2013/11/07 16:21:33  stream
 * Lot of fixes to support 2G+ files. Supports 2G+ on Windows/MSVC
 *
 * Revision 2.230  2013/10/23 19:25:56  stream
 * EWOULDBLOCK, O_BINARY, O_NOINHERIT could be defined to wrong value
 *
 * Revision 2.229  2013/06/29 07:20:49  gul
 * Fix warning on windows
 *
 * Revision 2.228  2013/02/04 12:47:12  gul
 * New config option "listen"
 *
 * Revision 2.227  2013/02/03 21:37:45  gul
 * New option "rename-style [postfix|extension]"
 *
 * Revision 2.226  2013/02/03 07:28:50  gul
 * Possible segfault on after_session perl hook
 *
 * Revision 2.225  2013/01/24 17:25:35  gul
 * Support "-pipe" option on Win32
 *
 * Revision 2.224  2012/09/24 00:26:42  gul
 * Resolve logic changed
 *
 * Revision 2.223  2012/09/22 05:17:17  gul
 * Fix compilation under Windows
 *
 * Revision 2.222  2012/09/20 12:16:53  gul
 * Added "call via external pipe" (for example ssh) functionality.
 * Added "-a", "-f" options, removed obsoleted "-u" and "-i" (for win32).
 *
 * Revision 2.221  2012/06/26 09:55:46  gul
 * MD5 password is not mandatory on incoming
 *
 * Revision 2.220  2012/06/26 09:44:27  gul
 * Code style
 *
 * Revision 2.219  2012/05/13 17:26:40  gul
 * Possibility to specify $passwd for session in on_handshake() perl hook
 *
 * Revision 2.218  2012/03/10 06:56:59  gul
 * Improved error reporting on seek error
 *
 * Revision 2.217  2012/01/30 23:02:14  green
 * Corrected FTS-1027 support, allow list of hash algos
 *
 * Revision 2.216  2012/01/27 18:25:45  green
 * Improved getpeername() error handling
 *
 * Revision 2.215  2012/01/26 22:58:35  green
 * Workaround for OS/2 kLIBC 0.6.4, getpeername() always returns -1
 *
 * Revision 2.214  2012/01/26 18:50:06  green
 * Ensure the host information is not invalid
 *
 * Revision 2.213  2012/01/08 19:18:03  green
 * Improved hostname lookup and logging
 *
 * Revision 2.212  2012/01/08 17:34:57  green
 * Avoid using MAXHOSTNAMELEN
 *
 * Revision 2.211  2012/01/08 14:09:04  green
 * Corrected initialization of getaddrinfo hints
 *
 * Revision 2.210  2012/01/07 23:38:45  green
 * Improved getnameinfo handling, retry without name resolution
 *
 * Revision 2.209  2012/01/07 16:56:28  green
 * Improved getnameinfo error handling
 *
 * Revision 2.208  2012/01/07 16:34:00  green
 * Add error id where gai_strerror() is used
 *
 * Revision 2.207  2012/01/07 16:22:26  green
 * Fix some compiler warnings
 *
 * Revision 2.206  2012/01/07 11:54:04  green
 * Fix MSVC6 compilation errors
 *
 * Revision 2.205  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.204  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.203  2011/02/19 06:23:05  gul
 * Cosmetics
 *
 * Revision 2.202  2011/02/19 06:08:27  gul
 * Yet another possible segfault on session start
 *
 * Revision 2.201  2011/01/24 10:44:53  gul
 * Possible segfault on session start
 *
 * Revision 2.200  2010/12/12 09:44:11  gul
 * Use Sleep() instead of select(0,NULL,NUL,NULL,...) under WIN32
 *
 * Revision 2.199  2010/05/22 08:11:30  gul
 * Call after_session() hook after removing bsy
 *
 * Revision 2.198  2009/11/27 14:19:33  stas
 * fix typo
 *
 * Revision 2.197  2009/11/22 07:52:53  gul
 * Send M_ERR and increase undialable on error rename received file
 *
 * Revision 2.196  2009/06/15 22:41:46  stas
 * Don't process second M_PWD
 *
 * Revision 2.195  2009/05/31 07:16:17  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.194  2009/02/14 13:14:43  gul
 * Bugfix: segfault on crafted input sequences,
 * possible remote DoS for multithread versions (win32 and OS/2).
 * Thanks to Dennis Yurichev.
 *
 * Revision 2.193  2008/08/05 06:05:16  gul
 * Optimize srif functions params
 *
 * Revision 2.192  2008/02/25 10:38:10  gul
 * Fixed incorrect byte counters in log message about compressed files
 *
 * Revision 2.191  2008/01/16 10:05:43  gul
 * Fix for previous patch
 *
 * Revision 2.190  2008/01/15 23:08:16  stas
 * Log message with recommendation about NR mode to workaround remote bug
 *
 * Revision 2.189  2008/01/14 20:45:44  gul
 * Workaroud bug of earlyer binkd versions with partial files and not NR-mode
 *
 * Revision 2.188  2008/01/14 11:42:53  gul
 * Fixed bug in protocol logic (partial files send without NR-mode)
 *
 * Revision 2.187  2007/10/30 07:43:11  gul
 * Removed copy/pasted code from prev patch
 *
 * Revision 2.186  2007/10/30 07:33:25  gul
 * New config option dont-send-empty
 *
 * Revision 2.185  2007/09/11 11:18:34  gul
 * Use NR workaround for all binkd versions before 0.9.5
 *
 * Revision 2.184  2007/09/04 06:04:50  gul
 * Use workaround of NR-mode bug only for binkd/0.9.4
 *
 * Revision 2.183  2007/09/03 22:46:34  gul
 * Remove workaround for asymmentric NR-mode
 *
 * Revision 2.182  2006/07/24 21:00:32  gul
 * use MSG_NOSIGNAL in send()
 *
 * Revision 2.181  2005/11/07 17:54:00  stas
 * rtrim
 *
 * Revision 2.180  2005/10/03 07:52:23  gul
 * Fixed memory leak from 1.0a-466 (thanks to Roman Trunov)
 *
 * Revision 2.179  2005/10/02 21:47:35  gul
 * set_rlimit() perl hook
 *
 * Revision 2.178  2005/10/02 20:48:40  gul
 * - add $traf_mail and $traf_files vars for on_call() and on_handshake() hooks;
 * - optimize queue scan in perl hooks;
 * - documentation for $rc var in after_session() hook.
 *
 * Revision 2.177  2005/10/02 18:03:26  gul
 * Verbously report about mail/files for us when receive TRF from remote.
 *
 * Revision 2.176  2005/10/02 15:03:11  gul
 * Fileboxes did not works for unlisted nodes
 *
 * Revision 2.175  2005/09/28 19:02:25  gul
 * Fixed unsigned int arithmetics in rate-limits
 *
 * Revision 2.174  2005/09/27 20:15:43  gul
 * Hopefully fixed compilation under windows
 *
 * Revision 2.173  2005/09/26 19:01:03  gul
 * bw limits code partially rewrited (not tested)
 *
 * Revision 2.172  2005/09/23 13:04:15  gul
 * Fix warning
 *
 * Revision 2.171  2005/09/12 17:07:35  gul
 * Turn compression on if OPT EXTCMD GZIP received after login phase
 *
 * Revision 2.170  2005/07/04 18:24:43  gul
 * Move events checking and running to inb_test() for reducing repeated code;
 * do not run immediate events twice;
 * fixed argus-style freqs (not tested).
 *
 * Revision 2.169  2005/06/06 17:13:56  stream
 * Fuck this buggy CVS compression!
 *
 * Revision 2.167  2005/02/09 17:33:59  val
 * config docs and makefile changes for the bandwidth limiting code
 *
 * Revision 2.166  2004/11/21 12:18:05  val
 * bandwidth limiting code is now implemented for receiving too
 *
 * Revision 2.165  2004/11/05 12:24:58  gul
 * hide-aka & present-aka did not work on outgoing sessions
 *
 * Revision 2.164  2004/11/03 08:22:21  stas
 * Set 'unsigned char *' to prevent warnings
 *
 * Revision 2.163  2004/10/20 17:30:55  gul
 * Fixed segfault on transmission compressed files
 * when sizeof(off_t)!=sizeof(long) and loglevel>3.
 *
 * Revision 2.162  2004/10/19 16:28:19  gul
 * Do not remove complete received but not renamed partial files
 * for prevent data loss in ND-mode.
 * Remove all partial files for node after successfull session.
 *
 * Revision 2.161  2004/10/18 15:16:32  gul
 * Minor bugfix (patch from Victor Levenets <aq@takas.lt>)
 *
 * Revision 2.160  2004/10/01 09:55:06  gul
 * Fixed memory leak
 * (Reported by Victor Levenets <aq@takas.lt>)
 *
 * Revision 2.159  2004/09/21 08:32:11  val
 * bandwidth limiting logic changed from "max for akas" to "min for akas"
 *
 * Revision 2.158  2004/09/06 10:47:04  val
 * bandwidth limiting code advancements, `listed' session state fix
 *
 * Revision 2.157  2004/09/02 09:31:52  val
 * release CPU when limiting bandwidth on Win32
 *
 * Revision 2.156  2004/09/02 08:56:19  val
 * bandwidth limiting config parameter 'limit-rate'
 *
 * Revision 2.155  2004/08/30 08:05:14  val
 * bandwidth limiting code [experimental]
 *
 * Revision 2.154  2004/08/04 11:32:29  gul
 * Attemp to support large files (>4G)
 *
 * Revision 2.153  2004/01/23 18:09:31  gul
 * Fixed erroneous "Unknown option ... ignored" message
 *
 * Revision 2.152  2004/01/08 13:27:47  val
 * * extend struct dirent for dos and win32 in order to get file attribute
 * * ignore hidden files in boxes for dos/win32/os2
 * * if we can differ files from directories w/o stat(), don't call stat()
 *   when scanning boxes (unix: freebsd)
 * * if we can't unlink file, don't send it again in the same session
 * * for dos/win32/os2 try to clear read/only attribute if can't unlink file
 *
 * Revision 2.151  2004/01/08 13:03:51  val
 * * new functions for parsing and updating addresses in pkt header (raw, char*)
 * * use these functions in shared aka logic
 * * set password in pkt to the pkt password for the main aka of sharing node
 * * config file description updated
 *
 * Revision 2.150  2004/01/08 12:57:18  val
 * * parse up to 3 comma-separated passwords (in,pkt,out)
 * * use out password for outgoing sessions if it's set
 *
 * Revision 2.149  2004/01/08 11:36:06  gul
 * Fix typo in previous patch
 *
 * Revision 2.148  2004/01/07 13:12:29  gul
 * Update 2003->2004 in copyright notices
 *
 * Revision 2.147  2004/01/07 12:23:39  gul
 * Remove zaccept keyword, receiving compressed files possibility
 * is always on now if binkd was compiled with zlib/bzip2 support.
 *
 * Revision 2.146  2003/12/28 10:23:25  gul
 * Print file offset on "receiving interrupted" log message
 *
 * Revision 2.145  2003/12/26 21:12:06  gul
 * Change unixtime and file length/offset to unsigned in protocol messages
 *
 * Revision 2.144  2003/12/15 00:03:55  gul
 * NZ option for M_GET command - request uncompressed file
 *
 * Revision 2.143  2003/12/10 11:12:10  gul
 * Minor fix in decompression deinit
 *
 * Revision 2.142  2003/12/09 21:58:20  gul
 * Bugfix in resend file in compression mode,
 * new functions compress_abort() and decompress_abort().
 *
 * Revision 2.141  2003/11/20 16:21:17  gul
 * Bugfix in sending ND-status with compression
 *
 * Revision 2.140  2003/10/30 11:11:31  gul
 * Drop incoming session if secure remote AKA is busy
 *
 * Revision 2.139  2003/10/30 10:57:46  gul
 * Change inb_done arguments, optimize a bit
 *
 * Revision 2.138  2003/10/30 10:37:00  gul
 * Do not append file partially received from busy remote aka,
 * non-destructive skip it.
 *
 * Revision 2.137  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.136  2003/10/24 06:38:41  val
 * fix warning
 *
 * Revision 2.135  2003/10/20 18:44:51  gul
 * Inhibit incorrect error message
 *
 * Revision 2.134  2003/10/20 18:24:07  gul
 * Compression bugfix
 *
 * Revision 2.133  2003/10/19 22:44:17  gul
 * Add xstrcat(), use dynamic strings for OPT
 *
 * Revision 2.132  2003/10/19 12:21:47  gul
 * Stream compression
 *
 * Revision 2.131  2003/10/14 11:37:47  gul
 * Fix typo
 *
 * Revision 2.130  2003/10/14 07:34:28  gul
 * Use getwordx() for parse optional M_FILE params
 *
 * Revision 2.129  2003/10/12 12:58:40  gul
 * No changes ;)
 *
 * Revision 2.128  2003/10/07 17:57:09  gul
 * Some small changes in close threads function.
 * Inhibit errors "socket operation on non-socket" on break.
 *
 * Revision 2.127  2003/09/24 07:32:16  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.126  2003/09/22 11:38:47  val
 * new ip checking modes in VAL_STYLE: ipNoUnknown, ipNoError;
 * val's and gul's -ip modes can be used per node as ipRelaxed and ipResolved
 * (lacks readcfg support still)
 *
 * Revision 2.125  2003/09/22 09:54:41  gul
 * Screen output semaphoring, prevent mixing output from threads
 *
 * Revision 2.124  2003/09/19 13:54:30  gul
 * undef VAL_STYLE ip check by default
 *
 * Revision 2.123  2003/09/19 12:52:57  val
 * fix bug with not sending GZ parameter to M_FILE after M_GET
 *
 * Revision 2.122  2003/09/17 07:04:45  val
 * Cosmetics and comment on #define VAL_STYLE
 *
 * Revision 2.121  2003/09/16 06:38:44  val
 * correct IP checking algorithms (gul's one is buggy), correct get_defnode_info()
 *
 * Revision 2.120  2003/09/15 21:10:09  gul
 * Fix remote IP check logic
 *
 * Revision 2.119  2003/09/15 06:57:09  val
 * compression support via zlib: config keywords, improvements, OS/2 code
 *
 * Revision 2.118  2003/09/12 09:09:38  val
 * zlib compression support and configure for unix (my first try to write
 * autoconf script, i hope it works on your system ;-)
 *
 * Revision 2.116  2003/09/08 16:39:39  stream
 * Fixed race conditions when accessing array of nodes in threaded environment
 * ("jumpimg node structures")
 *
 * Revision 2.115  2003/09/05 10:17:21  gul
 * Send argus-compatible freqs.
 * Warning: works only with prescan!
 *
 * Revision 2.114  2003/09/05 09:57:16  gul
 * Process multiply M_NUL FREQ messages
 *
 * Revision 2.113  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.112  2003/09/05 06:44:05  val
 * Argus-style freq's (M_NUL FREQ) support, not tested yet
 *
 * Revision 2.111  2003/08/29 13:27:34  gul
 * Do not save zero-length .dt files
 *
 * Revision 2.110  2003/08/26 22:18:48  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.109  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.108  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.107  2003/08/25 19:09:29  gul
 * Flush file buffer after receive data frame,
 * drop session if extra bytes received.
 *
 * Revision 2.106  2003/08/24 19:42:08  gul
 * Get FTN-domain from matched zone in exp_ftnaddress()
 *
 * Revision 2.105  2003/08/24 18:54:30  gul
 * Bugfix in timeout check on win32
 *
 * Revision 2.104  2003/08/24 01:35:59  hbrew
 * Update for previous patch
 *
 * Revision 2.103  2003/08/24 00:45:44  hbrew
 * win9x-select-workaround fix, thanks to Pavel Gulchouck
 *
 * Revision 2.102  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.101  2003/08/19 10:16:12  gul
 * Rename trunc() -> trunc_file() due to conflict under OS/2 EMX
 *
 * Revision 2.100  2003/08/18 07:35:08  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.99  2003/08/18 07:29:09  val
 * multiple changes:
 * - perl error handling made via fork/thread
 * - on_log() perl hook
 * - perl: msg_send(), on_send(), on_recv()
 * - unless using threads define log buffer via xalloc()
 *
 * Revision 2.98  2003/08/17 08:12:05  gul
 * Fix typo
 *
 * Revision 2.97  2003/08/16 09:47:25  gul
 * Autodetect tzoff if not specified
 *
 * Revision 2.96  2003/08/15 08:48:50  gul
 * Compilation error fixed
 *
 * Revision 2.95  2003/08/14 14:19:37  gul
 * Drop remote AKA with another password on outgoing sessions
 *
 * Revision 2.94  2003/08/14 08:29:22  gul
 * Use snprintf() from sprintf.c if no such libc function
 *
 * Revision 2.93  2003/08/13 11:59:21  gul
 * Undo my prev patch, sorry ;)
 *
 * Revision 2.92  2003/08/13 11:49:05  gul
 * correct previous fix
 *
 * Revision 2.91  2003/08/13 11:35:26  hbrew
 * Fix warning.
 *
 * Revision 2.90  2003/08/13 08:02:51  val
 * define DELAY_ADR ifdef WITH_PERL (todo: provide more flexible logic)
 *
 * Revision 2.89  2003/08/11 08:36:41  gul
 * workaround winsock bug
 *
 * Revision 2.88  2003/07/06 10:34:27  gul
 * Migrate workaround of 100% CPU load with winsock from stable branch
 *
 * Revision 2.87  2003/07/06 10:18:55  gul
 * Increase loglevel for "Watinig for M_GOT" message
 *
 * Revision 2.86  2003/07/06 08:32:31  gul
 * Decrease logging about link status changes
 *
 * Revision 2.85  2003/07/06 06:48:25  gul
 * Using state->out.fa bugfix
 *
 * Revision 2.84  2003/07/03 05:43:41  gul
 * Another fix for previous patch
 *
 * Revision 2.83  2003/07/02 18:16:43  gul
 * Bugfix for patch about send status without NR-mode
 *
 * Revision 2.82  2003/06/26 13:22:24  gul
 * *** empty log message ***
 *
 * Revision 2.81  2003/06/26 13:21:32  gul
 * More clean status process in no-NR mode
 *
 * Revision 2.80  2003/06/26 12:53:31  gul
 * Send status in no-NR mode to avoid file loosing
 *
 * Revision 2.79  2003/06/25 07:25:00  stas
 * Simple code, continue bugfix to responce negative timestamp
 *
 * Revision 2.78  2003/06/24 13:46:32  stas
 * Fix max value of type time_t
 *
 * Revision 2.77  2003/06/24 08:08:46  stas
 * Bugfix: do not transmit negative value of file time in binkp command-frames and check file time in received frames
 *
 * Revision 2.76  2003/06/24 06:33:42  gul
 * Fix for previous patch
 *
 * Revision 2.75  2003/06/24 06:28:21  gul
 * Check IP for all remote AKAs on outgoing calls
 *
 * Revision 2.74  2003/06/21 19:35:45  gul
 * Fixed remote ip check
 *
 * Revision 2.73  2003/06/21 15:31:48  hbrew
 * Fix warning
 *
 * Revision 2.72  2003/06/21 08:41:29  gul
 * "try" and "hold" did not work if connection closed by remote
 *
 * Revision 2.71  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.70  2003/06/12 08:21:43  val
 * 'skipmask' is replaced with 'skip', which allows more skipping features
 *
 * Revision 2.69  2003/06/08 13:40:07  gul
 * Avoid warning
 *
 * Revision 2.68  2003/06/07 08:46:25  gul
 * New feature added: shared aka
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "protocol.h"
#include "ftnaddr.h"
#include "ftnnode.h"
#include "ftndom.h"
#include "ftnq.h"
#include "iptools.h"
#include "tools.h"
#include "bsy.h"
#include "inbound.h"
#include "srif.h"
#include "readflo.h"
#include "protoco2.h"
#include "prothlp.h"
#include "assert.h"
#include "binlog.h"
#include "setpttl.h"
#include "sem.h"
#include "md5b.h"
#include "crypt.h"
#include "compress.h"

#ifdef WITH_PERL
#include "perlhooks.h"
#endif
#include "rfc2553.h"

/* define to enable val's code for -ip checks (default is gul's code) */
#undef VAL_STYLE
#ifdef VAL_STYLE
enum { ipNoCheck=0, ipResolved=1, ipStrict=2, ipRelaxed, ipNoUnknown, ipNoError };
#endif

static char *scommand[] = {"NUL", "ADR", "PWD", "FILE", "OK", "EOB",
                           "GOT", "ERR", "BSY", "GET", "SKIP"};

/*
 * Fills <<state>> with initial values, allocates buffers, etc.
 */
static int init_protocol (STATE *state, SOCKET socket_in, SOCKET socket_out, FTN_NODE *to, FTN_ADDR *fa, BINKD_CONFIG *config)
{
  char val[4];
  socklen_t lval;

  memset (state, 0, sizeof (STATE));

  state->major = 1;
  state->minor = 0;
  state->msgs_in_batch = 0;
  state->off_req_sent = 0;
  state->waiting_for_GOT = 0;
  state->send_eof = 0;
  state->inbound = config->inbound_nonsecure;
  state->io_error = 0;
  state->ibuf = xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->isize = -1;
  state->obuf = xalloc (MAX_BLKSIZE + BLK_HDR_SIZE + 1);
  state->optr = 0;
  state->oleft = 0;
  state->bytes_sent = state->bytes_rcvd = 0;
  state->files_sent = state->files_rcvd = 0;
  state->to = to;
  state->remote_fa = fa;
  state->NR_flag = (to && (to->NR_flag == NR_ON || to->ND_flag == ND_ON)) ? WANT_NR : NO_NR;
  state->ND_flag = (to && to->ND_flag == ND_ON) ? THEY_ND : NO_ND;
  state->MD_flag = 0;
  state->MD_challenge = NULL;
  state->crypt_flag = no_crypt ? NO_CRYPT : WE_CRYPT;
  strcpy(state->expected_pwd, "-");
  state->skip_all_flag = state->r_skipped_flag = 0;
  state->maxflvr = 'h';
  state->listed_flag = 0;
  state->fa = NULL;
  state->nfa = state->nallfa = 0;
  state->nAddr = 0; state->pAddr = NULL;
#ifdef WITH_PERL
  state->perl_set_lvl = 0;
#endif
  state->delay_ADR = (config->akamask.first != NULL) ? 1 : 0;
#ifdef WITH_PERL
  state->delay_ADR |= (config->perl_ok & (1<<4)) != 0;
#endif
  state->delay_EOB = 0;
  state->state_ext = P_NA;
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  state->z_canrecv = state->z_cansend = state->z_oleft = 0;
  state->z_obuf = xalloc (ZBLKSIZE);
#endif
#ifdef WITH_ZLIB
# ifdef ZLIBDL
  if (zlib_loaded)
# endif
  state->z_canrecv |= 1;
#endif
#ifdef WITH_BZLIB2
# ifdef ZLIBDL
  if (bzlib2_loaded)
# endif
  state->z_canrecv |= 2;
#endif
  setsockopts (state->s_in  = socket_in);
  setsockopts (state->s_out = socket_out);
  TF_ZERO (&state->in);
  TF_ZERO (&state->out);
  TF_ZERO (&state->flo);
  TF_ZERO (&state->in_complete);
  state->ND_addr.z = -1;
  state->start_time = safe_time();
  state->evt_queue = NULL;
  state->config = config;
  lval = sizeof(val);
  if (getsockopt (socket_in, SOL_SOCKET, SO_TYPE, val, &lval) == -1)
  { /* assume it's not a socket */
    state->pipe = 1;
    Log (6, "binkp init done, pipe handles are %i/%i", state->s_in, state->s_out);
  }
  else
  {
    Log (6, "binkp init done, socket # is %i", state->s_in);
  }
  return 1;
}

/*
 * Close file currently receiving,
 * remove .hr and .dt if it's partial pkt or zero-length
 */
static int close_partial (STATE *state, BINKD_CONFIG *config)
{
  boff_t s;

  if (state->in.f)
  {
    s = ftello (state->in.f);
    Log (1, "receiving of %s interrupted at %" PRIuMAX, state->in.netname,
         (uintmax_t) s);
    if (ispkt (state->in.netname))
    {
      Log (2, "%s: partial .pkt", state->in.netname);
      s = 0;
    }
    else if (s == 0)
    {
      Log (4, "%s: empty partial", state->in.netname);
    }
    fclose (state->in.f);
    state->in.f = NULL;
    if (s == 0)
      inb_reject (state, config);
  }
  TF_ZERO (&state->in);
  return 0;
}

/*
 * Clears protocol buffers and queues, closes files, etc.
 */
static int deinit_protocol (STATE *state, BINKD_CONFIG *config, int status)
{
  int i;

  close_partial(state, config);
  if (state->out.f)
    fclose (state->out.f);
  if (state->flo.f)
    fclose (state->flo.f);
  if (state->killlist)
    free_killlist (&state->killlist, &state->n_killlist);
  if (state->rcvdlist)
    free_rcvdlist (&state->rcvdlist, &state->n_rcvdlist);
  for (i = 0; i < state->n_nosendlist; i++)
    xfree(state->nosendlist[i]);
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  xfree (state->z_obuf);
  if (state->z_recv && state->z_idata)
    decompress_deinit(state->z_recv, state->z_idata);
  if (state->z_send && state->z_odata)
    compress_abort(state->z_send, state->z_odata);
#endif
  xfree (state->ibuf);
  xfree (state->obuf);
  xfree (state->msgs);
  xfree (state->sent_fls);
  for (i = 0; i < state->nfa; ++i)
    bsy_remove (state->fa + i, F_BSY, config);

#ifdef WITH_PERL
  perl_after_session(state, status);
#endif

  if (state->q)
    q_free (state->q, config);
  xfree (state->fa);
  xfree (state->pAddr);
  xfree (state->MD_challenge);
  rel_grow_handles(-state->nfa);
  Log (6, "binkp deinit done...");
  return 0;
}

/* Process rcvdlist */
static FTNQ *process_rcvdlist (STATE *state, FTNQ *q, BINKD_CONFIG *config)
{
  int i;

  Log (6, "processing rcvd list");
  for (i = 0; i < state->n_rcvdlist; ++i)
  {
    q = evt_run(q, state->rcvdlist[i].name, 1, state, config);
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
void msg_send2 (STATE *state, t_msg m, char *s1, char *s2)
{
  if (!s1)
    s1 = "";
  if (!s2)
    s2 = "";
#ifdef WITH_PERL
  if (!perl_on_send(state, &m, &s1, &s2)) return;
#endif
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
  vsnprintf (msg_text, sizeof (msg_text), s, ap);
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
    Log(5, "Waiting for M_GOT");
  }
}

/*
 * Sends next msg from the msg queue or next data block
 */
static int send_block (STATE *state, BINKD_CONFIG *config)
{
  int i, n, save_errno;
  const char *save_err;

  /* Have something to send in buffers */
  if (state->optr && state->oleft)
  {
    Log (7, "sending %i byte(s)", state->oleft);
    if (state->pipe)
      /* TODO: this call should be non-blocking on WIN32 */
      n = write (state->s_out, state->optr, state->oleft);
    else
      n = send (state->s_out, state->optr, state->oleft, MSG_NOSIGNAL);
#ifdef BW_LIM
    state->bw_send.bytes += n;
#endif
    if (state->pipe)
    {
      save_errno = errno;
      save_err = strerror(errno);
      Log (7, "write() done, rc=%i", n);
    }
    else
    {
      save_errno = TCPERRNO;
      save_err = TCPERR ();
      Log (7, "send() done, rc=%i", n);
    }
    if (n == state->oleft)
    {
      state->optr = 0;
      state->oleft = 0;
      Log (7, "data sent");
    }
    else if (n == -1)
    {
      if ((state->pipe == 0 && save_errno != TCPERR_WOULDBLOCK && save_errno != TCPERR_AGAIN) ||
          (state->pipe != 0 && save_errno != EWOULDBLOCK && errno != EAGAIN))
      {
        state->io_error = 1;
        if (!binkd_exit)
        {
          Log (1, "%s: %s", state->pipe ? "write" : "send", save_err);
          if (state->to)
            bad_try (&state->to->fa, save_err, BAD_IO, config);
        }
        return 0;
      }
      Log (7, "data transfer would block");
      return 2;
    }
    else if (n == 0)
    {
      /* pipe is not ready? */
      return 2;
    }
    else
    {
      state->optr += n;
      state->oleft -= n;
      Log (7, "partially sent, %i byte(s) left", state->oleft);
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

          Log (7, "put next msg to obuf, %i", state->msgs[i].sz);
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
      int sz;
      unsigned char *buf = (unsigned char *)state->obuf + BLK_HDR_SIZE;

      if (state->out.f)
      {
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
        if (state->z_send)
        { sz = ZBLKSIZE - state->z_oleft;
          buf = (unsigned char *)state->z_obuf + state->z_oleft;
        } else
          sz = config->oblksize;
        sz = min ((boff_t) sz, state->out.size - ftello (state->out.f));
#else
        /* OK to truncate to 32bits because config->oblksize is plain int */
        sz = (int) min ((boff_t) config->oblksize, state->out.size - ftello (state->out.f));
#endif
      }
      else
      {
        state->send_eof = 0;
        sz = 0;
      }
      Log (10, "next block to send: %u byte(s)", sz);
      mkhdr (state->obuf, sz);
      if (sz != 0)
      {
        Log (10, "freading %u byte(s)", sz);
        if ((n = fread (buf, 1, sz, state->out.f)) < (int) sz)
        {
          Log (1, "error reading %s: expected %u, read %i",
               state->out.path, sz, n);
          return 0;
        }

        /* Dirty hack :-) - if
         *  1. this is the first block of the file, and
         *  2. this is pkt-header, and
         *  3. pkt destination is shared address
         *  change destination address to main aka.
         */
        if ((ftello(state->out.f)==(boff_t)sz) && (sz >= 60) /* size of pkt header + 2 bytes */
            && ispkt(state->out.netname))
        {
          short cz, cnet, cnode, cp;
          SHARED_CHAIN *chn;
          if (pkt_getaddr(buf, NULL, NULL, NULL, NULL, &cz, &cnet, &cnode, &cp)) {
            Log(9, "First block of %s", state->out.path);
            Log(7, "PKT dest: %d:%d/%d.%d", cz, cnet, cnode, cp);
            /* Scan all shared addresses */
            for (chn = config->shares.first; chn; chn = chn->next)
            {
              if ((chn->sha.z    == cz) &&
                  (chn->sha.net  == cnet)  &&
                  (chn->sha.node == cnode) &&
                  (chn->sha.p    == cp))
              { /* Found */
                FTN_ADDR *fa = NULL;
                if (state->to) fa = &state->to->fa;
                  else if (state->fa) fa = state->fa;
                if (fa)
                { /* Change to main address and check */
                  pkt_setaddr(buf, -1, -1, -1, -1, (short)fa->z, (short)fa->net, (short)fa->node, (short)fa->p);
                  pkt_getaddr(buf, NULL, NULL, NULL, NULL, &cz, &cnet, &cnode, &cp);
                  Log(7, "Change dest to: %d:%d/%d.%d", cz, cnet, cnode, cp);
                  /* Set corresponding pkt password */
                  {
                    FTN_NODE *fn = state->to ? state->to : get_node_info(fa, config);
                    memset(buf+26, 0, 8);
                    if (fn->pkt_pwd) memmove(buf+26, fn->pkt_pwd, 8);
                  }
                }
                break;
              }
            }
          }
        }
      }
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
      if (state->z_send && state->out.f)
      {
        int nput = 0;  /* number of compressed bytes */
        int nget = 0;  /* number of read uncompressed bytes from buffer */
        int ocnt;      /* number of bytes compressed by one call */
        int rc;
        boff_t fleft;

        sz += state->z_oleft;
        while (1)
        {
          ocnt = config->oblksize - nput;
          nget = sz;
          fleft = state->out.size - ftello(state->out.f);
          rc = do_compress(state->z_send,
                           state->obuf + BLK_HDR_SIZE + nput, &ocnt,
                           state->z_obuf, &nget,
                           fleft ? 0 : 1,
                           state->z_odata);
          if (rc == -1) {
            Log (1, "error compression %s, rc=%d", state->out.path, rc);
            return 0;
          }
          state->z_osize += nget;
          state->z_cosize += ocnt;
          nput += ocnt;
          if (!fleft && rc == 1) break;
          if (nput == config->oblksize) break;
          sz = min(fleft, ZBLKSIZE);
          if (sz == 0) continue;
          Log (10, "freading %u byte(s)", sz);
          if ((n = fread (state->z_obuf, 1, sz, state->out.f)) < (int) sz)
          {
            Log (1, "error reading %s: expected %u, read %i",
                 state->out.path, sz, n);
            return 0;
          }
        }
        /* left rest of incoming (uncompressed) buffer */
        if (nget < sz) {
          memmove(state->z_obuf, state->z_obuf + nget, sz - nget);
          state->z_oleft = sz - nget;
        } else
          state->z_oleft = 0;
        sz = nput;
        mkhdr(state->obuf, sz);
        if (!fleft && rc == 1)
        {
          Log(4, "Compressed %" PRIuMAX " bytes to %" PRIuMAX " for %s, ratio %.1f%%",
              (uintmax_t)state->z_osize, (uintmax_t)state->z_cosize,
              state->out.netname, 100.0 * state->z_cosize / (state->z_osize ? state->z_osize : 1));
          compress_deinit(state->z_send, state->z_odata);
          state->z_odata = NULL;
          state->z_send = 0;
        }
      }
#endif

      if (config->percents && state->out.f && state->out.size > 0)
      {
        LockSem(&lsem);
        printf ("%-20.20s %3.0f%%\r", state->out.netname,
                100.0 * ftello (state->out.f) / (float) state->out.size);
        fflush (stdout);
        ReleaseSem(&lsem);
      }

      if (state->out.f && (sz == 0 || state->out.size == ftello(state->out.f))
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
          && !state->z_send
#endif
         )
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
static char *parse_msg_args (int ac, char **av, char *s, char *ID, STATE *state)
{
  char *rc = parse_args (ac, av, s, ID);

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
    if (delete(path) != 0) {
#if defined(WIN32) || defined(OS2) || defined(DOS)
      /* clear r/o attribute and try again */
      if (chmod(path, S_IREAD | S_IWRITE) == 0 && delete(path) != 0) {
#endif
      /* add to list not to send it again */
      Log (5, "adding file `%s' to not-to-send list", path);
      state->nosendlist = xrealloc(state->nosendlist, (state->n_nosendlist+1)*sizeof(state->nosendlist[0]));
      state->nosendlist[state->n_nosendlist++] = xstrdup(path);
#if defined(WIN32) || defined(OS2) || defined(DOS)
      }
#endif
    }
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
                              char *file, char action, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1], *w = 0;
  FILE *flo = 0;
  boff_t offset = 0, curr_offset;
  int i;
  int seek_flag = 0;                       /* Seek _state->flo.f_ to */

  /* _offset_ after processing */
  int empty_flo_flag = 1;

  if (file)
    Log (5, "removing from spool: %s", file);
  else if (flopath)
    Log (5, "removing flo: %s", flopath);
  else
    Log (1, "internal error in remove_from_spool!");

  if (flopath && *flopath)               /* A file attached via .?lo */
  {
    if (state->flo.f && !strcmp (state->flo.path, flopath))
    {
      flo = state->flo.f;
      offset = ftello (flo);
      fseeko (flo, 0, SEEK_SET);
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
      curr_offset = ftello (flo);
      if (!fgets (buf, MAXPATHLEN, flo))
        break;
      for (i = strlen (buf) - 1; i >= 0 && isspace (buf[i]); --i)
        buf[i] = 0;
      if (buf[0] == '\0') continue;

      if (file && (!strcmp (file, buf) ||
                 ((*buf == '^' || *buf == '#') && !strcmp (file, buf + 1))))
      {
        clearerr (flo);
        if (fseeko (flo, curr_offset, SEEK_SET) == EOF)
          Log (1, "remove_from_spool: fseek(%s): %s", flopath,
               strerror (errno));
        else if (putc ('~', flo) == EOF)
          Log (1, "remove_from_spool: fputc(%s): %s", flopath,
               strerror (errno));
        fflush (flo);
        /* The line was marked, now skip it */
        if (!fgets (buf, MAXPATHLEN, flo))
          Log (1, "remove_from_spool: fgets(%s): %s", flopath,
               strerror (errno));
        /* We've found the file in flo, so try to translate it's name before
         * the action */
        if (w == 0 && (w = trans_flo_line (file, config->rf_rules.first)) != 0)
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
      fseeko (flo, offset, SEEK_SET);
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

static void do_prescan(STATE *state, BINKD_CONFIG *config)
{
  char s[64];
  uintmax_t netsize, filessize;
  int i;

  if (OK_SEND_FILES (state, config) && config->prescan)
  {
    state->q = q_scan_addrs (0, state->fa, state->nfa, state->to ? 1 : 0, config);
    q_get_sizes (state->q, &netsize, &filessize);
    sprintf(s, "%" PRIuMAX " %" PRIuMAX, netsize, filessize);
    msg_send2 (state, M_NUL, "TRF ", s);
    if (state->major * 100 + state->minor <= 100)
      for (i = q_freq_num (state->q); i>0; i--)
        msg_send2 (state, M_NUL, "FREQ", "");
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
static int NUL (STATE *state, char *buf, int sz, BINKD_CONFIG *config)
{
  char *s, *a, *b;

  UNUSED_ARG(sz);
  UNUSED_ARG(config);

  Log (3, "%s", s = strquote (buf, SQ_CNTRL));
  if (!memcmp (s, "VER ", 4) &&
      (a = strstr (s, PRTCLNAME "/")) != 0 &&
      (b = strstr (a, ".")) != 0)
  {
    state->major = atoi (a + 6);
    state->minor = atoi (b + 1);
    Log (6, "remote uses " PRTCLNAME " v.%i.%i", state->major, state->minor);
    if (!memcmp(s + 4, "binkd/0.9/", 10) ||
        !memcmp(s + 4, "binkd/0.9.1/", 12) ||
        !memcmp(s + 4, "binkd/0.9.2/", 12) ||
        !memcmp(s + 4, "binkd/0.9.3/", 12) ||
        (!memcmp(s + 4, "binkd/0.9.3", 11) && isalpha(s[15]) && s[16]=='/') ||
        !memcmp(s + 4, "binkd/0.9.4/", 12))
    {
      state->buggy_NR = 1;
      Log (5, "remote has NR bug, use workaround");
    }
  }
  else if (!memcmp (s, "TRF ", 4))
  {
    char *mail, *files;
    if ((mail = getwordx (s + 4, 1, 0)) != NULL &&
        (files = getwordx (s + 4, 2, 0)) != NULL)
    {
      Log (2, "Remote has %sb of mail and %sb of files for us", mail, files);
      free(files);
    }
    if (mail) free(mail);
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
      if (!strncmp(w, "CRAM-", 5) && !no_MD5 &&
          state->to && (state->to->MD_flag >= 0))
      {
        Log(2, "Remote requests MD mode");
        xfree(state->MD_challenge);
        state->MD_challenge=MD_getChallenge(w, NULL);
      }
#ifdef WITH_ZLIB
      if (!strcmp (w, "GZ"))
      {
        Log(2, "Remote supports GZ mode");
#ifdef ZLIBDL
        if (zlib_loaded)
#endif
        state->z_cansend |= 1;
      }
#endif
#ifdef WITH_BZLIB2
      if (!strcmp (w, "BZ2"))
      {
        Log(2, "Remote supports BZ2 mode");
#ifdef ZLIBDL
        if (bzlib2_loaded)
#endif
        state->z_cansend |= 2;
      }
#endif
      if (!strcmp (w, "EXTCMD"))
      {
        state->extcmd = 1;  /* They can accept extra params for commands */
        Log(2, "Remote supports EXTCMD mode");
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
  else if (!memcmp (s, "FREQ", 4)) {
    state->delay_EOB++;
    Log(2, "Remote claims to have a FREQ for us");
  }
  free (s);
  return 1;
}

/*
 * Handles M_ERR msg from the remote
 */
static int RError (STATE *state, char *buf, int sz, BINKD_CONFIG *config)
{
  char *s;

  UNUSED_ARG(sz);

  Log (1, "rerror: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s, BAD_MERR, config);
  free (s);
  return 0;
}

static int BSY (STATE *state, char *buf, int sz, BINKD_CONFIG *config)
{
  char *s;

  UNUSED_ARG(sz);

  Log (1, "got M_BSY: %s", s = strquote (buf, SQ_CNTRL));
  if (state->to)
    bad_try (&state->to->fa, s, BAD_MBSY, config);
  free (s);
  return 0;
}

static char * add_shared_akas(char * s, BINKD_CONFIG *config)
{
  int i;
  char * w, *c;
  SHARED_CHAIN   * chn;
  FTN_ADDR_CHAIN * fta;
  FTN_ADDR fa;
  int count = 0;
  char * ad = 0;
  char szFTNAddr[FTN_ADDR_SZ + 1];

  for (i = 1; (w = getwordx (s, i, 0)) != 0; ++i)
  {
    if (parse_ftnaddress (w, &fa, config->pDomains.first) && is5D (&fa))
    {
      for(chn = config->shares.first;chn;chn = chn->next)
      {
        if (ftnaddress_cmp(&fa,&chn->sha) == 0)
        {
          /* I think, that if shared address was exposed
           * by remote node, it should be deleted...
           */
          ftnaddress_to_str (szFTNAddr, &fa);
          Log (1, "shared aka `%s' used by node %s", szFTNAddr,s);
          /* fill this aka by spaces */
          c = strstr(s, w);
          if (c)
          {
            memset(c, ' ', strlen(w));
            i--;
          }
          break;
        }
        else
        {
          for (fta = chn->sfa.first; fta; fta = fta->next)
          {
            if (ftnaddress_cmp(&fta->fa, &fa) == 0)
            {
              if (ad == 0)
              {
                ad = xalloc(FTN_ADDR_SZ+1);
                ad[0] = 0;
                count = 1;
              } else
                ad = xrealloc(ad, (++count)*(FTN_ADDR_SZ+1));
              ftnaddress_to_str (szFTNAddr,&chn->sha);
              strcat(ad, " ");
              strcat(ad, szFTNAddr);
              Log(2, "shared aka %s is added", szFTNAddr);
              break;
            }
          }
        }
      }
    }
    free (w);
  }
  return ad;
}

static void send_ADR (STATE *state, BINKD_CONFIG *config) {
  char szFTNAddr[FTN_ADDR_SZ + 1];
  char *szAkas;
  int i, N;
  struct akachain *ps;

  Log(7, "send_ADR(): got %d remote addresses", state->nfa);

  if (!state->pAddr) {
    state->nAddr = config->nAddr;
    state->pAddr = xalloc(state->nAddr * sizeof(FTN_ADDR));
    memcpy(state->pAddr, config->pAddr, state->nAddr*sizeof(FTN_ADDR));
  }

  N = state->nAddr;
  for (ps = config->akamask.first; ps; ps = ps->next)
  {
    int t = (ps->type & 0x7f);
    int rc;

    if (state->to)
      rc = ftnamask_cmpm(ps->mask, 1, &(state->to->fa)) == (ps->type & 0x80 ? -1 : 0);
    else
      rc = ftnamask_cmpm(ps->mask, state->nfa, state->fa) == (ps->type & 0x80 ? -1 : 0);
    /* hide aka */
    if (t == ACT_HIDE && rc) {
      i = 0;
      while (i < state->nAddr)
        if (ftnaddress_cmp(state->pAddr+i, &(ps->fa)) == 0) {
          char buf[FTN_ADDR_SZ];
          ftnaddress_to_str(buf, &(ps->fa));
          Log(3, "hiding aka %s", buf);
          if (i < state->nAddr-1)
            memmove(state->pAddr+i, state->pAddr+i+1, (state->nAddr-1-i)*sizeof(FTN_ADDR));
          state->nAddr--;
        }
        else i++;
    }
    /* present aka */
    else if (t == ACT_PRESENT && rc) {
      for (i = 0; i < state->nAddr+1; i++)
        if (i == state->nAddr || ftnaddress_cmp(state->pAddr+i, &(ps->fa)) == 0) break;
      if (i == state->nAddr) {
        char buf[FTN_ADDR_SZ];
        ftnaddress_to_str(buf, &(ps->fa));
        Log(3, "presenting aka %s", buf);
        state->nAddr++;
        if (state->nAddr > N) {
          state->pAddr = xrealloc(state->pAddr, state->nAddr * sizeof(FTN_ADDR));
          N++;
        }
        memcpy(state->pAddr+state->nAddr-1, &(ps->fa), sizeof(FTN_ADDR));
      }
    }
  }

  szAkas = xalloc (state->nAddr * (FTN_ADDR_SZ + 1));
  *szAkas = 0;
  for (i = 0; i < state->nAddr; ++i)
  {
    ftnaddress_to_str (szFTNAddr, state->pAddr + i);
    strcat (szAkas, " ");
    strcat (szAkas, szFTNAddr);
  }
  msg_send2 (state, M_ADR, szAkas, 0);
  free (szAkas);
}

static int ADR (STATE *state, char *s, int sz, BINKD_CONFIG *config)
{
  int i, j, main_AKA_ok = 0;
#ifndef VAL_STYLE
  int ip_verified = 0;
#else
  enum { CHECK_OFF, CHECK_NA, CHECK_OK, CHECK_WRONG } ip_check;
  enum { FOUND_NONE=0, FOUND_ALL=1, FOUND_UNKNOWN=2, FOUND_ERROR=4 } ip_found;
# define anyAddress(x) ((x) & FOUND_ALL)
# define anyUnknown(x) ((x) & FOUND_UNKNOWN)
# define anyError(x)   ((x) & FOUND_ERROR)
#endif
  char *w;
  FTN_ADDR fa;
  FTN_NODE *pn;
  char szFTNAddr[FTN_ADDR_SZ + 1];
  int secure_NR, unsecure_NR;
  int secure_ND, unsecure_ND;
#ifdef BW_LIM
  int bw_send_unlim = 0, bw_recv_unlim = 0;
#endif

  s[sz] = 0;
  secure_NR = unsecure_NR = NO_NR;
  secure_ND = unsecure_ND = NO_ND;

  /* My hack is in using string with shared addresses
   * instead s
   */
  {
    char *ss, *ad = NULL;
    if (config->shares.first)
    {
      ad = add_shared_akas(s, config);
      if (ad)
      {
        ss = xalloc(strlen(s) + strlen(ad) + 1);
        strcpy(ss, s);
        strcat(ss, ad);
        s = ss;
        s[sz = sz + strlen(ad)] = 0;
      }
    }
    w = getwordx (s, 1, 0);
    if (w == 0)
    {
      if (ad)
      {
        free (s); free (ad);
      }
      Log (1, "all akas was removed as shared");
      return 0;
    }
    free (w);
  }

  /* set expected password on outgoing session
   * for drop remote AKAs with another passwords */
  if (state->to)
  {
    strncpy(state->expected_pwd, (state->to->out_pwd ? state->to->out_pwd : "-"), sizeof(state->expected_pwd));
    state->expected_pwd[sizeof(state->expected_pwd) - 1] = '\0';
  }

  for (i = 1; (w = getwordx (s, i, 0)) != 0; ++i)
  {
    if (!parse_ftnaddress (w, &fa, config->pDomains.first) || !is4D (&fa))
    {
      char *q = strquote (s, SQ_CNTRL);

      msg_send2 (state, M_ERR, "Bad address", 0);
      Log (1, "remote passed bad address: `%s'", q);
      free (w);
      free (q);
      return 0;
    }

    free (w);

#ifdef VAL_STYLE
    ip_check = CHECK_OFF;
#endif

    if (!fa.domain[0])
      strcpy (fa.domain, get_matched_domain(fa.z, config->pAddr, config->nAddr, config->pDomains.first));

    ftnaddress_to_str (szFTNAddr, &fa);
    pn = get_node_info(&fa, config);

    if (pn && pn->restrictIP
        && (state->to == 0 || ((!pn->pipe || !pn->pipe[0])
#ifdef HTTPS
                               && !config->proxy[0] && !config->socks[0]
#endif
                              )))
    { int i, rc;
#ifndef VAL_STYLE
      int ipok = 0;
#endif
      char host[BINKD_FQDNLEN + 1];       /* current host/port */
      char *port;
      struct sockaddr_storage sin;
      struct addrinfo *ai, *aiHead, hints;
      int aiErr;

      memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_flags = AI_NUMERICHOST;
      if ((rc = getaddrinfo(state->ipaddr, NULL, &hints, &aiHead)) == 0)
      {
        if (aiHead)
        {
          memcpy(&sin, aiHead->ai_addr, aiHead->ai_addrlen);
          freeaddrinfo(aiHead);
        }
        else
        {
#ifndef VAL_STYLE
          ipok = 2;
#endif
          Log (3, "%s, getaddrinfo error (empty result)", state->ipaddr);
        }
      }
      else
      {
        Log (3, "%s, getaddrinfo error: %s (%d)", state->ipaddr, gai_strerror(rc), rc);
#ifndef VAL_STYLE
          ipok = 2;
#endif
      }

      /* setup hints for getaddrinfo */
      memset((void *)&hints, 0, sizeof(hints));
      hints.ai_family = PF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = AI_NUMERICSERV;

#ifdef VAL_STYLE
      ip_found = FOUND_NONE; ip_check = CHECK_NA;
#endif

      for (i = 1; pn->hosts &&
           (rc = get_host_and_port(i, host, &port, pn->hosts, &pn->fa, config)) != -1; ++i)
      {
        if (rc == 0)
        {
          Log (1, "%s: %i: error parsing host list", pn->hosts, i);
          continue;
        }
        if (strcmp(host, "-") == 0)
          continue;

        Log (5, "resolving `%s'...", host);
        aiErr = getaddrinfo(host, NULL, &hints, &aiHead);
        if (aiErr != 0)
        {
          Log(3, "%s, getaddrinfo error: %s (%d)", host, gai_strerror(aiErr), aiErr);
#ifdef VAL_STYLE
          if (aiErr == EAI_NONAME)
            ip_found |= FOUND_UNKNOWN;
          else
            ip_found |= FOUND_ERROR;
#endif
          continue;
        }

        for (ai = aiHead; ai != NULL; ai = ai->ai_next)
#ifndef VAL_STYLE
          if (sockaddr_cmp_addr(ai->ai_addr, (struct sockaddr *)&sin) == 0)
          {
            ipok = 1;
            break;
          } else if (ipok == 0)
            ipok = -1; /* resolved and no match */
#else
        {
          ip_found |= FOUND_ALL;
          if (sockaddr_cmp_addr(ai->ai_addr, (struct sockaddr *)&sin) == 0)
          {
            ip_check = CHECK_OK; 
            break;
          }
        }
#endif
        freeaddrinfo(aiHead);

#ifndef VAL_STYLE
        if (ipok == 1)
#else
        if (ip_check == CHECK_OK)
#endif
          break;
      }
#ifndef VAL_STYLE
      if (ipok == 1)
      { /* matched */
        ip_verified = 2;
      } else if (ipok<0 || pn->restrictIP == 2)
      { /* not matched or unresolvable with strict check */
        if (pn->pwd && strcmp(pn->pwd, "-") && state->to == 0)
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
            Log(2, "Addr %s dropped - unresolvable IP", szFTNAddr);
          else
            Log(2, "Addr %s dropped - not from allowed IP", szFTNAddr);
          continue;
        }
      }
#else
      if (ip_check != CHECK_OFF && ip_check != CHECK_OK) {
        if (
          /* val's -ip */
          (pn->restrictIP == ipRelaxed && ip_found == FOUND_ALL) ||
          /* gul's -ip */
          (pn->restrictIP == ipResolved && anyAddress(ip_found)) ||
          /* -ip=nounknown */
          (pn->restrictIP == ipNoUnknown && anyUnknown(ip_found)) ||
          /* -ip=noerror */
          (pn->restrictIP == ipNoError && anyError(ip_found)) ||
          /* -ip=strict */
          pn->restrictIP == ipStrict
        ) ip_check = CHECK_WRONG;
      }
      if (ip_check == CHECK_WRONG && !state->to && pn->pwd && strcmp(pn->pwd, "-")) {
        Log (1, "addr: %s (not from allowed remote IP, aborted)", szFTNAddr);
        msg_send2 (state, M_ERR, "Bad source IP", 0);
        return 0;
      }
      else if (ip_check == CHECK_WRONG) {
        Log (2, "addr: %s (not from allowed remote IP, disabled)", szFTNAddr);
        continue;
      }
#endif
    }
#ifndef VAL_STYLE
    else if (pn)
    { /* no check ip -> reset restrict */
      ip_verified = 1;
    }
#endif

    if (pn) state->listed_flag |= pn->listed;
    if (state->expected_pwd[0] && pn)
    {
      char *pwd = state->to ? pn->out_pwd : pn->pwd;
      if (pwd && strcmp(pwd, "-"))
      {
        if (strcmp (pwd, "!") == 0 && ftnaddress_cmp (state->remote_fa, &fa) != 0)
        { /* drop session if remote has aka with disabled password */
          Log (1, "Password authentication disabled for node %s", szFTNAddr);
          state->expected_pwd[0] = '\0';
          continue;
        }
        else if (strcmp (state->expected_pwd, "-") == 0 || strcmp (state->expected_pwd, "!") == 0)
        {
          strnzcpy(state->expected_pwd, pwd, sizeof(state->expected_pwd));
          state->MD_flag |= pn->MD_flag;
        }
        else if (strcmp (state->expected_pwd, pwd) == 0 || strcmp (pwd, "!") == 0)
          state->MD_flag |= pn->MD_flag;
        else
        {
          if (state->to)
            Log (2, "inconsistent pwd settings for this node, aka %s dropped", szFTNAddr);
          else
          { /* drop incoming session with M_ERR "Bad password" */
            Log (1, "inconsistent pwd settings for this node");
            state->expected_pwd[0] = '\0';
          }
          continue;
        }
      }
    }

    if (bsy_add (&fa, F_BSY, config))
    {
#ifndef VAL_STYLE
      Log (2, "addr: %s", szFTNAddr);
#else
      char *s;
      if (ip_check == CHECK_OK) s = "remote IP ok";
      else if (ip_check == CHECK_OFF) s = "remote IP unchecked";
      else s = "remote IP can't be verified";
      Log (2, "addr: %s (%s)", szFTNAddr, s);
#endif
#ifndef HAVE_THREADS
      if (state->nfa == 0)
        setproctitle ("%c %s [%s]",
                      state->to ? 'o' : 'i',
                      szFTNAddr,
                      state->peer_name);
#endif
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
#ifdef BW_LIM
      if (pn && pn->listed) {
        if (pn->bw_send == 0) bw_send_unlim = 1;
        else if (pn->bw_send < 0
                 && (!state->bw_send.rel
                 || (unsigned long)(-pn->bw_send) < state->bw_send.rel))
          state->bw_send.rel = -pn->bw_send;
        else if (pn->bw_send > 0
                 && (!state->bw_send.abs
                 || (unsigned long)pn->bw_send < state->bw_send.abs))
          state->bw_send.abs = pn->bw_send;

        if (pn->bw_recv == 0) bw_recv_unlim = 1;
        else if (pn->bw_recv < 0
                 && (!state->bw_recv.rel
                 || (unsigned long)(-pn->bw_recv) < state->bw_recv.rel))
          state->bw_recv.rel = -pn->bw_recv;
        else if (pn->bw_recv > 0
                 && (!state->bw_recv.abs
                 || (unsigned long)pn->bw_recv < state->bw_recv.abs))
          state->bw_recv.abs = pn->bw_recv;
      }
#endif
    }
    else
    {
      Log (2, "addr: %s (n/a or busy)", szFTNAddr);
#if 1
      if (pn && pn->pwd && strcmp(pn->pwd, "-") && state->to == 0)
      {
        Log (1, "Secure AKA %s busy, drop the session", szFTNAddr);
        msg_sendf (state, M_BSY, "Secure AKA %s busy", szFTNAddr);
        return 0;
      }
#endif
      state->fa = xrealloc (state->fa, sizeof (FTN_ADDR) * ++state->nallfa);
      memcpy (state->fa + state->nallfa - 1, &fa, sizeof (FTN_ADDR));
    }

    if (!state->to && pn)
    { if (pn->ND_flag)
      {
        if (pn->pwd && strcmp(pn->pwd, "-"))
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
      else if (pn->NR_flag)
      {
        if (pn->pwd && strcmp(pn->pwd, "-"))
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
    bad_try (&state->to->fa, "Remote has no needed AKA", BAD_AKA, config);
    return 0;
  }
#ifndef VAL_STYLE
  if (ip_verified < 0)
  { /* strict IP check and no address resolved */
    Log (1, "Remote IP check failed");
    msg_send2 (state, M_ERR, "Bad remote IP", 0);
    return 0;
  }
  else if (ip_verified == 2)
    Log (4, "Remote IP matched");
  else if (state->to == 0)
    Log (5, "Remote IP not checked");
#endif

  if (!state->to)
  {
#ifdef WITH_PERL
    char *s = perl_on_handshake(state);
    if (s && *s) {
      Log (1, "aborted by Perl on_handshake(): %s", s);
      msg_send2 (state, M_ERR, s, 0);
      return 0;
    }
#endif
    if (state->delay_ADR) send_ADR (state, config);

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

#ifdef BW_LIM
  pn = NULL;
  {
    FTN_ADDR dfa = {"defnode", 0, 0, 0, 0};
    pn = get_node_info(&dfa, config);
  }
  if (bw_send_unlim) state->bw_send.abs = state->bw_send.rel = 0;
  /* use defnode's -bw settings if no explicit */
  else if (!state->bw_send.abs && !state->bw_send.rel && pn) {
    if (pn->bw_send >= 0) state->bw_send.abs = pn->bw_send;
    else state->bw_send.rel = -pn->bw_send;
  }
  if (state->bw_send.abs || state->bw_send.rel)
    Log (7, "Session send rate limit is %s cps or %d%%",
            describe_rate(state->bw_send.abs), state->bw_send.rel);

  if (bw_recv_unlim) state->bw_recv.abs = state->bw_recv.rel = 0;
  /* use defnode's -bw settings if no explicit */
  else if (!state->bw_recv.abs && !state->bw_recv.rel && pn) {
    if (pn->bw_recv >= 0) state->bw_recv.abs = pn->bw_recv;
    else state->bw_recv.rel = -pn->bw_recv;
  }
  if (state->bw_recv.abs || state->bw_recv.rel)
    Log (7, "Session recv rate limit is %s cps or %d%%",
            describe_rate(state->bw_recv.abs), state->bw_recv.rel);
#endif

  if (state->to)
  {
    do_prescan (state, config);
    if (state->MD_challenge)
    {
      char *tp=MD_buildDigest(state->to->out_pwd ? state->to->out_pwd : "-", state->MD_challenge);
      if (!tp)
      {
        Log(2, "Unable to build MD5 digest");
        bad_try (&state->to->fa, "Unable to build MD5 digest", BAD_AUTH, config);
        return 0;
      }
      msg_send2 (state, M_PWD, tp, 0);
      state->MD_flag = 1;
      free(tp);
    }
    else if ((state->to->MD_flag == 1) && !no_MD5) /* We do not want to talk without MD5 */
    {
      Log(2, "CRAM-MD5 is not supported by remote");
      bad_try (&state->to->fa, "CRAM-MD5 is not supported by remote", BAD_AUTH, config);
      return 0;
    }
    else
      msg_send2 (state, M_PWD, state->to->out_pwd ? state->to->out_pwd : "-", 0);
  }
  return 1;
}

static char *select_inbound (FTN_ADDR *fa, int secure_flag, BINKD_CONFIG *config)
{
  FTN_NODE *node = NULL;
  char *p;

  if (fa) node = get_node_info(fa, config);
  p = ((fa && node && node->ibox) ? node->ibox :
          (secure_flag == P_SECURE ? config->inbound : config->inbound_nonsecure));
  return p;
}

static int complete_login (STATE *state, BINKD_CONFIG *config)
{
  state->inbound = select_inbound (state->fa, state->state, config);
  if (OK_SEND_FILES (state, config) && state->q == NULL)
    state->q = q_scan_addrs (0, state->fa, state->nfa, state->to ? 1 : 0, config);
  if (OK_SEND_FILES (state, config))
    state->q = q_sort (state->q, state->fa, state->nfa, config);
  state->msgs_in_batch = 0;               /* Forget about login msgs */
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
    Log (3, "Crypt allowed only with MD5 authentication");
  }
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT) && strcmp (state->expected_pwd, "!") == 0)
  { state->crypt_flag = NO_CRYPT;
    Log (3, "Crypt allowed only with password authentication");
  }
  else if (state->crypt_flag == (WE_CRYPT|THEY_CRYPT))
  { char *p;
    state->crypt_flag = YES_CRYPT;
    Log (3, "session in CRYPT mode");
    if (state->to)
    { init_keys(state->keys_out, state->to->out_pwd ? state->to->out_pwd : "-");
      init_keys(state->keys_in,  "-");
      for (p=state->to->out_pwd ? state->to->out_pwd : "-"; *p; p++)
        update_keys(state->keys_in, (int)*p);
    } else
    { init_keys(state->keys_in, state->expected_pwd);
      init_keys(state->keys_out,  "-");
      for (p=state->expected_pwd; *p; p++)
        update_keys(state->keys_out, (int)*p);
    }
  }
  if (state->crypt_flag!=YES_CRYPT) state->crypt_flag=NO_CRYPT;
#ifdef WITH_PERL
  {
    char *s = perl_after_handshake(state);
    if (s && *s) {
      Log (1, "aborted by Perl after_handshake(): %s", s);
      msg_send2 (state, M_ERR, s, 0);
      return 0;
    }
  }
#endif
  return 1;
}

static int PWD (STATE *state, char *pwd, int sz, BINKD_CONFIG *config)
{
  int bad_pwd=STRNICMP(pwd, "CRAM-", 5);
  int no_password=!strcmp (state->expected_pwd, "-");
  char *szOpt;

  UNUSED_ARG(sz);

  if (state->to)
  { Log (1, "unexpected password from the remote on outgoing call: `%s'", pwd);
    return 1;
  }
  if (state->state != P_NULL)
  { Log (2, "Double M_PWD from remote! Ignored.", pwd);
    msg_send2 (state, M_NUL, "MSG Warning: double of password is received (M_PWD more one)!", 0);
    return 0;
  }

  if (no_password && bad_pwd)
  {
    do_prescan (state, config);
    state->state = P_NONSECURE;
    if (strcmp (pwd, "-"))
      Log (1, "unexpected password from the remote: `%s'", pwd);
  }
  else
  {
    if ((state->MD_flag == 1) || (!bad_pwd && state->MD_challenge))
    {
      char *sp;
      if (bad_pwd && state->MD_flag)
      {
        msg_send2(state, M_ERR, "You must support MD5", 0);
        Log (1, "Caller does not support MD5");
        return 0;
      }
      state->MD_flag = 1;
      if (strcmp (state->expected_pwd, "!") == 0)
        bad_pwd = 0;
      else if ((sp = MD_buildDigest(state->expected_pwd, state->MD_challenge)) != NULL)
      {
        bad_pwd = (STRICMP(sp, pwd) != 0);
        free(sp);
        sp = NULL;
      }
      else
      {
        Log (2, "Unable to build Digest");
        bad_pwd = 1;
      }
    }
    else
    {
      bad_pwd = (state->expected_pwd[0] == 0 || strcmp (state->expected_pwd, pwd));
      if (strcmp (state->expected_pwd, "!") == 0)
        bad_pwd = 0;
    }

    if (bad_pwd && !no_password) /* I don't check password if we do not need one */
    {
      msg_send2 (state, M_ERR, "Bad password", 0);
      Log (1, "`%s': incorrect password", pwd);
      return 0;
    }
    else
    {
      if (no_password)
      {
        state->state = P_NONSECURE;
        do_prescan (state, config);
        if (bad_pwd) {
          Log (1, "unexpected password digest from the remote");
          state->state_ext = P_WE_NONSECURE;
        }
      }
      else
      {
        state->state = P_SECURE;
        do_prescan (state, config);
      }
    }
  }

  if (state->state != P_SECURE)
    state->crypt_flag = NO_CRYPT;
  else if (state->crypt_flag == (THEY_CRYPT | WE_CRYPT) && !state->MD_flag)
  { state->crypt_flag = NO_CRYPT;
    Log (4, "Crypt allowed only with MD5 authorization");
  }
  else if (state->crypt_flag == (THEY_CRYPT | WE_CRYPT) && strcmp (state->expected_pwd, "!") == 0)
  { state->crypt_flag = NO_CRYPT;
    Log (3, "Crypt allowed only with password authentication");
  }

  if ((state->ND_flag & WE_ND) && (state->ND_flag & CAN_NDA) == 0)
    state->ND_flag |= THEY_ND;
  if ((state->ND_flag & WE_ND) == 0 && (state->ND_flag & CAN_NDA) == 0)
    state->ND_flag &= ~THEY_ND;
  if (state->buggy_NR && (state->NR_flag & WANT_NR))
  { /* workaround bug of old binkd */
    /* force symmetric NR-mode with it */
    state->NR_flag |= WE_NR;
    Log (5, "Turn on NR-mode with this link (remote has buggy NR)");
  }

  szOpt = xstrdup(" EXTCMD");
  if (state->NR_flag & WANT_NR) xstrcat(&szOpt, " NR");
  if (state->ND_flag & THEY_ND) xstrcat(&szOpt, " ND");
  if ((!(state->ND_flag & WE_ND)) != (!(state->ND_flag & THEY_ND))) xstrcat(&szOpt, " NDA");
  if (state->crypt_flag == (WE_CRYPT | THEY_CRYPT)) xstrcat(&szOpt, " CRYPT");
#ifdef WITH_ZLIB
  if (state->z_canrecv & 1) xstrcat(&szOpt, " GZ");
#endif
#ifdef WITH_BZLIB2
  if (state->z_canrecv & 2) xstrcat(&szOpt, " BZ2");
#endif
  msg_send2 (state, M_NUL, "OPT", szOpt);
  xfree (szOpt);
  msg_send2 (state, M_OK, state->state==P_SECURE ? "secure" : "non-secure", 0);
  return complete_login (state, config);
}

static int OK (STATE *state, char *buf, int sz, BINKD_CONFIG *config)
{
  char *w;
  int i;
  UNUSED_ARG(sz);

  if (!state->to) return 0;
  state->state = state->to->out_pwd && strcmp(state->to->out_pwd, "-") != 0 ? P_SECURE : P_NONSECURE;
  for (i = 1; (w = getwordx (buf, i, 0)) != 0; ++i)
  {
    if (state->state == P_SECURE && strcmp(w, "non-secure") == 0)
    {
      state->crypt_flag=NO_CRYPT; /* some development binkd versions send OPT CRYPT with unsecure session */
      Log (1, "Warning: remote set UNSECURE session");
      state->state_ext = P_REMOTE_NONSECURE;
    }
    free(w);
  }
  if (state->ND_flag == WE_ND || state->ND_flag == THEY_ND)
    state->ND_flag = 0; /* remote does not support asymmetric ND-mode */
  return complete_login (state, config);
}

/* val: checks file against skip rules */
struct skipchain *skip_test(STATE *state, BINKD_CONFIG *config)
{
  struct skipchain *ps;
  addrtype amask = 0;

  amask |= (state->listed_flag) ? A_LST : A_UNLST;
  amask |= (state->state == P_SECURE) ? A_PROT : A_UNPROT;
  for (ps = config->skipmask.first; ps; ps = ps->next)
  {
    if ( (ps->atype & amask) && pmatch_ncase(ps->mask, state->in.netname) )
    {
      if (ps->size >=0 && state->in.size >= ps->size)
        return ps;
      return NULL;
    }
  }
  return NULL;
}

#ifdef BW_LIM
static void setup_rate_limit (STATE *state, BINKD_CONFIG *config, BW *bw,
                                char *fname)
{
  struct ratechain *ps;
  addrtype amask = 0;
  int rlim = -100;

  amask |= (state->listed_flag) ? A_LST : A_UNLST;
  amask |= (state->state == P_SECURE) ? A_PROT : A_UNPROT;
  for (ps = config->rates.first; ps; ps = ps->next)
  {
    if ( (ps->atype & amask) && pmatch_ncase(ps->mask, fname) )
    {
      Log (7, "%s matches rate limit mask %s", fname, ps->mask);
      rlim = ps->rate;
      break;
    }
  }
  /* if mask is set to unlim rate, set to unlim */
  if (rlim == 0) bw->rlim = 0;
  else {
    /* if mask set to specific rate, adjust to remote percent rate */
    if (rlim > 0)
      bw->rlim = bw->rel ? rlim * bw->rel / 100 : rlim;
    /* if mask set to relative rate, adjust node absolute rate by it */
    else
      bw->rlim = bw->abs ? bw->abs * (-rlim) / 100 : 0;
  }
#ifdef WITH_PERL
  perl_setup_rlimit(state, bw, fname);
#endif
  if (bw->rlim)
    Log (3, "rate limit for %s is %d cps", fname, bw->rlim);
  else
    Log (5, "rate for %s is unlimited", fname);
  bw->utime.tv_sec = bw->utime.tv_usec = 0;
}

static void setmintv(struct timeval *tv, unsigned long msecs)
{
  unsigned long sec, usec;

  sec = msecs / 1000000;
  usec = msecs % 1000000;
  if (tv->tv_sec > sec || (tv->tv_sec == sec && tv->tv_usec > usec)) {
    tv->tv_sec = sec;
    tv->tv_usec = usec;
  }
}

#define BW_TIME_INT  10000000ul                /* 10 sec */
static int check_rate_limit(BW *bw, struct timeval *tv)
{
  struct timeval ctime;
  double cps;
  unsigned long dt;

  if (bw->rlim == 0) return 0;
  gettvtime(&ctime);
  if (ctime.tv_sec < bw->utime.tv_sec ||
      (ctime.tv_sec == bw->utime.tv_sec && ctime.tv_usec < bw->utime.tv_usec)) {
    Log(3, "System time steps back, reset rate-limiting");
    bw->utime.tv_sec = bw->utime.tv_usec = 0;
  }
  if (bw->utime.tv_sec == 0 && bw->utime.tv_usec == 0) {
    bw->utime.tv_sec = ctime.tv_sec;
    bw->utime.tv_usec = ctime.tv_usec;
    bw->cps = 0;
    bw->cpsN = 0;
    return 0;
  }
  if (ctime.tv_sec == bw->utime.tv_sec && ctime.tv_usec == bw->utime.tv_usec) {
    if (bw->rlim && bw->bytes > bw->rlim) {
      setmintv(tv, 100000); /* 0.1 sec */
      return 1;
    }
    else
      return 0;
  }
  dt = (ctime.tv_sec - bw->utime.tv_sec) * 1000000ul + ctime.tv_usec - bw->utime.tv_usec;
  cps = (bw->bytes * 1000000. / dt);
  bw->bytes = 0;
  bw->utime.tv_sec = ctime.tv_sec;
  bw->utime.tv_usec = ctime.tv_usec;
  if (bw->cpsN < BW_TIME_INT) {
    bw->cps = (bw->cpsN * bw->cps + cps * dt) / (bw->cpsN + dt);
    bw->cpsN += dt;
    if (bw->cpsN > BW_TIME_INT) bw->cpsN = BW_TIME_INT;
  }
  else if (dt >= BW_TIME_INT) bw->cps = cps;
  else bw->cps = ((BW_TIME_INT - dt) * bw->cps + cps * dt) / BW_TIME_INT;
  Log (9, "current cps is %u, avg. cps is %u", (int)cps, (int)bw->cps);
  if (bw->cps <= bw->rlim)
    return 0;
  dt = (unsigned long) (bw->cpsN * (bw->cps / bw->rlim - 1) + 1000);
  setmintv(tv, dt);
  return 1;
}
#endif

/*
 * Handles M_FILE msg from the remote
 * M_FILE args: "%s %lu %lu %lu", filename, size, time, offset
 * M_FILE ext.: "%s %lu %lu %lu %s...", space-separated params
 *     currently, only GZ and BZ2 parameters supported
 */
static int start_file_recv (STATE *state, char *args, int sz, BINKD_CONFIG *config)
{
  int argc = 4;
  char *argv[4], *w;
  boff_t offset;
  UNUSED_ARG(sz);

  if ((args = parse_msg_args (argc, argv, args, "M_FILE", state)) != NULL)
  {
    /* They request us for offset (M_FILE "name size time -1") */
    int off_req = 0;

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    if (state->z_recv && state->z_idata)
    {
      decompress_deinit(state->z_recv, state->z_idata);
      state->z_idata = NULL;
    }
    state->z_recv = 0;
    state->z_isize = state->z_cisize = 0;
#endif
    if (state->in.f &&                       /* Already receiving smthing */
        strcmp (argv[0], state->in.netname))        /* ...a file with another *
                                                 * name! */
    {
      close_partial (state, config);
    }
    if ((state->ND_flag & THEY_ND) && state->in_complete.netname[0])
    { /* rename complete received file to its true form */
      /* and run events */
      if (inb_done (&(state->in_complete), state, config) == 0)
      {
        msg_send2 (state, M_ERR, "Local error saving file", 0);
        if (state->to)
          bad_try (&state->to->fa, "Local error saving file", BAD_IO, config);
        return 0; /* error, drop session */
      }
      TF_ZERO (&state->in_complete);
    }
    if (state->in.f == 0)
    { char *errmesg=NULL;

      state->in.start = safe_time();
      strnzcpy (state->in.netname, argv[0], MAX_NETNAME);
      state->in.size = (boff_t)strtoumax (argv[1], NULL, 10);
      errno=0;
      state->in.time = safe_atol (argv[2], &errmesg);
      if (errmesg) {
          Log ( 1, "File time parsing error: %s! (M_FILE \"%s %s %s %s\")", errmesg, argv[0], argv[1], argv[0], argv[2], argv[3] );
      }
    }
    offset = (boff_t) strtoumax (argv[3], NULL, 10);
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
      char realname[MAXPATHLEN + 1];
      struct skipchain *mask;
#ifdef WITH_PERL
      int rc;

      if ((rc = perl_before_recv(state, offset)) > 0) {
        Log (1, "skipping %s (%sdestructive, %" PRIuMAX " byte(s), by Perl before_recv)",
             state->in.netname, rc == SKIP_D ? "" : "non-",
             (uintmax_t) state->in.size);
        msg_sendf (state, (t_msg)(rc == SKIP_D ? M_GOT : M_SKIP),
                   "%s %" PRIuMAX " %" PRIuMAX,
                   state->in.netname,
                   (uintmax_t) state->in.size,
                   (uintmax_t) state->in.time);
        return 1;
      }
#endif
      /* val: skip check */
      if ((mask = skip_test(state, config)) != NULL) {
        Log (1, "skipping %s (%sdestructive, %" PRIuMAX " byte(s), mask %s)",
             state->in.netname, mask->destr ? "" : "non-",
             (uintmax_t) state->in.size, mask->mask);
        msg_sendf (state, (t_msg)(mask->destr ? M_GOT : M_SKIP),
                   "%s %" PRIuMAX " %" PRIuMAX,
                   state->in.netname,
                   (uintmax_t) state->in.size,
                   (uintmax_t) state->in.time);
        return 1;
      }
      /* val: /skip check */

      if (inb_test (state->in.netname, state->in.size,
                    state->in.time, state->inbound, realname,
                    config->renamestyle))
      {
        Log (2, "already have %s (%s, %" PRIuMAX " byte(s))",
             state->in.netname, realname, (uintmax_t) state->in.size);
        msg_sendf (state, M_GOT, "%s %" PRIuMAX " %" PRIuMAX,
                   state->in.netname,
                   (uintmax_t) state->in.size,
                   (uintmax_t) state->in.time);
        return 1;
      }
      else if (!state->skip_all_flag)
      {
        if ((state->in.f = inb_fopen (state, config)) == 0)
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
        msg_sendf (state, M_SKIP, "%s %" PRIuMAX " %" PRIuMAX,
                   state->in.netname,
                   (uintmax_t) state->in.size,
                   (uintmax_t) state->in.time);
        if (state->in.f)
          fclose (state->in.f);
        TF_ZERO (&state->in);
        return 1;
      }
    }

    if (off_req || offset != ftello (state->in.f))
    {
      Log (2, "have %" PRIuMAX " byte(s) of %s",
           (uintmax_t) ftello (state->in.f), state->in.netname);
      msg_sendf (state, M_GET, "%s %" PRIuMAX " %" PRIuMAX " %" PRIuMAX,
                 state->in.netname,
                 (uintmax_t) state->in.size, (uintmax_t) state->in.time,
                 (uintmax_t) ftello (state->in.f));
      ++state->GET_FILE_balance;
      fclose (state->in.f);
      TF_ZERO (&state->in);
      return 1;
    }
    else if (offset != 0 || (state->NR_flag & THEY_NR))
    {
      --state->GET_FILE_balance;
    }

    Log (3, "receiving %s (%" PRIuMAX " byte(s), off %" PRIuMAX ")",
         state->in.netname, (uintmax_t) (state->in.size), (uintmax_t) offset);
#ifdef BW_LIM
    setup_rate_limit(state, config, &state->bw_recv, state->in.netname);
#endif

    for (argc = 1; (w = getwordx (args, argc, 0)) != 0; ++argc)
    {
      if (w[0] == '\0') ;
#ifdef WITH_ZLIB
      else if (strcmp(w, "GZ") == 0)
      {
        if (state->z_recv == 0)
          Log (4, "gzip mode is on for %s", state->in.netname);
        state->z_recv |= 1;
      }
#endif
#ifdef WITH_BZLIB2
      else if (strcmp(w, "BZ2") == 0)
      {
        if (state->z_recv == 0)
          Log (4, "bzip2 mode is on for %s", state->in.netname);
        state->z_recv |= 2;
      }
#endif
      else
        Log (4, "Unknown option %s for %s ignored", w, state->in.netname);
      free(w);
    }

#if defined(WITH_ZLIB) && defined(WITH_BZLIB2)
    if (state->z_recv == 3) {
      Log (1, "Both GZ and BZ2 extras are specified for %s", state->in.netname);
      msg_send2 (state, M_ERR, "Can't handle GZ and BZ2 at the same time for ", state->in.netname);
      return 0;
    }
#endif

    if (fseeko (state->in.f, offset, SEEK_SET) == -1)
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

static int ND_set_status(char *status, FTN_ADDR *fa, STATE *state, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN+1];
  FILE *f;
  int  rc;

  UNUSED_ARG(state);

  if (fa->z==-1)
  { Log(8, "ND_set_status: unknown address for '%s'", status);
    return 0;
  }
  ftnaddress_to_filename (buf, fa, config);
  if (*buf=='\0') return 0;
  strnzcat(buf, ".stc", sizeof(buf));
  if (!status || !*status)
  {
    if (unlink(buf) == 0)
    { Log(5, "Clean link status for %u:%u/%u.%u",
          fa->z, fa->net, fa->node, fa->p);
      return 1;
    }
    rc = errno;
    if (access(buf, F_OK) == 0)
    { Log(1, "Can't unlink %s: %s!", buf, strerror(rc));
      return 0;
    }
    return 1;
  }
  else
  {
    Log(5, "Set link status for %u:%u/%u.%u to '%s'",
        fa->z, fa->net, fa->node, fa->p, status);
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

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
static void z_send_init(STATE *state, BINKD_CONFIG *config, char **extra)
{
  int rc;

  *extra = "";
  if (state->z_cansend && state->extcmd && state->out.size >= config->zminsize
      && zrule_test(ZRULE_ALLOW, state->out.netname, config->zrules.first)) {
#ifdef WITH_BZLIB2
    if (!state->z_send && (state->z_cansend & 2)) {
      *extra = " BZ2"; state->z_send = 2;
      Log (4, "bzip2 mode is on for %s", state->out.netname);
    }
#endif
#ifdef WITH_ZLIB
    if (!state->z_send && (state->z_cansend & 1)) {
      *extra = " GZ"; state->z_send = 1;
      Log (4, "gzip mode is on for %s", state->out.netname);
    }
#endif
    if (state->z_send)
      if ((rc = compress_init(state->z_send, config->zlevel, &state->z_odata)))
      {
        Log (1, "compress_init failed (rc=%d), send uncompressed file %s",
             rc, state->out.netname);
        *extra = "";
        state->z_send = 0;
      }
    state->z_osize = state->z_cosize = 0;
  }
}

static void z_send_stop(STATE *state)
{
  if (state->z_send && state->z_odata)
  { compress_abort(state->z_send, state->z_odata);
    state->z_odata = NULL;
    state->z_oleft = 0;
  }
  state->z_send = 0;
  state->z_osize = state->z_cosize = 0;
}
#else
#define z_send_init(state, config, extra)        (*(extra) = "")
#define z_send_stop(state)
#endif

/*
 * M_GET message from the remote: Resend file from offset
 * M_GET args: "%s %lu %lu %lu", filename, size, time, offset
 */
static int GET (STATE *state, char *args, int sz, BINKD_CONFIG *config)
{
  int argc = 4;
  char *argv[4];
  char *extra;
  int i, rc = 0, nz = 0;
  boff_t offset, fsize=0;
  time_t ftime=0;

  UNUSED_ARG(sz);

  if ((args = parse_msg_args (argc, argv, args, "M_GET", state)) != NULL)
  { {char *errmesg = NULL;
      fsize = (boff_t)strtoumax (argv[1], NULL, 10);
      ftime = safe_atol (argv[2], &errmesg);
      if(errmesg){
        Log ( 1, "File time parsing error: %s! (M_GET \"%s %s %s %s\")", errmesg, argv[0], argv[1], argv[0], argv[2], argv[3] );
      }
    }
    /* Check if the file was already sent */
    for (i = 0; i < state->n_sent_fls; ++i)
    {
      if (!tfile_cmp (state->sent_fls + i, argv[0], fsize, ftime))
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
         !tfile_cmp (&state->out, argv[0], fsize, ftime))
    {
      if (!state->out.f)
      { /* response for status */
        rc = 1;
        /* to satisfy remote GET_FILE_balance */
        msg_sendf (state, M_FILE, "%s %" PRIuMAX " %" PRIuMAX " %" PRIuMAX,
                   state->out.netname, (uintmax_t) state->out.size,
                   (uintmax_t) state->out.time, strtoumax(argv[3], NULL, 10));
        if (strtoumax(argv[3], NULL, 10) == (uintmax_t) state->out.size &&
            (state->ND_flag & WE_ND))
        {
          state->send_eof = 1;
          state->waiting_for_GOT = 1;
          Log(5, "Waiting for M_GOT");
          state->off_req_sent = 0;
          return rc;
        }
        else
          /* request from offset 0 - file already renamed */
          ND_set_status("", &state->out.fa, state, config);
        TF_ZERO(&state->out);
      }
      else if ((offset = (boff_t)strtoumax (argv[3], NULL, 10)) > state->out.size)
      {
        Log (1, "GET: remote requests seeking %s to %" PRIuMAX ", file size " PRIuMAX,
             argv[0], (uintmax_t) offset, (uintmax_t) state->out.size);
        msg_sendf(state, M_ERR, "Invalid M_GET violates binkp: offset " PRIuMAX " after end of file, file %s size " PRIuMAX,
                  (uintmax_t)offset, argv[0], (uintmax_t)state->out.size);
        /* touch the file and drop session */
        fclose(state->out.f);
        state->out.f=NULL;
        touch(state->out.path, time(NULL));
        rc = 0;
      }
      else if (fseeko (state->out.f, offset, SEEK_SET) == -1)
      {
        Log (1, "GET: error seeking %s to %" PRIuMAX ": %s",
             argv[0], (uintmax_t) offset, strerror (errno));
        msg_sendf(state, M_ERR, "Error seeking: %s size " PRIuMAX " to offset " PRIuMAX,
                  argv[0], (uintmax_t)state->out.size, (uintmax_t)offset);
        fclose(state->out.f);
        state->out.f=NULL;
        rc = 0;
      }
      else
      {
        Log (2, "sending %s from %" PRIuMAX, argv[0], (uintmax_t) offset);
        for (argc = 1; (extra = getwordx (args, argc, 0)) != 0; ++argc)
        {
          if (strcmp(extra, "GZ") == 0 || strcmp(extra, "BZ2") == 0) ;
          else if (strcmp(extra, "NZ") == 0) nz = 1;
          else if (extra[0])
            Log (4, "Unknown option %s for %s ignored", extra, argv[0]);
          free(extra);
        }
        z_send_stop(state);
        if (!nz) z_send_init(state, config, &extra);
        msg_sendf (state, M_FILE, "%s %" PRIuMAX " %" PRIuMAX " %" PRIuMAX "%s",
                   state->out.netname,
                   (uintmax_t) state->out.size,
                   (uintmax_t) state->out.time,
                   (uintmax_t) offset, extra);
        rc = 1;
      }
    }
    else
      Log (1, "unexpected M_GET for %s", argv[0]);
    ND_set_status("", &state->ND_addr, state, config);
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
 * M_SKIP args: "%s %lu %lu", filename, size, time
 */
static int SKIP (STATE *state, char *args, int sz, BINKD_CONFIG *config)
{
  const int argc = 3;
  char *argv[3];
  int n;
  time_t ftime=0;
  boff_t fsize=0;

  UNUSED_ARG(sz);

  if (parse_msg_args (argc, argv, args, "M_SKIP", state))
  {
    {
      char *errmesg=NULL;
      fsize = (boff_t)strtoumax (argv[1], NULL, 10);
      ftime = safe_atol (argv[2], &errmesg);
      if (errmesg)
      {
        Log ( 1, "File time parsing error: %s! (M_SKIP \"%s %s %s\")", errmesg, argv[0], argv[1], argv[0], argv[2] );
      }
    }
    for (n = 0; n < state->n_sent_fls; ++n)
    {
      if (!tfile_cmp (state->sent_fls + n, argv[0], fsize, ftime))
      {
        state->r_skipped_flag = 1;
        Log (2, "%s skipped by remote", state->sent_fls[n].netname);
        memcpy (&state->ND_addr, &state->sent_fls[n].fa, sizeof(FTN_ADDR));
        remove_from_sent_files_queue (state, n);
      }
    }
    if (!tfile_cmp (&state->out, argv[0], fsize, ftime))
    {
      state->r_skipped_flag = 1;
      if (state->out.f)
        fclose (state->out.f);
      else
      {
        Log (1, "Cannot skip ND-status, session dropped");
        msg_send2(state, M_ERR, "Cannot skip ND-status", 0);
        return 0;
      }
      Log (2, "%s skipped by remote", state->out.netname);
      TF_ZERO (&state->out);
    }
    ND_set_status("", &state->ND_addr, state, config);
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
 * M_GOT args: "%s %lu %lu", filename, size, time
 */
static int GOT (STATE *state, char *args, int sz, BINKD_CONFIG *config)
{
  const int argc = 3;
  char *argv[3];
  int n, rc=1;
  char *status = NULL;
  time_t ftime=0;
  boff_t fsize=0;
  char *errmesg = NULL;
  char *saved_args;

  UNUSED_ARG(sz);

  saved_args = xstrdup(args);

  if (parse_msg_args (argc, argv, args, "M_GOT", state))
  {
    if ((state->NR_flag & WE_NR) && !(state->ND_flag & WE_ND))
      ND_set_status("", &state->ND_addr, state, config);
    else
      status = saved_args;
    fsize = (boff_t) strtoumax (argv[1], NULL, 10);
    errno = 0;
    ftime = safe_atol (argv[2], &errmesg);
    if (errmesg)
    {
      Log ( 1, "File time parsing error: %s! (M_GOT \"%s %s %s\")", errmesg, argv[0], argv[1], argv[0], argv[2] );
    }
    if (!tfile_cmp (&state->out, argv[0], fsize, ftime))
    {
      Log (2, "remote already has %s", state->out.netname);
      if (state->out.f)
      {
        fclose (state->out.f);
        state->out.f = NULL;
      }
      memcpy(&state->ND_addr, &state->out.fa, sizeof(state->out.fa));
      if (state->ND_flag & WE_ND)
        Log (7, "Set ND_addr to %u:%u/%u.%u",
             state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
      if (status)
      {
        if (state->off_req_sent)
          rc = ND_set_status("", &state->ND_addr, state, config);
        else
          rc = ND_set_status(status, &state->ND_addr, state, config);
      }
      state->waiting_for_GOT = state->off_req_sent = 0;
      Log(9, "Don't waiting for M_GOT");
      remove_from_spool (state, state->out.flo,
                         state->out.path, state->out.action, config);
      TF_ZERO (&state->out);
    }
    else
    {
      for (n = 0; n < state->n_sent_fls; ++n)
      {
        if (!tfile_cmp (state->sent_fls + n, argv[0], fsize, ftime))
        {
          char szAddr[FTN_ADDR_SZ + 1];

          ftnaddress_to_str (szAddr, &state->sent_fls[n].fa);
          state->bytes_sent += state->sent_fls[n].size;
          ++state->files_sent;
          memcpy (&state->ND_addr, &state->sent_fls[n].fa, sizeof(FTN_ADDR));
          if (state->ND_flag & WE_ND)
             Log (7, "Set ND_addr to %u:%u/%u.%u",
                  state->ND_addr.z, state->ND_addr.net, state->ND_addr.node, state->ND_addr.p);
          Log (2, "sent: %s (%" PRIuMAX ", %.2f CPS, %s)",
               state->sent_fls[n].path,
               (uintmax_t) state->sent_fls[n].size,
               (double) (state->sent_fls[n].size) /
               (safe_time() == state->sent_fls[n].start ?
                1 : (safe_time() - state->sent_fls[n].start)), szAddr);
          if (status)
          {
            if (state->off_req_sent || !(state->ND_flag & WE_ND))
              rc = ND_set_status("", &state->ND_addr, state, config);
            else
              rc = ND_set_status(status, &state->ND_addr, state, config);
          }
          state->waiting_for_GOT = 0;
          Log(9, "Don't waiting for M_GOT");
#ifdef WITH_PERL
          perl_after_sent(state, n);
#endif
          remove_from_spool (state, state->sent_fls[n].flo,
                        state->sent_fls[n].path, state->sent_fls[n].action, config);
          remove_from_sent_files_queue (state, n);
          break;                       /* we have ACK for _ONE_ file */
        }
      }
    }
  }
  else
    rc = 0;

  free(saved_args);
  return rc;
}

static int EOB (STATE *state, char *buf, int sz, BINKD_CONFIG *config)
{
  UNUSED_ARG(buf);
  UNUSED_ARG(sz);

  state->remote_EOB = 1;
  if (state->in.f)
  {
    boff_t offset;
 
    offset = ftello (state->in.f);
    if ((state->NR_flag & THEY_NR) == 0 && offset != 0)
    {
      char nodestr[FTN_ADDR_SZ];
      ftnaddress_to_str (nodestr, state->fa);
      fclose (state->in.f);
      state->in.f = NULL;
      Log (1, "receiving of %s interrupted", state->in.netname);
      Log (2, "Remove partially received %s (%" PRIuMAX " of %" PRIuMAX " bytes) due to remote bug",
          state->in.netname, (uintmax_t) offset, (uintmax_t) state->in.size);
      Log (1, "Turn on the NR mode for node %s in config to prevent this error, please", nodestr);
      inb_reject (state, config);
      TF_ZERO (&state->in);
    }
    else
      close_partial (state, config);
  }
  if ((state->ND_flag & THEY_ND) && state->in_complete.netname[0])
  { /* rename complete received file to its true form */
    /* and run events */
    if (inb_done (&(state->in_complete), state, config) == 0)
    {
      msg_send2 (state, M_ERR, "Local error saving file", 0);
      if (state->to)
        bad_try (&state->to->fa, "Local error saving file", BAD_IO, config);
      return 0; /* error, drop session */
    }
    TF_ZERO (&state->in_complete);
  }
  state->delay_EOB = 0; /* val: we can do it anyway now :) */
  return 1;
}

typedef int command (STATE *state, char *buf, int sz, BINKD_CONFIG *config);
static command *commands[] =
{
  NUL, ADR, PWD, start_file_recv, OK, EOB, GOT, RError, BSY, GET, SKIP
};

/* Recvs next block, processes msgs or writes down the data from the remote */
static int recv_block (STATE *state, BINKD_CONFIG *config)
{
  int no;

  int sz = state->isize == -1 ? BLK_HDR_SIZE : state->isize;

  if (sz == 0)
    no = 0;
  else
  {
    if (state->pipe)
      no = read (state->s_in, state->ibuf + state->iread, sz - state->iread);
    else
      no = recv (state->s_in, state->ibuf + state->iread, sz - state->iread, 0);
    Log (9, "Read %i bytes", no);
    if (no == -1)
    {
      const char *save_err;

      if (state->pipe && (errno == EWOULDBLOCK || errno == EAGAIN))
        return 1;
      if (!state->pipe && (TCPERRNO == TCPERR_WOULDBLOCK || TCPERRNO == TCPERR_AGAIN))
        return 1;
      save_err = state->pipe ? strerror(errno) : TCPERR();
      state->io_error = 1;
      if (!binkd_exit)
      {
        Log (1, "%s: %s", state->pipe ? "read" : "recv", save_err);
        if (state->to)
          bad_try (&state->to->fa, save_err, BAD_IO, config);
      }
      return 0;
    }
  }
#ifdef BW_LIM
  state->bw_recv.bytes += no;
#endif
  if (state->crypt_flag == YES_CRYPT)
    decrypt_buf(state->ibuf + state->iread, no, state->keys_in);
  state->iread += no;
  /* assert (state->iread <= sz); */
  if (state->iread == sz)
  {
    if (state->isize == -1)               /* reading block header */
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

#ifdef WITH_PERL
        perl_on_recv(state, state->ibuf, state->isize);
#endif
        if (state->isize == 0)
          Log (1, "zero length command from remote (must be at least 1)");
        else if ((unsigned) (state->ibuf[0]) > M_MAX)
          Log (1, "unknown msg type from remote: %u", state->ibuf[0]);
        else
        {
          state->ibuf[state->isize] = 0;
          Log (5, "rcvd msg %s %s", scommand[(unsigned char)(state->ibuf[0])], state->ibuf+1);
          rc = commands[(unsigned) (state->ibuf[0])]
            (state, state->ibuf + 1, state->isize - 1, config);
        }

        if (rc == 0)
        {
          state->iread = 0;
          return 0;
        }
      }
      else if (state->in.f)
      {
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
        if (state->z_recv)
        {
          int rc = 0, nget = state->isize, zavail, nput;
          char zbuf[ZBLKSIZE];
          char *buf = state->ibuf;

          if (state->z_idata == NULL)
          {
            if (decompress_init(state->z_recv, &state->z_idata))
            {
              Log (1, "Can't init decompress");
              return 0;
            } else
              Log (8, "decompress_init success");
          }
          while (nget)
          {
            zavail = ZBLKSIZE;
            nput = nget;
            rc = do_decompress(state->z_recv, zbuf, &zavail, buf, &nput,
                               state->z_idata);
            if (rc < 0)
            {
              Log (1, "Decompress %s error %d", state->in.netname, rc);
              return 0;
            }
            else
              Log (10, "%d bytes of data decompressed to %d", nput, zavail);
            if (zavail != 0 && fwrite (zbuf, zavail, 1, state->in.f) < 1)
            {
              Log (1, "write error: %s", strerror(errno));
              decompress_abort(state->z_recv, state->z_idata);
              state->z_idata = NULL;
              return 0;
            }
            buf += nput;
            nget -= nput;
            state->z_isize += zavail;
            state->z_cisize += nput;
          }
          if (rc == 1)
          { if ((rc = decompress_deinit(state->z_recv, state->z_idata)) < 0)
              Log (1, "decompress_deinit retcode %d", rc);
            state->z_idata = NULL;
          }
          if (fflush(state->in.f))
          {
            Log (1, "write error: %s", strerror(errno));
            return 0;
          }
        }
        else
#endif
        if (state->isize != 0 &&
            (fwrite (state->ibuf, state->isize, 1, state->in.f) < 1 ||
            fflush (state->in.f)))
        {
          Log (1, "write error: %s", strerror(errno));
          return 0;
        }
        if (config->percents && state->in.size > 0)
        {
          LockSem(&lsem);
          printf ("%-20.20s %3.0f%%\r", state->in.netname,
                  100.0 * ftello (state->in.f) / (float) state->in.size);
          fflush (stdout);
          ReleaseSem(&lsem);
        }
        if (ftello (state->in.f) == state->in.size)
        {
          if (fclose (state->in.f))
          {
            Log (1, "Cannot fclose(%s): %s!",
                 state->in.netname, strerror (errno));
            state->in.f = NULL;
            return 0;
          }
          state->in.f = NULL;
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
          if (state->z_recv)
          {
            Log (4, "File %s compressed size %" PRIuMAX " bytes, compress ratio %.1f%%",
                 state->in.netname, (uintmax_t) state->z_cisize,
                 100.0 * state->z_cisize / state->z_isize);
            if (state->z_idata)
            {
              Log (1, "Warning: extra compressed data ignored");
              decompress_deinit(state->z_recv, state->z_idata);
              state->z_idata = NULL;
            }
          }
#endif
          if (state->ND_flag & THEY_ND)
          {
            Log (5, "File %s complete received, waiting for renaming",
                 state->in.netname);
            memcpy(&state->in_complete, &state->in, sizeof(state->in_complete));
          }
          else
          {
            if (inb_done (&(state->in), state, config) == 0)
            {
              msg_send2 (state, M_ERR, "Local error saving file", 0);
              if (state->to)
                bad_try (&state->to->fa, "Local error saving file", BAD_IO, config);
              return 0; /* error, drop session */
            }
          }
          msg_sendf (state, M_GOT, "%s %" PRIuMAX " %" PRIuMAX,
                     state->in.netname,
                     (uintmax_t) state->in.size,
                     (uintmax_t) state->in.time);
          TF_ZERO (&state->in);
        }
        else if (ftello (state->in.f) > state->in.size)
        {
          Log (1, "rcvd %" PRIuMAX " extra bytes!",
               (uintmax_t) (ftello (state->in.f) - state->in.size));
          return 0;
        }
      }
      else if (state->isize > 0)
      {
        Log (7, "ignoring data block (%" PRIuMAX " byte(s))",
             (uintmax_t) state->isize);
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
      char *s_err = "connection closed by foreign host";
      Log (1, "recv: %s", s_err);
      if (state->to)
        bad_try (&state->to->fa, s_err, BAD_IO, config);
    }
    return 0;
  }
  else
    return 1;
}

static int banner (STATE *state, BINKD_CONFIG *config)
{
  int tz;
  char szLocalTime[60];
  char *szOpt;
  time_t t;
  struct tm tm;
  char *dayweek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  if (!no_MD5 && !state->to &&
      (state->MD_challenge = MD_getChallenge(NULL, state)) != NULL)
  {  /* Answering side MUST send CRAM message as a very first M_NUL */
    char s[MD_CHALLENGE_LEN*2+15]; /* max. length of opt string */
    strcpy(s, "OPT ");
    MD_toString(s + 4, state->MD_challenge[0], state->MD_challenge + 1);
    msg_send2 (state, M_NUL, s, "");
  }
  else
    state->MD_flag = 0;

  msg_send2 (state, M_NUL, "SYS ", config->sysname);
  msg_send2 (state, M_NUL, "ZYZ ", config->sysop);
  msg_send2 (state, M_NUL, "LOC ", config->location);
  msg_send2 (state, M_NUL, "NDL ", config->nodeinfo);

  t = safe_time();
  tz = tz_off(t, config->tzoff);
  safe_localtime (&t, &tm);

#if 0
  sprintf (szLocalTime, "%s, %2d %s %d %02d:%02d:%02d %c%02d%02d (%s)",
           dayweek[tm->tm_wday], tm->tm_mday, month[tm->tm_mon],
           tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec,
           (tz>=0) ? '+' : '-', abs(tz)/60, abs(tz)%60,
           tzname[tm->tm_isdst>0 ? 1 : 0]);
#else
  snprintf (szLocalTime, sizeof(szLocalTime),
            "%s, %2d %s %d %02d:%02d:%02d %c%02d%02d",
            dayweek[tm.tm_wday], tm.tm_mday, month[tm.tm_mon],
            tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
            (tz>=0) ? '+' : '-', abs(tz)/60, abs(tz)%60);
#endif

  msg_send2 (state, M_NUL, "TIME ", szLocalTime);

  msg_sendf (state, M_NUL,
    "VER " MYNAME "/" MYVER "%s " PRTCLNAME "/" PRTCLVER, get_os_string ());

#ifdef WITH_PERL
  if (state->to) {
    char *s = perl_on_handshake(state);
    if (s && *s) {
      Log (1, "aborted by Perl on_handshake(): %s", s);
      msg_send2 (state, M_ERR, s, 0);
      return 0;
    }
  }
#endif

  if (state->to || !state->delay_ADR) send_ADR (state, config);

  if (state->to) {
    szOpt = xstrdup(" NDA EXTCMD");
    if (state->NR_flag & WANT_NR) xstrcat(&szOpt, " NR");
    if (state->ND_flag & THEY_ND) xstrcat(&szOpt, " ND");
    if (state->crypt_flag & WE_CRYPT) xstrcat(&szOpt, " CRYPT");
#ifdef WITH_ZLIB
    if (state->z_canrecv & 1) xstrcat(&szOpt, " GZ");
#endif
#ifdef WITH_BZLIB2
    if (state->z_canrecv & 2) xstrcat(&szOpt, " BZ2");
#endif
    msg_send2(state, M_NUL, "OPT", szOpt);
    xfree(szOpt);
  }
  return 1;
}

static int start_file_transfer (STATE *state, FTNQ *file, BINKD_CONFIG *config)
{
  struct stat sb;
  FILE *f = NULL;
  int action = -1, i, dontsend = 0;
  char *extra;

  if (state->out.f)
    fclose (state->out.f);
  TF_ZERO (&state->out);               /* No file in transfer */

  if (state->flo.f == 0)               /* There is no open .?lo */
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
      /* look for the file in not-to-send list */
      for (i = 0; i < state->n_nosendlist; i++)
        if (strcmp(file->path, state->nosendlist[i]) == 0) return 0;

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
        memcpy (&state->flo.fa, &file->fa, sizeof(FTN_ADDR));
        state->flo.f = f;
      }
    }
    memcpy (&state->out.fa, &file->fa, sizeof(FTN_ADDR));
    if ((state->ND_flag & WE_ND) == 0)
      memcpy(&state->ND_addr, &file->fa, sizeof(state->ND_addr));
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
        remove_from_spool (state, state->flo.path, 0, 0, config);
        TF_ZERO (&state->flo);
        return 0;
      }

      if ((w = trans_flo_line (state->out.path, config->rf_rules.first)) != 0)
        Log (5, "%s mapped to %s", state->out.path, w);

      /* look for the file in not-to-send list */
      for (i = 0; i < state->n_nosendlist; i++)
        if (strcmp(w ? w : file->path, state->nosendlist[i]) == 0) {
            xfree (w);
            remove_from_spool (state, state->out.flo,
                               state->out.path, state->out.action, config);
          break;
        }
      if (i < state->n_nosendlist) continue;

      if ((f = fopen (w ? w : state->out.path, "rb")) == 0 ||
          fstat (fileno (f), &sb) == -1 ||
          (sb.st_mode & S_IFDIR) != 0)
      {
        Log (1, "start_file_transfer: %s: %s",
             w ? w : state->out.path, strerror (errno));
        if (f) fclose(f);
        xfree (w);
        remove_from_spool (state, state->out.flo,
                           state->out.path, state->out.action, config);
      }
      else
      {
        xfree (w);
        break;
      }
    }
    memcpy (&state->out.fa, &state->flo.fa, sizeof(FTN_ADDR));
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
  netname (state->out.netname, &state->out, config);
  if ((state->out.type == 'm' || (ispkt(state->out.netname) && config->dontsendempty >= EMPTY_ARCMAIL)) && state->out.size <= 60)
  {
    Log (3, "skip empty pkt %s, %" PRIuMAX " bytes", state->out.path,
         (uintmax_t) state->out.size);
    dontsend = 1;
  }
  else if (config->dontsendempty >= EMPTY_ARCMAIL &&
      state->out.size == 0 && isarcmail(state->out.netname))
  {
    Log (3, "skip empty arcmail %s", state->out.path);
    dontsend = 1;
  }
  else if (config->dontsendempty == EMPTY_ALL && state->out.size == 0)
  {
    Log (3, "skip empty attach %s", state->out.path);
    dontsend = 1;
  }
  if (dontsend)
  {
    if (state->out.f) fclose(state->out.f);
    remove_from_spool (state, state->out.flo,
                       state->out.path, state->out.action, config);
    TF_ZERO (&state->out);
    return 0;
  }
#ifdef WITH_PERL
  if (perl_before_send(state) > 0) {
    Log(3, "sending %s aborted by Perl before_send()", state->out.path);
    if (state->out.f) fclose(state->out.f);
    remove_from_spool (state, state->out.flo,
                       state->out.path, state->out.action, config);
    TF_ZERO (&state->out);
    return 0;
  }
#endif
  Log (2, "sending %s as %s (%" PRIuMAX ")",
       state->out.path, state->out.netname, (uintmax_t) state->out.size);
#ifdef BW_LIM
  setup_rate_limit(state, config, &state->bw_send, state->out.netname);
#endif

  z_send_init(state, config, &extra);

  if (state->NR_flag & WE_NR)
  {
    msg_sendf (state, M_FILE, "%s %" PRIuMAX " %" PRIuMAX " -1%s",
               state->out.netname, (uintmax_t) state->out.size,
               (uintmax_t) state->out.time, extra);
    state->off_req_sent = 1;
  }
  else if (state->out.f == NULL)
    /* status with no NR-mode */
    msg_sendf (state, M_FILE, "%s %" PRIuMAX " %" PRIuMAX " %" PRIuMAX "%s",
               state->out.netname, (uintmax_t) state->out.size,
               (uintmax_t) state->out.time,
               (uintmax_t) state->out.size, extra);
  else
    msg_sendf (state, M_FILE, "%s %" PRIuMAX " %" PRIuMAX " 0%s",
               state->out.netname, (uintmax_t) state->out.size,
               (uintmax_t) state->out.time, extra);

  return 1;
}

static void log_end_of_session (int status, STATE *state, BINKD_CONFIG *config)
{
  char szFTNAddr[FTN_ADDR_SZ + 1];

  BinLogStat (status, state, config);

  if (state->to)
    ftnaddress_to_str (szFTNAddr, &state->to->fa);
  else if (state->fa)
    ftnaddress_to_str (szFTNAddr, state->fa);
  else
    strcpy (szFTNAddr, "?");

  Log (2, "done (%s%s, %s, S/R: %i/%i (%" PRIuMAX "/%" PRIuMAX " bytes))",
       state->to ? "to " : (state->fa ? "from " : ""), szFTNAddr,
       status ? "failed" : "OK",
       state->files_sent, state->files_rcvd,
       state->bytes_sent, state->bytes_rcvd);
}

void protocol (SOCKET socket_in, SOCKET socket_out, FTN_NODE *to, FTN_ADDR *fa, char *current_addr, BINKD_CONFIG *config)
{
  STATE state;
  struct timeval tv;
  fd_set r, w;
  int no, rd;
#ifdef WIN32
  unsigned long t_out = 0;
  unsigned long u_nettimeout = config->nettimeout*1000000l;
#endif
  struct sockaddr_storage peer_name;
  socklen_t peer_name_len = sizeof (peer_name);
  char host[BINKD_FQDNLEN + 1];
  char ipaddr[BINKD_FQDNLEN + 1];
  char ownhost[BINKD_FQDNLEN + 1];
  char service[MAXSERVNAME + 1];
  char ownserv[MAXSERVNAME + 1];
  const char *save_err = NULL;
  int status;
#ifdef BW_LIM
  int limited;
#endif

  if (!init_protocol (&state, socket_in, socket_out, to, fa, config))
    return;

  /* initialize variables */
  memset(&peer_name, 0, sizeof (peer_name));
  host[0] = '\0';
  service[0] = '\0';
  status = -1;

  if (current_addr)
  {
    struct addrinfo hints, *hres;

    strnzcpy(ipaddr, current_addr, BINKD_FQDNLEN);
    /* resolve current_addr to numeric form in peer_name */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICHOST;
    if ((status = getaddrinfo(current_addr, NULL, &hints, &hres)) == 0)
    {
      if (hres)
      {
        memcpy(&peer_name, hres->ai_addr, hres->ai_addrlen);
        freeaddrinfo(hres);
      }
      else
      {
        status = -1;
        if (!to)
          Log (1, "%s, getaddrinfo error (empty result)", current_addr);
      }
    }
    else
    {
      if (!to)
        Log (1, "%s, getaddrinfo error: %s (%d)", current_addr, gai_strerror(status), status);
    }
  }
  else if (!state.pipe)
  {
    if ((status = getpeername (socket_in, (struct sockaddr *)&peer_name, &peer_name_len)) != 0)
    {
      if (!binkd_exit)
        Log (1, "getpeername: %s", TCPERR());
    }
    /* verify that output of getpeername() is safe (enough) and resolve
     * IP and hostname if so requested and possible.
     */
    if (status != 0 || ((struct sockaddr *)&peer_name)->sa_family == 0 || 
         peer_name_len > sizeof(peer_name))
    {
      strnzcpy(ipaddr, "unknown", BINKD_FQDNLEN);
      status = -1;
    }
    else
    {
      if ((status = getnameinfo((struct sockaddr *)&peer_name, peer_name_len,
		ipaddr, sizeof(ipaddr), service, sizeof(service),
		NI_NUMERICSERV | NI_NUMERICHOST)) != 0)
      {
        Log(1, "Error in numeric getnameinfo(): %s (%d)", 
	      gai_strerror(status), status);
        strnzcpy(ipaddr, "unknown", BINKD_FQDNLEN);
      }
    }
  }
  else
    strnzcpy(ipaddr, "unknown", BINKD_FQDNLEN);
  if (status == 0 && config->backresolv)
  {
    status = getnameinfo((struct sockaddr *)&peer_name, peer_name_len, 
		host, sizeof(host), NULL, 0, NI_NAMEREQD);
    if (status != 0 && status != EAI_NONAME)
      Log(2, "Error in getnameinfo(): %s (%d)", 
	  gai_strerror(status), status);
  }

  state.ipaddr = ipaddr;
  state.peer_name = (*host != '\0' ? host : ipaddr);

#ifndef HAVE_THREADS
  setproctitle ("%c [%s]", to ? 'o' : 'i', state.peer_name);
#endif
  if (*host != '\0')
    Log (2, "%s session with %s [%s]",
       to ? "outgoing" : "incoming",
       host, ipaddr);
  else
    Log (2, "%s session with %s",
       to ? "outgoing" : "incoming",
       ipaddr);

  if (state.pipe || getsockname (socket_in, (struct sockaddr *)&peer_name, &peer_name_len) == -1)
  {
    if (!state.pipe && !binkd_exit)
      Log (1, "getsockname: %s", TCPERR ());
    memset(&peer_name, 0, sizeof (peer_name));
  }
  else
  {
    status = getnameinfo((struct sockaddr *)&peer_name, peer_name_len, 
		ownhost, sizeof(ownhost), 
		ownserv, sizeof(ownserv), NI_NUMERICHOST | NI_NUMERICSERV);
    if (status == 0)
    {
      state.our_ip=ownhost;
      state.our_port=atoi(ownserv);
    }
    else
      Log(2, "Error in getnameinfo(): %s (%d)", gai_strerror(status), status);
  }

  if (banner (&state, config) == 0) ;
  else if (n_servers > config->max_servers && !to)
  {
    Log (1, "too many servers");
    msg_send2 (&state, M_BSY, "Too many servers", 0);
  }
  else
  {
    while (1)
    {
      /* If the queue is not empty and there is no file in transfer */
      if (!state.local_EOB && state.q && state.out.f == 0 &&
          !state.waiting_for_GOT && !state.off_req_sent && state.state!=P_NULL)
      {
        FTNQ *q;

        while (1)
        {                               /* Next .pkt, .flo or a file */
          q = 0;
          if (state.flo.f ||
              (q = select_next_file (state.q, state.fa, state.nfa)) != 0)
          {
            if (start_file_transfer (&state, q, config))
              break;
          }
          else
          {
            q_free (state.q, config);
            state.q = 0;
            break;
          }
        }
      }

      /* No more files to send in this batch, so send EOB */
      if (!state.out.f && !state.q && !state.local_EOB && state.state != P_NULL && state.sent_fls == 0)
      {
        /* val: don't send EOB for binkp/1.0 if delay_EOB is set */
        if (!state.delay_EOB || (state.major * 100 + state.minor > 100)) {
          state.local_EOB = 1;
          msg_send2 (&state, M_EOB, 0, 0);
        }
      }

      FD_ZERO (&r);
      FD_ZERO (&w);
      tv.tv_sec = config->nettimeout;               /* Set up timeout for select() */
      tv.tv_usec = 0;
#ifdef BW_LIM
      limited = 0;
      if (check_rate_limit(&state.bw_recv, &tv))
        limited = 1;
      else
#endif
        FD_SET (socket_in, &r);
      if (state.msgs ||
          (state.out.f && !state.off_req_sent && !state.waiting_for_GOT) ||
          state.oleft || state.send_eof) {
#ifdef BW_LIM
        if (check_rate_limit(&state.bw_send, &tv))
          limited = 1;
        else
#endif
          FD_SET (socket_out, &w);
      }

      if (state.remote_EOB && state.sent_fls == 0 && state.local_EOB &&
          state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
      {
        /* End of the current batch */
        if (state.rcvdlist)
        {
          state.q = process_rcvdlist (&state, state.q, config);
          q_to_killlist (&state.killlist, &state.n_killlist, state.q);
          free_rcvdlist (&state.rcvdlist, &state.n_rcvdlist);
        }
        Log (6, "there were %i msgs in this batch", state.msgs_in_batch);
        if (state.msgs_in_batch <= 2 || (state.major * 100 + state.minor <= 100))
        { /* Only M_EOBs in last batch (binkp 1.1) or protocol is binkp 1.0 (or lower), close session */
          ND_set_status("", &state.ND_addr, &state, config);
          state.ND_addr.z=-1;
          break;
        }
        else
        {
          /* Start the next batch */
          state.msgs_in_batch = 0;
          state.remote_EOB = state.local_EOB = 0;
          if (OK_SEND_FILES (&state, config))
          {
            state.q = q_scan_boxes (state.q, state.fa, state.nfa, state.to ? 1 : 0, config);
            state.q = q_sort(state.q, state.fa, state.nfa, config);
          }
          continue;
        }
      }

#if defined(WIN32) /* workaround winsock bug */
      if (t_out >= u_nettimeout)
      {
        Log (8, "win timeout detected (nettimeout=%u sec, t_out=%lu sec)", config->nettimeout, t_out/1000000);
        no = 0;
      }
      else
#endif
      {
        Log (8, "tv.tv_sec=%lu, tv.tv_usec=%lu",
           (unsigned long) tv.tv_sec, (unsigned long) tv.tv_usec);
#ifdef WIN32
        if (state.pipe)
        {
          no = 2;
          if (!FD_ISSET (socket_out, &w))
            no--;
          if (!FD_ISSET (socket_in, &r))
          {
            no--;
            if (no == 0)
              no = SELECT (1, &r, &w, 0, &tv); /* just wait */
          }
          else
          {
            unsigned long avail = 0;
            if (!PeekNamedPipe((HANDLE)_get_osfhandle(socket_in), NULL, 0, NULL, &avail, NULL))
            {
              if (!binkd_exit)
                Log (1, "PeekNamedPipe error, errcode %lu", GetLastError());
            }
            else if (!avail)
              FD_CLR (socket_in, &r);
            /* if we have no input data, &r unset and no == 1 */
          }
        }
        else
#endif
          no = SELECT ((socket_in > socket_out ? socket_in : socket_out) + 1, &r, &w, 0, &tv);
        if (no < 0)
          save_err = TCPERR ();
        Log (8, "selected %i (r=%i, w=%i)", no, FD_ISSET (socket_in, &r), FD_ISSET (socket_out, &w));
      }
      bsy_touch (config);                       /* touch *.bsy's */
      if (no == 0
#ifdef BW_LIM
          && !limited
#endif
          )
      {
        state.io_error = 1;
        Log (1, "timeout!");
        if (to)
          bad_try (&to->fa, "Timeout!", BAD_IO, config);
        break;
      }
      else if (no < 0)
      {
        state.io_error = 1;
        if (!binkd_exit)
        {
          Log (1, "select: %s (args: %i %i)", save_err, socket_in, tv.tv_sec);
          if (to)
            bad_try (&to->fa, save_err, BAD_IO, config);
        }
        break;
      }
      rd = FD_ISSET (socket_in, &r);
      if (rd)       /* Have something to read */
      {
        if (!recv_block (&state, config))
          break;
      }
      if (FD_ISSET (socket_out, &w))       /* Clear to send */
      {
        no = send_block (&state, config);
        if (!no && no != 2)
          break;
      }
#if defined(WIN32) /* workaround - give up CPU */
      if ((!state.pipe && FD_ISSET(socket_out, &w) && no == 2 && !rd) || /* win9x: write always allowed */
          (state.pipe && !rd && !FD_ISSET(socket_out, &w)))             /* pipe: cannot wait for read */
      {
        tv.tv_sec = 0;
        tv.tv_usec = w9x_workaround_sleep; /* see iphdr.h */
        FD_ZERO (&r);
#ifdef BW_LIM
        limited = 0;
#endif
        if (!state.pipe)
        {
#ifdef BW_LIM
          if (check_rate_limit(&state.bw_recv, &tv))
            limited = 1;
          else
#endif
            FD_SET (socket_in, &r);
        }
        Log (9, "select for giveup cpu, r=%i, w=0, tv_sec=%lu, tv_usec=%lu", FD_ISSET(socket_in, &r), (unsigned long) tv.tv_sec, (unsigned long) tv.tv_usec);
        if (!SELECT (socket_in + 1, &r, 0, 0, &tv)
#ifdef BW_LIM
            && !limited
#endif
            )
          t_out += w9x_workaround_sleep;
        else
          t_out = 0;
      }
      else
        t_out = 0;
#endif
    }
  }

  /* Flush input queue */
  while (!state.io_error)
  {
    if (state.pipe)
      no = read (socket_in, state.ibuf, BLK_HDR_SIZE + MAX_BLKSIZE);
    else
      no = recv (socket_in, state.ibuf, BLK_HDR_SIZE + MAX_BLKSIZE, 0);
    if (no == 0)
      break;
    if (no < 0)
    {
      if ((state.pipe == 0 && TCPERRNO != TCPERR_WOULDBLOCK && TCPERRNO != TCPERR_AGAIN) ||
          (state.pipe == 1 && errno != EWOULDBLOCK && errno != EAGAIN))
        state.io_error = 1;
      break;
    }
    else
      Log (9, "Purged %d bytes from input queue", no);
  }

  /* Still have something to send */
  while (!state.io_error &&
        (state.msgs || (state.optr && state.oleft)) && send_block (&state, config));

  if (state.local_EOB && state.remote_EOB && state.sent_fls == 0 &&
      state.GET_FILE_balance == 0 && state.in.f == 0 && state.out.f == 0)
  {
    /* Successful session */
    status = 0;
    log_end_of_session (status, &state, config);
    process_killlist (state.killlist, state.n_killlist, 's');
    inb_remove_partial (&state, config);
    if (to)
      good_try (&to->fa, "CONNECT/BND", config);
  }
  else
  {
    /* Unsuccessful session */
    status = 1;
    log_end_of_session (status, &state, config);
    process_killlist (state.killlist, state.n_killlist, 0);
    if (to)
    {
      /* We called and there were still files in transfer -- restore poll */
      if (tolower (state.maxflvr) != 'h')
      {
        Log (4, "restoring poll with `%c' flavour", state.maxflvr);
        create_poll (&state.to->fa, state.maxflvr, config);
      }
    }
  }

  if (to && state.r_skipped_flag && config->hold_skipped > 0)
  {
    Log (2, "holding skipped mail for %lu sec",
         (unsigned long) config->hold_skipped);
    hold_node (&to->fa, safe_time() + config->hold_skipped, config);
  }

  deinit_protocol (&state, config, status);
  evt_set (state.evt_queue);
  state.evt_queue = NULL;
  Log (4, "session closed, quitting...");
}
