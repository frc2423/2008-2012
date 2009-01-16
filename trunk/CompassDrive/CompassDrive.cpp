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

	float left_Motors = tankLeftMotors(angle_Change, turn_Rate, joystick2_y);
	float right_Motors = tankRightMotors(angle_Change, turn_Rate, joystick2_y);
	
	
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
	float angleChange_H = .7;
	float angleChange_L = .1;
	float speed_H = .7;
	float speed_L = .1;
	
	float turn_Rate;
    
	//float max_Turn_Point = 180;
	//float speed_Limit = 1.0;
	
	angleChange_ =  1.0 - (angleChange_ / 180.0);
	
	if (angleChange_ < 0)
    {
        angleChange_ = angleChange_ * -1;
    }
	
	if (angleChange_ >= angleChange_H)
	{
		angleChange_ = angleChange_H;
	}
	else if (angleChange_ <= angleChange_L)
	{
		angleChange_ = angleChange_L;
	}
	
	turn_Rate = (((angleChange_ - angleChange_L) * (speed_H - speed_L)) / 
			(angleChange_H - angleChange_L)) + angleChange_L;
	
	turn_Rate = (1.0 - turn_Rate) * speed;
		
	
	/*if (angleChange_ > max_Turn_Point)
	{
		angleChange_ = max_Turn_Point; //speed_Limit;
	}
	
	turn_Rate = ((speed * angleChange_) / max_Turn_Point); // + 1 - speed_Limit;
	*/
	

	
	return turn_Rate;
}


float CompassDrive::tankLeftMotors(float angleChange, float turn_Rate, float speed)
{	
	float left_Motors;
	
	if((speed > -.1) && (speed < .1))
	{
		left_Motors = 0.0;
	}
	
	else if (angleChange > 0.0)
	{
		left_Motors = turn_Rate; //(turn_Rate * 1.5) -.5;
	}
	else
	{
		left_Motors = speed;
	}
	
	return left_Motors;
}

float CompassDrive::tankRightMotors(float angleChange, float turn_Rate, float speed)
{	
	float right_Motors;
	
	if((speed > -.1) && (speed < .1))
	{
		right_Motors = 0.0;
	}
	
	else if (angleChange < 0.0)
	{
		right_Motors = turn_Rate;//(turn_Rate * 1.5) - .5;
	}
	else
	{
		right_Motors = speed;
	}
	
	return right_Motors;
}
