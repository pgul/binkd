/*

   getopt.h

   Update history:

      01Oct89 Add function prototype for getopt                      ahd
  */

/* avoid conflicts if getopt() exists but HAVE_GETOPT not defined */
#define getopt(argc, argv, opts)	binkd_getopt(argc, argv, opts)
#define init_getopt			binkd_init_getopt
#define optind				binkd_optind
#define optopt				binkd_optopt
#define optarg				binkd_optarg

extern int getopt(int argc, char **argv, const char *opts);
extern void init_getopt(void);

extern int optind;
extern int optopt;
extern int opterr;
extern char *optarg;
