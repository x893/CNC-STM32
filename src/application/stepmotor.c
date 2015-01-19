#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "global.h"

#define STEPS_BUF_SIZE	8

#if (STEPS_MOTORS > 0)
	typedef struct
	{
		uint32_t steps[STEPS_MOTORS];
		uint32_t pscValue[STEPS_MOTORS];
		uint32_t arrValue[STEPS_MOTORS];
		uint32_t f[STEPS_MOTORS];
		uint8_t  dir[STEPS_MOTORS];
	} LINE_DATA;
	volatile LINE_DATA steps_buf[STEPS_BUF_SIZE];
#endif

volatile int8_t steps_buf_count;
		 int8_t steps_buf_get, steps_buf_put;

#if (USE_STEP_DEBUG == 1)
	LINE_DATA cur_steps_buf; // for debug only
#endif

uint8_t mx_enable = 0;

typedef struct mx_pin_init_s {
	GPIO_TypeDef *	Port;
	uint16_t		Pin;
} mx_pin_init_t;

typedef struct mx_timer_init_s {
	TIM_TypeDef *	Timer;
	uint8_t			IRQn;
	uint32_t		Clock;
} mx_timer_init_t;

const mx_pin_init_t mx_limits[] = {
#ifdef LIMIT_X_PORT
	{ LIMIT_X_PORT,	LIMIT_X_PIN	},
#endif
#ifdef LIMIT_Y_PORT
	{ LIMIT_Y_PORT,	LIMIT_Y_PIN	},
#endif
#ifdef LIMIT_Z_PORT
	{ LIMIT_Z_PORT,	LIMIT_Z_PIN	},
#endif
	{ NULL }
};

#if (STEPS_MOTORS > 0)

	const mx_pin_init_t mx_enables[] = {
	#ifdef M0_EN_PORT
		{ M0_EN_PORT,	M0_EN_PIN	},
	#endif
	#ifdef M1_EN_PORT
		{ M1_EN_PORT,	M1_EN_PIN	},
	#endif
	#ifdef M2_EN_PORT
		{ M2_EN_PORT,	M2_EN_PIN	},
	#endif
	#ifdef M3_EN_PORT
		{ M3_EN_PORT,	M3_EN_PIN	},
	#endif
	#ifdef MX_EN_PORT
		{ MX_EN_PORT,	MX_EN_PIN	},
	#endif
		{ NULL }
	};

	const mx_pin_init_t mx_dirs[] = {
	#ifdef M0_DIR_PORT
		{ M0_DIR_PORT,	M0_DIR_PIN	},
	#endif
	#ifdef M1_DIR_PORT
		{ M1_DIR_PORT,	M1_DIR_PIN	},
	#endif
	#ifdef M2_DIR_PORT
		{ M2_DIR_PORT,	M2_DIR_PIN	},
	#endif
	#ifdef M3_DIR_PORT
		{ M3_DIR_PORT,	M3_DIR_PIN	},
	#endif
		{ NULL }
	};

	const mx_pin_init_t mx_steps[] = {
	#ifdef M0_DIR_PORT
		{ M0_STEP_PORT,	M0_STEP_PIN	},
	#endif
	#ifdef M1_DIR_PORT
		{ M1_STEP_PORT,	M1_STEP_PIN	},
	#endif
	#ifdef M2_DIR_PORT
		{ M2_STEP_PORT,	M2_STEP_PIN	},
	#endif
	#ifdef M3_DIR_PORT
		{ M3_STEP_PORT,	M3_STEP_PIN	},
	#endif
		{ NULL }
	};

	const mx_timer_init_t mx_timers[] = {
	#ifdef M0_TIM
		{ M0_TIM, M0_TIM_IRQn, M0_TIM_CLK },
	#endif
	#ifdef M1_TIM
		{ M1_TIM, M1_TIM_IRQn, M1_TIM_CLK },
	#endif
	#ifdef M2_TIM
		{ M2_TIM, M2_TIM_IRQn, M2_TIM_CLK },
	#endif
	#ifdef M3_TIM
		{ M3_TIM, M3_TIM_IRQn, M3_TIM_CLK },
	#endif
		{ NULL }
	};

#endif

#if (STEPS_MOTORS > 0)
	volatile struct
	{
		  int32_t	globalSteps;
		 uint32_t	steps;
			 bool	clk;
			 bool	dir;
	volatile bool	isInProc;
	} step_motors[STEPS_MOTORS];
#endif

void stepm_ports_init(const mx_pin_init_t * mx_port, GPIO_InitTypeDef * GPIO_InitStructure)
{
	while (mx_port->Port != NULL)
	{
		GPIO_ResetBits(mx_port->Port, mx_port->Pin);
		GPIO_InitStructure->GPIO_Pin = mx_port->Pin;
		GPIO_Init(mx_port->Port, GPIO_InitStructure);
		mx_port++;
	}
}

void stepm_in_ports_init(const mx_pin_init_t * mx_port, GPIO_InitTypeDef * GPIO_InitStructure)
{
	while (mx_port->Port != NULL)
	{
		GPIO_InitStructure->GPIO_Pin = mx_port->Pin;
		GPIO_Init(mx_port->Port, GPIO_InitStructure);
		mx_port++;
	}
}

void stepm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	steps_buf_count = 0;
	steps_buf_get = steps_buf_put = 0;

#if (STEPS_MOTORS > 0)

	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBase;

	mx_enable = 0;
	for (int i = 0; i < STEPS_MOTORS; i++)
	{
		step_motors[i].steps = 0;
		step_motors[i].clk = true;
		step_motors[i].isInProc = false;
		step_motors[i].globalSteps = 0;
	}

	PIN_SPEED_MID();
	PIN_OUTPUT_PP();
	stepm_ports_init(mx_enables, &GPIO_InitStructure);
	stepm_ports_init(mx_dirs, &GPIO_InitStructure);
	stepm_ports_init(mx_steps, &GPIO_InitStructure);
	
	PIN_INPUT_PU();
	stepm_in_ports_init(mx_limits, &GPIO_InitStructure);

	TIM_TimeBase.TIM_Prescaler = 1799;	// any
	TIM_TimeBase.TIM_Period = 100;		// any
	TIM_TimeBase.TIM_ClockDivision = 0;
	TIM_TimeBase.TIM_CounterMode = TIM_CounterMode_Up;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	const mx_timer_init_t * mx_timer = mx_timers;
	while (mx_timer->Timer != NULL)
	{
		NVIC_InitStructure.NVIC_IRQChannel = mx_timer->IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_Init(&NVIC_InitStructure);

		RCC_APB1PeriphClockCmd(mx_timer->Clock, ENABLE);
		TIM_TimeBaseInit(mx_timer->Timer, &TIM_TimeBase);
		mx_timer->Timer->EGR = TIM_PSCReloadMode_Update;
		TIM_ClearITPendingBit(mx_timer->Timer, TIM_IT_Update);
		TIM_ARRPreloadConfig(mx_timer->Timer, ENABLE);
		TIM_ITConfig(mx_timer->Timer, TIM_IT_Update, ENABLE);
		mx_timer++;
	}
#endif
}

#if (STEPS_MOTORS > 0)
void stepm_nextMove(void)
{
	int i;
	for (i = 0; i < STEPS_MOTORS; i++)
		if (step_motors[i].isInProc)
			return;

	// Check for moves presents in steps buffer and all motors stops
	while (steps_buf_count != 0)
	{
		// Load next move
		__disable_irq();
		LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_get]);
	#if (USE_STEP_DEBUG == 1)
		memcpy(&cur_steps_buf, p, sizeof(cur_steps_buf)); // for debug
	#endif

		mx_enable = 0;
	#ifdef MX_EN_PORT
		GPIO_ResetBits(mx_enables[0].Port, mx_enables[0].Pin);
	#endif
		for (i = 0; i < STEPS_MOTORS; i++)
		{
	#ifndef MX_EN_PORT
			GPIO_ResetBits(mx_enables[i].Port, mx_enables[i].Pin);
	#endif
			if (p->steps[i] != 0)
			{
				step_motors[i].steps = p->steps[i];
				step_motors[i].dir = p->dir[i];

	#if (USE_ENCODER == 1)
				if (i == MX_ENCODER && isEncoderCorrection && step_motors[i].steps > ENCODER_CORRECTION_MIN_STEPS)
				{
					int32_t enVal = encoderZvalue();
					int32_t globalSteps = stepm_getCurGlobalStepsNum(i);
					int32_t dv = enVal * MM_PER_360 * 1000L / ENCODER_Z_CNT_PER_360 - globalSteps * MM_PER_360 * 1000L / SM_Z_STEPS_PER_360; // delta in 1/1000 mm
					encoderCorrectionDelta = dv;	// TODO for debug.. will be removed
					if (labs(encoderCorrectionMaxDelta) < labs(encoderCorrectionDelta))
						encoderCorrectionMaxDelta = encoderCorrectionDelta;

					if (dv > ENCODER_Z_MIN_MEASURE)
					{
						encoderCorrectionCntDown++;	// TODO for debug.. will be removed

						step_motors[i].globalSteps++;
						if (step_motors[i].dir)
							step_motors[i].steps--;
						else
							step_motors[i].steps++;
					}
					if (dv < -ENCODER_Z_MIN_MEASURE)
					{
						encoderCorrectionCntUp++;	// TODO for debug.. will be removed

						step_motors[i].globalSteps--;
						if (step_motors[i].dir) step_motors[i].steps++;
						else step_motors[i].steps--;
					}
				}
	#endif
				step_motors[i].clk = true;
				MX_STEP_OFF(mx_steps[i].Port, mx_steps[i].Pin);
				GPIO_WriteBit(mx_dirs[i].Port, mx_dirs[i].Pin, p->dir[i] ? Bit_SET : Bit_RESET);
	#ifdef MX_EN_PORT
				mx_enable |= (1 << i);	// M?_ENABLE;
	#else
				GPIO_SetBits(mx_enables[i].Port, mx_enables[i].Pin);
	#endif
				mx_timers[i].Timer->PSC = p->pscValue[i];
				TIM_SetAutoreload(mx_timers[i].Timer, p->arrValue[i]);
				step_motors[i].isInProc = true;
			}
		}

		steps_buf_get++;
		if (steps_buf_get >= STEPS_BUF_SIZE)
			steps_buf_get = 0;
		steps_buf_count--;

		for (i = 0; i < STEPS_MOTORS; i++)
			if (step_motors[i].isInProc)
				TIM_Cmd(mx_timers[i].Timer, ENABLE);
	#ifdef MX_EN_PORT
		if (mx_enable)
			GPIO_SetBits(mx_enables[0].Port, mx_enables[0].Pin);
	#endif
		__enable_irq();
		if (mx_enable != 0)
			break;
	}
}
#endif


void stepm_proc(uint8_t id)
{

#if (STEPS_MOTORS > 0)

	if (id >= STEPS_MOTORS)
		return;

	TIM_ClearITPendingBit(mx_timers[id].Timer, TIM_IT_Update);

	if (limits_chk())
	{
		stepm_EmergeStop();
		return;
	}

	if (step_motors[id].isInProc)
	{
		if (step_motors[id].clk)
			MX_STEP_ON(mx_steps[id].Port, mx_steps[id].Pin);
		else
			MX_STEP_OFF(mx_steps[id].Port, mx_steps[id].Pin);

		step_motors[id].clk = !step_motors[id].clk;
		if (!step_motors[id].clk)
		{
			if (step_motors[id].steps != 0)
				step_motors[id].steps--;
			if (step_motors[id].dir)
				step_motors[id].globalSteps++;
			else
				step_motors[id].globalSteps--;
		}
		else if (step_motors[id].steps == 0)
		{
			TIM_Cmd(mx_timers[id].Timer, DISABLE);
			step_motors[id].isInProc = false;
			stepm_nextMove();
		}
	}
#endif
}

void stepm_EmergeStop(void)
{
	__disable_irq();

	steps_buf_count = 0;
	steps_buf_get = steps_buf_put = 0;
#if (STEPS_MOTORS > 0)
	for (int i = 0; i < STEPS_MOTORS; i++)
	{
		step_motors[i].steps = 0;
		if (step_motors[i].clk)
		{
			TIM_Cmd(mx_timers[i].Timer, DISABLE);
			TIM_ClearITPendingBit(mx_timers[i].Timer, (TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 | TIM_IT_COM | TIM_IT_Trigger | TIM_IT_Break));
			step_motors[i].isInProc = false;
	#ifdef MX_EN_PORT
			mx_enable &= ~(1 << i);	// clear M?_ENABLE
	#else
			GPIO_ResetBits(mx_enables[i].Port, mx_enables[i].Pin);
	#endif
		}
	}
	#ifdef MX_EN_PORT
	if (mx_enable == 0)
		GPIO_ResetBits(mx_enables[0].Port, mx_enables[0].Pin);
	#endif
#endif
	__enable_irq();
}

void stepm_addMove(uint32_t steps[4], uint32_t frq[4], uint8_t dir[4])
{

#if (STEPS_MOTORS > 0)

	uint32_t pscValue;
	int i;

	bool empty = true;
	for (i = 0; i < STEPS_MOTORS; i++)
		if (steps[i] != 0)
		{
			empty = false;
			break;
		}
	if (empty)
		return;

	LINE_DATA *p = (LINE_DATA *)(&steps_buf[steps_buf_put]);

	while (steps_buf_count >= STEPS_BUF_SIZE)
	{
		// __WFI();
	}

	for (i = 0; i < STEPS_MOTORS; i++)
	{
		uint32_t f = frq[i];
		if (f > (15000 * K_FRQ))
			f = 15000 * K_FRQ;	// 15kHz
		if (f < K_FRQ)
			f = K_FRQ;			// 1Hz

		// SystemCoreClock / (psc * arr) = frq
		pscValue = 1;

		uint32_t arrValue = (SystemCoreClock / 2 * K_FRQ) / f; // (1 falling age on 2 IRQ)
		while ((arrValue & 0xffff0000) != 0)
		{
			pscValue = pscValue << 1;
			arrValue = arrValue >> 1;
		}
		pscValue--;
		p->f[i] = frq[i]; // for debug
		p->arrValue[i] = arrValue;
		p->pscValue[i] = pscValue;
		p->dir[i] = dir[i];
		p->steps[i] = steps[i];
	}

	steps_buf_put++;
	if (steps_buf_put >= STEPS_BUF_SIZE)
		steps_buf_put = 0;
	__disable_irq();
	steps_buf_count++;
	if (mx_enable == 0)
		stepm_nextMove();
	__enable_irq();

#endif
}

int32_t stepm_getRemainLines(void)
{
	return steps_buf_count;
}

int32_t stepm_inProc(void)
{
	if (steps_buf_count > 0)
		return true;
#if (STEPS_MOTORS > 0)
	for (int i = 0; i < STEPS_MOTORS; i++)
		if (step_motors[i].isInProc)
			return true;
#endif
	return false;
}

uint32_t stepm_LinesBufferIsFull(void)
{
	return steps_buf_count >= STEPS_BUF_SIZE;
}

int32_t stepm_getCurGlobalStepsNum(uint8_t id)
{
#if (STEPS_MOTORS > 0)
	return step_motors[id].globalSteps;
#else
	return 0;
#endif
}

void stepm_ZeroGlobalCrd(void)
{
#if (STEPS_MOTORS > 0)
	for (int i = 0; i < STEPS_MOTORS; i++)
		step_motors[i].globalSteps = 0;
#endif
}

#if (USE_STEP_DEBUG == 1)
void step_dump()
{
	#if (USE_LCD != 0)
	LINE_DATA *p = (LINE_DATA *)(&cur_steps_buf);
	for (int i = 0; i < 4; i++)
	{
		scr_gotoxy(1, 7 + i);
		scr_printf("%d,%d,%d,%d [%d]   ", p->steps[i], p->dir[i], p->pscValue[i], p->arrValue[i], p->f[i]); // TODO
	}
	#endif
}
#endif
