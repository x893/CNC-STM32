#include "global.h"
#include "core_cm3.h"
#include "mass_mal.h"

void USB_Init(void);

ErrorStatus HSEStartUpStatus;
RCC_ClocksTypeDef RCC_Clocks;

#ifdef  __cplusplus
extern "C" {
#endif
	uint32_t GetCpuClock()
	{
		return RCC_Clocks.SYSCLK_Frequency;
	}
#ifdef  __cplusplus
}
#endif

void SystemStartup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Unlock the internal flash */
	FLASH_Unlock();

	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);	/* Flash 2 wait state */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	/* HCLK = SYSCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);		/* PCLK2 = HCLK */
		RCC_PCLK1Config(RCC_HCLK_Div2);		/* PCLK1 = HCLK/2 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);	// ADCCLK = PCLK2/6 = 12MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	/* PLLCLK = 8MHz * 9 = 72 MHz */
		RCC_PLLCmd(ENABLE);					/* Enable PLL */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);	/* Select PLL as system clock source */
		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x08);
	}
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(SystemFrequency / 1000);

	RCC_APB2PeriphClockCmd(0
		| RCC_APB2Periph_AFIO
		| RCC_APB2Periph_GPIOA
		| RCC_APB2Periph_GPIOB
		| RCC_APB2Periph_GPIOC
		| RCC_APB2Periph_GPIOD
		| RCC_APB2Periph_GPIOE
		, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);	// LED on board

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_PIN;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);	// KEY on board

#ifdef HAS_FLASH
	SST25_flashInit();
#endif
	rs232_init();
	kbd_init();
	stepm_init();
#ifdef HAS_EXTRUDER
	extrudT_init();
#endif
#ifdef HAS_ENCODER
	encoder_int();
#endif

	limits_init();
	rtc_init();

	Lcd_Configuration();
	ili9320_Initializtion();
	ili9320_Clear(0);
	ili9320_BackLight(1);

	win_showMsgWin();
	delayMs(500);
	scr_puts("   ---- CNC MM -----");

	MAL_Init(0);

	// USB configure
	/* USB_DISCONNECT_PIN used as USB pull-up */
	GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

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
}
