#ifndef __STM32LCD_H__
#define __STM32LCD_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdint.h>

#include "stm32f429i_discovery_ioe.h"

#if (USE_LCD == 2)

	#include "stm32f429i_discovery_lcd.h"

	#define LCD_WIDTH		320
	#define LCD_HEIGHT		240

	void LCD_SetTextColor(uint16_t Color);
	void LCD_Clear(uint16_t color);
	void LCD_PutChar(uint16_t x, uint16_t y, uint8_t Ascii, uint16_t fc, uint16_t bc);
	void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
	void LCD_SetPoint(uint16_t x, uint16_t y, uint16_t color);
	#define LCD_VLine(x, y, h, c)	do { LCD_SetTextColor(c); LCD_DrawLine(y, (LCD_PIXEL_HEIGHT - 1 - x), h, LCD_DIR_HORIZONTAL); } while (0)

	#define White          LCD_COLOR_WHITE
	#define Black          LCD_COLOR_BLACK
	#define Grey           LCD_COLOR_GREY
	#define Blue           LCD_COLOR_BLUE
	#define Blue2          LCD_COLOR_BLUE2
	#define Red            LCD_COLOR_RED
	#define Magenta        LCD_COLOR_MAGENTA
	#define Green          LCD_COLOR_GREEN
	#define Cyan           LCD_COLOR_CYAN
	#define Yellow         LCD_COLOR_YELLOW

#else

	#define LCD_Configuration()
	#define LCD_Initialization()
	#define LCD_Clear(color)
	#define LCD_BackLight(on)

#endif

#ifdef __cplusplus
}
#endif

#endif
