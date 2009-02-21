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

	Counter countXhi, countXlow, countYhi, countYlow;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),
	{
		GetWatchdog().SetExpiration(0.1);
		
		// specify the source
		countXhi.SetUpSource(3);
		countXlow.SetUpSource(3);
		countYhi.SetUpSource(4);
		countYlow.SetUpSource(4);
		
		// specify the edge
		countXhi.SetUpSourceEdge(true, false);
		countXlow.SetUpSourceEdge(false, true);
		countXhi.SetUpSourceEdge(true, false);
		countYlow.SetUpSourceEdge(false, true);
		
		// setup the counters
		countXhi.SetPulseLengthMode(1.0);
		countXlow.SetPulseLengthMode(1.0);
		countYhi.SetPulseLengthMode(1.0);
		countYlow.SetPulseLengthMode(1.0);
		
		countXhi.Start();
		countXlow.Start();
		countYhi.Start();
		countYlow.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double tm = GetTime();
		double vx = 0.0, vy = 0.0;
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			if (GetTime() - tm > 0.1)
			{
				double axH = countXhi.GetPeriod();
				double axL = countXlow.GetPeriod();
				double ayH = countYhi.GetPeriod();
				double ayL = countYlow.GetPeriod();
				
				// convert to m/s^2
				double ax = (((axH / (axH + axL)) - .5) * 8.0) * 9.81;
				double ay = (((ayH / (ayH + ayL)) - .5) * 8.0) * 9.81;
								
				// or
				// ax = (((axH / 0.01) - .5) * 8.0) * 9.81;
				// ay = (((ayH / 0.01) - .5) * 8.0) * 9.81;
								
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				lcd->PrintfLine(DriverStationLCD::kUser_Line3, "ax: %f m/s^2", ax);
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, "%.6f %.6f", axH, axL);
				lcd->PrintfLine(DriverStationLCD::kUser_Line5, "ay: %f m/s^2", ay);
				lcd->PrintfLine(DriverStationLCD::kUser_Line6, "%.6f %.6f", ayH, ayL);
				
				// euler' integration method.. bad bad bad
				vx += ax / (axH + axL);
				vy += ay / (ayH + ayL);
				//lcd->PrintfLine(DriverStationLCD::kUser_Line5, "vx: %.1f", vx);
				//lcd->PrintfLine(DriverStationLCD::kUser_Line6, "vy: %.1f", vy);
				
				lcd->UpdateLCD();
				tm = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

