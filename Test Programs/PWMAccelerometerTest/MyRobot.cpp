#include "WPILib.h"
#include "DriverStationLCD.h"

#include "PositionTracking.h"

#include <cmath>
#include <numeric>

#include "filters.h"

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
	
	LowPassFilter filterX;
	LowPassFilter filterY;
	
	double avgX, avgY;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),
		in1(8), in2(7)
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
	
	void GetAcceleration(double &ax, double &ay)
	{
		double axH = countXhi.GetPeriod();
		double axL = countXlow.GetPeriod();
		double ayH = countYhi.GetPeriod();
		double ayL = countYlow.GetPeriod();
				
		//axH = axH - fmod(axH, 0.00001);
		//ayH = ayH - fmod(ayH, 0.00001);
				
				// convert to m/s^2 -- 50% duty cycle is 0g
		ax = (((axH / (axH + axL)) - .5) * 8.0) * 9.81;
		ay = (((ayH / (ayH + ayL)) - .5) * 8.0) * 9.81;
	}
	
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		
		AverageWindowFilter<double, 20> fx, fy;
		double ax, ay, lastAx = 0, lastAy = 0;
		
		int state = 0;
		
		while (IsAutonomous())
		{
			GetAcceleration(ax, ay);
			fx.AddPoint( ax - avgX );
			fy.AddPoint( ay - avgY );
				
			ax = fx.GetAverage();
			ay = fy.GetAverage();
			
			switch (state)
			{
			case 0:
				
				myRobot.Drive(1.0, 0.0);
				
				
				if (fabs(ay - lastAy) > 1.0)
					++state;
				
				
				break;
				
			case 1:
				
				myRobot.Drive(-.5, 0.0);
				
				Wait(3);
				++state;
				break;
				
			case 2:
				
				myRobot.Drive(0.0, 0.0);
				break;
				
			}
			
			lastAx = ax;
			lastAy = ay;
			
			Wait(0.05);
		}
		
	}
	

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		double tm = GetTime();
		
		//AccelerationReset();
		
		AverageWindowFilter<double, 20> fx, fy;
		
		GetWatchdog().SetEnabled(false);
		
		double ax = 0, lastAx = 0;
		double ay = 0, lastAy = 0;
		
		for (int i = 0; i < 20; i++)
		{
			GetAcceleration(ax, ay);
			fx.AddPoint(ax);
			fy.AddPoint(ay);
			
			Wait(0.05);
		}
		
		avgX = fx.GetAverage();
		avgY = fy.GetAverage();
		
		double minX = 0, maxX = 0;
		double minY = 0, maxY = 0;
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			if (GetTime() - tm > 0.05)
			{
				GetAcceleration(ax, ay);
				
				fx.AddPoint( ax - avgX );
				fy.AddPoint( ay - avgY );
				
				ax = fx.GetAverage();
				ay = fy.GetAverage();
				
				minX = min(fabs(ax - lastAx), minX);
				maxX = max(fabs(ax - lastAx), maxX);
				minY = min(fabs(ay - lastAy), minY);
				maxY = max(fabs(ay - lastAy), maxY);
				
				lastAx = ax;
				lastAy = ay;
				
				//AccelerationUpdate( ax, ay, .1);
				
				//get the filtered acceleration, velocity and position
				//GetAcceleration( &ax, &ay);
				
				
				// or
				// ax = (((axH / 0.01) - .5) * 8.0) * 9.81;
				// ay = (((ayH / 0.01) - .5) * 8.0) * 9.81;
								
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				
				
				lcd->Printf(DriverStationLCD::kUser_Line3, 1, "ax: %f m/s^2            ", ax);
				lcd->Printf(DriverStationLCD::kUser_Line4, 1, "%.6f %.6f               ", minX, maxX);
				lcd->Printf(DriverStationLCD::kUser_Line5, 1, "ay: %f m/s^2            ", ay);
				lcd->Printf(DriverStationLCD::kUser_Line6, 1, "%.6f %.6f               ", minY, maxY);
				
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

