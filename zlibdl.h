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
/*
 * $Id$
 *
 * $Log $
 *
 */

#ifndef _ZLIBDL_H_
#define _ZLIBDL_H_

/* ---------------- common ------------------- */

int do_compress(int type, char *dst, int *dst_len, char *src, int src_len, int lvl);
int do_decompress(int type, char *dst, int *dst_len, char *src, int src_len);

/* ---------------- zlib stuff --------------- */
#ifdef WITH_ZLIB

#ifndef ZLIBDL
#include "zconf.h"
#include "zlib.h"
#else

#define Z_DEFAULT_COMPRESSION 6
#ifdef WIN32
# define ZLIB_CALLCONV __stdcall
#endif

/* type for compress() and decompress() */
typedef int ZLIB_CALLCONV zlib_compress_func(char *, unsigned long *, const char *, int, int);
typedef int ZLIB_CALLCONV zlib_uncompress_func(char *, unsigned long *, const char *, int);

/* actual compress() and decompress() */
extern zlib_compress_func *dl_compress;
extern zlib_uncompress_func *dl_uncompress;

#define compress2(a1,a2,a3,a4,a5)	(*dl_compress)(a1,a2,a3,a4,a5)
#define uncompress(a1,a2,a3,a4)		(*dl_uncompress)(a1,a2,a3,a4)

/* loading function */
int zlib_init(const char *dll_name);
#endif /* ZLIBDL */

#endif /* WITH_ZLIB */

/* ---------------- bzlib2 stuff --------------- */
#ifdef WITH_BZLIB2

#ifndef ZLIBDL
#include "bzlib.h"
#else

#ifdef WIN32
# define BZLIB2_CALLCONV __stdcall
#endif

/* typedefs */
typedef int BZLIB2_CALLCONV bzlib_compress_func (char*, unsigned int*, char*, unsigned int, int, int, int);
typedef int BZLIB2_CALLCONV bzlib_decompress_func (char*, unsigned int*, char*, unsigned int, int, int);

/* actual BZ2_bzBuffToBuffCompress() and BZ2_bzBuffToBuffDecompress() */
extern bzlib_compress_func *dl_bzCompress;
extern bzlib_decompress_func *dl_bzDecompress;

#define BZ2_bzBuffToBuffCompress(a1,a2,a3,a4,a5,a6,a7)	(*dl_bzCompress)(a1,a2,a3,a4,a5,a6,a7)
#define BZ2_bzBuffToBuffDecompress(a1,a2,a3,a4,a5,a6)	(*dl_bzDecompress)(a1,a2,a3,a4,a5,a6)

/* loading function */
int bzlib2_init(const char *dll_name);
#endif /* ZLIBDL */

#endif /* WITH_BZLIB */

#endif
