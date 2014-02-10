/**
  ******************************************************************************
  * @file    FW_upgrade/inc/usbh_usr.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Header file for usbh_usr.c.
  ******************************************************************************
  */ 
#ifndef __USH_USR_H__
#define __USH_USR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "global.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"

/* State Machine for the USBH_USR_ApplicationState */
#define USH_USR_READY		((uint8_t)0x00)
#define USH_USR_FS_INIT		((uint8_t)0x01)
#define USH_USR_OVERCURRENT	((uint8_t)0x02)
#define USH_USR_NOTSUPPORED	((uint8_t)0x03)
#define USH_USR_BADSPEED	((uint8_t)0x04)
#define USH_USR_DISCONNECTED ((uint8_t)0x05)
#define USH_USR_UNRECOVERED	((uint8_t)0x06)
#define USH_USR_ATTACHED	((uint8_t)0x07)
#define USH_USR_FS_NOMOUNT	((uint8_t)0x08)
#define USH_USR_PROCESS		((uint8_t)0x09)
extern uint8_t USBH_USR_ApplicationState;

extern  USBH_Usr_cb_TypeDef USR_Callbacks;

void USBH_USR_Init(void);
void USBH_USR_DeviceAttached(void);
void USBH_USR_ResetDevice(void);
void USBH_USR_DeviceDisconnected (void);
void USBH_USR_OverCurrentDetected (void);
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed); 
void USBH_USR_Device_DescAvailable(void *);
void USBH_USR_DeviceAddressAssigned(void);
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc);
void USBH_USR_Manufacturer_String(void *);
void USBH_USR_Product_String(void *);
void USBH_USR_SerialNum_String(void *);
void USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
int USBH_USR_MSC_Application(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceNotSupported(void);
void USBH_USR_UnrecoveredError(void);

void USBH_USR_BackgroundProcess (void);

void Fail_Handler(void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#ifdef __cplusplus
}
#endif

#endif /*__USH_USR_H__*/
