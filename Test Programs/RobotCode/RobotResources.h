#ifndef ROBOTRESOURCES.H
#define ROBOTRESOURCES.H

#include <WPILib.h>

class RobotResources
{
public:
	Gyro& GetGyro(){return (*m_gyro)};
	Encoder& GetLeftEncoder(){return (*m_leftEncoder)};
	Encoder& GetRightEncoder(){return (*m_rightEncoder)};
	Joystick& GetJoystick(){return (*m_joystick)};
	
	
	friend RobotResources& Resources()

private:
	Gyro m_gyro;
	Encoder m_leftEncoder;
	Encoder m_rightEncoder;
	Joystick m_joystick;
	
	RobotResources();
	RobotResources (const RobotResouces&){};
	RobotResources& operator=(const RobotResources&){};
	static RobotResources* m_instance;
};
	
	
