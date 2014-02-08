#ifndef __ILI9320_H__
#define __ILI9320_H__

#include "global.h"

#define LCD_WIDTH		320
#define LCD_HEIGHT		240

void ili9320_Configuration(void);
void ili9320_Initialization(void);
void ili9320_Clear(uint16_t dat);
void ili9320_BackLight(uint8_t status);
void ili9320_SetPoint(uint16_t x, uint16_t y, uint16_t color);
void ili9320_PutChar(uint16_t x, uint16_t y, uint8_t c, uint16_t charColor, uint16_t bkColor);
void ili9320_VLine(uint16_t x0, uint16_t y0, uint16_t h, uint16_t color);

#define LCD_Configuration()			ili9320_Configuration()
#define LCD_Initialization()		ili9320_Initialization()
#define LCD_Clear(color)			ili9320_Clear(color)
#define LCD_BackLight(on)			ili9320_BackLight(on)
#define LCD_SetPoint(x,y,color)		ili9320_SetPoint(x, y, color);
#define LCD_PutChar(x,y,ch,fcolor,bcolor)	ili9320_PutChar(x,y,ch,fcolor,bcolor);
#define LCD_VLine(x0,y0,height,color)		ili9320_VLine(x0,y0,height,color);

#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#endif
