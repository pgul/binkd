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

#ifdef ZLIBDL

#ifdef WIN32
#include <windows.h>
#ifdef _MSC_VER
#pragma optimize("", off)
#endif
#endif

#ifdef OS2
#define INCL_DOSMODULEMGR
#include <os2.h>
#endif

#include "compress.h"

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
int zlib_init(char *dll_name) {
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
int bzlib2_init(char *dll_name) {
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

