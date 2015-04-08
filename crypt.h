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
