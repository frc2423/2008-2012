#ifndef __user_program_h_
#define __user_program_h_

#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"

/* set up a few descriptive aliases */
#define RightTrigState p2_sw_trig
#define LeftTrigState  p1_sw_trig

#define leftJoystickMotorCtl_p1_y p1_y	//inputs
#define leftJoystickMotorCtl_p1_x p1_x
#define leftMotorOut1_pwm03 pwm03		//outputs
#define leftMotorOut2_pwm04 pwm04

#define rightJoystickMotorCtl_p2_y p2_y	//inputs
#define rightMotorOut1_pwm01 pwm01		//outputs
#define rightMotorOut2_pwm02 pwm02

#define irButton2 rc_dig_in13
#define irButton1 rc_dig_in14
#define irButton3 rc_dig_in15
#define irButton0 rc_dig_in16

/*
 * prototype for the entry function for the 
 * kwarqs robot routines
*/
void KwarqsStart(void);
















#endif