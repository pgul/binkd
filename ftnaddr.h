#ifndef _ftnaddr_h
#define _ftnaddr_h

#define MAX_DOMAIN 32
#define FTN_ADDR_SZ (80+MAX_DOMAIN) /* Max length of a stringized fido address */

typedef struct _FTN_ADDR FTN_ADDR;
struct _FTN_ADDR
{
  char domain[MAX_DOMAIN + 1];
  int z, net, node, p; /* -1==unknown or wildcard */
};

/*
 * 1 -- parsed ok, 0 -- syntax error
 */
int parse_ftnaddress (char *s, FTN_ADDR *fa);

/*
 * Not safe! Give it at least FTN_ADDR_SZ buffer.
 */
void ftnaddress_to_str (char *s, FTN_ADDR *fa);

/*
 * Expands an address using pAddr[0] (pAddr[0] is my main a.k.a.)
 */
void exp_ftnaddress (FTN_ADDR *fa);

/*
 *  Returns 0 if match.
 */
int ftnaddress_cmp (FTN_ADDR *, FTN_ADDR *);

/*
 *  S should have space for MAXPATHLEN chars, sets s to "" if no domain.
 */
void ftnaddress_to_filename (char *s, FTN_ADDR *fa);

/*
 *  2:5047/13.1 -> p1.f13.n5047.z2.fidonet.net.
 *  S should have space for MAXHOSTNAMELEN chars.
 */
void ftnaddress_to_domain (char *s, FTN_ADDR *fa);

#define is4D(fa) ((fa)->z != -1 && (fa)->node != -1 && \
                  (fa)->net != -1 && (fa)->p != -1)
#define is5D(fa) (is4D(fa) && (fa)->domain[0])
#define FA_ZERO(fa) (memset((fa)->domain, 0, sizeof((fa)->domain)), \
		     (fa)->z = (fa)->net = (fa)->node = (fa)->p = -1)
#define FA_ISNULL(fa) (!((fa)->domain[0]) && (fa)->z == -1 && \
		       (fa)->net == -1 && (fa)->node == -1 && (fa)->p == -1)

#endif
