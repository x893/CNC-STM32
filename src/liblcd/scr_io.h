#ifndef SCR_IO_H_
#define SCR_IO_H_

#include "ili9320.h"

#ifndef HAS_LCD

#define scr_printf(...)
#define scr_puts(...)
#define scr_setfullTextWindow()
#define scr_gotoxy(...)
#define scr_fontColor(...)
#define scr_clrEndl()
#define scr_fontColorInvers()
#define scr_fontColorNormal()
#define scr_setScrollOn(x)

#define win_showErrorWin()
#define win_showMsgWin()
#define win_showMenuScroll(...)
#define win_showMenu(...)


#else

#define TEXT_Y_MAX 15

void scr_gotoxy(int8_t col, int8_t row);
int8_t scr_getCurRow(void);
void scr_fontColor(unsigned short fontC, unsigned short fontBkC);
void scr_fontColorInvers(void);
void scr_fontColorNormal(void);
void scr_setfullTextWindow(void);
void scr_setTextWindow(uint16_t x, uint16_t y, uint8_t maxCol, uint8_t maxRow);
void scr_setScrollOn(uint8_t flag);
void scr_putc(char);
void scr_puts(const char*);
void scr_clrEndl(void);
void scr_itoa(long val, int radix, int len);
void scr_printf(const char*, ...);
void scr_dump(const unsigned char *buff, int sz);

void win_showErrorWin(void);
void win_showMsgWin(void);
void win_showProgressBar(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint8_t p);
void win_showMenu(uint16_t x, uint16_t y, uint8_t col, uint8_t row);
void win_showMenuScroll(uint16_t x, uint16_t y, uint8_t col, uint8_t row, uint8_t startPos, uint8_t selPos, uint8_t lines);

#endif
#endif
