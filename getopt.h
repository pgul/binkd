/*

   getopt.h

   Update history:

      01Oct89 Add function prototype for getopt                      ahd
  */

extern int getopt(int argc, char **argv, char *opts);
extern void init_getopt(void);

extern int optind;
extern char *optarg;
