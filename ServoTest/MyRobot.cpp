#include "WPILib.h"

#include <math.h>
#include "KwarqsWheelServo.h"

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
	
	KwarqsWheelServo servo;
	
public:
	RobotDemo(void):
		myRobot(1, 2),
		stick(1),
		servo(4, 5, 6, 45.0, 250)
	{
		GetWatchdog().SetExpiration(100);	
		
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
		double time = GetTime();
		
		double lastP = 0, lastI = 0;
		
		GetWatchdog().SetEnabled(true);
		
		printf("Entered OperatorControl()\n");
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			double setPoint = DriverStation::GetInstance()->GetAnalogIn(1);
			double p = DriverStation::GetInstance()->GetAnalogIn(2);
			double i = DriverStation::GetInstance()->GetAnalogIn(3);
			
			if (p > 1000.0)
				p = 1000.0;
			p = p / 1000.0;
			
			if (i > 1000.0)
				i = 1000.0;
			i = i / 1000.0;
			
			// only change if there is a significant difference
			if (fabs(p - lastP) > 0.05 || fabs(i - lastI) > 0.05)
			{
				servo.TuneParameters(p, i, 0);
				lastP = p;
				lastI = i;
			}
			
			
			if (setPoint > 1000.0)
				setPoint = 1000.0;
			
			setPoint = ceil(setPoint /(1000.0/ 360.0));
			
			servo.SetAngle(setPoint);
			
			if (GetTime() - time > 0.1)
			{
				printf("Setpoint: %.3f, Current: %f, p: %.3f (%.3f) i: %.3f (%.3f)\r",
						setPoint, servo.GetCurrentAngle(),
						p, lastP, i, lastI);
				
				fflush(stdout);
				time = GetTime();
			}
			
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

