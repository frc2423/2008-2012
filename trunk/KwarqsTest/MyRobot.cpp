#include "WPILib.h"
#include <cmath>

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
	Servo servo;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		gyro(1),
		servo(9)
	{
		GetWatchdog().SetExpiration(100);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		/*
		GetWatchdog().SetEnabled(false);
		myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
		*/
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double time = GetTime();
		GetWatchdog().SetEnabled(true);
      
		gyro.SetSensitivity(0.007);
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			
			double angle = 360 - fabs(fmod(gyro.GetAngle(), 360));
			
			if (angle > 180)
			{
				angle = angle - 180;
			}
			
			servo.SetAngle(angle);
			
			if (GetTime() - time > 1.0)
			{
				printf("Gyro: %f, servo: %f\n", gyro.GetAngle(), angle);
				time = GetTime();
			}
			

			
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

