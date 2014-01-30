/*
 * limits.h
 *      Author: Mm
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#ifndef _WINDOWS

#include "core_cm3.h"
#include "global.h"

void limits_init(void);
__STATIC_INLINE uint8_t limitX_chk(void) { return !(XPORT->IDR & XPIN); }
__STATIC_INLINE uint8_t limitY_chk(void) { return !(YPORT->IDR & YPIN); }
__STATIC_INLINE uint8_t limitZ_chk(void) { return !(ZPORT->IDR & ZPIN); }
__STATIC_INLINE uint8_t limits_chk(void) { return limitX_chk() || limitY_chk() || limitZ_chk(); }


#endif

#endif /* LIMITS_H_ */
