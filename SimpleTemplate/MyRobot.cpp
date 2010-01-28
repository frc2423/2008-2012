#include <WPILib.h>
#include <WebDMA/WebDMA.h>

#include "CoordinateSystem.h"

#include <cmath>

#define WHEEL_RADIUS 0.1524
#define WHEEL_BASE 0.5
#define SLOT_1 4
#define SLOT_2 6
#define SLOT SLOT_1

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
	Encoder leftEncoder; // detects the speed that the wheels are turning at
	Encoder rightEncoder;
	WebDMA webdma;

	CoordinateSystem coordinateSystem;
	
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		leftEncoder(SLOT, 3, SLOT, 4),
		rightEncoder(SLOT, 1, SLOT, 2),
		coordinateSystem(&leftEncoder, &rightEncoder, &webdma)
	{
		coordinateSystem.SetWheelInformation(WHEEL_RADIUS, WHEEL_BASE);

		
		GetWatchdog().SetExpiration(0.1);
		webdma.Enable("80", "/www");
		
		leftEncoder.Start();
		rightEncoder.Start();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		//myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		//Wait(2.0); 				//    for 2 seconds
		//myRobot.Drive(0.0, 0.0); 	// stop robot
		
		
		while (IsAutonomous())
		{
			if( leftEncoder.GetDistance() >= 500.0)
			{
				myRobot.Drive(0.2, 0.0);
			}
			else
			{
				myRobot.Drive(0.0, 0.0);
			}
		}
		
		/*bool canMove = true;
		
		if(canMove && stick.GetTrigger())
		{
			canMove = false;
		}
		else if(!canMove && encoder.GetDistance() <= 50.0)
		{
			myRobot.Drive(0.2, 0.0);
		}
		else if(!canMove && encoder.GetDistance() > 50.0)
		{
			myRobot.Drive(0.0, 0.0);
			canMove = true;
			encoder.Reset();
		}*/
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if (abs(stick.GetX()) <= .1)
			{
				myRobot.Drive( stick.GetMagnitude()* stick.GetY() / abs(stick.GetY()), 0.0 );
			}
			else
			{
				myRobot.Drive( stick.GetMagnitude()* stick.GetY() / abs(stick.GetY()), pow( (stick.GetThrottle()+ 1) / 2, abs(stick.GetY())) * stick.GetX() );
			}

			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

