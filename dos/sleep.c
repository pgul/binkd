#include <dos.h>
#include <time.h>
#include "../sys.h"

#ifdef CATCH_TIMER

#define TIME_FREQ 1193180l

static unsigned long counter;
static void (_cdecl _interrupt _far *old_timer)(void);

static void _cdecl _interrupt _far new_timer(void)
{
  if (counter) counter--;
  /* don't know how to make "jmp old_timer" from C :-( */
  old_timer();
}
#endif

void dos_sleep(int sec)
{
  static int dv = -1, win, os2;
  union REGS reg_in, reg_out;
#ifndef CATCH_TIMER
  time_t t, start = time(NULL);
#endif

  if (dv == -1) {
    /* dv installation check */
    reg_in.x.ax = 0x2b01;
    reg_in.x.cx = 0x4445;
    reg_in.x.dx = 0x5351;
    intdos(&reg_in, &reg_out);
    if (reg_out.h.al != 0xff) {
      dv = 1;
      win = os2 = 0;
    } else {
      dv = 0;
      /* windows installation check */
      reg_in.x.ax = 0x1600;
      int86(0x2f, &reg_in, &reg_out);
      if (reg_out.h.al & 0x7f) {
	win = 1;
	os2 = 0;
      } else {
	win = 0;
	/* os/2 installation check */
	reg_in.x.ax = 0x4010;
	int86(0x2f, &reg_in, &reg_out);
	if (reg_out.x.ax == 0x4010)
	  os2 = 0;
	else
	  os2 = 1;
      }
    }
  }

#ifdef CATCH_TIMER
  /* save timer interrupt */
  old_timer = _dos_getvect(8);
  /* count timer */
  if ((unsigned long)sec > 0xfffffffful/TIME_FREQ)
    counter = (TIME_FREQ/0x100)*sec/0x100;
  else
    counter = TIME_FREQ*sec/0x10000ul;
  /* set our timer */
  _dos_setvect(8, new_timer);
#endif
  do {
    /* giveup cpu */
    int86(0x28, &reg_in, &reg_out);
    if (dv) {
      reg_in.x.ax = 0x1000;
      int86(0x15, &reg_in, &reg_out);
    } else if (win || os2) {
      reg_in.x.ax = 0x1680;
      int86(0x2f, &reg_in, &reg_out);
    }
#ifndef CATCH_TIMER
    t = time(NULL);
    if (t < start) start = t; /* time shifted */
  } while (t < start + sec);
#else
  } while (counter > 0);
  /* restore timer */
  _dos_setvect(8, old_timer);
#endif
}
