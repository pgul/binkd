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
 * Revision 2.4  2003/09/24 07:32:17  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 *
 */

#ifdef ZLIBDL

#ifdef WIN32
#include <windows.h>
#endif

#ifdef OS2
#define INCL_DOSMODULEMGR
#endif

#endif /* ZLIBDL */

#include "zlibdl.h"
#include "tools.h"

#ifdef ZLIBDL

#ifdef WITH_ZLIB
zlib_compress_func *dl_compress = NULL;
zlib_uncompress_func *dl_uncompress = NULL;

/* loading function */
int zlib_init(const char *dll_name) {
#if defined(WIN32)
  HINSTANCE hl = LoadLibrary(dll_name);
  if (hl) {
    dl_compress = (void*)GetProcAddress(hl, "compress2");
    dl_uncompress = (void*)GetProcAddress(hl, "uncompress");
  }
#elif defined(OS2)
  char buf[256];
  HMODULE hl;
  if (DosLoadModule(buf, sizeof(buf), dll_name, &hl)) {
    DosQueryProcAddr(hl, 0, "compress2", (PFN*)(&dl_compress));
    DosQueryProcAddr(hl, 0, "uncompress", (PFN*)(&dl_uncompress));
  }
#endif
  return dl_compress && dl_uncompress ? 1 : 0;
}
#endif /* WITH_ZLIB */

#ifdef WITH_BZLIB2
bzlib_compress_func *dl_bzCompress = NULL;
bzlib_decompress_func *dl_bzDecompress = NULL;

/* loading function */
int bzlib2_init(const char *dll_name) {
#if defined(WIN32)
  HINSTANCE hl = LoadLibrary(dll_name);
  if (hl) {
    dl_bzCompress = (void*)GetProcAddress(hl, "BZ2_bzBuffToBuffCompress");
    dl_bzDecompress = (void*)GetProcAddress(hl, "BZ2_bzBuffToBuffDecompress");
  }
#elif defined(OS2)
  char buf[256];
  HMODULE hl;
  if (DosLoadModule(buf, sizeof(buf), dll_name, &hl)) {
    DosQueryProcAddr(hl, 0, "BZ2_bzBuffToBuffCompress", (PFN*)(&dl_bzCompress));
    DosQueryProcAddr(hl, 0, "BZ2_bzBuffToBuffDecompress", (PFN*)(&dl_bzDecompress));
  }
#endif
  return dl_bzCompress && dl_bzDecompress ? 1 : 0;
}
#endif /* WITH_BZLIB */

#endif /* ZLIBDL */

int do_compress(int type, char *dst, int *dst_len, char *src, int src_len, 
                int lvl) {
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: 
      if (lvl == 0) lvl = 6;
      return BZ2_bzBuffToBuffCompress(dst, dst_len, src, src_len, lvl, 0, 0);
#endif
#ifdef WITH_ZLIB
    case 1: 
      if (lvl == 0) lvl = Z_DEFAULT_COMPRESSION;
      return compress2(dst, dst_len, src, src_len, lvl);
#endif
    default:
      Log (1, "Unknown compression method: %d; data lost", type);
  }
  return 0;
}

int do_decompress(int type, char *dst, int *dst_len, char *src, int src_len) {
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: 
      return BZ2_bzBuffToBuffDecompress(dst, dst_len, src, src_len, 0, 0);
#endif
#ifdef WITH_ZLIB
    case 1: 
      return uncompress(dst, dst_len, src, src_len);
#endif
    default:
      Log (1, "Unknown decompression method: %d; data lost", type);
  }
  return 0;
}
