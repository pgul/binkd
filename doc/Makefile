# $Id$
#
# makefile to build documentation

MAKEHTML    = makeinfo --html --no-split
MAKEHTMLDIR = makeinfo --html
MAKEPDF     = texi2dvi --pdf
MAKEINFO    = makeinfo --no-split
MAKETEXT    = makeinfo --no-headers
MAKEXML     = makeinfo --xml --no-split

default:
	@echo Run make html  to make plain html
	@echo Run make html-dir  to make each chapter in separated html file
	@echo Run make text  to make plain text
	@echo Run make pfd   to make PDF files, requires texi2dvi and pfdtex programs
	@echo Run make info  to make info files

html:
	$(MAKEHTML) howto-ru.texi
	$(MAKEHTML) howto-en.texi

html-dir:
	$(MAKEHTMLDIR) howto-ru.texi -o binkd-howto-ru
	$(MAKEHTMLDIR) howto-en.texi -o binkd-howto-en

pdf:
	$(MAKEPDF) howto-ru.texi
	$(MAKEPDF) howto-en.texi

info:
	$(MAKEINFO) howto-ru.texi
	$(MAKEINFO) howto-en.texi

text:
	$(MAKETEXT) howto-ru.texi > howto-ru.txt
	$(MAKETEXT) howto-en.texi > howto-en.txt

xml:
	$(MAKEXML) howto-ru.texi
	$(MAKEXML) howto-en.texi
