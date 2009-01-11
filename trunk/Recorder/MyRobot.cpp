#include "WPILib.h"
#include "Recorder.h"

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

	Jaguar leftMotor;
	Jaguar rightMotor;
	
	Recorder recorder;
	
public:
	RobotDemo(void):
		myRobot(leftMotor, rightMotor),
		stick(1),
		leftMotor(1),
		rightMotor(2)
	{
		GetWatchdog().SetExpiration(100);
		
		recorder.AddMotor(&leftMotor);
		recorder.AddMotor(&rightMotor);
	}


	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		
		if (recorder.StartPlayback())
		{
			while (IsAutonomous() && recorder.Playback());
		}
	}


	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			// use the trigger to start recording.. at the moment,
			// it just gets ignored if you call it more than once
			if (stick.GetTrigger())
				recorder.StartRecording();
	
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			// always call the recording routine
			recorder.Record();
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

