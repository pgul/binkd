Perl Hooks for binkd

Overview

Perl hooks are subs called in certain cases from binkd. They have access
to internal data structures and can implement custom logic.

Currently, Perl 5+ is required on Unix'es (fork model) and Perl 5.6+ 
on Windows and OS/2 (thread model).

Hooks Calling Scheme

                        starting binkd
                             |
                         on_start()
                             |
           /----------------/ \----------------\
           |                                   |
    (server manager)                    (client manager)
           |                                   |
           |                        there's a node to call
           |                                   |
           |                                on_call()
           |                                   |
    incoming connection              --------------------
           |                         |                  |
    got remote addr        connection established  can't connect
           |                         |                  |
           \----------\ /------------/              on_error()
                       |
                 on_handshake()
                       |
                complete login                     \
                       |                           |
               after_handshake()                   |
                       |                           |
          /-----------/ \------------\             |
    there's a file to send   remote sends a file   |
          |                          |             |
    before_send()               before_recv()      \  if error occurs
          |                          |              \       |
    setup_rlimit()              setup_rlimit()      /   on_error()
          |                          |             /
       sending                   receiving         |
          |                          |             |
     after_sent()               after_recv()       |
          .                          .             |
          .                          .             |
          \-----------\ /------------/             |
                       |                           /
                 after_session()
                                    .
                                    .
                            shutting down binkd
                                    |
                                 on_exit()

               on_log()        - before writing a string to log
               config_loaded() - after reading and loading config
               need_reload()   - need to reload config?


Common Data Structures

%config - hash, has the following keys:
              log, loglevel, conlog, tzoff, sysname, sysop, location, nodeinfo,
              bindaddr, iport, oport, maxservers, maxclients, oblksize,
              inbound, inbound_nonsecure, temp_inbound, minfree, 
              minfree_nonsecure, hold, hold_skipped, backresolv, send_if_pwd,
              filebox, brakebox, root_domain, check_pkthdr, pkthdr_badext
              (if compiled with zlib support:)
              zblksize, zminsize
              and keys defined in binkd config as perl-var

%domain - hash, each element corresponds to a domain (or alias):
              key - domain name
              value - pointer to hash with keys: path, dir, defzone, root_domain

@addr   - array of the node's addresses

@listen - array of listen addresses, each element is a pointer to hash with two keys:
              'addr' and 'port'

@ftans, @overwrite - ftrans'es and overwrite's

@skip   - array of skip rules, each element is a pointer to hash with keys:
              mask, type (see below), size, destr

%share  - hash, each element corresponds to a shared aka record:
              key - shared address
              values - pointer to array of nodes to add the shared aka

%node   - hash, each element corresponds to a node record:
              key - node address
              value - pointer to hash with keys:
                        hosts, pwd, ibox, obox, NR, ND, MD, HC, IP, NP

All these structures are read-only. All addresses are 5d.
Address type (in %skip, $config{check_pkthdr}) is enum, symbolic names for
values are not implemented yet.


Common Functions

These utility functions are available for Perl program:

  1. Log([$level, ]$str) - writes a $str to binkd log with $level (def - 3)
  2. aeq($a1, $a2[, ..., $aN]) - returns 1 if first arg matches any of the rest
                                 e.g.: if (aeq('463/68.1', @he)) {}
  3. arm(\@arr, $a1[, ..., $aN]) - deletes addresses $a1..$aN from @arr
                                   e.g.: arm(\@me, '550/180.1', '550/180.2')

  For specifying an address you can omit zone and domain (both are taken from 
  the main aka in this case) and point (set to 0 if not specified).

  So, if main aka is 2:550/180@fidonet, you can write "550/0" for 
  2:550/0.0@fidonet and "463/68" for 2:463/68.0@fidonet

  $a1 for aeq() and $a1...$aN for arm() now can be also address-masks - you
  can use '*' to match any literal portion of the address, but in this case
  comparison is done as in strings ('2:463/180' won't match '463/*', you must
  write '2:463/*'; '2:463/180.1' won't match '*.1', use '*.1@*')

  Functions to be expected: hm... any suggestions? ;-)


Session Data

  This variables are set for session hooks depending of their availability:

               1 2 3   <- session vars level (see hooks description)

  $call        + + +   1 if outgoing connection, 0 otherwise
  $start       + + +   unixtime of session start
  $host        + + +   peer name
  $ip          + + +   peer ip
  $our_ip      + + +   our ip :)
  $secure        + +   $SECURE, $NONSECURE, $WE_NONSECURE, $REMOTE_NONSECURE
  $sysname       + +   remote system name
  $sysop         + +   remote sysop
  $location      + +   remote location
  $traf_mail   + + +   mail traffic in bytes
  $traf_file   + + +   other traffic in bytes
  %opt           + +   hash with keys: ND, NR, MD, crypt, GZ
  @he          + + +   array of remote aka
  @me          + + +   array of present our akas
  $bytes_rcvd      +   bytes received
  $bytes_sent      +   bytes sent
  $files_rcvd      +   files received
  $files_sent      +   files sent
  @queue         +     current queue (see below)
  $z_send        +     use compression on being sent file if set to 1
  $z_recv        +     use compression on being received file if set to 1

  Queue (@queue) can me modified (including reordering) only in following hooks:
  after_handshake, after_recv, before_send.
 
  @queue is array (elements are in the same order as they will be sent), each
  element is pointer to hash with keys:
    file   - file name with full path (still, not translated by ftrans)
    size   - file size (zero if unknown)
    time   - file mtime (zero if unknown)
    sent   - 1 if file is being sent or is already sent
    flvr   - flavour (upper- or lowercase chars "icdofh")
    action - action to be done upon completion ('d'elete, 't'runcate, '')
    type   - file type ('m'ail, 'l'o, 'r'equst, 's'tatus, 'd'ir, '')
    addr   - aka the file is destined for

  Actually, binkd doesn't always care to provide size and time :-) And the
  only mandatory field you are to define to put file into the queue is file.


Hooks Description

1) on_start()
   - called when the main program starts

2) on_exit()
   - if binkd is called from inetd, on_exit() called when session ends
   - if there's a server manager running called when it shuts down
   - otherwise called if the client manager shuts down
     (conclusion: so, it should only be called once ;-)

3) on_call()
   - called when client manager is about to call a node
   - return 0 to abort the call, non-zero to proceed
   - defined vars: $addr  - node to be called,
                   $hosts - hosts lists for node (can be changed),
                   $traf_mail, $traf_file - mail and other traffic in bytes,
                   $proxy, $socks - proxy or socks used for the call
                                    (can be changed).

4) on_error()
   - called when various errors occur
   - defined vars: $addr - node (when calling or in session with)
                   $error - error message
                   $where - $BAD_CALL, $BAD_MERR, $BAD_MBSY, $BAD_IO, 
                            $BAD_TIMEOUT, $BAD_AKA, $BAD_AUTH

5) on_handshake()
   - for client called upon establishing connection (before any output)
   - for server called after receiving remote addresses (before addr is sent)
   - best for hide_aka and present_aka logic :-)
   - can be used for external authentication
   - defined vars: session level 1 (@he contains address of called node for
                   client, not actual received addresses)
   - return non-empty string to abort session with that reason
     otherwise, if @me is defined present @me as our akas
   - you can set $passwd variable to override configured password for the node

6) after_handshake()
   - called after complete login information transferred
   - defined vars: session level 2
   - return non-empty string to abort session with that reason

7) after_session()
   - called after session ends
   - defined vars: session level 3,
                   $rc   - 0 if session was unsuccessful

8) before_recv()
   - called just before we receive a file (before skipmasks are checked)
   - defined vars: session level 2,
                   $name - name of file (netname), 
                   $size - file size, 
                   $time - file mtime, 
                   $offs - starting offset remote suggests
   - return 0 to accept file, 1 to skip, 2 to skip destructively

9) after_recv()
   - called just before we rename received file to it's final name
   - defined vars: session level 2,
                   $name - netname, $size, $time (see below),
                   $tmpfile - full file name of actual temporary file,
                   $file - full file name the temp file to be renamed to
   - return 0 to cancel any changes to $file and use binkd logic,
            1 to try to rename file to $file or use binkd-renaming scheme
            2 to kill tmpfile (you should do renaming manually!)
   - if you unset $file and return non-zero, tmpfile will be killed too

10) before_send()
    - called just before sending a file (after ftrans are applied)
    - defined vars: session level 2,
                    $file - full file name of the local file,
                    $name - netname,
                    $size, $time (as usual)
    - return 0 to send the file (and you can change $name)
             1 to cancel this file

11) after_sent()
    - called after file have been sent
    - defined vars: session level 2,
                    $file - full file name of the local file,
                    $name - netname,
                    $size, $time (guess what ;-),
                    $start - unixtime when the file sending began,
                    $action - what to do with file ('d'elete, 't'runcate, '')
    - return non-zero if you've changed $action and want this behaviour

12) on_log()
    - called when a message is about to be logged
    - defined vars: $_   - message to be logged
                    $lvl - log level of message
    - return non-zero if you want to update $_ and/or $lvl, otherwise message
      and level are both unchanged

13) setup_rlimit()
    - called before sending or receiving file
    - defined vars: session level 2,
                    $file   - network name of file
                    $rlimit - config rate limit for the file in bps
    - $rlimit can be changed. 0 - speed is unlimited.

14) need_reload()
    - called with rescan_delay interval
    - defined vars: $_        - is any config file changed since last readcfg
                    @conflist - list of config files
    - return 1 if you want to reload config, 2 - do not reload,
      0 or undef - use binkd logic

15) config_loaded()
    - called after successfully loading config

If a hook sub is not present, it won't be called. If an error occurs while
running sub, the sub won't be disabled (so, error can occur again).


