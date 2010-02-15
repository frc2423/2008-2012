#include "WPILib.h"
#include <WebDMA/WebDMA.h>
#include "RobotResources.h"
#include "ExampleMode.h"
#include "RobotMode.h"
#include "VisionMode.h"
#include "CompassMode.h"
#include "AutonomousMode.h"
#include "PSUSBMode.h"
#include "AutonomousVisionMode.h"

#include <limits>
#undef min
#undef max

class RobotDemo : public SimpleRobot
{
	RobotResources resources;
	ExampleMode example;
	VisionMode vision;
	CompassMode compass;
	AutonomousMode autonomous;
	PSUSBMode PSUSB;
	AutonomousVisionMode autonomousVision;
	Mode mode;

public:
	RobotDemo(void):
		example(resources),
		vision(resources),
		compass(resources),
		autonomous(resources),
		PSUSB(resources),
		autonomousVision(resources, 1),
		mode(&autonomousVision)

	{
		GetWatchdog().SetExpiration(0.1);
		
		mode.Add(&example);
		mode.Add(&vision);
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
		
		bool prev_top = false;
		bool prev_trigger = false;
		int stored_mode = 0;
		
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			//mode switch code
			
			if(resources.stick.GetTop() && !prev_top)
			{	
				mode.Next();
				prev_top = true;
			}
			else if(!resources.stick.GetTop() && prev_top)
			{
				prev_top = false;
			}
			
			
			if(resources.stick.GetTrigger() && !prev_trigger)
			{
				stored_mode = mode.GetMode();
				prev_trigger = true;
				mode.Set(1);
			}
			else if(!resources.stick.GetTrigger() && prev_trigger)
			{
				mode.Set(stored_mode);
				prev_trigger = false;
			}
			else
			{
				mode.run();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

