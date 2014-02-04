#ifndef __memory_H
#define __memory_H

#include "STM32Fxxx.h"

#define TXFR_IDLE     0
#define TXFR_ONGOING  1

void Write_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);
void Read_Memory  (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);

#endif /* __memory_H */
