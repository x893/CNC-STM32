#ifndef _ILI9320_H_
#define _ILI9320_H_

#include "global.h"

#ifndef HAS_LCD

#define Lcd_Configuration()		{}
#define ili9320_Initializtion()	{}
#define ili9320_Clear(x)		{}
#define ili9320_BackLight(x)	{}
#define GUI_Rectangle(...)		{}
#define GUI_Line(...)			{}

#else

#define LCD_WIDTH		320
#define LCD_HEIGHT		240

#define FONT_STEP_X		8
#define FONT_STEP_Y		16

void ili9320_SetCursor(uint16_t x, uint16_t y);
void ili9320_WrVcamLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color[]);

void Lcd_Configuration(void);
uint16_t CheckController(void);
void ili9320_Initializtion(void);
void ili9320_WriteRegister(uint16_t index, uint16_t dat);
void ili9320_SetWindows(uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY);
void ili9320_DrawPicture(uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY, uint16_t *pic);
void ili9320_SetPoint(uint16_t x, uint16_t y, uint16_t point);
void ili9320_PutChar(uint16_t x, uint16_t y, uint8_t c, uint16_t charColor, uint16_t bkColor);
void ili9320_Clear(uint16_t dat);
void ili9320_Delay(uint32_t nCount);

void ili9320_WriteData(uint16_t dat);
void ili9320_WriteIndex(uint16_t idx);

void ili9320_VLine(uint16_t x0, uint16_t y0, uint16_t h, uint16_t color);
void ili9320_BackLight(uint8_t status);

uint16_t ili9320_BGR2RGB(uint16_t c);

uint16_t ili9320_ReadData(void);
uint16_t ili9320_ReadRegister(uint16_t index);

uint16_t GUI_Color565(uint32_t RGB);

void GUI_Text(uint16_t x, uint16_t y, const uint8_t *str, uint16_t len, uint16_t Color, uint16_t bkColor);
void GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void GUI_Circle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color, uint8_t fill);
void GUI_Rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t fill);
void GUI_Square(uint16_t x0, uint16_t y0, uint16_t with, uint16_t color, uint8_t fill);

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

#ifdef HAS_TOUCH
	uint16_t Touch_MeasurementX(void);
	uint16_t Touch_MeasurementY(void);

	// INT   (ADC touch)	- PE12
	uint8_t isTouch_pen(void);
	uint16_t Touch_ScrX(void);
	uint16_t Touch_ScrY(void);
#endif

#endif
