#ifndef __CRT_H__
#define __CRT_H__

#ifdef  __cplusplus
 extern "C" {
#endif

typedef void( *IrqHandlerFunc )( void );

#if (RAM_VEC_TABLE == 1)

typedef enum
{
    vtStackEnd = 0 ,            /* The initial stack pointer*/
    vtResetHandler,             /* The reset handler*/
    vtNMIException,
    vtHardFaultException,
    vtMemManageException,
    vtBusFaultException,
    vtUsageFaultException,
    vtReserve0,
    vtReserve1,
    vtReserve2,
    vtReserve3,            /* Reserved */
    vtvPortSVCHandler, //SVCHandler,
    vtDebugMonitor,
    vtReserve4,                      /* Reserved */
    vtxPortPendSVHandler, //PendSVC,
    vtxPortSysTickHandler, //SysTickHandler,
    vtWWDG_IRQHandler,
    vtPVD_IRQHandler,
    vtTAMPER_IRQHandler,
    vtRTC_IRQHandler,
    vtFLASH_IRQHandler,
    vtRCC_IRQHandler,
    vtEXTI0_IRQHandler,
    vtEXTI1_IRQHandler,
    vtEXTI2_IRQHandler,
    vtEXTI3_IRQHandler,
    vtEXTI4_IRQHandler,
    vtDMA1_Channel1_IRQHandler,
    vtDMA1_Channel2_IRQHandler,
    vtDMA1_Channel3_IRQHandler,
    vtDMA1_Channel4_IRQHandler,
    vtDMA1_Channel5_IRQHandler,
    vtDMA1_Channel6_IRQHandler,
    vtDMA1_Channel7_IRQHandler,
    vtADC1_2_IRQHandler,
    vtUSB_HP_CAN_TX_IRQHandler,
    vtUSB_LP_CAN_RX0_IRQHandler,
    vtCAN_RX1_IRQHandler,
    vtCAN_SCE_IRQHandler,
    vtEXTI9_5_IRQHandler,
    vtTIM1_BRK_IRQHandler,
    vtTIM1_UP_IRQHandler,
    vtTIM1_TRG_COM_IRQHandler,
    vtTIM1_CC_IRQHandler,
    vtTIM2_IRQHandler,
    vtTIM3_IRQHandler,
    vtTIM4_IRQHandler,
    vtI2C1_EV_IRQHandler,
    vtI2C1_ER_IRQHandler,
    vtI2C2_EV_IRQHandler,
    vtI2C2_ER_IRQHandler,
    vtSPI1_IRQHandler,
    vtSPI2_IRQHandler,
    vtUSART1_IRQHandler,
    vtUSART2_IRQHandler,
    vtUSART3_IRQHandler,
    vtEXTI15_10_IRQHandler,
    vtRTCAlarm_IRQHandler,
    vtUSBWakeUp_IRQHandler,
#if defined (TE_STM32F103)
    vtTIM8_BRK_IRQHandler,
    vtTIM8_UP_IRQHandler,
    vtTIM8_TRG_COM_IRQHandler,
    vtTIM8_CC_IRQHandler,
    vtADC3_IRQHandler,
    vtFSMC_IRQHandler,
    vtSDIO_IRQHandler,
    vtTIM5_IRQHandler,
    vtSPI3_IRQHandler,
    vtUART4_IRQHandler,
    vtUART5_IRQHandler,
    vtTIM6_IRQHandler,
    vtTIM7_IRQHandler,
    vtDMA2_Channel1_IRQHandler,
    vtDMA2_Channel2_IRQHandler,
    vtDMA2_Channel3_IRQHandler,
    vtDMA2_Channel4_5_IRQHandler,
#endif
    vtVecCount
} VectorType ;

//extern IrqHandlerFunc __attribute__ ((section(".ram_vec"))) ram_vec_table [] ;

void vec_table_copy(uint8_t map_needed) ;
void vec_map2ram (void) ;
void vec_map2flash (void) ;
void vec_set ( VectorType vec_type  , void* handler ) ;
void vec_table_copy2ram(uint8_t map_needed ) ;

#endif /*RAM_VEC_TABLE*/

#ifdef  __cplusplus
 }
#endif

#endif /*__CRT_H__*/
