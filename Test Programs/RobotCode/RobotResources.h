#ifndef ROBOTRESOURCES_H
#define ROBOTRESOURCES_H

#include <WPILib.h>
#include <WebDMA/WebDMA.h>

#define WHEEL_RADIUS (0.1016 * (15.0 / 40.0))
#define WHEEL_BASE 0.55245
#define SLOT_1 4
#define SLOT_2 6
#define SLOT SLOT_1

struct RobotResources
{
	Encoder leftEncoder;	// detects the speed that the wheels are turning at
	Encoder rightEncoder;
	Gyro gyro;
	
	Jaguar roller;
	Joystick stick;			// only joystick
	RobotDrive myRobot; 	// robot drive system

	WebDMA webdma;
	
	RobotResources():
		leftEncoder(SLOT, 1, SLOT, 2),
		rightEncoder(SLOT, 3, SLOT, 4),
		gyro(1),
		
		roller(SLOT, 3),
		stick(1),			// these must be initialized in the same order
		myRobot(1, 2)		// as they are declared above.
	
	{
		gyro.SetSensitivity(0.007);
	}
	
};

#endif
