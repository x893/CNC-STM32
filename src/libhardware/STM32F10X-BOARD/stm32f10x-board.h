#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include "stm32f10x.h"

#define USE_FLASH		0
#define USE_EXTRUDER	0
#define USE_RS232		0
#define USE_DEBUG_MODE	0
#define USE_STEP_DEBUG	1
#define USE_RTC			1
#define USE_LCD			1
#define USE_KEYBOARD	1
#define USE_USB			1
#define USE_USB_MSD		1
#define USE_SDCARD		1
#define USE_ENCODER		1

/*
 * ----- sensors ---------------------------------------
 * PC6, PC7 - encoder	Z encoder mode TIM8 (5 V tolerant!)
 */
#if (USE_ENCODER == 1)
	#define ENCODER_PORT	GPIOC
	#define ENCODER_PINS	(GPIO_Pin_6 | GPIO_Pin_7)
#endif

#define STEPS_MOTORS	4

//------- stepmotor -------------------------------------
// 74hc14 - inverter on the step motors board. STEP on falling edge
//  _______        ______
//         x      |
//         |______|

#define M0_TIM				TIM2
#define M0_TIM_IRQHandler	TIM2_IRQHandler
#define M0_TIM_INIT()		\
	do {													\
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			\
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		\
		NVIC_Init(&NVIC_InitStructure);							\
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	\
		TIM_TimeBaseInit(M0_TIM, &TIM_TimeBase);				\
		M0_TIM->EGR = TIM_PSCReloadMode_Update;					\
		TIM_ARRPreloadConfig(M0_TIM, ENABLE);					\
		TIM_ITConfig(M0_TIM, TIM_IT_Update, ENABLE);			\
		TIM_Cmd(M0_TIM, ENABLE);								\
	} while (0)
#define M0_EN_PORT		GPIOA
#define M0_EN_PIN		GPIO_Pin_1
#define M0_DIR_PORT		GPIOA
#define M0_DIR_PIN		GPIO_Pin_2
#define M0_STEP_PORT	GPIOA
#define M0_STEP_PIN		GPIO_Pin_3

#define M1_TIM				TIM3
#define M1_TIM_IRQHandler	TIM3_IRQHandler
#define M1_TIM_INIT()		\
	do {													\
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			\
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		\
		NVIC_Init(&NVIC_InitStructure);							\
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	\
		TIM_TimeBaseInit(M1_TIM, &TIM_TimeBase);				\
		M1_TIM->EGR = TIM_PSCReloadMode_Update;					\
		TIM_ARRPreloadConfig(M1_TIM, ENABLE);					\
		TIM_ITConfig(M1_TIM, TIM_IT_Update, ENABLE);			\
		TIM_Cmd(M1_TIM, ENABLE);								\
	} while (0)
#define M1_EN_PORT		GPIOE
#define M1_EN_PIN		GPIO_Pin_0
#define M1_DIR_PORT		GPIOB
#define M1_DIR_PIN		GPIO_Pin_9
#define M1_STEP_PORT	GPIOB
#define M1_STEP_PIN		GPIO_Pin_1

#define M2_TIM				TIM4
#define M2_TIM_IRQHandler	TIM4_IRQHandler
#define M2_TIM_INIT()		\
	do {														\
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;			\
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		\
		NVIC_Init(&NVIC_InitStructure);							\
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	\
		TIM_TimeBaseInit(M2_TIM, &TIM_TimeBase);				\
		M2_TIM->EGR = TIM_PSCReloadMode_Update;					\
		TIM_ARRPreloadConfig(M2_TIM, ENABLE);					\
		TIM_ITConfig(M2_TIM, TIM_IT_Update, ENABLE);			\
		TIM_Cmd(M2_TIM, ENABLE);								\
	} while (0)
#define M2_EN_PORT		GPIOB
#define M2_EN_PIN		GPIO_Pin_11
#define M2_DIR_PORT		GPIOB
#define M2_DIR_PIN		GPIO_Pin_12
#define M2_STEP_PORT	GPIOD
#define M2_STEP_PIN		GPIO_Pin_12

//	M3 not used
#define M3_TIM_INIT()
/*
#define M3_TIM				TIM5
#define M3_TIM_IRQHandler	TIM5_IRQHandler
#define M3_TIM_INIT()		\
	do {													\
	NVIC_InitStructure.NVIC_IRQChannel = TIM5;				\
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		\
	NVIC_Init(&NVIC_InitStructure);							\
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	\
	TIM_TimeBaseInit(TIM5, &TIM_TimeBase);					\
	TIM5->EGR = TIM_PSCReloadMode_Update;					\
	TIM_ARRPreloadConfig(TIM5, ENABLE);						\
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);				\
	TIM_Cmd(TIM5, ENABLE);									\
	} while (0)
	#define M3_EN_PORT		GPIOB
	#define M3_EN_PIN		GPIO_Pin_11
	#define M3_DIR_PORT		GPIOD
	#define M3_DIR_PIN		GPIO_Pin_6
	#define M3_STEP_PORT	GPIOB
	#define M3_STEP_PIN		GPIO_Pin_10
	*/

void LedOn(void);
void LedOff(void);
#define LED_PORT			GPIOB
#define LED_PIN				GPIO_Pin_5
#define LED_ON()			LedOn()
#define LED_OFF()			LedOff()
#define LEDRED_ON()			LedOn()
#define LEDRED_OFF()		LedOff()

#if (USE_SDCARD == 1)
	#include "sdcard.h"
	#define SD_DATA_PORT			GPIOC
	/* PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK */
	#define SD_DATA_PINS			(GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12)
	#define SD_CMD_PORT				GPIOD
	#define SD_CMD_PIN				GPIO_Pin_2
#endif

#if (USE_USB == 1)
	#define USB_DISCONNECT_PORT		GPIOC
	#define USB_DISCONNECT_PIN		GPIO_Pin_13
	#define USB_CONNECTED()			GPIO_ResetBits(USB_DISCONNECT_PORT, USB_DISCONNECT_PIN)
	#define USB_DISCONNECTED()		GPIO_ResetBits(USB_DISCONNECT_PORT, USB_DISCONNECT_PIN)
#endif

#if (USE_FLASH == 1)

	#include "flash_spi.h"

	#define SST25_PINS			(GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)
	#define SST25_PORT			GPIOA
	#define SST25_CS_PIN		GPIO_Pin_4
	#define SST25_CS_PORT		GPIOA
	#define SST25_SPI			SPI1
	#define SST25_SPI_CLK()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
	#define SST25_PORT_CLK()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

#endif

#if (USE_LCD == 1)
	#define LCD_BACKLIGHT_CLK	RCC_APB2Periph_GPIOD
	#define LCD_BACKLIGHT_PORT	GPIOD
	#define LCD_BACKLIGHT_PIN	GPIO_Pin_13

	#define LCD_BACKLIGHT_ON()	GPIO_SetBits(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN)
	#define LCD_BACKLIGHT_OFF()	GPIO_ResetBits(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN)

	#define Bank1_LCD_D			((uint32_t)0x60020000)	/* disp Data ADDR */
	#define Bank1_LCD_C			((uint32_t)0x60000000)	/* disp Reg  ADDR */

	#define LCD_RST_CLK			RCC_APB2Periph_GPIOE
	#define LCD_RST_PORT		GPIOE
	#define LCD_RST_PIN			GPIO_Pin_1
	#define LCD_RESET_LOW()		GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN)
	#define LCD_RESET_HIGH()	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN)

	// PD14-D0, PD15-D1, PD0-D2,   PD1-D3,   PE7-D4,   PE8-D5,  PE9-D6, PE10-D7,
	// PE11-D8, PE12-D9, PE13-D10, PE14-D11, PE15-D12, PD8-D13, PD9-D14, PD10-D15
	#define LCD_DATA_D_PORT		GPIOD
	#define LCD_DATA_D_PINS		(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15)
	#define LCD_DATA_E_PORT		GPIOE
	#define LCD_DATA_E_PINS		(GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
	//	PD11-RS (A16), PD4-nOE, PD5-nWE, PD7-LCD-CS
	#define LCD_CTRL_PORT		GPIOD
	#define LCD_CTRL_PINS		GPIO_Pin_11 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
#endif

#if (USE_KEYBOARD == 1)
	/*----- keyboard ----------------------------------------
	*  col: PE2, PE3, PE4, PE5
	*  row: PE6, PC1, PC2, PC3
	* ------- stepmotor -------------------------------------
	*    EN    DIR   STEP
	* 0: PA1   PA2   PA3
	* 1: PE0   PB9   PB1
	* 2: PB11  PB12  PD12
	* 3: PB11  PD6   PB10
	*/
	//----- keyboard ----------------------------------------
	///   0: 1: 2: 3:
	//0:  1  2  3  A
	//1:  4  5  6  B
	//2:  7  8  9  C
	//3:  *  0  #  D
	#define COL0_PORT			GPIOE
	#define COL0_PIN			GPIO_Pin_2
	#define COL1_PORT			GPIOE
	#define COL1_PIN			GPIO_Pin_3
	#define COL2_PORT			GPIOE
	#define COL2_PIN			GPIO_Pin_4
	#define COL3_PORT			GPIOE
	#define COL3_PIN			GPIO_Pin_5

	#define ROW0_PORT			GPIOE
	#define ROW0_PIN			GPIO_Pin_6
	#define ROW1_PORT			GPIOC
	#define ROW1_PIN			GPIO_Pin_1
	#define ROW2_PORT			GPIOC
	#define ROW2_PIN			GPIO_Pin_2
	#define ROW3_PORT			GPIOC
	#define ROW3_PIN			GPIO_Pin_3
#endif

#if (USE_RS232 == 1)
	#include "rs232_interface.h"
	#define RS232_INIT()		rs232_init()
	#define RS232_USART			USART1
	#define	RS232_USART_CLK()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
	#define RS232_USART_IRQ		USART1_IRQn
	#define RS232_USART_TX_PORT	GPIOA
	#define RS232_USART_TX_PIN	GPIO_Pin_9
	#define RS232_USART_RX_PORT	GPIOA
	#define RS232_USART_RX_PIN	GPIO_Pin_10
#endif

#define LIMIT_X_PORT			GPIOA
#define LIMIT_X_PIN				GPIO_Pin_0
#define LIMIT_X_STATE			0
#define LIMIT_Y_PORT			GPIOD
#define LIMIT_Y_PIN				GPIO_Pin_3
#define LIMIT_Y_STATE			0
#define LIMIT_Z_PORT			GPIOB
#define LIMIT_Z_PIN				GPIO_Pin_8
#define LIMIT_Z_STATE			0

#if (USE_RTC == 1)
	#include "rtc.h"
#endif

#if (USE_LCD == 1)
	#include "ili9320.h"
#endif

#define PIN_SPEED_LOW()			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz
#define PIN_SPEED_MID()			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz
#define PIN_SPEED_HIGH()		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz
#define PIN_OUTPUT_PP()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP
#define PIN_ALTFUNC_PP()		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP
#define PIN_INPUT_PD()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD
#define PIN_INPUT_PU()			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU
#define PIN_INPUT()				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING
#define PIN_SET_MODE(port,pin)	\
	GPIO_InitStructure.GPIO_Pin = pin;	\
	GPIO_Init(port, &GPIO_InitStructure)

#define USART_RX_READY()	(USART_GetITStatus(RS232_USART, USART_IT_RXNE) != RESET)
#define USART_SEND(c)		USART_SendData(RS232_USART, c)
#define USART_READ()		USART_ReceiveData(RS232_USART)

void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Reset_Device(void);

#define BULK_MAX_PACKET_SIZE	64

#endif
