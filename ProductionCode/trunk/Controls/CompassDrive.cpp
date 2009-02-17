

#include <WPILib.h>

#include "../Framework/math.h"
#include "../Framework/KwarqsConstants.h"

#include "CompassDrive.h"


CompassDrive::CompassDrive(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_stick(FIRST_JOYSTICK_PORT),
	m_position(PositionInformation::GetInstance())
{}


void CompassDrive::Move()
{	
	double speed, wheel_Direction, rotation;
	
	speed = CalculateSpeed();
	wheel_Direction = CalculateWheelDirection();
	rotation = pow(m_stick.GetTwist() * -1, 3);
	
	m_driveController->Move(speed, wheel_Direction, rotation, m_stick.GetTrigger());
}

double CompassDrive::robotCompass()
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
	
	
		
		

