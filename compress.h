/*
 *  compress.h -- common compression interface
 *
 *  compress.h is a part of binkd project
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
 * Revision 2.2  2003/10/27 23:22:54  gul
 * Fix OS/2 compilation
 *
 * Revision 2.1  2003/10/19 12:21:46  gul
 * Stream compression
 *
 *
 */

#ifndef _COMPRESS_H_
#define _COMPRESS_H_

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)

int compress_init(int type, int lvl, void **data);
int do_compress(int type, char *dst, int *dst_len, char *src, int *src_len, int finish, void *data);
void compress_deinit(int type, void *data);
int decompress_init(int type, void **data);
int do_decompress(int type, char *dst, int *dst_len, char *src, int *src_len, void *data);
int decompress_deinit(int type, void *data);

#define ZBLKSIZE	1024	/* read/write file buffer size */

#ifdef ZLIBDL

#ifdef WITH_ZLIB
extern int zlib_loaded;

/* loading function */
int zlib_init(char *dll_name);
#endif

#ifdef WITH_BZLIB2
extern int bzlib2_loaded;

/* loading function */
int bzlib2_init(char *dll_name);
#endif

#endif /* ZLIBDL */

#endif /* defined(WITH_ZLIB) || defined(WITH_BZLIB2) */

#endif /* _COMPRESS_H_ */
