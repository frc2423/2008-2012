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

#include <limits>
#undef min
#undef max

class RobotDemo : public SimpleRobot
{
	RobotResources resources;
	ExampleMode example;
	Vision vision;
	CompassMode compass;
	PSUSBMode PSUSB;
	Kicker kicker;
	AutonomousVisionMode autonomousVision;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		PSUSB(resources),
		kicker(resources),
		autonomousVision(resources, kicker, 1),
		mode(&autonomousVision)


	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		mode.Add(&compass);
		mode.Add(&PSUSB);
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
		
		bool prev_Previous = false;
		bool prev_Next = false;
		bool prev_button3 = false;
		int stored_mode = 0;
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			kicker.Kick(resources.stick.GetTrigger());
			
			//mode switch code
			
			if(resources.stick.GetRawButton(4) && !prev_Previous)
			{	
				mode.Previous();
				prev_Previous = true;
			}
			else if(!resources.stick.GetRawButton(4) && prev_Previous)
			{
				prev_Previous = false;
			}		
			
			if(resources.stick.GetRawButton(5) && !prev_Next)
			{	
				mode.Next();
				prev_Next = true;
			}
			else if(!resources.stick.GetRawButton(5) && prev_Next)
			{
				prev_Next = false;
			}
			
			
			if(resources.stick.GetRawButton(3) && !prev_button3) 
			{
				vision.enable();
				prev_button3 = true;
			}
			else if(resources.stick.GetRawButton(3) && prev_button3) 
			{	
				vision.run();
			}
			else if(!resources.stick.GetRawButton(3) && prev_button3) 
			{	
				vision.disable();
				prev_button3 = false;
			}
			else
			{
				mode.run();
			}
			
			/*
			if(resources.stick.GetRawButton(3) && !prev_button3)
			{
				stored_mode = mode.GetMode();
				prev_button3 = true;
				mode.Set(1);
			}
			else if(!resources.stick.GetRawButton(3) && prev_button3)
			{
				mode.Set(stored_mode);
				prev_button3 = false;
			}
			else
			{
				mode.run();
			}
			*/
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

