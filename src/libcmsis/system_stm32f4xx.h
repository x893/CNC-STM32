/**
  ******************************************************************************
  * @file    system_stm32f4xx.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-November-2013
  * @brief   CMSIS Cortex-M4 Device System Source File for STM32F4xx devices.       
  ******************************************************************************  
  */ 
#ifndef __SYSTEM_STM32F4XX_H
#define __SYSTEM_STM32F4XX_H

#ifdef __cplusplus
	extern "C" {
#endif 

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F4XX_H */
