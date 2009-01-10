#include <stdio.h>
#include "CompassDrive.h"
#include <cmath>


void CompassDrive::Drive(float robot_Compass, float joystick_x, float joystick_y, float joystick2_y)
{
	
	float angle_Change = angleChange(robot_Compass, joystick_x, joystick_y);
	float turn_Rate = turnRate(robot_Compass, joystick_x, joystick_y);
	float left_Motors = motorSpeedDuel(joystick2_y);
	float right_Motors = motorSpeedDuel(joystick2_y);

	left_Motors = tankLeftMotors(left_Motors, angle_Change, turn_Rate, left_Motors);
	right_Motors = tankRightMotors(right_Motors, angle_Change, turn_Rate, right_Motors);
	
	printf("Left Motor value: %f, Right Motor value: %f\n", left_Motors, right_Motors);
	
	

		
}




float CompassDrive::controllerCompass(float joystick_x, float joystick_y)                                          //gets angle from controller. Does not need to be called by user. Use angleChange instead.
{
	float controller_Compass;
	
	if ((joystick_y > 0) && (joystick_x > 0))
	{
		controller_Compass = (atan(joystick_y/joystick_x) * 57.29577951);
	}
	else if ((joystick_y < 0) && (joystick_x < 0))
	{
        controller_Compass = (atan((joystick_y * -1)/(joystick_x * -1)) * 57.29577951) + 180;
    }
	else if ((joystick_y > 0) && (joystick_x < 0))
	{
		controller_Compass = (atan((joystick_x * -1)/joystick_y) * 57.29577851) + 90;
	}
	else if ((joystick_y < 0) && (joystick_x > 0))
	{
         controller_Compass = (atan(joystick_x/(joystick_y * -1)) * 57.29577851) + 270;
    }
	else if ((joystick_y == 0) && (joystick_x > 0))
	{
		controller_Compass = 0;
	}
	else if ((joystick_y == 0) && (joystick_x < 0))
	{
		controller_Compass = 180;
	}
	else if ((joystick_y > 0) && (joystick_x == 0))
	{
		controller_Compass = 90;
	}
	else if ((joystick_y < 0) && (joystick_x == 0))
	{
		controller_Compass = 270;
	}
	
	return controller_Compass;
}

float CompassDrive::angleChange(float robot_Compass, float joystick_x, float joystick_y)                           //gets value from controller_Compass. Finds the angle the robot must turn.
{
	float angleChange;
	float controller_Compass = controllerCompass(joystick_x, joystick_y);
	
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

float CompassDrive::turnRate(float robot_Compass, float joystick_x, float joystick_y)                              //Finds how fast robot must turn based on angleChange().
{
    float angleChange_ = angleChange(robot_Compass, joystick_x, joystick_y);
    
	float turn_Rate;
	float abs_angleChange;
	
	if (angleChange_ < 0)
    {
        abs_angleChange = angleChange_ * -1;
    }
    else
    {
        abs_angleChange = angleChange_;
    }								                    
	
	if (abs_angleChange <= 10)
	{
		turn_Rate = .9;
	}
	else if (abs_angleChange <= 20)
	{
		turn_Rate = .8;
	}
	else if (abs_angleChange <= 45)
	{
		turn_Rate = .7;
	}
	else if (abs_angleChange <= 90)
	{
		turn_Rate = .6;
	}
	else if (abs_angleChange <= 180)
	{
		turn_Rate = .5;
	}
	return turn_Rate;
}

float CompassDrive::motorSpeedDuel(float joystick2_y)                                                               //Gives speed for two joystick control.
{
	if (joystick2_y < 0)
	{
		joystick2_y = 0;
	}
	return joystick2_y;
}


float CompassDrive::tankLeftMotors(float left_Motors, float angleChange, float turn_Rate, float speed)
{	
	if (angleChange < 0)
	{
		left_Motors = speed * turn_Rate;
	}
	return left_Motors;
}

float CompassDrive::tankRightMotors(float right_Motors, float angleChange, float turn_Rate, float speed)
{	
	if (angleChange > 0)
	{
		right_Motors = speed * turn_Rate;
	}
	return right_Motors;
}
