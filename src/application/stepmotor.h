#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_

#include <stdint.h>

void stepm_init(void);
void stepm_EmergeStop(void);
extern uint8_t stepmPause;

void stepm_addMove(uint32_t steps[], uint32_t frq[], uint8_t dir[]);

uint32_t stepm_LinesBufferIsFull(void);
int32_t stepm_getRemainLines(void);

int32_t stepm_getCurGlobalStepsNum(uint8_t id);
void stepm_ZeroGlobalCrd(void);
int32_t stepm_inProc(void);
void step_dump(void);

#endif /* STEPMOTOR_H_ */
