#include <stdio.h>
#include "CompassDrive.h"
#include <cmath>


CompassDrive::CompassDrive(KwarqsDriveController * driveController) :
	KwarqsMovementControl(driveController),
	direction(1),
	buttonPrev(false)
{
	stick1 = Joystick::GetStickForPort(FIRST_JOYSTICK_PORT);
	gyro1 = new Gyro(1);
	gyro1->SetSensitivity(0.007);
}

/*void CompassDrive::Drive()*/
void CompassDrive::Move()
{
	getVariables();
	control(1, 1);
	float turn_Rate = turnRate(2, .7, .01, .9, .01);

	//printf("speed: %f, turn_Rate: %f\r", speed, turn_Rate);
	
	m_driveController->Move(speed, turn_Rate);
}

void CompassDrive::getVariables()
{	
	robot_Compass = gyro1->GetAngle();
	
	joystick1_x = stick1->GetX();
	//joystick2_x;
	joystick1_y = stick1->GetY() * -1;
	//joystick2_y;
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
	
	if (direction == -1)
	{
		robot_Compass += 180;
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

float CompassDrive::turnRate(int turn_Type, float angleChange_H, float angleChange_L, float speed_H, float speed_L)                              //Finds how fast robot must turn based on angleChange().
{
	//angleChange_H = .7;
	//angleChange_L = .15;
	//speed_H = .9;
	//speed_L = .2;
	
	float joystick_x, joystick_y;
		
	if (get_Turn == 1)
	{
		joystick_x = joystick1_x;
		joystick_y = joystick1_y;
	}
	else if (get_Turn == 2)
	{
		joystick_x = joystick2_x;
		joystick_y = joystick2_y;
	}
	
    float angleChange_ = angleChange(robot_Compass, joystick_x, joystick_y);
	
	float turn_Rate;
	
	//angleChange_H = .7;
	//angleChange_L = .15;
	//speed_H = .9;
	//speed_L = .2;	
    
	angleChange_ = angleChange_ / 180.0;
	
	float angleChange2 = angleChange_;
	
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
			
	if (turn_Type == 1)
	{
		turn_Rate = turn_Rate * speed;
	}
	//else if (turn_Type == 2)
	//{
		//turn_Rate = (1.0 - turn_Rate * 2.0) * speed;
	//	turn_Rate = turn_Rate;
	//}
	
	if (angleChange2 > 0)
	{
		turn_Rate = turn_Rate * -1;
	}
	
	if ((speed > -.1) && (speed < .1))
	{
		turn_Rate = 0.0;
	}
		
	return turn_Rate;
} 


void CompassDrive::control(int turn_Joystick, int speed_Joystick)
{	
	if (turn_Joystick == 1)
	{
		get_Turn = 1;
	}
	else if (turn_Joystick == 2)
	{
		get_Turn = 2;
	}
		
	if (speed_Joystick == turn_Joystick)
	{
		if (speed_Joystick == 1)
		{
			speed = sqrt((joystick1_x * joystick1_x) + (joystick1_y * joystick1_y));
		}
		else if (speed_Joystick == 2)
		{
			speed = sqrt((joystick2_x * joystick2_x) + (joystick2_y * joystick2_y));
		}
	}
	else
	{
		if (speed_Joystick == 1)
		{
			speed = joystick1_y;
		}
		else if (speed_Joystick == 2)
		{
			speed = joystick2_y;
		}
	}
}

float CompassDrive::Direction(bool button)
{	
	if (button == true)
	{
		direction = -1;
	}
	else
	{
		direction = 1;
	}
	
	return direction;
}

/* float CompassDrive::tankLeftMotors(float turn_Rate, float speed)
{	
    float angleChange_ = angleChange(robot_Compass, joystick_x, joystick_y);
	
	float left_Motors;
	
	if((speed > -.1) && (speed < .1))
	{
		left_Motors = 0.0;
	}
	
	if ((direction == 1) && (angleChange_ > 0.0))
	{
		left_Motors = turn_Rate;
	}
	else if ((direction == -1) && (angleChange_ < 0.0))
	{
		left_Motors = turn_Rate;
	}
	else
	{
		left_Motors = speed;
	}
	
	return left_Motors * direction;
}

float CompassDrive::tankRightMotors(float angleChange, float turn_Rate, float speed)
{	
    float angleChange_ = angleChange(robot_Compass, joystick_x, joystick_y);
	
	float right_Motors;
	
	if((speed > -.1) && (speed < .1))
	{
		right_Motors = 0.0;
	}
	
	if ((direction == 1) && (angleChange_ < 0.0))
	{
		right_Motors = turn_Rate;
	}
	else if ((direction == -1) && (angleChange_ > 0.0))
	{
		right_Motors = turn_Rate;
	}
	else
	{
		right_Motors = speed;
	}
	
	return right_Motors * direction;
}

*/


