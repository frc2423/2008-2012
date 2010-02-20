#include "CompassMode.h"
#include <math.h>
#include "Error.h"
#include "kwarqs_math.h"



CompassMode::CompassMode(RobotResources& resources):
	m_resources(resources)
{}

void CompassMode::OnEnable()
{}

void CompassMode::Main()
{
	m_resources.myRobot.Drive(speed(), turnRate());
}

void CompassMode::OnDisable()
{}


double CompassMode::speed()
{
	/* float direction = fabs(m_resources.stick.GetY()) / m_resources.stick.GetY();
	 * return (m_resources.stick.GetMagnitude() * direction);
	*/
	
	return (m_resources.stick.GetMagnitude());
}
 
double CompassMode::turnRate()
{
	float direction_Stick = m_resources.stick.GetDirectionDegrees();
	float direction_Robot = m_resources.gyro.GetAngle();
	
	double rate = angle_distance( direction_Stick, direction_Robot );
	double short_Direction = shortDirection(direction_Stick, direction_Robot);
	
	return rate * short_Direction;
}
