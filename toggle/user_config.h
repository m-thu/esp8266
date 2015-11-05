/* this file gets included in "osapi.h", don't rename or remove it */
#include "os_type.h"
#include "stdint.h"

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

/* suppress compiler warnings for SDK functions without a prototype */
void ets_timer_setfn(os_timer_t *, os_timer_func_t *, void *);
void ets_timer_arm_new(os_timer_t *, uint32_t, bool, uint32_t);
void ets_timer_disarm(os_timer_t *);
void *pvPortMalloc(size_t, const char *, uint32_t);
void ets_delay_us(uint16_t);

#endif
