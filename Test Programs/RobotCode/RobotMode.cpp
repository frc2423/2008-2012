#include "RobotMode.h"

RobotMode::available_Modes = 0;

RobotMode::RobotMode(const RobotResources& resources):
m_gyro(resources.GetGyro()),
m_leftEncoder(resources.GetLeftEncoder()),
m_rightEncoder(resources.GetRightEncoder()),
m_joystick(resources.GetJoystick()),
m_mode(0)


{
	
	
}

//Calls a RobotMode by number
void RobotMode::mode(void)
{
	int mode = m_mode % available_modes;
	
	switch(mode)
	{
		case 1:
			//mode 1 is called here
			break;
		default:
			//default mode is called here
			break;
	}
}

//Write mode code here