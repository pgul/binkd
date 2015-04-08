/*
 *  ftnaddr.c -- Handle FTN addresses
 *
 *  ftnaddr.c is a part of binkd project
 *
 *  Copyright (C) 1996,1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <string.h>
#include <ctype.h>

#include "sys.h"
#include "tools.h"
#include "ftndom.h"
#include "ftnaddr.h"
#include "iphdr.h"

typedef struct
{
  char c;
  int n;
  char s[MAX_DOMAIN + 1];
}
TOKEN;

#define T_NULL 0
#define T_SEP  1
#define T_NUM  2
#define T_WORD 3

#define token_type(t)  (*((t).s) ? T_WORD : \
			((t).c ? T_SEP : \
			 ((t).n == -1 ? T_NULL : T_NUM)))

static int gettoken (TOKEN *token, char **s)
{
  memset (token, 0, sizeof (TOKEN));

  if (!**s)
  {
    token->n = -1;
    return 0;
  }
  else if (strchr (":/.@", **s))
    token->c = *((*s)++);
  else if (isdigit (**s))
  {
    while (**s && isdigit (**s))
      token->n = token->n * 10 + *((*s)++) - '0';
  }
  else if (isalpha (**s))
  {
    int i;

    for (i = 0; i < MAX_DOMAIN &&
         (**s == '.' || **s == '-' || **s == '_' || isalnum (**s)); ++i)
      token->s[i] = *((*s)++);
    token->s[i] = 0;
  }
  else
    return -1;
  return 1;
}

#define TOKENS 10

int parse_ftnaddress (char *s, FTN_ADDR *fa, FTN_DOMAIN *pDomains)
{
  TOKEN token[TOKENS];
  int i;

  FA_ZERO (fa);

  if (!s) return 0;

  for (i = 0; i < TOKENS; ++i)
    if (gettoken (token + i, &s) < 0)
      return 0;

  i = 0;

  if (token_type (token[i]) == T_NUM && token[i + 1].c == ':')
  {
    fa->z = token[i].n;
    i += 2;
  }

  if (token_type (token[i]) == T_NUM && token[i + 1].c == '/')
  {
    fa->net = token[i++].n;
  }

  if (token[i].c == '/')
    ++i;

  if (token_type (token[i]) == T_NUM)
  {
    fa->node = token[i++].n;
  }
  else if (token[i].c != '.')
    return 0;

  if (token[i].c == '.')
  {
    if (token_type (token[i + 1]) != T_NUM)
      return 0;
    fa->p = token[i + 1].n;
    i += 2;
  }

  if (token[i].c == '@')
  {
    FTN_DOMAIN *d;

    if (/*token[1].c != ':' ||*/ token_type (token[i + 1]) != T_WORD)
      return 0;

    strcpy (fa->domain, (d = get_domain_info (token[i + 1].s, pDomains)) ?
	    (d->alias4 ? d->alias4->name : d->name) : token[i + 1].s);
    i += 2;
  }

  if (token_type (token[i]) != T_NULL ||
      (fa->node == -1 && fa->p == -1) ||
      (fa->domain[0] && fa->z == -1) ||
      (fa->z != -1 && fa->net == -1) ||
      (fa->net != -1 && fa->node == -1))
    return 0;

  if (fa->p == -1)
    fa->p = 0;

  return 1;
}

/*
 * Not safe! Give it at least FTN_ADDR_SZ buffer.
 */
void xftnaddress_to_str(char *s, FTN_ADDR *fa, int force_point)
{
  int n = 0;
  *s = 0;
  if (fa->z != -1)
    n += sprintf (s + n, "%i:", fa->z);
  if (fa->net != -1)
    n += sprintf (s + n, "%i", fa->net);
  if (fa->node != -1)
    n += sprintf (s + n, "/%i", fa->node);
  if (fa->p != 0 || fa->node == -1 || force_point)
    n += sprintf (s + n, ".%i", fa->p);
  if (fa->domain[0])
    n += sprintf (s + n, "@%s", fa->domain);
}

void exp_ftnaddress (FTN_ADDR *fa, FTN_ADDR *pAddr, int nAddr, FTN_DOMAIN *pDomains)
{
  if (!pAddr)
    Log (0, "you should define your address right after domains");
  if (fa->z == -1) {
    /* val: set default zone for a domain */
    FTN_DOMAIN *d = get_domain_info(fa->domain, pDomains);
    fa->z = (d ? (d->alias4 ? *(d->alias4->z) : *(d->z)) : pAddr[0].z);
  }
  if (fa->net == -1)
    fa->net = pAddr[0].net;
  if (fa->node == -1)
    fa->node = pAddr[0].node;
  if (fa->domain[0] == 0)
    strcpy (fa->domain, get_matched_domain(fa->z, pAddr, nAddr, pDomains));
}

int ftnaddress_cmp (FTN_ADDR *a, FTN_ADDR *b)
{
  int i;

  if (!a && !b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((i = STRICMP (a->domain, b->domain)) != 0)
    return i;
  if (a->z != b->z)
    return a->z - b->z;
  if (a->net != b->net)
    return a->net - b->net;
  if (a->node != b->node)
    return a->node - b->node;
  if (a->p != b->p)
    return a->p - b->p;
  return 0;
}
/*
 *  Compare address array with mask, return 0 if any element matches
 */
int ftnamask_cmpm (char *mask, int cnt, FTN_ADDR *fa) {
  int i;
  char buf[FTN_ADDR_SZ];

  for (i = 0; i < cnt; i++) {
    xftnaddress_to_str(buf, &(fa[i]), 1);
    if ( pmatch_ncase(mask, buf) ) return 0;
  }
  return -1;
}

/*
 *  2:5047/13.1 -> p1.f13.n5047.z2.binkp.net.
 *  S should have space for BINKD_FQDNLEN chars.
 */
void ftnaddress_to_domain (char *s, FTN_ADDR *fa, FTN_DOMAIN *d, char *domain)
{
  if (fa->p == 0)
    sprintf (s, "f%i.n%i.z%i.", fa->node, fa->net, fa->z);
  else
    sprintf (s, "p%i.f%i.n%i.z%i.", fa->p, fa->node, fa->net, fa->z);
  d = get_domain_info(fa->domain, d);
  if (d && d->idomain)
    strnzcat (s, d->idomain, BINKD_FQDNLEN);
  else
    strnzcat (s, domain, BINKD_FQDNLEN);
}


/*
 *  S should have space for MAXPATHLEN chars, sets s to "" if no domain.
 */
void ftnaddress_to_filename_ (char *s, FTN_ADDR *fa, FTN_DOMAIN *pDomains
#ifdef AMIGADOS_4D_OUTBOUND
		, int aso
#endif
		)
{
  FTN_DOMAIN *d;

  if ((d = get_domain_info (fa->domain, pDomains)) == 0)
  {
    *s = 0;
  }
  else
  {
    char ext[] = "\0ext";	       /* ".ext" */
    char pnt[] = "\0pnt/0000xxxx";     /* ".pnt..." */

#ifdef AMIGADOS_4D_OUTBOUND
    if (aso)
      snprintf(s, MAXPATHLEN, "%s%s%s%s%u.%u.%u.%u",
         d->path, PATH_SEPARATOR, d->dir,
         PATH_SEPARATOR, fa->z, fa->net, fa->node, fa->p);
    else
#endif
    {
      if (fa->z != d->z[0])
	sprintf (ext, ".%03x", fa->z);

      if (fa->p != 0)
	sprintf (pnt, ".pnt%s%08x", PATH_SEPARATOR, fa->p);

      snprintf(s, MAXPATHLEN, "%s%s%s%s%s%04x%04x%s",
        d->path, PATH_SEPARATOR, d->dir, ext, PATH_SEPARATOR,
        fa->net, fa->node, pnt);
    }
  }
}
