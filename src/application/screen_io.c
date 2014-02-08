#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "screen_io.h"

#if (USE_LCD == 1)

#define screen_font_from_c
#include "screen_font.h"

uint16_t fontColor = 0xFFFF, fontBkColor = 0;
uint16_t fix_fontColor = 0xFFFF, fix_fontBkColor = 0;
int8_t cur_col = 0, cur_row = 0,
	cur_width = LCD_WIDTH / FONT_STEP_X,
	cur_height = LCD_HEIGHT / FONT_STEP_Y;
bool flagScroll = true;
uint16_t cur_x_ofs = 0, cur_y_ofs = 0;

void scr_setfullTextWindow(void)
{
	cur_x_ofs = 0;
	cur_y_ofs = 0;
	cur_width = LCD_WIDTH / FONT_STEP_X;
	cur_height = LCD_HEIGHT / FONT_STEP_Y;
}

void scr_setTextWindow(uint16_t x, uint16_t y, uint8_t maxCol, uint8_t maxRow)
{
	cur_x_ofs = x;
	cur_y_ofs = y;
	cur_width = ((maxCol * FONT_STEP_X + x) >= LCD_WIDTH) ? (LCD_WIDTH - x) / FONT_STEP_X : maxCol;
	cur_height = ((maxRow * FONT_STEP_Y + y) >= LCD_HEIGHT) ? (LCD_HEIGHT - y) / FONT_STEP_Y : maxRow;
}

void scr_setScrollOn(uint8_t flag)
{
	flagScroll = flag;
}

void scr_gotoxy(int8_t col, int8_t row)
{
	cur_col = col;
	cur_row = row;
}

int8_t scr_getCurRow(void)
{
	return cur_row;
}

void scr_fontColor(unsigned short fontC, unsigned short fontBkC)
{
	fontBkColor = fix_fontBkColor = fontBkC;
	fontColor = fix_fontColor = fontC;
}

void scr_fontColorInvers(void)
{
	fontBkColor = fix_fontColor;
	fontColor = fix_fontBkColor;
}

void scr_fontColorNormal(void)
{
	fontColor = fix_fontColor;
	fontBkColor = fix_fontBkColor;
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	uint16_t x, y;
	uint16_t dx;
	uint16_t dy;

	if (y0 == y1)
	{
		if (x0 <= x1)	{ x = x0; }
		else			{ x = x1; x1 = x0; }
		while (x <= x1)
			LCD_SetPoint(x++, y0, color);
		return;
	}
	else if (y0 > y1)
		dy = y0 - y1;
	else
		dy = y1 - y0;

	if (x0 == x1)
	{
		if (y0 <= y1)	{ y = y0; }
		else			{ y = y1; y1 = y0; }
		while (y <= y1)
			LCD_SetPoint(x0, y++, color);
		return;
	}
	else if (x0 > x1)
	{
		dx = x0 - x1;
		x = x1; x1 = x0;
		y = y1; y1 = y0;
	}
	else
	{
		dx = x1 - x0;
		x = x0; y = y0;
	}

	if (dx == dy)
	{
		while (x <= x1)
		{
			if (y > y1)	y--;
			else		y++;
			LCD_SetPoint(x++, y, color);
		}
	}
	else
	{
		LCD_SetPoint(x, y, color);
		if (y < y1)
		{
			if (dx > dy)
			{
				s16 p = dy * 2 - dx;
				s16 twoDy = 2 * dy;
				s16 twoDyMinusDx = 2 * (dy - dx);
				while (x < x1)
				{
					if (p < 0)
						p += twoDy;
					else
					{
						y++;
						p += twoDyMinusDx;
					}
					LCD_SetPoint(x++, y, color);
				}
			}
			else
			{
				s16 p = dx * 2 - dy;
				s16 twoDx = 2 * dx;
				s16 twoDxMinusDy = 2 * (dx - dy);
				while (y < y1)
				{
					if (p < 0)
						p += twoDx;
					else
					{
						x++;
						p += twoDxMinusDy;
					}
					LCD_SetPoint(x, y++, color);
				}
			}
		}
		else
		{
			if (dx > dy)
			{
				s16 p = dy * 2 - dx;
				s16 twoDy = 2 * dy;
				s16 twoDyMinusDx = 2 * (dy - dx);
				while (x < x1)
				{
					if (p < 0)	{ p += twoDy; }
					else		{ y--; p += twoDyMinusDx; }
					LCD_SetPoint(++x, y, color);
				}
			}
			else
			{
				s16 p = dx * 2 - dy;
				s16 twoDx = 2 * dx;
				s16 twoDxMinusDy = 2 * (dx - dy);
				while (y1 < y)
				{
					if (p < 0)	{ p += twoDx; }
					else		{ x++; p += twoDxMinusDy; }
					LCD_SetPoint(x, --y, color);
				}
			}
		}
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_Rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t fill)
{
	uint16_t h = y1 - y0;
	if (fill)
	{
		for (h++; x0 <= x1; x0++)
			LCD_VLine(x0, y0, h, color);
		return;
	}
	LCD_VLine(x0, y0, h, color);
	scr_Line(x0, y0, x1, y0, color);
	scr_Line(x0, y1, x1, y1, color);
	LCD_VLine(x1, y0, h, color);
}

/****************************************************************************
 *
 ****************************************************************************/
void  scr_Square(uint16_t x0, uint16_t y0, uint16_t with, uint16_t color, uint8_t fill)
{
	scr_Rectangle(x0, y0, x0 + with, y0 + with, color, fill);
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_putc(char c)
{
	if (cur_col >= cur_width && flagScroll)
	{
		cur_col = 0;
		cur_row++;
	}

	if (cur_row >= cur_height && flagScroll)
		cur_row = 0;

	switch (c)
	{
	case '\t':
		cur_col = ((cur_col + 8) >> 3) << 3;
		break;
	case '\n':
		cur_row++;
		cur_col = 0;
		break;
	case '\r':
		break;
	default:
		if (cur_col >= 0 && cur_col < cur_width && cur_row >= 0 && cur_row < cur_height)
			LCD_PutChar(
				(uint16_t)cur_col * FONT_STEP_X + cur_x_ofs,
				(uint16_t)cur_row * FONT_STEP_Y + cur_y_ofs,
				c,
				fontColor,
				fontBkColor
			);
		cur_col++;
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_puts(const char* str)
{
	while (*str)
		scr_putc(*str++);
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_clrEndl(void)
{
	while (cur_col < cur_width)
		scr_putc(' ');
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_itoa(long val, int radix, int len)
{
	uint8_t c, r, sgn = 0, pad = ' ';
	uint8_t s[20], i = 0;
	unsigned long v;

	if (radix < 0)
	{
		radix = -radix;
		if (val < 0)
		{
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0)
	{
		len = -len;
		pad = '0';
	}
	if (len > 20)
		return;
	do
	{
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);

	if (sgn) s[i++] = sgn;
	while (i < len)	s[i++] = pad;
	do
		scr_putc(s[--i]);
	while (i);
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_ftoa(double val, int len)
{
	if (val < 0)
	{
		scr_putc('-');
		val = -val;
	}
	long n = (long)val;
	scr_itoa(n, 10, len);
	scr_putc('.');
	n = (int)(val * 100.0) % 100;
	scr_itoa(n, 10, -2);
}

/****************************************************************************
 *
 ****************************************************************************/
void scr_printf(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;
	va_start(arp, str);

	while ((d = *str++) != 0)
	{
		if (d != '%')
		{
			scr_putc(d);
			continue;
		}
		d = *str++;
		w = r = s = l = 0;
		if (d == '0')
		{
			d = *str++;
			s = 1;
		}
		while ((d >= '0') && (d <= '9'))
		{
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l')
		{
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's')
		{
			scr_puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c')
		{
			scr_putc((char)va_arg(arp, int));
			continue;
		}
		if (d == 'f')
		{
			scr_ftoa((double)va_arg(arp, double), w);
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (!r) break;
		if (l)
		{
			scr_itoa((long)va_arg(arp, long), r, w);
		}
		else
		{
			if (r > 0)
				scr_itoa((unsigned long)va_arg(arp, int), r, w);
			else
				scr_itoa((long)va_arg(arp, int), r, w);
		}
	}
	va_end(arp);
}

/****************************************************************************
 *
 ****************************************************************************/
const char hexAlpha[] = "0123456789ABCDEF";
void scr_dump(const uint8_t *buff, int sz)
{
	int i;
	for (i = 0; i < sz; i++)
	{
		scr_putc(hexAlpha[(buff[i] >> 4) & 0x0F]);
		scr_putc(hexAlpha[buff[i] & 0x0F]);
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void win_showWindow(
	uint16_t x0, uint16_t y0,
	uint8_t colSz, uint8_t rowSz,
	uint16_t cBorder,
	uint16_t cFill,
	uint16_t cText
	)
{
	uint16_t x1 = x0 + (uint16_t)colSz * FONT_STEP_X + 10;
	uint16_t y1 = y0 + (uint16_t)rowSz * FONT_STEP_Y + 10;
	scr_Rectangle(x0, y0, x1, y1, cBorder, false);
	scr_Rectangle(x0 + 1, y0 + 1, x1 - 1, y1 - 1, cBorder, false);
	scr_Rectangle(x0 + 2, y0 + 2, x1 - 2, y1 - 2, cBorder, false);
	scr_Rectangle(x0 + 3, y0 + 3, x1 - 3, y1 - 3, cFill, true);
	scr_setTextWindow(
		x0 + 5,
		y0 + 5,
		(x1 - x0 - 10) / FONT_STEP_X,
		(y1 - y0 - 10) / FONT_STEP_Y
	);
	scr_fontColor(cText, cFill);
}

/****************************************************************************
 *
 ****************************************************************************/
void win_showErrorWin(void)
{
	win_showWindow(
		FONT_STEP_X * 4 - 5,
		FONT_STEP_Y * 4 - 5,
		32, 7,
		Yellow, Red, White
	);
	scr_gotoxy(0, 0);
	scr_setScrollOn(true);
}

/****************************************************************************
 *
 ****************************************************************************/
void win_showMsgWin(void)
{
	win_showWindow(
		FONT_STEP_X * 4 - 5,
		FONT_STEP_Y * 4 - 5,
		32, 8,
		Magenta, Blue2, Cyan
	);
	scr_gotoxy(0, 0);
	scr_setScrollOn(true);
}

/****************************************************************************
 *
 ****************************************************************************/
void win_showProgressBar(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint8_t p)
{
	uint16_t n = (dx) * p / 100;
	scr_Rectangle(x, y, x + dx, y + dy, Red, false);
	scr_Rectangle(x + 1, y + 1, x + dx - 1, y + dy - 1, Red, false);
	scr_Rectangle(x + 2 + n - 1, y + 2, x + dx - 2, y + dy - 2, Black, true);
	scr_Rectangle(x + 2, y + 2, x + n + 1, y + dy - 2, Yellow, true);
}

/****************************************************************************
 *
 ****************************************************************************/
void win_showMenu(uint16_t x, uint16_t y, uint8_t col, uint8_t row)
{
	win_showWindow(x, y, col, row, Green, Cyan, Black);
	scr_gotoxy(0, 0);
	scr_setScrollOn(false);
}

/****************************************************************************
 *
 ***************************************************************************/
void win_showMenuScroll(
	uint16_t x, uint16_t y,
	uint8_t col, uint8_t row,
	uint8_t startPos, uint8_t selPos, uint8_t lines
	)
{
	uint16_t x1 = x + (uint16_t)col * FONT_STEP_X + 10;
	uint16_t y1 = y + (uint16_t)row * FONT_STEP_Y + 10;
	uint16_t h = (y1 - y) - 4;
	uint16_t n = row > lines ? h : h * row / lines;
	uint16_t ofs = row > lines ? 0 : h * startPos / lines;
	win_showWindow(x, y, col, row, White, Blue, Yellow);
	scr_Rectangle(x1, y, x1 + 7, y1, White, true);
	scr_Rectangle(x1 - 1, y + ofs + 2, x1 + 4, y + ofs + n, Blue2, true);

	scr_setTextWindow(x + 5, y + 5, col, row);
	scr_gotoxy(0, -(int8_t)startPos);
	scr_setScrollOn(false);
}

#endif
