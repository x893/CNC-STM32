#ifndef RS232_INTERFACE_H_
#define RS232_INTERFACE_H_

#define STATE_INIT 0
#define STATE_HD0 1
#define STATE_HD1 2
#define STATE_FILE_NAME_SZ 3
#define STATE_FILE_NAME 4
#define STATE_FSIZE0 5
#define STATE_FSIZE1 6
#define STATE_FSIZE2 7
#define STATE_FSIZE3 8

#define STATE_CRC0 9
#define STATE_CRC1 10
#define STATE_CRC2 11
#define STATE_CRC3 12

#define STATE_SEND_SYNC 13
#define STATE_RCV_FILE 14
#define STATE_RCV_END 15

#define RCV_ERR_NO 0
#define RCV_ERR_HD0 -1
#define RCV_ERR_HD1 -2
#define RCV_WRONG_CRC -3
#define RCV_ERR_LOST_DATA -4

#ifdef HAS_RS232
	void rs232_init(void);
	void rf_putc(char c);
	void rf_puts(const char *str);
	void rf_printf(const char* str, ...);
#else
	#define rs232_init()
#endif

#endif /* RS232_INTERFACE_H_ */
