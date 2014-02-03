#include "global.h"

#if (USE_ENCODER == 1)

int32_t encoderCorrectionCntUp, encoderCorrectionCntDown, encoderCorrectionDelta, encoderCorrectionMaxDelta;
bool isEncoderCorrection = false;

void encoder_int()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef timer_base;
	TIM_ICInitTypeDef TIM_ICInitStruct;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = ENCODER_PINS;
	GPIO_Init(ENCODER_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&timer_base);
	timer_base.TIM_Period = 0xFFFF;
	timer_base.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &timer_base);

	// Debounce filter
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 3;
	TIM_ICInit(TIM8, &TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 3;
	TIM_ICInit(TIM8, &TIM_ICInitStruct);

	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	//TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	//NVIC_EnableIRQ(TIM8_IRQn);
	//-------------
	// if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET) {
	//   TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
	//   direction = (TIM8->CR1 & TIM_CR1_DIR ? FORWARD : BACKWARD);
	// }

	TIM_Cmd(TIM8, ENABLE);

	encoderZreset();
	encoderCorrectionCntUp = encoderCorrectionCntDown = encoderCorrectionDelta = encoderCorrectionMaxDelta = 0;
}

#endif
