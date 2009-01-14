#include <stdio.h>
#include "CompassDrive.h"
#include <cmath>

CompassDrive::CompassDrive(SpeedController * leftMotor, SpeedController * rightMotor) :
	m_leftMotor(leftMotor),
	m_rightMotor(rightMotor)
{
}


void CompassDrive::Drive(float robot_Compass, float joystick_x, float joystick_y, float joystick2_y)
{
	
	float angle_Change = angleChange(robot_Compass, joystick_x, joystick_y);
	float turn_Rate = turnRate(robot_Compass, joystick_x, joystick_y, joystick2_y);

	left_Motors = tankLeftMotors(angle_Change, turn_Rate, joystick2_y);
	right_Motors = tankRightMotors(angle_Change, turn_Rate, joystick2_y);
	
	
	m_leftMotor->Set(left_Motors*-1);
	m_rightMotor->Set(right_Motors);
}




float CompassDrive::controllerCompass(float joystick_x, float joystick_y)                                          //gets angle from controller. Does not need to be called by user. Use angleChange instead.
{
	float controller_Compass;
	
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

float CompassDrive::angleChange(float robot_Compass, float joystick_x, float joystick_y)                           //gets value from controller_Compass. Finds the angle the robot must turn.
{
	float angleChange;
	float controller_Compass = controllerCompass(joystick_x, joystick_y);
	
	if ((robot_Compass > 360) || (robot_Compass < -360))
	{
		robot_Compass = fmod(robot_Compass, 360);
	}
	
	if (robot_Compass < 0)
	{
		robot_Compass = robot_Compass + 360;
	}
	
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

float CompassDrive::turnRate(float robot_Compass, float joystick_x, float joystick_y, float speed)                              //Finds how fast robot must turn based on angleChange().
{
    float angleChange_ = angleChange(robot_Compass, joystick_x, joystick_y);
    
	float max_Turn_Point = 90;
	float speed_Limit = 1.0;
	float speed = joystick2_y;
	
	float turn_Rate;
	
	if (angleChange_ < 0)
    {
        angleChange_ = angleChange_ * -1;
    }							                    
	
	if (angleChange_ > max_Turn_Point)
	{
		turn_Rate = speed_Limit;
	}
	else
	{
		turn_Rate = ((speed * angleChange_ * speed_Limit) / max_Turn_Point) + 1 - speed_Limit;
	}
	

	
	return turn_Rate;
}


float CompassDrive::tankLeftMotors(float angleChange, float turn_Rate, float speed)
{	
	if (angleChange > 0)
	{
		left_Motors = (turn_Rate * 2) -1;
	}
	else
	{
		left_Motors = speed;
	}
	
	return left_Motors;
}

float CompassDrive::tankRightMotors(float angleChange, float turn_Rate, float speed)
{	
	if (angleChange < 0)
	{
		right_Motors = (turn_Rate * 2) - 1;
	}
	else
	{
		right_Motors = speed;
	}
	
	return right_Motors;
}
