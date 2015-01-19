#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#if   defined( STM32F10X_HD )
	#include "stm32f10x-board.h"
#elif defined( STM32F429_439xx )
	#include "stm32f429i-disco.h"
#elif defined( STM32F40_41xxx )
	#include "stm32f4-disco.h"
#endif

#if (USE_DEBUG_MODE == 1)
	#undef USE_RS232
	#define USE_RS232	1

	#define DBG(...) { rf_printf(__VA_ARGS__); }
#else
	#define DBG(...) { }
#endif

#ifdef LIMIT_X_PORT
	static __INLINE uint8_t limitX_chk(void) { return ((LIMIT_X_PORT->IDR & LIMIT_X_PIN) == LIMIT_X_STATE ? 1 : 0); }
#else
	#define limitX_chk()	0
#endif
#ifdef LIMIT_Y_PORT
	static __INLINE uint8_t limitY_chk(void) { return ((LIMIT_Y_PORT->IDR & LIMIT_Y_PIN) == LIMIT_Y_STATE ? 1 : 0); }
#else
	#define limitY_chk()	0
#endif
#ifdef LIMIT_Z_PORT
	static __INLINE uint8_t limitZ_chk(void) { return ((LIMIT_Z_PORT->IDR & LIMIT_Z_PIN) == LIMIT_Z_STATE ? 1 : 0); }
#else
	#define limitZ_chk()	0
#endif
static __INLINE uint8_t limits_chk(void) { return limitX_chk() || limitY_chk() || limitZ_chk(); }

#define RS232_SPEED		115200

#define SYS_NO_ACTION	0
#define SYS_READ_FLASH	1

#include "screen_io.h"
// #include "rtc.h"
#include "ff.h"
#include "keyboard.h"
#include "gcode.h"
#include "stepmotor.h"
// #include "scan.h"
#include "extruder_t.h"
#include "encoder.h"

void delayMs(uint16_t msec);
char *str_trim(char *str);

void manualMode(void);
void showCriticalStatus(char *msg, int st);
uint16_t calcColor(uint8_t val);

uint32_t Seconds(void);

#endif
