#include <WPILib.h>
#include <WebDMA/WebDMA.h>

#include "CoordinateSystem.h"

#include <cmath>

#define WHEEL_RADIUS (0.1524 * (22.0 / 15.0))
#define WHEEL_BASE 0.625
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
		leftEncoder(SLOT, 1, SLOT, 2),
		rightEncoder(SLOT, 3, SLOT, 4),
		coordinateSystem(&leftEncoder, &rightEncoder, &webdma)
	{
		coordinateSystem.SetWheelInformation(WHEEL_RADIUS, WHEEL_BASE);

		
		GetWatchdog().SetExpiration(0.1);
		webdma.Enable();
		
		leftEncoder.Start();
		rightEncoder.Start();
		
		coordinateSystem.Start();
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
		
		
		int autonomousState = 1;
		
		while (IsAutonomous())
		{
/*			if( leftEncoder.GetDistance() >= 500.0)
			{
				myRobot.Drive(0.2, 0.0);
			}
			else
			{
				myRobot.Drive(0.0, 0.0);
			}         */
		
			double x, y, angle;
			coordinateSystem.getData(x, y, angle);
			
			switch(autonomousState)
			{
				case 1:
					myRobot.Drive(-0.5, 0.0);
					if( y >= 1.0)
						autonomousState = 2;
					break;
				case 2:
					myRobot.Drive(-0.2, -0.3);
					if( angle >= 90.0)
						autonomousState = 3;
					break;
				case 3:
					myRobot.Drive(-0.5, 0.0);
					if( x <= -1.0)
						autonomousState = 4;
					break;
				case 4:
					myRobot.Drive(-0.2, -0.3);
					if( angle >= 180.0)
						autonomousState = 5;
					break;
				case 5:
					myRobot.Drive(-0.5, 0.0);
					if( y >= 0.0)
						autonomousState = 6;
					break;
				case 6:
					myRobot.Drive(-0.2, -0.3);
					if( angle >= 270.0)
						autonomousState = 7;
					break;
				case 7:
					myRobot.Drive(-0.5, 0.0);
					if( x >= 0.0)
						autonomousState = 8;
					break;
				case 8:
					myRobot.Drive(-0.2, -0.3);
					if( angle >= 0.0 )
						autonomousState = 9;
					break;
				case 9:
					myRobot.Drive(0.0, 0.0);
					break;
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
			
		/*	if (abs(stick.GetX()) <= .1)
			{
				myRobot.Drive( stick.GetMagnitude()* stick.GetY() / abs(stick.GetY()), 0.0 );
			}
			else
			{
				myRobot.Drive( stick.GetMagnitude()* stick.GetY() / abs(stick.GetY()), pow( (stick.GetThrottle()+ 1) / 2, abs(stick.GetY())) * stick.GetX() );
			}
		*/

			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

