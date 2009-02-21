/**
	\file 		CompassDriveII.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>

#include "CompassDriveII.h"

#include "../Framework/math.h"
#include "../Framework/KwarqsConstants.h"
#include "../Framework/DriverStationLCD.h"


CompassDriveII::CompassDriveII(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_stick(FIRST_JOYSTICK_PORT),
	m_stick2(SECOND_JOYSTICK_PORT),
	m_position(PositionInformation::GetInstance())
{}



void CompassDriveII::Move()
{	
	double speed, wheel_Direction;
	
	// normal compass drive stuff here
	speed = CalculateSpeed();
	wheel_Direction = CalculateWheelDirection();
	
	// special compass drive II thing: adjust the front of the
	// robot to where the nose should be
	double y2 = m_stick2.GetY() * -1, x2 = m_stick2.GetX();
	double rotation = 0;
	
	// if we're meant to be rotating, do so
	if (fabs(__hypot(x2, y2)) > 0.0)
	{
		double face_angle = (atan2(y2, x2) * (180/M_PI) - 90.0 );			
		if (face_angle < 0) face_angle += 360;

		rotation = m_nosePointer.GetRotation(face_angle);
	}
	
	m_driveController->Move(speed, wheel_Direction, rotation, m_stick.GetTop());
}

double CompassDriveII::robotCompass()
{
	double robot_Compass = m_position->GetAngle();

	robot_Compass = fmod(robot_Compass, 360);
	if (robot_Compass < 0)
	{
		robot_Compass = robot_Compass + 360;
	}
	
	return robot_Compass;
}


//gets angle from controller. Does not need to be called by user. Use angleChange instead.
double CompassDriveII::controllerCompass()
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


double CompassDriveII::CalculateWheelDirection()
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

double CompassDriveII::CalculateSpeed()
{
	return __hypot(m_stick.GetX(), m_stick.GetY()*-1);
}





