#include "WPILib.h"
#include <cmath>
#include "math_definitions.h"
/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	
	Gyro gyro;
	
	// moving right is positive

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		gyro(1)
	{
		GetWatchdog().SetExpiration(100);
		gyro.SetSensitivity(0.007);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		//GetWatchdog().SetEnabled(false);
		//myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		//Wait(2.0); 				//    for 2 seconds
		//myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		printf("Entered OperatorControl\n");
	
		float time = GetTime();
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			// compass drive II (one stick)
			float x = stick.GetX();
			float y = stick.GetY() * -1;
			
			// convert rectangular coordinates to polar
			float speed = hypot(x, y) * M_SQRT1_2;
			float desired_heading = (atan2( y , x ) * (180 / M_PI)) + 90;
			if (desired_heading < 0) desired_heading = 360 - desired_heading;
			
			// get the robot heading (0 to 360 degrees)
			float heading = fmod(gyro.GetAngle(), 360.0);
			if (heading < 0) heading = 360 - heading;
			
			// convert the difference to a value between 1 and -1
			float rate_of_turn = (desired_heading - heading) * (1/360);
			
			// allow some absolute error
			if (fabs(heading - desired_heading) < 2.0)
				rate_of_turn = 0;
			
			// don't bother with this until you verify the params
			//myRobot.Drive(speed, rate_of_turn);
			
			// debug information
			if (GetTime() - time > 0.25)
			{
				printf("X: %.3f, Y: %.3f S: %.3f DH: %.3f, H: %.3f rt: %.3f\r", 
						x, y
						speed, desired_heading,
						heading,
						rate_of_turn);
				time = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

