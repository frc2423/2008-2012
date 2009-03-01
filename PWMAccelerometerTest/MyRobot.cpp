#include "WPILib.h"
#include "DriverStationLCD.h"

#include "PositionTracking.h"

#include <cmath>
#include <numeric>



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

	DigitalInput in1, in2;
	Counter countXhi, countXlow, countYhi, countYlow;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),
		in1(3), in2(4)
	{
		GetWatchdog().SetExpiration(0.1);
		
		// specify the source
		countXhi.SetUpSource(in1);
		countXlow.SetUpSource(in1);
		countYhi.SetUpSource(in2);
		countYlow.SetUpSource(in2);
		
		// setup the counters
		countXhi.SetSemiPeriodMode(true);
		countXlow.SetSemiPeriodMode(false);
		countYhi.SetSemiPeriodMode(true);
		countYlow.SetSemiPeriodMode(false);
		
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
		
		AccelerationReset();
		
		
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
				
				axH = axH - fmod(axH, 0.00001);
				ayH = ayH - fmod(ayH, 0.00001);
				
				// convert to m/s^2 -- 50% duty cycle is 0g
				double ax = (((axH / (0.01)) - .5) * 8.0) * 9.81;
				double ay = (((ayH / (0.01)) - .5) * 8.0) * 9.81;
				
				//AccelerationUpdate( ax, ay, .1);
				
				//get the filtered acceleration, velocity and position
				//GetAcceleration( &ax, &ay);
				
				
				// or
				// ax = (((axH / 0.01) - .5) * 8.0) * 9.81;
				// ay = (((ayH / 0.01) - .5) * 8.0) * 9.81;
								
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				lcd->Printf(DriverStationLCD::kUser_Line3, 1, "ax: %f m/s^2            ", ax);
				lcd->Printf(DriverStationLCD::kUser_Line4, 1, "%.6f %.6f               ", axH, axL);
				lcd->Printf(DriverStationLCD::kUser_Line5, 1, "ay: %f m/s^2            ", ay);
				lcd->Printf(DriverStationLCD::kUser_Line6, 1, "%.6f %.6f               ", ayH, ayL);
				
				// euler' integration method.. bad bad bad
				//vx += ax / (0.01);
				//vy += ay / (0.01);
				//lcd->Printf(DriverStationLCD::kUser_Line4, 1, "vx: %.1f", vx);
				//lcd->Printf(DriverStationLCD::kUser_Line6, 1, "vy: %.1f", vy);
				
				lcd->UpdateLCD();
				tm = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

