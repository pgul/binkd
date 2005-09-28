/*
 * $Id$
 *
 * $Log$
 * Revision 2.5  2005/09/28 20:40:45  gul
 * Optional parameter root-domain for domain config option.
 *
 * Revision 2.4  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.3  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/08/18 07:35:08  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.1  2003/06/07 08:46:25  gul
 * New feature added: shared aka
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _ftnaddr_h
#define _ftnaddr_h

#include "btypes.h"

#define FTN_ADDR_SZ (80+MAX_DOMAIN) /* Max length of a stringized fido address */

/*
 * 1 -- parsed ok, 0 -- syntax error
 */
int parse_ftnaddress (char *s, FTN_ADDR *fa, FTN_DOMAIN *pDomains);

/*
 * Not safe! Give it at least FTN_ADDR_SZ buffer.
 */
void xftnaddress_to_str (char *s, FTN_ADDR *fa, int force_point);
#define ftnaddress_to_str(s, fa) xftnaddress_to_str(s, fa, 0)

/*
 * Expands an address using pAddr[0] (pAddr[0] is my main a.k.a.)
 */
void exp_ftnaddress (FTN_ADDR *fa, FTN_ADDR *pAddr, int nAddr, FTN_DOMAIN *pDomains);

/*
 *  Returns 0 if match.
 */
int ftnaddress_cmp (FTN_ADDR *, FTN_ADDR *);

/*
 *  Compare address array with mask, return 0 if any element matches
 */
int ftnamask_cmpm(char *, int, FTN_ADDR *);

/*
 *  Compare string address with mask, return 0 if match
 */
#define ftnamask_cmps(mask, addr) (!pmatch_ncase(mask, addr))

/*
 *  S should have space for MAXPATHLEN chars, sets s to "" if no domain.
 */
#ifdef AMIGADOS_4D_OUTBOUND
void ftnaddress_to_filename_ (char *s, FTN_ADDR *fa, FTN_DOMAIN *pDomains, int aso);
#define ftnaddress_to_filename(s, fa, config) ftnaddress_to_filename_(s, fa, (config)->pDomains.first, (config)->aso)
#else
void ftnaddress_to_filename_ (char *s, FTN_ADDR *fa, FTN_DOMAIN *pDomains);
#define ftnaddress_to_filename(s, fa, config) ftnaddress_to_filename_(s, fa, (config)->pDomains.first)
#endif

/*
 *  2:5047/13.1 -> p1.f13.n5047.z2.fidonet.net.
 *  S should have space for MAXHOSTNAMELEN chars.
 */
void ftnaddress_to_domain (char *s, FTN_ADDR *fa, FTN_DOMAIN *d, char *domain);

#define is4D(fa) ((fa)->z != -1 && (fa)->node != -1 && \
  (fa)->net != -1 && (fa)->p != -1)
#define is5D(fa) (is4D(fa) && (fa)->domain[0])
#define FA_ZERO(fa) (memset((fa)->domain, 0, sizeof((fa)->domain)), \
  (fa)->z = (fa)->net = (fa)->node = (fa)->p = -1)
#define FA_ISNULL(fa) (!((fa)->domain[0]) && (fa)->z == -1 && \
  (fa)->net == -1 && (fa)->node == -1 && (fa)->p == -1)

/*
 * Structures for shared aka
 * Linked list contains shared aka info.
 * Each info is linked list in turn, which
 * contains addresses in main domain and pointer
 * to header of this list
 */
typedef struct _SHARED_CHAIN    SHARED_CHAIN;
typedef struct _FTN_ADDR_CHAIN  FTN_ADDR_CHAIN;

struct _FTN_ADDR_CHAIN {
  FTN_ADDR_CHAIN *next;
  FTN_ADDR        fa;
};

struct _SHARED_CHAIN {
  SHARED_CHAIN                 *next;
  FTN_ADDR                      sha;
  DEFINE_LIST(_FTN_ADDR_CHAIN)  sfa;
};

#endif
