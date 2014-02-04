#ifndef __MASS_MAL_H
#define __MASS_MAL_H

#include "stm32fxxx.h"

#define MAL_OK   0
#define MAL_FAIL 1
#define MAX_LUN  1

u16 MAL_Init (u8 lun);
u16 MAL_GetStatus (u8 lun);
u16 MAL_Read(u8 lun, u32 Memory_Offset, u32 *Readbuff, u16 Transfer_Length);
u16 MAL_Write(u8 lun, u32 Memory_Offset, u32 *Writebuff, u16 Transfer_Length);

#endif /* __MASS_MAL_H */
