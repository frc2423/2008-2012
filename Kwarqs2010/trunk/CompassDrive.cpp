/**
	\file 		CompassMode.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#include "CompassDrive.h"
#include <math.h>
#include "kwarqs_math.h"


CompassDrive::CompassDrive(RobotResources& resources):
	m_resources(resources)
{}


double CompassDrive::speed()
{
	/* float direction = fabs(m_resources.stick.GetY()) / m_resources.stick.GetY();
	 * return (m_resources.stick.GetMagnitude() * direction);
	*/
	
	return (m_resources.stick.GetMagnitude());
}

//Makes the robot face a specific direction relative to gyro angle
double CompassDrive::turnRate(float direction_Stick)
{
	float direction_Robot = m_resources.gyro.GetAngle();
	
	double rate = angle_distance( direction_Stick, direction_Robot );
	double short_Direction = shortDirection(direction_Stick, direction_Robot);
	
	return rate * short_Direction;
}
