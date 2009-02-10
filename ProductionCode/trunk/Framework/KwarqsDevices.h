/**
	\file 		KwarqsDevices.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$

	This file contains initialization definitions that define channel number
	or other needed data for motors and sensors. The idea is to keep all of
	the configuration data in a central place so that its easier to modify
	later on if we need to move things around. 
*/

#ifndef FRAMEWORK_KWARQS_DEVICES_H
#define FRAMEWORK_KWARQS_DEVICES_H

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

#define MOTOR_LF_SLOT				4
#define MOTOR_LR_SLOT				4
#define MOTOR_RF_SLOT				5
#define MOTOR_RR_SLOT				5

#define PWM_LF_JAGUAR				1
#define PWM_LR_JAGUAR				2
#define PWM_RF_JAGUAR				1
#define PWM_RR_JAGUAR				2

// encoder channels: motor
#define ENCODER_MOTOR_LF_1			1
#define ENCODER_MOTOR_LF_2			1
#define ENCODER_MOTOR_LR_1			1
#define ENCODER_MOTOR_LR_2			1
#define ENCODER_MOTOR_RF_1			1
#define ENCODER_MOTOR_RF_2			1
#define ENCODER_MOTOR_RR_1			1
#define ENCODER_MOTOR_RR_2			1


// servos
#define SERVO_LF_SLOT				4
#define SERVO_LR_SLOT				4
#define SERVO_RF_SLOT				5
#define SERVO_RR_SLOT				5

#define PWM_LF_VICTOR				3
#define PWM_LR_VICTOR				4
#define PWM_RF_VICTOR				3
#define PWM_RR_VICTOR				4

// encoder channels: servo
#define ENCODER_SERVO_LF_1			1
#define ENCODER_SERVO_LF_2			1
#define ENCODER_SERVO_LR_1			1
#define ENCODER_SERVO_LR_2			1
#define ENCODER_SERVO_RF_1			1
#define ENCODER_SERVO_RF_2			1
#define ENCODER_SERVO_RR_1			1
#define ENCODER_SERVO_RR_2			1

// servo calibration signals
#define SERVO_CAL_LF				1
#define SERVO_CAL_LR				1
#define SERVO_CAL_RF				1
#define SERVO_CAL_RR				1

// servo constants
#define SERVO_F_SCALE				45
#define SERVO_F_TICKS				1000

#define SERVO_R_SCALE				45
#define SERVO_R_TICKS				1000


#endif
