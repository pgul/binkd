/*
 * $Id$
 *
 * $Log$
 * Revision 2.1  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _bsy_h
#define _bsy_h

#include "ftnaddr.h"

typedef unsigned char bsy_t;

#define F_BSY ((bsy_t)'b')
#define F_CSY ((bsy_t)'c')

/*
 */
void bsy_init(void);

/*
 * Test & add a busy-flag. 1 -- ok, 0 -- failed
 */
int bsy_add(FTN_ADDR *fa, bsy_t bt);

/*
 * Test a busy-flag. 1 -- free, 0 -- busy
 */
int bsy_test(FTN_ADDR *fa, bsy_t bt);

/*
 */
void bsy_remove(FTN_ADDR *fa, bsy_t bt);

/*
 * For exitlist...
 */
void bsy_remove_all(void);

/*
 * Touchs all our .bsy's if needed
 */
void bsy_touch (void);
#define BSY_TOUCH_DELAY 60

#endif
