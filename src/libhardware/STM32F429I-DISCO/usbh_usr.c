/**
  ******************************************************************************
  * @file    FW_upgrade/src/usbh_usr.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   This file includes the usb host user callbacks
  ******************************************************************************
  */ 
#include "usbh_usr.h"
#include "ff.h"

USBH_Usr_cb_TypeDef USR_Callbacks =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};

uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;

extern USB_OTG_CORE_HANDLE USB_OTG_Core;
extern USBH_HOST USB_Host;
extern FATFS fatfs;

/**
  * @brief  USBH_USR_Init
  *
  * @param  None
  * @retval None
  */
void USBH_USR_Init(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

/**
  * @brief  USBH_USR_DeviceAttached
  *         Displays the message on LCD on device attached
  * @param  None
  * @retval None
  */
void USBH_USR_DeviceAttached(void)
{
	// USBH_USR_ApplicationState = USH_USR_ATTACHED;
}

/**
  * @brief  USBH_USR_UnrecoveredError
  * @param  None
  * @retval None
  */
void USBH_USR_UnrecoveredError (void)
{
	USBH_USR_ApplicationState = USH_USR_UNRECOVERED;
	Fail_Handler();
}

/**
  * @brief  USBH_DisconnectEvent
  *         Device disconnect event
  * @param  None
  * @retval Staus
  */
void USBH_USR_DeviceDisconnected (void)
{
	USBH_USR_ApplicationState = USH_USR_DISCONNECTED;
	Fail_Handler();
}

/**
  * @brief  USBH_USR_ResetUSBDevice
  * @param  None
  * @retval None
  */
void USBH_USR_ResetDevice(void)
{
	/* callback for USB-Reset */
}

/**
  * @brief  USBH_USR_DeviceSpeedDetected
  *         Displays the message on LCD for device speed
  * @param  Device speed:
  * @retval None
  */
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if ((DeviceSpeed != HPRT0_PRTSPD_FULL_SPEED)
	&&  (DeviceSpeed != HPRT0_PRTSPD_LOW_SPEED)
		)
	{
		USBH_USR_ApplicationState = USH_USR_BADSPEED;
		Fail_Handler();
	}
}

/**
  * @brief  USBH_USR_Device_DescAvailable
  * @param  device descriptor
  * @retval None
  */
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ }

/**
  * @brief  USBH_USR_DeviceAddressAssigned
  *         USB device is successfully assigned the Address
  * @param  None
  * @retval None
  */
void USBH_USR_DeviceAddressAssigned(void)
{ }

/**
  * @brief  USBH_USR_Conf_Desc
  * @param  Configuration descriptor
  * @retval None
  */
void USBH_USR_Configuration_DescAvailable(
	USBH_CfgDesc_TypeDef * cfgDesc,
    USBH_InterfaceDesc_TypeDef * itfDesc,
    USBH_EpDesc_TypeDef * epDesc
	)
{ }

/**
  * @brief  USBH_USR_Manufacturer_String
  * @param  Manufacturer String
  * @retval None
  */
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{ }

/**
  * @brief  USBH_USR_Product_String
  * @param  Product String
  * @retval None
  */
void USBH_USR_Product_String(void *ProductString)
{ }

/**
  * @brief  USBH_USR_SerialNum_String
  * @param  SerialNum_String
  * @retval None
  */
void USBH_USR_SerialNum_String(void *SerialNumString)
{ }

/**
  * @brief  EnumerationDone 
  *         User response request is displayed to ask application jump to class
  * @param  None
  * @retval None
  */
void USBH_USR_EnumerationDone(void)
{ }

/**
  * @brief  USBH_USR_DeviceNotSupported
  *         Device is not supported
  * @param  None
  * @retval None
  */
void USBH_USR_DeviceNotSupported(void)
{
	USBH_USR_ApplicationState = USH_USR_NOTSUPPORED;
	Fail_Handler();
}

/**
  * @brief  USBH_USR_UserInput
  *         User Action for application state entry
  * @param  None
  * @retval USBH_USR_Status : User response for key button
  */
USBH_USR_Status USBH_USR_UserInput(void)
{	/* callback for Key botton: set by software in this case */
	return USBH_USR_RESP_OK;
}

/**
  * @brief  USBH_USR_OverCurrentDetected
  *         Over Current Detected on VBUS
  * @param  None
  * @retval None
  */
void USBH_USR_OverCurrentDetected (void)
{
	USBH_USR_ApplicationState = USH_USR_OVERCURRENT;
}

/**
  * @brief  USBH_USR_MSC_Application
  * @param  None
  * @retval Status
  */
void showNonCriticalStatus(char *msg, int st);

int USBH_USR_MSC_Application(void)
{
	if (USBH_USR_ApplicationState != USH_USR_READY
	&&	USBH_USR_ApplicationState != USH_USR_FS_NOMOUNT
	&&	USBH_USR_ApplicationState != USH_USR_PROCESS
		)
	{
		FRESULT fres = f_mount(0, &fatfs);
		if (fres != FR_OK )
		{
			USBH_USR_ApplicationState = USH_USR_FS_NOMOUNT;
			showNonCriticalStatus(	" Mount flash error [code:%d]\n"
									" It used for any CNC process\n"
									" Eject and insert again", fres
									);
			return(-1); 
		}
		USBH_USR_ApplicationState = USH_USR_READY;
	}
	return 0;
}

/**
  * @brief  USBH_USR_DeInit
  *         Deint User state and associated variables
  * @param  None
  * @retval None
  */
void USBH_USR_DeInit(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

/**
  * @brief  This function handles the program fail.
  * @param  None
  * @retval None
  */
void Fail_Handler(void)
{
	LEDRED_ON();
}

void USBH_USR_BackgroundProcess(void)
{
	if ((	USBH_USR_ApplicationState != USH_USR_READY
		&&	USBH_USR_ApplicationState != USH_USR_PROCESS
			)
	||  HCD_IsDeviceConnected(&USB_OTG_Core) == 0
		)
		USBH_Process(&USB_OTG_Core, &USB_Host);
}
