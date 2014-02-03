#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#if   defined( STM32F10X_HD )
	#include "stm32f10x.h"
#elif defined( STM32F2XX )
	#include "stm32f2xx.h"
#else
	#error "MCU not define"
#endif

//*** <<< Use Configuration Wizard in Context Menu >>> ***
//	<e> Use LCD ILI9320
//	</e>
#define USE_LCD			1
//	<e> Use encoder
//	</e>
#define USE_ENCODER		1
//	<e> Use flash chip SST25
//	</e>
#define USE_FLASH		0
//	<e> Use extruder
//	</e>
#define USE_EXTRUDER	0

//	<e> Use USART for debug messafes
//	</e>
#define USE_RS232		0
//	<e> Use Debug mode
//	</e>
#define USE_DEBUG_MODE	0

#define USE_STEP_DEBUG	1

// <<< end of configuration section >>>

#if (USE_DEBUG_MODE == 1)
	#undef USE_RS232
	#define USE_RS232	1
#endif

#if (USE_DEBUG_MODE == 1)
	#define DBG(...) { rf_printf(__VA_ARGS__); }
#else
	#define DBG(...) { }
#endif

/*
 * ----- sensors ---------------------------------------
 * PC6, PC7 - encoder	Z encoder mode TIM8 (5 V tolerant!)
 */
#if (USE_ENCODER == 1)
	#define ENCODER_PORT	GPIOC
	#define ENCODER_PINS	(GPIO_Pin_6 | GPIO_Pin_7)
#endif

/*
 * ----- position switch ----------------------------------
 *  X,Y,Z: PA0, PB8, PD3
 */
#define LIMIT_X_PORT	GPIOA
#define LIMIT_X_PIN		GPIO_Pin_0
#define LIMIT_Y_PORT	GPIOD
#define LIMIT_Y_PIN		GPIO_Pin_3
#define LIMIT_Z_PORT	GPIOB
#define LIMIT_Z_PIN		GPIO_Pin_8

static __INLINE uint8_t limitX_chk(void) { return !(LIMIT_X_PORT->IDR & LIMIT_X_PIN); }
static __INLINE uint8_t limitY_chk(void) { return !(LIMIT_Y_PORT->IDR & LIMIT_Y_PIN); }
static __INLINE uint8_t limitZ_chk(void) { return !(LIMIT_Z_PORT->IDR & LIMIT_Z_PIN); }
static __INLINE uint8_t limits_chk(void) { return limitX_chk() || limitY_chk() || limitZ_chk(); }


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

#if (USE_LCD == 1)
	#define LCD_BACKLIGHT_PORT	GPIOD
	#define LCD_BACKLIGHT_PIN	GPIO_Pin_13

	#define LCD_BACKLIGHT_ON()	do { LCD_BACKLIGHT_PORT->BSRR = LCD_BACKLIGHT_PIN; } while(0)
	#define LCD_BACKLIGHT_OFF()	do { LCD_BACKLIGHT_PORT->BRR  = LCD_BACKLIGHT_PIN; } while (0)

	#define Bank1_LCD_D			((uint32_t)0x60020000)	/* disp Data ADDR */
	#define Bank1_LCD_C			((uint32_t)0x60000000)	/* disp Reg  ADDR */

	#define LCD_RST_PORT		GPIOE
	#define LCD_RST_PIN			GPIO_Pin_1
	#define LCD_RESET_LOW()		do { LCD_RST_PORT->BRR  = LCD_RST_PIN; } while (0)
	#define LCD_RESET_HIGH()	do { LCD_RST_PORT->BSRR = LCD_RST_PIN; } while (0)

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

#define LED_PORT			GPIOB
#define LED_PIN				GPIO_Pin_5
#define LED_ON()			do { LED_PORT->BSRR = LED_PIN; } while(0)
#define LED_OFF()			do { LED_PORT->BRR = LED_PIN; } while (0)

#define SD_DATA_PORT		GPIOC
/* PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK */
#define SD_DATA_PINS		(GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12)
#define SD_CMD_PORT			GPIOD
#define SD_CMD_PIN			GPIO_Pin_2

#define USB_DISCONNECT_PORT		GPIOC
#define USB_DISCONNECT_PIN	GPIO_Pin_13
#define USB_CONNECTED()		do { USB_DISCONNECT_PORT->BRR  = USB_DISCONNECT_PIN; } while (0)
#define USB_DISCONNECTED()	do { USB_DISCONNECT_PORT->BSRR = USB_DISCONNECT_PIN; } while (0)

#if (USE_FLASH == 1)
	#define SST25_PINS			(GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)
	#define SST25_PORT			GPIOA
	#define SST25_CS_PIN		GPIO_Pin_4
	#define SST25_CS_PORT		GPIOA
	#define SST25_SPI			SPI1
	#define SST25_SPI_CLK()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
	#define SST25_PORT_CLK()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#endif

#define RS232_USART			USART1
#define	RS232_USART_CLK()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define RS232_USART_IRQ		USART1_IRQn
#define RS232_USART_TX_PORT	GPIOA
#define RS232_USART_TX_PIN	GPIO_Pin_9
#define RS232_USART_RX_PORT	GPIOA
#define RS232_USART_RX_PIN	GPIO_Pin_10

#include "hw_config.h"
#include "scr_io.h"
#include "rtc.h"
#include "ff.h"
#include "sdcard.h"
#include "flash_spi.h"
#include "rs232_interface.h"
#include "keyboard.h"
#include "gcode.h"
#include "stepmotor.h"
// #include "scan.h"
#include "extruder_t.h"
#include "encoder.h"

void delayMs(uint32_t msec);
char *str_trim(char *str);
uint8_t questionYesNo(char *msg, char *param);

void manualMode(void);
void showCriticalStatus(char *msg, int st);
uint16_t calcColor(uint8_t val);

/*
typedef struct {
	uint16_t magicValue;
	double startX;
	double startY;
	double startZ;
	double extruder_k;
} FLASH_VALUES;

extern FLASH_VALUES commonValues;
*/

#endif
