/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : All processing related to Mass Storage Demo (Endpoint 0)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usb_prop_H
#define __usb_prop_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported constants --------------------------------------------------------*/
#define Mass_Storage_GetConfiguration          NOP_Process
/* #define Mass_Storage_SetConfiguration          NOP_Process*/
#define Mass_Storage_GetInterface              NOP_Process
#define Mass_Storage_SetInterface              NOP_Process
#define Mass_Storage_GetStatus                 NOP_Process
/* #define Mass_Storage_ClearFeature              NOP_Process*/
#define Mass_Storage_SetEndPointFeature        NOP_Process
#define Mass_Storage_SetDeviceFeature          NOP_Process
/*#define Mass_Storage_SetDeviceAddress          NOP_Process*/

/* MASS Storage Requests*/
#define GET_MAX_LUN                0xFE
#define MASS_STORAGE_RESET         0xFF
#define LUN_DATA_LENGTH            1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MASS_init(void);
void MASS_Reset(void);
void Mass_Storage_SetConfiguration(void);
void Mass_Storage_ClearFeature(void);
void Mass_Storage_SetDeviceAddress (void);
void MASS_Status_In (void);
void MASS_Status_Out (void);
RESULT MASS_Data_Setup(u8);
RESULT MASS_NoData_Setup(u8);
RESULT MASS_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *MASS_GetDeviceDescriptor(u16 );
u8 *MASS_GetConfigDescriptor(u16);
u8 *MASS_GetStringDescriptor(u16);
u8 *Get_Max_Lun(u16 Length);

#endif /* __usb_prop_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

