/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : mass_mal.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Medium Access Layer interface
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "global.h"
#include "sdcard.h"
#include "mass_mal.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 Mass_Memory_Size[2];
u32 Mass_Block_Size[2];
u32 Mass_Block_Count[2];
vu32 Status = 0;

SD_CardInfo SDCardInfo;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u16 MAL_Init(u8 lun)
{
	uint16_t status = MAL_OK;
	uint32_t DeviceSizeMul, NumberOfBlocks = 0;

	switch (lun)
	{
	case 0:
		if ((Status = SD_Init()) != SD_OK)
			showCriticalStatus(" SD init error [code:%d]\n  Only RESET possible at now", Status);
		scr_puts("\nRead CSD/CID MSD registers..");
		if ((Status = SD_GetCardInfo(&SDCardInfo)) != SD_OK)
			showCriticalStatus(" SD get card info\n  error [code:%d]\n  Only RESET possible at now", Status);
		scr_printf("\nSD size:%d\nblock:%d type:%d", SDCardInfo.CardCapacity, SDCardInfo.CardBlockSize, SDCardInfo.CardType);
		scr_puts("\nSD select..");
		if ((Status = SD_SelectDeselect((u32)(SDCardInfo.RCA << 16))) != SD_OK)
			showCriticalStatus(" SD select card\n  error [code:%d]\n  Only RESET possible at now", Status);
		if ((Status = SD_SetDeviceMode(SD_POLLING_MODE)) != SD_OK)
			showCriticalStatus(" SD set device mode\n  error [code:%d]\n  Only RESET possible at now", Status);

		DeviceSizeMul = (SDCardInfo.SD_csd.DeviceSizeMul + 2);
		if (SDCardInfo.CardType == SDIO_HIGH_CAPACITY_SD_CARD)
			Mass_Block_Count[0] = (SDCardInfo.SD_csd.DeviceSize + 1) * 1024;
		else
		{
			NumberOfBlocks  = ((1 << (SDCardInfo.SD_csd.RdBlockLen)) / 512);
			Mass_Block_Count[0] = ((SDCardInfo.SD_csd.DeviceSize + 1) * (1 << DeviceSizeMul) << (NumberOfBlocks/2));
		}
		Mass_Block_Size[0]  = 512;
		Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
//      Status = SD_SetDeviceMode(SD_DMA_MODE);
		break;
	default:
		return MAL_FAIL;
	}
	return status;
}
/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u16 MAL_Write(u8 lun, u32 Memory_Offset, u32 *Writebuff, u16 Transfer_Length)
{
	switch (lun)
	{
	case 0:
		Status = SD_WriteBlock(Memory_Offset, Writebuff, Transfer_Length);
		if ( Status != SD_OK ) return MAL_FAIL;
		break;
	default:
		return MAL_FAIL;
	}
	return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
u16 MAL_Read(u8 lun, u32 Memory_Offset, u32 *Readbuff, u16 Transfer_Length)
{
	switch (lun)
	{
	case 0:
		Status = SD_ReadBlock(Memory_Offset, Readbuff, Transfer_Length);
		if ( Status != SD_OK )
			return MAL_FAIL;
		break;
    default:
		return MAL_FAIL;
	}
	return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u16 MAL_GetStatus(u8 lun) {
 return lun == 0? MAL_OK:MAL_FAIL;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
