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

// analog sidecar
#define GYRO_CHANNEL				1

#define ACCELEROMETER_X_CHANNEL		3
#define ACCELEROMETER_Y_CHANNEL		4	

// digital sidecar 

// pwm channels
#define LEFT_MOTOR_PWM				1
#define RIGHT_MOTOR_PWM				2


// operator station
#define FIRST_JOYSTICK_PORT			1
#define SECOND_JOYSTICK_PORT		2

#endif
