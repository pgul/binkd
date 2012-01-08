/*
 *  confopt.h -- string version of binkd options
 *
 *  confopt.h is a part of binkd project
 *
 *  Copyright (C) 2003 Alexander Reznikov, homebrewer@yandex.ru (Fido 2:4600/220)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.9  2012/01/08 16:23:52  green
 * Fixed compilation in Cygwin/MinGW
 *
 * Revision 2.8  2012/01/07 17:00:21  green
 * Added detection for PCC compiler
 *
 * Revision 2.7  2012/01/07 13:24:44  green
 * Fixed small typo
 *
 * Revision 2.6  2012/01/07 13:16:18  green
 * Some more information in binkd -vv output
 *
 * Revision 2.5  2008/01/15 11:19:01  gul
 * Show bwlim setting on "binkd -vv" output
 *
 * Revision 2.4  2004/02/07 14:06:06  hbrew
 * Macros: RTLDLL-->RTLSTATIC, BINKDW9X-->BINKD9X
 *
 * Revision 2.3  2003/11/17 01:03:27  hbrew
 * Fix BINKDW9X macro
 *
 * Revision 2.2  2003/11/04 00:47:52  hbrew
 * Cosmetic
 *
 * Revision 2.1  2003/11/04 00:46:20  hbrew
 * confopt added.
 *
 */

/* Optional and compilation options */
/* Compiler: */
#if defined(__MINGW32__)
#  define _DBNKD_COMPILER "mingw32"
#elif defined(_MSC_VER)
#  define _DBNKD_COMPILER "msvc"
#elif defined(__WATCOMC__)
#  define _DBNKD_COMPILER "watcom"
#elif defined(IBMC)
#  define _DBNKD_COMPILER "ibmc"
#elif defined(__EMX__)
#  define _DBNKD_COMPILER "gcc (emx)"
#elif defined(__MSC__)
#  define _DBNKD_COMPILER "msc"
#elif defined(__PCC__)
#  define _DBNKD_COMPILER "pcc"
#elif defined(__clang__)
#  define _DBNKD_COMPILER "clang"
#elif defined(__GNUC__)
#  define _DBNKD_COMPILER "gcc"
#else
#  define _DBNKD_COMPILER "unknown compiler"
#endif

/* binkd9x: */
#if defined(WIN32) && defined(BINKD9X)
#  define _DBNKD_BINKD9X ", binkd9x"
#else
#  define _DBNKD_BINKD9X
#endif

/* rtlstatic: */
#if defined(WIN32) && defined(RTLSTATIC)
#  define _DBNKD_RTLSTATIC ", static"
#else
#  define _DBNKD_RTLSTATIC
#endif

/* debug: */
#if defined(DEBUG)
#  define _DBNKD_DEBUG ", debug"
#else
#  define _DBNKD_DEBUG
#endif
#if defined(DEBUGCHILD)
#  define _DBNKD_DEBUGCHILD ", debugchild (no fork)"
#else
#  define _DBNKD_DEBUGCHILD
#endif

/* zlib, bzlib2: */

#ifdef WITH_ZLIB
#  ifdef ZLIBDL
#    define _DBNKD_ZLIB ", zlibdl"
#  else
#    define _DBNKD_ZLIB ", zlib"
#  endif
#else
#  define _DBNKD_ZLIB
#endif
#ifdef WITH_BZLIB2
#  ifdef ZLIBDL
#    define _DBNKD_BZLIB2 ", bzlib2dl"
#  else
#    define _DBNKD_BZLIB2 ", bzlib2"
#  endif
#else
#  define _DBNKD_BZLIB2
#endif

/* perl: */
#ifdef WITH_PERL
#  ifdef PERLDL
#    define _DBNKD_PERL ", perldl"
#  else
#    define _DBNKD_PERL ", perl"
#  endif
#else
#  define _DBNKD_PERL
#endif

/* core options: */
#ifdef HTTPS
#  define _DBNKD_HTTPS ", https"
#else
#  define _DBNKD_HTTPS
#endif
#ifdef NTLM
#  define _DBNKD_NTLM ", ntlm"
#else
#  define _DBNKD_NTLM
#endif
#ifdef AMIGADOS_4D_OUTBOUND
#  define _DBNKD_AMIGADOS_4D_OUTBOUND ", amiga_4d_outbound"
#else
#  define _DBNKD_AMIGADOS_4D_OUTBOUND
#endif
#ifdef BW_LIM
#  define _DBNKD_BW_LIM ", bwlim"
#else
#  define _DBNKD_BW_LIM
#endif
#ifdef IPV6
#  define _DBNKD_IPV6 ", ipv6"
#else
#  define _DBNKD_IPV6
#endif
#ifdef WITH_FSP1035
#  define _DBNKD_FSP1035 ", fsp1035"
#else
#  define _DBNKD_FSP1035
#endif


#define _DBNKD _DBNKD_COMPILER _DBNKD_BINKD9X _DBNKD_RTLSTATIC _DBNKD_DEBUG \
               _DBNKD_DEBUGCHILD _DBNKD_ZLIB _DBNKD_BZLIB2 _DBNKD_PERL      \
               _DBNKD_HTTPS _DBNKD_NTLM _DBNKD_AMIGADOS_4D_OUTBOUND         \
               _DBNKD_BW_LIM _DBNKD_IPV6 _DBNKD_FSP1035
