#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f10x.h"

#define HAS_LCD
#define HAS_ENCODER
// #define HAS_FLASH
// #define HAS_EXTRUDER

/*----- keyboard ----------------------------------------
*  col: PE2, PE3, PE4, PE5
*  row: PE6, PC1, PC2, PC3
* ------- stepmotor -------------------------------------
*    EN    DIR   STEP
* 0: PA1   PA2   PA3
* 1: PE0   PB9   PB1
* 2: PB11  PB12  PD12
* 3: PB11  PD6   PB10
* ----- position switch ----------------------------------
*  X,Y,Z: PA0,PB8,PD3
* ----- sensors ---------------------------------------
* PC6, PC7 - encoder	Z encoder mode TIM8 (5 V tolerant!)
*/
#ifdef HAS_ENCODER
	#define ENCODER_PINS	(GPIO_Pin_6 | GPIO_Pin_7)
	#define ENCODER_PORT	GPIOC
#endif

//----- limit switch ----------------------------------
#define XPORT			GPIOA
#define XPIN			GPIO_Pin_0
#define ZPORT			GPIOB
#define ZPIN			GPIO_Pin_8
#define YPORT			GPIOD
#define YPIN			GPIO_Pin_3

//------- stepmotor -------------------------------------
// 74hc14 - inverter on the step motors board. STEP on falling edge
//  _______        ______
//         x      |
//         |______|
#define M0_EN_PORT		GPIOA
#define M0_EN_PIN		GPIO_Pin_1
#define M0_DIR_PORT		GPIOA
#define M0_DIR_PIN		GPIO_Pin_2
#define M0_STEP_PORT	GPIOA
#define M0_STEP_PIN		GPIO_Pin_3

#define M1_EN_PORT		GPIOE
#define M1_EN_PIN		GPIO_Pin_0
#define M1_DIR_PORT		GPIOB
#define M1_DIR_PIN		GPIO_Pin_9
#define M1_STEP_PORT	GPIOB
#define M1_STEP_PIN		GPIO_Pin_1

#define M2_EN_PORT		GPIOB
#define M2_EN_PIN		GPIO_Pin_11
#define M2_DIR_PORT		GPIOB
#define M2_DIR_PIN		GPIO_Pin_12
#define M2_STEP_PORT	GPIOD
#define M2_STEP_PIN		GPIO_Pin_12

#define M3_EN_PORT		GPIOB
#define M3_EN_PIN		GPIO_Pin_11
#define M3_DIR_PORT		GPIOD
#define M3_DIR_PIN		GPIO_Pin_6
#define M3_STEP_PORT	GPIOB
#define M3_STEP_PIN		GPIO_Pin_10

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

#ifdef HAS_LCD
	#define LCD_PWM_PORT		GPIOD
	#define LCD_PWM_PIN			GPIO_Pin_13

	#define LCD_BACKLIGHT_ON()	do { LCD_PWM_PORT->BSRR = LCD_PWM_PIN; } while(0)
	#define LCD_BACKLIGHT_OFF()	do { LCD_PWM_PORT->BRR  = LCD_PWM_PIN; } while (0)

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
	//	PD11-RS, PD4-nOE, PD5-nWE, PD7-LCD-CS
	#define LCD_CTRL_PORT		GPIOD
	#define LCD_CTRL_PINS		GPIO_Pin_11 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
#endif

#define LED_PORT			GPIOB
#define LED_PIN				GPIO_Pin_5
#define LED_ON()			do { LED_PORT->BSRR = LED_PIN; } while(0)
#define LED_OFF()			do { LED_PORT->BRR = LED_PIN; } while (0)

#define KEY_PORT			GPIOB
#define KEY_PIN				GPIO_Pin_15

#define SD_DATA_PORT		GPIOC
/* PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK */
#define SD_DATA_PINS		(GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12)
#define SD_CMD_PORT			GPIOD
#define SD_CMD_PIN			GPIO_Pin_2

#define USB_DISCONNECT		GPIOC
#define USB_DISCONNECT_PIN	GPIO_Pin_13

#ifdef HAS_FLASH
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
#define RS232_USART_TX_PIN	GPIO_Pin_9
#define RS232_USART_TX_PORT	GPIOA
#define RS232_USART_RX_PIN	GPIO_Pin_10
#define RS232_USART_RX_PORT	GPIOA

#include "hw_config.h"
#include "scr_io.h"
#include "rtc.h"
#include "ff.h"
#include "sdcard.h"
#ifdef HAS_FLASH
	#include "flash_spi.h"
#endif
#include "rs232_interface.h"
#include "keyboard.h"
#include "gcode.h"
#include "stepmotor.h"

#ifdef HAS_EXTRUDER
	#include "extruder_t.h"
#endif

#include "limits.h"
// #include "scan.h"
#include "encoder.h"

void delayMs(uint32_t msec);
char *str_trim(char *str);
uint8_t questionYesNo(char *msg, char *param);

void manualMode(void);

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

void showCriticalStatus(char *msg, int st);
uint16_t calcColor(uint8_t val);

//#define DEBUG_MODE
#ifdef DEBUG_MODE
	#define DBG(...) { rf_printf(__VA_ARGS__); }
#else
	#define DBG(...) { }
#endif

#endif /* GLOBAL_H_ */
