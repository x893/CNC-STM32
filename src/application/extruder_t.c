#include "global.h"

#ifdef HAS_EXTRUDER

void extrudT_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/* Configure SPI2 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_14; // PB13-SCK, PB15-MOSI PB14-MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	extrudT_FunOff();

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructure.TIM_Prescaler = 719; // 100 000
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 1000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM8, ENABLE);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	// TIM_Cmd(TIM8, ENABLE);
}

static int16_t extrudTemperatureReal = 0x7777, extrudTemperatureWait;

int16_t extrudT_getTemperatureReal(void) {
	return extrudTemperatureReal;
}

int16_t extrudT_getTemperatureWait(void) {
	return extrudTemperatureWait;
}

void extrudT_setTemperature(int16_t val) {
	extrudTemperatureWait = val;
}

uint8_t extrudT_isReady(void) {
	int dt = extrudTemperatureReal - extrudTemperatureWait;
	return dt < 2 && dt > -3;
}

static uint32_t MAX31855_data;
static uint8_t MAX31855_cntTx = 0, MAX31855_cntRx = 0;

void extrudT_proc(void) {
	if (MAX31855_cntRx == 0) {
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET ||
			SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_OVR) == SET) {
			SPI_I2S_ReceiveData(SPI2);
		}
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
	}
}

void extrudT_irq(void) {
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET) {
		SPI_I2S_SendData(SPI2, 0x0F);
		MAX31855_cntTx++;
		if (MAX31855_cntTx == 4) {
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
			MAX31855_cntTx = 0;
		}
	}

	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET){
		uint32_t d = SPI_I2S_ReceiveData(SPI2) & 0x0FF;
		if (MAX31855_cntRx == 0) MAX31855_data = d;
		else MAX31855_data |= d << MAX31855_cntRx;
		MAX31855_cntRx += 8;
		if (MAX31855_cntRx == 32) {
			MAX31855_cntRx = 0;
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
			extrudTemperatureReal = MAX31855_data; // TODO
		}
	}
}

void extrudT_FunOff(void) { GPIOB->BRR = GPIO_Pin_12; }
void extrudT_FunOn(void) { GPIOB->BSRR = GPIO_Pin_12; }

#endif
