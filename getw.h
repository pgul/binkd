/*
 * getwordx public domain library v.2.0
 * (c) 1995,1996,1997 <maloff@tts.magadan.su>
 */
/*
 * $Id$
 *
 * $Log$
 * Revision 2.1  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 *
 */

/*
 * Supported flags:
 */
#define GWX_SUBST 1		       /* Perform %VAR% substs */
#define GWX_HASH  2		       /* Process `#' comments */
#define GWX_NOESC 4		       /* Treat `\' as a regular character */

/* Example: fldsep == ":", fldskip == " \t" */
#define DEF_FLDSEP  " \t\n\r"
#define DEF_FLDSKIP " \t\n\r"

/*
 * Src is a source string, n is a word number (1...), returned string must
 * be free()'d. Returns 0 if there is no word #n.
 */
char *getwordx2 (char *src, int n, int flags, char *fldsep, char *fldskip);

#define getwordx(src,n,flags) \
		getwordx2(src, n, flags, DEF_FLDSEP, DEF_FLDSKIP)
#define getword(src,n) getwordx(src, n, GWX_SUBST | GWX_HASH)
