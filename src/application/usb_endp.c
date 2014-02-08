#include "usb_lib.h"
#include "usb_bot.h"
#include "usb_istr.h"

#if (USE_USB == 1)

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    : EP1 IN Callback Routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
#if (USE_USB_MSD == 1)
	Mass_Storage_In();
#endif
}

/*******************************************************************************
* Function Name  : EP2_OUT_Callback.
* Description    : EP2 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
#if (USE_USB_MSD == 1)
	Mass_Storage_Out();
#endif
}
#endif
