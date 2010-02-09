#ifndef EXAMPLEMODE.H
#define EXAMPLEMODE.H

#include <WPILib.h>

class ExampleMode: public RobotMode
{
public:
	ExampleMode(const RobotResources& resources):
		leftEncoder(resources.GetLeftEncoder()),
		rightEncoder(resources.GetRightEncoder()),
		gyro(resources.GetGyro()),
		joystick(resources.GetJoystick())
	{};
		
	void Main()
	{
		
	}
	
	
private:
	Encoder leftEncoder;
	Encoder rightEncoder;
	Gyro gyro;
	Joystick joystick;
};