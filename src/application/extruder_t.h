#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "global.h"

#if (USE_EXTRUDER == 1)

void extrudT_init(void);
int16_t extrudT_getTemperatureReal(void);
int16_t extrudT_getTemperatureWait(void);
void extrudT_setTemperature(int16_t val);
uint8_t extrudT_isReady(void);
void extrudT_FunOff(void);
void extrudT_FunOn(void);

void extrudT_proc(void);
void extrudT_irq(void);

#endif
#endif
