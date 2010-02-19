#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "Vision.h"
#include "CompassMode.h"
#include "PSUSBMode.h"
#include "AutonomousVisionMode.h"
#include "Kicker.h"
#include "EncoderMode.h"
#include "ButtonLatch.h"

#include <limits>
#undef min
#undef max

struct StackEnable {
	StackEnable()
	{
		wpi_stackTraceEnable(true);
	}
};

class RobotDemo : public SimpleRobot
{
	StackEnable unused;
	
	RobotResources resources;
	ExampleMode example;
	Vision vision;
	CompassMode compass;
	Kicker kicker;
	AutonomousVisionMode autonomousVision;
	EncoderMode encoderMode;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		kicker(resources),
		autonomousVision(resources, kicker, 1),
		encoderMode(resources),
		mode(&autonomousVision)


	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		//mode.Add(&compass);
		//mode.Add(&encoderMode);
		
		kicker.Start();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		mode.Autonomous();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(true);
		
		ButtonLatch eitherButton;
		ButtonLatch leftVision;
		ButtonLatch rightVision;
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if( resources.stick.GetTrigger()) kicker.Kick();
			
			
			leftVision.Set( resources.stick.GetRawButton(4));
			rightVision.Set( resources.stick.GetRawButton(5));
			eitherButton.Set(resources.stick.GetRawButton(3));
			
			if (leftVision.TurnedOn())
			{
				vision.PreferLeft();
			}
			else if (rightVision.TurnedOn())
			{
				vision.PreferRight();
			}
			else if (eitherButton.TurnedOn())
			{
				vision.PreferEither();
			}
			else if (
				leftVision.TurnedOff() ||
				rightVision.TurnedOff() ||
				eitherButton.TurnedOff()
			){
				vision.DisableMotorControl();
			}
			else if (
					leftVision.Off() &&
					rightVision.Off() &&
					eitherButton.Off()
			){
				// do normal things here
				mode.run();
			}
			
			// DO NOT TAKE THIS OUT
			Wait(0.005);
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

