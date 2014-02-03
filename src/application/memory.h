/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : memory.h
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Memory management layer
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef __memory_H
#define __memory_H

#include "stm32f10x.h"

#define TXFR_IDLE     0
#define TXFR_ONGOING  1

void Write_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);
void Read_Memory  (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length);

#endif /* __memory_H */
