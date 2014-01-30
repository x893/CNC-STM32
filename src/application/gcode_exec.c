#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef _WINDOWS
#include "stdafx.h"
#else
#include "global.h"
#endif

#include "gcode.h"

#define ENABLE_SHOW_MAX_TIME_STEPS 640
#define MAX_STR_SIZE 150
//#define NO_ACCELERATION_CORRECTION

SM_PARAM _smParam;
uint8_t isPause = FALSE;

double k_scr;
short prev_scrX, prev_scrY;

int curGCodeMode;
uint32_t commonTimeIdeal, commonTimeReal, startWorkTime;
uint8_t isGcodeStop;
#ifdef HAS_EXTRUDER
uint8_t isExtruderOn;
#endif
double minX, maxX, minY, maxY, minZ, maxZ;

#define CRD_X 0
#define CRD_Y 1
#define CRD_Z 2
#define CRD_E 3

#ifndef NO_ACCELERATION_CORRECTION
typedef struct {
	uint32_t steps[4], frq[4];
	uint8_t isNullCmd, dir[4], changeDir;
	double length;
	int32_t cos_a;
	uint32_t feed_rate;
} MVECTOR;
#endif

#define MAX_SHOW_GCODE_LINES 2
typedef struct {
	char cmd[MAX_STR_SIZE];
	int lineNum;
} GCODE_CMD;

#define MVECTOR_SZ 3
#define TABLE_CENTER_X MAX_TABLE_SIZE_X/4
#define TABLE_CENTER_Y MAX_TABLE_SIZE_Y/4

struct {
#ifndef NO_ACCELERATION_CORRECTION
	MVECTOR mvector[MVECTOR_SZ];
	int32_t mvectPtrCur, mvectCnt;
#endif
	GCODE_CMD gcode[MAX_SHOW_GCODE_LINES];
	int gcodePtrCur;
	int32_t stepsFromStartX, stepsFromStartY, stepsFromStartZ, stepsFromStartE;
	int32_t stepsX, stepsY, stepsZ;
} linesBuffer;

short crdXtoScr(double x) { return (short)(x*k_scr + 0.5) + 8; }
short crdYtoScr(double y) {
	short v = (short)(y*k_scr + 0.5);
	return v > 239 ? 0 : 239 - v;
}

void initGcodeProc(void)
{
	double kx = 310.0 / MAX_TABLE_SIZE_X;
	double ky = 238.0 / MAX_TABLE_SIZE_Y;
	k_scr = kx > ky ? ky : kx;

	ili9320_Clear(0);

	memset(&linesBuffer, 0, sizeof(linesBuffer));
#ifndef NO_ACCELERATION_CORRECTION
	linesBuffer.mvectPtrCur = 0;
	linesBuffer.mvector[0].isNullCmd = TRUE;
	linesBuffer.mvectCnt = 1;
#endif
	prev_scrX = crdXtoScr(TABLE_CENTER_X); prev_scrY = crdYtoScr(TABLE_CENTER_Y);
	linesBuffer.stepsFromStartX = linesBuffer.stepsFromStartY = linesBuffer.stepsFromStartZ = linesBuffer.stepsFromStartE = 0;
	linesBuffer.stepsX = linesBuffer.stepsY = linesBuffer.stepsZ = 0;
#ifdef HAS_EXTRUDER
	isExtruderOn = FALSE;
#endif
	minX = maxX = minY = maxY = minZ = maxZ = 0;
	gc_init();
	stepm_init();
	commonTimeIdeal = commonTimeReal = 0;
	isGcodeStop = FALSE;
	startWorkTime = RTC_GetCounter();
}

//---------------------------------------------------------------------

void cnc_gfile(char *fileName, int mode)
{
	int n;
	FIL fid;
	int lineNum;
	uint8_t hasMoreLines;

	initGcodeProc();
	FRESULT res = f_open(&fid, fileName, FA_READ);
	if (res != FR_OK)
	{
		win_showErrorWin();
		scr_printf("Error open file:'%s'\nStatus:%d [%d]", fileName, (int)res, SD_errno);
		return;
	}
	curGCodeMode = mode;

	if ((curGCodeMode & GFILE_MODE_MASK_SHOW) != 0)
	{
		GUI_Rectangle(crdXtoScr(0), crdYtoScr(MAX_TABLE_SIZE_Y), crdXtoScr(MAX_TABLE_SIZE_X), crdYtoScr(0), Red, FALSE);
		GUI_Line(prev_scrX, 30, prev_scrX, 240 - 30, Green);
		GUI_Line(50, prev_scrY, 320 - 50, prev_scrY, Green);
	}
	if ((curGCodeMode & GFILE_MODE_MASK_EXEC) != 0)
	{
		scr_fontColor(Blue, Black);
		scr_gotoxy(3, 14);
		scr_puts("C-Cancel  A-Pause 0/1-encoder");
	}
	lineNum = 1;
	hasMoreLines = TRUE;
	do
	{
		// char str[MAX_STR_SIZE];
		static char fileBuf[16000];
		//char fileBuf[MAX_STR_SIZE];
		char *p = fileBuf, *str;

		while (TRUE)
		{
			*p = 0;
			str = p + 1;
			if ((fileBuf + sizeof(fileBuf)-str) < (MAX_STR_SIZE + 1))
				break;
			if (f_gets(str, MAX_STR_SIZE, &fid) == NULL)
			{
				hasMoreLines = FALSE;
				break;
			}
			str_trim(str);
			*p = (uint8_t)strlen(str) + 1;
			p += *p + 1;
		}
		for (p = fileBuf; !isGcodeStop && *p != 0; lineNum++, p += *p + 1)
		{
			uint8_t st;

			str = p + 1;
			if ((curGCodeMode & GFILE_MODE_MASK_EXEC) != 0)
			{
				int i;
				GCODE_CMD *gp;

				linesBuffer.gcodePtrCur++;
				if (linesBuffer.gcodePtrCur > (MAX_SHOW_GCODE_LINES - 1))
					linesBuffer.gcodePtrCur = 0;
				gp = &linesBuffer.gcode[linesBuffer.gcodePtrCur];
				strcpy(gp->cmd, str); gp->lineNum = lineNum;
				scr_fontColor(Green, Black);
				//	if(stepm_getRemainLines() > 1) {
				for (i = 0, n = linesBuffer.gcodePtrCur + 1; i < MAX_SHOW_GCODE_LINES; i++, n++)
				{
					if (n >(MAX_SHOW_GCODE_LINES - 1))
						n = 0;
					gp = &linesBuffer.gcode[n];
					scr_gotoxy(1, i);
					if (gp->lineNum) scr_printf("%d:%s", gp->lineNum, gp->cmd);
					scr_clrEndl();
				}
				//	}
			}
			DBG("\n   [gcode:%d] %s", lineNum, str);
			st = gc_execute_line(str);
			if (st != GCSTATUS_OK)
			{
				scr_fontColor(Red, Black); scr_gotoxy(1, 12);
				switch (st)
				{
				case GCSTATUS_BAD_NUMBER_FORMAT:
					scr_puts("BAD_NUMBER_FORMAT");
					break;
				case GCSTATUS_EXPECTED_COMMAND_LETTER:
					scr_puts("EXPECTED_COMMAND_LETTER");
					break;
				case GCSTATUS_UNSUPPORTED_STATEMENT:
					scr_puts("UNSUPPORTED_STATEMENT");
					break;
				case GCSTATUS_FLOATING_POINT_ERROR:
					scr_puts("FLOATING_POINT_ERROR");
					break;
				case GCSTATUS_UNSUPPORTED_PARAM:
					scr_puts("UNSUPPORTED_PARAM");
					break;
				case GCSTATUS_UNSOPORTED_FEEDRATE:
					scr_puts("GCSTATUS_UNSOPORTED_FEEDRATE");
					break;
				case GCSTATUS_TABLE_SIZE_OVER_X:
					scr_puts("GCSTATUS_TABLE_SIZE_OVER_X");
					break;
				case GCSTATUS_TABLE_SIZE_OVER_Y:
					scr_puts("GCSTATUS_TABLE_SIZE_OVER_Y");
					break;
				case GCSTATUS_TABLE_SIZE_OVER_Z:
					scr_puts("GCSTATUS_TABLE_SIZE_OVER_Z");
					break;
				case GCSTATUS_CANCELED:
					scr_puts("GCSTATUS_CANCELED");
					break;
				}
				scr_printf(" in line [%d]:\n '%s'", lineNum, str);
				f_close(&fid); return;
			}
		}
	} while (!isGcodeStop && hasMoreLines);

	f_close(&fid);

	if ((curGCodeMode & GFILE_MODE_MASK_EXEC) == 0)
	{
		short scrX = crdXtoScr(TABLE_CENTER_X);
		short scrY = crdYtoScr(TABLE_CENTER_Y);
		int t1 = commonTimeIdeal / 1000;
		int t2 = commonTimeReal / 1000;

		GUI_Line(scrX - 8, scrY, scrX + 8, scrY, Red);
		GUI_Line(scrX, scrY - 8, scrX, scrY + 8, Red);
		scr_fontColor(Green, Black); scr_gotoxy(1, 0);
		scr_printf("Time %02d:%02d:%02d(%02d:%02d:%02d) N.cmd:%d",
			t1 / 3600, (t1 / 60) % 60, t1 % 60,
			t2 / 3600, (t2 / 60) % 60, t2 % 60,
			lineNum
			);
		scr_printf("\n X%f/%f Y%f/%f Z%f/%f", minX, maxX, minY, maxY, minZ, maxZ);
	}
	else
	{
#ifndef NO_ACCELERATION_CORRECTION
		cnc_line(0, 0, 0, 0, 0, 0);
		cnc_line(0, 0, 0, 0, 0, 0);
#endif
	}
}

uint16_t calcColor(uint8_t val)
{
	if (val < 12) return (uint16_t)0xFFFF - (val << 6);
	if (val < 20) return (uint16_t)0xF81F - ((val - 12) << 12);
	if (val < 32) return (uint16_t)0x047F + ((val - 20) << 6);
	if (val < 40) return (uint16_t)0x07FF - ((val - 32) << 2);
	if (val < 48) return (uint16_t)0x1F70 + ((val - 40) << 13);
	if (val < 64) return (uint16_t)0xFF00 - ((val - 48) << 6);
	return 0x80F0;
}

void cnc_dwell(int pause)
{
	commonTimeIdeal += pause;
	commonTimeReal += pause;
}
//=================================================================================================================================

extern const char axisName[5];
const double axisK[4] = {
	SM_X_STEPS_PER_MM,
	SM_Y_STEPS_PER_MM,
	SM_Z_STEPS_PER_MM,
	SM_E_STEPS_PER_MM
};
uint8_t cnc_waitSMotorReady(void)
{
	static uint32_t time = 0;
	static uint8_t isStepDump = FALSE;
	int i;

	do
	{
		if (stepm_getRemainLines() > 1)
		{
			scr_fontColor(Yellow, Black);
			for (i = 0; i < 3; i++)
			{
				int32_t globalSteps = stepm_getCurGlobalStepsNum(i);
				double n = (double)globalSteps / axisK[i];
				scr_gotoxy(1 + i * 10, 3); scr_printf("%c:%f ", axisName[i], n);
#ifdef HAS_ENCODER
				if (i == 2)
				{
					int32_t enVal = encoderZvalue();
					double encValmm = (double)(enVal*MM_PER_360) / ENCODER_Z_CNT_PER_360;
					scr_gotoxy(1 + 2 * 10, 4); scr_printf("errZ:%f  ", encValmm - n);
					if (isEncoderCorrection)
					{
						scr_gotoxy(1, 6);	scr_printf("dZ:%d[%d]    ", encoderCorrectionDelta, encoderCorrectionMaxDelta);
						scr_gotoxy(15, 6); scr_printf("Up:%d Dn:%d  ", encoderCorrectionCntUp, encoderCorrectionCntDown);
					}
				}
#endif
			}
			if (isStepDump)
				step_dump();
			if (time != RTC_GetCounter())
			{
				uint32_t t;
				//scr_gotoxy(2,12);
				//scr_fontColor(_smParam.maxSpindleTemperature >  extrudT_getTemperatureReal()? Green:Red,Black);
				//scr_printf("spindle t:%dC  ", extrudT_getTemperatureReal());
				time = RTC_GetCounter();
				t = time - startWorkTime;
				scr_gotoxy(30, 12); scr_fontColor(Cyan, Black);
				scr_printf("%02d:%02d:%02d", t / 3600, (t / 60) % 60, t % 60);
			}
		}
		switch (kbd_getKey())
		{
		case KEY_C:
			stepm_EmergeStop();
			return FALSE;
		case KEY_A:
			isPause = TRUE;
			break;
		case KEY_0:
			isEncoderCorrection = FALSE;
			scr_gotoxy(1, 6);
			scr_clrEndl();
			break;
		case KEY_1:
			isEncoderCorrection = TRUE;
			break;
		case KEY_7:
			isStepDump = TRUE;
			break;
		case KEY_8:
			isStepDump = FALSE;
			break;
		}
	} while (stepm_LinesBufferIsFull());
	if (limits_chk())
	{
		stepm_EmergeStop();
		scr_fontColor(Red, Black);	scr_gotoxy(7, 11); scr_puts("LIMITS ERROR!"); scr_clrEndl();
		return FALSE;
	}
	return TRUE;
}

uint8_t sendLine(uint32_t fxyze[], uint32_t abs_dxyze[], uint8_t dir_xyze[])
{
	uint32_t f = 0;
	uint32_t i, n = 0;

	for (i = 0; i < 3; i++)
	{
		if (fxyze[i] > f)
		{
			f = fxyze[i];
			n = i;
		}
	}
	// if(abs_dxyze[n] > 20) {
	//  for(i = 0; i < 4; i++) fxyze[i] = (uint32_t)((uint64_t)f * (uint64_t)abs_dxyze[i]/abs_dxyze[n]);
	if (abs_dxyze[n] > 10)
	{
		for (i = 0; i < 4; i++)
			if (i != n)
			{
				fxyze[i] = (uint32_t)(((uint64_t)f * (uint64_t)abs_dxyze[i]) / abs_dxyze[n]) + 1; //+1 for debug
			}
	}
	if ((curGCodeMode & GFILE_MODE_MASK_EXEC) == 0)
	{
		int i;
		for (i = 0; i < 4; i++)
			if (fxyze[i] != 0)
			{
				commonTimeReal += abs_dxyze[i] * 1000L * K_FRQ / fxyze[i];
				break;
			}

		if ((curGCodeMode & GFILE_MODE_MASK_SHOW) != 0)
		{
			double x, y;
			short scrX, scrY;

			if (dir_xyze[0])	linesBuffer.stepsX += abs_dxyze[0];
			else				linesBuffer.stepsX -= abs_dxyze[0];

			if (dir_xyze[1])	linesBuffer.stepsY += abs_dxyze[1];
			else				linesBuffer.stepsY -= abs_dxyze[1];

			if (dir_xyze[2])	linesBuffer.stepsZ += abs_dxyze[2];
			else				linesBuffer.stepsZ -= abs_dxyze[2];

			x = (double)linesBuffer.stepsX / SM_X_STEPS_PER_MM;
			y = (double)linesBuffer.stepsY / SM_Y_STEPS_PER_MM;

			scrX = crdXtoScr(TABLE_CENTER_X + x);
			scrY = crdYtoScr(TABLE_CENTER_Y + y);

			if (scrX != prev_scrX || prev_scrY != scrY)
				GUI_Line(prev_scrX, prev_scrY, scrX, scrY, calcColor((uint8_t)(linesBuffer.stepsZ * 5 / SM_X_STEPS_PER_MM) & 0x1F));
			prev_scrX = scrX;
			prev_scrY = scrY;
		}
		return TRUE;
	}
#ifdef DEBUG_MODE
	DBG("\n\tsendLine dx:%c%d(%d)\tdy:%c%d(%d)\tdz:%c%d(%d) tz:%d ty:%d tz:%d",
		dir_xyze[0] ? '+' : '-', abs_dxyze[0], fxyze[0],
		dir_xyze[1] ? '+' : '-', abs_dxyze[1], fxyze[1],
		dir_xyze[2] ? '+' : '-', abs_dxyze[2], fxyze[2],
		fxyze[0] == 0 ? 0 : abs_dxyze[0] * 1000L * K_FRQ / fxyze[0],
		fxyze[1] == 0 ? 0 : abs_dxyze[1] * 1000L * K_FRQ / fxyze[1],
		fxyze[2] == 0 ? 0 : abs_dxyze[2] * 1000L * K_FRQ / fxyze[2]);
	{
		static uint32_t prevF[3] = { 0, 0, 0 };
		static uint8_t prevDir[3] = { 0, 0, 0 };
		int i;

		for (i = 0; i < 3; i++)
		{
			double p;
			if ((p = labs(prevF[i] - fxyze[i]) / (double)_smParam.smoothStartF_from0[i]) > 1.5)
			{
				p *= 100;
				DBG(" !!!!!! <%d:%d/%d\t%f> %s", i, labs(prevF[i] - fxyze[i]), _smParam.smoothStartF_from0[i], p, p > 250 ? "$$$" : "");
			}
			if (prevDir[i] != dir_xyze[i] && (p = fxyze[i] / (double)_smParam.smoothStartF_from0[i]) > 1.5)
			{
				p *= 100;
				DBG(" !!!!!! ## <%d:%d/%d\t%f> %s", i, labs(prevF[i] - fxyze[i]), _smParam.smoothStartF_from0[i], p, p > 250 ? "$$$" : "");
			}
			if (fxyze[i] > _smParam.maxFeedRate[i])
			{
				DBG(" !!!!!! <%d:%d/%d too hight feedrate!> ", i, fxyze[i], _smParam.maxFeedRate[i]);
			}
			if (fxyze[i] < (50 * SM_X_STEPS_PER_MM / 60) && abs_dxyze[i] > 120)
			{
				DBG(" !!!!!! <%d:%d too slow feedrate!> ", i, fxyze[i]);
			}
		}
		for (i = 0; i < 3; i++)
		{
			prevF[i] = fxyze[i];
			prevDir[i] = dir_xyze[i];
		}
	}
#endif

	if (!cnc_waitSMotorReady()) return FALSE;
	if (isPause)
	{
		scr_fontColor(Black, White);
		scr_gotoxy(1, 13);
		scr_puts(" PAUSE..'B'-continue 'C'-cancel");
		scr_clrEndl();
		while (stepm_inProc()) {}
		stepm_EmergeStop();
		while (isPause)
		{
			switch (kbd_getKey())
			{
			case KEY_C: return FALSE;
			case KEY_B: isPause = FALSE;
			}
		}
		scr_fontColor(White, Black);
		scr_gotoxy(1, 13);
		scr_clrEndl();
	}
	stepm_addMove(abs_dxyze, fxyze, dir_xyze);
	return TRUE;
}

#ifndef NO_ACCELERATION_CORRECTION
__STATIC_INLINE int8_t findInAccelerationCrd(MVECTOR *p, MVECTOR *p_in, int32_t *smothdF, int32_t *frqStart)
{
	int8_t crd = -1;
	int32_t  i, j, df[3], step_df[3], frq_in[3], dt[3], max_time = 0;

	// разница в скоростях 
	for (i = 0; i < 3; i++)
	{
		frq_in[i] = p->changeDir ? 0 : p_in->frq[i];
		if (p->frq[i] != 0) df[i] = labs((int32_t)frq_in[i] - (int32_t)p->frq[i]);
	}
	for (i = 0; i < 3; i++)
	{
		if (p->frq[i] == 0 || df[i] < _smParam.smoothStartF_from0[i])
		{
			dt[i] = 0;
			continue;
		} // движения по координате - нет. значит торможение ускорение отсутвует.
		step_df[i] = _smParam.smoothAF[i];
		dt[i] = df[i] / step_df[i]; // время необходимое на изменение скорости
		for (j = 0; j < 3; j++)
		{ // коррекция ускорения с учетом остальных осей.
			int32_t f;
			if (i == j || p->steps[j] == 0) continue;
			f = (int32_t)((int64_t)step_df[i] * (int64_t)p->steps[j] / p->steps[i]);
			if (f >(_smParam.smoothAF[j] * 3 / 2))
			{
				dt[i] = 0;
				//step_df[i] = step_df[i]*_smParam.smoothAF[j]/f;
			}
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (max_time < dt[i])
		{
			crd = (int8_t)i;
			max_time = dt[i];
		} // ищем ось с максимальным временем ускорения      
	}
	// если разница в скоростях меньше той, при которой можно стартовать с 0 и не было коррекций
	if (crd >= 0 && df[crd] < _smParam.smoothStartF_from0[crd] && step_df[crd] == _smParam.smoothAF[crd])
		return -1;
	*smothdF = frq_in[crd] < (int32_t)p->frq[crd] ? step_df[crd] : -step_df[crd];
	*frqStart = frq_in[crd];
	return crd;
}

__STATIC_INLINE int8_t findOutAccelerationCrd(MVECTOR *p, MVECTOR *p_out, int32_t *smothdF, int32_t *frqStop)
{
	int8_t crd = -1;
	int32_t i, j, df[3], frq_out[3], step_df[3], max_time = 0, dt[3];

	// разница в скоростях
	for (i = 0; i < 3; i++)
	{
		frq_out[i] = p_out->changeDir ? 0 : p_out->frq[i];
		df[i] = labs((int32_t)p->frq[i] - frq_out[i]);
	}
	for (i = 0; i < 3; i++)
	{
		// движения по координате - нет. значит торможение ускорение отсутвует
		// и так же запрешено ускорение выше скорости, указанной для отрезка
		if (p->frq[i] == 0 || frq_out[i] >(int32_t)p->frq[i]) { dt[i] = 0; continue; }
		step_df[i] = _smParam.smoothAF[i];
		dt[i] = df[i] / step_df[i]; // время (msec) необходимое на изменение скорости
		for (j = 0; j < 3; j++) { // коррекция ускорения с учетом остальных осей.
			int32_t f;
			if (i == j || p->steps[j] == 0) continue;
			f = (int32_t)((int64_t)step_df[i] * (int64_t)p->steps[j] / p->steps[i]);
			if (f >(_smParam.smoothAF[j] * 3 / 2)) {
				dt[i] = 0;
				//    step_df[i] = step_df[i]*_smParam.smoothAF[j]/f;
			}
		}
	}
	for (i = 0; i < 3; i++) {
		if (max_time < dt[i]) { crd = (int8_t)i; max_time = dt[i]; } // ищем ось с максимальным временем ускорения      
	}
	if (crd >= 0 && df[crd] < _smParam.smoothStopF_to0[crd] && step_df[crd] == _smParam.smoothAF[crd]) return -1; // если разница в скоростях меньше той, при которой можно старотовать с 0
	*smothdF = frq_out[crd] < (int32_t)p->frq[crd] ? step_df[crd] : -step_df[crd];
	*frqStop = frq_out[crd];
	return crd;
}

// интегрировние числовое, что бы проверить если ли возможность затормозить на коротких отрезках
__STATIC_INLINE int32_t chk_Speed(int32_t i, uint32_t fxyze[], uint32_t abs_dxyze[])
{
	if (fxyze[i] > (uint32_t)_smParam.smoothStopF_to0[i])
	{
		uint32_t dv = _smParam.smoothAF[i];
		uint32_t v = dv, s = 0, max_s = abs_dxyze[i] / 3 * 2;
		do
		{
			s += v*SM_SMOOTH_TFEED / 1000 / K_FRQ, v += _smParam.smoothAF[i];
		} while (s < max_s);
		if (v < fxyze[i])
			return abs_dxyze[i] * 1000L * K_FRQ / (v - dv) + 1;
	}
	return -1;
}
#endif

uint8_t smothLine(
	int32_t dx, int32_t dy, int32_t dz, int32_t de,
	int32_t time_msec,
	double moveLength, uint32_t feed_rate
	)
{
	uint32_t abs_dxyze[4];
	uint32_t fxyze[4];
	uint8_t dir_xyze[4];
	int32_t n;
#ifndef NO_ACCELERATION_CORRECTION
	MVECTOR *p_next, *p_cur, *p_prev;
	int32_t i;
#endif

	abs_dxyze[CRD_X] = labs(dx);
	abs_dxyze[CRD_Y] = labs(dy);
	abs_dxyze[CRD_Z] = labs(dz);
	abs_dxyze[CRD_E] = labs(de);
	dir_xyze[CRD_X] = dx > 0;
	dir_xyze[CRD_Y] = dy > 0;
	dir_xyze[CRD_Z] = dz > 0;
	dir_xyze[CRD_E] = de > 0;
#ifdef DEBUG_MODE
	{
		DBG("\n[%d]-> orig.line dx:%d dy:%d dz:%d", linesBuffer.mvectCnt, dx, dy, dz);
		if (linesBuffer.mvectCnt == 405)
		{
			Sleep(100);
		}
	}
#endif
	while (time_msec > 0)
	{
		DBG(" T:%d", time_msec);
		fxyze[CRD_Z] = abs_dxyze[CRD_Z] * 1000L * K_FRQ / time_msec;
		if (fxyze[CRD_Z] > _smParam.maxFeedRate[CRD_Z])
		{
			DBG(" cZ");
			time_msec = (uint64_t)abs_dxyze[CRD_Z] * (1000L * K_FRQ) / _smParam.maxFeedRate[CRD_Z] + 1;
			continue;
		}
		if ((n = chk_Speed(CRD_Z, fxyze, abs_dxyze)) > 0)
		{
			DBG(" bZ");
			time_msec = n; continue;
		}

		fxyze[CRD_Y] = abs_dxyze[CRD_Y] * 1000L * K_FRQ / time_msec;
		if (fxyze[CRD_Y] > _smParam.maxFeedRate[CRD_Y])
		{
			DBG(" cY");
			time_msec = (uint64_t)abs_dxyze[CRD_Y] * (1000L * K_FRQ) / _smParam.maxFeedRate[CRD_Y] + 1;
			continue;
		}
		if ((n = chk_Speed(CRD_Y, fxyze, abs_dxyze)) > 0)
		{
			DBG(" bY");
			time_msec = n; continue;
		}

		fxyze[CRD_X] = (uint64_t)abs_dxyze[CRD_X] * (1000L * K_FRQ) / time_msec;
		if (fxyze[CRD_X] > _smParam.maxFeedRate[CRD_X])
		{
			DBG(" cX");
			time_msec = (uint64_t)abs_dxyze[CRD_X] * (1000L * K_FRQ) / _smParam.maxFeedRate[CRD_X] + 1;
			continue;
		}
		if ((n = chk_Speed(CRD_X, fxyze, abs_dxyze)) > 0)
		{
			DBG(" bX");
			time_msec = n; continue;
		}

		fxyze[CRD_E] = (uint64_t)abs_dxyze[CRD_E] * (1000L * K_FRQ) / time_msec;
		break;
	}
	DBG(" ->T%d", time_msec);

#ifdef NO_ACCELERATION_CORRECTION
	return sendLine(fxyze, abs_dxyze, dir_xyze);
#else
	linesBuffer.mvectPtrCur++;
	if (linesBuffer.mvectPtrCur >= MVECTOR_SZ) linesBuffer.mvectPtrCur = 0;
	//-------------------
	p_next = &linesBuffer.mvector[n = linesBuffer.mvectPtrCur];
	if ((--n) < 0) n = MVECTOR_SZ - 1;
	p_cur = &linesBuffer.mvector[n];
	if ((--n) < 0) n = MVECTOR_SZ - 1;
	p_prev = &linesBuffer.mvector[n];
	//------------------

	if (time_msec == 0)
	{
		for (i = 0; i < 4; i++)
		{
			p_next->frq[i] = 0;
			p_next->steps[i] = 0;
		}
		p_next->isNullCmd = TRUE;
	}
	else
	{
		// ArtCam breaks long sections into small retaining speed!
		// Combining these portions into one.
		if (p_cur->feed_rate == feed_rate)
		{
			for (i = n = 0; i < 3; i++)
			{
				if (p_cur->steps[i] == 0 && abs_dxyze[i] == 0)
					n++;
				else if (p_cur->dir[i] != dir_xyze[i])
					n += 10;
			}
			if (n == 2)
			{
				for (i = n = 0; i < 4; i++)
					p_cur->steps[i] += abs_dxyze[i];
				if (p_cur->frq[0] < fxyze[0] || p_cur->frq[1] < fxyze[1] || p_cur->frq[2] < fxyze[2])
				{
					for (i = n = 0; i < 4; i++)
						p_cur->frq[i] = fxyze[i];
				}
				p_cur->length += moveLength;
				p_cur->isNullCmd = FALSE;
				linesBuffer.mvectPtrCur--;
				if (linesBuffer.mvectPtrCur < 0)
					linesBuffer.mvectPtrCur = MVECTOR_SZ - 1;
				DBG("\nSUM vectors"); return TRUE;
			}
		}
		p_next->isNullCmd = FALSE;
		p_next->length = moveLength;
		p_next->feed_rate = feed_rate;
		for (i = 0; i < 4; i++)
		{
			p_next->steps[i] = abs_dxyze[i];
			p_next->frq[i] = fxyze[i];
			p_next->dir[i] = dir_xyze[i];
		}
	}
	if (linesBuffer.mvectCnt > 1)
	{
		p_next->cos_a = 0;
		if (!p_cur->isNullCmd)
		{
			p_next->changeDir = FALSE;
			for (i = 0; !p_next->changeDir && i < 3; i++)
			{
				if (p_next->dir[i] != p_cur->dir[i]
					|| (p_next->steps[i] == 0 && p_cur->steps[i] != 0)
					|| (p_next->steps[i] != 0 && p_cur->steps[i] == 0)
					)
					p_next->changeDir = TRUE;
			}
			p_next->cos_a = 0;
			if (!p_next->changeDir)
			{
				// Integer calculations only if the movement is less than the steps fit in 32 bits
				const uint32_t k[3] = { SM_X_STEPS_PER_MM, SM_Y_STEPS_PER_MM, SM_Z_STEPS_PER_MM };
				for (i = 0; i < 3; i++)
				{
					int32_t v = (int32_t)((uint32_t)p_next->steps[i] * 1000L / (uint32_t)(p_next->length * k[i]))
						* (int32_t)((uint32_t)p_cur->steps[i] * 1000L / (uint32_t)(p_cur->length * k[i]));
					if (p_next->dir[i] == p_cur->dir[i])
						p_next->cos_a += v;
					else
						p_next->cos_a -= v;
				}
				if (p_next->cos_a < SM_SMOOTH_COS_A)
					p_next->changeDir = TRUE;
			}
		}
		else
			p_next->changeDir = TRUE;
	}
	linesBuffer.mvectCnt++;
	if (linesBuffer.mvectCnt < 3 || p_cur->isNullCmd)
		return TRUE;
	//------------------
#ifdef DEBUG_MODE
	DBG("\nl.prev dx:%c%d(%d)\tdy:%c%d(%d)\tdz:%c%d(%d)", p_prev->dir[0] ? '+' : '-', p_prev->steps[0], p_prev->frq[0],
		p_prev->dir[1] ? '+' : '-', p_prev->steps[1], p_prev->frq[1],
		p_prev->dir[2] ? '+' : '-', p_prev->steps[2], p_prev->frq[2]);
	DBG("\nl.cur dx:%c%d(%d)\tdy:%c%d(%d)\tdz:%c%d(%d) cos_a:%f", p_cur->dir[0] ? '+' : '-', p_cur->steps[0], p_cur->frq[0],
		p_cur->dir[1] ? '+' : '-', p_cur->steps[1], p_cur->frq[1],
		p_cur->dir[2] ? '+' : '-', p_cur->steps[2], p_cur->frq[2], p_cur->cos_a / 1000000.0);
	DBG("\nl.next dx:%c%d(%d)\tdy:%c%d(%d)\tdz:%c%d(%d) cos_a:%f", p_next->dir[0] ? '+' : '-', p_next->steps[0], p_next->frq[0],
		p_next->dir[1] ? '+' : '-', p_next->steps[1], p_next->frq[1],
		p_next->dir[2] ? '+' : '-', p_next->steps[2], p_next->frq[2], p_next->cos_a / 1000000.0);
#endif
	// Search coordinate where necessary braking or acceleration
	int8_t crd_in, crd_out, has_max_frq = TRUE;
	int32_t df_in = 0, df_out = 0, frq_in = 0, frq_out = 0, frq;

	crd_in = findInAccelerationCrd(p_cur, p_prev, &df_in, &frq_in);
	crd_out = findOutAccelerationCrd(p_cur, p_next, &df_out, &frq_out);

	DBG("\ncrd_in:%d df_in:%d frq_in:%d  crd_out:%d df_out:%d frq_out:%d",
		crd_in, df_in, frq_in,
		crd_out, df_out, frq_out);

	if (crd_out > 0 && df_out < 0)
	{
		crd_out = -1;
		DBG("\nOut acceleration disabled");
	}

	if (crd_in < 0 && crd_out < 0)
	{	// Entire line without the need to accelerate or brake
		return sendLine(p_cur->frq, p_cur->steps, p_cur->dir);
	}

	/*
	double t0 = 0, t1 = 0;
	int32_t s0 = 0, s1 = 0;
	if(crd_in >= 0) {
	t0 = ((int32_t)p_cur->frq[crd_in]-frq_in)/((double)df_in*SM_SMOOTH_TFEED/1000);
	s0 = (int32_t)(frq_in/K_FRQ * t0 + df_in*SM_SMOOTH_TFEED/K_FRQ * t0*t0/2);
	}
	if(crd_out >= 0) {
	t1 = ((int32_t)p_cur->frq[crd_out]-frq_out)/((double)df_out*SM_SMOOTH_TFEED/1000);
	s1 = (int32_t)(frq_out/K_FRQ * t0 + df_out*SM_SMOOTH_TFEED/K_FRQ * t1*t1/2);
	}
	DBG("\nin:%fs %d  out:%f %d", t0, s0, t1, s1);
	*/

	int32_t fBreakage_out = 0, sBreakage_out = -1;
	// Search pitch axis crd_out want to start braking (if necessary)
	if (crd_out >= 0)
	{
		int32_t tmp_dd = 0, s_in = 0;
		int32_t f2 = frq_out, max_f = p_cur->frq[crd_out], length_steps = p_cur->steps[crd_out];
		int32_t f1 = crd_in >= 0 ? (int32_t)((int64_t)frq_in*p_cur->steps[crd_out] / p_cur->steps[crd_in]) : (int32_t)p_cur->frq[crd_out];
		int32_t dd_f1 = crd_in >= 0 ? (int32_t)((int64_t)df_in*p_cur->steps[crd_out] / p_cur->steps[crd_in]) : 0;

		sBreakage_out = 0;	// sBreakage_out - number of steps from the end
		for (i = 0; TRUE; i++)
		{	// We go from the end of the braking distance
			fBreakage_out = (f2 += df_out);
			if (crd_in >= 0)
			{
				// If you have not reached a maximum speed of incoming
				if ((dd_f1 < 0 && f1 > max_f) || (dd_f1 > 0 && f1 < max_f))
				{
					f1 += dd_f1;
					s_in += f1*SM_SMOOTH_TFEED / K_FRQ / 1000; // Integration over the boost phase
				}
			}
			if (f2 > max_f)
			{	// if reached maximum speed specified for the vector braking
				break;
			}
			sBreakage_out += (tmp_dd = f2*SM_SMOOTH_TFEED / K_FRQ / 1000);	// Time integration braking area
			if (sBreakage_out >= (int32_t)p_cur->steps[crd_out])
			{
				sBreakage_out = length_steps; crd_in = -1;
				break;
			}
			// If the acceleration and deceleration areas converged before reaching a maximum speed vector
			if ((s_in + sBreakage_out) >= length_steps)
			{
				has_max_frq = FALSE;
				if (labs(f1 - f2) > df_out && dd_f1 > 0)
					if (f1  < f2)
					{
						sBreakage_out -= tmp_dd;
					}
					else
					{
						while (labs(f1 - f2) > df_out && sBreakage_out < length_steps)
						{
							fBreakage_out = (f2 += df_out);
							sBreakage_out += f2*SM_SMOOTH_TFEED / K_FRQ / 1000;
							f1 -= dd_f1;
						}
					}
				break;
			}
		}
		if (length_steps - sBreakage_out < 2)
		{
			crd_in = -1;
			sBreakage_out = length_steps;  // For cases where the length of the segment stabilized speed is low
		}
	}
	DBG("\n crd_out=%d sBreakage_out=%d fBreakage_out=%d", crd_out, sBreakage_out, fBreakage_out);
	uint8_t isProcess = TRUE;
	uint32_t remainSteps[4];

	for (i = 0; i < 4; i++)
		remainSteps[i] = p_cur->steps[i];

	// =========== Input braking / acceleration
	if (crd_in >= 0)
	{
		uint8_t isProcess = TRUE;
		frq = frq_in + df_in;
		while (isProcess && ((df_in < 0 && frq >(int32_t)p_cur->frq[crd_in]) || (df_in > 0 && frq < (int32_t)p_cur->frq[crd_in])))
		{
			for (i = 0; i < 4; i++)
			{
				fxyze[i] = (uint32_t)((uint64_t)frq * (uint64_t)p_cur->steps[i] / p_cur->steps[crd_in]);
				abs_dxyze[i] = (uint64_t)fxyze[i] * SM_SMOOTH_TFEED / K_FRQ / 1000;
			}
			if (crd_out >= 0 && abs_dxyze[crd_out] >(remainSteps[crd_out] - sBreakage_out))
			{
				n = remainSteps[crd_out] - sBreakage_out;
				for (i = 0; i < 4; i++)
					abs_dxyze[i] = (uint64_t)n*p_cur->steps[i] / p_cur->steps[crd_out];
				isProcess = FALSE;
			}
			for (i = 0; i < 4; i++)
				if (abs_dxyze[i] != 0 && remainSteps[i] <= abs_dxyze[i])
				{
					for (i = 0; i < 4; i++)
						abs_dxyze[i] = remainSteps[i];
					isProcess = FALSE;
					break;
				}

			if (!sendLine(fxyze, abs_dxyze, p_cur->dir))
				return FALSE;
			for (i = 0; i < 4; i++)
				remainSteps[i] -= abs_dxyze[i];
			frq += df_in;
		}
	}
	// ========= No braking
	if (crd_out < 0)
	{
		if (remainSteps[crd_in] == 0)
		{
			// Set the input speed to the next line
			for (i = 0; i < 4; i++)
				p_cur->frq[i] = fxyze[i];
			return TRUE;
		}
		return sendLine(p_cur->frq, remainSteps, p_cur->dir); // Remain line
	}
	// =========== Line with established frequency
	if (remainSteps[crd_out] >(uint32_t)sBreakage_out)
	{
		for (i = 0; i < 4; i++)
		{
			uint32_t dd = (int64_t)sBreakage_out*p_cur->steps[i] / p_cur->steps[crd_out];
			abs_dxyze[i] = (dd <= remainSteps[i]) ? remainSteps[i] - dd : 0;
			remainSteps[i] -= abs_dxyze[i];
			if (has_max_frq)
				fxyze[i] = p_cur->frq[i];
		}
		if (!sendLine(fxyze, abs_dxyze, p_cur->dir))
			return FALSE;
		if (sBreakage_out == 0)
			return TRUE;
		//f_out = p_cur->frq[crd_out];
	}
	// ============= Braking
	for (frq = fBreakage_out; isProcess && remainSteps[crd_out] > 0;)
	{
		frq -= df_out;
		if (labs(frq - frq_out) < labs(df_out))
		{
			frq = frq_out < _smParam.smoothStopF_to0[crd_out] ? _smParam.smoothStopF_to0[crd_out] : frq_out;
			for (i = 0; i < 4; i++)
			{
				fxyze[i] = (uint32_t)((uint64_t)frq * (uint64_t)p_cur->steps[i] / p_cur->steps[crd_out]);
				abs_dxyze[i] = remainSteps[i];
			}
			isProcess = FALSE;
		}
		else
		{
			for (i = 0; i < 4; i++)
			{
				fxyze[i] = (uint32_t)((uint64_t)frq * (uint64_t)p_cur->steps[i] / p_cur->steps[crd_out]);
				abs_dxyze[i] = (uint64_t)fxyze[i] * SM_SMOOTH_TFEED / K_FRQ / 1000L;
			}
		}

		for (i = 0; i < 4; i++)
		{
			if (abs_dxyze[i] > remainSteps[i] || (abs_dxyze[i] != 0 && (abs_dxyze[i] - remainSteps[i]) < 15))
			{
				for (i = 0; i < 4; i++)
				{
					abs_dxyze[i] = remainSteps[i];
				}
				isProcess = FALSE;
				break;
			}
		}
		if (!sendLine(fxyze, abs_dxyze, p_cur->dir))
			return FALSE;
		for (i = 0; i < 4; i++)
			remainSteps[i] -= abs_dxyze[i];
	}
	// Set the input speed to the next line
	for (i = 0; i < 4; i++)
		p_cur->frq[i] = fxyze[i];
#endif
	return TRUE;
}

uint8_t cnc_line(
	double x, double y, double z,
	double extruder_length,
	double moveLength,
	double feed_rate
	)
{
	int32_t
		newX = lround(x * SM_X_STEPS_PER_360 / MM_PER_360),
		newY = lround(y * SM_Y_STEPS_PER_360 / MM_PER_360),
		newZ = lround(z * SM_Z_STEPS_PER_360 / MM_PER_360),
		newE = lround(extruder_length * SM_E_STEPS_PER_MM);
	int32_t dx = newX - linesBuffer.stepsFromStartX;
	int32_t dy = newY - linesBuffer.stepsFromStartY;
	int32_t dz = newZ - linesBuffer.stepsFromStartZ;
	int32_t de = newE - linesBuffer.stepsFromStartE;
	linesBuffer.stepsFromStartX = newX;
	linesBuffer.stepsFromStartY = newY;
	linesBuffer.stepsFromStartZ = newZ;
	linesBuffer.stepsFromStartE = newE;
	uint32_t time_msec = (uint32_t)(moveLength * (60000.0 / feed_rate));
	commonTimeIdeal += time_msec;

	if ((dx != 0 || dy != 0 || dx != 0) && time_msec == 0)
		time_msec = 1;
	if (de < 0)
		de = newE; // 91384.9586 - max value for skeinforge.py

	DBG("\n AX:%d AY:%d AZ:%d", newX, newY, newZ);

	if (kbd_getKey() == KEY_C)
		return FALSE;
	if (x < minX) minX = x;
	if (x > maxX) maxX = x;
	if (y < minY) minY = y;
	if (y > maxY) maxY = y;
	if (z < minZ) minZ = z;
	if (z > maxZ) maxZ = z;

	//=======================================
	// if((curGCodeMode & GFILE_MODE_MASK_EXEC) != 0) {
	return smothLine(dx, dy, dz, de, time_msec, moveLength, (uint32_t)feed_rate);
	// }
	// return TRUE;
}

void cnc_end(void)
{
	isGcodeStop = TRUE;
}

#ifdef HAS_EXTRUDER
void cnc_extruder_stop(void)
{
	isExtruderOn = FALSE;
	cnc_waitSMotorReady();
	uint8_t dir[4] = { 0, 0, 0, 1 };
	uint32_t steps[4] = { 0, 0, 0, SM_E_STEPS_PER_MM / 2 };
	const uint32_t frq[4] = {
		SM_MANUAL_MODE_STEPS_PER_SEC * K_FRQ,
		SM_MANUAL_MODE_STEPS_PER_SEC * K_FRQ,
		SM_MANUAL_MODE_STEPS_PER_SEC * K_FRQ,
		SM_MANUAL_MODE_STEPS_PER_SEC * K_FRQ
	};
	stepm_addMove(steps, frq, dir);
}

void cnc_extruder_on(void)
{
	isExtruderOn = TRUE;
}

void cnc_extruder_t(int temperature, int isWait)
{
	extrudT_setTemperature((int16_t)temperature);
	if (isWait)
	{
		if ((curGCodeMode & GFILE_MODE_MASK_EXEC) != 0)
		{
			scr_fontColor(Red, Black);
			while (!extrudT_isReady())
			{
				scr_gotoxy(3, 2);
				scr_fontColor(Yellow, Blue);
				scr_printf("extruder t:%03d/%03d", extrudT_getTemperatureReal(), extrudT_getTemperatureWait());
				if (kbd_getKey() == KEY_C)
					return;
			}
			scr_gotoxy(3, 2);
			scr_fontColor(White, Black);
			scr_puts("                        ");
		}
	}
}
#endif
