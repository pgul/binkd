#include <stdlib.h>
#include <string.h>
#include "des.h"
#include "md4.h"

    /*
     * turns a 56 bit key into the 64 bit, odd parity key and sets the key.
     * The key schedule ks is also set.
     */
    void setup_des_key(unsigned char key_56[], des_key_schedule ks)
    {
        des_cblock key;

        key[0] = key_56[0];
        key[1] = ((key_56[0] << 7) & 0xFF) | (key_56[1] >> 1);
        key[2] = ((key_56[1] << 6) & 0xFF) | (key_56[2] >> 2);
        key[3] = ((key_56[2] << 5) & 0xFF) | (key_56[3] >> 3);
        key[4] = ((key_56[3] << 4) & 0xFF) | (key_56[4] >> 4);
        key[5] = ((key_56[4] << 3) & 0xFF) | (key_56[5] >> 5);
        key[6] = ((key_56[5] << 2) & 0xFF) | (key_56[6] >> 6);
        key[7] =  (key_56[6] << 1) & 0xFF;

        des_set_odd_parity(&key);
        des_set_key(&key, ks);
    }

    /*
     * takes a 21 byte array and treats it as 3 56-bit DES keys. The
     * 8 byte plaintext is encrypted with each key and the resulting 24
     * bytes are stored in the results array.
     */
    void calc_resp(unsigned char *keys, unsigned char *plaintext, unsigned char *results)
    {
        des_key_schedule ks;

        setup_des_key(keys, ks);
        des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) results, ks, DES_ENCRYPT);

        setup_des_key(keys+7, ks);
        des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) (results+8), ks, DES_ENCRYPT);

        setup_des_key(keys+14, ks);
        des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) (results+16), ks, DES_ENCRYPT);
    }

int encrypt(char *passw, int idx, unsigned char *nonce, unsigned char *lm_resp,
    unsigned char *nt_resp)
{
    unsigned char nt_hpw[21];
    MD4_CTX context;
    char  *nt_pw;
    des_cblock cb;
    unsigned char magic[] = { 0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 };
    unsigned char lm_hpw[21];
    des_key_schedule ks;

    /* setup LanManager password */

    unsigned char  lm_pw[14];
    int   len = strlen(passw);
    if (len > 14)  len = 14;

    for (idx=0; idx<len; idx++)
        lm_pw[idx] = toupper(passw[idx]);
    for (; idx<14; idx++)
        lm_pw[idx] = 0;


    /* create LanManager hashed password */

    setup_des_key(lm_pw, ks);
    des_ecb_encrypt(&magic, &cb, ks, 1);
    memcpy(lm_hpw, cb, 8);

    setup_des_key(lm_pw+7, ks);
    des_ecb_encrypt(&magic, &cb, ks, 1);
    memcpy(lm_hpw + 8, cb, 8);

    memset(lm_hpw+16, 0, 5);


    /* create NT hashed password */

    len = strlen(passw);
    nt_pw = (char*)malloc(2*len);
    for (idx=0; idx<len; idx++)
    {
        nt_pw[2*idx]   = passw[idx];
        nt_pw[2*idx+1] = 0;
    }

    MD4_Init(&context);
    MD4_Update(&context, nt_pw, 2*len);
    MD4_Final(nt_hpw, &context);
    free(nt_pw);

    memset(nt_hpw+16, 0, 5);


    /* create responses */

    calc_resp(lm_hpw, nonce, lm_resp);
    calc_resp(nt_hpw, nonce, nt_resp);
    return 0;
}

typedef struct {
        char    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
        char    type;            // 0x01
        char    zero1[3];
        unsigned short   flags;           // 0xb203
        char    zero2[2];

        short   dom_len1;         // domain string length
        short   dom_len2;         // domain string length
        short   dom_off;          // domain string offset
        char    zero3[2];

        short   host_len1;        // host string length
        short   host_len2;        // host string length
        short   host_off;         // host string offset (always 0x20)
        char    zero4[2];

        char    host_dom[1000];  // host string (ASCII) domain string (ASCII)
} Type1message;

typedef struct {
        char    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
        char    type;            // 0x03
        char    zero1[3];

        short   lm_resp_len;     // LanManager response length (always 0x18)
        short   lm_resp_len1;    // LanManager response length (always 0x18)
        short   lm_resp_off;     // LanManager response offset
        char    zero2[2];

        short   nt_resp_len;     // NT response length (always 0x18)
        short   nt_resp_len1;    // NT response length (always 0x18)
        short   nt_resp_off;     // NT response offset
        char    zero3[2];

        short   dom_len;         // domain string length
        short   dom_len1;        // domain string length
        short   dom_off;         // domain string offset (always 0x40)
        char    zero4[2];

        short   user_len;        // username string length
        short   user_len1;       // username string length
        short   user_off;        // username string offset
        char    zero5[2];

        short   host_len;        // host string length
        short   host_len1;       // host string length
        short   host_off;        // host string offset
        char    zero6[6];

        short   msg_len;         // message length
        char    zero7[2];

        unsigned short   flags;           // 0x8201
        char    zero8[2];

        char    data[1024];
} Type3message;


#ifdef L_ENDIAN
#define mkls(x) (x)
#else
short mkls(short x)
{
  return ((x>>8) & 0xFF)| ((x&0xFF)<<8);
}
#endif

static char b64t[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int enbase64(char *data, int size, char *p)
{	
  int i;
  int c;
  unsigned char *q;
  char *s = p;
  q = (unsigned char*)data;
  i=0;
  for(i = 0; i < size;){
    c=q[i++];
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    p[0]=b64t[(c&0x00fc0000) >> 18];
    p[1]=b64t[(c&0x0003f000) >> 12];
    p[2]=b64t[(c&0x00000fc0) >> 6];
    p[3]=b64t[(c&0x0000003f) >> 0];
    if(i > size)
      p[3]='=';
    if(i > size+1)
      p[2]='=';
    p+=4;
  }
  return p - s;
}

static int debase64(char *s, unsigned char *rc)
{
  int i, j;
  char *sp;
  if ((!s) || (!rc)) return (-1);
  for(i = j = 0; ((sp = strchr(b64t, s[i])) != NULL); i++)
  {
    int k = (sp - b64t);
    switch(i%4)
    {
      case 0: rc[j] = k<<2; break;
      case 1: rc[j++] |= k>>4; rc[j] = (k << 4) & 0xF0; break;
      case 2: rc[j++] |= k>>2; rc[j] = (k << 6) & 0xC0; break;
      case 3: rc[j++] |= k; break;
    }
  }
  return j;
}


 /*
  *  User data: username:password:host:domain
  */
int getNTLM1(char *udata, char *result)
{
  Type1message m;
  int i;
  char *td = strdup(udata);
  char *host;
  char *domain = strrchr(td, '/');
  if (!domain) 
  {
    free(td);
    return 1;
  }
  domain[0] = 0;
  domain++;
  host = strrchr(td, '/');
  if (!host)
  {
    free(td);
    return 2;
  }
  host++;
  memset(&m, 0, sizeof(m));
  strcpy(m.protocol, "NTLMSSP");
  m.type=1;
  m.flags=mkls(0xB203);
  m.dom_len1 = m.dom_len2 = mkls(strlen(domain));
  m.host_len1 = m.host_len2 = mkls(strlen(host));
  m.host_off = mkls(0x20);
  m.dom_off = mkls(0x20 + strlen(host));
  strcpy(m.host_dom, host);
  strcpy(m.host_dom + strlen(host), domain);
  for (i = 0; m.host_dom[i]; i++) m.host_dom[i] = toupper(m.host_dom[i]);
  i+=0x20;
  free(td);
  enbase64((char*)&m, i, result);
  return 0;
}

 /*
  *  User data: username:password:host:domain
  */
int getNTLM2(char *udata, char *req, char *result)
{
  int i, j;
  Type3message m;
  unsigned char nonce[8];
  char *user = strdup(udata);
  char *password;
  char *host;
  char *domain = strrchr(user, '/');

  if (!domain)
  {
    free(user);
    return 1;
  }
  domain[0] = 0;
  domain++;
  host = strrchr(user, '/');
  if (!host)
  {
    free(user);
    return 2;
  }
  host[0] = 0;
  host++;
  password = strrchr(user, '/');
  if (!password)
  {
    free(user);
    return 3;
  }
  password[0] = 0;
  password++;

  j = debase64(req, (unsigned char*)m.data);
  memcpy(nonce, m.data + 24, 8);

  memset(&m, 0, sizeof(m));
  strcpy(m.protocol, "NTLMSSP");
  m.type = 3;
  m.lm_resp_len = m.lm_resp_len1 = mkls(0x18);
  m.nt_resp_len = m.nt_resp_len1 = mkls(0x18);
  m.dom_len = m.dom_len1 = mkls(strlen(domain)*2);
  m.dom_off = mkls(0x40);
  m.user_len = m.user_len1 = mkls(strlen(user)*2);
  m.user_off = mkls(0x40 + strlen(domain)*2);
  m.host_len = m.host_len1 = mkls(strlen(host)*2);
  m.host_off = mkls(0x40 + strlen(domain)*2 + strlen(user)*2);
  m.lm_resp_off = mkls(0x40 + strlen(domain)*2 + strlen(user)*2 + strlen(host)*2);
  m.nt_resp_off = mkls(0x58 + strlen(domain)*2 + strlen(user)*2 + strlen(host)*2);
  m.flags = mkls(0x8201);
  for (i = j = 0; domain[i]; i++, j+=2) m.data[j] = toupper(domain[i]);
  for (i = 0; user[i]; i++, j+=2) m.data[j] = user[i];
  for (i = 0; host[i]; i++, j+=2) m.data[j] = toupper(host[i]);
  encrypt(password, 0, nonce, (unsigned char*)m.data+j, (unsigned char*)(m.data+j+24));
  j += 0x40+48;
  m.msg_len = mkls(j);
  enbase64((char*)&m, j, result);
  return 0;
}
