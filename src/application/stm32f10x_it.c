#include "global.h"
#include "diskio.h"

struct SysTickTimers_s {
	uint32_t Seconds;
	volatile uint16_t Timer;
	uint16_t LedTimer;
	uint16_t MS1000;
#if (USE_KEYBOARD != 0)
	uint8_t KbdTimer;
#endif
#if (USE_EXTRUDER == 1)
	uint8_t ExtruderTimer;
#endif
} SysTickTimers;

void delayMs(uint16_t msec)
{
	SysTickTimers.Timer = 1;
	while (SysTickTimers.Timer != 0)
	{ }
	SysTickTimers.Timer = msec;
	while (SysTickTimers.Timer != 0)
	{
		// __WFI();
	}
}

uint16_t GetKbdTimer(void)
{
	return SysTickTimers.KbdTimer;
}

void SetKbdTimer(uint16_t msec)
{
	SysTickTimers.KbdTimer = msec;
}

uint32_t Seconds(void)
{
	return SysTickTimers.Seconds;
}
void SysTick_Handler(void)
{
	register struct SysTickTimers_s *vars = &SysTickTimers;

	if (vars->Timer != 0)
		vars->Timer--;

	vars->MS1000++;
	if (vars->MS1000 == 1000)
	{
		vars->MS1000 = 0;
		vars->Seconds++;
	}

	vars->LedTimer++;
	if (vars->LedTimer == 1900)
	{
		LED_ON();
	}
	else if (vars->LedTimer >= 2000)
	{
		vars->LedTimer = 0;
		LED_OFF();
	}

#if (USE_KEYBOARD == 1)
	vars->KbdTimer++;
	if (vars->KbdTimer >= 12)
	{
		vars->KbdTimer = 0;
		kbd_proc();
	}
#elif (USE_KEYBOARD == 2)
	if (vars->KbdTimer != 0)
		vars->KbdTimer--;
#endif

#if (USE_EXTRUDER == 1)
	vars->ExtruderTimer++;
	if (vars->ExtruderTimer >= 20)
	{
		vars->ExtruderTimer = 0;
		extrudT_proc();
	}
#endif
}

#if (STEPS_MOTORS > 0)
void stepm_proc(uint8_t id);

#ifdef M0_TIM_IRQHandler
void M0_TIM_IRQHandler(void)
{
	stepm_proc(0);
}
#endif

#ifdef M1_TIM_IRQHandler
void M1_TIM_IRQHandler(void)
{
	stepm_proc(1);
}
#endif
#ifdef M2_TIM_IRQHandler
void M2_TIM_IRQHandler(void)
{
	stepm_proc(2);
}
#endif

#ifdef M3_TIM_IRQHandler
void M3_TIM_IRQHandler(void)
{
	stepm_proc(3);
}
#endif
#endif	/* (STEPS_MOTORS > 0) */

#if (USE_RS232 == 1)
void rs232_proc(void);
void RS232_IRQHandler(void)
{
	rs232_proc();
}
#endif
