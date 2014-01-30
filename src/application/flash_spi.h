#ifndef FLASH_SPI_H_
#define FLASH_SPI_H_

#define FLASH_SBIT_BUSY 0x01 // 1 = Internal Write operation is in progress
#define FLASH_SBIT_WEL 0x02 // 1 = Device is memory Write enabled
#define FLASH_SBIT_AAI 0x40 // Auto Address Increment Programming status (1 = AAI programming mode/0 = Byte-Program mode)

void SST25_flashInit(void);
uint8_t SST25_flashGetSatus(void);
void SST25_flashEraseAll(void);
void SST25_flashWrEnable(void);
void SST25_flashWrByte(uint32_t addr, uint8_t data);
void SST25_flashRdBytes(uint32_t addr, uint8_t *buf, uint32_t sz);
uint16_t SST25_flashReadID(void);


#endif /* FLASH_SPI_H_ */

