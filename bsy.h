#ifndef _bsy_h
#define _bsy_h

#include "ftnaddr.h"

typedef unsigned char bsy_t;

#define F_BSY ((bsy_t)'b')
#define F_CSY ((bsy_t)'c')

/*
 */
void bsy_init();

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
void bsy_remove_all();

/*
 * Touchs all our .bsy's if needed
 */
void bsy_touch ();
#define BSY_TOUCH_DELAY 60

#endif
