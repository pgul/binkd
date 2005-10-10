/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.5  2005/10/10 16:24:22  stas
 * Change method for generate 8+3 bundle name from ASO bundle name
 *
 * Revision 2.4  2005/10/10 15:44:24  stas
 * Move CRC32's define into crypt.h
 *
 * Revision 2.3  2005/10/10 15:43:12  stas
 * Prevent double include crypt.h
 *
 * Revision 2.2  2001/02/21 06:25:21  gul
 * changed crlf to cr in the source file
 *
 * Revision 2.1  2001/02/20 12:01:50  gul
 * rename encrypt to encrypt_buf to avoid conflict with unistd.h
 *
 * Revision 2.0  2001/02/15 11:02:16  gul
 * Added crypt traffic possibility
 *
 *
 */
#ifndef _BINKD_CRYPT_H_
#define _BINKD_CRYPT_H_

#define CRC32(c, b) (crc_32_tab[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
extern unsigned long crc_32_tab[256];

int  update_keys (unsigned long keys[3], int c);
void init_keys (unsigned long keys[3], const char *passwd);
int  decrypt_byte (unsigned long keys[3]);
void decrypt_buf (char *buf, unsigned int bufsize, unsigned long keys[3]);
void encrypt_buf (char *buf, unsigned int bufsize, unsigned long keys[3]);

#endif
