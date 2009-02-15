/**
	\file 		KwarqsConstants.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$

	This file contains initialization definitions that define channel number
	or other needed data for motors and sensors. The idea is to keep all of
	the configuration data in a central place so that its easier to modify
	later on if we need to move things around. 
*/

#ifndef FRAMEWORK_KWARQS_CONSTANTS_H
#define FRAMEWORK_KWARQS_CONSTANTS_H


#define SLOT_1 4
#define SLOT_2 6

// operator station
#define FIRST_JOYSTICK_PORT			1
#define SECOND_JOYSTICK_PORT		2

// analog sidecar
#define GYRO_CHANNEL				1

#define ACCELEROMETER_X_CHANNEL		3
#define ACCELEROMETER_Y_CHANNEL		4	

// digital sidecar
//
//	LF: left-front
//	LR: left-rear
//	RF: right-front
//	RR: right-rear
//

// drive motors

#define MOTOR_LF_SLOT				SLOT_1
#define MOTOR_LR_SLOT				SLOT_1
#define MOTOR_RF_SLOT				SLOT_2
#define MOTOR_RR_SLOT				SLOT_2

#define PWM_LF_JAGUAR				1
#define PWM_LR_JAGUAR				3
#define PWM_RF_JAGUAR				3
#define PWM_RR_JAGUAR				1

// displacement of motors from center of rotation
#define LF_DISPLACEMENT				-1,1
#define LR_DISPLACEMENT				-1,-1
#define RF_DISPLACEMENT				1,1
#define RR_DISPLACEMENT				1,-1

// encoder channels: motor
#define ENCODER_MOTOR_LF_1			5
#define ENCODER_MOTOR_LF_2			14
#define ENCODER_MOTOR_LR_1			7
#define ENCODER_MOTOR_LR_2			8
#define ENCODER_MOTOR_RF_1			5
#define ENCODER_MOTOR_RF_2			6
#define ENCODER_MOTOR_RR_1			7
#define ENCODER_MOTOR_RR_2			8


// servos
#define SERVO_LF_SLOT				SLOT_1
#define SERVO_LR_SLOT				SLOT_1
#define SERVO_RF_SLOT				SLOT_2
#define SERVO_RR_SLOT				SLOT_2

#define PWM_LF_VICTOR				2
#define PWM_LR_VICTOR				4
#define PWM_RF_VICTOR				2
#define PWM_RR_VICTOR				4

#define SERVO_LF_INVERT				false
#define SERVO_LR_INVERT				true
#define SERVO_RF_INVERT				false
#define SERVO_RR_INVERT				true


// encoder channels: servo
#define ENCODER_SERVO_LF_1			1
#define ENCODER_SERVO_LF_2			2
#define ENCODER_SERVO_LR_1			3
#define ENCODER_SERVO_LR_2			4
#define ENCODER_SERVO_RF_1			1
#define ENCODER_SERVO_RF_2			2
#define ENCODER_SERVO_RR_1			3
#define ENCODER_SERVO_RR_2			4

// servo calibration signals
#define SERVO_CAL_LF				9
#define SERVO_CAL_LR				10
#define SERVO_CAL_RF				11
#define SERVO_CAL_RR				10

// servo constants
#define SERVO_F_SCALE				25
#define SERVO_F_TICKS				3272

#define SERVO_R_SCALE				25
#define SERVO_R_TICKS				3272

// global servo constants
#define SERVO_PID_P					0.95
#define SERVO_PID_I					0.0
#define SERVO_PID_D					0.0

// aggregated parameters (just ignore these)

#define SERVO_PARAMS(lr, fr)		SERVO_##lr##fr##_SLOT, \
									PWM_##lr##fr##_VICTOR, \
									ENCODER_SERVO_##lr##fr##_1, \
									ENCODER_SERVO_##lr##fr##_2, \
									SERVO_CAL_##lr##fr, \
									SERVO_##fr##_SCALE, \
									SERVO_##fr##_TICKS, \
									SERVO_##lr##fr##_INVERT

#define SERVO_LF_PARAMETERS 		SERVO_PARAMS(L,F)
#define SERVO_LR_PARAMETERS			SERVO_PARAMS(L,R)
#define SERVO_RF_PARAMETERS			SERVO_PARAMS(R,F)
#define SERVO_RR_PARAMETERS			SERVO_PARAMS(R,R)


#endif
