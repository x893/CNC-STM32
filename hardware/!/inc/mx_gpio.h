/**
  ******************************************************************************
  * File Name          : mx_gpio.h
  * Date               : 06/02/2014 20:46:43
  * Description        : This file contains all the functions prototypes for 
  *                      the mx_gpio  
  ******************************************************************************
  *
  * COPYRIGHT 2014 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __mx_pinoutConfig_H
#define __mx_pinoutConfig_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
void mx_pinout_config(void);
#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */
