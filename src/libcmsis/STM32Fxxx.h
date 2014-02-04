#ifndef __STM32FXXX_H__
#define __STM32FXXX_H__

#if   defined( STM32F10X_HD )
	#include "stm32f10x.h"

	#define GPIOD_CLOCK			RCC_APB2Periph_GPIOD
	#define GPIOE_CLOCK			RCC_APB2Periph_GPIOE
	#define GPIO_CLK(clk, en)	RCC_APB2PeriphClockCmd(clk, en)

	#define PIN_SPEED_LOW()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz
	#define PIN_SPEED_MID()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz
	#define PIN_SPEED_HIGH()	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz
	#define PIN_OUTPUT_PP()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP
	#define PIN_ALTFUNC_PP()	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP
	#define PIN_INPUT_PD()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD
	#define PIN_INPUT_PU()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU
	#define PIN_INPUT()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING

	#define PIN_SET_MODE(port,pin)	\
		GPIO_InitStructure.GPIO_Pin = pin;	\
		GPIO_Init(port, &GPIO_InitStructure)


#elif defined( STM32F2XX )
	#include "stm32f2xx.h"

	#define GPIOD_CLOCK			RCC_AHB1Periph_GPIOD
	#define GPIOE_CLOCK			RCC_AHB1Periph_GPIOE
	#define GPIO_CLK(clk, en)	RCC_AHB1PeriphResetCmd(clk, en)

	#define PIN_SPEED_LOW()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz
	#define PIN_SPEED_MID()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz
	#define PIN_SPEED_HIGH()	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz
	#define PIN_OUTPUT_PP()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;	\
								GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	\
								GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP
	#define PIN_INPUT_PD()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;	\
								GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN
	#define PIN_INPUT_PU()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;	\
								GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP
	#define PIN_INPUT()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN
	#define PIN_SET_MODE(port,pin)			\
		GPIO_InitStructure.GPIO_Pin = pin;	\
		GPIO_Init(port, &GPIO_InitStructure)

#elif defined( STM32F4XX ) || defined( STM32F429_439xx )
	#include "stm32f4xx.h"
#else
	#error "MCU not define"
#endif

#endif /* __STM32FXXX_H__ */
