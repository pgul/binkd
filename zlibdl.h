/*
 *  zlibdl.h -- zlib dynamic load interface
 *
 *  zlibdl.h is a part of binkd project
 *
 *  Copyright (C) 2003  val khokhlov, FIDONet 2:550/180
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _ZLIBDL_H_
#define _ZLIBDL_H_

#ifdef WIN32
#include <windows.h>
#endif

#ifdef ZLIBDL
#if defined(VISUALCPP)
#define ZEXP WINAPI
#define ZEXT
#elif defined(__MINGW32__)
#define ZEXT WINAPI
#define ZEXP
#elif defined(__WATCOMC__) && defined(OS2)
#define ZEXP cdecl
#define ZEXT
#else
#define ZEXP
#define ZEXT
#endif
#endif

#if defined(__WATCOMC__) && defined(OS2)
/* Popular z.dll compiled by EMX with cdecl calling conv. */
#define ZEXPORT cdecl
#endif

/* ---------------- zlib stuff --------------- */
#ifdef WITH_ZLIB

#ifdef WIN32
#define WINDOWS  1
#ifdef ZLIBDL
#define ZLIB_DLL 1
#endif
#endif

#include "zconf.h"
#include "zlib.h"

#ifdef ZLIBDL

#define deflateInit_	(*dl_deflateInit_)
#define deflate		(*dl_deflate)
#define deflateEnd	(*dl_deflateEnd)
#define inflateInit_	(*dl_inflateInit_)
#define inflate		(*dl_inflate)
#define inflateEnd	(*dl_inflateEnd)

extern int ZEXT (ZEXP *dl_deflateInit_)(z_stream *, int, const char *, int);
extern int ZEXT (ZEXP *dl_deflate)(z_stream *, int);
extern int ZEXT (ZEXP *dl_deflateEnd)(z_stream *);
extern int ZEXT (ZEXP *dl_inflateInit_)(z_stream *, const char *, int);
extern int ZEXT (ZEXP *dl_inflate)(z_stream *, int);
extern int ZEXT (ZEXP *dl_inflateEnd)(z_stream *);

#endif /* ZLIBDL */

#endif /* WITH_ZLIB */

/* ---------------- bzlib2 stuff --------------- */
#ifdef WITH_BZLIB2

#ifdef WIN32
#define _WIN32    1
#define BZLIB_DLL 1
#endif

#include <stdio.h>
#include "bzlib.h"

#ifdef ZLIBDL

#define BZ2_bzCompressInit	(*dl_BZ2_bzCompressInit)
#define BZ2_bzCompress		(*dl_BZ2_bzCompress)
#define BZ2_bzCompressEnd	(*dl_BZ2_bzCompressEnd)
#define BZ2_bzDecompressInit	(*dl_BZ2_bzDecompressInit)
#define BZ2_bzDecompress	(*dl_BZ2_bzDecompress)
#define BZ2_bzDecompressEnd	(*dl_BZ2_bzDecompressEnd)

extern int ZEXT (ZEXP *dl_BZ2_bzCompressInit)(bz_stream*, int, int, int);
extern int ZEXT (ZEXP *dl_BZ2_bzCompress)(bz_stream *, int);
extern int ZEXT (ZEXP *dl_BZ2_bzCompressEnd)(bz_stream *);
extern int ZEXT (ZEXP *dl_BZ2_bzDecompressInit)(bz_stream *, int, int);
extern int ZEXT (ZEXP *dl_BZ2_bzDecompress)(bz_stream *);
extern int ZEXT (ZEXP *dl_BZ2_bzDecompressEnd)(bz_stream *);

#endif

#endif /* WITH_BZLIB */

#endif
