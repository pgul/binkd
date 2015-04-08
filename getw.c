/*
 * getwordx public domain library v.2.0
 * (c) 1995,1996,1997 <maloff@tts.magadan.su>
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>

#include "Config.h"
#include "sys.h"
#include "tools.h"

char *getwordx2 (const char *src, int n, int flags, char *fldsep, char *fldskip)
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
      else if (strchr (fldsep, *src) && !quoted)
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
