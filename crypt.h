/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.0  2001/02/15 11:02:16  gul
 * Added crypt traffic possibility
 *
 *
 */
int  update_keys (unsigned long keys[3], int c);
void init_keys (unsigned long keys[3], const char *passwd);
int  decrypt_byte (unsigned long keys[3]);
void decrypt (char *buf, unsigned int bufsize, unsigned long keys[3]);
void encrypt (char *buf, unsigned int bufsize, unsigned long keys[3]);
