/*
 * $Id$
 *
 * Revision history:
 * $Log$
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
int  update_keys (unsigned long keys[3], int c);
void init_keys (unsigned long keys[3], const char *passwd);
int  decrypt_byte (unsigned long keys[3]);
void decrypt_buf (char *buf, unsigned int bufsize, unsigned long keys[3]);
void encrypt_buf (char *buf, unsigned int bufsize, unsigned long keys[3]);
