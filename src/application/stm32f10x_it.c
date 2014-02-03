#include "global.h"
#include "diskio.h"
#include "ili9320.h"
#include "usb_lib.h"
#include "usb_istr.h"

void SysTickHandler(void)
{
	static uint16_t ledCount = 0;
	static uint16_t kbdCount = 0;
#if (USE_EXTRUDER == 1)
	static uint16_t  extruderTemperatureCnt = 0;
#endif

	ledCount++;
	if (ledCount == 500)
	{
		LED_ON();
	}
	else if (ledCount >= 1000)
	{
		ledCount = 0;
		LED_OFF();
	}

	kbdCount++;
	if (kbdCount >= 12)
	{
		kbdCount = 0;
		kbd_proc();
	}

#if (USE_EXTRUDER == 1)
	extruderTemperatureCnt++;
	if(extruderTemperatureCnt >= 20)
	{
		extruderTemperatureCnt = 0;
		extrudT_proc();
	}
#endif
}

void USB_HP_CAN_TX_IRQHandler(void)
{
	CTR_HP();
}
void USB_LP_CAN_RX0_IRQHandler(void)
{
	USB_Istr();
}

void stepm_proc(uint8_t id);

#ifdef M0_TIM_IRQHandler
void M0_TIM_IRQHandler(void)
{
	TIM_ClearITPendingBit(M0_TIM, TIM_IT_Update);
	stepm_proc(0);
}
#endif

#ifdef M1_TIM_IRQHandler
void M1_TIM_IRQHandler(void)
{
	TIM_ClearITPendingBit(M1_TIM, TIM_IT_Update);
	stepm_proc(1);
}
#endif
#ifdef M2_TIM_IRQHandler
void M2_TIM_IRQHandler(void)
{
	TIM_ClearITPendingBit(M2_TIM, TIM_IT_Update);
	stepm_proc(2);
}
#endif

#ifdef M3_TIM_IRQHandler
void M3_TIM_IRQHandler(void)
{
	TIM_ClearITPendingBit(M3_TIM, TIM_IT_Update);
	stepm_proc(3);
}
#endif

void SPI2_IRQHandler(void)
{
#if (USE_EXTRUDER == 1)
	extrudT_irq();
#endif
}

#if (USE_RS232 == 1)
void rs232_proc(void);
void USART1_IRQHandler(void)
{
	rs232_proc();
}
#endif

void NMIException(void) {}
void HardFaultException(void)	{ while (1) {} }
void MemManageException(void)	{ while (1) {} }
void BusFaultException(void)	{ while (1) {} }
void UsageFaultException(void)	{ while (1) {} }
void DebugMonitor(void) {}
void SVCHandler(void) {}
void PendSVC(void) {}

void WWDG_IRQHandler(void) {}
void PVD_IRQHandler(void) {}
void TAMPER_IRQHandler(void) {}
void RTC_IRQHandler(void) {}
void FLASH_IRQHandler(void) {}
void RCC_IRQHandler(void) {}
void EXTI0_IRQHandler(void) {}
void EXTI1_IRQHandler(void) {}
void EXTI2_IRQHandler(void) {}
void EXTI3_IRQHandler(void) {}
void EXTI4_IRQHandler(void) {}

void DMA1_Channel1_IRQHandler(void) {}
void DMA1_Channel2_IRQHandler(void) {}
void DMA1_Channel3_IRQHandler(void) {}
void DMA1_Channel4_IRQHandler(void) {}
void DMA1_Channel5_IRQHandler(void) {}
void DMA1_Channel6_IRQHandler(void) {}
void DMA1_Channel7_IRQHandler(void) {}

void ADC1_2_IRQHandler(void) {}

void CAN_RX1_IRQHandler(void) {}
void CAN_SCE_IRQHandler(void) {}
void EXTI9_5_IRQHandler(void) {}
void TIM1_BRK_IRQHandler(void) {}
void TIM1_UP_IRQHandler(void) {}
void TIM1_TRG_COM_IRQHandler(void) {}
void TIM1_CC_IRQHandler(void) {}

void I2C1_EV_IRQHandler(void) {}
void I2C1_ER_IRQHandler(void) {}
void I2C2_EV_IRQHandler(void) {}
void I2C2_ER_IRQHandler(void) {}

void SPI1_IRQHandler(void) {}

void USART2_IRQHandler(void) {}
void USART3_IRQHandler(void) {}
void EXTI15_10_IRQHandler(void) {}

void RTCAlarm_IRQHandler(void) {}
void USBWakeUp_IRQHandler(void) {}

void TIM8_BRK_IRQHandler(void) {}
void TIM8_UP_IRQHandler(void) {}
void TIM8_TRG_COM_IRQHandler(void) {}
void TIM8_CC_IRQHandler(void) {}

void ADC3_IRQHandler(void) {}
void FSMC_IRQHandler(void) {}

void SDIO_IRQHandler(void) {}

void SPI3_IRQHandler(void) {}
void UART4_IRQHandler(void) {}
void UART5_IRQHandler(void) {}
void TIM6_IRQHandler(void) {}
void TIM7_IRQHandler(void) {}

void DMA2_Channel1_IRQHandler(void) {}
void DMA2_Channel2_IRQHandler(void) {}
void DMA2_Channel3_IRQHandler(void) {}
void DMA2_Channel4_5_IRQHandler(void) {}
