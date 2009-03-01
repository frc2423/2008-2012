#include "WPILib.h"

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

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1)		// as they are declared above.
	{
		
	
		
		GetWatchdog().SetExpiration(0.1);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		AnalogTrigger trig1(2);
		trig1.SetLimitsVoltage(1.5, 2.5);
			
		AnalogTrigger trig2(3);
		trig2.SetLimitsVoltage(1.5, 2.5);

		Encoder encoder(
			trig1.CreateOutput(AnalogTriggerOutput::kRisingPulse),
			trig2.CreateOutput(AnalogTriggerOutput::kRisingPulse),
			false, Encoder::k2X); 
		
		double tm = GetTime();
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			
			if (GetTime() - tm > 0.25)
			{
				printf("Encoder: %d\r", encoder.Get());
				tm = GetTime();
			}
			
			Wait(0.005);				// wait for a motor update time
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

