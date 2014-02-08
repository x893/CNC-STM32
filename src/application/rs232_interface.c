#include <string.h>
#include <stdarg.h>
#include "global.h"

#if (USE_RS232 == 1)

uint8_t bufferBytes[1024];
volatile uint32_t indexGet = 0, indexPut = 0, countBytes = 0;

void rs232_proc()
{
	if (USART_RX_READY())
	{
		USART_SEND(USART_READ());
	}
	if (USART_GetITStatus(RS232_USART, USART_IT_TXE) != RESET)
	{
		if (indexGet != indexPut)
		{
			USART_SendData(RS232_USART, (uint16_t)bufferBytes[indexGet]);
			indexGet++;
			countBytes--;
			if (indexGet >= sizeof(bufferBytes))
				indexGet = 0;
		}
		if (indexGet == indexPut)
			USART_ITConfig(RS232_USART, USART_IT_TXE, DISABLE);
	}
}

void rf_putc(char c)
{
	while (countBytes == (sizeof(bufferBytes) - 1)) {}
	bufferBytes[indexPut] = c;
	indexPut++;
	countBytes++;
	if (indexPut >= sizeof(bufferBytes))
		indexPut = 0;
	USART_ITConfig(RS232_USART, USART_IT_TXE, ENABLE);
}

void rf_puts(const char *str)
{
	while (*str != 0)
		rf_putc(*str++);
}

uint8_t rf_itoa(int32_t val, int radix, int len, char *sout, unsigned char ptr)
{
	uint8_t c, r, sgn = 0, pad = ' ';
	uint8_t s[20], i = 0;
	uint32_t v;

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
		return ptr;
	do
	{
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);

	if (sgn)
		s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
	sout[ptr++] = (s[--i]);
	while (i);
	return ptr;
}

void rf_printf(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;
	va_start(arp, str);
	char sout[256];
	unsigned char ptr = 0;

	while ((d = *str++) != 0)
	{
		if (d != '%')
		{
			sout[ptr++] = d;
			continue;
		}
		d = *str++; w = r = s = l = 0;
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
			char *s = va_arg(arp, char*);
			while (*s != 0)
			{
				sout[ptr++] = *s;
				s++;
			}
			continue;
		}
		if (d == 'c')
		{
			sout[ptr++] = (char)va_arg(arp, int);
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (!r) break;
		if (l)
		{
			ptr = rf_itoa((long)va_arg(arp, long), r, w, sout, ptr);
		}
		else
		{
			if (r > 0) ptr = rf_itoa((unsigned long)va_arg(arp, int), r, w, sout, ptr);
			else	ptr = rf_itoa((long)va_arg(arp, int), r, w, sout, ptr);
		}
	}
	va_end(arp);
	sout[ptr] = 0;
	rf_puts(sout);
}
#endif
