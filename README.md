Binkd is a Fidonet mailer designed to operate via TCP/IP networks.

As a FTN-compatible internet daemon, it makes possible efficient
utilization of TCP/IP protocol suite as a transport layer in
FTN-based (Fido Technology Network) networks.

## Compiling

non-UNIX:

1. Find in mkfls/ a subdirectory for your system/compiler, copy all files 
   to the root of the sources.
2. Run make (nmake, wmake or gmake, name of make's binary is rely with C
   compiler).

UNIXes:

1. Copy all files from mkfls/unix/ to the root of binkd sources:

		cd /usr/src/binkd-1.1
		cp mkfls/unix/* .
2. Run configure:

		sh configure
3. Run make.

## Installation

1. Edit sample binkd.cfg.
2. Run binkd.

## More info

**Echomail areas:**
* RU.BINKD (russian)
* BINKD (international)

**Web site:** http://www.corbina.net/~maloff/binkd/

**FTP:** ftp://happy.kiev.ua/pub/fidosoft/mailer/binkd/

**The mirrors:**
   * ftp://fido.thunderdome.us/pub/mirror/binkd/
   * ftp://cube.sut.ru/pub/mirror/binkd/
   * http://binkd.spb.ru

**Documentation:**

* [English manual for binkd 0.9.2](http://web.archive.org/web/20131010041927/http://www.doe.carleton.ca/~nsoveiko/fido/binkd/man/) © Nick Soveiko (<nsoveiko@doe.carleton.ca>)
* [Russian manual for binkd 0.9.9](http://binkd.grumbler.org/binkd-ug-ru.htm.win.ru) © Stas Degteff (`2:5080/102@fidonet`)
* [FAQ](http://binkd.grumbler.org/binkdfaq.shtml)

**Authors:** Dmitry Maloff <maloff@corbina.net> and others.

**Bug reporting:** <binkd-bugs@happy.kiev.ua>, also RU.BINKD or BINKD echoconferences.

**Binkd developers mailing list:** <binkd-dev@happy.kiev.ua> (send `subscribe binkd-dev` to <majordomo@happy.kiev.ua> for subscribe).
