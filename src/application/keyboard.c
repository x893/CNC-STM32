#include "global.h"


void kbd_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = COL0_PIN; GPIO_Init(COL0_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = COL1_PIN; GPIO_Init(COL1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = COL2_PIN; GPIO_Init(COL2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = COL3_PIN; GPIO_Init(COL3_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = ROW0_PIN; GPIO_Init(ROW0_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ROW1_PIN; GPIO_Init(ROW1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ROW2_PIN; GPIO_Init(ROW2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ROW3_PIN; GPIO_Init(ROW3_PORT, &GPIO_InitStructure);
}

volatile uint8_t _keyBuffer[6], _keyPressedCounter = 0;

static void kbd_putKey(uint8_t key)
{
	if (_keyPressedCounter >= sizeof(_keyBuffer)) return;
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
		COL0_PORT->BRR = COL0_PIN;
		COL1_PORT->BRR = COL1_PIN;
		COL2_PORT->BRR = COL2_PIN;
		COL3_PORT->BRR = COL3_PIN;
		switch (row)
		{
		case 0: COL0_PORT->BSRR = COL0_PIN; break;
		case 1: COL1_PORT->BSRR = COL1_PIN; break;
		case 2: COL2_PORT->BSRR = COL2_PIN; break;
		case 3: COL3_PORT->BSRR = COL3_PIN; break;
		}
	}
	else
	{
		if (ROW0_PORT->IDR & ROW0_PIN) kbd_putKey(0x10 | row);
		if (ROW1_PORT->IDR & ROW1_PIN) kbd_putKey(0x20 | row);
		if (ROW2_PORT->IDR & ROW2_PIN) kbd_putKey(0x30 | row);
		if (ROW3_PORT->IDR & ROW3_PIN) kbd_putKey(0x40 | row);
	}
	//	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET) kbd_putKey(0x07F); // key on main board
}

int kbd_getKey(void)
{
	return _keyPressedCounter == 0 ? -1 : _keyBuffer[--_keyPressedCounter];
}
