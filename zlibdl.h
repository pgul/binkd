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
 */

#ifndef _ZLIBDL_H_
#define _ZLIBDL_H_

#ifdef WIN32
# define ZLIB_CALLCONV __stdcall
#endif

/* type for compress() and decompress() */
typedef int ZLIB_CALLCONV zlib_compress_func(char *, unsigned long *, const char *, int);

/* actual compress() and decompress() */
extern zlib_compress_func *dl_compress, *dl_uncompress;

#define compress(a1,a2,a3,a4)   (*dl_compress)(a1,a2,a3,a4)
#define uncompress(a1,a2,a3,a4) (*dl_uncompress)(a1,a2,a3,a4)

/* loading function */
int zlib_init(const char *dll_name);

#endif
