#include "global.h"
#include "screen_font.h"
#include <string.h>

#ifdef USE_STM32F429I_DISCO

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

void WaitButtonPress(void)
{
	while (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) != Bit_RESET) { }
	while (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) != Bit_SET) { }
}
uint8_t IsButtonPress(void)
{
	return
		((GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET)
		? 0
		: 1
		);
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

	if (IOE_Config() != IOE_OK && IOE_Config() != IOE_OK)
	{
		LEDRED_ON();
	}
}

const TPKey_p * TPKeys;

uint16_t GetKbdTimer(void);
void SetKbdTimer(uint16_t msec);

int kbd_getKey(void)
{
	static uint16_t tp_last = 0;
	static uint16_t x, y;

	const TPKey_p * keys = TPKeys;

	if (keys != NULL && GetKbdTimer() == 0)
	{
		TP_STATE* TPState = IOE_TP_GetState();
		if (TPState->TouchDetected)
		{
			x = LCD_WIDTH - 1 - TPState->Y;
			y = TPState->X;
			tp_last = TPState->TouchDetected;
		}
		else if (tp_last)
		{
			const TPKey_t * key;
			tp_last = 0;
				
			while ((key = *keys++) != NULL)
			{
				if (key->KeyCode != 0)
				{
					if (x >= key->XTop && x <= key->XBottom
					&&	y >= key->YLeft && y <= key->YRight
						)
					{
						SetKbdTimer(50);
						return key->KeyCode;
					}
				}
			}
		}
		SetKbdTimer(50);
	}
	return -1;
}

const TPKey_p * SetTouchKeys(const TPKey_p * keys)
{
	const TPKey_p * tp_current = TPKeys;
	const char *text;
	const TPKey_t * key;
	TPKeys = keys;
	if (keys != NULL)
		while ((key = *keys++) != NULL)
		{
			if (key->KeyCode == 0)
			{
				scr_Rectangle( key->XTop, key->YLeft, key->XBottom, key->YRight, Black, true);
			}
			else if (key->Text != NULL)
			{
				scr_Rectangle( key->XTop, key->YLeft, key->XBottom, key->YRight, White, true);
				text = key->Text;
				uint16_t width = strlen(text) * FONT_STEP_X;
				uint16_t x = (key->XTop + key->XBottom - width) / 2;
				uint16_t y = (key->YLeft + key->YRight - FONT_STEP_Y) / 2;
				while (*text != 0)
				{
					LCD_PutChar(
						x,
						y,
						*text++,
						Black,
						White
					);
					x += FONT_STEP_X;
				}
			}
		}
	return tp_current;
}

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

void SystemStatus(uint32_t seconds)
{
	scr_fontColor(Red, Black);
	switch(USBH_USR_ApplicationState)
	{
	case USH_USR_READY:
		scr_fontColor(Green, Black);
		scr_puts("READY      ");
		LEDRED_OFF();
		break;
	case USH_USR_FS_INIT:
		scr_fontColor(Yellow, Black);
		if (seconds & 1)
			scr_puts("WAIT FLASH ");
		else
			scr_puts("           ");
		break;
	case USH_USR_OVERCURRENT:
		scr_puts("OVERCURRENT");
		break;
	case USH_USR_NOTSUPPORED:
		scr_puts("NOT SUPPORT");
		break;
	case USH_USR_BADSPEED:
		scr_puts("BAD SPEED  ");
		break;
	case USH_USR_DISCONNECTED:
		scr_fontColor(White, Black);
		scr_puts("DISCONNECT ");
		break;
	case USH_USR_UNRECOVERED:
		scr_puts("UNRECOVERED");
		break;
	case USH_USR_ATTACHED:
		scr_fontColor(White, Black);
		scr_puts("ATTACH     ");
	case USH_USR_FS_NOMOUNT:
		scr_puts("MOUNT ERROR");
		break;
	case USH_USR_PROCESS:
		scr_fontColor(Green, Black);
		scr_puts("MOUNTED    ");
		break;
	}
}

#if (USE_USB == 1)
extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE *pdev);

/**
  * @brief  OTG_HS_IRQHandler.
  *         This function handles USB-On-The-Go HS global interrupt request.
  *         requests.
  * @param  None
  * @retval None
  */
void OTG_HS_IRQHandler(void)
{
	USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

#endif
#endif /* USE_STM32F429I_DISCO */
