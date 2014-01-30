#include "global.h"
#include "ili9320.h"

#ifdef HAS_TOUCH

#define CH_X  0xD0	// 0x90
#define CH_Y  0x90	// 0xd0

#define TP_CS()  GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define TP_DCS() GPIO_SetBits(GPIOB, GPIO_Pin_7)

unsigned char isTouch_pen(void)
{
	return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
}

unsigned char SPI_WriteByte(unsigned char data)
{
	//Wait until the transmit buffer is empty
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	// Send the byte
	SPI_I2S_SendData(SPI1, data);
	//Wait until a data is received
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	// Get the received data
	return SPI_I2S_ReceiveData(SPI1);
}

void SpiDelay(unsigned int DelayCnt)
{
	unsigned int i;
	for (i = 0; i < DelayCnt; i++) __NOP();
}

static uint16_t TPReadX(void)
{
	uint16_t v;
	TP_CS();
	SpiDelay(10);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0xd0);
	SpiDelay(10);
	v = SPI_WriteByte(0x00);
	v <<= 8;
	v |= SPI_WriteByte(0x00);
	SpiDelay(10);
	TP_DCS();
	return v;
}

static uint16_t TPReadY(void)
{
	uint16_t v;
	TP_CS(); SpiDelay(10);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x90); SpiDelay(10);
	v = SPI_WriteByte(0x00); v <<= 8;
	v |= SPI_WriteByte(0x00); SpiDelay(10);
	TP_DCS();
	return v;
}

uint16_t Touch_MeasurementX(void)
{
	u8 i;
	u32 p = 0;
	for (i = 0; i < 8; i++) {
		p += TPReadX();
		SpiDelay(1000);
	}
	return (uint16_t)(p >>= 3);
}

unsigned short Touch_MeasurementY(void)
{
	u8 i;
	u32 p = 0;
	for (i = 0; i < 8; i++)
	{
		p += TPReadY();
		SpiDelay(1000);
	}
	return (uint16_t)(p >>= 3);
}

uint16_t Touch_ScrX(void)
{
	int adx = Touch_MeasurementX();
	uint16_t sx = 0;
	int r = adx - 2000;
	r *= 239;
	sx = r / (29000 - 2000);
	if (sx <= 0 || sx > 240)
		return 0;
	return sx;
}

uint16_t Touch_ScrY(void)
{
	int ady = Touch_MeasurementY();
	uint16_t sy = 0;
	int r = ady - 1500;
	r *= 319 + 15;
	sy = r / (30700 - 1500);
	if (sy <= 0 || sy > 340)
		return 0;
	return sy;
}

#endif
