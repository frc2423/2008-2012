#ifndef ROBOTRESOURCES.H
#define ROBOTRESOURCES.H

#include <WPILib.h>

struct RobotResources
{
	Encoder leftEncoder;
	Encoder rightEncoder;
	Gyro gyro;
	Joystick joystick;
	
	RobotResources():
		leftEncoder(),
		rightEncoder(),
		gyro(),
		joystick()
	{};

public:
	Encoder& GetLeftEncoder(void){return leftEncoder;};
	Encoder& GetRightEncoder(void){return rightEncoder;};
	Gyro& GetGyro(void){return gyro;};
	Joystick& GetJoystick(void){return joystick;};
	
	RobotResources resources;


};


	
