/*
 * getwordx public domain library v.2.0
 * (c) 1995,1996,1997 <maloff@tts.magadan.su>
 */

#define MAX_ENV_VAR_NAME 256

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
