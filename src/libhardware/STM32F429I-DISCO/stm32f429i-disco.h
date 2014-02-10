#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif
											  
#include "stm32f4xx.h"

#if !defined (USE_STM32F429I_DISCO)
	#define USE_STM32F429I_DISCO
#endif

#define USE_FLASH		0
#define USE_EXTRUDER	0
#define USE_RS232		1
#define USE_DEBUG_MODE	0
#define USE_STEP_DEBUG	0
#define USE_RTC			0
/*
	0	Disabled
	1	Enabled with ILI9320
	2	Enabled with build-in STM32F429I-DISCOVERY
*/
#define USE_LCD			2
/*
	0	Disabled
	1	Use matrix 4x4
	2	Use button on board
*/
#define USE_KEYBOARD	2
#define USE_USB			1
#define USE_USB_MSD		0
#define USE_SDCARD		2
#define USE_ENCODER		0

#define STEPS_MOTORS	1

#define M0_TIM				TIM2
#define M0_TIM_IRQn			TIM2_IRQn
#define M0_TIM_IRQHandler	TIM2_IRQHandler

#define M0_EN_PORT		GPIOE
#define M0_EN_PIN		GPIO_Pin_2
#define M0_DIR_PORT		GPIOE
#define M0_DIR_PIN		GPIO_Pin_3
#define M0_STEP_PORT	GPIOE
#define M0_STEP_PIN		GPIO_Pin_4

#define PIN_SPEED_LOW()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz
#define PIN_SPEED_MID()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz
#define PIN_SPEED_HIGH()	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz
#define PIN_OUTPUT_PP()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;	\
							GPIO_InitStructure.GPIO_OType = GPIO_OType_PP
#define PIN_INPUT_PD()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;	\
							GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN
#define PIN_INPUT_PU()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;	\
							GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP
#define PIN_INPUT()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN
#define PIN_SET_MODE(port,pin)			\
	GPIO_InitStructure.GPIO_Pin = pin;	\
	GPIO_Init(port, &GPIO_InitStructure)

#define LED3_PIN			GPIO_Pin_13
#define LED3_PORT			GPIOG

#define LED4_PIN			GPIO_Pin_14
#define LED4_PORT			GPIOG

void Led3On(void);
void Led3Off(void);
#define LED_ON()			Led3On()
#define LED_OFF()			Led3Off()

void Led4On(void);
void Led4Off(void);
#define LEDRED_ON()			Led4On()
#define LEDRED_OFF()		Led4Off()


#define BUTTON_PIN					GPIO_Pin_0
#define BUTTON_PORT					GPIOA

#define RS232_USART					USART1
#define RS232_CLOCK(en)			 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, en)
#define RS232_TX_PIN				GPIO_Pin_9
#define RS232_TX_PORT				GPIOA
#define RS232_TX_CLK				RCC_AHB1Periph_GPIOA
#define RS232_TX_SOURCE				GPIO_PinSource9
#define RS232_TX_AF					GPIO_AF_USART1
#define RS232_RX_PIN				GPIO_Pin_10
#define RS232_RX_PORT				GPIOA
#define RS232_RX_CLK				RCC_AHB1Periph_GPIOA
#define RS232_RX_SOURCE				GPIO_PinSource10
#define RS232_RX_AF					GPIO_AF_USART1
#define RS232_IRQn					USART1_IRQn
#define RS232_IRQHandler			USART1_IRQHandler

#define USART_RX_READY()	(USART_GetITStatus(RS232_USART, USART_IT_RXNE) != RESET)
#define USART_SEND(c)		USART_SendData(RS232_USART, c)
#define USART_READ()		USART_ReceiveData(RS232_USART)

#define I2C_SPEED			100000

void delayMs(uint16_t msec);

#if (USE_LCD == 2)
	#include "stm32f429i-disco-lcd.h"
#endif

#if (USE_USB == 1)
	#define USE_USB_OTG_HS
	#define USE_EMBEDDED_PHY
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F429I_DISCOVERY_H */
