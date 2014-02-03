#include "ili9320.h"

#if (USE_LCD == 1)

/****************************************************************************
 *
 ****************************************************************************/
#define  GUI_CmpColor(color1, color2)	( (color1 & 0x01) == (color2 & 0x01) )

/****************************************************************************
 *
 ****************************************************************************/
uint16_t GUI_Color565(uint32_t RGB)
{
	uint8_t  r, g, b;

	b = (RGB >> (0 + 3)) & 0x1F;	// B
	g = (RGB >> (8 + 2)) & 0x3F;	// G
	r = (RGB >> (16 + 3)) & 0x1F;	// R

	return ((r << 11) + (g << 5) + (b << 0));
}

/****************************************************************************
 *
 ****************************************************************************/
void GUI_Text(uint16_t x, uint16_t y, const uint8_t *str, uint16_t len, uint16_t Color, uint16_t bkColor)
{
	uint8_t i;
	for (i = 0; i < len; i++)
	{
		ili9320_PutChar((x + 8 * i), y, *str++, Color, bkColor);
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	uint16_t x, y;
	uint16_t dx;// = abs(x1 - x0);
	uint16_t dy;// = abs(y1 - y0);

	if (y0 == y1)
	{
		if (x0 <= x1)
		{
			x = x0;
		}
		else
		{
			x = x1;
			x1 = x0;
		}
		while (x <= x1)
		{
			ili9320_SetPoint(x, y0, color);
			x++;
		}
		return;
	}
	else if (y0 > y1)
		dy = y0 - y1;
	else
		dy = y1 - y0;

	if (x0 == x1)
	{
		if (y0 <= y1)
			y = y0;
		else
		{
			y = y1;
			y1 = y0;
		}
		while (y <= y1)
			ili9320_SetPoint(x0, y++, color);

		return;
	}
	else if (x0 > x1)
	{
		dx = x0 - x1;
		x = x1;
		x1 = x0;
		y = y1;
		y1 = y0;
	}
	else
	{
		dx = x1 - x0;
		x = x0;
		y = y0;
	}

	if (dx == dy)
	{
		while (x <= x1)
		{
			if (y > y1)
				y--;
			else
				y++;
			ili9320_SetPoint(x++, y, color);
		}
	}
	else
	{
		ili9320_SetPoint(x, y, color);
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
					ili9320_SetPoint(x++, y, color);
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
					ili9320_SetPoint(x, y++, color);
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
					x++;
					if (p < 0)
					{
						p += twoDy;
					}
					else
					{
						y--;
						p += twoDyMinusDx;
					}
					ili9320_SetPoint(x, y, color);
				}
			}
			else
			{
				s16 p = dx * 2 - dy;
				s16 twoDx = 2 * dx;
				s16 twoDxMinusDy = 2 * (dx - dy);
				while (y1 < y)
				{
					y--;
					if (p < 0)
					{
						p += twoDx;
					}
					else
					{
						x++;
						p += twoDxMinusDy;
					}
					ili9320_SetPoint(x, y, color);
				}
			}
		}
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void GUI_Circle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color, uint8_t fill)
{
	uint16_t x, y;
	s16 delta, tmp;
	x = 0;
	y = r;
	delta = 3 - (r << 1);

	while (y > x)
	{
		if (fill)
		{
			GUI_Line(cx + x, cy + y, cx - x, cy + y, color);
			GUI_Line(cx + x, cy - y, cx - x, cy - y, color);
			GUI_Line(cx + y, cy + x, cx - y, cy + x, color);
			GUI_Line(cx + y, cy - x, cx - y, cy - x, color);
		}
		else
		{
			ili9320_SetPoint(cx + x, cy + y, color);
			ili9320_SetPoint(cx - x, cy + y, color);
			ili9320_SetPoint(cx + x, cy - y, color);
			ili9320_SetPoint(cx - x, cy - y, color);
			ili9320_SetPoint(cx + y, cy + x, color);
			ili9320_SetPoint(cx - y, cy + x, color);
			ili9320_SetPoint(cx + y, cy - x, color);
			ili9320_SetPoint(cx - y, cy - x, color);
		}
		x++;
		if (delta >= 0)
		{
			y--;
			tmp = (x << 2);
			tmp -= (y << 2);
			delta += (tmp + 10);
		}
		else
		{
			delta += ((x << 2) + 6);
		}
	}
}

/****************************************************************************
 *
 ****************************************************************************/
void GUI_Rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint8_t fill)
{
	uint16_t h = y1 - y0;
	if (fill)
	{
		for (h++; x0 <= x1; x0++)
			ili9320_VLine(x0, y0, h, color);
		return;
	}
	ili9320_VLine(x0, y0, h, color);
	GUI_Line(x0, y0, x1, y0, color);
	GUI_Line(x0, y1, x1, y1, color);
	ili9320_VLine(x1, y0, h, color);
}

/****************************************************************************
 *
 ****************************************************************************/
void  GUI_Square(uint16_t x0, uint16_t y0, uint16_t with, uint16_t color, uint8_t fill)
{
	GUI_Rectangle(x0, y0, x0 + with, y0 + with, color, fill);
}
#endif
