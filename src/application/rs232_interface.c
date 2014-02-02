#include <string.h>
#include <stdarg.h>

#include "global.h"
#include "Crc32.h"

#ifdef HAS_RS232

uint8_t bufferBytes[1024];
volatile uint32_t indexGet, indexPut, countBytes;

void rs232_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RS232_USART_CLK();
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = RS232_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS232_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(RS232_USART_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS232_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(RS232_USART_RX_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(RS232_USART, &USART_InitStructure);
	USART_Cmd(RS232_USART, ENABLE);
	indexGet = indexPut = countBytes = 0;
	USART_ITConfig(RS232_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(RS232_USART, USART_IT_TXE, DISABLE);
}

void rs232_proc()
{
	if (USART_GetITStatus(RS232_USART, USART_IT_RXNE) != RESET)
	{
		USART_SendData(RS232_USART, USART_ReceiveData(RS232_USART));
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

