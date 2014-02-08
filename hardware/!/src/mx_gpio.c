/**
  ******************************************************************************
  * File Name          : mx_gpio.c
  * Date               : 06/02/2014 20:46:42
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT 2014 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mx_gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

void mx_pinout_config(void) {
	/* Private typedef ---------------------------------------------------------*/
	GPIO_InitTypeDef GPIO_InitStruct;


	/** FSMC_SDRAM GPIO Configuration	
		 PF0	 ------> FSMC_SDRAM_A0
		 PF1	 ------> FSMC_SDRAM_A1
		 PF2	 ------> FSMC_SDRAM_A2
		 PF3	 ------> FSMC_SDRAM_A3
		 PF4	 ------> FSMC_SDRAM_A4
		 PF5	 ------> FSMC_SDRAM_A5
		 PC0	 ------> FSMC_SDRAM_SDNWE
		 PC2	 ------> FSMC_SDRAM_SDNE0
		 PC3	 ------> FSMC_SDRAM_SDCKE0
		 PF11	 ------> FSMC_SDRAM_SDNRAS
		 PF12	 ------> FSMC_SDRAM_A6
		 PF13	 ------> FSMC_SDRAM_A7
		 PF14	 ------> FSMC_SDRAM_A8
		 PF15	 ------> FSMC_SDRAM_A9
		 PG0	 ------> FSMC_SDRAM_A10
		 PG1	 ------> FSMC_SDRAM_A11
		 PE7	 ------> FSMC_SDRAM_D4
		 PE8	 ------> FSMC_SDRAM_D5
		 PE9	 ------> FSMC_SDRAM_D6
		 PE10	 ------> FSMC_SDRAM_D7
		 PE11	 ------> FSMC_SDRAM_D8
		 PE12	 ------> FSMC_SDRAM_D9
		 PE13	 ------> FSMC_SDRAM_D10
		 PE14	 ------> FSMC_SDRAM_D11
		 PE15	 ------> FSMC_SDRAM_D12
		 PD8	 ------> FSMC_SDRAM_D13
		 PD9	 ------> FSMC_SDRAM_D14
		 PD10	 ------> FSMC_SDRAM_D15
		 PD14	 ------> FSMC_SDRAM_D0
		 PD15	 ------> FSMC_SDRAM_D1
		 PG4	 ------> FSMC_SDRAM_BA0
		 PG5	 ------> FSMC_SDRAM_BA1
		 PG8	 ------> FSMC_SDRAM_SDCLK
		 PD0	 ------> FSMC_SDRAM_D2
		 PD1	 ------> FSMC_SDRAM_D3
		 PG15	 ------> FSMC_SDRAM_SDNCAS
		 PB5	 ------> FSMC_SDRAM_SDCKE1
		 PB6	 ------> FSMC_SDRAM_SDNE1
		 PE0	 ------> FSMC_SDRAM_NBL0
		 PE1	 ------> FSMC_SDRAM_NBL1
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOB, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);


	//!!! GPIO_Init is commented because some parameters are missing
	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	//GPIO_InitStruct.GPIO_Mode = ;
	//GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FSMC);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FSMC);


	/** I2C3 GPIO Configuration	
		 PC9	 ------> I2C3_SDA
		 PA8	 ------> I2C3_SCL
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);


	/** LTDC GPIO Configuration	
		 PF10	 ------> LTDC_DE
		 PA3	 ------> LTDC_B5
		 PA4	 ------> LTDC_VSYNC
		 PA6	 ------> LTDC_G2
		 PB0	 ------> LTDC_R3
		 PB1	 ------> LTDC_R6
		 PB10	 ------> LTDC_G4
		 PB11	 ------> LTDC_G5
		 PG6	 ------> LTDC_R7
		 PG7	 ------> LTDC_CLK
		 PC6	 ------> LTDC_HSYNC
		 PC7	 ------> LTDC_G6
		 PA11	 ------> LTDC_R4
		 PA12	 ------> LTDC_R5
		 PC10	 ------> LTDC_R2
		 PD3	 ------> LTDC_G7
		 PD6	 ------> LTDC_B2
		 PG10	 ------> LTDC_G3
		 PG11	 ------> LTDC_B3
		 PG12	 ------> LTDC_B4
		 PB8	 ------> LTDC_B6
		 PB9	 ------> LTDC_B7
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_CAN1);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_CAN1);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_CAN1);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_CAN1);


	/** SPI5 GPIO Configuration	
		 PF7	 ------> SPI5_SCK
		 PF8	 ------> SPI5_MISO
		 PF9	 ------> SPI5_MOSI
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);


	/** SYS GPIO Configuration	
		 PC14/OSC32_IN	 ------> SYS_OSC32_IN
		 PC15/OSC32_OUT	 ------> SYS_OSC32_OUT
		 PH0/OSC_IN	 ------> SYS_OSC_IN
		 PH1/OSC_OUT	 ------> SYS_OSC_OUT
		 PA13	 ------> SYS_JTMS-SWDIO
		 PA14	 ------> SYS_JTCK-SWCLK
		 PB3	 ------> SYS_JTDO-SWO
	*/



	/** USART1 GPIO Configuration	
		 PA9	 ------> USART1_TX
		 PA10	 ------> USART1_RX
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);


	/** USB_OTG_HS GPIO Configuration	
		 PB12	 ------> USB_OTG_HS_ID
		 PB13	 ------> USB_OTG_HS_VBUS
		 PB14	 ------> USB_OTG_HS_DM
		 PB15	 ------> USB_OTG_HS_DP
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin alternate function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_FSMC);


	/** Configure pins as GPIO
		 PC1	 ------> GPIO_Output
		 PA0/WKUP	 ------> GPIO_Input
		 PA1	 ------> GPIO_Input
		 PA2	 ------> GPIO_Input
		 PA7	 ------> GPIO_Output
		 PC4	 ------> GPIO_Output
		 PC5	 ------> GPIO_Input
		 PD11	 ------> GPIO_Output
		 PD12	 ------> GPIO_Output
		 PD13	 ------> GPIO_Output
		 PA15	 ------> GPIO_Input
		 PD2	 ------> GPIO_Output
		 PD4	 ------> GPIO_Output
		 PD5	 ------> GPIO_Output
		 PD7	 ------> GPIO_Output
		 PG13	 ------> GPIO_Output
		 PG14	 ------> GPIO_Output
	*/


	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
