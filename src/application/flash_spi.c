/*
 * flash_spi.c
 *
 *  Created on: 01.12.2011
 *      Author: Mm
 */

#include "global.h"

#define CS_ON()		{ SST25_CS_PORT->BRR  = SST25_CS_PIN; }
#define CS_OFF()	{ SST25_CS_PORT->BSRR = SST25_CS_PIN; }

void SST25_flashInit(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI1 GPIOA clocks */
	SST25_SPI_CLK();
	SST25_PORT_CLK();

	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = SST25_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SST25_PORT, &GPIO_InitStructure);

	/* Configure PA.4 as Output push-pull, used as Flash Chip select */
	CS_OFF();
	GPIO_InitStructure.GPIO_Pin = SST25_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SST25_CS_PORT, &GPIO_InitStructure);

	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SST25_SPI, &SPI_InitStructure);

	SPI_Cmd(SST25_SPI, ENABLE);
}


static uint8_t SPI_WriteByte(uint8_t data)
{
	while (SPI_I2S_GetFlagStatus(SST25_SPI, SPI_I2S_FLAG_TXE) == RESET); //Wait until the transmit buffer is empty
	SPI_I2S_SendData(SST25_SPI, data); // Send the byte
	while (SPI_I2S_GetFlagStatus(SST25_SPI, SPI_I2S_FLAG_RXNE) == RESET); //Wait until a data is received
	return SPI_I2S_ReceiveData(SST25_SPI); // Get the received data
}


void SST25_flashEraseAll(void)
{
	CS_ON();
	SPI_WriteByte(0x50);
	CS_OFF(); __NOP(); __NOP(); __NOP();
	CS_ON();
	SPI_WriteByte(0x01);
	SPI_WriteByte(0x00);
	CS_OFF();
	SST25_flashWrEnable();
	while ((SST25_flashGetSatus() & FLASH_SBIT_BUSY) != 0);
	CS_ON();
	SPI_WriteByte(0xC7);
	CS_OFF();
}

#define FLASH_SBIT_BUSY 0x01 // 1 = Internal Write operation is in progress
#define FLASH_SBIT_WEL 0x02 // 1 = Device is memory Write enabled
#define FLASH_SBIT_AAI 0x40 // Auto Address Increment Programming status (1 = AAI programming mode/0 = Byte-Program mode)

uint8_t SST25_flashGetSatus(void) {
	CS_ON();
	SPI_WriteByte(0x05);
	uint8_t b = SPI_WriteByte(0);
	CS_OFF();
	return b;
}

void SST25_flashWrEnable(void) {
	CS_ON();
	SPI_WriteByte(0x06);
	CS_OFF();
}

void SST25_flashWrByte(uint32_t addr, uint8_t data)
{
	uint8_t st;
	while (((st = SST25_flashGetSatus()) & FLASH_SBIT_BUSY) != 0);
	if ((st & FLASH_SBIT_WEL) == 0) SST25_flashWrEnable();
	CS_ON();
	SPI_WriteByte(0x02);
	SPI_WriteByte((uint8_t)(addr >> 16));
	SPI_WriteByte((uint8_t)(addr >> 8));
	SPI_WriteByte((uint8_t)addr);
	SPI_WriteByte(data);
	CS_OFF();
}

void SST25_flashRdBytes(uint32_t addr, uint8_t *buf, uint32_t sz)
{
	while ((SST25_flashGetSatus() & FLASH_SBIT_BUSY) != 0);
	CS_ON();
	SPI_WriteByte(0x03);
	SPI_WriteByte((uint8_t)(addr >> 16));
	SPI_WriteByte((uint8_t)(addr >> 8));
	SPI_WriteByte((uint8_t)addr);
	for (uint16_t i = 0; i < sz; i++) buf[i] = SPI_WriteByte(0);
	CS_OFF();
}

uint16_t SST25_flashReadID(void)
{
	CS_ON();
	SPI_WriteByte(0x90);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);
	uint16_t id = SPI_WriteByte(0) << 8;	//BFH: SST
	id |= SPI_WriteByte(0);					//41H:SST25VF016B
	CS_OFF();
	return id;
}
