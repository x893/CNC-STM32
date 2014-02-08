/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_mem.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Utility functions for memory transfers to/from PMA
*******************************************************************************/

#include "usb_lib.h"

/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf: pointer to user memory area.
*                  - wPMABufAddr: address into PMA.
*                  - wNBytes: no. of bytes to be copied.
* Output         : None.
* Return         : None	.
*******************************************************************************/
void UserToPMABufferCopy(u8 *pbUsrBuf, u16 wPMABufAddr, u16 wNBytes)
{
	u32 n = (wNBytes + 1) >> 1;   /* n = (wNBytes + 1) / 2 */
	u32 i, temp1, temp2;
	u16 *pdwVal;
	pdwVal = (u16 *)(wPMABufAddr * 2 + PMAAddr);
	for (i = n; i != 0; i--)
	{
		temp1 = (u16)* pbUsrBuf;
		pbUsrBuf++;
		temp2 = temp1 | (u16)* pbUsrBuf << 8;
		*pdwVal++ = temp2;
		pdwVal++;
		pbUsrBuf++;
	}
}
/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void PMAToUserBufferCopy(u8 *pbUsrBuf, u16 wPMABufAddr, u16 wNBytes)
{
	u32 n = (wNBytes + 1) >> 1;/* /2*/
	u32 i;
	u32 *pdwVal;
	pdwVal = (u32 *)(wPMABufAddr * 2 + PMAAddr);
	for (i = n; i != 0; i--)
	{
		*(u16*)pbUsrBuf++ = *pdwVal++;
		pbUsrBuf++;
	}
}
