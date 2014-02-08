#ifndef __TOUCH_H__
#define __TOUCH_H__

#ifdef USE_TOUCH

	uint16_t Touch_MeasurementX(void);
	uint16_t Touch_MeasurementY(void);

	// INT   (ADC touch)	- PE12
	uint8_t isTouch_pen(void);
	uint16_t Touch_ScrX(void);
	uint16_t Touch_ScrY(void);

#endif
#endif
