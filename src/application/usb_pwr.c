#include "global.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"

vu32 bDeviceState = UNCONNECTED; /* USB device status */
volatile bool fSuspendEnabled = true;  /* true when suspend is possible */

struct
{
	volatile RESUME_STATE eState;
	volatile u8 bESOFcnt;
} ResumeS;

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config(FunctionalState NewState)
{
	if (NewState != DISABLE)
		USB_CONNECTED();
	else
		USB_DISCONNECTED();
}


/*******************************************************************************
* Function Name  : PowerOn
* Description    :
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOn(void)
{
	u16 wRegVal;

	/*** cable plugged-in ? ***/
	/*while(!CablePluggedIn());*/
	USB_Cable_Config(ENABLE);

	/*** CNTR_PWDN = 0 ***/
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);

	/*** CNTR_FRES = 0 ***/
	wInterrupt_Mask = 0;
	_SetCNTR(wInterrupt_Mask);
	/*** Clear pending interrupts ***/
	_SetISTR(0);
	/*** Set interrupt mask ***/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : PowerOff
* Description    : handles switch-off conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOff()
{
	/* disable all ints and force USB reset */
	_SetCNTR(CNTR_FRES);
	/* clear interrupt status register */
	_SetISTR(0);
	/* Disable the Pull-Up*/
	USB_Cable_Config(DISABLE);
	/* switch-off device */
	_SetCNTR(CNTR_FRES + CNTR_PDWN);
	/* sw variables reset */
	/* ... */

	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0)
		bDeviceState = CONFIGURED;
	else
		bDeviceState = ATTACHED;
}


/*******************************************************************************
* Function Name  : Suspend
* Description    : sets suspend mode operating conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Suspend(void)
{
	u16 wCNTR;
	/* suspend preparation */
	/* ... */

	/* macrocell enters suspend mode */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
	/* power reduction */
	/* ... on connected devices */


	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);

	/* switch-off the clocks */
	/* ... */
	Enter_LowPowerMode();

}

/*******************************************************************************
* Function Name  : Resume_Init
* Description    : Handles wake-up restoring normal operations
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Resume_Init(void)
{
	u16 wCNTR;
	/* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
	/* restart the clocks */
	/* ...  */

	/* CNTR_LPMODE = 0 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);

	/* restore full power */
	/* ... on connected devices */
	Leave_LowPowerMode();

	/* reset FSUSP bit */
	_SetCNTR(IMR_MSK);

	/* reverse suspend preparation */
	/* ... */

}

/*******************************************************************************
* Function Name  : Resume
* Description    : This is the state machine handling resume operations and
*                 timing sequence. The control is based on the Resume structure
*                 variables and on the ESOF interrupt calling this subroutine
*                 without changing machine state.
* Input          : a state machine value (RESUME_STATE)
*                  RESUME_ESOF doesn't change ResumeS.eState allowing
*                  decrementing of the ESOF counter in different states.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Resume(RESUME_STATE eResumeSetVal)
{
	u16 wCNTR;

	if (eResumeSetVal != RESUME_ESOF)
		ResumeS.eState = eResumeSetVal;

	switch (ResumeS.eState)
	{
	case RESUME_EXTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_OFF;
		break;
	case RESUME_INTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_START;
		break;
	case RESUME_LATER:
		ResumeS.bESOFcnt = 2;
		ResumeS.eState = RESUME_WAIT;
		break;
	case RESUME_WAIT:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0)
			ResumeS.eState = RESUME_START;
		break;
	case RESUME_START:
		wCNTR = _GetCNTR();
		wCNTR |= CNTR_RESUME;
		_SetCNTR(wCNTR);
		ResumeS.eState = RESUME_ON;
		ResumeS.bESOFcnt = 10;
		break;
	case RESUME_ON:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0)
		{
			wCNTR = _GetCNTR();
			wCNTR &= (~CNTR_RESUME);
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_OFF;
		}
		break;
	case RESUME_OFF:
	case RESUME_ESOF:
	default:
		ResumeS.eState = RESUME_OFF;
		break;
	}
}
