#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "global.h"

#if (USE_ENCODER == 1)

#include <stdint.h>

void encoder_int(void);
static __INLINE int32_t encoderZvalue() { return (0x007FFF - (int32_t)TIM_GetCounter(TIM8)) >> 2; }
static __INLINE void encoderZreset() {  TIM8->CNT = 0x7FFF; }

#define ENCODER_CORRECTION_MIN_STEPS	150
#define ENCODER_Z_CNT_PER_360			512
// 0.014 mm
#define ENCODER_Z_MIN_MEASURE			(MM_PER_360 * 1500 / ENCODER_Z_CNT_PER_360)

extern bool isEncoderCorrection;
extern int32_t encoderCorrectionCntUp, encoderCorrectionCntDown, encoderCorrectionDelta, encoderCorrectionMaxDelta;

#endif
#endif
