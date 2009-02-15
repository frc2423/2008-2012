#include "WPILib.h"

#include <math.h>
#include "KwarqsWheelServo.h"

#define M_PI 3.14159265358979323846

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
	// slot, pwm, e1, e2, cal
	RobotDemo(void):
		myRobot(1, 2),
		stick(1),
		servo(6, 4, 3, 4, 10, 25.0, 3272, 0, true)
	{
		// LF: 4, 2, 1, 2, 9, 25.0, 3272, 0
		// LR: 4, 4, 3, 4, 10, 25.0, 3272, 0, true
		// 
		// RF: 6, 2, 1, 2, 11, 25.0, 3272, 0
		// RR: 6, 4, 3, 4, 10, 25.0, 3272, 0, true
		
		GetWatchdog().SetExpiration(100);	
		
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}
	
	bool ConvertStickToAngle(double &angle)
	{
		double y = stick.GetY() * -1, x = stick.GetX();
		
		double speed = sqrt(x*x + y*y);
		
		double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 );			
		if (desired_angle < 0) desired_angle += 360;
		
		if (fabs(speed) < 0.01)
			return false;
		
		angle = desired_angle;
		return true;
	}
	
	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double time = GetTime();
		
		double lastP = 0, lastI = 0;
		
		bool disabled = false;
		
		double setPoint = 0;
		
		GetWatchdog().SetEnabled(true);
		
		printf("Entered OperatorControl()\n");
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			//double setPoint = DriverStation::GetInstance()->GetAnalogIn(1);
			
			double p = DriverStation::GetInstance()->GetAnalogIn(2);
			double i = DriverStation::GetInstance()->GetAnalogIn(3);
			
			if (stick.GetTrigger())
			{
				// control motor manually
				if (!disabled)
					servo.Disable();
					
				servo.m_motor.Set(stick.GetY()*-1);
				disabled = true;
			}
			else
			{	
				if (disabled)
				{
					servo.Enable();
					disabled = true;
				}
			
				/*
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
				 */
				
				// only change angle if desired
				if (ConvertStickToAngle(setPoint))
					servo.SetAngle(setPoint);
				
				//if (setPoint > 1000.0)
				//	setPoint = 1000.0;
				
				//setPoint = ceil(setPoint /(1000.0/ 360.0));
			}
			
			if (GetTime() - time > 0.25)
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

