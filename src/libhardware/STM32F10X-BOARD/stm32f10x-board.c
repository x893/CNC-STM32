#include "global.h"
#include "mass_mal.h"
#include "usb_lib.h"
#include "usb_istr.h"

#ifdef STM32F10X_HD

void LedOn(void)
{
	GPIO_SetBits(LED_PORT, LED_PIN);
}
void LedOff(void)
{
	GPIO_ResetBits(LED_PORT, LED_PIN);
}

void USB_Init(void);

/***************************************************
 *	Initialize hardware
 */
void SystemStartup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(0
		| RCC_APB2Periph_AFIO
		| RCC_APB2Periph_GPIOA
		| RCC_APB2Periph_GPIOB
		| RCC_APB2Periph_GPIOC
		| RCC_APB2Periph_GPIOD
		| RCC_APB2Periph_GPIOE
		, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	LEDRED_OFF();

	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		LEDRED_ON();
		while (1) { }
	}

#if (USE_FLASH == 1)
	SST25_flashInit();
#endif

#if (USE_RS232 == 1)
	USART_InitTypeDef USART_InitStructure;

	RS232_USART_CLK();
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = RS232_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS232_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(RS232_USART_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS232_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(RS232_USART_RX_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = RS232_SPEED;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(RS232_USART, &USART_InitStructure);
	USART_Cmd(RS232_USART, ENABLE);

	USART_ITConfig(RS232_USART, USART_IT_RXNE, ENABLE);
#endif

#if (USE_KEYBOARD == 1)
	kbd_init();
#endif

	stepm_init();

#if (USE_EXTRUDER == 1)
	extrudT_init();
#endif
#if (USE_ENCODER == 1)
	encoder_int();
#endif

	PIN_INPUT_PU();
	PIN_SET_MODE(LIMIT_X_PORT, LIMIT_X_PIN);
	PIN_SET_MODE(LIMIT_Y_PORT, LIMIT_Y_PIN);
	PIN_SET_MODE(LIMIT_Z_PORT, LIMIT_Z_PIN);

#if (USE_RTC == 1)
	rtc_init();
#endif

#if (USE_LCD == 1)
	LCD_Configuration();
	LCD_Initialization();
	LCD_Clear(Black);
	LCD_BackLight(1);

	win_showMsgWin();
	delayMs(500);
	if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == SUCCESS)
		scr_puts("   ---- CNC MM -----");
	else
		showCriticalStatus("HSE FAIL", 0);
#endif

#if (USE_SDCARD == 1)
	MAL_Init(0);
#endif

#if (USE_USB == 1)
	// USB configure
	/* USB_DISCONNECT_PIN used as USB pull-up */

	GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(USB_DISCONNECT_PORT, &GPIO_InitStructure);

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USB_Init();
#endif
}

uint8_t SystemProcess(void)
{
	return SYS_NO_ACTION;
}
void SystemStatus(void)
{ }

#if (USE_USB == 1)
void USB_HP_CAN1_TX_IRQHandler(void)
{
	CTR_HP();
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}
#endif

#if (USE_EXTRUDER == 1)
void SPI2_IRQHandler(void)
{
	extrudT_irq();
}
#endif

#endif
