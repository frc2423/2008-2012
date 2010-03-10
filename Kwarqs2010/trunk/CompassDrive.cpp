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
/*
 * turnHigh sets how fast the robot can turn
 * turnOn sets the lowest angle distance the robot is NOT spinning at full speed
 */

double CompassDrive::turnRate(float turnHigh, float turnOn)
{
	if( turnHigh > 1.0 )
		turnHigh = 1.0;
	if( turnHigh < 0.0 )
		turnHigh = 0.0;
	
	if( turnOn > 1.0 )
		turnOn = 1.0;
	if( turnOn < 0.0 )
		turnOn = 0.0;
	
	float direction_Stick = m_resources.stick.GetDirectionDegrees();
	float direction_Robot = m_resources.gyro.GetAngle();
	
	double rate;
	
	if( (angle_distance( direction_Stick, direction_Robot ) / 180.0) > turnOn)
	{	
		rate = turnHigh;
	}
	else
	{
		float maxRate = 180.0 * turnOn;
		rate = turnHigh * angle_distance( direction_Stick, direction_Robot ) / maxRate;
	}
	
	int short_Direction = shortDirection(direction_Stick, direction_Robot);
	
	
	
	return rate * (float)short_Direction;
}
