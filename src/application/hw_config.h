#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

void SystemStartup(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Reset_Device(void);

#define BULK_MAX_PACKET_SIZE	64

#endif  /*__HW_CONFIG_H*/
