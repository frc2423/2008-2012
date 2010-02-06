#include "RobotResources.h"

RobotResources::RobotResources():
m_gyro(),
m_leftEncoder(),
m_rightEncoder(),
m_joystick()
{}

RobotResources* RobotResources::m_instance = NULL;


RobotResources& Resources()
{
	if(!RobotResources::m_instance)
		m_instance = new RobotResources;
	
	return(*RobotResources::m_instance)
}