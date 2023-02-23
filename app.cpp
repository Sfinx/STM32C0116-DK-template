
#include "app.h"
#include <string.h>

void main() {
 board_init();
 status.start = 1;
 while(1) {
  wdt_reset();
  debug("%sapp...", strtime());
  ms_delay(1000);
 } // while(1)
}

// called from systick interrupt
void _1_sec_tick() { }

// called from systick interrupt
void _100_ms_tick() {
 static char ledv;
 static short t;
 if (status.blink && (++t >= status.blink_delay)) {
   t = 0;
   set_led(ledv);
   ledv = ledv ? 0 : 1;
 }
}

status_t status;
