/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_init.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Initialization routines & global variables
*******************************************************************************/
#include "global.h"

#if (USE_USB == 1)

#include "usb_lib.h"

/*  The number of current endpoint, it will be used to specify an endpoint */
u8	EPindex;
/*  The number of current device, it is an index to the Device_Table */
/* u8	Device_no; */
/*  Points to the DEVICE_INFO structure of current device */
/*  The purpose of this register is to speed up the execution */
DEVICE_INFO *pInformation;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
DEVICE_PROP *pProperty;
/*  Temporary save the state of Rx & Tx status. */
/*  Whenever the Rx or Tx state is changed, its value is saved */
/*  in this variable first and will be set to the EPRB or EPRA */
/*  at the end of interrupt process */
u16	SaveState;
u16  wInterrupt_Mask;
DEVICE_INFO	Device_Info;
USER_STANDARD_REQUESTS  *pUser_Standard_Requests;

/*******************************************************************************
* Function Name  : USB_Init
* Description    : USB system initialization
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Init(void)
{
	pInformation = &Device_Info;
	pInformation->ControlState = 2;
	pProperty = &Device_Property;
	pUser_Standard_Requests = &User_Standard_Requests;
	/* Initialize devices one by one */
	pProperty->Init();
}
#endif
