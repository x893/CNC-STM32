/**
  ******************************************************************************
  * @file    fonts.h
  * @author  MCD Application Team
  * @version V5.0.2
  * @date    05-March-2012
  * @brief   Header for fonts.c file
  ******************************************************************************
  */

#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// #define LCD_DEFAULT_FONT	Font8x12

typedef struct _tFont
{    
  const uint16_t * table;
  uint16_t Width;
  uint16_t Height;
} sFONT;

extern sFONT Font16x24;
extern sFONT Font12x12;
extern sFONT Font8x16;
extern sFONT Font8x12;
extern sFONT Font8x8;

#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_H */
