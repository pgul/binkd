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

#if defined(ZLIBDL) && defined(_MSC_VER)
#pragma optimize("", off)
#endif

#include <stdlib.h>
#include "sys.h"
#include "zlibdl.h"
#include "compress.h"
#include "tools.h"

int compress_init(int type, int lvl, void **data)
{
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      *data = calloc(1, sizeof(bz_stream));
      if (*data == NULL) {
        Log (1, "compress_init: not enough memory (%lu needed)", sizeof(bz_stream));
        return BZ_MEM_ERROR;
      }
      /* if (lvl <= 0) */ lvl = 1; /* default is small (100K) buffer */
      return BZ2_bzCompressInit((bz_stream *)*data, lvl, 0, 0);
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      *data = calloc(1, sizeof(z_stream));
      if (*data == NULL) {
        Log (1, "compress_init: not enough memory (%lu needed)", sizeof(z_stream));
        return Z_MEM_ERROR;
      }
      /* 0 is default compression level */
      /* no compression means send file without GZ flag, not 0 compress level */
      if (lvl <= 0) lvl = Z_DEFAULT_COMPRESSION;
      return deflateInit((z_stream *)*data, lvl);
    }
#endif
    default:
      Log (1, "Unknown compression method: %d; data lost", type);
  }
  return -1;
}

int do_compress(int type, char *dst, int *dst_len, char *src, int *src_len, 
                int finish, void *data) {
  int rc;
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      bz_stream *zstrm = (bz_stream *)data;
      zstrm->next_in = (char *)src;
      zstrm->avail_in = (unsigned int)*src_len;
      zstrm->next_out = (char *)dst;
      zstrm->avail_out = (unsigned int)*dst_len;
      rc = BZ2_bzCompress(zstrm, finish ? BZ_FINISH : 0);
      *src_len -= (int)zstrm->avail_in;
      *dst_len -= (int)zstrm->avail_out;
      if (rc == BZ_RUN_OK || rc == BZ_FLUSH_OK || rc == BZ_FINISH_OK) rc = 0;
      if (rc == BZ_STREAM_END) rc = 1;
      return rc;
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      z_stream *zstrm = (z_stream *)data;
      zstrm->next_in = (Bytef *)src;
      zstrm->avail_in = (uLong)*src_len;
      zstrm->next_out = (Bytef *)dst;
      zstrm->avail_out = (uLong)*dst_len;
      rc = deflate(zstrm, finish ? Z_FINISH : 0);
      *src_len -= (int)zstrm->avail_in;
      *dst_len -= (int)zstrm->avail_out;
      if (rc == Z_STREAM_END) rc = 1;
      return rc;
    }
#endif
    default:
      Log (1, "Unknown compression method: %d; data lost", type);
  }
  return -1;
}

void compress_deinit(int type, void *data)
{
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      int rc = BZ2_bzCompressEnd((bz_stream *)data);
      if (rc < 0) Log (1, "BZ2_bzCompressEnd error: %d", rc);
      break;
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      int rc = deflateEnd((z_stream *)data);
      if (rc < 0) Log (1, "deflateEnd error: %d", rc);
      break;
    }
#endif
    default:
      Log (1, "Unknown compression method: %d", type);
  }
  free(data);
  return;
}
void compress_abort(int type, void *data)
{
  char buf[1024];
  int i, j;

  if (data) { 
    Log (4, "Purge compress buffers");
    do {
      i=sizeof(buf);
      j=0;
    } while (do_compress(type, buf, &i, NULL, &j, 1, data) == 0 && i > 0);
    compress_deinit(type, data);
  }
}

int decompress_init(int type, void **data)
{
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      *data = calloc(1, sizeof(bz_stream));
      if (*data == NULL) {
        Log (1, "decompress_init: not enough memory (%lu needed)", sizeof(bz_stream));
        return BZ_MEM_ERROR;
      }
      return BZ2_bzDecompressInit((bz_stream *)*data, 0, 0);
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      *data = calloc(1, sizeof(z_stream));
      if (*data == NULL) {
        Log (1, "decompress_init: not enough memory (%lu needed)", sizeof(z_stream));
        return Z_MEM_ERROR;
      }
      return inflateInit((z_stream *)*data);
    }
#endif
    default:
      Log (1, "Unknown compression method: %d; data lost", type);
  }
  return -1;
}

int do_decompress(int type, char *dst, int *dst_len, char *src, int *src_len, void *data) {
  int rc;
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      bz_stream *zstrm = (bz_stream *)data;
      zstrm->next_in = (char *)src;
      zstrm->avail_in = (unsigned int)*src_len;
      zstrm->next_out = (char *)dst;
      zstrm->avail_out = (unsigned int)*dst_len;
      rc = BZ2_bzDecompress(zstrm);
      *src_len -= (int)zstrm->avail_in;
      *dst_len -= (int)zstrm->avail_out;
      if (rc == BZ_RUN_OK || rc == BZ_FLUSH_OK) rc = 0;
      if (rc == BZ_STREAM_END) rc = 1;
      return rc;
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      z_stream *zstrm = (z_stream *)data;
      zstrm->next_in = (Bytef *)src;
      zstrm->avail_in = (uLong)*src_len;
      zstrm->next_out = (Bytef *)dst;
      zstrm->avail_out = (uLong)*dst_len;
      rc = inflate(zstrm, 0);
      *src_len -= (int)zstrm->avail_in;
      *dst_len -= (int)zstrm->avail_out;
      if (rc == Z_STREAM_END) rc = 1;
      return rc;
    }
#endif
    default:
      Log (1, "Unknown compression method: %d; data lost", type);
  }
  return 0;
}

int decompress_deinit(int type, void *data)
{
  int rc = -1;
  switch (type) {
#ifdef WITH_BZLIB2
    case 2: {
      rc = BZ2_bzDecompressEnd((bz_stream *)data);
      break;
    }
#endif
#ifdef WITH_ZLIB
    case 1: {
      rc = inflateEnd((z_stream *)data);
      break;
    }
#endif
    default:
      Log (1, "Unknown compression method: %d", type);
      break;
  }
  free(data);
  return rc;
}

int decompress_abort(int type, void *data) {
  char buf[1024];
  int i, j;

  if (data) {
    Log (4, "Purge decompress buffers");
    do {
      i=sizeof(buf);
      j=0;
    } while (do_decompress(type, buf, &i, NULL, &j, data) == 0 && i > 0);
    return decompress_deinit(type, data);
  }
  return 0;
}
