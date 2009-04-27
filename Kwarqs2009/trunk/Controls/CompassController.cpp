/**
	\file 		CompassController.cpp
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>

#include "CompassController.h"

#include "../KwarqsLib/math.h"
#include "../KwarqsLib/KwarqsConstants.h"


CompassController::CompassController(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	m_controller(CONTROLLER_JOYSTICK_PORT),
	m_position(PositionInformation::GetInstance()),
	m_nosePointer("CompassController"),
	m_spinEvent(0.025),
	m_noseDirection(0)
{	
	m_bigButtonAngle = WebInterface::CreateDoubleProxy("CompassController", "High Adjustment", 
		DoubleProxyFlags().default_value(35).minval(-180).maxval(180).step(.1));
		
	m_smallButtonAngle = WebInterface::CreateDoubleProxy("CompassController", "Low Adjustment", 
		DoubleProxyFlags().default_value(15).minval(-180).maxval(180).step(.1));
	
	m_override = WebInterface::CreateBoolProxy("CompassController", "Override", false);
	
	m_speed = WebInterface::CreateDoubleProxy("CompassController", "Speed", 
		DoubleProxyFlags().default_value(0).minval(-1).maxval(1).step(.1));
	
	m_heading = WebInterface::CreateDoubleProxy("CompassController", "Heading", 
		DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(1));
	
	m_angle = WebInterface::CreateDoubleProxy("CompassController", "Angle", 
		DoubleProxyFlags().default_value(0).minval(0).maxval(360).step(1));
}



void CompassController::Move()
{	
	double speed, wheel_Direction;
	
	// normal compass drive stuff here
	speed = CalculateSpeed();
	wheel_Direction = CalculateWheelDirection();
	
	// adjust the front of the robot to where the nose should be
	double y2 = m_controller.GetRawAxis(4) * -1, x2 = m_controller.GetRawAxis(3);
	double rotation = 0;
	
	// if we're asked to point in a particular direction, do it
	if (fabs(__hypot(x2, y2)) > 0.25)
	{
		m_noseDirection = (atan2(y2, x2) * (180/M_PI) - 90.0 );	
	}
	else if (m_spinEvent.DoEvent())
	{
		// use the buttons to spin around 
		
		// idea: it may be a useful idea to eventually drift back 
		// to the 'real' nose position if the person lets go of the
		// buttons, so the bot doesn't just keep turning unexpectedly
		
		double big = m_bigButtonAngle;
		double small = m_smallButtonAngle;
		
		double current_angle = m_position->GetNormalizedFieldAngle()*-1;
		
		// go left
		if (m_controller.GetRawButton(7))
			m_noseDirection = current_angle + big;
		
		// go right
		if (m_controller.GetRawButton(8))
			m_noseDirection = current_angle - big;

		// go left a tiny bit
		if (m_controller.GetRawButton(5))
			m_noseDirection = current_angle + small;
		
		// go right a tiny bit
		if (m_controller.GetRawButton(6))
			m_noseDirection = current_angle - small;
	}
	
	if (m_override)
	{
		speed = m_speed;
		m_noseDirection = m_angle;
		wheel_Direction = m_heading;
	}

	rotation = m_nosePointer.GetRotation(m_noseDirection);

	m_driveController->Move(speed, wheel_Direction, rotation, m_controller.GetRawButton(1));
}

double CompassController::robotCompass()
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
double CompassController::controllerCompass()
{
	double controller_Compass;
	double joystick_x = m_controller.GetX(), joystick_y = m_controller.GetY() * -1;
	
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


double CompassController::CalculateWheelDirection()
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

double CompassController::CalculateSpeed()
{
	return __hypot(m_controller.GetX(), m_controller.GetY()*-1);
}





