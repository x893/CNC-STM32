/*--------------File Info-------------------------------------------------------
** 文   件   名:  Config.h
** 最后修改日期:  2008.3.28
** 版        本:  V1.0
** 描        述:  配置文件、裁剪不需要的功能					
**------------------------------------------------------------------------------
** Created   by:    		
** Created date:			
*******************************************************************************/
#ifndef __Config_h
#define __Config_h    

#include "stm32f10x.h"
//#include "stm32f10x_type.h"
#include "integer.h"

#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

//SD卡测试使能
#define TEST_SD_EN  

//FATFS测试使能
#define TEST_FATFS_EN 

//复杂的printp()函数是否使用
#define PRINTP_EN  	

#endif

