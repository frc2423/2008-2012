/**
	\file 		CompassDrive.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>

#include "../KwarqsLib/math.h"
#include "../KwarqsLib/KwarqsConstants.h"

#include "CompassDrive.h"


CompassDrive::CompassDrive(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_stick(FIRST_JOYSTICK_PORT),
	m_position(PositionInformation::GetInstance()),
	m_twistEvent(0.05),
	m_noseAngle(0)
{}


void CompassDrive::Move()
{	
	double speed, wheel_Direction, rotation;
	
	speed = CalculateSpeed();
	wheel_Direction = CalculateWheelDirection();
	
	// every period of time, add a bit to the angle depending on the twist
	if (m_twistEvent.DoEvent())
		m_noseAngle = (m_position->GetNormalizedFieldAngle()*-1) + (m_stick.GetTwist() * -25);
	
	// continuously adjust the nose so we're always pointing the same
	// direction relative to the field
	rotation = m_nosePointer.GetRotation(m_noseAngle);
	
	m_driveController->Move(speed, wheel_Direction, rotation, m_stick.GetTop());
}

double CompassDrive::robotCompass()
{
	double robot_Compass = m_position->GetFieldAngle();

	robot_Compass = fmod(robot_Compass, 360);
	if (robot_Compass < 0)
	{
		robot_Compass = robot_Compass + 360;
	}
	
	return robot_Compass;
}


//gets angle from controller. Does not need to be called by user. Use angleChange instead.
double CompassDrive::controllerCompass()
{
	double controller_Compass;
	double joystick_x = m_stick.GetX(), joystick_y = m_stick.GetY() * -1;
	
	if ((joystick_x > 0) && (joystick_y > 0))
	{
		controller_Compass = (atan(joystick_x/joystick_y) * 57.29577951);
	}
	else if ((joystick_x > 0) && (joystick_y < 0))
	{
        controller_Compass = (atan((joystick_y * -1)/(joystick_x)) * 57.29577951) + 90;
    }
	else if ((joystick_x < 0) && (joystick_y < 0))
	{
		controller_Compass = (atan((joystick_x * -1)/(joystick_y * -1))) * 57.29577851 +180;
	}
	else if ((joystick_x < 0) && (joystick_y > 0))
	{
         controller_Compass = (atan(joystick_y/(joystick_x * -1)) * 57.29577851) + 270;
    }
	else if ((joystick_x == 0) && (joystick_y > 0))
	{
		controller_Compass = 0;
	}
	else if ((joystick_x > 0) && (joystick_y == 0))
	{
		controller_Compass = 90;
	}
	else if ((joystick_x == 0) && (joystick_y < 0))
	{
		controller_Compass = 180;
	}
	else if ((joystick_x < 0) && (joystick_y == 0))
	{
		controller_Compass = 270;
	}
	else
	{
		controller_Compass = 0;
	}
	
	return controller_Compass;
}


double CompassDrive::CalculateWheelDirection()
{
	double robot_Compass = robotCompass();
	double controller_Compass = controllerCompass();
	double angleChange;
	

	if (robot_Compass - controller_Compass > 180)		                           
	{
		angleChange = robot_Compass - controller_Compass - 360;
	}
	else if (robot_Compass - controller_Compass < -180)
	{
		angleChange = robot_Compass - controller_Compass + 360;
	}
	else
	{
		angleChange = robot_Compass - controller_Compass;
	}
	
	return angleChange;
}

double CompassDrive::CalculateSpeed()
{
	return __hypot(m_stick.GetX(), m_stick.GetY()*-1);
}
	
	
		
		

