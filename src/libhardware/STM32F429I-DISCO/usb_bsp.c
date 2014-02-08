/**
  ******************************************************************************
  * @file    FW_upgrade/src/usb_bsp.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   USB bsp
  ******************************************************************************
  */ 
#include "usb_bsp.h"
#include "usb_conf.h"
#include "stm32f429i_discovery.h"

#define TIM_MSEC_DELAY				0x01
#define TIM_USEC_DELAY				0x02
#define HOST_OVRCURR_PORT			GPIOE
#define HOST_OVRCURR_LINE			GPIO_Pin_1
#define HOST_OVRCURR_PORT_SOURCE	GPIO_PortSourceGPIOE
#define HOST_OVRCURR_PIN_SOURCE		GPIO_PinSource1
#define HOST_OVRCURR_PORT_RCC		RCC_APB2Periph_GPIOE
#define HOST_OVRCURR_EXTI_LINE		EXTI_Line1
#define HOST_OVRCURR_IRQn			EXTI1_IRQn 

#define HOST_POWERSW_PORT_RCC		RCC_APB2Periph_GPIOC
#define HOST_POWERSW_PORT			GPIOC
#define HOST_POWERSW_VBUS			GPIO_Pin_4

/** 
  * @brief  BSP_Init.
  *         Board user initializations.
  * @param  None
  * @retval None
  */
void BSP_Init(void)
{
}
/**
  * @brief  USB_OTG_BSP_Init
  *         Initilizes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configuration for USB OTG HS used in FS mode with EMBEDDED PHY */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS) ; 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS) ;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS, ENABLE);
}

/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	 
	NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
  /*
  On-chip 5 V VBUS generation is not supported. For this reason, a charge pump 
  or, if 5 V are available on the application board, a basic power switch, must 
  be added externally to drive the 5 V VBUS line. The external charge pump can 
  be driven by any GPIO output. When the application decides to power on VBUS 
  using the chosen GPIO, it must also set the port power bit in the host port 
  control and status register (PPWR bit in OTG_FS_HPRT).
  
  Bit 12 PPWR: Port power
  The application uses this field to control power to this port, and the core 
  clears this bit on an overcurrent condition.
  */  

	if (0 == state)
	{ 
		/* DISABLE is needed on output of the Power Switch */
		GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
	}
	else
	{
		/*ENABLE the Power Switch by driving the Enable LOW */
		GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
	}
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);  
  
	GPIO_InitStructure.GPIO_Pin = HOST_POWERSW_VBUS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(HOST_POWERSW_PORT, &GPIO_InitStructure);

	/* By Default, DISABLE is needed on output of the Power Switch */
	GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  
	delayMs(200);   /* Delay is need for stabilising the Vbus Low 
	in Reset Condition, when Vbus=1 and Reset-button is pressed by user */
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint16_t usec)
{
	__IO uint32_t count = 0;
	const uint32_t utime = (SystemCoreClock / 1000000 * usec / 7);
	do
	{
		if ( ++count > utime )
		{
			return ;
		}
	}
	while (1);
}
void USB_OTG_BSP_mDelay (const uint16_t msec)
{
	delayMs(msec);
}