/*
** @(#)getopt.c   2.2 (smail) 1/26/87

   01 Oct 89   Added function prototype for getopt()              ahd
   22 Dec 91   Use Mitch Mitchell's fixes for - option            ahd
*/

/*
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain.
 *
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */

#ifdef BSD
#include <strings.h>
#else
#include <string.h>
#define  index strchr
#endif
#include <stdio.h>

#define ERR(s,c) if (opterr) fprintf(stderr,"%s%s%c\n", argv[0],s,c) /* ahd */

/*--------------------------------------------------------------------*/
/*                    UUPC/extended include files                     */
/*--------------------------------------------------------------------*/

#include "getopt.h"

int   optind = 1;
int   opterr = 1;
int   optopt = 0;
char  *optarg = NULL;
static int sp = 1;

void init_getopt(void)
{
	optind = 1;
	opterr = 1;
	sp = 1;
	optarg = NULL;
	optopt = 0;
}

int getopt(int argc, char **argv, const char *opts)
{
   register int c;
   register char *cp;

   if (optind < argc && argv[optind][0] == '-' && argv[optind][1] == '\0') {
	   if((cp=index(opts, '-')) != NULL) {
		   optind++;
		   return('-');
	   } else {
		   optind++;
		   return('?');
	   }
   }

   if(sp == 1) {
	  if(optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		 return(EOF);
	  else if(strcmp(argv[optind], "--") == 0) {
		 optind++;
		 return(EOF);
	  }
   }

   optopt = c = argv[optind][sp];

   if(c == ':' || (cp=index(opts, c)) == NULL) {
      ERR(": illegal option -- ", c);
      if(argv[optind][++sp] == '\0') {
         optind++;
         sp = 1;
      }
      return('?');
   }

   if(*++cp == ':') {
      if(argv[optind][sp+1] != '\0')
         optarg = &argv[optind++][sp+1];
      else if(++optind >= argc) {
		 ERR(": option requires an argument -- ", c);
         sp = 1;
         return('?');
      } else
         optarg = argv[optind++];
      sp = 1;
   } else {
      if(argv[optind][++sp] == '\0') {
         sp = 1;
         optind++;
      }
      optarg = NULL;
   }
   return(c);
} /* getopt */
