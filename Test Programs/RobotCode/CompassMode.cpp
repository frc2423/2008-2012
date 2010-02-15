#include "CompassMode.h"
#include <math.h>
#include "Error.h"



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
	float direction_Stick = fmod(m_resources.stick.GetDirectionDegrees(), 360.0);
	float direction_Robot = fmod(m_resources.gyro.GetAngle(), 360.0);
	
	float rotationDirection_Stick = fabs(direction_Stick) / d_err(direction_Stick);
	float rotationDirection_Robot = fabs(direction_Robot) / d_err(direction_Robot);
	
	if(fabs(direction_Stick) > 180.0) 
		direction_Stick = rotationDirection_Stick * (direction_Stick - 360.0);
	
	if(fabs(direction_Robot) > 180.0)
			direction_Robot = rotationDirection_Robot * (direction_Robot - 360.0);
	
	
	float direction_Err = direction_Stick - direction_Robot;
	
	float turn_Rate = direction_Err / 180.0;
	
	if(m_resources.stick.GetDirectionDegrees() <= 0.0) 
		return (-1.0 * turn_const * turn_Rate);
	else 
		return (turn_const * turn_Rate); 
}
