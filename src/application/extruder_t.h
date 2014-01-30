#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

void extrudT_init(void);
int16_t extrudT_getTemperatureReal(void);
int16_t extrudT_getTemperatureWait(void);
void extrudT_setTemperature(int16_t val);
uint8_t extrudT_isReady(void);
void extrudT_FunOff(void);
void extrudT_FunOn(void);

void extrudT_proc(void);
void extrudT_irq(void);

#endif /* TEMPERATURE_H_ */
