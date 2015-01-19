#include "global.h"

#if (USE_USB == 1)
	#include "usbh_core.h"
	#include "usbh_usr.h"
	#include "usbh_msc_core.h"
	#include "usb_hcd_int.h"

	USB_OTG_CORE_HANDLE	USB_OTG_Core;
	USBH_HOST			USB_Host;
#endif

void Led3On(void)	{	GPIO_SetBits(LED3_PORT, LED3_PIN);		}
void Led3Off(void)	{	GPIO_ResetBits(LED3_PORT, LED3_PIN);	}

void Led4On(void)	{	GPIO_SetBits(LED4_PORT, LED4_PIN);		}
void Led4Off(void)	{	GPIO_ResetBits(LED4_PORT, LED4_PIN);	}

uint8_t SystemProcess(void)
{
#if (USE_USB == 1)
	USBH_USR_BackgroundProcess();	/* Handle USB Host background process */
	if (USBH_USR_ApplicationState == USH_USR_READY)
	{
		USBH_USR_ApplicationState = USH_USR_PROCESS;
		return SYS_READ_FLASH;
	}
#endif
	return SYS_NO_ACTION;
}

/***************************************************
 *	Initialize hardware
 */
void SystemStartup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(0
		| RCC_AHB1Periph_GPIOA
		| RCC_AHB1Periph_GPIOB
		| RCC_AHB1Periph_GPIOC
		| RCC_AHB1Periph_GPIOD
		| RCC_AHB1Periph_GPIOE
		| RCC_AHB1Periph_GPIOF
		| RCC_AHB1Periph_GPIOG
		, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = LED3_PIN;
	GPIO_Init(LED3_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED4_PIN;
	GPIO_Init(LED4_PORT, &GPIO_InitStructure);

	LED_OFF();
	LEDRED_OFF();

	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		LEDRED_ON();
		while (1) { }
	}

#if (USE_RS232 == 1)

	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = RS232_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHB1PeriphClockCmd(RS232_TX_CLK | RS232_RX_CLK, ENABLE);
	RS232_CLOCK(ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(RS232_TX_PORT, RS232_TX_SOURCE, RS232_TX_AF);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(RS232_RX_PORT, RS232_RX_SOURCE, RS232_RX_AF);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = RS232_TX_PIN;
	GPIO_Init(RS232_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS232_RX_PIN;
	GPIO_Init(RS232_RX_PORT, &GPIO_InitStructure);

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

	/* Initialize Button */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
	GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

	stepm_init();

#if (USE_EXTRUDER == 1)
	extrudT_init();
#endif
#if (USE_ENCODER == 1)
	encoder_int();
#endif

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

#ifdef LIMIT_X_PORT
	PIN_SET_MODE(LIMIT_X_PORT, LIMIT_X_PIN);
#endif
#ifdef LIMIT_Y_PORT
	PIN_SET_MODE(LIMIT_Y_PORT, LIMIT_Y_PIN);
#endif
#ifdef LIMIT_Z_PORT
	PIN_SET_MODE(LIMIT_Z_PORT, LIMIT_Z_PIN);
#endif

#if (USE_RTC == 1)
	rtc_init();
#endif

#if (USE_LCD == 2)
	LCD_Init();
	LCD_LayerInit();
	LTDC_Cmd(ENABLE);

	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_SetTransparency(0);
	LCD_SetLayer(LCD_BACKGROUND_LAYER);

	LCD_Clear(Black);
	if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != SUCCESS)
	{
		showCriticalStatus("HSE FAIL", 0);
		LEDRED_ON();
		while (1) { }
	}
#endif

#if (USE_USB == 1)
	USBH_Init(&USB_OTG_Core, USB_OTG_HS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);  
#endif
}

void SystemStatus(uint32_t seconds)
{
}
