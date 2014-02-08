#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

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
#define USE_LCD			1
#define USE_KEYBOARD	0
#define USE_USB			1
#define USE_USB_MSD		0
#define USE_SDCARD		0
#define USE_ENCODER		0

#define STEPS_MOTORS	0

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

#if (USE_LCD == 1)
	#include "stm32f429i-disco-lcd.h"
#endif

#if (USE_USB == 1)
	#define USE_USB_OTG_HS
	#define USE_EMBEDDED_PHY
#endif

#define USER_BUTTON_PIN					GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT			GPIOA
#define USER_BUTTON_GPIO_CLK			RCC_AHB1Periph_GPIOA
#define USER_BUTTON_EXTI_LINE			EXTI_Line0
#define USER_BUTTON_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE		EXTI_PinSource0
#define USER_BUTTON_EXTI_IRQn			EXTI0_IRQn 

#define sEE_I2C						I2C3
#define sEE_I2C_CLK					RCC_APB1Periph_I2C3
#define sEE_I2C_SCL_PIN				GPIO_Pin_8
#define sEE_I2C_SCL_GPIO_PORT		GPIOA
#define sEE_I2C_SCL_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define sEE_I2C_SCL_SOURCE			GPIO_PinSource8
#define sEE_I2C_SCL_AF				GPIO_AF_I2C3
#define sEE_I2C_SDA_PIN				GPIO_Pin_9
#define sEE_I2C_SDA_GPIO_PORT		GPIOC
#define sEE_I2C_SDA_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define sEE_I2C_SDA_SOURCE			GPIO_PinSource9
#define sEE_I2C_SDA_AF				GPIO_AF_I2C3
#define sEE_M24C64_32

#define sEE_I2C_DMA					DMA1   
#define sEE_I2C_DMA_CHANNEL			DMA_Channel_3
#define sEE_I2C_DMA_STREAM_TX		DMA1_Stream4
#define sEE_I2C_DMA_STREAM_RX		DMA1_Stream2   
#define sEE_I2C_DMA_CLK				RCC_AHB1Periph_DMA1
#define sEE_I2C_DR_Address			((uint32_t)0x40005C10)
#define sEE_USE_DMA
   
#define sEE_I2C_DMA_TX_IRQn			DMA1_Stream4_IRQn
#define sEE_I2C_DMA_RX_IRQn			DMA1_Stream2_IRQn
#define sEE_I2C_DMA_TX_IRQHandler	DMA1_Stream4_IRQHandler
#define sEE_I2C_DMA_RX_IRQHandler	DMA1_Stream2_IRQHandler   
#define sEE_I2C_DMA_PREPRIO			0
#define sEE_I2C_DMA_SUBPRIO			0
   
#define sEE_TX_DMA_FLAG_FEIF		DMA_FLAG_FEIF4
#define sEE_TX_DMA_FLAG_DMEIF		DMA_FLAG_DMEIF4
#define sEE_TX_DMA_FLAG_TEIF		DMA_FLAG_TEIF4
#define sEE_TX_DMA_FLAG_HTIF		DMA_FLAG_HTIF4
#define sEE_TX_DMA_FLAG_TCIF		DMA_FLAG_TCIF4
#define sEE_RX_DMA_FLAG_FEIF		DMA_FLAG_FEIF2
#define sEE_RX_DMA_FLAG_DMEIF		DMA_FLAG_DMEIF2
#define sEE_RX_DMA_FLAG_TEIF		DMA_FLAG_TEIF2
#define sEE_RX_DMA_FLAG_HTIF		DMA_FLAG_HTIF2
#define sEE_RX_DMA_FLAG_TCIF		DMA_FLAG_TCIF2
   
#define sEE_DIRECTION_TX			0
#define sEE_DIRECTION_RX			1  

/* I2C clock speed configuration (in Hz), used by I2C EEPROM memory and IO Expander drivers */
#ifndef I2C_SPEED
	#define I2C_SPEED				100000
#endif /* I2C_SPEED */

void sEE_LowLevel_DeInit(void);
void sEE_LowLevel_Init(void);
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F429I_DISCOVERY_H */
