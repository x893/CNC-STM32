/*
 * scan.h
 *      Author: Mm
 */
/*
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "global.h"

static char scanFileName[40];

void initSensor(void) {
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_InitStructure.GPIO_Pin = SENSOR_PIN; GPIO_Init(SENSOR_PORT, &GPIO_InitStructure);
}

static struct {
 struct {
 	uint16_t n;
 	int16_t x, y, z;
 } list[64];
} scan_blk;

static uint8_t saveBlk(FIL *f) {
 UINT bw;
	FRESULT fres;
 if((fres = f_write(f, &scan_blk, sizeof(scan_blk), &bw)) != FR_OK || bw < sizeof(scan_blk)) {
	 f_close(f);
	 win_showErrorWin();
	 scr_printf("Error:'%s'", scanFileName);
	 if(fres != FR_OK) scr_printf("\n Status:%d [%d/%d]", fres, SD_errno, SD_step);
	 else scr_printf("\n Disk full (?) [%d/%d]", SD_errno, SD_step);
	 scr_puts("\n\n\t PRESS C-KEY");
 	while(kbd_getKey() != KEY_C);
 	return FALSE;
 }
 return TRUE;
}

static void scanPause(void) {
//	stepm_stopAll();
	scr_fontColor(Black,White); scr_gotoxy(1,13); scr_puts(" PAUSE.. press 'B' for continue"); scr_clrEndl();
	while(kbd_getKey() != KEY_B);
	scr_fontColor(White, Black); scr_gotoxy(1,13); scr_clrEndl();
}

static void scanShowHd(void) {
	ili9320_Clear(0);
	scr_fontColor(White,Black);	scr_gotoxy(1,3); scr_printf("Scan:'%s'",scanFileName);
 scr_fontColor(Blue,Black); scr_gotoxy(3,14); scr_puts("'C' - cancel  'A' - pause");
}

static uint8_t getShowSensorTouch(void) {
	uint8_t s = sensorTouch();
	GUI_Rectangle(8, 0, 16, 16, s? Red:Green, TRUE);
	return s;
}

static void strApp2D(char *str, int v) {
	int n = strlen(str);
	str[n++] = (v/10)%10+'0';
	str[n++] = v%10+'0';
	str[n] = 0;
}
static void scan(int k) {
	FIL f;
	FRESULT fres;
	uint32_t cnt = 0;
	int32_t sx = stepm_getCurGlobalStepsNum(0), sy = stepm_getCurGlobalStepsNum(1);
	int32_t x, y, z;
 uint8_t x_dir = sx < 0;
 uint16_t cntx = 0, cnty = 0;
 int32_t min_z = INT_MAX, max_z = INT_MIN, k_scr = SM_Z_STEPS_PER_MM;
 RTC_t rtc;
 rtc_gettime(&rtc);
 strcpy(scanFileName, "s"); strApp2D(scanFileName, rtc.mday);
 strApp2D(scanFileName, rtc.month); strApp2D(scanFileName, rtc.year-2000);
 strcat(scanFileName, "_");
 strApp2D(scanFileName, rtc.hour); strApp2D(scanFileName, rtc.min);strApp2D(scanFileName, rtc.sec);

 if((fres = f_open(&f, scanFileName, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK) {
		win_showErrorWin();
		scr_printf("Error open file:'%s'\n Status:%d [%d/%d]", scanFileName, fres, SD_errno, SD_step);
		scr_puts("\n\n\t PRESS C-KEY");
		while(kbd_getKey() != KEY_C);
		return;
 }
 scanShowHd();
	memset(&scan_blk, 0, sizeof(scan_blk));
 while(TRUE) {
 	stepm_addMove(0, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, x_dir);
 	while(stepm_isInProc(0)) {
 		switch(kbd_getKey()) {
 		 case KEY_C:	f_close(&f); return;
 		 case KEY_A: scanPause();	break;
 		}
 		if(!stepm_isInProc(2)) stepm_addMove(2, 1,	SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, !getShowSensorTouch());
 	}
 	uint8_t sensSt = getShowSensorTouch();
		stepm_addMove(2, SM_Z_STEPS_PER_MM*MAX_TABLE_SIZE_Z/4,
            				SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, !sensSt);
 	while(sensSt != getShowSensorTouch()) {
 		if(!stepm_isInProc(2)) {
 			win_showErrorWin();
 			scr_puts("SENSOR ERROR\n\n\t C- CANCEL  D-CONTINUE");
 			switch(kbd_getKey()) {
 			 case KEY_C:	f_close(&f); return;
 			 case KEY_D:
 			 	sensSt = getShowSensorTouch();
 			 	stepm_addMove(2, SM_Z_STEPS_PER_MM*MAX_TABLE_SIZE_Z/4,	SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, !sensSt);
 			 	scanShowHd();
 			}
 		}
 		switch(kbd_getKey()) {
 		 case KEY_C:	f_close(&f); return;
 		 case KEY_A: scanPause();	break;
 		}
 	}
 	//stepm_stop(2); z = stepm_getCurGlobalStepsNum(2);
 	x = stepm_getCurGlobalStepsNum(0); y = stepm_getCurGlobalStepsNum(1);
 	int scr_x = ((x < 0)? x:-x)/k;
 	int scr_z = z/k_scr;
  GUI_Line(scr_x, 237, scr_x, 237-scr_z, calcColor((uint8_t)cnty & 0x1F));
  if(max_z < z) max_z = z;
  if(min_z > z) min_z = z;

 	int p = cnt%64;
 	scan_blk.list[p].n = cntx++;	scan_blk.list[p].x = (uint16_t)x;
 	scan_blk.list[p].y = (uint16_t)y; scan_blk.list[p].z = (uint16_t)z;
 	cnt++;
 	if(cnt%64 == 0) {
 		if(!saveBlk(&f)) return;
 		memset(&scan_blk, 0, sizeof(scan_blk));
 	}
 	if((sx < 0 && (x >=0 || x <= sx)) || (sx > 0 && (x >= sx || x <= 0))) {
 		stepm_addMove(1, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, sy < 0);
 		x_dir = !x_dir; cntx = 0; cnty++;
  	while(stepm_isInProc(1));
 		y = stepm_getCurGlobalStepsNum(1);
 		if((sy < 0 && (y >= 0 || y <= sy)) || (sy > 0 && (y >= sy || y <= 0)))
 			break;
 		k_scr = (max_z-min_z)/200;
 	}
 }
 if(saveBlk(&f)) f_close(&f);
}

void scanMode(void) {
	static const char axisName[4] = "XYZE";
	static const double axisK[4] = {SM_X_STEPS_PER_MM,SM_Y_STEPS_PER_MM, SM_Z_STEPS_PER_MM, SM_E_STEPS_PER_MM};
	int i, k = SM_X_STEPS_PER_MM;

	ili9320_Clear(0); scr_setfullTextWindow();
	scr_gotoxy(10,0); scr_fontColor(Green,Black);
	scr_puts("CNC SCAN MODE");
	scr_fontColor(Yellow,Black);
	scr_puts("\n  key '6' - [X+]  key '4' - [X-] ");
	scr_puts("\n  key '2' - [Y+]  key '8' - [Y-] ");
	scr_puts("\n  key '1' - [Z+]  key '7' - [Z-] ");
	scr_puts("\n  'A'k+1 'B'k-1 '*'k=1 '#'k++");
	scr_puts("\n  'D' - start scan   C - exit");
	scr_puts("\n   0 - set end point for scan");
	scr_fontColor(White,Black);
	delayMs(700);
	while(kbd_getKey()>=0);
	while(TRUE) {
		getShowSensorTouch();
		for(i = 0; i < 3; i++) {
			double n = (double)stepm_getCurGlobalStepsNum(i)/axisK[i];
			scr_gotoxy(1+i*10,TEXT_Y_MAX-1); scr_printf("a%c:%f ", axisName[i], n);
			scr_gotoxy(1+i*10,TEXT_Y_MAX-2);
			scr_printf("n%c:%d  ", axisName[i], stepm_getRemSteps(i));
		}
		scr_gotoxy(2,TEXT_Y_MAX-3); scr_printf("step per key press: %f mm ", (double)k/SM_X_STEPS_PER_MM);
		switch(kbd_getKey()) {
		 case KEY_0:
		  while(stepm_isAnyInProc());
		  stepm_ZeroGlobalCrd();
		 	break;
		 case KEY_6: stepm_addMove(0, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 1); break;
		 case KEY_4: stepm_addMove(0, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 0); break;
		 case KEY_2: stepm_addMove(1, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 1); break;
		 case KEY_8: stepm_addMove(1, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 0); break;
		 case KEY_1: stepm_addMove(2, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 1); break;
		 case KEY_7: stepm_addMove(2, k, SM_MANUAL_MODE_STEPS_PER_SEC*K_FRQ, 0); break;
		 case KEY_A:
		 	if(k < 100) k++;
		 	break;
		 case KEY_B:
		 	if(k > 1) k--;
		 	break;
		 case KEY_STAR:
		 	k = 1;
		 	break;
		 case KEY_DIES:
		 	if(k < SM_X_STEPS_PER_MM) k+=SM_X_STEPS_PER_MM/8;
		 	break;
		 case KEY_C:
		 	stepm_EmergeStop();
		 	return;
		 case KEY_D:
		 	scan(k);
		 	return;
		}
	}
}
	*/

void scanMode(void) {

}
