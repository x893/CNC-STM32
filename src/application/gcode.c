/* This code is inspired by the "Grbl"
 * Copyright (c) 2009-2011 Simen Svale Skogsrud
 * Copyright (c) 2011 Sungeun K. Jeon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifdef _WINDOWS
	#include "stdafx.h"
#else
	#include "global.h"
#endif

#include "gcode.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define MM_PER_INCH (25.4)

#define NEXT_ACTION_DEFAULT 0
#define NEXT_ACTION_DWELL_G4 1
#define NEXT_ACTION_GO_HOME_G28 2
#define NEXT_ACTION_RESET_XYZ_G92 3
#define NEXT_ACTION_STOP 4
#define NEXT_ACTION_SEEK_G0 5 // G0 
#define NEXT_ACTION_LINEAR_G1 6 // G1
#define NEXT_ACTION_EXTRUDER_STOP 7
#define NEXT_ACTION_EXTRUDER_ON 8
#define NEXT_ACTION_EXTRUDER_FAST_T 9
#define NEXT_ACTION_EXTRUDER_WAIT_T 10
#define NEXT_ACTION_CW_ARC 11
#define NEXT_ACTION_CCW_ARC 12

typedef struct {
	uint8_t status_code;
	uint8_t inches_mode;             /* 0 = millimeter mode, 1 = inches mode {G20, G21} */
	uint8_t absolute_mode;           /* 0 = relative motion, 1 = absolute motion {G90, G91} */
	uint8_t spindle_on, extruder_on;
	double feed_rate, seek_rate;     /* Millimeters/second */
	double extruder_length, extruder_k;
	double position[3];              /* Where the interpreter considers the tool to be at this point in the code */
	int16_t s_value;           /* RPM/100 or temperature of the current extruder */
	uint8_t next_action;  /* The action that will be taken by the parsed line */
} parser_state_t;

parser_state_t gc;

#define FAIL(status) { gc.status_code = status; return(gc.status_code); }

double strtod_M(const char *str, char **endptr)
{
	double number = 0.0;
	double div = 0.0;
	bool negative = false;
	bool plus = false;
	bool skip = true;
	char c;
	while ((c = *str) != 0)
	{
		if (c == '+')
		{
			if (skip && !plus)
			{
				plus = true;
				skip = false;
			}
			else
				break;
		}
		else if (skip && !negative && c == '-')
		{
			if (skip && !negative)
			{
				negative = true;
				skip = false;
			}
			else
				break;
		}
		else if (c == '.')
		{
			if (div == 0.0)
				div = 1.0;
			else
				break;
		}
		else if (c >= '0' && c <= '9')
		{
			skip = false;
			if (div == 0.0)
				number = number * 10.0 + (double)(c - '0');
			else
			{
				div *= 10.0;
				number += ((double)(c - '0') / div);
			}
		}
		else if (!skip)
		{
			break;
		}
		str++;
	}

	if (negative) number = -number;
	if (endptr != NULL) *endptr = (char *)str;
	return number;
}

static int read_double(char *line, int *char_counter, double *double_ptr)
{
	char *start = line + *char_counter;
	char *end;

	*double_ptr = strtod_M(start, &end);
	if (end == start)
	{
		gc.status_code = GCSTATUS_BAD_NUMBER_FORMAT;
		return false;
	}
	*char_counter = (int)(end - line);
	return true;
}

static int next_statement(char *letter, double *double_ptr, char *line, int *char_counter)
{
	while (line[*char_counter] == ' ') (*char_counter)++;

	if (line[*char_counter] == 0 || line[*char_counter] == ';' ||
		line[*char_counter] == '\n' || line[*char_counter] == '\r') return false;
	*letter = line[*char_counter];
	if ((*letter < 'A') || (*letter > 'Z'))
	{
		gc.status_code = GCSTATUS_EXPECTED_COMMAND_LETTER;
		return false;
	}
	(*char_counter)++;
	return read_double(line, char_counter, double_ptr);
}

void gc_init(void)
{
	memset(&gc, 0, sizeof(gc));
	gc.feed_rate = SM_DEFAULT_FEED_RATE;
	gc.seek_rate = SM_DEFAULT_SEEK_RATE;
	gc.absolute_mode = true;
	// gc.startPosX = commonValues.startX; gc.startPosY = commonValues.startY;
	// gc.startPosZ = commonValues.startZ;
	gc.extruder_k = 1; //commonValues.extruder_k;
	gc.next_action = NEXT_ACTION_DEFAULT;
}

static double to_millimeters(double value)
{
	return (gc.inches_mode ? (value * MM_PER_INCH) : value);
}

static void mc_arc(double *position, double *target, double *offset, double feed_rate, double radius, uint8_t isclockwise)
{
	double center_axisX = position[X_AXIS] + offset[X_AXIS];
	double center_axisY = position[Y_AXIS] + offset[Y_AXIS];
	double move_z = target[Z_AXIS] - position[Z_AXIS];
	double r_axisX = -offset[X_AXIS];  // Radius vector from center to current location
	double r_axisY = -offset[Y_AXIS];
	double rt_axisX = target[X_AXIS] - center_axisX;
	double rt_axisY = target[Y_AXIS] - center_axisY;

	// CCW angle between position and target from circle center. Only one atan2() trig computation required.
	double angular_travel = atan2(r_axisX*rt_axisY - r_axisY*rt_axisX, r_axisX*rt_axisX + r_axisY*rt_axisY);
	double millimeters_of_travel;
	uint16_t segments;

	if (angular_travel < 0) { angular_travel += 2 * M_PI; }
	if (isclockwise) { angular_travel -= 2 * M_PI; }
	if (angular_travel == 0.0) angular_travel = (isclockwise) ? -2 * M_PI : 2 * M_PI;

	millimeters_of_travel = hypot(angular_travel*radius, fabs(move_z));
	if (millimeters_of_travel <= DEFAULT_MM_PER_ARC_SEGMENT) { return; }
	segments = (int16_t)floor(millimeters_of_travel / DEFAULT_MM_PER_ARC_SEGMENT);

	//  scr_fontColor(White,Black); scr_gotoxy(1,13);	scr_printf("angle:%f steps:%d", angular_travel, segments); scr_clrEndl();

	double theta_per_segment = angular_travel / segments;
	double dz = move_z / segments;

	/* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
	   and phi is the angle of rotation. Based on the solution approach by Jens Geisler.
	   r_T = [cos(phi) -sin(phi);
	   sin(phi)  cos(phi] * r ;

	   For arc generation, the center of the circle is the axis of rotation and the radius vector is
	   defined from the circle center to the initial position. Each line segment is formed by successive
	   vector rotations. This requires only two cos() and sin() computations to form the rotation
	   matrix for the duration of the entire arc. Error may accumulate from numerical round-off, since
	   all double numbers are single precision on the Arduino. (true double precision will not have
	   round off issues for CNC applications.) Single precision error can accumulate to be greater than
	   tool precision in some cases. Therefore, arc path correction is implemented.

	   Small angle approximation may be used to reduce computation overhead further. This approximation
	   holds for everything, but very small circles and large mm_per_arc_segment values. In other words,
	   theta_per_segment would need to be greater than 0.1 rad and N_ARC_CORRECTION would need to be large
	   to cause an appreciable drift error. N_ARC_CORRECTION~=25 is more than small enough to correct for
	   numerical drift error. N_ARC_CORRECTION may be on the order a hundred(s) before error becomes an
	   issue for CNC machines with the single precision Arduino calculations.

	   This approximation also allows mc_arc to immediately insert a line segment into the planner
	   without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
	   a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead.
	   This is important when there are successive arc motions.
	   */
	// Vector rotation matrix values
	double cos_T = 1 - 0.5*theta_per_segment*theta_per_segment; // Small angle approximation
	double sin_T = theta_per_segment;

	double arc_target[3];
	double sin_Ti;
	double cos_Ti;
	double r_axisi;
	double moveLength, dx, dy;
	uint16_t i;
	int8_t count = 0;
	// Initialize
	arc_target[Z_AXIS] = position[Z_AXIS];
	arc_target[X_AXIS] = position[X_AXIS];
	arc_target[Y_AXIS] = position[Y_AXIS];
	// double moveSum = 0;
	//  uint32_t timeSum = 0;

	for (i = 1; i < segments; i++)
	{	// Increment (segments-1)
		if (count < N_ARC_CORRECTION)
		{	// Apply vector rotation matrix
			r_axisi = r_axisX*sin_T + r_axisY*cos_T;
			r_axisX = r_axisX*cos_T - r_axisY*sin_T;
			r_axisY = r_axisi;
			count++;
		}
		else
		{	// Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments.
			// Compute exact location by applying transformation matrix from initial radius vector(=-offset).
			cos_Ti = cos(i*theta_per_segment);
			sin_Ti = sin(i*theta_per_segment);
			r_axisX = -offset[X_AXIS] * cos_Ti + offset[Y_AXIS] * sin_Ti;
			r_axisY = -offset[X_AXIS] * sin_Ti - offset[Y_AXIS] * cos_Ti;
			count = 0;
		}

		// Update arc_target location
		arc_target[X_AXIS] = center_axisX + r_axisX;
		arc_target[Y_AXIS] = center_axisY + r_axisY;
		arc_target[Z_AXIS] += dz;

		dx = arc_target[X_AXIS] - position[X_AXIS];
		dy = arc_target[Y_AXIS] - position[Y_AXIS];
		if (isnan(dx) && isnan(dy) && isnan(dz)) moveLength = 0;
		else moveLength = sqrt(dx*dx + dy*dy + dz*dz);

		if (!cnc_line(arc_target[X_AXIS], arc_target[Y_AXIS], arc_target[Z_AXIS], 0, moveLength, feed_rate))
		{
			gc.status_code = GCSTATUS_CANCELED;
			return;
		}
		position[X_AXIS] = arc_target[X_AXIS];
		position[Y_AXIS] = arc_target[Y_AXIS];
		position[Z_AXIS] = arc_target[Z_AXIS];
	}
	// Ensure last segment arrives at target location.
	dx = target[X_AXIS] - arc_target[X_AXIS];
	dy = target[Y_AXIS] - arc_target[Y_AXIS];
	dz = target[Z_AXIS] - arc_target[Z_AXIS];
	if (isnan(dx) && isnan(dy) && isnan(dz))
		moveLength = 0;
	else
		moveLength = sqrt(dx * dx + dy * dy + dz * dz);
	if (!cnc_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], 0, moveLength, gc.feed_rate))
	{
		gc.status_code = GCSTATUS_CANCELED;
	}
}

// Executes one line of 0-terminated G-Code. The line is assumed to contain only upper case
// characters and signed floating point values (no whitespace).
uint8_t gc_execute_line(char *line)
{
	double feed_rate;
	double extrudeLength;
	int char_counter = 0;
	char letter;
	double value, oldPosition[3], dx, dy, dz, moveLength, offset[3], radius = 0;
	int pause_value = 0;
	uint8_t radius_mode = false;

	gc.status_code = GCSTATUS_OK;
	
	if (line[0] == ';'
		|| line[0] == '('
		|| line[0] == '%'
		)
		return GCSTATUS_OK;	// comments

	// Pass 1: Commands
	while (next_statement(&letter, &value, line, &char_counter))
	{
		int int_value = (int)value;
		switch (letter)
		{
		case 'G':
			switch (int_value)
			{
			case 0: gc.next_action = NEXT_ACTION_SEEK_G0; break;
			case 1: gc.next_action = NEXT_ACTION_LINEAR_G1; break;
			case 2: gc.next_action = NEXT_ACTION_CW_ARC; break;
			case 3: gc.next_action = NEXT_ACTION_CCW_ARC; break;
			case 4: gc.next_action = NEXT_ACTION_DWELL_G4; break;
			case 20: gc.inches_mode = true; break;
			case 21: gc.inches_mode = false; break;
			case 30:
			case 28: gc.next_action = NEXT_ACTION_GO_HOME_G28; break;
			case 90: gc.absolute_mode = true; break;
			case 91: gc.absolute_mode = false; break;
			case 92: gc.next_action = NEXT_ACTION_RESET_XYZ_G92; break;
			case 64:
			case 40:
			case 17: // G17 Выбор рабочей плоскости X-Y
			case 94: // Feedrate per minute
			case 98: // Feedrate per minute (group type A)
			case 97: // Constant spindle speed M T Takes an S address integer, which is interpreted as rev/min (rpm). The default speed mode per system parameter if no mode is programmed. 
			case 49: // Tool length offset compensation cancel
				break;
			default: FAIL(GCSTATUS_UNSUPPORTED_STATEMENT);
			}
			break;
		case 'M':
			switch (int_value)
			{
			case 112: // Emergency Stop 
			case 0:
			case 1:
			case 2:
			case 30:
			case 60:
				gc.next_action = NEXT_ACTION_STOP;
				break;
			case 3: gc.spindle_on = 1; break;
				//	case 4: gc.spindle_direction = -1; break;
			case 5: gc.spindle_on = 0; break;
#if (USE_EXTRUDER == 1)
			case 101: //  M101 Turn extruder 1 on Forward 
				gc.next_action = NEXT_ACTION_EXTRUDER_ON;
				break;
			case 103: //  M103 Turn all extruders off 
				gc.next_action = NEXT_ACTION_EXTRUDER_STOP;
				break;
			case 104: // M104: Set Extruder Temperature (Fast) Example: M104 S190 Set the temperature of the current extruder to 190oC
				gc.next_action = NEXT_ACTION_EXTRUDER_FAST_T;
				break;
			case 109: // Set Extruder Temperature Example: M109 S190 
				gc.next_action = NEXT_ACTION_EXTRUDER_WAIT_T;
				break;
				//	case 102: //  M102 Turn extruder 1 on Reverse 
#endif
			case 23: // Thread gradual pullout ON
			case 24: // Thread gradual pullout OFF
			case 52: // Unload Last tool from spindle
			case 49: // Feedrate override NOT allowed
			case 48: // Feedrate override allowed
			case 8:  // Coolant on
			case 9:  // Coolant off
			case 105: // M105: Get Extruder Temperature Example: M105 Request the temperature of the current extruder and the build base in degrees Celsius. The temperatures are returned to the host computer. For example, the line sent to the host in response to this command looks like 
			case 106: // M106: Fan On Example: M106 S127 Turn on the cooling fan at half speed. Optional parameter 'S' declares the PWM value (0-255) 
			case 107: // Fan Off 
			case 108: // M108: Set Extruder Speed  Sets speed of extruder motor. (Deprecated in current firmware, see M113) 
			case 110: // Set Current Line Number 
			case 113: // Set Extruder PWM 
			case 140: // Bed Temperature (Fast) Example: M140 S55 Set the temperature of the build bed to 55oC 
			case 141: //Chamber Temperature (Fast) Example: M141 S30 Set the temperature of the chamber to 30oC
			case 142: // Holding Pressure Example: M142 S1 Set the holding pressure of the bed to 1 bar. 
			case 6:
				return gc.status_code;
			default: FAIL(GCSTATUS_UNSUPPORTED_STATEMENT);
			}
			break;
		}
		if (gc.status_code)
			return(gc.status_code);
	}
	if (gc.status_code)
		return(gc.status_code);

	char_counter = 0;
	oldPosition[X_AXIS] = gc.position[X_AXIS];
	oldPosition[Y_AXIS] = gc.position[Y_AXIS];
	oldPosition[Z_AXIS] = gc.position[Z_AXIS];
	offset[0] = offset[1] = offset[2] = 0;

	// Pass 2: Parameters
	extrudeLength = 0;
	while (next_statement(&letter, &value, line, &char_counter))
	{
		double unit_millimeters_value = to_millimeters(value);
		switch (letter)
		{
		case 'E': extrudeLength = value; break;
		case 'F':
			if (gc.next_action == NEXT_ACTION_SEEK_G0)
				gc.seek_rate = unit_millimeters_value;
			else
				gc.feed_rate = unit_millimeters_value; // millimeters pr min
			//	if (unit_millimeters_value > SM_MAX_FEEDRATE)
			//		FAIL(GCSTATUS_UNSOPORTED_FEEDRATE);
			break;
		case 'P': pause_value = (int)value; break;
		case 'S': gc.s_value = (int16_t)value; break;
		case 'X':
		case 'Y':
		case 'Z':
			if (gc.absolute_mode)
				gc.position[letter - 'X'] = unit_millimeters_value;
			else
				gc.position[letter - 'X'] += unit_millimeters_value;
			break;
		case 'I':
		case 'J':
		case 'K':
			offset[letter - 'I'] = unit_millimeters_value;
			break;
		case 'R':
			radius = unit_millimeters_value;
			radius_mode = true;
			break;
		case 'G':
		case 'N':
		case 'M':
			break;
		default:
			FAIL(GCSTATUS_UNSUPPORTED_PARAM);
		}
	}
	if (gc.status_code)
		return gc.status_code;
	if (gc.next_action == NEXT_ACTION_GO_HOME_G28)
	{
		gc.position[0] = gc.position[1] = gc.position[2] = 0;
	}
	dx = gc.position[X_AXIS] - oldPosition[X_AXIS];
	dy = gc.position[Y_AXIS] - oldPosition[Y_AXIS];
	dz = gc.position[Z_AXIS] - oldPosition[Z_AXIS];

	if (fabs(dx) < SM_TOO_SHORT_SEGMENT_MM) { dx = 0.0; gc.position[X_AXIS] = oldPosition[X_AXIS]; }
	if (fabs(dy) < SM_TOO_SHORT_SEGMENT_MM) { dy = 0.0; gc.position[Y_AXIS] = oldPosition[Y_AXIS]; }
	if (fabs(dz) < SM_TOO_SHORT_SEGMENT_MM) { dz = 0.0; gc.position[Z_AXIS] = oldPosition[Z_AXIS]; }

	moveLength = sqrt(dx * dx + dy * dy + dz * dz);
	feed_rate = gc.next_action == (NEXT_ACTION_SEEK_G0 ? gc.seek_rate : gc.feed_rate);
	if (gc.extruder_on)
	{
		if (extrudeLength == 0.0)
			gc.extruder_length += moveLength*gc.extruder_k;
		else
			gc.extruder_length = extrudeLength;
	}

	if ((gc.next_action == NEXT_ACTION_SEEK_G0 ||
		gc.next_action == NEXT_ACTION_LINEAR_G1 ||
		gc.next_action == NEXT_ACTION_CW_ARC ||
		gc.next_action == NEXT_ACTION_CCW_ARC)
		&& moveLength < SM_TOO_SHORT_SEGMENT_MM)
	{
		// too short move.. Ignore
		gc.position[X_AXIS] = oldPosition[X_AXIS];
		gc.position[Y_AXIS] = oldPosition[Y_AXIS];
		gc.position[Z_AXIS] = oldPosition[Z_AXIS];
		return(gc.status_code);
	}

	// Perform any physical actions
	switch (gc.next_action)
	{
	case NEXT_ACTION_DWELL_G4:
		cnc_dwell(pause_value * 1000);
		break;
	case NEXT_ACTION_RESET_XYZ_G92:
		gc.position[0] = gc.position[1] = gc.position[2] = 0;
		break;
	case NEXT_ACTION_STOP:
		cnc_end();
		break;
	case NEXT_ACTION_GO_HOME_G28:
	case NEXT_ACTION_SEEK_G0:
		if (!cnc_line(gc.position[X_AXIS], gc.position[Y_AXIS], gc.position[Z_AXIS], 0, moveLength, feed_rate))  // mm * 60000/(mm/min) = msec
			return GCSTATUS_CANCELED;
		break;
	case NEXT_ACTION_LINEAR_G1:
		if (!cnc_line(gc.position[X_AXIS], gc.position[Y_AXIS], gc.position[Z_AXIS], gc.extruder_length, moveLength, feed_rate))
			return GCSTATUS_CANCELED;
		break;
	case NEXT_ACTION_CW_ARC:
	case NEXT_ACTION_CCW_ARC:
		if (radius_mode)
		{
			double h_x2_div_d;
			/* We need to calculate the center of the circle that has the designated radius and passes
				   through both the current position and the target position. This method calculates the following
				   set of equations where [x,y] is the vector from current to target position, d == magnitude of
				   that vector, h == hypotenuse of the triangle formed by the radius of the circle, the distance to
				   the center of the travel vector. A vector perpendicular to the travel vector [-y,x] is scaled to the
				   length of h [-y/d*h, x/d*h] and added to the center of the travel vector [x/2,y/2] to form the new point
				   [i,j] at [x/2-y/d*h, y/2+x/d*h] which will be the center of our arc.

				   d^2 == x^2 + y^2
				   h^2 == r^2 - (d/2)^2
				   i == x/2 - y/d*h
				   j == y/2 + x/d*h

				   O <- [i,j]
				   -  |
				   r      -     |
				   -        |
				   -           | h
				   -              |
				   [0,0] ->  C -----------------+--------------- T  <- [x,y]
				   | <------ d/2 ---->|

				   C - Current position
				   T - Target position
				   O - center of circle that pass through both C and T
				   d - distance from C to T
				   r - designated radius
				   h - distance from center of CT to O

				   Expanding the equations:

				   d -> sqrt(x^2 + y^2)
				   h -> sqrt(4 * r^2 - x^2 - y^2)/2
				   i -> (x - (y * sqrt(4 * r^2 - x^2 - y^2)) / sqrt(x^2 + y^2)) / 2
				   j -> (y + (x * sqrt(4 * r^2 - x^2 - y^2)) / sqrt(x^2 + y^2)) / 2

				   Which can be written:

				   i -> (x - (y * sqrt(4 * r^2 - x^2 - y^2))/sqrt(x^2 + y^2))/2
				   j -> (y + (x * sqrt(4 * r^2 - x^2 - y^2))/sqrt(x^2 + y^2))/2

				   Which we for size and speed reasons optimize to:

				   h_x2_div_d = sqrt(4 * r^2 - x^2 - y^2)/sqrt(x^2 + y^2)
				   i = (x - (y * h_x2_div_d))/2
				   j = (y + (x * h_x2_div_d))/2

				   */
			h_x2_div_d = -sqrt(4 * radius*radius - dx*dx - dy*dy) / hypot(dx, dy); // == -(h * 2 / d)
			// If r is smaller than d, the arc is now traversing the complex plane beyond the reach of any
			// real CNC, and thus - for practical reasons - we will terminate promptly:
			if (isnan(h_x2_div_d))
				FAIL(GCSTATUS_FLOATING_POINT_ERROR);
			// Invert the sign of h_x2_div_d if the circle is counter clockwise (see sketch below)
			if (gc.next_action == NEXT_ACTION_CCW_ARC)
				h_x2_div_d = -h_x2_div_d;

			/* The counter clockwise circle lies to the left of the target direction. When offset is positive,
			   the left hand circle will be generated - when it is negative the right hand circle is generated.
			   T  <-- Target position
			   ^
			   Clockwise circles with this center         |          Clockwise circles with this center will have
			   will have > 180 deg of angular travel      |          < 180 deg of angular travel, which is a good thing!
			   \         |          /
			   center of arc when h_x2_div_d is positive ->  x <----- | -----> x <- center of arc when h_x2_div_d is negative
			   |
			   |
			   C  <-- Current position                                 */
			// Negative R is g-code-alese for "I want a circle with more than 180 degrees of travel" (go figure!),
			// even though it is advised against ever generating such circles in a single line of g-code. By
			// inverting the sign of h_x2_div_d the center of the circles is placed on the opposite side of the line of
			// travel and thus we get the unadvisably long arcs as prescribed.
			if (radius < 0)
			{
				h_x2_div_d = -h_x2_div_d;
				radius = -radius; // Finished with r. Set to positive for mc_arc
			}
			// Complete the operation by calculating the actual center of the arc
			offset[X_AXIS] = 0.5 * (dx - (dy * h_x2_div_d));
			offset[Y_AXIS] = 0.5 * (dy + (dx * h_x2_div_d));
		}
		else
		{	// Offset mode specific computations
			radius = hypot(offset[X_AXIS], offset[Y_AXIS]); // Compute arc radius for mc_arc
		}
		mc_arc(oldPosition, gc.position, offset, gc.feed_rate, radius, gc.next_action == NEXT_ACTION_CW_ARC);
		break;
#if (USE_EXTRUDER == 1)
	case NEXT_ACTION_EXTRUDER_STOP:
		cnc_extruder_stop();
		gc.extruder_on = false;
		break;
	case NEXT_ACTION_EXTRUDER_ON:
		cnc_extruder_on();
		gc.extruder_on = true;
		break;
	case NEXT_ACTION_EXTRUDER_FAST_T:
		cnc_extruder_t(gc.s_value, false);
		break;
	case NEXT_ACTION_EXTRUDER_WAIT_T:
		cnc_extruder_t(gc.s_value, true);
		break;
#endif
	}
	return(gc.status_code);
}
