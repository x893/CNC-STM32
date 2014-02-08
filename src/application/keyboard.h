#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#if (USE_KEYBOARD == 1)

	#define FLASH_KEYS()	while (kbd_getKey() != -1)
	#define WAIT_KEY_C()	while (kbd_getKey() != KEY_C)
	#define WAIT_KEY_D()	while (kbd_getKey() != KEY_D)
	#define IS_KEY_C()		(kbd_getKey() == KEY_C)
	#define KEY_0 0x13
	#define KEY_1 0x30
	#define KEY_2 0x10
	#define KEY_3 0x40
	#define KEY_4 0x31
	#define KEY_5 0x11
	#define KEY_6 0x41
	#define KEY_7 0x32
	#define KEY_8 0x12
	#define KEY_9 0x42
	#define KEY_A 0x20
	#define KEY_B 0x21
	#define KEY_C 0x22
	#define KEY_D 0x23
	#define KEY_STAR 0x33
	#define KEY_DIES 0x43

	void kbd_init(void);
	void kbd_proc(void);
	int kbd_getKey(void);

#else

	#define FLASH_KEYS()
	#define WAIT_KEY_C()
	#define WAIT_KEY_D()
	#define IS_KEY_C()		(0)

	#define kbd_init()

#endif

#endif /* KEYBOARD_H_ */
