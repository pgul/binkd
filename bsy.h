#ifndef _bsy_h
#define _bsy_h

typedef unsigned char bsy_t;

#define F_BSY ((bsy_t)'b')
#define F_CSY ((bsy_t)'c')

/*
 */
void bsy_init(void);

/*
 * Test & add a busy-flag. 1 -- ok, 0 -- failed
 */
int bsy_add(FTN_ADDR *fa, bsy_t bt, BINKD_CONFIG *config);

/*
 * Test a busy-flag. 1 -- free, 0 -- busy
 */
int bsy_test(FTN_ADDR *fa, bsy_t bt, BINKD_CONFIG *config);

/*
 */
void bsy_remove(FTN_ADDR *fa, bsy_t bt, BINKD_CONFIG *config);

/*
 * For exitlist...
 */
void bsy_remove_all(BINKD_CONFIG *config);

/*
 * Touchs all our .bsy's if needed
 */
void bsy_touch (BINKD_CONFIG *config);
#define BSY_TOUCH_DELAY 60

#endif
