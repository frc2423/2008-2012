#include "WPILib.h"

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

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1)		// as they are declared above.
	{
		GetWatchdog().SetExpiration(0.1);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double time = GetTime() ;
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			
			
			if (GetTime()- time > .001){
				for(int i = 1; i <= 12; i++)
				{
					printf("%d: %d ", i, (int)stick.GetRawButton(i));
				}
				
				printf("\r");
				
			/*	printf ( "x: %.3f, y: %.3f, z: %.3f, twist: %.3f, throttle: %.3f, trigger: %d, top: %d\r", 
						stick.GetX(),
						stick.GetY(),
						stick.GetZ(),
						stick.GetTwist(),
						stick.GetThrottle(),
						(int)stick.GetTrigger(),
						(int)stick.GetTop()
						
					) ; */
				
				time = GetTime () ;
			}
			
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

