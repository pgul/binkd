/*
 * getwordx public domain library v.2.0
 * (c) 1995,1996,1997 <maloff@tts.magadan.su>
 */
/*
 * $Id$
 *
 * $Log$
 * Revision 2.5  2003/05/27 18:58:42  gul
 * Minor fix in parsing quoted words
 *
 * Revision 2.4  2003/04/07 18:24:54  gul
 * Move functions declaration to header-file
 *
 * Revision 2.3  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.2  2003/03/01 18:06:45  gul
 * Add include <sys/types.h>
 *
 * Revision 2.1  2003/01/29 19:32:03  gul
 * Code cleanup, prevent segfault on bad config
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 *
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>

#include "tools.h"

char *getwordx2 (char *src, int n, int flags, char *fldsep, char *fldskip)
{
  char *dest;
  char quoted = 0;
  int i;

  if (!src) return NULL;
  dest = xstrdup(src);
  while (1)
  {
    while (*src && strchr (fldskip, *src))
      ++src;

    if (!*src || ((flags & GWX_HASH) && *src == '#'))
    {
      free (dest);
      return NULL;
    }

    for (i = 0;; ++i, ++src)
    {
      if ((flags & GWX_NOESC) == 0 && *src == '\\')
      {
	int base = 8, ch = 0;

	++src;
	if (!isdigit (*src) && *src != 'x' && *src != 'X')
	{
	  switch (*src)
	    {
	      case 'a':	       /* bell */
		ch = '\a';
		break;
	      case 'b':	       /* bksp */
		ch = '\b';
		break;
	      case 'f':	       /* ff */
		ch = '\f';
		break;
	      case 'n':	       /* lf */
		ch = '\n';
		break;
	      case 'r':	       /* cr */
		ch = '\r';
		break;
	      case 't':	       /* tab */
		ch = '\t';
		break;
	      case 'E':	       /* ESC */
		ch = 0033;
		break;
	      default:
		ch = *src;
		break;
	    }
	  ++src;
	}
	else
	{
	  if (toupper (*src) == 'X')
	  {
	    ++src;
	    base = 16;
	  }
	  for (; isxdigit (*src); ++src)
	  {
	    if (isdigit (*src))
	      ch = ch * base + *src - '0';
	    else if (base == 16)
	      ch = ch * base + tolower (*src) - 'a' + 0xa;
	    else
	      break;
	  }
	}
	dest[i] = (char) ch;
	--src;
      }
      else if (!*src || *src == '\n' || *src == '\r')
	break;
      else if (*src == '\"')
      {
	quoted = ~quoted;
	i--;
      }
      else if ((strchr (fldsep, *src) || ((flags & GWX_HASH) && *src == '#')) &&
	       !quoted)
	break;
      else if (*src == '%' && (flags & GWX_SUBST))
      {
	++src;
	if (*src == '%')
	{
	  dest[i] = '%';
	}
	else
	{
	  char varname[MAX_ENV_VAR_NAME + 1];
	  char *var;
	  unsigned k;

	  for (k = 0; k < sizeof (varname) &&
	       !isspace (*src) &&
	       *src != '%'; ++k)
	    varname[k] = toupper (src++[0]);
	  varname[k] = 0;

	  if ((var = getenv (varname)) == 0)
	    var = "";

	  dest = xrealloc (dest, strlen (src) + strlen (var) + 1);
	  strcpy (dest + i, var);
	  i += strlen (var) - 1;
	}
      }
      else
	dest[i] = *src;
    }

    if (--n == 0)
    {
      dest[i] = 0;
      return dest;
    }

    if (*src && strchr (fldsep, *src))
      ++src;
  }
}
