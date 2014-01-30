#ifndef ENCODER_H_
#define ENCODER_H_

#include "global.h"

#ifdef HAS_ENCODER

#include <stdint.h>

void encoder_int(void);
__STATIC_INLINE int32_t encoderZvalue() { return (0x007FFF - (int32_t)TIM_GetCounter(TIM8)) >> 2; }
__STATIC_INLINE void encoderZreset() {  TIM8->CNT = 0x7FFF; }

#define ENCODER_CORRECTION_MIN_STEPS	150

#define ENCODER_Z_CNT_PER_360	512
#define ENCODER_Z_MIN_MEASURE	(MM_PER_360 * 1500 / ENCODER_Z_CNT_PER_360) // 0.014 mm

extern int8_t isEncoderCorrection;
// TODO for debug.. will be removed
extern int32_t encoderCorrectionCntUp, encoderCorrectionCntDown, encoderCorrectionDelta, encoderCorrectionMaxDelta;

#endif
#endif
