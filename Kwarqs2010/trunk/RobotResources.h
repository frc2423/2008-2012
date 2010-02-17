#ifndef ROBOTRESOURCES_H
#define ROBOTRESOURCES_H

#include <WPILib.h>
#include <WebDMA/WebDMA.h>

#define WHEEL_RADIUS (0.1016 * (15.0 / 40.0))
#define WHEEL_BASE 0.55245
#define DIGITAL_SLOT_1 4
#define DIGITAL_SLOT_2 6
#define DIGITAL_SLOT DIGITAL_SLOT_1
#define ANALOG_SLOT 1

struct RobotResources
{
	Encoder leftEncoder;	// detects the speed that the wheels are turning at
	Encoder rightEncoder;
	Gyro gyro;
	
	Joystick stick;			// only joystick
	Joystick stick2;
	RobotDrive myRobot; 	// robot drive system

	WebDMA webdma;
	
	RobotResources():
		leftEncoder(DIGITAL_SLOT, 1, DIGITAL_SLOT, 2),
		rightEncoder(DIGITAL_SLOT, 3, DIGITAL_SLOT, 4),
		gyro(ANALOG_SLOT),
		
		stick(1),			// these must be initialized in the same order
		stick2(2),
		myRobot(1, 2)		// as they are declared above.
	
	{
		gyro.SetSensitivity(0.007);
	}
	
};

#endif
