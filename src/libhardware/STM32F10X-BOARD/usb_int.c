/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_int.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Utility functions for memory transfers to/from PMA
*******************************************************************************/
#include "global.h"

#if (USE_USB == 1)

#include "usb_lib.h"

u16 SaveRState;
u16 SaveTState;

extern void(*pEpInt_IN[7])(void);    /*  Handles IN  interrupts   */
extern void(*pEpInt_OUT[7])(void);   /*  Handles OUT interrupts   */

/*******************************************************************************
* Function Name  : CTR_LP.
* Description    : Low priority Endpoint Correct Transfer interrupt's service
*                  routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CTR_LP(void)
{
	u32 wEPVal = 0;

	while (((wIstr = _GetISTR()) & ISTR_CTR) != 0)		/* stay in loop while pending ints */
	{
		_SetISTR((u16)CLR_CTR);							/* clear CTR flag */
		EPindex = (u8)(wIstr & ISTR_EP_ID);				/* extract highest priority endpoint number */
		if (EPindex == 0)
		{
			/* Decode and service control endpoint interrupt */
			/* calling related service routine */
			/* (Setup0_Process, In0_Process, Out0_Process) */
			SaveRState = _GetEPRxStatus(ENDP0);			/* save RX & TX status */
			SaveTState = _GetEPTxStatus(ENDP0);			/* and set both to NAK */
			_SetEPRxStatus(ENDP0, EP_RX_NAK);
			_SetEPTxStatus(ENDP0, EP_TX_NAK);

			if ((wIstr & ISTR_DIR) == 0)				/* DIR bit = origin of the interrupt */
			{
				/* DIR = 0 */
				/* DIR = 0      => IN  int */
				/* DIR = 0 implies that (EP_CTR_TX = 1) always  */

				_ClearEP_CTR_TX(ENDP0);
				In0_Process();

				_SetEPRxStatus(ENDP0, SaveRState);		/* before terminate set Tx & Rx status */
				_SetEPTxStatus(ENDP0, SaveTState);
				return;
			}
			else
			{
				/* DIR = 1 */
				/* DIR = 1 & CTR_RX       => SETUP or OUT int */
				/* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */

				wEPVal = _GetENDPOINT(ENDP0);
				if ((wEPVal & EP_CTR_TX) != 0)
				{
					_ClearEP_CTR_TX(ENDP0);
					In0_Process();
					
					_SetEPRxStatus(ENDP0, SaveRState);	/* before terminate set Tx & Rx status */
					_SetEPTxStatus(ENDP0, SaveTState);
					return;
				}
				else if ((wEPVal &EP_SETUP) != 0)
				{
					_ClearEP_CTR_RX(ENDP0);				/* SETUP bit kept frozen while CTR_RX = 1 */
					Setup0_Process();

					_SetEPRxStatus(ENDP0, SaveRState);	/* before terminate set Tx & Rx status */
					_SetEPTxStatus(ENDP0, SaveTState);
					return;
				}

				else if ((wEPVal & EP_CTR_RX) != 0)
				{
					_ClearEP_CTR_RX(ENDP0);
					Out0_Process();

					_SetEPRxStatus(ENDP0, SaveRState);	/* before terminate set Tx & Rx status */
					_SetEPTxStatus(ENDP0, SaveTState);
					return;
				}
			}
		}
		else
		{
			/* Decode and service non control endpoints interrupt  */
			wEPVal = _GetENDPOINT(EPindex);			/* process related endpoint register */
			if ((wEPVal & EP_CTR_RX) != 0)
			{
				_ClearEP_CTR_RX(EPindex);			/* clear int flag */
				(*pEpInt_OUT[EPindex - 1])();		/* call OUT service function */

			}
			if ((wEPVal & EP_CTR_TX) != 0)
			{
				_ClearEP_CTR_TX(EPindex);			/* clear int flag */
				(*pEpInt_IN[EPindex - 1])();		/* call IN service function */
			}
		}
	}
}

/*******************************************************************************
* Function Name  : CTR_HP.
* Description    : High Priority Endpoint Correct Transfer interrupt's service
*                  routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CTR_HP(void)
{
	u32 wEPVal = 0;

	while (((wIstr = _GetISTR()) & ISTR_CTR) != 0)
	{
		_SetISTR((u16)CLR_CTR);				/* clear CTR flag */
		EPindex = (u8)(wIstr & ISTR_EP_ID);	/* extract highest priority endpoint number */
		wEPVal = _GetENDPOINT(EPindex);		/* process related endpoint register */
		if ((wEPVal & EP_CTR_RX) != 0)
		{
			_ClearEP_CTR_RX(EPindex);		/* clear int flag */
			(*pEpInt_OUT[EPindex - 1])();	/* call OUT service function */
		}
		else if ((wEPVal & EP_CTR_TX) != 0)
		{
			_ClearEP_CTR_TX(EPindex);		/* clear int flag */
			(*pEpInt_IN[EPindex - 1])();	/* call IN service function */
		}
	}
}
#endif
