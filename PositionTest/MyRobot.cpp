#include "WPILib.h"
#include "PositionInformation.h"

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
	
	
	PositionInformation * positionInfo;

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1)		// as they are declared above.
	{
		GetWatchdog().SetExpiration(0.1);
		
		// initialize this
		positionInfo = PositionInformation::GetInstance();
		
		double x, y;
		positionInfo->GetAccelerationBias(x, y);
		
		printf("Acceleration bias: %f, %f\n", x, y);
		
		//positionInfo->SetAccelerationBias( .9511, .6285);
	}


	void Autonomous(void)
	{
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double time = GetTime();
		printf("Entering OperatorControl()\n");
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			if (GetTime() - time > 0.1)
			{
				double px = 0.0, py = 0.0;
				double vx = 0.0, vy = 0.0;
				
				positionInfo->GetPosition(px, py);
				positionInfo->GetVelocity(vx, vy);
				
				printf("Position: x: %f, y: %f; Velocity: x: %f, y: %f\r", 
						px, py, vx, vy);
				
				time = GetTime();
			}
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

