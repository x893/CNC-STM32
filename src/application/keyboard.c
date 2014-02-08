#include "global.h"

#if (USE_KEYBOARD == 1)

volatile uint8_t _keyBuffer[6], _keyPressedCounter = 0;

void kbd_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	PIN_SPEED_MID();
	PIN_OUTPUT_PP();
	PIN_SET_MODE(COL0_PORT, COL0_PIN);
	PIN_SET_MODE(COL1_PORT, COL1_PIN);
	PIN_SET_MODE(COL2_PORT, COL2_PIN);
	PIN_SET_MODE(COL3_PORT, COL3_PIN);

	PIN_INPUT_PD();
	PIN_SET_MODE(ROW0_PORT, ROW0_PIN);
	PIN_SET_MODE(ROW1_PORT, ROW1_PIN);
	PIN_SET_MODE(ROW2_PORT, ROW2_PIN);
	PIN_SET_MODE(ROW3_PORT, ROW3_PIN);
}

void kbd_putKey(uint8_t key)
{
	if (_keyPressedCounter >= sizeof(_keyBuffer))
		return;
	_keyBuffer[_keyPressedCounter++] = key;
}

void kbd_proc(void)
{
	static uint8_t scan_stage = 0;
	if (++scan_stage >= 8)
		scan_stage = 0;
	uint8_t row = scan_stage >> 1;

	if ((scan_stage & 0x01) == 0)
	{
		GPIO_ResetBits(COL0_PORT, COL0_PIN);
		GPIO_ResetBits(COL1_PORT, COL1_PIN);
		GPIO_ResetBits(COL2_PORT, COL2_PIN);
		GPIO_ResetBits(COL3_PORT, COL3_PIN);
		switch (row)
		{
		case 0: GPIO_SetBits(COL0_PORT, COL0_PIN); break;
		case 1: GPIO_SetBits(COL1_PORT, COL1_PIN); break;
		case 2: GPIO_SetBits(COL2_PORT, COL2_PIN); break;
		case 3: GPIO_SetBits(COL3_PORT, COL3_PIN); break;
		}
	}
	else
	{
		if (GPIO_ReadInputDataBit(ROW0_PORT, ROW0_PIN)) kbd_putKey(0x10 | row);
		if (GPIO_ReadInputDataBit(ROW1_PORT, ROW1_PIN)) kbd_putKey(0x20 | row);
		if (GPIO_ReadInputDataBit(ROW2_PORT, ROW2_PIN)) kbd_putKey(0x30 | row);
		if (GPIO_ReadInputDataBit(ROW3_PORT, ROW3_PIN)) kbd_putKey(0x40 | row);
	}
}

int kbd_getKey(void)
{
	return _keyPressedCounter == 0 ? -1 : _keyBuffer[--_keyPressedCounter];
}

#endif
