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

#define CALIBRATION_SWITCH			1

#define BCD_IN_3 1
#define BCD_IN_2 2
#define BCD_IN_1 4
#define BCD_IN_0 5

#define DRIVE_RECORDER_STICK 		2
#define DRIVE_RECORDER_SWITCH		1

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

// testbot:

#define MOTOR_L_SLOT				SLOT_1
#define MOTOR_R_SLOT				SLOT_1

#define PWM_L_JAGUAR					1
#define PWM_R_JAGUAR					2

#define MOTOR_L_INVERT				false
#define MOTOR_R_INVERT				false

#define ENCODER_MOTOR_L_1			1
#define ENCODER_MOTOR_L_2			2
#define ENCODER_MOTOR_R_1			3
#define ENCODER_MOTOR_R_2			4

#define ENCODER_MOTOR_L_INVERT		true
#define ENCODER_MOTOR_R_INVERT		false


#define LEFT_MOTOR_PWM 1
#define RIGHT_MOTOR_PWM 2

// drive motors

#define MOTOR_LF_SLOT				SLOT_1
#define MOTOR_LR_SLOT				SLOT_1
#define MOTOR_RF_SLOT				SLOT_2
#define MOTOR_RR_SLOT				SLOT_2

#define MOTOR_LF_INVERT				true
#define MOTOR_LR_INVERT				true
#define MOTOR_RF_INVERT				false
#define MOTOR_RR_INVERT				false

#define PWM_LF_JAGUAR				1
#define PWM_LR_JAGUAR				3
#define PWM_RF_JAGUAR				1
#define PWM_RR_JAGUAR				3

// displacement of motors from center of rotation
#define LF_DISPLACEMENT				-0.5,1
#define LR_DISPLACEMENT				-0.5,-1
#define RF_DISPLACEMENT				0.5,1
#define RR_DISPLACEMENT				0.5,-1

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

#define SERVO_LF_VICTOR				2
#define SERVO_LR_VICTOR				4
#define SERVO_RF_VICTOR				2
#define SERVO_RR_VICTOR				4

#define SERVO_LF_INVERT_MOTOR		true
#define SERVO_LF_INVERT_ENCODER		true
#define SERVO_LR_INVERT_MOTOR		true
#define SERVO_LR_INVERT_ENCODER		false
#define SERVO_RF_INVERT_MOTOR		false
#define SERVO_RF_INVERT_ENCODER		false
#define SERVO_RR_INVERT_MOTOR 		true
#define SERVO_RR_INVERT_ENCODER		false


// encoder channels: servo
#define ENCODER_SERVO_LF_1			1
#define ENCODER_SERVO_LF_2			2
#define ENCODER_SERVO_LR_1			3
#define ENCODER_SERVO_LR_2			4
#define ENCODER_SERVO_RF_1			1
#define ENCODER_SERVO_RF_2			2
#define ENCODER_SERVO_RR_1			3
#define ENCODER_SERVO_RR_2			4

// servo calibration signals (hall effect sensors)
#define SERVO_LF_CAL_PORT			9
#define SERVO_LR_CAL_PORT			10
#define SERVO_RF_CAL_PORT			11
#define SERVO_RR_CAL_PORT			10

// angle that the calibration point represents (in degrees)
#define SERVO_LF_CAL_OFFSET			0.0
#define SERVO_LR_CAL_OFFSET			0.0	
#define SERVO_RF_CAL_OFFSET			0.0
#define SERVO_RR_CAL_OFFSET			0.0

// servo constants (front)
#define SERVO_F_SCALE				25.0
#define SERVO_F_TICKS				3272

// servo constants (back)
#define SERVO_R_SCALE				25.0
#define SERVO_R_TICKS				3272

// global servo constants
#define SERVO_PID_P					0.95
#define SERVO_PID_I					0.0
#define SERVO_PID_D					0.0

// aggregated parameters (just ignore these)

#define SERVO_PARAMS(lr, fr)		SERVO_##lr##fr##_SLOT, \
									SERVO_##lr##fr##_VICTOR, \
									ENCODER_SERVO_##lr##fr##_1, \
									ENCODER_SERVO_##lr##fr##_2, \
									SERVO_##lr##fr##_CAL_PORT, \
									SERVO_##lr##fr##_CAL_OFFSET, \
									SERVO_##fr##_SCALE, \
									SERVO_##fr##_TICKS, \
									SERVO_##lr##fr##_INVERT_MOTOR, \
									SERVO_##lr##fr##_INVERT_ENCODER

#define SERVO_LF_PARAMETERS 		SERVO_PARAMS(L,F)
#define SERVO_LR_PARAMETERS			SERVO_PARAMS(L,R)
#define SERVO_RF_PARAMETERS			SERVO_PARAMS(R,F)
#define SERVO_RR_PARAMETERS			SERVO_PARAMS(R,R)


#define MOTOR_PARAMS(arg)			MOTOR_##arg##_SLOT, \
									PWM_##arg##_JAGUAR, \
									ENCODER_MOTOR_##arg##_1, \
									ENCODER_MOTOR_##arg##_2, \
									MOTOR_##arg##_INVERT, \
									ENCODER_MOTOR_##arg##_INVERT

#define MOTOR_LF_PARAMETERS			MOTOR_PARAMS(LF)
#define MOTOR_LR_PARAMETERS			MOTOR_PARAMS(LR)
#define MOTOR_RF_PARAMETERS			MOTOR_PARAMS(RF)
#define MOTOR_RR_PARAMETERS			MOTOR_PARAMS(RR)

#define MOTOR_L_PARAMETERS			MOTOR_PARAMS(L)
#define MOTOR_R_PARAMETERS			MOTOR_PARAMS(R)




#endif
