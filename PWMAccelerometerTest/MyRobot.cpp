#include "WPILib.h"
#include "DriverStationLCD.h"



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

	Counter countX, countY;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),
		countX(3), countY(4)
	{
		GetWatchdog().SetExpiration(0.1);
		
		// setup the counter
		countX.SetPulseLengthMode(1.0);
		countY.SetPulseLengthMode(1.0);
		
		countX.Start();
		countY.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double tm = GetTime();
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			if (GetTime() - tm > 0.25)
			{
				double ax = countX.GetPeriod();
				double ay = countY.GetPeriod();
				
				// convert to m/s^2
				ax = (((ax / 0.01) - .5) * 8.0) * 9.81;
				ay = (((ay / 0.01) - .5) * 8.0) * 9.81;
								
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				lcd->PrintfLine(DriverStationLCD::kUser_Line3, "ax: %f m/s^2", ax);
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, "ay: %f m/s^2", ay);
				//lcd->PrintfLine(DriverStationLCD::kUser_Line5, "vx: %.1f", vx);
				//lcd->PrintfLine(DriverStationLCD::kUser_Line6, "vy: %.1f", vy);
				
				tm = GetTime();
			}
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

