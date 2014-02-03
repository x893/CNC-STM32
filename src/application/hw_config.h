#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__

void SystemStartup(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Reset_Device(void);

#define BULK_MAX_PACKET_SIZE	64

#endif
