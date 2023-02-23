
#include <app.h>
#include <string.h>

uint8_t set_rdp() {
 FLASH_OBProgramInitTypeDef opts;
 opts.OptionType = OPTIONBYTE_RDP;
 opts.RDPLevel = OB_RDP_LEVEL_1;
 HAL_FLASH_Unlock();
 HAL_FLASH_OB_Unlock();
 HAL_StatusTypeDef res = HAL_FLASHEx_OBProgram(&opts);
 if (res == HAL_OK)
   HAL_FLASH_OB_Launch();
 HAL_FLASH_OB_Lock();
 HAL_FLASH_Lock();
 if (res == HAL_OK)
   return 0;
 return 1;
}

uint8_t rdp_enabled() {
 FLASH_OBProgramInitTypeDef opts;
 HAL_FLASHEx_OBGetConfig(&opts);
 return (opts.RDPLevel != OB_RDP_LEVEL_0);
}

void flash_protect() {
 debug("%sEnabling protection ... %s", strtime());
 if (!set_rdp())
  panic(FLASH_PROTECTION_OOPS);
 u8 flash_protected = rdp_enabled();
 debug("%s", flash_protected ? "done" : "failed");
 if (!flash_protected)
  panic(FLASH_PROTECTION_OOPS);
}

void set_led(u8 on) {
 if (on)
  LL_GPIO_ResetOutputPin(LED3_GPIO_PORT, LED3_PIN);
 else
  LL_GPIO_SetOutputPin(LED3_GPIO_PORT, LED3_PIN);
}

void blink(u16 period) {
 status.blink = 1;
 status.blink_delay = period;
}

#define CYCLES_PER_LOOP         3

void wait_cycles(uint32_t n) {
 uint32_t l = n/CYCLES_PER_LOOP;
 while(l--)
  asm volatile("nop;");
}

void hard_delay(uint32_t d) { wait_cycles(d); }

void panic_delay() {
 // hard_delay(0x1FFFFFF);
 LL_mDelay(2000);
}

void panic(int i) {
 if (status.panic)
   return;
 status.panic = 1;
 debug("%s%sOops [%d] !%s", strtime(), RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
 u8 on = 1;
 while(1) {
   set_led(on);
   panic_delay();
   on = on ? 0 : 1;
 }
}

void clock_init() {
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  // HSI configuration and activation
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1);
  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
  // LSI configuration and activation
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1);
  // Set AHB prescaler
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  // Sysclk activation on the HSI
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI);
  // Set APB1 prescaler
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  // set systick
  HAL_SuspendTick();
  LL_Init1msTick(HSI_VALUE);
  LL_SetSystemCoreClock(HSI_VALUE);
  HAL_ResumeTick();
}

void gpio_init() {
 LED3_GPIO_CLK_ENABLE();
 GPIO_InitTypeDef gpio = { 0 };
 // Configure IO in output push-pull mode to drive external LED
 gpio.Mode  = GPIO_MODE_OUTPUT_PP;
 gpio.Pull  = GPIO_PULLUP;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 gpio.Pin = LED3_PIN;
 HAL_GPIO_Init(LED3_GPIO_PORT, &gpio);
 set_led(0); // turn led off
}

void wdt_init() {
 LL_IWDG_Enable(IWDG);
 LL_IWDG_EnableWriteAccess(IWDG);
 LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_8); // ~1 sec
 LL_IWDG_SetReloadCounter(IWDG, 4078);
 while (LL_IWDG_IsReady(IWDG) != 1);
 LL_IWDG_ReloadCounter(IWDG);
}


void board_init() {
 memset(&status, 0, sizeof(status));
 debug("%sFirmware v%d.%d started", strtime(), APP_FW_VERSION >> 4, APP_FW_VERSION & 0xF);
 HAL_Init();
 debug("%sHAL inited", strtime());
 clock_init();
 debug("%sClock inited", strtime());
 gpio_init();
 debug("%sGPIO inited", strtime());
 blink(12);
#ifdef RELEASE_VERSION
 u8 flash_protected = rdp_enabled();
 debug("%sFlash is %sprotected", strtime(), flash_protected ? "" : "not ");
 if (!flash_protected) {
   blink(0); // indicate flash protect operation
   ms_delay(1000);
   flash_protect();
 }
#endif
 wdt_init();
 debug("%sWDT inited", strtime());
 blink(3); // normal operation
}

const char *strtime() {
 static char b[32];
 uchar days = (status.seconds / 86400);
 uchar hours = (status.seconds / 3600) % 24;
 uchar minutes = (status.seconds / 60) % 60;
 uchar seconds = status.seconds % 60;
 sprintf(b, "[%04d:%02d:%02d:%02d.%03d] ", days, hours, minutes, seconds, status.milliseconds);
 return b;
}

void wdt_reset() { LL_IWDG_ReloadCounter(IWDG); }
void _1_ms_tick() { }
void uart_irq() { }
