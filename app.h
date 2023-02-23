
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lib.h>

#ifdef __cplusplus
};
#endif

typedef struct status_t {
  volatile u64 seconds; // uptime
  volatile unsigned milliseconds:10;
  volatile unsigned panic:1;
  volatile unsigned blink:1; // 0 - no blink, 1 - blink
  volatile uchar blink_delay;
  uchar reset_type;
  volatile unsigned start:1;
} status_t;

extern status_t status;

#define APP_FW_VERSION		0x10
