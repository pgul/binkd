/*
 *  zlibdl.c -- zlib dynamic load interface
 *
 *  zlibdl.c is a part of binkd project
 *
 *  Copyright (C) 2003  val khokhlov, FIDONet 2:550/180
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
 * Revision 2.12  2003/10/20 18:04:46  gul
 * Previous patch break OS/2 compilation. Fixed.
 *
 * Revision 2.11  2003/10/20 17:57:13  gul
 * Dynamic load bzlib.dll built as C++
 *
 * Revision 2.10  2003/10/19 22:02:38  gul
 * OS/2 ZLIBDL fix
 *
 * Revision 2.9  2003/10/19 12:21:47  gul
 * Stream compression
 *
 * Revision 2.8  2003/10/06 08:25:28  val
 * turn off optimization for zlibdl.c
 *
 * Revision 2.7  2003/10/06 06:30:36  val
 * zlib code fix
 *
 * Revision 2.6  2003/09/25 06:41:43  val
 * fix compilation under win32
 *
 * Revision 2.5  2003/09/24 09:53:16  val
 * fix warnings
 *
 * Revision 2.4  2003/09/24 07:32:17  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 *
 */

#ifdef ZLIBDL

#ifdef WIN32
#include <windows.h>
#ifdef __MSC__
#pragma optimize("", off)
#endif
#endif

#ifdef OS2
#define INCL_DOSMODULEMGR
#include <os2.h>
#endif

#if defined(WIN32)
#define LOADFUNC(name)	if (loaded && (dl_##name = (void *)GetProcAddress(hl, #name)) == NULL) loaded = 0;
#define LOADFUNC2(name, size) \
    if (loaded && \
        (dl_##name = (void *)GetProcAddress(hl, #name)) == NULL && \
        (dl_##name = (void *)GetProcAddress(hl, #name "@" #size)) == NULL) \
	    loaded = 0;
#elif defined(OS2)
#define LOADFUNC(name) if (loaded && (DosQueryProcAddr(hl, 0, #name, (PFN*)(&dl_##name)) != 0 || dl_##name == NULL)) loaded = 0;
#define LOADFUNC2(name, size) if (loaded && (DosQueryProcAddr(hl, 0, #name, (PFN*)(&dl_##name)) != 0 || dl_##name == NULL)) loaded = 0;
#endif

#ifdef WITH_ZLIB
int zlib_loaded;

int (*dl_deflateInit_)();
int (*dl_deflate)();
int (*dl_deflateEnd)();
int (*dl_inflateInit_)();
int (*dl_inflate)();
int (*dl_inflateEnd)();

/* loading function */
int zlib_init(const char *dll_name) {
#if defined(WIN32)
  HINSTANCE hl = LoadLibrary(dll_name);
  if (hl)
#elif defined(OS2)
  char buf[256];
  HMODULE hl;
  if (DosLoadModule(buf, sizeof(buf), dll_name, &hl) == 0)
#endif
  { int loaded = 1;
    LOADFUNC(deflateInit_);
    LOADFUNC(deflate);
    LOADFUNC(deflateEnd);
    LOADFUNC(inflateInit_);
    LOADFUNC(inflate);
    LOADFUNC(inflateEnd);
    if (loaded) zlib_loaded = 1;
  }
  return zlib_loaded;
}
#endif

#ifdef WITH_BZLIB2
int bzlib2_loaded;

int (*dl_BZ2_bzCompressInit)();
int (*dl_BZ2_bzCompress)();
int (*dl_BZ2_bzCompressEnd)();
int (*dl_BZ2_bzDecompressInit)();
int (*dl_BZ2_bzDecompress)();
int (*dl_BZ2_bzDecompressEnd)();

/* loading function */
int bzlib2_init(const char *dll_name) {
#if defined(WIN32)
  HINSTANCE hl = LoadLibrary(dll_name);
  if (hl)
#elif defined(OS2)
  char buf[256];
  HMODULE hl;
  if (DosLoadModule(buf, sizeof(buf), dll_name, &hl))
#endif
  { int loaded = 1;
    LOADFUNC2(BZ2_bzCompressInit, 16);
    LOADFUNC2(BZ2_bzCompress, 8);
    LOADFUNC2(BZ2_bzCompressEnd, 4);
    LOADFUNC2(BZ2_bzDecompressInit, 12);
    LOADFUNC2(BZ2_bzDecompress, 4);
    LOADFUNC2(BZ2_bzDecompressEnd, 4);
    if (loaded) bzlib2_loaded = 1;
  }
  return bzlib2_loaded;
}
#endif

#endif

