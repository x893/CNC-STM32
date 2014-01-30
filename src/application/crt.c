#include "crt.h"
#include "stm32f10x_it.h"

void Reset_Handler(void) __attribute__((__interrupt__));

extern int  main(void);

#ifdef  __cplusplus
extern "C" {
#endif

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler( void );


#ifdef  __cplusplus
   }
#endif

/* init value for the stack pointer. defined in linker script */
extern unsigned _stack_end_;

__attribute__ ((section(".flash_vec_table")))
IrqHandlerFunc flash_vec_table[] =
{
  (IrqHandlerFunc)&_stack_end_,            /* The initial stack pointer*/
  Reset_Handler,             /* The reset handler*/
  NMIException,
  HardFaultException,
  MemManageException,
  BusFaultException,
  UsageFaultException,
  0, 0, 0, 0,            /* Reserved */
  SVCHandler,
  DebugMonitor,
  0,                      /* Reserved */
  PendSVC,
  SysTickHandler,
  WWDG_IRQHandler,
  PVD_IRQHandler,
  TAMPER_IRQHandler,
  RTC_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_IRQHandler,
  EXTI1_IRQHandler,
  EXTI2_IRQHandler,
  EXTI3_IRQHandler,
  EXTI4_IRQHandler,
  DMA1_Channel1_IRQHandler,
  DMA1_Channel2_IRQHandler,
  DMA1_Channel3_IRQHandler,
  DMA1_Channel4_IRQHandler,
  DMA1_Channel5_IRQHandler,
  DMA1_Channel6_IRQHandler,
  DMA1_Channel7_IRQHandler,
  ADC1_2_IRQHandler,
  USB_HP_CAN_TX_IRQHandler,
  USB_LP_CAN_RX0_IRQHandler,
  CAN_RX1_IRQHandler,
  CAN_SCE_IRQHandler,
  EXTI9_5_IRQHandler,
  TIM1_BRK_IRQHandler,
  TIM1_UP_IRQHandler,
  TIM1_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM4_IRQHandler,
  I2C1_EV_IRQHandler,
  I2C1_ER_IRQHandler,
  I2C2_EV_IRQHandler,
  I2C2_ER_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_IRQHandler,
  EXTI15_10_IRQHandler,
  RTCAlarm_IRQHandler,
  USBWakeUp_IRQHandler,
  TIM8_BRK_IRQHandler,
  TIM8_UP_IRQHandler,
  TIM8_TRG_COM_IRQHandler,
  TIM8_CC_IRQHandler,
  ADC3_IRQHandler,
  FSMC_IRQHandler,
  SDIO_IRQHandler,
  TIM5_IRQHandler,
  SPI3_IRQHandler,
  UART4_IRQHandler,
  UART5_IRQHandler,
  TIM6_IRQHandler,
  TIM7_IRQHandler,
  DMA2_Channel1_IRQHandler,
  DMA2_Channel2_IRQHandler,
  DMA2_Channel3_IRQHandler,
  DMA2_Channel4_5_IRQHandler,
};

void crt_init()
{
  // коприрование инициализированных данных в ОЗУ (секция .data)
  extern uint32_t _data_load_start_ ;
  extern uint32_t _data_start_ ;
  extern uint32_t _data_end_ ;
  volatile  uint32_t* data_load = &_data_load_start_ ;
  volatile  uint32_t* data = &_data_start_ ;
  volatile  uint32_t* data_end = &_data_end_ ;
  while( data < data_end )
         {
          *(data++) = *(data_load++);
         }

  // копирование секции .fast в ОЗУ
  extern unsigned _fast_load_start_ ;
  extern unsigned _fast_start_ ;
  extern unsigned _fast_end_   ;
  unsigned *fast_load = &_fast_load_start_ ;
  unsigned *fast = &_fast_start_ ;
  unsigned *fast_end = &_fast_end_ ;
  // коприрование кода в ОЗУ (секция .fast)
  while(fast < fast_end )
       {
        *(fast++) = *(fast_load++);
       }

  // инициализация данных в ОЗУ нулями(секция .bss)
  extern unsigned _bss_start_ ;
  extern unsigned _bss_end_   ;
  unsigned *bss = &_bss_start_ ;
  unsigned *bss_end = &_bss_end_ ;
  while(bss < bss_end )
   {
    *(bss++) = 0 ;
   }

  // инициализация системной библиотеки
  extern __attribute__ ((weak)) void __libc_init_array(void);
  if (__libc_init_array)
      __libc_init_array() ;

}

void Reset_Handler(void) {
   crt_init();

   main();
}

//void * __dso_handle = 0;

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


