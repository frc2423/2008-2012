/**
	\file 		RobotChassis.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-02-16 03:26:31 -0500 (Mon, 16 Feb 2009) $
	\version 	$Rev: 164 $
*/

#ifndef ROBOTCHASSIS_H
#define ROBOTCHASSIS_H

#include "Framework/KwarqsWheelMotor.h"
#include "Framework/KwarqsWheelServo.h"
#include "Framework/KwarqsConstants.h"

/**
	\brief Container class to hold shared chassis items
*/
struct RobotChassis
{
	// servos
	KwarqsWheelServo		servo_lf;
	KwarqsWheelServo		servo_rf;
	KwarqsWheelServo		servo_lr;
	KwarqsWheelServo		servo_rr;

	// motors
	KwarqsWheelMotor		motor_lf;
	KwarqsWheelMotor		motor_rf;
	KwarqsWheelMotor		motor_lr;
	KwarqsWheelMotor		motor_rr;

	
	RobotChassis() :
		servo_lf( SERVO_LF_PARAMETERS ),
		servo_rf( SERVO_RF_PARAMETERS ),
		servo_lr( SERVO_LR_PARAMETERS ),
		servo_rr( SERVO_RR_PARAMETERS ),
		
		motor_lf(MOTOR_LF_PARAMETERS ),
		motor_rf(MOTOR_RF_PARAMETERS ),
		motor_lr(MOTOR_LR_PARAMETERS ),
		motor_rr(MOTOR_RR_PARAMETERS )
	{}
};

#endif
